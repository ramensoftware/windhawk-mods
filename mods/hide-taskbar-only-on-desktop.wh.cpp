// ==WindhawkMod==
// @id hide-taskbar-only-on-desktop
// @name Hide Taskbar Only on Desktop
// @description Hides the taskbar on desktop while preserving taskbar and Windows shell UI interaction
// @version 1.10.0
// @author Sahil Dashoni
// @github https://github.com/Sahil-Dashoni
// @include windhawk.exe
// @compilerOptions -lshell32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Taskbar Only on Desktop

Hides the Windows taskbar when the desktop is active, while keeping it
visible whenever an application or shell UI is active.

## Features

- Hides the taskbar on the desktop.
- Shows the taskbar when an application becomes active.
- Automatically hides the taskbar after minimizing or closing the last application.
- Reveals the taskbar when the mouse enters the taskbar-sized hover area.
- The hover area follows the taskbar on the cursor's monitor.
- Uses the taskbar's actual rectangle and per-monitor DPI.
- Adds a configurable extra hover margin in millimeters.
- Uses a configurable delay after leaving the hover area.
- The delay is only used after a hover reveal.
- Minimizing or closing the last application hides the taskbar immediately.
- Keeps the taskbar available while interacting with taskbar buttons.
- Supports secondary taskbars on additional monitors.
- Uses independent per-monitor desktop/application state.
- If an application is open on one monitor while another monitor is
  showing the desktop, only the desktop monitor's taskbar is hidden.
- Supports bottom, top, left and right taskbar positions.
- Keeps the taskbar visible for Windows shell flyouts such as Start,
  Search, Notifications and Quick Settings.
- Keeps the taskbar visible while Alt+Tab is open, then uses the same
  configurable hide delay after Alt+Tab closes.
- Minimizing the last application clears any previous hover-reveal delay
  and hides the taskbar immediately on that monitor.
- Runs as a Windhawk tool instead of being injected into Explorer.

## Notes

This mod intentionally differs from native Windows auto-hide:
it hides the taskbar window without changing the desktop work area.

Because the taskbar is controlled by a separate tool process, an unexpected
termination of that process while the taskbar is hidden can leave the taskbar
hidden until Explorer is restarted. Normal disable/unload paths restore it.

For predictable behavior, disable Windows' own "Automatically hide the
taskbar" option while this mod is enabled.

This mod uses only foreground/minimize Windows events for application state
changes and only uses a short timer while cursor hover handling is actually
needed. It deliberately avoids system-wide object show/hide/destroy hooks.

On multi-monitor systems, each monitor's taskbar is evaluated independently.
A normal visible application intersecting a monitor keeps that monitor's
taskbar visible; a monitor with only the desktop can hide its taskbar.

Windows shell flyouts are treated as temporary shell interaction and keep
the taskbar visible while they are open. Alt+Tab is likewise treated as an
active shell interaction.

This mod was created with AI assistance.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- extraHoverMarginMm: 5
  $name: Extra hover margin (mm)
  $description: >-
    Adds extra space around the taskbar reveal area. The taskbar's own
    dimensions are automatically included. Default is 5 mm.

- autoHideDelayMs: 700
  $name: Auto-hide delay (ms)
  $description: >-
    How long the taskbar remains visible after a hover reveal or after
    Alt+Tab closes while no application is active on that monitor.
    Other hides, such as minimizing the last application, are immediate.

- hideSecondaryTaskbars: true
  $name: Hide secondary taskbars
  $description: >-
    Also hide taskbars on additional monitors. When disabled, secondary
    taskbars are always restored.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <atomic>
#include <stdio.h>

struct Settings {
    std::atomic<int> extraHoverMarginMm{5};
    std::atomic<DWORD> autoHideDelayMs{700};
    std::atomic<bool> hideSecondaryTaskbars{true};
};

Settings g_settings;

HANDLE g_hThread = nullptr;
DWORD g_threadId = 0;
HANDLE g_hThreadReadyEvent = nullptr;

constexpr UINT WM_APP_REFRESH_STATE = WM_APP + 1;
constexpr UINT WM_APP_STOP_THREAD = WM_APP + 2;
constexpr UINT kHoverTimerIntervalMs = 200;

UINT_PTR g_hoverTimerId = 0;

std::atomic<bool> g_refreshPosted{false};
std::atomic<bool> g_immediateHidePending{false};

constexpr size_t kMaxTaskbars = 16;
constexpr size_t kMaxWinEventHooks = 2;

struct TaskbarState {
    HWND hwnd = nullptr;
    HMONITOR monitor = nullptr;
    RECT monitorRect = {};
    bool isSecondary = false;
    bool hasApplication = false;
    bool taskbarIsForeground = false;
    bool shellUiForeground = false;
    bool shownDueToHover = false;
    bool shownDueToAltTab = false;
    ULONGLONG hideDeadline = 0;
};

TaskbarState g_taskbars[kMaxTaskbars] = {};
size_t g_taskbarCount = 0;

HWINEVENTHOOK g_hWinEventHooks[kMaxWinEventHooks] = {};


// Hidden window used to receive setting/display notifications without
// installing broad system-wide EVENT_OBJECT_* hooks.
HWND g_hSystemMessageWindow = nullptr;
const wchar_t kSystemMessageWindowClass[] =
    L"HideTaskbarOnlyOnDesktop.SystemMessageWindow";

// ============================================================
// Utility
// ============================================================

bool IsDesktopWindow(HWND hwnd) {
    if (!hwnd) {
        return false;
    }

    WCHAR className[256] = {};

    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0) {
        return false;
    }

    if (wcscmp(className, L"Progman") == 0) {
        return true;
    }

    if (wcscmp(className, L"WorkerW") == 0) {
        return FindWindowExW(
            hwnd,
            nullptr,
            L"SHELLDLL_DefView",
            nullptr
        ) != nullptr;
    }

    return false;
}


