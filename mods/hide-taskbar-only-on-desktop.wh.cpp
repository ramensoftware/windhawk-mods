// ==WindhawkMod==
// @id              hide-taskbar-only-on-desktop
// @name            Hide Taskbar Only on Desktop
// @description     Hides the taskbar when the desktop is active, while showing it for applications and on bottom-edge hover
// @version         1.1.0
// @author          Sahil Dashoni
// @github          https://github.com/Sahil-Dashoni
// @include         windhawk.exe
// @compilerOptions -lshell32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Taskbar Only on Desktop

Hides the Windows taskbar when the desktop is active, while keeping it
visible whenever an application or shell UI is active.

## Features

- Hides the taskbar on the desktop.
- Shows the taskbar immediately when an application becomes active.
- Automatically hides the taskbar after minimizing or closing the last
  application.
- Reveals the taskbar when the mouse enters the taskbar-sized hover area.
- The hover area automatically follows the taskbar size and DPI.
- Adds a configurable extra hover margin in millimeters.
- Uses a configurable delay after leaving the hover area.
- The delay is only used for a hover reveal.
- Minimizing or closing the last application hides the taskbar immediately.
- Keeps the taskbar available while interacting with taskbar buttons.
- Supports secondary taskbars on additional monitors.
- Supports different monitor DPI settings.
- Supports bottom, top, left and right taskbar positions.
- Does not inject the mod into Explorer.

## Notes

This mod intentionally differs from native Windows auto-hide:
it hides the taskbar window without changing the desktop work area.

If native Windows taskbar auto-hide is already enabled, this mod temporarily
stands down so the two mechanisms don't fight each other.

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
#include <algorithm>
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

HWINEVENTHOOK g_hWinEventHookForeground = nullptr;

std::atomic<bool> g_taskbarHidden{false};

bool g_onDesktopState = false;
bool g_shownDueToHover = false;
ULONGLONG g_hideDeadline = 0;

constexpr UINT WM_APP_REFRESH_STATE = WM_APP + 1;


// ============================================================
// Utility
// ============================================================

