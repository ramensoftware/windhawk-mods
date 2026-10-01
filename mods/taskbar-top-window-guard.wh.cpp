// ==WindhawkMod==
// @id              taskbar-top-window-guard
// @name            Taskbar Top Window Guard
// @description     Keeps windows out from under a top taskbar, with optional centering when windows open.
// @version         0.4.0
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
  $description: Center normal windows inside the usable monitor area when they are shown, restored, or uncloaked. This does not recenter a window just because you Alt+Tab to it.
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
  $description: How long to keep checking a window after it appears or becomes foreground. Helps apps which restore their saved position shortly after opening.
- recheckIntervalMs: 100
  $name: Recheck interval (ms)
  $description: Interval between position checks during the short recheck period.
- logCorrections: true
  $name: Log corrections
  $description: Write a Windhawk log entry whenever a window is moved.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Taskbar Top Window Guard

Prevents regular application windows from opening with their title bar hidden
behind a taskbar placed at the top of a monitor. This is useful with top-taskbar
configurations where Windows or an application restores a window into the area
occupied by the taskbar.

The mod preserves the application's requested size and position whenever
possible. If a window overlaps the protected top taskbar area, it is moved down
only as far as necessary to keep its top edge accessible.

## Optional centering

Enable **Center windows when they open** to center normal windows inside the
usable area of their monitor when they are shown, restored, or uncloaked.
Switching to an already-open window with Alt+Tab doesn't center it again.

Always-on-top windows are excluded from centering by default. This keeps
Picture-in-Picture windows, such as Chrome/Chromium PiP, at the position chosen
by the user. Top-taskbar protection still applies to those windows.

## How it works

The mod checks both the monitor work area and the actual `Shell_TrayWnd` /
`Shell_SecondaryTrayWnd` position. It also briefly rechecks windows after they
are shown or activated, which catches applications that restore a saved window
position shortly after opening. A lightweight visibility poll is used as a
fallback for applications that reuse existing windows or don't emit the
expected show/restore events.

Maximized and fullscreen windows are intentionally left untouched.
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windows.h>
#include <dwmapi.h>
#include <shellapi.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <unordered_set>

static std::atomic<bool> g_centerWindowsOnOpen{false};
static std::atomic<int> g_centerVerticalOffset{0};
static std::atomic<bool> g_centerAlwaysOnTopWindows{false};
static std::atomic<int> g_extraTopPadding{0};
static std::atomic<DWORD> g_recheckDurationMs{1500};
static std::atomic<UINT> g_recheckIntervalMs{100};
static std::atomic<bool> g_logCorrections{true};

static HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;
static HANDLE g_threadReadyEvent = nullptr;

static HWND g_recheckHwnd = nullptr;
static ULONGLONG g_recheckUntil = 0;
static ULONGLONG g_centerUntil = 0;
static UINT_PTR g_recheckTimer = 0;
static UINT_PTR g_pollTimer = 0;
static HWND g_lastForeground = nullptr;
static std::unordered_set<HWND> g_visibleWindows;
static bool g_visibleSnapshotInitialized = false;

static constexpr UINT kPollIntervalMs = 150;

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
    g_centerAlwaysOnTopWindows.store(Wh_GetIntSetting(L"centerAlwaysOnTopWindows") != 0);
    g_extraTopPadding.store(padding);
    g_recheckDurationMs.store(static_cast<DWORD>(duration));
    g_recheckIntervalMs.store(static_cast<UINT>(interval));
    g_logCorrections.store(Wh_GetIntSetting(L"logCorrections") != 0);
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

static bool IsEligibleWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd) || IsIconic(hwnd)) {
        return false;
    }

    if (GetAncestor(hwnd, GA_ROOT) != hwnd) {
        return false;
    }

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

    if (style & WS_CHILD) {
        return false;
    }

    if (exStyle & WS_EX_TOOLWINDOW) {
        return false;
    }

    if (exStyle & WS_EX_NOACTIVATE) {
        return false;
    }

    if (IsExcludedClass(hwnd)) {
        return false;
    }

    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(
            hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
        cloaked) {
        return false;
    }

    if (IsZoomed(hwnd)) {
        return false;
    }

    RECT rc = {};
    if (!GetWindowRect(hwnd, &rc)) {
        return false;
    }

    if ((rc.right - rc.left) < 120 || (rc.bottom - rc.top) < 70) {
        return false;
    }

    return true;
}

struct TopTaskbarSearchContext {
    HMONITOR monitor;
    RECT monitorRect;
    LONG taskbarBottom;
    bool found;
};