bool IsShellChromeClass(const WCHAR* className) {
    if (!className) {
        return false;
    }

    return
        wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0 ||
        wcscmp(className, L"ApplicationFrameWindow") == 0 ||
        wcscmp(className, L"Xaml_WindowedPopupClass") == 0 ||
        wcscmp(className, L"ControlCenterWindow") == 0 ||
        wcscmp(className, L"TopLevelWindowForOverflowXamlIsland") == 0 ||
        wcscmp(className, L"NotifyIconOverflowWindow") == 0;
}


bool IsTaskbarWindow(HWND hwnd) {
    if (!hwnd) {
        return false;
    }

    WCHAR className[256] = {};

    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0) {
        return false;
    }

    return wcscmp(className, L"Shell_TrayWnd") == 0 ||
           wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
}


// ============================================================
// Window detection
// ============================================================

void DiscoverTaskbars();
bool IsAltTabWindowClass(const WCHAR* className);


HWND FindPrimaryTaskbar();
bool IsTaskbarActuallyHidden(HWND hwnd);
bool IsShellUiClass(const WCHAR* className);

struct AppMonitorScan {
    TaskbarState* taskbars;
    size_t count;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    AppMonitorScan* scan =
        reinterpret_cast<AppMonitorScan*>(lParam);

    if (!scan || !scan->taskbars) {
        return TRUE;
    }

    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) {
        return TRUE;
    }

    if (IsTaskbarWindow(hwnd)) {
        return TRUE;
    }

    WCHAR className[256] = {};

    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0) {
        return TRUE;
    }

    if (
        IsShellChromeClass(className) ||
        IsShellUiClass(className)
    ) {
        return TRUE;
    }

    LONG_PTR exStyle =
        GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

    if (exStyle & WS_EX_TOOLWINDOW) {
        return TRUE;
    }

    BOOL cloaked = FALSE;

    if (
        SUCCEEDED(
            DwmGetWindowAttribute(
                hwnd,
                DWMWA_CLOAKED,
                &cloaked,
                sizeof(cloaked)
            )
        ) &&
        cloaked
    ) {
        return TRUE;
    }

    RECT rect = {};

    if (!GetWindowRect(hwnd, &rect)) {
        return TRUE;
    }

    if (
        rect.right <= rect.left ||
        rect.bottom <= rect.top
    ) {
        return TRUE;
    }

    if (
        GetWindow(hwnd, GW_OWNER) != nullptr &&
        !(exStyle & WS_EX_APPWINDOW)
    ) {
        return TRUE;
    }

    for (size_t i = 0; i < scan->count; ++i) {
        RECT intersection = {};

        if (
            IntersectRect(
                &intersection,
                &scan->taskbars[i].monitorRect,
                &rect
            )
        ) {
            scan->taskbars[i].hasApplication = true;
        }
    }

    return TRUE;
}


void ScanApplicationWindows() {
    for (size_t i = 0; i < g_taskbarCount; ++i) {
        g_taskbars[i].hasApplication = false;
    }

    if (!g_taskbarCount) {
        return;
    }

    AppMonitorScan scan{
        g_taskbars,
        g_taskbarCount
    };

    EnumWindows(
        EnumWindowsProc,
        reinterpret_cast<LPARAM>(&scan)
    );
}


// ============================================================
// Foreground / taskbar interaction
// ============================================================

bool IsTaskbarForegroundAndUnderCursor(HWND taskbar) {
    if (!IsTaskbarWindow(taskbar)) {
        return false;
    }

    if (GetForegroundWindow() != taskbar) {
        return false;
    }

    POINT pt = {};

    if (!GetCursorPos(&pt)) {
        return false;
    }

    RECT rect = {};

    if (!GetWindowRect(taskbar, &rect)) {
        return false;
    }

    return PtInRect(&rect, pt) != FALSE;
}


bool GetWindowProcessName(HWND hwnd, WCHAR* name, size_t nameCount) {
    if (!hwnd || !name || nameCount == 0) {
        return false;
    }

    DWORD processId = 0;

    if (!GetWindowThreadProcessId(hwnd, &processId) || !processId) {
        return false;
    }

    HANDLE process = OpenProcess(
        PROCESS_QUERY_LIMITED_INFORMATION,
        FALSE,
        processId
    );

    if (!process) {
        return false;
    }

    DWORD size = static_cast<DWORD>(nameCount);

    BOOL result = QueryFullProcessImageNameW(
        process,
        0,
        name,
        &size
    );

    CloseHandle(process);

    if (!result || size == 0) {
        return false;
    }

    const WCHAR* baseName = wcsrchr(name, L'\\');

    if (baseName) {
        baseName++;
        size_t length = wcslen(baseName);

        if (length + 1 <= nameCount) {
            memmove(name, baseName, (length + 1) * sizeof(WCHAR));
        }
    }

    return true;
}


bool IsKnownShellUiProcess(const WCHAR* processName) {
    if (!processName) {
        return false;
    }

    return
        _wcsicmp(processName, L"explorer.exe") == 0 ||
        _wcsicmp(processName, L"StartMenuExperienceHost.exe") == 0 ||
        _wcsicmp(processName, L"ShellExperienceHost.exe") == 0 ||
        _wcsicmp(processName, L"ShellHost.exe") == 0 ||
        _wcsicmp(processName, L"SearchHost.exe") == 0 ||
        _wcsicmp(processName, L"SearchApp.exe") == 0 ||
        _wcsicmp(processName, L"TextInputHost.exe") == 0;
}


bool IsShellUiWindow(HWND hwnd) {
    if (!hwnd || IsTaskbarWindow(hwnd)) {
        return false;
    }

    WCHAR processName[MAX_PATH] = {};

    if (!GetWindowProcessName(
            hwnd,
            processName,
            ARRAYSIZE(processName)
        )) {
        return false;
    }

    return IsKnownShellUiProcess(processName);
}


bool IsShellUiClass(const WCHAR* className) {
    if (!className) {
        return false;
    }

    return
        wcscmp(className, L"ControlCenterWindow") == 0 ||
        wcscmp(className, L"Xaml_WindowedPopupClass") == 0 ||
        wcscmp(className, L"TopLevelWindowForOverflowXamlIsland") == 0 ||
        wcscmp(className, L"NotifyIconOverflowWindow") == 0 ||
        wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0;
}