bool IsDesktopWindow(HWND hwnd) {
    if (!hwnd) {
        return false;
    }

    WCHAR className[256];

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

    WCHAR className[256];

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

    WCHAR className[256];

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

    /*
     * Don't blindly discard every owned window.
     *
     * Some applications use owned top-level windows for legitimate
     * dialogs. Only ignore windows which are clearly tool windows.
     */
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

    if (!foreground) {
        g_onDesktopState = !AnyOtherVisibleWindowExists();
        return;
    }

    /*
     * If the taskbar itself is foreground and the mouse is actually
     * over it, treat the user as interacting with the taskbar.
     *
     * This prevents the taskbar from immediately hiding after a
     * taskbar click while an application is still open.
     */
    if (IsTaskbarForegroundAndUnderCursor(foreground)) {
        g_onDesktopState = false;
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

    WCHAR className[256];

    if (GetClassNameW(
            foreground,
            className,
            ARRAYSIZE(className)
        ) &&
        IsAmbiguousForegroundClass(className)) {

        g_onDesktopState = !AnyOtherVisibleWindowExists();
        return;
    }

    /*
     * A normal foreground window means we're not on the desktop.
     */
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
        MonitorFromWindow(primary, MONITOR_DEFAULTTONULL) ==
            hMonitor) {
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

bool IsNativeTaskbarAutoHideEnabled() {
    APPBARDATA abd = {};
    abd.cbSize = sizeof(abd);

    UINT_PTR state =
        SHAppBarMessage(ABM_GETSTATE, &abd);

    return (state & ABS_AUTOHIDE) != 0;
}


// ============================================================
// Taskbar visibility
// ============================================================

void SetTaskbarVisibility(bool show) {
    HWND primary = FindPrimaryTaskbar();

    if (primary) {
        ShowWindow(
            primary,
            show ? SW_SHOW : SW_HIDE
        );
    }

    /*
     * Always restore secondary taskbars when showing.
     *
     * This is important when the user changes
     * hideSecondaryTaskbars from ON to OFF.
     */
    bool shouldProcessSecondary =
        show ||
        g_settings.hideSecondaryTaskbars.load(
            std::memory_order_relaxed
        );

    if (!shouldProcessSecondary) {
        return;
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
        ShowWindow(
            secondary,
            show ? SW_SHOW : SW_HIDE
        );
    }
}


// ============================================================
// Hover zone
// ============================================================

int MillimetersToPixels(int mm, UINT dpi) {
    if (mm <= 0) {
        return 0;
    }

    /*
     * 25.4 mm = 1 inch.
     *
     * Clamp the margin to avoid accidentally making the entire
     * monitor a hover area.
     */
    int maxPx = GetSystemMetrics(SM_CYSCREEN) / 2;

    int px = MulDiv(
        mm,
        static_cast<int>(dpi),
        254
    );

    if (px < 0) {
        px = 0;
    }

    if (px > maxPx) {
        px = maxPx;
    }

    return px;
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

    /*
     * If secondary taskbars are disabled for this mod,
     * don't use their area as a reveal zone.
     */
    WCHAR className[256];

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

    int marginPx = MillimetersToPixels(
        g_settings.extraHoverMarginMm.load(
            std::memory_order_relaxed
        ),
        dpi
    );

    RECT hoverRect = taskbarRect;

    InflateRect(
        &hoverRect,
        marginPx,
        marginPx
    );

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);

    if (!GetMonitorInfoW(monitor, &mi)) {
        return false;
    }

    /*
     * Keep the reveal area associated with the taskbar edge.
     *
     * The normal taskbar rectangle already represents the full
     * taskbar span, while the thickness determines the hover depth.
     */

    int monitorWidth =
        mi.rcMonitor.right - mi.rcMonitor.left;

    int monitorHeight =
        mi.rcMonitor.bottom - mi.rcMonitor.top;

    int taskbarWidth =
        taskbarRect.right - taskbarRect.left;

    int taskbarHeight =
        taskbarRect.bottom - taskbarRect.top;

    /*
     * Detect the edge on which the taskbar is docked.
     */
    int distanceTop =
        abs(taskbarRect.top - mi.rcMonitor.top);

    int distanceBottom =
        abs(taskbarRect.bottom - mi.rcMonitor.bottom);

    int distanceLeft =
        abs(taskbarRect.left - mi.rcMonitor.left);

    int distanceRight =
        abs(taskbarRect.right - mi.rcMonitor.right);

    int edgeDistance =
    std::min(
        std::min(distanceTop, distanceBottom),
        std::min(distanceLeft, distanceRight)
    );

    /*
     * If the taskbar is clearly docked to the bottom.
     */
    if (
        edgeDistance == distanceBottom &&
        taskbarHeight < monitorHeight
    ) {
        hoverRect.top =
            taskbarRect.top - marginPx;

        hoverRect.bottom =
            taskbarRect.bottom + marginPx;

        return PtInRect(
            &hoverRect,
            pt
        ) != FALSE;
    }

    /*
     * Top-docked taskbar.
     */
    if (
        edgeDistance == distanceTop &&
        taskbarHeight < monitorHeight
    ) {
        hoverRect.top =
            taskbarRect.top - marginPx;

        hoverRect.bottom =
            taskbarRect.bottom + marginPx;

        return PtInRect(
            &hoverRect,
            pt
        ) != FALSE;
    }

    /*
     * Left-docked taskbar.
     */
    if (
        edgeDistance == distanceLeft &&
        taskbarWidth < monitorWidth
    ) {
        hoverRect.left =
            taskbarRect.left - marginPx;

        hoverRect.right =
            taskbarRect.right + marginPx;

        return PtInRect(
            &hoverRect,
            pt
        ) != FALSE;
    }

    /*
     * Right-docked taskbar.
     */
    if (
        edgeDistance == distanceRight &&
        taskbarWidth < monitorWidth
    ) {
        hoverRect.left =
            taskbarRect.left - marginPx;

        hoverRect.right =
            taskbarRect.right + marginPx;

        return PtInRect(
            &hoverRect,
            pt
        ) != FALSE;
    }

    return PtInRect(
        &hoverRect,
        pt
    ) != FALSE;
}


// ============================================================
// State machine
// ============================================================

void UpdateTaskbarState() {
    /*
     * If native Windows auto-hide is enabled, don't fight it.
     */
    if (IsNativeTaskbarAutoHideEnabled()) {
        if (g_taskbarHidden.load(
                std::memory_order_relaxed
            )) {

            SetTaskbarVisibility(true);

            g_taskbarHidden.store(
                false,
                std::memory_order_relaxed
            );
        }

        g_hideDeadline = 0;
        g_shownDueToHover = false;

        return;
    }

    /*
     * Only calculate the hover zone when the desktop is actually
     * active. This avoids doing monitor/taskbar geometry work
     * unnecessarily while an application is focused.
     */
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

        if (
            g_taskbarHidden.load(
                std::memory_order_relaxed
            )
        ) {
            SetTaskbarVisibility(true);

            g_taskbarHidden.store(
                false,
                std::memory_order_relaxed
            );
        }

        return;
    }

    /*
     * Desktop + cursor inside hover area.
     */
    if (hovering) {
        g_hideDeadline = 0;
        g_shownDueToHover = true;

        if (
            g_taskbarHidden.load(
                std::memory_order_relaxed
            )
        ) {
            SetTaskbarVisibility(true);

            g_taskbarHidden.store(
                false,
                std::memory_order_relaxed
            );
        }

        return;
    }

    /*
     * Desktop + taskbar already hidden.
     */
    if (
        g_taskbarHidden.load(
            std::memory_order_relaxed
        )
    ) {
        g_hideDeadline = 0;
        return;
    }

    /*
     * Taskbar wasn't revealed by hover.
     *
     * Therefore this is an ordinary desktop transition and must
     * hide immediately.
     */
    if (!g_shownDueToHover) {
        SetTaskbarVisibility(false);

        g_taskbarHidden.store(
            true,
            std::memory_order_relaxed
        );

        g_hideDeadline = 0;

        return;
    }

    /*
     * The taskbar was revealed by hover and the cursor has now
     * left the hover zone.
     *
     * This is the ONLY path where the delay is used.
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
    }
    else if (now >= g_hideDeadline) {
        SetTaskbarVisibility(false);

        g_taskbarHidden.store(
            true,
            std::memory_order_relaxed
        );

        g_hideDeadline = 0;
        g_shownDueToHover = false;
    }
}


// ============================================================
// WinEvent hook
// ============================================================

void CALLBACK WinEventProc(
    HWINEVENTHOOK hWinEventHook,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG idChild,
    DWORD idEventThread,
    DWORD dwmsEventTime
) {
    if (
        idObject != OBJID_WINDOW ||
        event != EVENT_SYSTEM_FOREGROUND
    ) {
        return;
    }

    /*
     * WinEvent callbacks are delivered to the worker's message-loop
     * thread for OUTOFCONTEXT hooks.
     *
     * Refreshing immediately makes application activation feel
     * instant, while the timer handles minimize/close transitions.
     */
    RefreshDesktopState();
    UpdateTaskbarState();
}


