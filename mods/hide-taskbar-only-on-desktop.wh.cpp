// ==WindhawkMod==
// @id              hide-taskbar-only-on-desktop
// @name            Hide Taskbar Only on Desktop
// @description     Hides the taskbar on the desktop, shows it for other windows or when you hover near the bottom edge
// @version         1.0.0
// @author          Sahil Dashoni
// @github          https://github.com/Sahil-Dashoni
// @include         explorer.exe
// @compilerOptions -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Taskbar on Desktop

Hides the Windows taskbar whenever there's nothing else to show (desktop
focused, or every window minimized), and shows it again immediately when
any other window is focused.

You can also peek at the taskbar by moving the mouse to the bottom of the
screen while on the desktop - the reveal zone matches the taskbar's own
height (plus a small extra margin you can configure), not just a thin
strip at the very bottom, so hovering anywhere over where the taskbar
would be counts. The delay before hiding only applies to that peek: hover
in, then move away, and it waits a moment before hiding again. Every other
hide (switching to the desktop, closing or minimizing the last window,
etc.) hides instantly, no delay.

### Notes
- Works with a single taskbar, and optionally with secondary taskbars on
  extra monitors (toggle in the settings).
- Pressing the Windows key or clicking the Start button still works even
  while the taskbar is hidden, and will bring the taskbar back, since the
  Start menu counts as "another window".
- The "on desktop" check re-verifies itself roughly 10 times a second by
  checking for any other visible, non-minimized window, rather than
  depending on a single event. This is what makes minimizing the last
  window hide the taskbar right away, without needing an extra click.
- Opening Start, a system tray flyout ("show hidden icons"), or the
  notification/clock panel keeps the taskbar visible, since those count
  as "another window" too.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- extraHoverMarginMm: 5
  $name: Extra hover margin (mm)
  $description: >-
    The reveal zone at the bottom of the screen automatically matches your
    taskbar's actual height (including your display scaling), so hovering
    anywhere over where the taskbar would be reveals it. This setting adds
    a bit of extra margin above that, in millimeters, so you don't need to
    be pixel-perfect. Default 5mm.
- autoHideDelayMs: 700
  $name: Auto-hide delay after hover (ms)
  $description: >-
    How long to wait, after moving the mouse away from the bottom-edge
    hover zone, before the taskbar hides again. Only applies to that case
    - other hides (e.g. minimizing the last window) are instant.
    Default 700ms.
- hideSecondaryTaskbars: true
  $name: Hide secondary taskbars
  $description: >-
    Also hide/show taskbars on additional monitors, if you have "Show
    taskbar on all displays" enabled in Windows
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>

struct {
    int extraHoverMarginMm;
    DWORD autoHideDelayMs;
    bool hideSecondaryTaskbars;
} settings;

HWINEVENTHOOK g_hWinEventHookForeground;
HANDLE g_hThread;
DWORD g_threadId;

bool g_taskbarHidden = false;    // current actual visibility state
bool g_onDesktopState = false;   // true if "nothing else to show" right now
bool g_shownDueToHover = false;  // taskbar currently shown only because of
                                  // the bottom-edge hover peek
ULONGLONG g_hideDeadline = 0;    // tick count when a pending hover-dismiss
                                  // hide should fire (0 = none pending)

// Returns true if hwnd is the desktop window: either the classic "Progman"
// window, or a "WorkerW" that's actually hosting the desktop icons (rather
// than just being one of the extra wallpaper-helper WorkerW windows).
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
        if (FindWindowExW(hwnd, nullptr, L"SHELLDLL_DefView", nullptr)) {
            return true;
        }
    }

    return false;
}