bool IsAltTabWindowClass(const WCHAR* className) {
    if (!className) {
        return false;
    }

    return
        wcscmp(className, L"#32771") == 0 ||
        wcscmp(className, L"XamlExplorerHostIslandWindow") == 0;
}


bool IsVisibleShellFlyoutForMonitor(
    HMONITOR monitor,
    HWND hwnd
) {
    if (!monitor || !hwnd || !IsWindowVisible(hwnd)) {
        return false;
    }

    if (IsTaskbarWindow(hwnd)) {
        return false;
    }

    WCHAR className[256] = {};

    if (
        GetClassNameW(
            hwnd,
            className,
            ARRAYSIZE(className)
        ) == 0
    ) {
        return false;
    }

    if (!IsShellUiClass(className)) {
        return false;
    }

    /*
     * XAML popup classes are used by ordinary applications too.
     * Require a known Windows shell process before treating one as a
     * taskbar-preserving shell flyout.
     */
    WCHAR processName[MAX_PATH] = {};

    if (
        !GetWindowProcessName(
            hwnd,
            processName,
            ARRAYSIZE(processName)
        )
    ) {
        return false;
    }

    bool knownShellProcess =
        _wcsicmp(processName, L"explorer.exe") == 0 ||
        IsKnownShellUiProcess(processName);

    if (!knownShellProcess) {
        return false;
    }

    BOOL cloaked = FALSE;

    if (
        SUCCEEDED(
            DwmGetWindowAttribute(
                hwnd,
                DWMWA_CLOAKED,
                &cloaked,
                sizeof(cloaked)
            )
        ) &&
        cloaked
    ) {
        return false;
    }

    RECT windowRect = {};

    if (!GetWindowRect(hwnd, &windowRect)) {
        return false;
    }

    if (
        windowRect.right <= windowRect.left ||
        windowRect.bottom <= windowRect.top
    ) {
        return false;
    }

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);

    if (!GetMonitorInfoW(monitor, &mi)) {
        return false;
    }

    RECT intersection = {};

    return IntersectRect(
        &intersection,
        &mi.rcMonitor,
        &windowRect
    ) != FALSE;
}


struct ShellFlyoutScan {
    HMONITOR monitor;
    bool found;
};


BOOL CALLBACK EnumShellFlyoutProc(
    HWND hwnd,
    LPARAM lParam
) {
    ShellFlyoutScan* scan =
        reinterpret_cast<ShellFlyoutScan*>(lParam);

    if (!scan || scan->found) {
        return FALSE;
    }

    if (
        IsVisibleShellFlyoutForMonitor(
            scan->monitor,
            hwnd
        )
    ) {
        scan->found = true;
        return FALSE;
    }

    return TRUE;
}


bool HasVisibleShellFlyout(HMONITOR monitor) {
    if (!monitor) {
        return false;
    }

    ShellFlyoutScan scan = {};
    scan.monitor = monitor;

    EnumWindows(
        EnumShellFlyoutProc,
        reinterpret_cast<LPARAM>(&scan)
    );

    return scan.found;
}


BOOL CALLBACK EnumAltTabWindowProc(
    HWND hwnd,
    LPARAM lParam
) {
    bool* found =
        reinterpret_cast<bool*>(lParam);

    if (!found || !IsWindowVisible(hwnd)) {
        return TRUE;
    }

    WCHAR className[128] = {};

    if (
        GetClassNameW(
            hwnd,
            className,
            ARRAYSIZE(className)
        ) == 0 ||
        !IsAltTabWindowClass(className)
    ) {
        return TRUE;
    }

    /*
     * The modern Windows 11 switcher uses
     * XamlExplorerHostIslandWindow. Restrict it to the shell processes
     * that actually host the task switcher.
     */
    if (
        wcscmp(
            className,
            L"XamlExplorerHostIslandWindow"
        ) == 0
    ) {
        WCHAR processName[MAX_PATH] = {};

        if (
            !GetWindowProcessName(
                hwnd,
                processName,
                ARRAYSIZE(processName)
            )
        ) {
            return TRUE;
        }

        if (
            _wcsicmp(processName, L"explorer.exe") != 0 &&
            _wcsicmp(processName, L"ShellExperienceHost.exe") != 0
        ) {
            return TRUE;
        }
    }

    BOOL cloaked = FALSE;

    if (
        SUCCEEDED(
            DwmGetWindowAttribute(
                hwnd,
                DWMWA_CLOAKED,
                &cloaked,
                sizeof(cloaked)
            )
        ) &&
        cloaked
    ) {
        return TRUE;
    }

    *found = true;
    return FALSE;
}


bool IsAltTabActive() {
    /*
     * Modern Windows versions do not always expose the task switcher
     * through the legacy #32771 window class. Detect the actual Alt+Tab
     * shortcut while it is being held, and keep the legacy class check
     * as a fallback.
     */
    bool altTabWindow = false;

    EnumWindows(
        EnumAltTabWindowProc,
        reinterpret_cast<LPARAM>(&altTabWindow)
    );

    if (altTabWindow) {
        return true;
    }

    bool altDown =
        (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

    bool tabDown =
        (GetAsyncKeyState(VK_TAB) & 0x8000) != 0;

    return altDown && tabDown;
}


bool IsShellUiForegroundOnMonitor(
    HMONITOR monitor,
    HWND foreground
) {
    if (!monitor || !foreground) {
        return false;
    }

    WCHAR className[256] = {};

    if (
        GetClassNameW(
            foreground,
            className,
            ARRAYSIZE(className)
        ) == 0
    ) {
        return false;
    }

    if (IsAltTabWindowClass(className)) {
        return true;
    }

    if (!IsShellUiClass(className)) {
        return false;
    }

    if (!IsShellUiWindow(foreground)) {
        return false;
    }

    RECT rect = {};

    if (!GetWindowRect(foreground, &rect)) {
        return false;
    }

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);

    if (!GetMonitorInfoW(monitor, &mi)) {
        return false;
    }

    RECT intersection = {};

    return IntersectRect(
        &intersection,
        &mi.rcMonitor,
        &rect
    ) != FALSE;
}


