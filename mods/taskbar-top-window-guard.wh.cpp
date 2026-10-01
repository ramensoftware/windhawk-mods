// ==WindhawkMod==
// @id              taskbar-top-window-guard
// @name            Taskbar Top Window Guard
// @description     Keeps windows out from under a top taskbar, with optional centering when windows open.
// @version         0.5.0
// @author          Eron Greco Melo
// @github          https://github.com/eronGreco
// @include         windhawk.exe
// @compilerOptions -ldwmapi -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- centerWindowsOnOpen: false
  $name: Center windows when they open
  $description: Center normal windows the first time they appear, inside the usable monitor area. Existing/restored windows are not re-centered.
- centerVerticalOffset: 0
  $name: Center vertical offset
  $description: Extra vertical offset in pixels after centering. Positive values move the centered window down; negative values move it up.
- centerAlwaysOnTopWindows: false
  $name: Center always-on-top windows
  $description: When disabled (recommended), Picture-in-Picture and other always-on-top windows keep their own position instead of being centered.
- extraTopPadding: 0
  $name: Extra top padding
  $description: Extra pixels to keep between the taskbar and the visible top edge of a corrected window.
- recheckDurationMs: 1500
  $name: Recheck duration (ms)
  $description: How long to keep checking a window after it appears, moves, or becomes foreground. Helps apps which restore a saved position shortly after opening.
- recheckIntervalMs: 100
  $name: Recheck interval (ms)
  $description: Interval between checks during the short event-triggered recheck period.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Taskbar Top Window Guard

Prevents regular application windows from opening or being restored with their
title bar hidden behind a taskbar placed at the top of a monitor. It's intended
for top-taskbar configurations, including the **Taskbar on top for Windows 11**
Windhawk mod and Windows builds/configurations that place the taskbar at the top.

The mod preserves the application's requested size and position whenever
possible. If a window overlaps the protected top taskbar area, it's moved down
only as far as necessary to keep its top edge accessible.

Maximized and fullscreen windows are intentionally left untouched. Auto-hidden
taskbars also don't force windows down while hidden.

## Optional centering

Enable **Center windows when they open** to center normal windows inside the
usable area of their monitor on their first appearance. A window is remembered
for the lifetime of its HWND, so minimizing/restoring it, Alt+Tab, switching
virtual desktops, or hiding/showing it again won't re-center it.

Always-on-top windows are excluded from centering by default. This keeps
Picture-in-Picture windows, such as Chrome/Chromium PiP, at the position chosen
by the user. Owned windows such as dialogs are also left to their application.
Top-taskbar protection still applies to eligible always-on-top windows.

This option intentionally overlaps only part of **Center New Windows**. The
existing mod is the better choice if centering is your main goal. This mod's
centering is a lightweight companion to the top-taskbar guard: it runs from a
dedicated Windhawk tool process rather than injecting into every application,
centers against the taskbar-aware usable area, centers only once per window,
and skips always-on-top windows by default.

## How it works

The mod is event-driven. It listens for window show, foreground, restore,
uncloak, location-change, move/size, and destroy events. After a relevant event
it briefly rechecks only that window, which catches applications that restore a
saved position shortly after opening without continuously enumerating all
windows.

The usable top boundary comes from the monitor work area plus the actual
`Shell_TrayWnd` / `Shell_SecondaryTrayWnd` rectangle when necessary. Window
moves are queued asynchronously, and unresponsive applications are skipped so
a hung app can't block the guard.
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windows.h>
#include <dwmapi.h>
#include <shellapi.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <unordered_map>
#include <unordered_set>

static std::atomic<bool> g_centerWindowsOnOpen{false};
static std::atomic<int> g_centerVerticalOffset{0};
static std::atomic<bool> g_centerAlwaysOnTopWindows{false};
static std::atomic<int> g_extraTopPadding{0};
static std::atomic<DWORD> g_recheckDurationMs{1500};
static std::atomic<UINT> g_recheckIntervalMs{100};

static HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;
static HANDLE g_threadReadyEvent = nullptr;
static UINT_PTR g_recheckTimer = 0;

struct RecheckState {
    ULONGLONG until;
    ULONGLONG centerUntil;
};

static std::unordered_map<HWND, RecheckState> g_rechecks;
static std::unordered_set<HWND> g_seenWindows;
static std::unordered_set<HWND> g_moveSizeWindows;

