// ==WindhawkMod==
// @id hide-taskbar-only-on-desktop
// @name Hide Taskbar Only on Desktop
// @description Hides the taskbar when the desktop is active, while showing it for applications and on taskbar hover
// @version 1.3.0
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
- Supports bottom, top, left and right taskbar positions.
- Runs as a Windhawk tool instead of being injected into Explorer.

## Notes

This mod intentionally differs from native Windows auto-hide:
it hides the taskbar window without changing the desktop work area.

If native Windows taskbar auto-hide is enabled, this mod stands down so
the two mechanisms don't fight each other. If native auto-hide is enabled
while this mod has hidden the taskbar, the mod restores the taskbar first
so Windows can take control of it again.

This mod uses only foreground/minimize Windows events for application state
changes and only uses a short timer while cursor hover handling is actually
needed. It deliberately avoids system-wide object show/hide/destroy hooks.

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
  $name: Auto-hide delay after hover (ms)
  $description: >-
    How long the taskbar remains visible after the mouse leaves the
    hover area. This delay is only used after a hover reveal.
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
constexpr WPARAM kRefreshNativeAutoHide = 1;

constexpr UINT kHoverTimerIntervalMs = 200;

UINT_PTR g_hoverTimerId = 0;

std::atomic<bool> g_refreshPosted{false};
std::atomic<bool> g_nativeAutoHideEnabled{false};

bool g_onDesktopState = false;
// True only while the taskbar itself has foreground focus and the cursor
// is still over the taskbar. This keeps the hover timer alive after a
// taskbar click, so moving the cursor away can return to desktop state.
bool g_taskbarIsForeground = false;
bool g_shownDueToHover = false;
ULONGLONG g_hideDeadline = 0;

constexpr size_t kMaxWinEventHooks = 3;
HWINEVENTHOOK g_hWinEventHooks[kMaxWinEventHooks] = {};


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

    static const WCHAR* kShellClasses[] = {
        L"Progman",
        L"WorkerW",
        L"Shell_TrayWnd",
        L"Shell_SecondaryTrayWnd",
        L"Windows.UI.Core.CoreWindow",
        L"Xaml_WindowedPopupClass",
        L"TaskListThumbnailWnd",
        L"SysShadow",
        L"tooltips_class32",
        L"MSCTFIME UI",
        L"IME",
    };

    for (const WCHAR* shellClass : kShellClasses) {
        if (wcscmp(className, shellClass) == 0) {
            return true;
        }
    }

    return false;
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

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    bool* pFoundOther = reinterpret_cast<bool*>(lParam);

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

    if (IsShellChromeClass(className)) {
        return TRUE;
    }

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

    if (exStyle & WS_EX_TOOLWINDOW) {
        return TRUE;
    }

    BOOL cloaked = FALSE;

    if (SUCCEEDED(
            DwmGetWindowAttribute(
                hwnd,
                DWMWA_CLOAKED,
                &cloaked,
                sizeof(cloaked)
            )
        ) &&
        cloaked) {
        return TRUE;
    }

    RECT rect;

    if (!GetWindowRect(hwnd, &rect)) {
        return TRUE;
    }

    if (rect.right <= rect.left ||
        rect.bottom <= rect.top) {
        return TRUE;
    }

    // Keep legitimate owned application dialogs when they advertise
    // themselves as application windows.
    if (GetWindow(hwnd, GW_OWNER) != nullptr &&
        !(exStyle & WS_EX_APPWINDOW)) {
        return TRUE;
    }

    *pFoundOther = true;

    return FALSE;
}


bool AnyOtherVisibleWindowExists() {
    bool found = false;

    EnumWindows(
        EnumWindowsProc,
        reinterpret_cast<LPARAM>(&found)
    );

    return found;
}


// ============================================================
// Foreground / desktop state
// ============================================================

bool IsAmbiguousForegroundClass(const WCHAR* className) {
    if (!className) {
        return false;
    }

    return wcscmp(className, L"Shell_TrayWnd") == 0 ||
           wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
}


bool IsTaskbarForegroundAndUnderCursor(HWND foreground) {
    if (!IsTaskbarWindow(foreground)) {
        return false;
    }

    POINT pt;

    if (!GetCursorPos(&pt)) {
        return false;
    }

    RECT rect;

    if (!GetWindowRect(foreground, &rect)) {
        return false;
    }

    return PtInRect(&rect, pt) != FALSE;
}