static BOOL CALLBACK EnumTaskbarsProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<TopTaskbarSearchContext*>(lParam);

    wchar_t className[64] = {};
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        return TRUE;
    }

    if (wcscmp(className, L"Shell_TrayWnd") != 0 &&
        wcscmp(className, L"Shell_SecondaryTrayWnd") != 0) {
        return TRUE;
    }

    HMONITOR taskbarMonitor =
        MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);

    if (!taskbarMonitor || taskbarMonitor != ctx->monitor) {
        return TRUE;
    }

    RECT taskbarRect = {};
    if (!GetWindowRect(hwnd, &taskbarRect)) {
        return TRUE;
    }

    constexpr LONG kEdgeTolerance = 12;
    const LONG monitorTop = ctx->monitorRect.top;

    if (taskbarRect.top <= monitorTop + kEdgeTolerance &&
        taskbarRect.bottom > monitorTop &&
        taskbarRect.bottom < ctx->monitorRect.bottom) {
        ctx->taskbarBottom = std::max(ctx->taskbarBottom, taskbarRect.bottom);
        ctx->found = true;
    }

    return TRUE;
}

static bool GetProtectedTopForWindow(HWND hwnd, LONG* protectedTop) {
    HMONITOR monitor =
        MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

    if (!monitor) {
        return false;
    }

    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfoW(monitor, &mi)) {
        return false;
    }

    LONG safeTop = mi.rcWork.top;
    bool hasProtectedTop = mi.rcWork.top > mi.rcMonitor.top;

    TopTaskbarSearchContext ctx = {
        monitor,
        mi.rcMonitor,
        mi.rcMonitor.top,
        false
    };

    EnumWindows(EnumTaskbarsProc, reinterpret_cast<LPARAM>(&ctx));

    if (ctx.found) {
        safeTop = std::max(safeTop, ctx.taskbarBottom);
        hasProtectedTop = true;
    }

    if (!hasProtectedTop) {
        return false;
    }

    safeTop += g_extraTopPadding.load();
    *protectedTop = safeTop;
    return true;
}

static bool IsFullscreenLike(HWND hwnd, const RECT& visibleRect) {
    HMONITOR monitor =
        MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

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
    HMONITOR monitor =
        MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

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

    if (!g_centerAlwaysOnTopWindows.load()) {
        const LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        if (exStyle & WS_EX_TOPMOST) {
            return false;
        }
    }

    return true;
}

static void CenterWindowIfNeeded(HWND hwnd) {
    if (!g_centerWindowsOnOpen.load() || !IsEligibleForCentering(hwnd)) {
        return;
    }

    RECT windowRect = {};
    if (!GetWindowRect(hwnd, &windowRect)) {
        return;
    }

    RECT visibleRect = windowRect;
    if (FAILED(DwmGetWindowAttribute(
            hwnd,
            DWMWA_EXTENDED_FRAME_BOUNDS,
            &visibleRect,
            sizeof(visibleRect)))) {
        visibleRect = windowRect;
    }

    if (IsFullscreenLike(hwnd, visibleRect)) {
        return;
    }

    RECT usableRect = {};
    if (!GetUsableRectForWindow(hwnd, &usableRect)) {
        return;
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
        return;
    }

    if (SetWindowPos(
            hwnd,
            nullptr,
            newX,
            newY,
            0,
            0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE)) {
        if (g_logCorrections.load()) {
            wchar_t title[256] = {};
            GetWindowTextW(hwnd, title, ARRAYSIZE(title));
            Wh_Log(L"Centered hwnd=%p title='%s' x=%d y=%d", hwnd, title, newX, newY);
        }
    } else if (g_logCorrections.load()) {
        Wh_Log(L"Center SetWindowPos failed for hwnd=%p error=%lu", hwnd, GetLastError());
    }
}

static void CorrectWindowIfNeeded(HWND hwnd) {
    if (!IsEligibleWindow(hwnd)) {
        return;
    }

    LONG protectedTop = 0;
    if (!GetProtectedTopForWindow(hwnd, &protectedTop)) {
        return;
    }

    RECT windowRect = {};
    if (!GetWindowRect(hwnd, &windowRect)) {
        return;
    }

    RECT visibleRect = windowRect;
    if (FAILED(DwmGetWindowAttribute(
            hwnd,
            DWMWA_EXTENDED_FRAME_BOUNDS,
            &visibleRect,
            sizeof(visibleRect)))) {
        visibleRect = windowRect;
    }

    if (IsFullscreenLike(hwnd, visibleRect)) {
        return;
    }

    if (visibleRect.top >= protectedTop) {
        return;
    }

    if (visibleRect.bottom <= protectedTop) {
        return;
    }

    const LONG deltaY = protectedTop - visibleRect.top;
    const int newY = windowRect.top + deltaY;

    if (SetWindowPos(
            hwnd,
            nullptr,
            windowRect.left,
            newY,
            0,
            0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE)) {
        if (g_logCorrections.load()) {
            wchar_t title[256] = {};
            GetWindowTextW(hwnd, title, ARRAYSIZE(title));

            wchar_t className[128] = {};
            GetClassNameW(hwnd, className, ARRAYSIZE(className));

            Wh_Log(
                L"Corrected hwnd=%p class='%s' title='%s' deltaY=%ld protectedTop=%ld",
                hwnd,
                className,
                title,
                deltaY,
                protectedTop);
        }
    } else if (g_logCorrections.load()) {
        Wh_Log(
            L"SetWindowPos failed for hwnd=%p error=%lu",
            hwnd,
            GetLastError());
    }
}

