// ==WindhawkMod==
// @id              hide-taskbar-only-on-desktop
// @name            Hide Taskbar Only on Desktop
// @description     Hides the taskbar on the desktop, shows it for other windows or when you hover near the bottom edge.
// @version         1.1.0
// @author          Sahil Dashoni
// @github          https://github.com/Sahil-Dashoni
// @include         explorer.exe
// @compilerOptions -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Taskbar on Desktop

Hides the Windows taskbar while the desktop is active and shows it again
when an application or supported Windows shell UI is active.

### Features

- Hides the taskbar when no normal visible window is active.
- Shows the taskbar immediately when an application is active.
- Reveals the taskbar when the mouse enters the bottom area of a monitor.
- The reveal zone automatically follows the actual taskbar height.
- Adds a configurable extra hover margin in millimeters.
- Uses a configurable delay only when hiding after a mouse hover.
- Hides immediately after minimizing or closing the last application.
- Keeps the taskbar visible while interacting with taskbar buttons.
- Supports optional secondary taskbars on additional monitors.
- Re-checks the desktop state periodically so minimizing the last window
  does not require an extra desktop click.

### Settings

- Extra hover margin: additional space above the taskbar-height reveal zone.
- Auto-hide delay after hover: delay before hiding after leaving the reveal zone.
- Hide secondary taskbars: controls taskbars on additional monitors.

### Notes

The mod uses a small worker thread with a Windows message queue for
WinEvent notifications and periodic state checks. The worker is stopped
and joined before the mod is unloaded.
*/
// ==WindhawkModReadme==

// ==WindhawkModSettings==
/*
- extraHoverMarginMm: 5
  $name: Extra hover margin (mm)
  $description: >-
    Adds extra space above the automatically detected taskbar-height
    reveal zone. Default is 5 mm.

- autoHideDelayMs: 700
  $name: Auto-hide delay after hover (ms)
  $description: >-
    How long to wait after the mouse leaves the bottom-edge hover zone
    before hiding the taskbar. Only applies after a mouse hover reveal.
    Default is 700 ms.

- hideSecondaryTaskbars: true
  $name: Hide secondary taskbars
  $description: >-
    Also hide and show taskbars on additional monitors when Windows is
    configured to show taskbars on all displays.
*/
// ==WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>


// ============================================================
// Settings
// ============================================================

struct {
    int extraHoverMarginMm;
    DWORD autoHideDelayMs;
    bool hideSecondaryTaskbars;
} settings;


// ============================================================
// Global state
// ============================================================

HWINEVENTHOOK g_hWinEventHookForeground = nullptr;

HANDLE g_hThread = nullptr;
HANDLE g_stopEvent = nullptr;

DWORD g_threadId = 0;

bool g_taskbarHidden = false;
bool g_onDesktopState = false;
bool g_shownDueToHover = false;

ULONGLONG g_hideDeadline = 0;


// ============================================================
// Desktop detection
// ============================================================

bool IsDesktopWindow(HWND hwnd) {

    if (!hwnd)
        return false;


    WCHAR className[256] = {};

    if (GetClassNameW(
            hwnd,
            className,
            ARRAYSIZE(className)
        ) == 0) {

        return false;
    }


    if (wcscmp(
            className,
            L"Progman"
        ) == 0) {

        return true;
    }


    if (wcscmp(
            className,
            L"WorkerW"
        ) == 0) {

        return FindWindowExW(
                   hwnd,
                   nullptr,
                   L"SHELLDLL_DefView",
                   nullptr
               ) != nullptr;
    }


    return false;
}


// ============================================================
// Explorer / shell window classes
// ============================================================

bool IsShellChromeClass(
    const WCHAR* className
) {

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


    for (
        const WCHAR* shellClass :
        kShellClasses
    ) {

        if (wcscmp(
                className,
                shellClass
            ) == 0) {

            return true;
        }
    }


    return false;
}


// ============================================================
// Find any visible application window
// ============================================================