void RefreshDesktopState() {
    HWND foreground = GetForegroundWindow();

    // This flag is transient: it is true only while the taskbar itself
    // has focus and the cursor is over it. The hover timer uses it to
    // re-check the state after the cursor leaves the taskbar.
    g_taskbarIsForeground = false;

    if (!foreground) {
        g_onDesktopState = !AnyOtherVisibleWindowExists();
        return;
    }

    if (IsTaskbarWindow(foreground)) {
        if (IsTaskbarForegroundAndUnderCursor(foreground)) {
            g_taskbarIsForeground = true;
            g_onDesktopState = false;
            return;
        }

        // The taskbar can remain the foreground window after the user
        // moves the cursor away. In that situation, treat the desktop
        // as active if no other application window exists.
        g_onDesktopState = !AnyOtherVisibleWindowExists();
        return;
    }

    if (IsDesktopWindow(foreground)) {
        g_onDesktopState = true;
        return;
    }

    if (IsIconic(foreground)) {
        g_onDesktopState = !AnyOtherVisibleWindowExists();
        return;
    }

    WCHAR className[256] = {};

    if (GetClassNameW(
            foreground,
            className,
            ARRAYSIZE(className)
        ) &&
        IsAmbiguousForegroundClass(className)) {
        g_onDesktopState = !AnyOtherVisibleWindowExists();
        return;
    }

    g_onDesktopState = false;
}


// ============================================================
// Taskbar discovery
// ============================================================

HWND FindPrimaryTaskbar() {
    return FindWindowW(L"Shell_TrayWnd", nullptr);
}


HWND FindTaskbarForMonitor(HMONITOR hMonitor) {
    if (!hMonitor) {
        return nullptr;
    }

    HWND primary = FindPrimaryTaskbar();

    if (primary &&
        MonitorFromWindow(primary, MONITOR_DEFAULTTONULL) == hMonitor) {
        return primary;
    }

    HWND secondary = nullptr;

    while (
        (secondary = FindWindowExW(
            nullptr,
            secondary,
            L"Shell_SecondaryTrayWnd",
            nullptr
        )) != nullptr
    ) {
        if (
            MonitorFromWindow(
                secondary,
                MONITOR_DEFAULTTONULL
            ) == hMonitor
        ) {
            return secondary;
        }
    }

    return nullptr;
}


// ============================================================
// Native Windows auto-hide detection
// ============================================================

bool QueryNativeTaskbarAutoHide() {
    APPBARDATA abd = {};
    abd.cbSize = sizeof(abd);

    UINT_PTR state =
        SHAppBarMessage(ABM_GETSTATE, &abd);

    return (state & ABS_AUTOHIDE) != 0;
}


void RefreshNativeTaskbarAutoHideState() {
    bool enabled = QueryNativeTaskbarAutoHide();

    g_nativeAutoHideEnabled.store(
        enabled,
        std::memory_order_relaxed
    );
}


// ============================================================
// Taskbar visibility
// ============================================================