static void StopRecheckTimer() {
    if (g_recheckTimer) {
        KillTimer(nullptr, g_recheckTimer);
        g_recheckTimer = 0;
    }

    g_recheckHwnd = nullptr;
    g_recheckUntil = 0;
    g_centerUntil = 0;
}

static VOID CALLBACK RecheckTimerProc(
    HWND,
    UINT,
    UINT_PTR,
    DWORD) {

    if (!g_recheckHwnd ||
        !IsWindow(g_recheckHwnd) ||
        GetTickCount64() >= g_recheckUntil) {
        StopRecheckTimer();
        return;
    }

    const ULONGLONG now = GetTickCount64();
    if (g_centerUntil && now < g_centerUntil) {
        CenterWindowIfNeeded(g_recheckHwnd);
    }

    CorrectWindowIfNeeded(g_recheckHwnd);
}

static void StartRecheck(HWND hwnd, bool centerOnAppearance) {
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }

    if (centerOnAppearance) {
        CenterWindowIfNeeded(hwnd);
    }

    CorrectWindowIfNeeded(hwnd);

    DWORD duration = g_recheckDurationMs.load();
    if (!duration) {
        return;
    }

    g_recheckHwnd = hwnd;
    const ULONGLONG now = GetTickCount64();
    g_recheckUntil = now + duration;

    g_centerUntil = centerOnAppearance && g_centerWindowsOnOpen.load()
        ? now + std::min<DWORD>(duration, 700)
        : 0;

    if (g_recheckTimer) {
        KillTimer(nullptr, g_recheckTimer);
        g_recheckTimer = 0;
    }

    g_recheckTimer = SetTimer(
        nullptr,
        0,
        g_recheckIntervalMs.load(),
        RecheckTimerProc);

    if (!g_recheckTimer && g_logCorrections.load()) {
        Wh_Log(L"SetTimer failed: %lu", GetLastError());
    }
}

static BOOL CALLBACK CollectVisibleWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* windows = reinterpret_cast<std::unordered_set<HWND>*>(lParam);

    if (IsEligibleWindow(hwnd)) {
        windows->insert(hwnd);
    }

    return TRUE;
}

static void TakeVisibleWindowSnapshot() {
    std::unordered_set<HWND> current;
    EnumWindows(CollectVisibleWindowsProc, reinterpret_cast<LPARAM>(&current));
    g_visibleWindows.swap(current);
    g_visibleSnapshotInitialized = true;
    g_lastForeground = GetForegroundWindow();
}

static VOID CALLBACK PollTimerProc(HWND, UINT, UINT_PTR, DWORD) {
    std::unordered_set<HWND> current;
    EnumWindows(CollectVisibleWindowsProc, reinterpret_cast<LPARAM>(&current));

    if (!g_visibleSnapshotInitialized) {
        g_visibleWindows.swap(current);
        g_visibleSnapshotInitialized = true;
        g_lastForeground = GetForegroundWindow();
        return;
    }

    for (HWND hwnd : current) {
        if (g_visibleWindows.find(hwnd) == g_visibleWindows.end()) {
            StartRecheck(hwnd, true);
        }
    }

    HWND foreground = GetForegroundWindow();
    if (foreground && foreground != g_lastForeground) {
        StartRecheck(foreground, false);
        g_lastForeground = foreground;
    } else if (foreground) {
        const bool mouseDragging =
            (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ||
            (GetAsyncKeyState(VK_RBUTTON) & 0x8000) ||
            (GetAsyncKeyState(VK_MBUTTON) & 0x8000);

        if (!mouseDragging) {
            CorrectWindowIfNeeded(foreground);
        }
    }

    g_visibleWindows.swap(current);
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

    if (event == EVENT_OBJECT_SHOW ||
        event == EVENT_OBJECT_UNCLOAKED) {
        if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
            return;
        }
    }

    const bool centerOnAppearance =
        event == EVENT_OBJECT_SHOW ||
        event == EVENT_OBJECT_UNCLOAKED ||
        event == EVENT_SYSTEM_MINIMIZEEND;

    StartRecheck(hwnd, centerOnAppearance);
}