static void LoadSettings() {
    int centerOffset = Wh_GetIntSetting(L"centerVerticalOffset");
    int padding = Wh_GetIntSetting(L"extraTopPadding");
    int duration = Wh_GetIntSetting(L"recheckDurationMs");
    int interval = Wh_GetIntSetting(L"recheckIntervalMs");

    if (centerOffset < -1000) centerOffset = -1000;
    if (centerOffset > 1000) centerOffset = 1000;

    if (padding < 0) padding = 0;
    if (padding > 200) padding = 200;

    if (duration < 0) duration = 0;
    if (duration > 10000) duration = 10000;

    if (interval < 25) interval = 25;
    if (interval > 1000) interval = 1000;

    g_centerWindowsOnOpen.store(Wh_GetIntSetting(L"centerWindowsOnOpen") != 0);
    g_centerVerticalOffset.store(centerOffset);
    g_centerAlwaysOnTopWindows.store(
        Wh_GetIntSetting(L"centerAlwaysOnTopWindows") != 0);
    g_extraTopPadding.store(padding);
    g_recheckDurationMs.store(static_cast<DWORD>(duration));
    g_recheckIntervalMs.store(static_cast<UINT>(interval));
}

static bool IsExcludedClass(HWND hwnd) {
    wchar_t className[128] = {};
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        return false;
    }

    static const wchar_t* const excludedClasses[] = {
        L"Shell_TrayWnd",
        L"Shell_SecondaryTrayWnd",
        L"Progman",
        L"WorkerW",
        L"NotifyIconOverflowWindow",
        L"MultitaskingViewFrame",
        L"Windows.UI.Core.CoreWindow",
        L"Microsoft.UI.Content.PopupWindowSiteBridge",
        L"XamlExplorerHostIslandWindow"
    };

    for (const auto* excluded : excludedClasses) {
        if (wcscmp(className, excluded) == 0) {
            return true;
        }
    }

    return false;
}

// Used to remember windows that already existed before the mod saw them become
// visible. Unlike IsEligibleWindow, this deliberately accepts minimized and
// DWM-cloaked windows so virtual-desktop switches don't make them look new.
static bool IsTrackableTopLevelWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd) || GetAncestor(hwnd, GA_ROOT) != hwnd) {
        return false;
    }

    const LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    const LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

    if (style & WS_CHILD) {
        return false;
    }

    if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) {
        return false;
    }

    if (IsExcludedClass(hwnd)) {
        return false;
    }

    RECT rc = {};
    if (!GetWindowRect(hwnd, &rc)) {
        return false;
    }

    return (rc.right - rc.left) >= 120 && (rc.bottom - rc.top) >= 70;
}

static bool IsEligibleWindow(HWND hwnd) {
    if (!IsTrackableTopLevelWindow(hwnd) ||
        !IsWindowVisible(hwnd) ||
        IsIconic(hwnd) ||
        IsZoomed(hwnd)) {
        return false;
    }

    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(
            hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
        cloaked) {
        return false;
    }

    return true;
}

static bool IsWindowBeingMovedOrSized(HWND hwnd) {
    return g_moveSizeWindows.find(hwnd) != g_moveSizeWindows.end();
}

static bool IsAutoHideTaskbarEnabled() {
    APPBARDATA appBarData = {sizeof(appBarData)};
    return (SHAppBarMessage(ABM_GETSTATE, &appBarData) & ABS_AUTOHIDE) != 0;
}

static void ConsiderTaskbarWindow(
    HWND taskbar,
    HMONITOR targetMonitor,
    const RECT& monitorRect,
    LONG* taskbarBottom,
    bool* found) {

    if (!taskbar || !IsWindow(taskbar)) {
        return;
    }

    HMONITOR taskbarMonitor =
        MonitorFromWindow(taskbar, MONITOR_DEFAULTTONULL);
    if (!taskbarMonitor || taskbarMonitor != targetMonitor) {
        return;
    }

    RECT taskbarRect = {};
    if (!GetWindowRect(taskbar, &taskbarRect)) {
        return;
    }

    constexpr LONG kEdgeTolerance = 12;
    if (taskbarRect.top <= monitorRect.top + kEdgeTolerance &&
        taskbarRect.bottom > monitorRect.top &&
        taskbarRect.bottom < monitorRect.bottom) {
        *taskbarBottom = std::max(*taskbarBottom, taskbarRect.bottom);
        *found = true;
    }
}