void SetWindowVisibilityIfNeeded(HWND hwnd, bool show) {
    if (!hwnd) {
        return;
    }

    bool visible = IsWindowVisible(hwnd) != FALSE;

    if (visible != show) {
        ShowWindow(
            hwnd,
            show ? SW_SHOW : SW_HIDE
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


bool IsCursorInTaskbarHoverZone() {
    POINT pt;

    if (!GetCursorPos(&pt)) {
        return false;
    }

    HMONITOR monitor =
        MonitorFromPoint(
            pt,
            MONITOR_DEFAULTTONULL
        );

    if (!monitor) {
        return false;
    }

    HWND taskbar =
        FindTaskbarForMonitor(monitor);

    if (!taskbar) {
        return false;
    }

    WCHAR className[256] = {};

    if (
        GetClassNameW(
            taskbar,
            className,
            ARRAYSIZE(className)
        ) == 0
    ) {
        return false;
    }

    bool isSecondary =
        wcscmp(
            className,
            L"Shell_SecondaryTrayWnd"
        ) == 0;

    if (
        isSecondary &&
        !g_settings.hideSecondaryTaskbars.load(
            std::memory_order_relaxed
        )
    ) {
        return false;
    }

    RECT taskbarRect;

    if (!GetWindowRect(taskbar, &taskbarRect)) {
        return false;
    }

    if (
        taskbarRect.right <= taskbarRect.left ||
        taskbarRect.bottom <= taskbarRect.top
    ) {
        return false;
    }

    UINT dpi = GetDpiForWindow(taskbar);

    if (dpi == 0) {
        dpi = 96;
    }

    int marginPx =
        MillimetersToPixels(
            g_settings.extraHoverMarginMm.load(
                std::memory_order_relaxed
            ),
            dpi
        );

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);

    if (!GetMonitorInfoW(monitor, &mi)) {
        return false;
    }

    int monitorWidth =
        mi.rcMonitor.right - mi.rcMonitor.left;

    int monitorHeight =
        mi.rcMonitor.bottom - mi.rcMonitor.top;

    int maxMargin =
        monitorWidth < monitorHeight
            ? monitorWidth / 2
            : monitorHeight / 2;

    if (maxMargin < 0) {
        maxMargin = 0;
    }

    if (marginPx > maxMargin) {
        marginPx = maxMargin;
    }

    /*
     * The taskbar rectangle itself already identifies the correct edge
     * and thickness. Inflating it handles bottom, top, left, right and
     * vertical taskbars without assuming a particular docking position.
     */
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
    /*
     * If native Windows auto-hide is enabled, don't fight it.
     *
     * The native setting is cached and refreshed on state-changing
     * Windows events/settings refreshes, rather than being queried
     * on every cursor-poll tick.
     */
    if (
        g_nativeAutoHideEnabled.load(
            std::memory_order_relaxed
        )
    ) {
        g_hideDeadline = 0;
        g_shownDueToHover = false;
        StopHoverTimer();

        // Give control back to Windows' native auto-hide implementation.
        // This is important if native auto-hide was enabled while our
        // mod had previously hidden the taskbar with SW_HIDE.
        SetTaskbarVisibility(true);
        return;
    }

    bool hovering = false;

    if (g_onDesktopState) {
        hovering = IsCursorInTaskbarHoverZone();
    }

    /*
     * Application / non-desktop state.
     */
    if (!g_onDesktopState) {
        g_hideDeadline = 0;
        g_shownDueToHover = false;

        SetTaskbarVisibility(true);

        // If the taskbar itself has focus, keep polling so that moving
        // the cursor away from it can transition back to desktop state.
        EnsureHoverTimer(g_taskbarIsForeground);

        return;
    }

    /*
     * Desktop + cursor inside hover area.
     */
    if (hovering) {
        g_hideDeadline = 0;
        g_shownDueToHover = true;

        SetTaskbarVisibility(true);
        EnsureHoverTimer(true);

        return;
    }

    /*
     * Desktop + taskbar is already hidden.
     *
     * Do not use a cached hidden flag as the source of truth.
     * The actual taskbar window visibility is checked here.
     */
    if (IsPrimaryTaskbarHidden()) {
        /*
         * If secondary hiding was disabled while the primary taskbar
         * was already hidden, restore the secondary taskbars immediately.
         */
        if (
            !g_settings.hideSecondaryTaskbars.load(
                std::memory_order_relaxed
            )
        ) {
            SetSecondaryTaskbarsVisibility(true);
        } else {
            // Keep secondary taskbars consistent after Explorer recreates them.
            SetSecondaryTaskbarsVisibility(false);
        }

        g_hideDeadline = 0;
        EnsureHoverTimer(true);

        return;
    }

    /*
     * Taskbar is visible on the desktop and wasn't revealed by hover.
     * Hide immediately.
     */
    if (!g_shownDueToHover) {
        SetTaskbarVisibility(false);

        g_hideDeadline = 0;
        EnsureHoverTimer(true);

        return;
    }

    /*
     * The taskbar was revealed by hover and the cursor has now left
     * the hover zone. This is the only path where the configured delay
     * is used.
     */
    ULONGLONG now = GetTickCount64();

    DWORD delay =
        g_settings.autoHideDelayMs.load(
            std::memory_order_relaxed
        );

    if (delay > 60000) {
        delay = 60000;
    }

    if (g_hideDeadline == 0) {
        g_hideDeadline =
            now + static_cast<ULONGLONG>(delay);
    } else if (now >= g_hideDeadline) {
        SetTaskbarVisibility(false);

        g_hideDeadline = 0;
        g_shownDueToHover = false;
    }

    EnsureHoverTimer(true);
}


// ============================================================
// Worker refresh message
// ============================================================

void RequestStateRefresh(bool refreshNativeAutoHide = false) {
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
            refreshNativeAutoHide ? kRefreshNativeAutoHide : 0,
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
    RequestStateRefresh(event == EVENT_SYSTEM_FOREGROUND);
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
    RefreshNativeTaskbarAutoHideState();
    RefreshDesktopState();
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

            if (msg.wParam == kRefreshNativeAutoHide) {
                RefreshNativeTaskbarAutoHideState();
            }

            RefreshDesktopState();
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
            RefreshDesktopState();
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

    g_refreshPosted.store(
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

    DWORD delay =
        static_cast<DWORD>(
            Wh_GetIntSetting(
                L"autoHideDelayMs"
            )
        );

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

    if (delay > 60000) {
        delay = 60000;
    }

    g_settings.extraHoverMarginMm.store(
        margin,
        std::memory_order_relaxed
    );

    g_settings.autoHideDelayMs.store(
        delay,
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
     * Apply settings immediately instead of waiting for a timer tick.
     * In particular, this restores secondary taskbars when the setting
     * changes from ON to OFF while the desktop is already active.
     */
    RequestStateRefresh(true);
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

    /*
     * MinGW's swprintf_s requires the destination buffer size.
     * The previous version omitted it, which caused the PR build
     * to fail on the Windhawk compiler.
     */
    swprintf_s(
        commandLine,
        ARRAYSIZE(commandLine),
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
