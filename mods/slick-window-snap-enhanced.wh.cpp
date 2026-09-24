// ==WindhawkMod==
// @id              slick-window-snap-enhanced
// @name            Slick Window Snap Enhanced
// @description     Enhanced variant of Slick Window Arrangement with resize snapping and smarter corner alignment
// @version         1.1.0
// @author          Kirchlive
// @github          https://github.com/Kirchlive
// @include         *
// @compilerOptions -lcomctl32 -ldwmapi
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Based on Slick Window Arrangement by m417z.
// Enhancements by Kirchlive.

// ==WindhawkModReadme==
/*
# Slick Window Snap Enhanced

Derived from [Slick Window Arrangement](https://windhawk.net/mods/slick-window-arrangement) by m417z and extends it with:

- Snapping while resizing windows, including matching neighbour corners.
- Smarter snapping priority that prefers aligned edges even when windows are slightly offset on the orthogonal axis.
- Everything from the original mod: adjustable snap distance, optional sliding animation, modifier keys to disable snapping, and more.

![Demo](https://i.imgur.com/eTt3LOE.gif)

Default configuration tweaks:
- Snap distance: 15 px (was 20).
- Alt temporarily disables snapping (Shift disabled by default).
- Sliding slowdown: 3% for a snappier animation.

## Credits
- Original concept and implementation: m417z ([Slick Window Arrangement](https://windhawk.net/mods/slick-window-arrangement)).
- Enhancements: Kirchlive.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- SnapWindowsWhenDragging: true
  $name: Enable snapping while dragging
  $description: Keep windows magnetised to monitor borders and neighbours when dragging.
- SnapWindowsDistance: 15
  $name: Snap distance (pixels)
  $description: Maximum distance from an edge where snapping is triggered.
- KeysToDisableSnappingCtrl: false
  $name: Use Ctrl to disable snapping temporarily
  $description: Hold Ctrl while dragging/resizing to bypass snapping.
- KeysToDisableSnappingAlt: true
  $name: Use Alt to disable snapping temporarily
  $description: Hold Alt while dragging/resizing to bypass snapping.
- KeysToDisableSnappingShift: false
  $name: Use Shift to disable snapping temporarily
  $description: Hold Shift while dragging/resizing to bypass snapping.
- SlidingAnimation: true
  $name: Enable sliding animation
  $description: Let windows glide to a stop when the mouse button is released.
- SnapWindowsWhenSliding: true
  $name: Snap during sliding animation
  $description: Apply snapping constraints while the sliding animation is active.
- SlidingAnimationSlowdown: 3
  $name: Sliding slowdown (%)
  $description: 1-99. Higher values slow the animation more aggressively.
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <shellscalingapi.h>
#include <shobjidl.h>
#include <tlhelp32.h>
#include <windowsx.h>

#include <atomic>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct {
    bool snapWindowsWhenDragging;
    int snapWindowsDistance;
    bool keysToDisableSnappingCtrl;
    bool keysToDisableSnappingAlt;
    bool keysToDisableSnappingShift;
    bool slidingAnimation;
    bool snapWindowsWhenSliding;
    int slidingAnimationSlowdown;
} g_settings;

#ifndef SWP_STATECHANGED
#define SWP_STATECHANGED 0x8000
#endif

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

// https://devblogs.microsoft.com/oldnewthing/20200302-00/?p=103507
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

        MONITORINFO monitorInfo = { sizeof(monitorInfo) };
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

        InitialWndEnumProcParam enumParam;
        enumParam.hTargetWnd = hTargetWnd;
        EnumWindows(InitialWndEnumProc, (LPARAM)&enumParam);

        for (auto it = enumParam.windowRects.rbegin(); it != enumParam.windowRects.rend(); ++it) {
            const auto& rc = *it;

            RemoveOverlappedTargets(magnetTargetsLeft, rc.left, rc.right, rc.top, rc.bottom);
            RemoveOverlappedTargets(magnetTargetsTop, rc.top, rc.bottom, rc.left, rc.right);
            RemoveOverlappedTargets(magnetTargetsRight, rc.left, rc.right, rc.top, rc.bottom);
            RemoveOverlappedTargets(magnetTargetsBottom, rc.top, rc.bottom, rc.left, rc.right);

            magnetTargetsLeft.emplace(rc.left, rc.top, rc.bottom);
            magnetTargetsTop.emplace(rc.top, rc.left, rc.right);
            magnetTargetsRight.emplace(rc.right, rc.top, rc.bottom);
            magnetTargetsBottom.emplace(rc.bottom, rc.left, rc.right);
        }

        EnumDisplayMonitors(nullptr, nullptr, InitialMonitorEnumProc, (LPARAM)this);
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

        int newX = *x;
        int newY = *y;

        long targetLeft = FindClosestTarget(magnetTargetsLeft,
            sourceRect.right, sourceRect.top, sourceRect.bottom, magnetPixels);
        long targetRight = FindClosestTarget(magnetTargetsRight,
            sourceRect.left, sourceRect.top, sourceRect.bottom, magnetPixels);

        if (targetLeft != LONG_MAX && targetRight != LONG_MAX &&
            std::abs(targetLeft - sourceRect.right) < std::abs(targetRight - sourceRect.left)) {
            newX = targetLeft - *cx + windowBorderRect.right;
        }
        else if (targetRight != LONG_MAX) {
            newX = targetRight - windowBorderRect.left;
        }
        else if (targetLeft != LONG_MAX) {
            newX = targetLeft - *cx + windowBorderRect.right;
        }

        long targetTop = FindClosestTarget(magnetTargetsTop,
            sourceRect.bottom, sourceRect.left, sourceRect.right, magnetPixels);
        long targetBottom = FindClosestTarget(magnetTargetsBottom,
            sourceRect.top, sourceRect.left, sourceRect.right, magnetPixels);

        if (targetTop != LONG_MAX && targetBottom != LONG_MAX &&
            std::abs(targetTop - sourceRect.bottom) < std::abs(targetBottom - sourceRect.top)) {
            newY = targetTop - *cy + windowBorderRect.bottom;
        }
        else if (targetBottom != LONG_MAX) {
            newY = targetBottom - windowBorderRect.top;
        }
        else if (targetTop != LONG_MAX) {
            newY = targetTop - *cy + windowBorderRect.bottom;
        }

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

    void MagnetResize(HWND hSourceWnd, RECT* windowRect, WPARAM sizingEdge) {
        if (IsSnappingTemporarilyDisabled()) {
            return;
        }

        if (!windowRect) {
            return;
        }

        CalculateMetrics(hSourceWnd);

        if (magnetPixels <= 0) {
            return;
        }

        RECT frameRect = {
            windowRect->left + windowBorderRect.left,
            windowRect->top + windowBorderRect.top,
            windowRect->right - windowBorderRect.right,
            windowRect->bottom - windowBorderRect.bottom
        };

        bool adjustLeft = sizingEdge == WMSZ_LEFT || sizingEdge == WMSZ_TOPLEFT || sizingEdge == WMSZ_BOTTOMLEFT;
        bool adjustRight = sizingEdge == WMSZ_RIGHT || sizingEdge == WMSZ_TOPRIGHT || sizingEdge == WMSZ_BOTTOMRIGHT;
        bool adjustTop = sizingEdge == WMSZ_TOP || sizingEdge == WMSZ_TOPLEFT || sizingEdge == WMSZ_TOPRIGHT;
        bool adjustBottom = sizingEdge == WMSZ_BOTTOM || sizingEdge == WMSZ_BOTTOMLEFT || sizingEdge == WMSZ_BOTTOMRIGHT;

        if (adjustLeft) {
            int threshold = magnetPixels;
            int bestDelta = threshold + 1;
            long snapped = frameRect.left;

            auto consider = [&](long candidate) {
                if (candidate == LONG_MAX) {
                    return;
                }
                if (candidate >= frameRect.right) {
                    return;
                }
                int delta = (int)std::abs(frameRect.left - candidate);
                if (delta <= threshold && delta < bestDelta) {
                    bestDelta = delta;
                    snapped = candidate;
                }
            };

            consider(FindClosestTarget(magnetTargetsRight,
                frameRect.left, frameRect.top, frameRect.bottom, threshold));
            if (bestDelta <= threshold) {
                threshold = bestDelta;
            }
            consider(FindClosestTarget(magnetTargetsLeft,
                frameRect.left, frameRect.top, frameRect.bottom, threshold));

            if (snapped != frameRect.left && frameRect.right - snapped > 0) {
                frameRect.left = snapped;
            }
        }

        if (adjustRight) {
            int threshold = magnetPixels;
            int bestDelta = threshold + 1;
            long snapped = frameRect.right;

            auto consider = [&](long candidate) {
                if (candidate == LONG_MAX) {
                    return;
                }
                if (candidate <= frameRect.left) {
                    return;
                }
                int delta = (int)std::abs(frameRect.right - candidate);
                if (delta <= threshold && delta < bestDelta) {
                    bestDelta = delta;
                    snapped = candidate;
                }
            };

            consider(FindClosestTarget(magnetTargetsLeft,
                frameRect.right, frameRect.top, frameRect.bottom, threshold));
            if (bestDelta <= threshold) {
                threshold = bestDelta;
            }
            consider(FindClosestTarget(magnetTargetsRight,
                frameRect.right, frameRect.top, frameRect.bottom, threshold));

            if (snapped != frameRect.right && snapped - frameRect.left > 0) {
                frameRect.right = snapped;
            }
        }

        if (adjustTop) {
            int threshold = magnetPixels;
            int bestDelta = threshold + 1;
            long snapped = frameRect.top;

            auto consider = [&](long candidate) {
                if (candidate == LONG_MAX) {
                    return;
                }
                if (candidate >= frameRect.bottom) {
                    return;
                }
                int delta = (int)std::abs(frameRect.top - candidate);
                if (delta <= threshold && delta < bestDelta) {
                    bestDelta = delta;
                    snapped = candidate;
                }
            };

            consider(FindClosestTarget(magnetTargetsBottom,
                frameRect.top, frameRect.left, frameRect.right, threshold));
            if (bestDelta <= threshold) {
                threshold = bestDelta;
            }
            consider(FindClosestTarget(magnetTargetsTop,
                frameRect.top, frameRect.left, frameRect.right, threshold));

            if (snapped != frameRect.top && frameRect.bottom - snapped > 0) {
                frameRect.top = snapped;
            }
        }

        if (adjustBottom) {
            int threshold = magnetPixels;
            int bestDelta = threshold + 1;
            long snapped = frameRect.bottom;

            auto consider = [&](long candidate) {
                if (candidate == LONG_MAX) {
                    return;
                }
                if (candidate <= frameRect.top) {
                    return;
                }
                int delta = (int)std::abs(frameRect.bottom - candidate);
                if (delta <= threshold && delta < bestDelta) {
                    bestDelta = delta;
                    snapped = candidate;
                }
            };

            consider(FindClosestTarget(magnetTargetsTop,
                frameRect.bottom, frameRect.left, frameRect.right, threshold));
            if (bestDelta <= threshold) {
                threshold = bestDelta;
            }
            consider(FindClosestTarget(magnetTargetsBottom,
                frameRect.bottom, frameRect.left, frameRect.right, threshold));

            if (snapped != frameRect.bottom && snapped - frameRect.top > 0) {
                frameRect.bottom = snapped;
            }
        }

        windowRect->left = frameRect.left - windowBorderRect.left;
        windowRect->top = frameRect.top - windowBorderRect.top;
        windowRect->right = frameRect.right + windowBorderRect.right;
        windowRect->bottom = frameRect.bottom + windowBorderRect.bottom;
    }

private:
    UINT windowDpi = -1;
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

        magnetPixels = g_settings.snapWindowsDistance;
        if (windowDpi) {
            magnetPixels = MulDiv(magnetPixels, windowDpi, 96);
        }
    }

    struct InitialWndEnumProcParam {
        HWND hTargetWnd = nullptr;
        std::vector<RECT> windowRects;
    };

    static BOOL CALLBACK InitialWndEnumProc(HWND hWnd, LPARAM lParam) {
        auto& param = *(InitialWndEnumProcParam*)lParam;

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
        auto& windowMagnet = *(WindowMagnet*)lParam;

        MONITORINFO monitorInfo = { sizeof(monitorInfo) };
        GetMonitorInfo(monitor, &monitorInfo);

        auto& rc = monitorInfo.rcWork;

        windowMagnet.magnetTargetsLeft.emplace(rc.right, rc.top, rc.bottom);
        windowMagnet.magnetTargetsTop.emplace(rc.bottom, rc.left, rc.right);
        windowMagnet.magnetTargetsRight.emplace(rc.left, rc.top, rc.bottom);
        windowMagnet.magnetTargetsBottom.emplace(rc.top, rc.left, rc.right);

        return TRUE;
    }

    static void RemoveOverlappedTargets(std::set<std::tuple<long, long, long>>& magnetTargets,
        long start, long end, long otherAxisStart, long otherAxisEnd) {
        for (auto it = magnetTargets.lower_bound({ start, otherAxisStart, otherAxisStart });
            it != magnetTargets.end();) {
            auto a = std::get<0>(*it);
            auto b = std::get<1>(*it);
            auto c = std::get<2>(*it);

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
        int bestAxisDistance = magnetPixels + 1;
        long bestPrimaryDistance = magnetPixels + 1;

        long iterStart = source - magnetPixels;
        long iterEnd = source + magnetPixels;

        for (auto it = magnetTargets.lower_bound({ iterStart, otherAxisStart, otherAxisStart });
            it != magnetTargets.end();
            ++it) {
            auto a = std::get<0>(*it);
            auto b = std::get<1>(*it);
            auto c = std::get<2>(*it);

            if (a > iterEnd || (a == iterEnd && b > otherAxisEnd)) {
                break;
            }

            int axisDistance = 0;
            if (otherAxisEnd <= b) {
                axisDistance = (int)(b - otherAxisEnd);
            }
            else if (otherAxisStart >= c) {
                axisDistance = (int)(otherAxisStart - c);
            }

            if (axisDistance > magnetPixels) {
                continue;
            }

            long primaryDistance = std::abs(source - a);

            if (target == LONG_MAX
             || axisDistance < bestAxisDistance
             || (axisDistance == bestAxisDistance && primaryDistance < bestPrimaryDistance)) {
                target = a;
                bestAxisDistance = axisDistance;
                bestPrimaryDistance = primaryDistance;
            }
        }

        return target;
    }

    struct IsRectInWorkAreaMonitorEnumProcParam {
        const RECT* rc;
        bool inWorkArea;
    };

    static BOOL CALLBACK IsRectInWorkAreaMonitorEnumProc(HMONITOR monitor, HDC, LPRECT, LPARAM lParam) {
        auto& param = *(IsRectInWorkAreaMonitorEnumProcParam*)lParam;

        MONITORINFO monitorInfo = { sizeof(monitorInfo) };
        GetMonitorInfo(monitor, &monitorInfo);

        auto& rcA = *param.rc;
        auto& rcB = monitorInfo.rcWork;

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
        MovingState state = GetCurrentMovingState(hTargetWnd, *x, *y);

        // If window state changes, e.g. the window is snapped, don't adjust its
        // position, which could interfere with the snapping and doesn't make
        // sense in general.
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

        windowMagnet.MagnetMove(hTargetWnd, x, y, cx, cy);
    }

    void ForgetLastPos() {
        lastState.reset();
    }

    WindowMagnet& GetWindowMagnet() {
        return windowMagnet;
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

    static MovingState GetCurrentMovingState(HWND hTargetWnd, int x, int y) {
        return MovingState{
            .isMinimized = !!IsMaximized(hTargetWnd),
            .isMaximized = !!IsMinimized(hTargetWnd),
            .isArranged = pIsWindowArranged && !!pIsWindowArranged(hTargetWnd),
            .messagePos = GetMessagePos(),
            .x = x,
            .y = y,
        };
    }

    std::optional<MovingState> lastState;
    WindowMagnet windowMagnet;
};

class WindowMove {
public:
    WindowMove() {}

    void Reset() {
        lastState.reset();
        for (auto& snapshotItem : snapshot) {
            snapshotItem = nullptr;
        }
    }

    void UpdateWithNewPos(HWND hTargetWnd, int x, int y) {
        DWORD tickCount = GetTickCount();

        if (snapshot[0] && tickCount - snapshot[0]->tickCount >= 100) {
            // Waited for too long, reset.
            Reset();
        }

        WindowState state = GetWindowState(hTargetWnd);
        if (lastState && (state.isMinimized != lastState->isMinimized ||
                          state.isMaximized != lastState->isMaximized ||
                          state.isArranged != lastState->isArranged)) {
            // Window state changed, e.g. it was snapped, reset.
            Reset();
        }

        lastState = state;

        if (!snapshot[0]) {
            snapshot[0] = &snapshotStorage[0];
        }
        else if (!snapshot[1]) {
            snapshot[1] = &snapshotStorage[0];
            snapshot[0] = &snapshotStorage[1];
        }

        if (snapshot[1] && tickCount - snapshot[1]->tickCount >= 100) {
            if (!snapshot[2]) {
                snapshot[2] = &snapshotStorage[2];
            }

            auto* temp = snapshot[2];
            snapshot[2] = snapshot[1];
            snapshot[1] = snapshot[0];
            snapshot[0] = temp;
        }

        snapshot[0]->tickCount = tickCount;
        snapshot[0]->x = x;
        snapshot[0]->y = y;
    }

    bool CompleteMove(int* x, int* y, double* velocityX, double* velocityY) {
        if (!snapshot[0] || !snapshot[1]) {
            return false;
        }

        if (GetTickCount() - snapshot[0]->tickCount >= 100) {
            return false;
        }

        auto* winMoveLast = snapshot[0];
        auto* winMovePrev = snapshot[2] ? snapshot[2] : snapshot[1];

        int tickCountDiff = winMoveLast->tickCount - winMovePrev->tickCount;
        if (tickCountDiff <= 0) {
            return false;
        }

        *x = winMoveLast->x;
        *y = winMoveLast->y;

        // Velocity in client coordinates per second.
        *velocityX = 1000.0 * (winMoveLast->x - winMovePrev->x) / tickCountDiff;
        *velocityY = 1000.0 * (winMoveLast->y - winMovePrev->y) / tickCountDiff;

        return true;
    }

private:
    struct WindowState {
        bool isMinimized;
        bool isMaximized;
        bool isArranged;
    };

    struct MoveSnapshot {
        DWORD tickCount;
        int x, y;
    };

    static WindowState GetWindowState(HWND hTargetWnd) {
        return WindowState{
            .isMinimized = !!IsMaximized(hTargetWnd),
            .isMaximized = !!IsMinimized(hTargetWnd),
            .isArranged = pIsWindowArranged && !!pIsWindowArranged(hTargetWnd),
        };
    }

    std::optional<WindowState> lastState;
    MoveSnapshot snapshotStorage[3];
    MoveSnapshot* snapshot[3]{};
};

struct WindowSlideTimer {
public:
    WindowSlideTimer(TIMERPROC proc, int cursorX, int cursorY, int x, int y, double velocityX, double velocityY, std::optional<WindowMagnet> windowMagnet) :
        cursorPoint{ cursorX, cursorY }, x((double)x), y((double)y), velocityX(velocityX), velocityY(velocityY), windowMagnet(std::move(windowMagnet)) {
        timerId = SetTimer(nullptr, 0, 10, proc);

        HMONITOR monitor = MonitorFromPoint(cursorPoint, MONITOR_DEFAULTTONEAREST);

        MONITORINFO monitorInfo = { sizeof(monitorInfo) };
        GetMonitorInfo(monitor, &monitorInfo);
        CopyRect(&workArea, &monitorInfo.rcWork);
    }

    ~WindowSlideTimer() {
        KillTimer(nullptr, timerId);
    }

    WindowSlideTimer(const WindowSlideTimer&) = delete;
    WindowSlideTimer(WindowSlideTimer&&) = delete;
    WindowSlideTimer& operator=(const WindowSlideTimer&) = delete;
    WindowSlideTimer& operator=(WindowSlideTimer&&) = delete;

    UINT_PTR GetTimerId() const {
        return timerId;
    }

    bool SlideNextFrame(HWND hWnd) {
        RECT rect;
        GetWindowRect(hWnd, &rect);
        if (frame != 0) {
            // If the window's position or size changed, stop timer.
            if (
                lastX != rect.left ||
                lastY != rect.top ||
                lastCx != rect.right - rect.left ||
                lastCy != rect.bottom - rect.top
            ) {
                return false;
            }
        }

        int prevX = (int)x;
        int prevY = (int)y;

        // The default timer resolution on Windows is 15.6 ms.
        x += velocityX / (1000.0 / 15.6);
        y += velocityY / (1000.0 / 15.6);

        int currentX = (int)x;
        int currentY = (int)y;

        if (currentX == prevX && currentY == prevY) {
            return false;
        }

        POINT anchor{ currentX + cursorPoint.x, currentY + cursorPoint.y };
        if (!PtInRect(&workArea, anchor)) {
            bool foundNewMonitor = false;

            HMONITOR monitor = MonitorFromPoint(anchor, MONITOR_DEFAULTTONULL);
            if (monitor) {
                MONITORINFO monitorInfo = { sizeof(monitorInfo) };
                GetMonitorInfo(monitor, &monitorInfo);
                if (PtInRect(&monitorInfo.rcWork, anchor)) {
                    foundNewMonitor = true;
                    CopyRect(&workArea, &monitorInfo.rcWork);
                }
            }

            if (!foundNewMonitor) {
                if (anchor.x < workArea.left) {
                    x = workArea.left - cursorPoint.x;
                    velocityX = -velocityX * .05;
                }
                else if (anchor.x > workArea.right) {
                    x = workArea.right - cursorPoint.x;
                    velocityX = -velocityX * .05;
                }

                if (anchor.y < workArea.top) {
                    y = workArea.top - cursorPoint.y;
                    velocityY = -velocityY * .05;
                }
                else if (anchor.y > workArea.bottom) {
                    y = workArea.bottom - cursorPoint.y;
                    velocityY = -velocityY * .05;
                }

                currentX = (int)x;
                currentY = (int)y;
            }
        }

        int slidingAnimationSlowdown = g_settings.slidingAnimationSlowdown;
        if (slidingAnimationSlowdown < 1) {
            slidingAnimationSlowdown = 1;
        }
        else if (slidingAnimationSlowdown > 99) {
            slidingAnimationSlowdown = 99;
        }

        double slowdownMultiplier = (100 - g_settings.slidingAnimationSlowdown) / 100.0;

        velocityX *= slowdownMultiplier;
        velocityY *= slowdownMultiplier;

        if (windowMagnet) {
            int magnetX = currentX;
            int magnetY = currentY;
            int cx = rect.right - rect.left;
            int cy = rect.bottom - rect.top;
            windowMagnet->MagnetMove(hWnd, &magnetX, &magnetY, &cx, &cy);

            if (magnetX != currentX) {
                x = magnetX;
                currentX = magnetX;
                velocityX = 0.0;
            }

            if (magnetY != currentY) {
                y = magnetY;
                currentY = magnetY;
                velocityY = 0.0;
            }
        }

        SetWindowPos(hWnd, nullptr, currentX, currentY, 0, 0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

        lastX = currentX;
        lastY = currentY;
        lastCx = rect.right - rect.left;
        lastCy = rect.bottom - rect.top;

        frame++;
        return frame < 50;
    }

private:
    UINT_PTR timerId;
    int frame = 0;
    POINT cursorPoint;
    RECT workArea;
    double x, y;
    double velocityX, velocityY;
    int lastX, lastY, lastCx, lastCy;
    std::optional<WindowMagnet> windowMagnet;
};

std::atomic<bool> g_uninitializing;
std::atomic<int> g_hookRefCount;
thread_local std::unordered_map<HWND, WindowMoving> g_winMoving;
thread_local std::unordered_map<HWND, WindowMove> g_winMove;
thread_local std::unordered_map<HWND, WindowSlideTimer> g_winSlideTimers;

UINT g_unsubclassRegisteredMessage = RegisterWindowMessage(
    L"Windhawk_Unsubclass_slick-window-snap-enhanced");
std::mutex g_subclassedWindowsMutex;
std::unordered_set<HWND> g_subclassedWindows;

thread_local HHOOK g_callWndProcHook;
std::mutex g_allCallWndProcHooksMutex;
std::unordered_set<HHOOK> g_allCallWndProcHooks;

LRESULT CALLBACK SubclassWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

auto hookRefCountScope() {
    g_hookRefCount++;
    return std::unique_ptr<decltype(g_hookRefCount), void(*)(decltype(g_hookRefCount)*)>{
        &g_hookRefCount, [](auto hookRefCount) {
            (*hookRefCount)--;
        }};
}

void UnsubclassWindow(HWND hWnd)
{
    RemoveWindowSubclass(hWnd, SubclassWndProc, 0);

    std::lock_guard<std::mutex> guard(g_subclassedWindowsMutex);

    auto it = g_subclassedWindows.find(hWnd);
    if (it != g_subclassedWindows.end()) {
        g_subclassedWindows.erase(it);
    }
}

bool KillWindowSlideTimer(HWND hWnd)
{
    auto it = g_winSlideTimers.find(hWnd);
    if (it == g_winSlideTimers.end()) {
        return false;
    }

    g_winSlideTimers.erase(it);
    return true;
}

void CALLBACK WindowSlideTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idTimer, DWORD dwTime)
{
    auto it = g_winSlideTimers.begin();
    for (; it != g_winSlideTimers.end(); ++it) {
        auto& timer = it->second;
        if (timer.GetTimerId() == idTimer) {
            break;
        }
    }

    if (it == g_winSlideTimers.end()) {
        return;
    }

    HWND hTargetWnd = it->first;
    auto& timer = it->second;

    if (!timer.SlideNextFrame(hTargetWnd)) {
        g_winSlideTimers.erase(it);
        UnsubclassWindow(hTargetWnd);
    }
}

void OnEnterSizeMove(HWND hWnd)
{
    KillWindowSlideTimer(hWnd);

    if (g_settings.snapWindowsWhenDragging) {
        g_winMoving.try_emplace(hWnd, hWnd);
    }

    if (g_settings.slidingAnimation) {
        g_winMove.try_emplace(hWnd);
    }
}

void OnExitSizeMove(HWND hWnd)
{
    bool unsubclass = true;
    auto winMovingIt = g_winMoving.find(hWnd);
    auto winMoveIt = g_winMove.find(hWnd);

    if (winMoveIt != g_winMove.end()) {
        auto& windowMove = winMoveIt->second;

        int x, y;
        double velocityX, velocityY;
        if (windowMove.CompleteMove(&x, &y, &velocityX, &velocityY)) {
            DWORD messagePos = GetMessagePos();

            g_winSlideTimers.try_emplace(hWnd,
                WindowSlideTimerProc,
                GET_X_LPARAM(messagePos) - x,
                GET_Y_LPARAM(messagePos) - y,
                x,
                y,
                velocityX,
                velocityY,
                !g_settings.snapWindowsWhenSliding
                    ? std::nullopt
                    : std::optional(winMovingIt != g_winMoving.end()
                        ? std::move(winMovingIt->second.GetWindowMagnet())
                        : WindowMagnet(hWnd)));

            unsubclass = false;
        }

        g_winMove.erase(winMoveIt);
    }

    if (winMovingIt != g_winMoving.end()) {
        g_winMoving.erase(winMovingIt);
    }

    if (unsubclass) {
        UnsubclassWindow(hWnd);
    }
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

    auto& windowMoving = it->second;
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
        // Resize adjustments are handled by OnSizing, but reset the move history.
        windowMoving.ForgetLastPos();
    }
}

void OnWindowPosChanged(HWND hWnd, const WINDOWPOS* windowPos)
{
    auto it = g_winMove.find(hWnd);
    if (it == g_winMove.end()) {
        // SWP_STATECHANGED is set when the state changes, e.g. the window is
        // maximized.
        // 0x00300000 is set when the window is snapped, e.g. with Win+left.
        if ((windowPos->flags & SWP_STATECHANGED) || (windowPos->flags & 0x00300000)) {
            if (KillWindowSlideTimer(hWnd)) {
                UnsubclassWindow(hWnd);
            }
        }

        return;
    }

    auto& windowMove = it->second;

    if ((windowPos->flags & SWP_NOMOVE) || !(windowPos->flags & SWP_NOSIZE)) {
        // Somethings that's not a move happened, reset.
        windowMove.Reset();
        return;
    }

    windowMove.UpdateWithNewPos(hWnd, windowPos->x, windowPos->y);
}

void OnSizing(HWND hWnd, WPARAM edge, RECT* windowRect)
{
    if (!windowRect) {
        return;
    }

    auto it = g_winMoving.find(hWnd);
    if (it == g_winMoving.end()) {
        return;
    }

    it->second.GetWindowMagnet().MagnetResize(hWnd, windowRect, edge);
}

void OnSysCommand(HWND hWnd, WPARAM command)
{
    switch (command) {
    case SC_SIZE:
    case SC_MOVE:
    case SC_MINIMIZE:
    case SC_MAXIMIZE:
    case SC_CLOSE:
    case SC_MOUSEMENU:
    case SC_KEYMENU:
    case SC_RESTORE:
        {
            if (KillWindowSlideTimer(hWnd)) {
                UnsubclassWindow(hWnd);
            }
        }
        break;
    }
}

void OnNcDestroy(HWND hWnd)
{
    KillWindowSlideTimer(hWnd);
    UnsubclassWindow(hWnd);
}

LRESULT CALLBACK SubclassWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    auto hookScope = hookRefCountScope();

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

    case WM_WINDOWPOSCHANGED:
        OnWindowPosChanged(hWnd, (const WINDOWPOS*)lParam);
        break;

    case WM_SIZING:
        OnSizing(hWnd, wParam, (RECT*)lParam);
        break;

    case WM_SYSCOMMAND:
        OnSysCommand(hWnd, wParam);
        break;

    case WM_NCDESTROY:
        OnNcDestroy(hWnd);
        break;

    default:
        if (uMsg == g_unsubclassRegisteredMessage) {
            KillWindowSlideTimer(hWnd);
            RemoveWindowSubclass(hWnd, SubclassWndProc, 0);
        }
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    auto hookScope = hookRefCountScope();

    if (nCode != HC_ACTION) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;

    if (cwp->message == WM_ENTERSIZEMOVE) {
        WCHAR className[32];
        if (GetClassName(GetAncestor(cwp->hwnd, GA_ROOT), className, ARRAYSIZE(className)) &&
            (wcsicmp(className, L"Shell_TrayWnd") == 0 || wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0)) {
            // Skip the taskbar as it causes wired
            // behavior and doesn't make sense in general.
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
    auto hookScope = hookRefCountScope();

    if (lpMsg && lpMsg->hwnd) {
        SetWindowHookForUiThreadIfNeeded(lpMsg->hwnd);
    }

    return pOriginalDispatchMessageA(lpMsg);
}

using DispatchMessageW_t = decltype(&DispatchMessageW);
DispatchMessageW_t pOriginalDispatchMessageW;
LRESULT WINAPI DispatchMessageWHook(CONST MSG *lpMsg)
{
    auto hookScope = hookRefCountScope();

    if (lpMsg && lpMsg->hwnd) {
        SetWindowHookForUiThreadIfNeeded(lpMsg->hwnd);
    }

    return pOriginalDispatchMessageW(lpMsg);
}

using IsDialogMessageA_t = decltype(&IsDialogMessageA);
IsDialogMessageA_t pOriginalIsDialogMessageA;
LRESULT WINAPI IsDialogMessageAHook(HWND hDlg, LPMSG lpMsg)
{
    auto hookScope = hookRefCountScope();

    if (hDlg) {
        SetWindowHookForUiThreadIfNeeded(hDlg);
    }

    return pOriginalIsDialogMessageA(hDlg, lpMsg);
}

using IsDialogMessageW_t = decltype(&IsDialogMessageW);
IsDialogMessageW_t pOriginalIsDialogMessageW;
LRESULT WINAPI IsDialogMessageWHook(HWND hDlg, LPMSG lpMsg)
{
    auto hookScope = hookRefCountScope();

    if (hDlg) {
        SetWindowHookForUiThreadIfNeeded(hDlg);
    }

    return pOriginalIsDialogMessageW(hDlg, lpMsg);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        break;

    case DLL_THREAD_ATTACH:
        break;

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

    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}

void LoadSettings()
{
    g_settings.snapWindowsWhenDragging = Wh_GetIntSetting(L"SnapWindowsWhenDragging");
    g_settings.snapWindowsDistance = Wh_GetIntSetting(L"SnapWindowsDistance");
    g_settings.keysToDisableSnappingCtrl = Wh_GetIntSetting(L"KeysToDisableSnappingCtrl");
    g_settings.keysToDisableSnappingAlt = Wh_GetIntSetting(L"KeysToDisableSnappingAlt");
    g_settings.keysToDisableSnappingShift = Wh_GetIntSetting(L"KeysToDisableSnappingShift");
    g_settings.slidingAnimation = Wh_GetIntSetting(L"SlidingAnimation");
    g_settings.snapWindowsWhenSliding = Wh_GetIntSetting(L"SnapWindowsWhenSliding");
    g_settings.slidingAnimationSlowdown = Wh_GetIntSetting(L"SlidingAnimationSlowdown");
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

    // DispatchMessageA, DispatchMessageW could hopefully be enough to detect a message loop, but
    // DispatchMessageWorker, which implements DispatchMessageA, DispatchMessageW, is sometimes
    // called directly by functions such as DialogBoxParam.
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
        subclassedWindows = std::move(g_subclassedWindows);
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