static bool GetProtectedTopForWindow(HWND hwnd, LONG* protectedTop) {
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
        return false;
    }

    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfoW(monitor, &mi)) {
        return false;
    }

    LONG safeTop = mi.rcWork.top;
    bool hasProtectedTop = mi.rcWork.top > mi.rcMonitor.top;

    // With auto-hide, don't use the temporarily revealed taskbar rectangle as
    // a persistent boundary. rcWork is the stable Windows-provided boundary.
    if (!IsAutoHideTaskbarEnabled()) {
        LONG taskbarBottom = mi.rcMonitor.top;
        bool found = false;

        ConsiderTaskbarWindow(
            FindWindowW(L"Shell_TrayWnd", nullptr),
            monitor,
            mi.rcMonitor,
            &taskbarBottom,
            &found);

        for (HWND secondary = nullptr;
             (secondary = FindWindowExW(
                  nullptr,
                  secondary,
                  L"Shell_SecondaryTrayWnd",
                  nullptr));) {
            ConsiderTaskbarWindow(
                secondary,
                monitor,
                mi.rcMonitor,
                &taskbarBottom,
                &found);
        }

        if (found) {
            safeTop = std::max(safeTop, taskbarBottom);
            hasProtectedTop = true;
        }
    }

    if (!hasProtectedTop) {
        return false;
    }

    safeTop += g_extraTopPadding.load();
    *protectedTop = safeTop;
    return true;
}

static bool IsFullscreenLike(HWND hwnd, const RECT& visibleRect) {
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
        return false;
    }

    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfoW(monitor, &mi)) {
        return false;
    }

    constexpr LONG kTolerance = 3;
    return std::abs(visibleRect.left - mi.rcMonitor.left) <= kTolerance &&
           std::abs(visibleRect.top - mi.rcMonitor.top) <= kTolerance &&
           std::abs(visibleRect.right - mi.rcMonitor.right) <= kTolerance &&
           std::abs(visibleRect.bottom - mi.rcMonitor.bottom) <= kTolerance;
}

static bool GetUsableRectForWindow(HWND hwnd, RECT* usableRect) {
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
        return false;
    }

    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfoW(monitor, &mi)) {
        return false;
    }

    RECT result = mi.rcWork;

    LONG protectedTop = 0;
    if (GetProtectedTopForWindow(hwnd, &protectedTop)) {
        result.top = std::max(result.top, protectedTop);
    }

    if (result.right <= result.left || result.bottom <= result.top) {
        return false;
    }

    *usableRect = result;
    return true;
}

static bool IsEligibleForCentering(HWND hwnd) {
    if (!IsEligibleWindow(hwnd)) {
        return false;
    }

    // Let applications position their own owned dialogs relative to the owner.
    if (GetWindow(hwnd, GW_OWNER)) {
        return false;
    }

    if (!g_centerAlwaysOnTopWindows.load()) {
        const LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        if (exStyle & WS_EX_TOPMOST) {
            return false;
        }
    }

    return true;
}

static bool GetVisibleWindowRect(HWND hwnd, RECT* windowRect, RECT* visibleRect) {
    if (!GetWindowRect(hwnd, windowRect)) {
        return false;
    }

    *visibleRect = *windowRect;
    if (FAILED(DwmGetWindowAttribute(
            hwnd,
            DWMWA_EXTENDED_FRAME_BOUNDS,
            visibleRect,
            sizeof(*visibleRect)))) {
        *visibleRect = *windowRect;
    }

    return true;
}