void RefreshTaskbarForegroundState() {
    HWND foreground = GetForegroundWindow();

    for (size_t i = 0; i < g_taskbarCount; ++i) {
        TaskbarState& taskbar = g_taskbars[i];

        bool foregroundIsTaskbar =
            foreground == taskbar.hwnd;

        bool cursorOverTaskbar =
            IsTaskbarForegroundAndUnderCursor(
                taskbar.hwnd
            );

        taskbar.taskbarIsForeground =
            foregroundIsTaskbar;

        taskbar.shellUiForeground =
            IsShellUiForegroundOnMonitor(
                taskbar.monitor,
                foreground
            );

        if (cursorOverTaskbar) {
            taskbar.shownDueToHover = true;
            taskbar.shownDueToAltTab = false;
            taskbar.hideDeadline = 0;
        }
    }
}

void RefreshPerMonitorState() {
    /*
     * Application visibility is determined independently for each monitor.
     * A window spanning multiple monitors keeps taskbars on all intersected
     * monitors visible.
     */
    DiscoverTaskbars();
    ScanApplicationWindows();
    RefreshTaskbarForegroundState();
}


// ============================================================
// Taskbar discovery
// ============================================================

bool IsSecondaryTaskbar(HWND hwnd) {
    if (!hwnd) {
        return false;
    }

    WCHAR className[256] = {};

    if (
        GetClassNameW(
            hwnd,
            className,
            ARRAYSIZE(className)
        ) == 0
    ) {
        return false;
    }

    return wcscmp(
        className,
        L"Shell_SecondaryTrayWnd"
    ) == 0;
}


void DiscoverTaskbars() {
    TaskbarState oldStates[kMaxTaskbars] = {};
    size_t oldCount = g_taskbarCount;

    for (size_t i = 0; i < oldCount; ++i) {
        oldStates[i] = g_taskbars[i];
    }

    g_taskbarCount = 0;

    auto addTaskbar = [&](HWND hwnd) {
        if (!hwnd || g_taskbarCount >= kMaxTaskbars) {
            return;
        }

        for (size_t i = 0; i < g_taskbarCount; ++i) {
            if (g_taskbars[i].hwnd == hwnd) {
                return;
            }
        }

        HMONITOR monitor =
            MonitorFromWindow(
                hwnd,
                MONITOR_DEFAULTTONULL
            );

        if (!monitor) {
            return;
        }

        MONITORINFO mi = {};
        mi.cbSize = sizeof(mi);

        if (!GetMonitorInfoW(monitor, &mi)) {
            return;
        }

        TaskbarState state = {};
        state.hwnd = hwnd;
        state.monitor = monitor;
        state.monitorRect = mi.rcMonitor;
        state.isSecondary = IsSecondaryTaskbar(hwnd);

        for (size_t i = 0; i < oldCount; ++i) {
            if (oldStates[i].hwnd == hwnd) {
                state.shownDueToHover =
                    oldStates[i].shownDueToHover;
                state.shownDueToAltTab =
                    oldStates[i].shownDueToAltTab;
                state.hideDeadline =
                    oldStates[i].hideDeadline;
                break;
            }
        }

        g_taskbars[g_taskbarCount++] = state;
    };

    addTaskbar(
        FindWindowW(
            L"Shell_TrayWnd",
            nullptr
        )
    );

    HWND secondary = nullptr;

    while (
        (secondary = FindWindowExW(
            nullptr,
            secondary,
            L"Shell_SecondaryTrayWnd",
            nullptr
        )) != nullptr
    ) {
        addTaskbar(secondary);
    }
}


// ============================================================
// Taskbar visibility
// ============================================================

HWND FindPrimaryTaskbar() {
    return FindWindowW(L"Shell_TrayWnd", nullptr);
}

bool IsTaskbarActuallyHidden(HWND hwnd) {
    return hwnd != nullptr && IsWindowVisible(hwnd) == FALSE;
}

void SetWindowVisibilityIfNeeded(HWND hwnd, bool show) {
    if (!hwnd) {
        return;
    }

    bool visible = IsWindowVisible(hwnd) != FALSE;

    if (visible != show) {
        ShowWindowAsync(
            hwnd,
            show ? SW_SHOWNA : SW_HIDE
        );
    }
}


void SetSecondaryTaskbarsVisibility(bool show) {
    HWND secondary = nullptr;

    while (
        (secondary = FindWindowExW(
            nullptr,
            secondary,
            L"Shell_SecondaryTrayWnd",
            nullptr
        )) != nullptr
    ) {
        SetWindowVisibilityIfNeeded(secondary, show);
    }
}


void SetTaskbarVisibility(bool show) {
    HWND primary = FindPrimaryTaskbar();

    if (primary) {
        SetWindowVisibilityIfNeeded(primary, show);
    }

    bool hideSecondary =
        g_settings.hideSecondaryTaskbars.load(
            std::memory_order_relaxed
        );

    // When the setting is disabled, secondary taskbars must be restored
    // even while the primary taskbar is hidden on the desktop.
    if (show || hideSecondary) {
        SetSecondaryTaskbarsVisibility(show);
    } else {
        SetSecondaryTaskbarsVisibility(true);
    }
}


bool IsPrimaryTaskbarHidden() {
    HWND primary = FindPrimaryTaskbar();

    if (!primary) {
        return false;
    }

    return IsWindowVisible(primary) == FALSE;
}


// ============================================================
// Hover zone
// ============================================================

int MillimetersToPixels(int mm, UINT dpi) {
    if (mm <= 0 || dpi == 0) {
        return 0;
    }

    // px = mm * dpi / 25.4
    // Using integer arithmetic:
    // px = mm * 10 * dpi / 254
    return MulDiv(
        mm * 10,
        static_cast<int>(dpi),
        254
    );
}