BOOL CALLBACK EnumWindowsProc(
    HWND hwnd,
    LPARAM lParam
) {

    bool* pFoundOther =
        reinterpret_cast<bool*>(lParam);


    if (!IsWindowVisible(hwnd) ||
        IsIconic(hwnd)) {

        return TRUE;
    }


    /*
        Ignore owned popup windows such as
        tooltips and dropdowns.
    */

    if (GetWindow(
            hwnd,
            GW_OWNER
        ) != nullptr) {

        return TRUE;
    }


    WCHAR className[256] = {};


    /*
        IMPORTANT:
        Check GetClassNameW's return value.
    */

    if (GetClassNameW(
            hwnd,
            className,
            ARRAYSIZE(className)
        ) == 0) {

        return TRUE;
    }


    if (IsShellChromeClass(
            className
        )) {

        return TRUE;
    }


    LONG_PTR exStyle =
        GetWindowLongPtrW(
            hwnd,
            GWL_EXSTYLE
        );


    if (exStyle &
        WS_EX_TOOLWINDOW) {

        return TRUE;
    }


    /*
        Ignore cloaked windows, for example
        applications on another virtual desktop.
    */

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


    RECT rect{};


    if (!GetWindowRect(
            hwnd,
            &rect
        )) {

        return TRUE;
    }


    if (rect.right <= rect.left ||
        rect.bottom <= rect.top) {

        return TRUE;
    }


    *pFoundOther = true;


    /*
        We found one valid visible window.
    */

    return FALSE;
}


bool AnyOtherVisibleWindowExists() {

    bool found = false;


    EnumWindows(
        EnumWindowsProc,
        reinterpret_cast<LPARAM>(
            &found
        )
    );


    return found;
}


// ============================================================
// Foreground-window classification
// ============================================================

bool IsAmbiguousForegroundClass(
    const WCHAR* className
) {

    /*
        The taskbar's base windows can temporarily
        become the foreground window.

        In that case we use EnumWindows to determine
        whether another application is actually active.
    */

    return wcscmp(
               className,
               L"Shell_TrayWnd"
           ) == 0 ||

           wcscmp(
               className,
               L"Shell_SecondaryTrayWnd"
           ) == 0;
}


// ============================================================
// Refresh desktop state
// ============================================================

void RefreshDesktopState() {

    HWND foreground =
        GetForegroundWindow();


    /*
        No foreground window.
    */

    if (!foreground) {

        g_onDesktopState =
            !AnyOtherVisibleWindowExists();

        return;
    }


    /*
        Actual desktop window.
    */

    if (IsDesktopWindow(
            foreground
        )) {

        g_onDesktopState =
            true;

        return;
    }


    /*
        Minimized foreground window.

        Check whether another visible application exists.
    */

    if (IsIconic(
            foreground
        )) {

        g_onDesktopState =
            !AnyOtherVisibleWindowExists();

        return;
    }


    WCHAR className[256] = {};


    if (GetClassNameW(
            foreground,
            className,
            ARRAYSIZE(className)
        ) != 0) {

        if (IsAmbiguousForegroundClass(
                className
            )) {

            g_onDesktopState =
                !AnyOtherVisibleWindowExists();

            return;
        }
    }


    /*
        Any other non-minimized foreground window means
        we are not on the empty desktop.

        This also covers supported shell UI such as
        Start and notification panels.
    */

    g_onDesktopState =
        false;
}


// ============================================================
// Taskbar visibility
// ============================================================

void SetTaskbarVisibility(
    bool show
) {

    HWND hTaskbar =
        FindWindowW(
            L"Shell_TrayWnd",
            nullptr
        );


    if (hTaskbar) {

        ShowWindow(
            hTaskbar,
            show
                ? SW_SHOW
                : SW_HIDE
        );
    }


    if (!settings.hideSecondaryTaskbars)
        return;


    HWND hSecondary = nullptr;


    while (
        (hSecondary =
            FindWindowExW(
                nullptr,
                hSecondary,
                L"Shell_SecondaryTrayWnd",
                nullptr
            )) != nullptr
    ) {

        ShowWindow(
            hSecondary,
            show
                ? SW_SHOW
                : SW_HIDE
        );
    }
}


// ============================================================
// Hover-zone size
// ============================================================

int GetHoverZonePx(
    HWND hTaskbar,
    UINT dpi
) {

    /*
        Convert millimeters to pixels.

        25.4 mm = 1 inch.
    */

    int marginPx =
        MulDiv(
            settings.extraHoverMarginMm,
            static_cast<int>(dpi),
            254
        );


    if (marginPx < 0)
        marginPx = 0;


    /*
        Prefer the actual taskbar height.
    */

    RECT taskbarRect{};


    if (hTaskbar &&
        GetWindowRect(
            hTaskbar,
            &taskbarRect
        )) {

        int taskbarHeight =
            taskbarRect.bottom -
            taskbarRect.top;


        if (taskbarHeight > 0) {

            return taskbarHeight +
                   marginPx;
        }
    }


    /*
        Fallback for the short period where
        Explorer has not created the taskbar yet.
    */

    int fallbackHeight =
        MulDiv(
            48,
            static_cast<int>(dpi),
            96
        );


    return fallbackHeight +
           marginPx;
}