// Returns true when an asynchronous move was queued.
static bool CenterWindowIfNeeded(HWND hwnd) {
    if (!g_centerWindowsOnOpen.load() ||
        !IsEligibleForCentering(hwnd) ||
        IsWindowBeingMovedOrSized(hwnd) ||
        IsHungAppWindow(hwnd)) {
        return false;
    }

    RECT windowRect = {};
    RECT visibleRect = {};
    if (!GetVisibleWindowRect(hwnd, &windowRect, &visibleRect) ||
        IsFullscreenLike(hwnd, visibleRect)) {
        return false;
    }

    RECT usableRect = {};
    if (!GetUsableRectForWindow(hwnd, &usableRect)) {
        return false;
    }

    const LONG visibleWidth = visibleRect.right - visibleRect.left;
    const LONG visibleHeight = visibleRect.bottom - visibleRect.top;
    const LONG usableWidth = usableRect.right - usableRect.left;
    const LONG usableHeight = usableRect.bottom - usableRect.top;

    LONG desiredVisibleLeft = usableRect.left;
    LONG desiredVisibleTop = usableRect.top;

    if (visibleWidth < usableWidth) {
        desiredVisibleLeft =
            usableRect.left + (usableWidth - visibleWidth) / 2;
    }

    if (visibleHeight < usableHeight) {
        desiredVisibleTop =
            usableRect.top + (usableHeight - visibleHeight) / 2;
    }

    desiredVisibleTop += g_centerVerticalOffset.load();

    if (visibleWidth <= usableWidth) {
        desiredVisibleLeft = std::clamp(
            desiredVisibleLeft,
            usableRect.left,
            usableRect.right - visibleWidth);
    }

    if (visibleHeight <= usableHeight) {
        desiredVisibleTop = std::clamp(
            desiredVisibleTop,
            usableRect.top,
            usableRect.bottom - visibleHeight);
    }

    const LONG frameOffsetX = windowRect.left - visibleRect.left;
    const LONG frameOffsetY = windowRect.top - visibleRect.top;

    const int newX = desiredVisibleLeft + frameOffsetX;
    const int newY = desiredVisibleTop + frameOffsetY;

    if (std::abs(windowRect.left - newX) <= 1 &&
        std::abs(windowRect.top - newY) <= 1) {
        return false;
    }

    if (!SetWindowPos(
            hwnd,
            nullptr,
            newX,
            newY,
            0,
            0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                SWP_ASYNCWINDOWPOS)) {
        Wh_Log(
            L"Center SetWindowPos failed for hwnd=%p error=%lu",
            hwnd,
            GetLastError());
        return false;
    }

    Wh_Log(L"Queued centering for hwnd=%p x=%d y=%d", hwnd, newX, newY);
    return true;
}

// Returns true when an asynchronous corrective move was queued.
static bool CorrectWindowIfNeeded(HWND hwnd) {
    if (!IsEligibleWindow(hwnd) ||
        IsWindowBeingMovedOrSized(hwnd) ||
        IsHungAppWindow(hwnd)) {
        return false;
    }

    LONG protectedTop = 0;
    if (!GetProtectedTopForWindow(hwnd, &protectedTop)) {
        return false;
    }

    RECT windowRect = {};
    RECT visibleRect = {};
    if (!GetVisibleWindowRect(hwnd, &windowRect, &visibleRect) ||
        IsFullscreenLike(hwnd, visibleRect)) {
        return false;
    }

    if (visibleRect.top >= protectedTop || visibleRect.bottom <= protectedTop) {
        return false;
    }

    const LONG deltaY = protectedTop - visibleRect.top;
    const int newY = windowRect.top + deltaY;

    if (!SetWindowPos(
            hwnd,
            nullptr,
            windowRect.left,
            newY,
            0,
            0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                SWP_ASYNCWINDOWPOS)) {
        Wh_Log(
            L"SetWindowPos failed for hwnd=%p error=%lu",
            hwnd,
            GetLastError());
        return false;
    }

    Wh_Log(
        L"Queued correction for hwnd=%p deltaY=%ld protectedTop=%ld",
        hwnd,
        deltaY,
        protectedTop);
    return true;
}

static void StopRecheckTimerIfIdle() {
    if (g_rechecks.empty() && g_recheckTimer) {
        KillTimer(nullptr, g_recheckTimer);
        g_recheckTimer = 0;
    }
}

static VOID CALLBACK RecheckTimerProc(HWND, UINT, UINT_PTR, DWORD) {
    const ULONGLONG now = GetTickCount64();

    for (auto it = g_rechecks.begin(); it != g_rechecks.end();) {
        HWND hwnd = it->first;
        RecheckState& state = it->second;

        if (!IsWindow(hwnd) || now >= state.until) {
            it = g_rechecks.erase(it);
            continue;
        }

        if (IsWindowBeingMovedOrSized(hwnd)) {
            ++it;
            continue;
        }

        bool moveQueued = false;
        if (state.centerUntil && now < state.centerUntil) {
            moveQueued = CenterWindowIfNeeded(hwnd);
        } else {
            state.centerUntil = 0;
        }

        // An asynchronous centering move may not be reflected by GetWindowRect
        // immediately. Don't read stale geometry and queue a conflicting guard
        // move in the same tick.
        if (!moveQueued) {
            CorrectWindowIfNeeded(hwnd);
        }

        ++it;
    }

    StopRecheckTimerIfIdle();
}