static DWORD WINAPI WinEventThreadProc(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    MSG msg = {};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    HWINEVENTHOOK foregroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND,
        EVENT_SYSTEM_FOREGROUND,
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

    HWINEVENTHOOK minimizeEndHook = SetWinEventHook(
        EVENT_SYSTEM_MINIMIZEEND,
        EVENT_SYSTEM_MINIMIZEEND,
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

    if (!foregroundHook || !showHook || !minimizeEndHook || !uncloakedHook) {
        Wh_Log(L"One or more SetWinEventHook calls failed: %lu", GetLastError());
    }

    if (g_threadReadyEvent) {
        SetEvent(g_threadReadyEvent);
    }

    TakeVisibleWindowSnapshot();

    g_pollTimer = SetTimer(nullptr, 0, kPollIntervalMs, PollTimerProc);
    if (!g_pollTimer) {
        Wh_Log(L"Foreground polling timer failed: %lu", GetLastError());
    }

    StartRecheck(GetForegroundWindow(), false);

    BOOL result;
    while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0) {
        if (result == -1) {
            break;
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_pollTimer) {
        KillTimer(nullptr, g_pollTimer);
        g_pollTimer = 0;
    }

    g_visibleWindows.clear();
    g_visibleSnapshotInitialized = false;
    g_lastForeground = nullptr;

    StopRecheckTimer();

    if (uncloakedHook) UnhookWinEvent(uncloakedHook);
    if (minimizeEndHook) UnhookWinEvent(minimizeEndHook);
    if (showHook) UnhookWinEvent(showHook);
    if (foregroundHook) UnhookWinEvent(foregroundHook);

    return 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();

    g_threadReadyEvent =
        CreateEventW(nullptr, TRUE, FALSE, nullptr);

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

    DWORD waitResult =
        WaitForSingleObject(g_threadReadyEvent, 5000);

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
// Windhawk tool mod implementation.
//
// The mod itself runs in a dedicated windhawk.exe process instead of injecting
// into Explorer or every application.
////////////////////////////////////////////////////////////////////////////////

bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId = 0;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
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
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);

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
            reinterpret_cast<IMAGE_DOS_HEADER*>(GetModuleHandleW(nullptr));

        IMAGE_NT_HEADERS* ntHeaders =
            reinterpret_cast<IMAGE_NT_HEADERS*>(
                reinterpret_cast<BYTE*>(dosHeader) + dosHeader->e_lfanew);

        DWORD entryPointRVA =
            ntHeaders->OptionalHeader.AddressOfEntryPoint;

        void* entryPoint =
            reinterpret_cast<BYTE*>(dosHeader) + entryPointRVA;

        Wh_SetFunctionHook(
            entryPoint,
            reinterpret_cast<void*>(EntryPoint_Hook),
            nullptr);

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

    WCHAR currentProcessPath[MAX_PATH] = {};
    DWORD pathLength = GetModuleFileNameW(
        nullptr,
        currentProcessPath,
        ARRAYSIZE(currentProcessPath));

    if (pathLength == 0 || pathLength == ARRAYSIZE(currentProcessPath)) {
        Wh_Log(L"GetModuleFileName failed");
        return;
    }

    WCHAR commandLine[
        MAX_PATH + 2 +
        (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1
    ] = {};

    swprintf_s(
        commandLine,
        L"\"%s\" -tool-mod \"%s\"",
        currentProcessPath,
        WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE,
        LPCWSTR,
        LPWSTR,
        LPSECURITY_ATTRIBUTES,
        LPSECURITY_ATTRIBUTES,
        WINBOOL,
        DWORD,
        LPVOID,
        LPCWSTR,
        LPSTARTUPINFOW,
        LPPROCESS_INFORMATION,
        PHANDLE);

    auto pCreateProcessInternalW =
        reinterpret_cast<CreateProcessInternalW_t>(
            GetProcAddress(kernelModule, "CreateProcessInternalW"));

    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;

    PROCESS_INFORMATION pi = {};

    if (!pCreateProcessInternalW(
            nullptr,
            currentProcessPath,
            commandLine,
            nullptr,
            nullptr,
            FALSE,
            NORMAL_PRIORITY_CLASS,
            nullptr,
            nullptr,
            &si,
            &pi,
            nullptr)) {
        Wh_Log(L"CreateProcess failed: %lu", GetLastError());
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

    if (g_toolModProcessMutex) {
        ReleaseMutex(g_toolModProcessMutex);
        CloseHandle(g_toolModProcessMutex);
        g_toolModProcessMutex = nullptr;
    }

    ExitProcess(0);
}