// ============================================================
// Find taskbar belonging to a monitor
// ============================================================

HWND FindTaskbarForMonitor(
    HMONITOR hMonitor
) {

    if (!hMonitor)
        return nullptr;


    /*
        First look for the primary-style taskbar.
    */

    HWND hTaskbar = nullptr;


    while (
        (hTaskbar =
            FindWindowExW(
                nullptr,
                hTaskbar,
                L"Shell_TrayWnd",
                nullptr
            )) != nullptr
    ) {

        RECT rect{};


        if (!GetWindowRect(
                hTaskbar,
                &rect
            )) {

            continue;
        }


        POINT center{
            rect.left +
                (rect.right - rect.left) / 2,

            rect.top +
                (rect.bottom - rect.top) / 2
        };


        if (
            MonitorFromPoint(
                center,
                MONITOR_DEFAULTTONEAREST
            ) == hMonitor
        ) {

            return hTaskbar;
        }
    }


    /*
        Then look for secondary taskbars.
    */

    hTaskbar = nullptr;


    while (
        (hTaskbar =
            FindWindowExW(
                nullptr,
                hTaskbar,
                L"Shell_SecondaryTrayWnd",
                nullptr
            )) != nullptr
    ) {

        RECT rect{};


        if (!GetWindowRect(
                hTaskbar,
                &rect
            )) {

            continue;
        }


        POINT center{
            rect.left +
                (rect.right - rect.left) / 2,

            rect.top +
                (rect.bottom - rect.top) / 2
        };


        if (
            MonitorFromPoint(
                center,
                MONITOR_DEFAULTTONEAREST
            ) == hMonitor
        ) {

            return hTaskbar;
        }
    }


    return nullptr;
}


// ============================================================
// Bottom-edge detection
// ============================================================

bool IsCursorNearBottomEdge() {

    POINT pt{};


    if (!GetCursorPos(
            &pt
        )) {

        return false;
    }


    HMONITOR hMonitor =
        MonitorFromPoint(
            pt,
            MONITOR_DEFAULTTONEAREST
        );


    if (!hMonitor)
        return false;


    MONITORINFO monitorInfo{
        sizeof(monitorInfo)
    };


    if (!GetMonitorInfoW(
            hMonitor,
            &monitorInfo
        )) {

        return false;
    }


    /*
        RECT.right is an exclusive boundary.

        Therefore >= is used here rather than >.
    */

    if (
        pt.x <
            monitorInfo.rcMonitor.left ||

        pt.x >=
            monitorInfo.rcMonitor.right
    ) {

        return false;
    }


    /*
        Find the taskbar belonging to the monitor
        where the mouse currently is.

        This fixes the multi-monitor issue where
        the primary taskbar's height was previously
        used for every monitor.
    */

    HWND hTaskbar =
        FindTaskbarForMonitor(
            hMonitor
        );


    UINT dpi = 96;


    if (hTaskbar) {

        UINT taskbarDpi =
            GetDpiForWindow(
                hTaskbar
            );


        if (taskbarDpi != 0)
            dpi = taskbarDpi;
    }


    int hotZonePx =
        GetHoverZonePx(
            hTaskbar,
            dpi
        );


    if (hotZonePx < 1)
        hotZonePx = 1;


    return pt.y >=
               monitorInfo.rcMonitor.bottom -
                   hotZonePx &&

           pt.y <
               monitorInfo.rcMonitor.bottom;
}


// ============================================================
// Main taskbar state logic
// ============================================================