bool IsCursorInTaskbarHoverZone(
    TaskbarState& taskbar
) {
    if (!taskbar.hwnd) {
        return false;
    }

    if (
        taskbar.isSecondary &&
        !g_settings.hideSecondaryTaskbars.load(
            std::memory_order_relaxed
        )
    ) {
        return false;
    }

    POINT pt = {};

    if (!GetCursorPos(&pt)) {
        return false;
    }

    RECT taskbarRect = {};

    if (!GetWindowRect(taskbar.hwnd, &taskbarRect)) {
        return false;
    }

    if (
        taskbarRect.right <= taskbarRect.left ||
        taskbarRect.bottom <= taskbarRect.top
    ) {
        return false;
    }

    UINT dpi = GetDpiForWindow(taskbar.hwnd);

    if (!dpi) {
        dpi = 96;
    }

    int marginPx =
        MillimetersToPixels(
            g_settings.extraHoverMarginMm.load(
                std::memory_order_relaxed
            ),
            dpi
        );

    int monitorWidth =
        taskbar.monitorRect.right -
        taskbar.monitorRect.left;

    int monitorHeight =
        taskbar.monitorRect.bottom -
        taskbar.monitorRect.top;

    int maxMargin =
        monitorWidth < monitorHeight
            ? monitorWidth / 2
            : monitorHeight / 2;

    if (marginPx > maxMargin) {
        marginPx = maxMargin;
    }

    RECT hoverRect = taskbarRect;

    InflateRect(
        &hoverRect,
        marginPx,
        marginPx
    );

    return PtInRect(
        &hoverRect,
        pt
    ) != FALSE;
}


// ============================================================
// Timer management
// ============================================================

void StopHoverTimer() {
    if (g_hoverTimerId) {
        KillTimer(
            nullptr,
            g_hoverTimerId
        );

        g_hoverTimerId = 0;
    }
}


void EnsureHoverTimer(bool needed) {
    if (!needed) {
        StopHoverTimer();
        return;
    }

    if (g_hoverTimerId) {
        return;
    }

    g_hoverTimerId =
        SetTimer(
            nullptr,
            0,
            kHoverTimerIntervalMs,
            nullptr
        );

    if (!g_hoverTimerId) {
        Wh_Log(
            L"SetTimer failed: %lu",
            GetLastError()
        );
    }
}


// ============================================================
// State machine
// ============================================================

void UpdateTaskbarState() {
    if (!g_taskbarCount) {
        StopHoverTimer();
        return;
    }

    ULONGLONG now = GetTickCount64();

    DWORD delay =
        g_settings.autoHideDelayMs.load(
            std::memory_order_relaxed
        );

    for (size_t i = 0; i < g_taskbarCount; ++i) {
        TaskbarState& taskbar =
            g_taskbars[i];

        bool controlled =
            !taskbar.isSecondary ||
            g_settings.hideSecondaryTaskbars.load(
                std::memory_order_relaxed
            );

        if (!controlled) {
            taskbar.shownDueToHover = false;
            taskbar.hideDeadline = 0;

            SetWindowVisibilityIfNeeded(
                taskbar.hwnd,
                true
            );

            continue;
        }

        bool hovering =
            IsCursorInTaskbarHoverZone(taskbar);

        bool shellFlyoutOpen =
            taskbar.shellUiForeground ||
            HasVisibleShellFlyout(taskbar.monitor);

        bool altTabActive =
            IsAltTabActive();

        /*
         * If a normal application is present on this monitor, that
         * monitor's taskbar remains visible even when another monitor
         * is on the desktop.
         */
        if (taskbar.hasApplication) {
            taskbar.shownDueToHover = false;
            taskbar.hideDeadline = 0;

            SetWindowVisibilityIfNeeded(
                taskbar.hwnd,
                true
            );

            continue;
        }

        /*
         * Windows shell flyouts (Start, Search, Notifications, Quick
         * Settings, tray overflow, etc.) and Alt+Tab must keep the
         * taskbar visible while they are being used.
         */
        if (altTabActive) {
            /*
             * Alt+Tab is an explicit shell interaction. Keep the taskbar
             * visible while the switcher is open and remember that this
             * reveal came from Alt+Tab so the normal hide delay is applied
             * after Alt+Tab closes.
             */
            taskbar.shownDueToAltTab = true;
            taskbar.hideDeadline = 0;

            SetWindowVisibilityIfNeeded(
                taskbar.hwnd,
                true
            );

            continue;
        }

        if (shellFlyoutOpen) {
            taskbar.hideDeadline = 0;

            SetWindowVisibilityIfNeeded(
                taskbar.hwnd,
                true
            );

            continue;
        }

        /*
         * Hovering the taskbar reveals it immediately.
         */
        if (hovering) {
            taskbar.shownDueToHover = true;
            taskbar.shownDueToAltTab = false;
            taskbar.hideDeadline = 0;

            SetWindowVisibilityIfNeeded(
                taskbar.hwnd,
                true
            );

            continue;
        }

        /*
         * A taskbar click/reveal is tracked by shownDueToHover. Once the
         * cursor leaves the taskbar, the configured delay below is used.
         * Do not use Shell_TrayWnd being foreground as a permanent
         * "keep visible" condition, because Explorer may retain that
         * foreground window after the cursor leaves.
         *
         * Keyboard taskbar navigation such as Win+T is handled by the
         * foreground check only while the actual taskbar is the focused
         * window and the shell remains active; the hover state is still
         * what controls mouse-leave hiding.
         */

        /*
         * Apply the delay only after a hover/click reveal.
         */
        if (
            taskbar.taskbarIsForeground &&
            !taskbar.shownDueToHover
        ) {
            SetWindowVisibilityIfNeeded(
                taskbar.hwnd,
                true
            );

            continue;
        }

        if (taskbar.shownDueToAltTab) {
            /*
             * Alt+Tab has just closed. Apply the exact same configured delay
             * used by the hover reveal instead of hiding immediately.
             */
            if (!taskbar.hideDeadline) {
                taskbar.hideDeadline =
                    now +
                    static_cast<ULONGLONG>(delay);
            } else if (now >= taskbar.hideDeadline) {
                SetWindowVisibilityIfNeeded(
                    taskbar.hwnd,
                    false
                );

                taskbar.hideDeadline = 0;
                taskbar.shownDueToAltTab = false;
            }

            continue;
        }

        if (taskbar.shownDueToHover) {
            if (!taskbar.hideDeadline) {
                taskbar.hideDeadline =
                    now +
                    static_cast<ULONGLONG>(delay);
            } else if (now >= taskbar.hideDeadline) {
                SetWindowVisibilityIfNeeded(
                    taskbar.hwnd,
                    false
                );

                taskbar.hideDeadline = 0;
                taskbar.shownDueToHover = false;
            }

            continue;
        }

        /*
         * No application and no reveal: hide this monitor's taskbar
         * immediately.
         */
        SetWindowVisibilityIfNeeded(
            taskbar.hwnd,
            false
        );

        taskbar.hideDeadline = 0;
    }

    bool needTimer = false;

    for (size_t i = 0; i < g_taskbarCount; ++i) {
        TaskbarState& taskbar =
            g_taskbars[i];

        bool controlled =
            !taskbar.isSecondary ||
            g_settings.hideSecondaryTaskbars.load(
                std::memory_order_relaxed
            );

        if (!controlled) {
            continue;
        }

        if (
            taskbar.hasApplication == false ||
            taskbar.taskbarIsForeground ||
            taskbar.shellUiForeground ||
            taskbar.shownDueToHover ||
            taskbar.shownDueToAltTab ||
            IsTaskbarActuallyHidden(taskbar.hwnd)
        ) {
            needTimer = true;
            break;
        }
    }

    EnsureHoverTimer(needTimer);
}