bool IsShellChromeClass(const WCHAR* className) {
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

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    bool* pFoundOther = (bool*)lParam;

    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) {
        return TRUE;
    }

    // Skip owned windows (tooltips, dropdowns, small popups, etc.).
    if (GetWindow(hwnd, GW_OWNER) != nullptr) {
        return TRUE;
    }

    WCHAR className[256];
    GetClassNameW(hwnd, className, ARRAYSIZE(className));

    if (IsShellChromeClass(className)) {
        return TRUE;
    }

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) {
        return TRUE;
    }

    // Skip cloaked windows (e.g. UWP apps sitting on another virtual
    // desktop, which still report as "visible" otherwise).
    BOOL cloaked = FALSE;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                         sizeof(cloaked))) &&
        cloaked) {
        return TRUE;
    }

    RECT rect;
    if (GetWindowRect(hwnd, &rect)) {
        if (rect.right - rect.left <= 0 || rect.bottom - rect.top <= 0) {
            return TRUE;
        }
    }

    *pFoundOther = true;
    return FALSE;  // found one, no need to keep enumerating
}

// Checks for any real, non-minimized application window currently visible.
bool AnyOtherVisibleWindowExists() {
    bool found = false;
    EnumWindows(EnumWindowsProc, (LPARAM)&found);
    return found;
}

bool IsAmbiguousForegroundClass(const WCHAR* className) {
    // These are the taskbar's own base windows, not a popup on top of it.
    // The OS sometimes leaves one of these as the "foreground" window by
    // default (e.g. right after minimizing the last app) without the user
    // having actually clicked on the taskbar itself, so we can't trust
    // this alone and fall back to checking for other windows.
    return wcscmp(className, L"Shell_TrayWnd") == 0 ||
           wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

// Authoritative check for whether we're in "desktop mode" (hide the
// taskbar). Trusts a genuinely focused, visible window directly - this
// includes Start menu, tray icon flyouts, and the notification/clock
// panel, all of which should keep the taskbar visible. Only falls back to
// scanning for any other visible window when the foreground window itself
// is inconclusive (none, minimized, or the taskbar's own base window) -
// this is what catches minimizing/closing the last real window.
void RefreshDesktopState() {
    HWND fg = GetForegroundWindow();

    if (!fg) {
        g_onDesktopState = !AnyOtherVisibleWindowExists();
        return;
    }

    if (IsDesktopWindow(fg)) {
        g_onDesktopState = true;
        return;
    }

    if (IsIconic(fg)) {
        g_onDesktopState = !AnyOtherVisibleWindowExists();
        return;
    }

    WCHAR className[256];
    if (GetClassNameW(fg, className, ARRAYSIZE(className)) &&
        IsAmbiguousForegroundClass(className)) {
        g_onDesktopState = !AnyOtherVisibleWindowExists();
        return;
    }

    g_onDesktopState = false;
}

void SetTaskbarVisibility(bool show) {
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hTaskbar) {
        ShowWindow(hTaskbar, show ? SW_SHOW : SW_HIDE);
    }

    if (settings.hideSecondaryTaskbars) {
        HWND hSecondary = nullptr;
        while ((hSecondary = FindWindowExW(nullptr, hSecondary,
                                            L"Shell_SecondaryTrayWnd",
                                            nullptr)) != nullptr) {
            ShowWindow(hSecondary, show ? SW_SHOW : SW_HIDE);
        }
    }
}

// The reveal zone matches the taskbar's own current height (whatever size
// and display scaling you actually have it set to), plus a configurable
// extra margin on top. Falls back to a small DPI-scaled default if the
// taskbar's rect can't be read for some reason.
int GetHoverZonePx(HWND hTaskbar, UINT dpi) {
    int marginPx = (int)((double)settings.extraHoverMarginMm / 25.4 * dpi);

    RECT tbRect;
    if (hTaskbar && GetWindowRect(hTaskbar, &tbRect)) {
        int taskbarHeight = tbRect.bottom - tbRect.top;
        if (taskbarHeight > 0) {
            return taskbarHeight + marginPx;
        }
    }

    // Fallback: assume a ~48px (at 100% scaling) default taskbar height.
    int fallbackHeight = (int)(48.0 * dpi / 96.0);
    return fallbackHeight + marginPx;
}

// True if the cursor is within the taskbar-height-sized zone at the bottom
// edge of whichever monitor it's currently on.
bool IsCursorNearBottomEdge() {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return false;
    }

    HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfoW(hMonitor, &mi)) {
        return false;
    }

    if (pt.x < mi.rcMonitor.left || pt.x > mi.rcMonitor.right) {
        return false;
    }

    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    UINT dpi = hTaskbar ? GetDpiForWindow(hTaskbar) : 96;

    int hotZonePx = GetHoverZonePx(hTaskbar, dpi);
    if (hotZonePx < 1) {
        hotZonePx = 1;
    }

    return pt.y >= mi.rcMonitor.bottom - hotZonePx;
}