void UpdateTaskbarState() {

    bool hovering =
        IsCursorNearBottomEdge();


    /*
        --------------------------------------------------------
        APPLICATION / SHELL UI ACTIVE
        --------------------------------------------------------
    */

    if (!g_onDesktopState) {

        /*
            A real window is active.

            Always show immediately.
        */

        g_hideDeadline = 0;

        g_shownDueToHover = false;


        if (g_taskbarHidden) {

            SetTaskbarVisibility(
                true
            );

            g_taskbarHidden =
                false;
        }


        return;
    }


    /*
        --------------------------------------------------------
        DESKTOP + MOUSE IN REVEAL ZONE
        --------------------------------------------------------
    */

    if (hovering) {

        g_hideDeadline = 0;

        g_shownDueToHover = true;


        if (g_taskbarHidden) {

            SetTaskbarVisibility(
                true
            );

            g_taskbarHidden =
                false;
        }


        return;
    }


    /*
        --------------------------------------------------------
        DESKTOP + MOUSE OUTSIDE REVEAL ZONE
        --------------------------------------------------------
    */

    if (g_taskbarHidden) {

        g_hideDeadline = 0;

        return;
    }


    /*
        If the taskbar was visible for another reason
        such as minimizing/closing a window, hide it
        immediately.

        The hover delay is NOT used here.
    */

    if (!g_shownDueToHover) {

        SetTaskbarVisibility(
            false
        );

        g_taskbarHidden =
            true;

        g_hideDeadline = 0;

        return;
    }


    /*
        --------------------------------------------------------
        MOUSE-TRIGGERED HIDE DELAY
        --------------------------------------------------------

        This is the ONLY situation where the delay applies.
    */

    ULONGLONG now =
        GetTickCount64();


    if (g_hideDeadline == 0) {

        g_hideDeadline =
            now +
            settings.autoHideDelayMs;

    }
    else if (
        now >=
        g_hideDeadline
    ) {

        SetTaskbarVisibility(
            false
        );

        g_taskbarHidden =
            true;

        g_hideDeadline = 0;

        g_shownDueToHover =
            false;
    }
}


// ============================================================
// WinEvent callback
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

    UNREFERENCED_PARAMETER(
        hWinEventHook
    );

    UNREFERENCED_PARAMETER(
        hwnd
    );

    UNREFERENCED_PARAMETER(
        idEventThread
    );

    UNREFERENCED_PARAMETER(
        dwmsEventTime
    );


    if (
        event !=
            EVENT_SYSTEM_FOREGROUND ||

        idObject !=
            OBJID_WINDOW ||

        idChild !=
            CHILDID_SELF
    ) {

        return;
    }


    /*
        Immediately react to foreground changes.
    */

    RefreshDesktopState();

    UpdateTaskbarState();
}


// ============================================================
// Timer callback
// ============================================================

void CALLBACK TimerProc(
    HWND hwnd,
    UINT uMsg,
    UINT_PTR idEvent,
    DWORD dwTime
) {

    UNREFERENCED_PARAMETER(
        hwnd
    );

    UNREFERENCED_PARAMETER(
        uMsg
    );

    UNREFERENCED_PARAMETER(
        idEvent
    );

    UNREFERENCED_PARAMETER(
        dwTime
    );


    /*
        Periodic verification catches cases such as:

        - minimizing the last window
        - closing the last window
        - Explorer shell transitions
        - taskbar recreation
    */

    RefreshDesktopState();

    UpdateTaskbarState();
}


// ============================================================
// Worker thread
// ============================================================