// ============================================================
// Worker refresh message
// ============================================================

void RequestStateRefresh() {
    if (!g_threadId) {
        return;
    }

    bool expected = false;

    if (
        !g_refreshPosted.compare_exchange_strong(
            expected,
            true,
            std::memory_order_acq_rel,
            std::memory_order_relaxed
        )
    ) {
        return;
    }

    if (
        !PostThreadMessageW(
            g_threadId,
            WM_APP_REFRESH_STATE,
            0,
            0
        )
    ) {
        g_refreshPosted.store(
            false,
            std::memory_order_release
        );
    }
}


// ============================================================
// System notification window
// ============================================================

LRESULT CALLBACK SystemMessageWindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
) {
    UNREFERENCED_PARAMETER(hwnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);

    switch (message) {
    case WM_SETTINGCHANGE:
        /*
         * Native taskbar auto-hide can be changed from Windows Settings.
         * Re-query it only when this targeted system notification arrives.
         */
        RefreshPerMonitorState();
        UpdateTaskbarState();
        return 0;

    case WM_DISPLAYCHANGE:
    case WM_DPICHANGED:
        /*
         * Monitor layout or DPI can change taskbar geometry.
         */
        RefreshPerMonitorState();
        UpdateTaskbarState();
        return 0;

    case WM_NCDESTROY:
        if (g_hSystemMessageWindow == hwnd) {
            g_hSystemMessageWindow = nullptr;
        }
        break;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}


bool CreateSystemMessageWindow() {
    WNDCLASSW wc = {};
    wc.lpfnWndProc = SystemMessageWindowProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kSystemMessageWindowClass;

    if (!RegisterClassW(&wc)) {
        DWORD error = GetLastError();

        if (error != ERROR_CLASS_ALREADY_EXISTS) {
            Wh_Log(
                L"RegisterClassW failed: %lu",
                error
            );
            return false;
        }
    }

    g_hSystemMessageWindow =
        CreateWindowExW(
            0,
            kSystemMessageWindowClass,
            L"Hide Taskbar Only on Desktop",
            WS_OVERLAPPED,
            0,
            0,
            0,
            0,
            nullptr,
            nullptr,
            GetModuleHandleW(nullptr),
            nullptr
        );

    if (!g_hSystemMessageWindow) {
        Wh_Log(
            L"CreateWindowExW failed: %lu",
            GetLastError()
        );
        return false;
    }

    return true;
}


void DestroySystemMessageWindow() {
    if (g_hSystemMessageWindow) {
        DestroyWindow(g_hSystemMessageWindow);
        g_hSystemMessageWindow = nullptr;
    }
}


// ============================================================
// WinEvent hook
// ============================================================

bool IsRelevantWinEvent(DWORD event) {
    return
        event == EVENT_SYSTEM_FOREGROUND ||
        event == EVENT_SYSTEM_MINIMIZESTART ||
        event == EVENT_SYSTEM_MINIMIZEEND;
}


void CALLBACK WinEventProc(
    HWINEVENTHOOK,
    DWORD event,
    HWND,
    LONG idObject,
    LONG idChild,
    DWORD,
    DWORD
) {
    if (
        idObject != OBJID_WINDOW ||
        idChild != CHILDID_SELF ||
        !IsRelevantWinEvent(event)
    ) {
        return;
    }

    /*
     * Foreground changes are the main state transition we care about.
     * Minimize start/end are needed for the last-application case.
     *
     * We deliberately do not hook EVENT_OBJECT_SHOW/HIDE/DESTROY here.
     * Those events are system-wide and can fire for menus, tooltips,
     * controls and other transient windows. Avoiding them prevents a
     * synchronous SHAppBarMessage call on every such event.
     */
    if (
        event == EVENT_SYSTEM_MINIMIZESTART ||
        event == EVENT_SYSTEM_MINIMIZEEND
    ) {
        g_immediateHidePending.store(
            true,
            std::memory_order_release
        );
    }

    RequestStateRefresh();
}


// ============================================================
// Worker thread
// ============================================================

bool InstallWinEventHookFor(
    DWORD eventMin,
    DWORD eventMax,
    size_t index
) {
    if (index >= kMaxWinEventHooks) {
        return false;
    }

    g_hWinEventHooks[index] =
        SetWinEventHook(
            eventMin,
            eventMax,
            nullptr,
            WinEventProc,
            0,
            0,
            WINEVENT_OUTOFCONTEXT
        );

    if (!g_hWinEventHooks[index]) {
        Wh_Log(
            L"SetWinEventHook(%lu, %lu) failed: %lu",
            eventMin,
            eventMax,
            GetLastError()
        );

        return false;
    }

    return true;
}


void UninstallWinEventHooks() {
    for (size_t i = 0; i < kMaxWinEventHooks; i++) {
        if (g_hWinEventHooks[i]) {
            UnhookWinEvent(
                g_hWinEventHooks[i]
            );

            g_hWinEventHooks[i] = nullptr;
        }
    }
}


DWORD WINAPI HookThread(LPVOID) {
    /*
     * Use real physical screen coordinates for mixed-DPI monitors.
     */
    SetThreadDpiAwarenessContext(
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
    );

    /*
     * Force creation of this thread's USER message queue before
     * signalling readiness. This makes PostThreadMessageW safe.
     */
    MSG initialMessage = {};

    PeekMessageW(
        &initialMessage,
        nullptr,
        WM_USER,
        WM_USER,
        PM_NOREMOVE
    );

    if (g_hThreadReadyEvent) {
        SetEvent(g_hThreadReadyEvent);
    }

    CreateSystemMessageWindow();

    /*
     * Only hook events that represent meaningful application-state
     * transitions. In particular, do not hook EVENT_OBJECT_SHOW/HIDE/
     * DESTROY because those are generated system-wide for large numbers
     * of transient windows and controls.
     */
    bool hookOk = true;

    hookOk &= InstallWinEventHookFor(
        EVENT_SYSTEM_FOREGROUND,
        EVENT_SYSTEM_FOREGROUND,
        0
    );

    hookOk &= InstallWinEventHookFor(
        EVENT_SYSTEM_MINIMIZESTART,
        EVENT_SYSTEM_MINIMIZEEND,
        1
    );

    if (!hookOk) {
        Wh_Log(
            L"One or more WinEvent hooks failed"
        );
    }

    /*
     * Apply the correct initial state after the worker and hooks exist.
     */
        RefreshPerMonitorState();
    UpdateTaskbarState();

    MSG msg = {};

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_APP_STOP_THREAD) {
            PostQuitMessage(0);
            continue;
        }

        if (msg.message == WM_APP_REFRESH_STATE) {
            g_refreshPosted.store(
                false,
                std::memory_order_release
            );

            bool immediateHide =
                g_immediateHidePending.exchange(
                    false,
                    std::memory_order_acq_rel
                );

            RefreshPerMonitorState();

            if (immediateHide) {
                for (size_t i = 0; i < g_taskbarCount; ++i) {
                    if (!g_taskbars[i].hasApplication) {
                        /*
                         * A minimize/restore event is an application-state
                         * transition, not a taskbar hover. Do not let an
                         * earlier hover reveal delay this hide.
                         */
                        g_taskbars[i].shownDueToHover = false;
                        g_taskbars[i].shownDueToAltTab = false;
                        g_taskbars[i].hideDeadline = 0;

                        /*
                         * If Explorer reports Shell_TrayWnd as foreground
                         * during the transition, it must not keep the
                         * desktop taskbar visible.
                         */
                        g_taskbars[i].taskbarIsForeground = false;
                    }
                }
            }

            UpdateTaskbarState();
            continue;
        }

        if (
            msg.message == WM_TIMER &&
            msg.wParam == g_hoverTimerId
        ) {
            /*
             * The timer exists only while hover handling is needed.
             * Cursor movement is the one piece of state that Windows
             * does not provide as a suitable global WinEvent.
             */
            RefreshPerMonitorState();
            UpdateTaskbarState();

            continue;
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    StopHoverTimer();

    UninstallWinEventHooks();

    /*
     * Always restore the user's visible taskbars when the tool exits.
     */
    SetTaskbarVisibility(true);
    DestroySystemMessageWindow();

    g_refreshPosted.store(
        false,
        std::memory_order_release
    );

    g_immediateHidePending.store(
        false,
        std::memory_order_release
    );

    return 0;
}


// ============================================================
// Settings
// ============================================================

void LoadSettings() {
    int margin =
        Wh_GetIntSetting(
            L"extraHoverMarginMm"
        );

    int delay =
        Wh_GetIntSetting(
            L"autoHideDelayMs"
        );

    if (delay < 0) {
        delay = 0;
    } else if (delay > 60000) {
        delay = 60000;
    }

    bool secondary =
        Wh_GetIntSetting(
            L"hideSecondaryTaskbars"
        ) != 0;

    if (margin < 0) {
        margin = 0;
    }

    if (margin > 100) {
        margin = 100;
    }

    g_settings.extraHoverMarginMm.store(
        margin,
        std::memory_order_relaxed
    );

    g_settings.autoHideDelayMs.store(
        static_cast<DWORD>(delay),
        std::memory_order_relaxed
    );

    g_settings.hideSecondaryTaskbars.store(
        secondary,
        std::memory_order_relaxed
    );
}


// ============================================================
// Tool callbacks
// ============================================================

BOOL WhTool_ModInit() {
    Wh_Log(
        L"Hide Taskbar Only on Desktop: Init"
    );

    LoadSettings();

    g_hThreadReadyEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr
        );

    if (!g_hThreadReadyEvent) {
        Wh_Log(
            L"CreateEvent failed: %lu",
            GetLastError()
        );

        return FALSE;
    }

    g_hThread =
        CreateThread(
            nullptr,
            0,
            HookThread,
            nullptr,
            0,
            &g_threadId
        );

    if (!g_hThread) {
        Wh_Log(
            L"CreateThread failed: %lu",
            GetLastError()
        );

        CloseHandle(g_hThreadReadyEvent);
        g_hThreadReadyEvent = nullptr;

        return FALSE;
    }

    /*
     * Wait until the worker has created its message queue.
     *
     * The event handle remains valid until the worker is joined.
     * This avoids a race where the worker could call SetEvent on a
     * handle that the init thread already closed after a timeout.
     */
    DWORD result =
        WaitForSingleObject(
            g_hThreadReadyEvent,
            5000
        );

    if (result != WAIT_OBJECT_0) {
        Wh_Log(
            L"Worker thread failed to become ready"
        );

        /*
         * Ask the worker to stop using the same message queue that
         * will later be used for normal refresh requests.
         */
        while (
            !PostThreadMessageW(
                g_threadId,
                WM_APP_STOP_THREAD,
                0,
                0
            )
        ) {
            if (
                WaitForSingleObject(
                    g_hThread,
                    100
                ) != WAIT_TIMEOUT
            ) {
                break;
            }
        }

        WaitForSingleObject(
            g_hThread,
            INFINITE
        );

        CloseHandle(g_hThread);
        g_hThread = nullptr;
        g_threadId = 0;

        CloseHandle(g_hThreadReadyEvent);
        g_hThreadReadyEvent = nullptr;

        return FALSE;
    }

    return TRUE;
}