// ============================================================
// Worker thread
// ============================================================

DWORD WINAPI HookThread(LPVOID) {
    /*
     * Force creation of this thread's USER message queue before
     * WhTool_ModUninit can attempt PostThreadMessageW.
     */
    MSG initialMessage;

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

    g_hWinEventHookForeground =
        SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND,
            EVENT_SYSTEM_FOREGROUND,
            nullptr,
            WinEventProc,
            0,
            0,
            WINEVENT_OUTOFCONTEXT
        );

    if (!g_hWinEventHookForeground) {
        Wh_Log(
            L"SetWinEventHook failed: %lu",
            GetLastError()
        );
    }

    /*
     * Apply the correct initial state immediately.
     */
    RefreshDesktopState();
    UpdateTaskbarState();

    /*
     * 100 ms is retained because the desktop state must react
     * quickly to minimizing/closing the last application.
     */
    constexpr UINT kPollIntervalMs = 100;

    UINT_PTR timerId =
        SetTimer(
            nullptr,
            0,
            kPollIntervalMs,
            nullptr
        );

    if (!timerId) {
        Wh_Log(
            L"SetTimer failed: %lu",
            GetLastError()
        );
    }

    MSG msg;

    while (
        GetMessageW(
            &msg,
            nullptr,
            0,
            0
        ) > 0
    ) {
        if (msg.message == WM_APP_REFRESH_STATE) {
            RefreshDesktopState();
            UpdateTaskbarState();
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);

        /*
         * The timer generated by SetTimer(nullptr, ...) produces
         * WM_TIMER messages.
         */
        if (msg.message == WM_TIMER &&
            msg.wParam == timerId) {

            RefreshDesktopState();
            UpdateTaskbarState();
        }
    }

    if (timerId) {
        KillTimer(
            nullptr,
            timerId
        );
    }

    if (g_hWinEventHookForeground) {
        UnhookWinEvent(
            g_hWinEventHookForeground
        );

        g_hWinEventHookForeground = nullptr;
    }

    /*
     * Always restore the taskbar when the worker exits.
     */
    SetTaskbarVisibility(true);

    g_taskbarHidden.store(
        false,
        std::memory_order_relaxed
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

    /*
     * Prevent pathological values from making the UI unusable.
     */
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
    Wh_Log(L"Hide Taskbar Only on Desktop: Init");

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
     * This guarantees that PostThreadMessageW can be used safely
     * during shutdown.
     */
    DWORD result =
        WaitForSingleObject(
            g_hThreadReadyEvent,
            5000
        );

    CloseHandle(g_hThreadReadyEvent);
    g_hThreadReadyEvent = nullptr;

    if (result != WAIT_OBJECT_0) {
        Wh_Log(
            L"Worker thread failed to become ready"
        );

        /*
         * The worker is still alive. Stop it and wait for it.
         */
        while (
            !PostThreadMessageW(
                g_threadId,
                WM_QUIT,
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

        return FALSE;
    }

    return TRUE;
}


void WhTool_ModSettingsChanged() {
    Wh_Log(
        L"Hide Taskbar Only on Desktop: Settings changed"
    );

    /*
     * Atomics make this safe even though Windhawk may call
     * the settings callback from another thread.
     */
    LoadSettings();

    /*
     * The worker will observe the new values on its next
     * 100 ms update.
     */
}


void WhTool_ModUninit() {
    Wh_Log(
        L"Hide Taskbar Only on Desktop: Uninit"
    );

    if (g_hThread) {
        /*
         * The worker creates its USER message queue before
         * signalling g_hThreadReadyEvent, so this should normally
         * succeed immediately.
         *
         * Still retry in case the thread exited between checks.
         */
        while (
            !PostThreadMessageW(
                g_threadId,
                WM_QUIT,
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
         * Never use a finite timeout here.
         *
         * The mod must not be unloaded while HookThread can still
         * execute code from this module.
         */
        WaitForSingleObject(
            g_hThread,
            INFINITE
        );

        CloseHandle(g_hThread);

        g_hThread = nullptr;
        g_threadId = 0;
    }

    /*
     * The worker normally restores the taskbar itself, but do this
     * again here as a final safety measure.
     */
    SetTaskbarVisibility(true);

    g_taskbarHidden.store(
        false,
        std::memory_order_relaxed
    );
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
            LPSTARTUPINFOW,
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

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };

    PROCESS_INFORMATION pi{};

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
            L"CreateProcess failed"
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