// Single place that decides whether the taskbar should be shown or hidden.
// Only the "hover then move away" case gets a delay; every other hide is
// instant.
void UpdateTaskbarState() {
    bool hovering = IsCursorNearBottomEdge();

    if (!g_onDesktopState) {
        // A real window is focused: always show, instantly.
        g_hideDeadline = 0;
        g_shownDueToHover = false;
        if (g_taskbarHidden) {
            SetTaskbarVisibility(true);
            g_taskbarHidden = false;
        }
        return;
    }

    // On desktop.
    if (hovering) {
        g_hideDeadline = 0;
        g_shownDueToHover = true;
        if (g_taskbarHidden) {
            SetTaskbarVisibility(true);
            g_taskbarHidden = false;
        }
        return;
    }

    // On desktop, not hovering: the taskbar should end up hidden.
    if (g_taskbarHidden) {
        g_hideDeadline = 0;
        return;
    }

    if (!g_shownDueToHover) {
        // Taskbar was visible for some other reason (a window just got
        // minimized/closed, or desktop just got focused) - hide instantly.
        SetTaskbarVisibility(false);
        g_taskbarHidden = true;
        g_hideDeadline = 0;
        return;
    }

    // Taskbar was visible because of the hover peek, and the mouse has now
    // left the zone - this is the only case that gets a grace period.
    ULONGLONG now = GetTickCount64();
    if (g_hideDeadline == 0) {
        g_hideDeadline = now + settings.autoHideDelayMs;
    } else if (now >= g_hideDeadline) {
        SetTaskbarVisibility(false);
        g_taskbarHidden = true;
        g_hideDeadline = 0;
        g_shownDueToHover = false;
    }
}

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event,
                            HWND hwnd, LONG idObject, LONG idChild,
                            DWORD idEventThread, DWORD dwmsEventTime) {
    if (idObject != OBJID_WINDOW || event != EVENT_SYSTEM_FOREGROUND) {
        return;
    }

    // Quick, instant reaction for the common case (switching to a real
    // window). The poll timer re-verifies everything ~10x/sec regardless,
    // which is what reliably catches the minimize/close-last-window case.
    RefreshDesktopState();
    UpdateTaskbarState();
}

void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent,
                         DWORD dwTime) {
    RefreshDesktopState();
    UpdateTaskbarState();
}

// WINEVENT_OUTOFCONTEXT and a polling SetTimer both need a thread that
// pumps messages, so we spin up a dedicated thread for this instead of
// relying on whichever thread happens to call Wh_ModInit.
DWORD WINAPI HookThread(LPVOID param) {
    g_hWinEventHookForeground = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    RefreshDesktopState();
    UpdateTaskbarState();

    const UINT kPollIntervalMs = 100;
    UINT_PTR timerId = SetTimer(nullptr, 0, kPollIntervalMs, TimerProc);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (timerId) {
        KillTimer(nullptr, timerId);
    }
    if (g_hWinEventHookForeground) {
        UnhookWinEvent(g_hWinEventHookForeground);
        g_hWinEventHookForeground = nullptr;
    }

    return 0;
}

void LoadSettings() {
    settings.extraHoverMarginMm = Wh_GetIntSetting(L"extraHoverMarginMm");
    settings.autoHideDelayMs = (DWORD)Wh_GetIntSetting(L"autoHideDelayMs");
    settings.hideSecondaryTaskbars =
        Wh_GetIntSetting(L"hideSecondaryTaskbars") != 0;
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    g_hThread = CreateThread(nullptr, 0, HookThread, nullptr, 0, &g_threadId);

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_hThread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hThread, 3000);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }

    // Always leave the taskbar visible when the mod is disabled/unloaded.
    if (g_taskbarHidden) {
        SetTaskbarVisibility(true);
        g_taskbarHidden = false;
    }
}

// The mod settings were changed, reload them.
void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}