void WhTool_ModSettingsChanged() {
    Wh_Log(
        L"Hide Taskbar Only on Desktop: Settings changed"
    );

    LoadSettings();

    /*
     * Native taskbar auto-hide is a system setting, so refresh it here
     * rather than on every foreground/minimize event.
     */
    
    /*
     * Apply settings immediately instead of waiting for a timer tick.
     * In particular, this restores secondary taskbars when the setting
     * changes from ON to OFF while the desktop is already active.
     */
    RequestStateRefresh();
}


void WhTool_ModUninit() {
    Wh_Log(
        L"Hide Taskbar Only on Desktop: Uninit"
    );

    if (g_hThread) {
        /*
         * Never unload the mod while the worker could still execute
         * code from it. Use an application message which converts to
         * PostQuitMessage on the worker thread.
         */
        while (
            !PostThreadMessageW(
                g_threadId,
                WM_APP_STOP_THREAD,
                0,
                0
            )
        ) {
            if (
                WaitForSingleObject(
                    g_hThread,
                    100
                ) != WAIT_TIMEOUT
            ) {
                break;
            }
        }

        /*
         * IMPORTANT:
         * No finite timeout here. The module must remain mapped until
         * the worker has completely stopped and removed its hooks.
         */
        WaitForSingleObject(
            g_hThread,
            INFINITE
        );

        CloseHandle(g_hThread);

        g_hThread = nullptr;
        g_threadId = 0;
    }

    if (g_hThreadReadyEvent) {
        CloseHandle(g_hThreadReadyEvent);
        g_hThreadReadyEvent = nullptr;
    }

    /*
     * Final restoration in case the worker exited through an unusual
     * path. ShowWindow on an already-visible taskbar is harmless.
     */
    SetTaskbarVisibility(true);
}