DWORD WINAPI HookThread(
    LPVOID param
) {

    UNREFERENCED_PARAMETER(
        param
    );


    /*
        Force creation of this thread's message queue.

        This is done before the worker starts depending
        on message delivery.
    */

    MSG initMsg{};


    PeekMessageW(
        &initMsg,
        nullptr,
        WM_USER,
        WM_USER,
        PM_NOREMOVE
    );


    /*
        Install foreground WinEvent hook.
    */

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
        Initial state.
    */

    RefreshDesktopState();

    UpdateTaskbarState();


    /*
        Poll approximately 10 times per second.
    */

    const UINT kPollIntervalMs =
        100;


    UINT_PTR timerId =
        SetTimer(
            nullptr,
            0,
            kPollIntervalMs,
            TimerProc
        );


    if (!timerId) {

        Wh_Log(
            L"SetTimer failed: %lu",
            GetLastError()
        );
    }


    /*
        Wait for either:

        - the stop event
        - Windows messages

        Using an event instead of PostThreadMessage(WM_QUIT)
        makes shutdown reliable even during early initialization.
    */

    while (true) {

        DWORD waitResult =
            MsgWaitForMultipleObjects(
                1,
                &g_stopEvent,
                FALSE,
                INFINITE,
                QS_ALLINPUT
            );


        /*
            Stop requested.
        */

        if (
            waitResult ==
            WAIT_OBJECT_0
        ) {

            break;
        }


        /*
            Messages are waiting.
        */

        if (
            waitResult ==
            WAIT_OBJECT_0 + 1
        ) {

            MSG msg{};


            while (
                PeekMessageW(
                    &msg,
                    nullptr,
                    0,
                    0,
                    PM_REMOVE
                )
            ) {

                /*
                    If another component posts WM_QUIT,
                    stop cleanly.
                */

                if (
                    msg.message ==
                    WM_QUIT
                ) {

                    SetEvent(
                        g_stopEvent
                    );

                    break;
                }


                TranslateMessage(
                    &msg
                );


                DispatchMessageW(
                    &msg
                );
            }


            if (
                WaitForSingleObject(
                    g_stopEvent,
                    0
                ) ==
                WAIT_OBJECT_0
            ) {

                break;
            }


            continue;
        }


        /*
            Unexpected wait failure.
        */

        Wh_Log(
            L"MsgWaitForMultipleObjects failed: %lu",
            GetLastError()
        );

        break;
    }


    /*
        The worker owns these resources, so it cleans them
        up before the thread exits.
    */

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

        g_hWinEventHookForeground =
            nullptr;
    }


    return 0;
}


// ============================================================
// Settings
// ============================================================

void LoadSettings() {

    settings.extraHoverMarginMm =
        Wh_GetIntSetting(
            L"extraHoverMarginMm"
        );


    settings.autoHideDelayMs =
        static_cast<DWORD>(
            Wh_GetIntSetting(
                L"autoHideDelayMs"
            )
        );


    settings.hideSecondaryTaskbars =
        Wh_GetIntSetting(
            L"hideSecondaryTaskbars"
        ) != 0;


    /*
        Safety limits.
    */

    if (
        settings.extraHoverMarginMm <
        0
    ) {

        settings.extraHoverMarginMm =
            0;
    }


    if (
        settings.extraHoverMarginMm >
        50
    ) {

        settings.extraHoverMarginMm =
            50;
    }


    if (
        settings.autoHideDelayMs >
        10000
    ) {

        settings.autoHideDelayMs =
            10000;
    }
}


// ============================================================
// Windhawk initialization
// ============================================================

BOOL Wh_ModInit() {

    Wh_Log(
        L"Init"
    );


    LoadSettings();


    /*
        Manual-reset stop event.

        The worker waits on this event while also
        processing its message queue.
    */

    g_stopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr
        );


    if (!g_stopEvent) {

        Wh_Log(
            L"CreateEvent failed: %lu",
            GetLastError()
        );

        return FALSE;
    }


    /*
        Start worker thread.
    */

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


        CloseHandle(
            g_stopEvent
        );


        g_stopEvent =
            nullptr;

        g_threadId =
            0;


        return FALSE;
    }


    return TRUE;
}


// ============================================================
// Windhawk uninitialization
// ============================================================

void Wh_ModUninit() {

    Wh_Log(
        L"Uninit"
    );


    /*
        Tell the worker to stop.
    */

    if (g_stopEvent) {

        SetEvent(
            g_stopEvent
        );
    }


    /*
        IMPORTANT:

        Wait until the worker has actually exited before
        closing its handle or allowing the mod to unload.

        This prevents the worker from executing code from
        the unloaded mod.
    */

    if (g_hThread) {

        WaitForSingleObject(
            g_hThread,
            INFINITE
        );


        CloseHandle(
            g_hThread
        );


        g_hThread =
            nullptr;
    }


    g_threadId =
        0;


    if (g_stopEvent) {

        CloseHandle(
            g_stopEvent
        );


        g_stopEvent =
            nullptr;
    }


    /*
        Always restore the taskbar when the mod is disabled.
    */

    SetTaskbarVisibility(
        true
    );


    g_taskbarHidden =
        false;

    g_onDesktopState =
        false;

    g_shownDueToHover =
        false;

    g_hideDeadline =
        0;
}


// ============================================================
// Settings changed
// ============================================================

void Wh_ModSettingsChanged() {

    Wh_Log(
        L"SettingsChanged"
    );


    LoadSettings();


    /*
        The worker's 100 ms polling cycle will pick up
        the new settings.

        No cross-thread message or resource manipulation
        is needed here.
    */
}