static void EnsureRecheckTimer() {
    if (g_recheckTimer || g_rechecks.empty()) {
        return;
    }

    g_recheckTimer = SetTimer(
        nullptr,
        0,
        g_recheckIntervalMs.load(),
        RecheckTimerProc);

    if (!g_recheckTimer) {
        Wh_Log(L"SetTimer failed: %lu", GetLastError());
    }
}

static void StartRecheck(HWND hwnd, bool firstAppearanceCandidate) {
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }

    bool firstAppearance = false;
    if (firstAppearanceCandidate && IsTrackableTopLevelWindow(hwnd)) {
        firstAppearance = g_seenWindows.insert(hwnd).second;
    }

    if (!IsEligibleWindow(hwnd)) {
        return;
    }

    const ULONGLONG now = GetTickCount64();
    bool moveQueued = false;

    const bool shouldCenter =
        firstAppearance &&
        g_centerWindowsOnOpen.load() &&
        IsEligibleForCentering(hwnd);

    if (shouldCenter) {
        moveQueued = CenterWindowIfNeeded(hwnd);
    }

    if (!moveQueued) {
        CorrectWindowIfNeeded(hwnd);
    }

    const DWORD duration = g_recheckDurationMs.load();
    if (!duration) {
        return;
    }

    RecheckState& state = g_rechecks[hwnd];
    state.until = std::max(state.until, now + duration);

    // Some applications reapply their remembered position shortly after their
    // first ShowWindow. Reapply centering briefly, without letting a later
    // foreground event cancel the pending centering window.
    if (shouldCenter) {
        state.centerUntil = std::max(
            state.centerUntil,
            now + std::min<DWORD>(duration, 700));
    }

    EnsureRecheckTimer();
}

static BOOL CALLBACK RememberExistingWindowsProc(HWND hwnd, LPARAM) {
    if (IsTrackableTopLevelWindow(hwnd)) {
        g_seenWindows.insert(hwnd);
    }
    return TRUE;
}

static void CALLBACK WinEventProc(
    HWINEVENTHOOK,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG idChild,
    DWORD,
    DWORD) {

    if (!hwnd) {
        return;
    }

    if (event == EVENT_OBJECT_DESTROY) {
        if (idObject == OBJID_WINDOW && idChild == CHILDID_SELF) {
            g_seenWindows.erase(hwnd);
            g_moveSizeWindows.erase(hwnd);
            g_rechecks.erase(hwnd);
            StopRecheckTimerIfIdle();
        }
        return;
    }

    if (event == EVENT_SYSTEM_MOVESIZESTART) {
        g_moveSizeWindows.insert(hwnd);
        auto stateIt = g_rechecks.find(hwnd);
        if (stateIt != g_rechecks.end()) {
            stateIt->second.centerUntil = 0;
        }
        return;
    }

    if (event == EVENT_SYSTEM_MOVESIZEEND) {
        g_moveSizeWindows.erase(hwnd);
        StartRecheck(hwnd, false);
        return;
    }

    if (event == EVENT_OBJECT_SHOW ||
        event == EVENT_OBJECT_UNCLOAKED ||
        event == EVENT_OBJECT_LOCATIONCHANGE) {
        if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
            return;
        }
    }

    switch (event) {
        case EVENT_OBJECT_SHOW:
        case EVENT_OBJECT_UNCLOAKED:
            StartRecheck(hwnd, true);
            break;

        case EVENT_OBJECT_LOCATIONCHANGE:
            if (!IsWindowBeingMovedOrSized(hwnd)) {
                StartRecheck(hwnd, false);
            }
            break;

        case EVENT_SYSTEM_MINIMIZEEND:
        case EVENT_SYSTEM_FOREGROUND:
            // Restoring or focusing an existing window must enforce the guard,
            // but must not make "center when they open" behave like Alt+Tab
            // centering.
            StartRecheck(hwnd, false);
            break;
    }
}

