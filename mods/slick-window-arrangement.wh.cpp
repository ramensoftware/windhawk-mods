// ==WindhawkMod==
// @id              slick-window-arrangement
// @name            Slick Window Arrangement
// @description     Make window arrangement more slick and pleasant with a sliding animation and snapping
// @version         1.0.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         *
// @compilerOptions -lcomctl32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Slick Window Arrangement
Window arrangement is boring, make it more slick and pleasant with a
sliding animation and window snapping.

![demonstration](https://i.imgur.com/GB4guXU.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- SnapWindowsWhenDragging: true
  $name: Snap windows when dragging
- SnapWindowsDistance: 25
  $name: Snap windows distance
  $description: Set the required distance for windows to snap to other windows
- KeysToDisableSnapping:
  - Ctrl: false
  - Alt: true
  - Shift: false
  $name: Keys to temporarily disable snapping
  $description: A combination of keys that can be used to temporarily disable snapping
- SlidingAnimation: true
  $name: Sliding animation
  $description: Keep sliding the window after it's being moved
- SnapWindowsWhenSliding: true
  $name: Snap windows when sliding
- SlidingAnimationSlowdown: 15
  $name: Sliding animation slowdown
  $description: Set a smaller value for a sliding animation that lasts longer (between 1 and 99)
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

            if (target != LONG_MAX) {
                if (a == target) {
                    continue;
                }

                if (std::abs(source - a) >= std::abs(source - target)) {
                    break;
                }
            }

            if (otherAxisStart < c && otherAxisEnd > b) {
                target = a;
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
        DWORD messagePos = GetMessagePos();

        if (firstMoveDone) {
            // Adjust window pos, which can be off in the DPI contexts:
            // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE
            // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2

            int lastDeltaX = GET_X_LPARAM(lastMessagePos) - lastX;
            int lastDeltaY = GET_Y_LPARAM(lastMessagePos) - lastY;

            int deltaX = GET_X_LPARAM(messagePos) - *x;
            int deltaY = GET_Y_LPARAM(messagePos) - *y;

            *x -= lastDeltaX - deltaX;
            *y -= lastDeltaY - deltaY;
        }

        lastMessagePos = messagePos;
        lastX = *x;
        lastY = *y;

        windowMagnet.MagnetMove(hTargetWnd, x, y, cx, cy);

        firstMoveDone = true;
    }

    WindowMagnet& GetWindowMagnet() {
        return windowMagnet;
    }

private:
    bool firstMoveDone = false;
    DWORD lastMessagePos;
    int lastX, lastY;
    WindowMagnet windowMagnet;
};

class WindowMove {
public:
    WindowMove() {}

    void Reset() {
        for (auto& snapshotItem : snapshot) {
            snapshotItem = nullptr;
        }
    }

    void UpdateWithNewPos(int x, int y) {
        DWORD tickCount = GetTickCount();

        if (snapshot[0] && tickCount - snapshot[0]->tickCount >= 100) {
            // Waited for too long, reset.
            Reset();
        }

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
    struct MoveSnapshot {
        DWORD tickCount;
        int x, y;
    };

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
    L"Windhawk_Unsubclass_slick-window-arrangement");
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

    auto& timer = it->second;
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

    bool posChanged = rc.left != windowPos->x || rc.top != windowPos->y;
    bool sizeChanged = rc.right - rc.left != windowPos->cx || rc.bottom - rc.top != windowPos->cy;

    if (posChanged && !sizeChanged) {
        auto it = g_winMoving.find(hWnd);
        if (it != g_winMoving.end()) {
            auto& windowMoving = it->second;
            windowMoving.PreProcessPos(hWnd, &windowPos->x, &windowPos->y, &windowPos->cx, &windowPos->cy);
        }
    }
    else if (sizeChanged) {
        // Maybe support resize one day...
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

    windowMove.UpdateWithNewPos(windowPos->x, windowPos->y);
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

void OnDestroy(HWND hWnd)
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

    case WM_SYSCOMMAND:
        OnSysCommand(hWnd, wParam);
        break;

    case WM_DESTROY:
        OnDestroy(hWnd);
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
    g_settings.keysToDisableSnappingCtrl = Wh_GetIntSetting(L"KeysToDisableSnapping.Ctrl");
    g_settings.keysToDisableSnappingAlt = Wh_GetIntSetting(L"KeysToDisableSnapping.Alt");
    g_settings.keysToDisableSnappingShift = Wh_GetIntSetting(L"KeysToDisableSnapping.Shift");
    g_settings.slidingAnimation = Wh_GetIntSetting(L"SlidingAnimation");
    g_settings.snapWindowsWhenSliding = Wh_GetIntSetting(L"SnapWindowsWhenSliding");
    g_settings.slidingAnimationSlowdown = Wh_GetIntSetting(L"SlidingAnimationSlowdown");
}

BOOL Wh_ModInit(void)
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

void Wh_ModUninit(void)
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

void Wh_ModSettingsChanged(void)
{
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}