// ============================================================
// Windhawk Tool Mod launcher
//
// IMPORTANT:
// Keep the official Windhawk launcher section below unchanged.
// ============================================================

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;

    if (
        ProcessIdToSessionId(
            GetCurrentProcessId(),
            &sessionId
        ) &&
        sessionId == 0
    ) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;

    int argc;

    LPWSTR* argv =
        CommandLineToArgvW(
            GetCommandLine(),
            &argc
        );

    if (!argv) {
        Wh_Log(
            L"CommandLineToArgvW failed"
        );

        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (
            wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0
        ) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (
            wcscmp(argv[i], L"-tool-mod") == 0
        ) {
            isToolModProcess = true;

            if (
                wcscmp(
                    argv[i + 1],
                    WH_MOD_ID
                ) == 0
            ) {
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
            CreateMutexW(
                nullptr,
                TRUE,
                L"windhawk-tool-mod_" WH_MOD_ID
            );

        if (!g_toolModProcessMutex) {
            Wh_Log(
                L"CreateMutex failed"
            );

            ExitProcess(1);
        }

        if (
            GetLastError() ==
            ERROR_ALREADY_EXISTS
        ) {
            Wh_Log(
                L"Tool mod already running (%s)",
                WH_MOD_ID
            );

            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            reinterpret_cast<IMAGE_DOS_HEADER*>(
                GetModuleHandle(nullptr)
            );

        IMAGE_NT_HEADERS* ntHeaders =
            reinterpret_cast<IMAGE_NT_HEADERS*>(
                reinterpret_cast<BYTE*>(dosHeader) +
                dosHeader->e_lfanew
            );

        DWORD entryPointRVA =
            ntHeaders->OptionalHeader
                .AddressOfEntryPoint;

        void* entryPoint =
            reinterpret_cast<BYTE*>(dosHeader) +
            entryPointRVA;

        Wh_SetFunctionHook(
            entryPoint,
            reinterpret_cast<void*>(
                EntryPoint_Hook
            ),
            nullptr
        );

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

    switch (
        GetModuleFileNameW(
            nullptr,
            currentProcessPath,
            ARRAYSIZE(currentProcessPath)
        )
    ) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(
                L"GetModuleFileName failed"
            );

            return;
    }

    WCHAR commandLine[
        MAX_PATH +
        2 +
        (sizeof(
            L" -tool-mod \"" WH_MOD_ID "\""
        ) / sizeof(WCHAR)) -
        1
    ];

    swprintf_s(
        commandLine,
        L"\"%s\" -tool-mod \"%s\"",
        currentProcessPath,
        WH_MOD_ID
    );

    HMODULE kernelModule =
        GetModuleHandleW(
            L"kernelbase.dll"
        );

    if (!kernelModule) {
        kernelModule =
            GetModuleHandleW(
                L"kernel32.dll"
            );

        if (!kernelModule) {
            Wh_Log(
                L"No kernelbase.dll/kernel32.dll"
            );

            return;
        }
    }

    using CreateProcessInternalW_t =
        BOOL(WINAPI*)(
            HANDLE,
            LPCWSTR,
            LPWSTR,
            LPSECURITY_ATTRIBUTES,
            LPSECURITY_ATTRIBUTES,
            WINBOOL,
            DWORD,
            LPVOID,
            LPCWSTR,
            LPSTARTUPINFO,
            LPPROCESS_INFORMATION,
            PHANDLE
        );

    CreateProcessInternalW_t
        pCreateProcessInternalW =
            reinterpret_cast<
                CreateProcessInternalW_t
            >(
                GetProcAddress(
                    kernelModule,
                    "CreateProcessInternalW"
                )
            );

    if (!pCreateProcessInternalW) {
        Wh_Log(
            L"No CreateProcessInternalW"
        );

        return;
    }

    STARTUPINFO si = {};
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;

    PROCESS_INFORMATION pi = {};

    if (
        !pCreateProcessInternalW(
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
            nullptr
        )
    ) {
        Wh_Log(
            L"CreateProcess failed: %lu",
            GetLastError()
        );

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