static DWORD WINAPI WinEventThreadProc(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    MSG msg = {};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    // Remember all current top-level application windows, including minimized
    // and DWM-cloaked ones, so enabling the mod or switching virtual desktops
    // never makes an old window look newly opened.
    EnumWindows(RememberExistingWindowsProc, 0);

    HWINEVENTHOOK foregroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND,
        EVENT_SYSTEM_FOREGROUND,
        nullptr,
        WinEventProc,
        0,
        0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    HWINEVENTHOOK moveSizeHook = SetWinEventHook(
        EVENT_SYSTEM_MOVESIZESTART,
        EVENT_SYSTEM_MOVESIZEEND,
        nullptr,
        WinEventProc,
        0,
        0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    HWINEVENTHOOK minimizeEndHook = SetWinEventHook(
        EVENT_SYSTEM_MINIMIZEEND,
        EVENT_SYSTEM_MINIMIZEEND,
        nullptr,
        WinEventProc,
        0,
        0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    HWINEVENTHOOK destroyHook = SetWinEventHook(
        EVENT_OBJECT_DESTROY,
        EVENT_OBJECT_DESTROY,
        nullptr,
        WinEventProc,
        0,
        0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    HWINEVENTHOOK showHook = SetWinEventHook(
        EVENT_OBJECT_SHOW,
        EVENT_OBJECT_SHOW,
        nullptr,
        WinEventProc,
        0,
        0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    HWINEVENTHOOK locationHook = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE,
        EVENT_OBJECT_LOCATIONCHANGE,
        nullptr,
        WinEventProc,
        0,
        0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    HWINEVENTHOOK uncloakedHook = SetWinEventHook(
        EVENT_OBJECT_UNCLOAKED,
        EVENT_OBJECT_UNCLOAKED,
        nullptr,
        WinEventProc,
        0,
        0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    if (!foregroundHook ||
        !moveSizeHook ||
        !minimizeEndHook ||
        !destroyHook ||
        !showHook ||
        !locationHook ||
        !uncloakedHook) {
        Wh_Log(L"One or more SetWinEventHook calls failed: %lu", GetLastError());
    }

    if (g_threadReadyEvent) {
        SetEvent(g_threadReadyEvent);
    }

    // Enforce the guard for the active window when the mod starts, but don't
    // center it merely because the mod was enabled.
    StartRecheck(GetForegroundWindow(), false);

    BOOL result;
    while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0) {
        if (result == -1) {
            break;
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_recheckTimer) {
        KillTimer(nullptr, g_recheckTimer);
        g_recheckTimer = 0;
    }

    g_rechecks.clear();
    g_seenWindows.clear();
    g_moveSizeWindows.clear();

    if (uncloakedHook) UnhookWinEvent(uncloakedHook);
    if (locationHook) UnhookWinEvent(locationHook);
    if (showHook) UnhookWinEvent(showHook);
    if (destroyHook) UnhookWinEvent(destroyHook);
    if (minimizeEndHook) UnhookWinEvent(minimizeEndHook);
    if (moveSizeHook) UnhookWinEvent(moveSizeHook);
    if (foregroundHook) UnhookWinEvent(foregroundHook);

    return 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();

    g_threadReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_threadReadyEvent) {
        Wh_Log(L"CreateEvent failed: %lu", GetLastError());
        return FALSE;
    }

    g_hookThread = CreateThread(
        nullptr,
        0,
        WinEventThreadProc,
        nullptr,
        0,
        &g_hookThreadId);

    if (!g_hookThread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        CloseHandle(g_threadReadyEvent);
        g_threadReadyEvent = nullptr;
        return FALSE;
    }

    DWORD waitResult = WaitForSingleObject(g_threadReadyEvent, 5000);
    if (waitResult != WAIT_OBJECT_0) {
        Wh_Log(L"Hook thread didn't initialize correctly");
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, 2000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        CloseHandle(g_threadReadyEvent);
        g_threadReadyEvent = nullptr;
        return FALSE;
    }

    CloseHandle(g_threadReadyEvent);
    g_threadReadyEvent = nullptr;

    Wh_Log(L"Taskbar Top Window Guard initialized");
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    if (g_hookThread) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        g_hookThreadId = 0;
    }

    if (g_threadReadyEvent) {
        CloseHandle(g_threadReadyEvent);
        g_threadReadyEvent = nullptr;
    }
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
