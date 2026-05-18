// ==WindhawkMod==
// @id              monocle-wm
// @name            Monocle Window Manager
// @description     Only one window visible at a time. Auto-minimizes non-focused windows, force-maximizes the focused one
// @version         2.0.0
// @author          Tederby
// @include         explorer.exe
// @compilerOptions -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Monocle Window Manager

A tiling-inspired mod that enforces a **monocle layout**: only one application
window is visible at any given time.

## How it works

- When you focus a window, **all other app windows are minimized** and the
  focused window is **force-maximized**.
- When you manually minimize an app (via taskbar or minimize button), you
  simply see the desktop — no other window pops up.
- **Win+D** shows the desktop. Pressing **Win+D** again restores and focuses
  the most recent app.

## Settings

| Setting | Default | Description |
|---------|---------|-------------|
| Maximize new window | Yes | Force-maximize the newly focused window |
| Ignore fullscreen apps | Yes | Don't interfere with exclusive-fullscreen apps |

## Known Issues

- **Elevated (admin) apps are not managed.** Windows with elevated privileges
  (e.g. Task Manager, Registry Editor, installers run as Administrator) cannot
  be minimized or maximized by this mod. This is a Windows security restriction
  called **UIPI (User Interface Privilege Isolation)** — processes running at
  normal integrity level (like `explorer.exe`) are blocked from manipulating
  windows of higher-integrity processes. This is by design and cannot be
  bypassed without running the mod itself at elevated privileges.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- MaximizeNewWindow: true
  $name: Maximize new window
  $description: Force-maximize the newly focused window if it supports maximizing
- IgnoreFullscreen: true
  $name: Ignore fullscreen apps
  $description: Don't minimize or maximize windows that are in exclusive fullscreen mode
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>

// ---------------------------------------------------------------------------
// Global state
// ---------------------------------------------------------------------------

struct {
    bool maximizeNewWindow;
    bool ignoreFullscreen;
} g_settings;

// Worker thread handles
static HANDLE g_hThread    = nullptr;
static DWORD  g_dwThreadId = 0;

// Event hooks (owned by worker thread)
static HWINEVENTHOOK g_hFgHook  = nullptr;

// Last valid app the user interacted with (survives desktop transitions)
static HWND g_hwndLastActive = nullptr;

// Single-instance guard
static HANDLE g_hMutex  = nullptr;
static bool   g_isOwner = false;

// Custom message to tell the worker thread to exit
#define WM_MONOCLE_QUIT (WM_USER + 0x4D4F)  // "MO"

// Pending maximize (deferred via timer to avoid restore-race)
static HWND      g_hwndPendingMaximize = nullptr;
static UINT_PTR  g_maximizeTimerId     = 0;

// ---------------------------------------------------------------------------
// Window class ignore-list
// ---------------------------------------------------------------------------

static const wchar_t* const g_ignoredClasses[] = {
    // Taskbar
    L"Shell_TrayWnd",
    L"Shell_SecondaryTrayWnd",

    // Desktop
    L"Progman",
    L"WorkerW",

    // Start Menu / Search / Action Center
    L"Windows.UI.Core.CoreWindow",

    // Notification area
    L"NotifyIconOverflowWindow",
    L"TopLevelWindowForOverflowXamlIsland",

    // Explorer XAML islands
    L"XamlExplorerHostIslandWindow",

    // Task-switching / virtual desktop UI
    L"MultitaskingViewFrame",
    L"ForegroundStaging",

    // Shell infrastructure
    L"ApplicationManager_DesktopShellWindow",
    L"EdgeUiInputTopWndClass",
    L"EdgeUiInputWndClass",
    L"NativeHWNDHost",
    L"SearchPane",
    L"LockScreenBackstopFrame",

    // Win11 XAML / composition
    L"InputNonClientPointerSource",
    L"Xaml_WindowedPopupClass",

    // Tooltip & IME
    L"tooltips_class32",
    L"IME",
    L"MSCTFIME UI",

    // Windhawk itself
    L"WindhawkUI",

    nullptr  // sentinel
};

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static bool IsIgnoredClass(HWND hwnd) {
    wchar_t className[256] = {};
    if (GetClassNameW(hwnd, className, _countof(className)) == 0)
        return true;

    for (int i = 0; g_ignoredClasses[i]; ++i) {
        if (_wcsicmp(className, g_ignoredClasses[i]) == 0)
            return true;
    }
    return false;
}

static bool IsCloaked(HWND hwnd) {
    BOOL cloaked = FALSE;
    HRESULT hr = DwmGetWindowAttribute(
        hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    return SUCCEEDED(hr) && cloaked;
}

static bool IsFullscreenWindow(HWND hwnd) {
    HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (!hMon) return false;

    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfoW(hMon, &mi)) return false;

    RECT rc;
    if (!GetWindowRect(hwnd, &rc)) return false;

    return rc.left   <= mi.rcMonitor.left  &&
           rc.top    <= mi.rcMonitor.top   &&
           rc.right  >= mi.rcMonitor.right &&
           rc.bottom >= mi.rcMonitor.bottom;
}

/// Returns true for "real" application windows we should manage.
static bool IsValidAppWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd))  return false;
    if (!IsWindowVisible(hwnd))    return false;
    if (IsIgnoredClass(hwnd))      return false;
    if (IsCloaked(hwnd))           return false;

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW)  return false;
    if (exStyle & WS_EX_NOACTIVATE)  return false;

    // Must have a title
    if (GetWindowTextLengthW(hwnd) == 0) return false;

    // Skip tiny owned popups (tooltips, menus)
    HWND owner = GetWindow(hwnd, GW_OWNER);
    if (owner) {
        RECT rc;
        if (GetWindowRect(hwnd, &rc)) {
            int w = rc.right - rc.left;
            int h = rc.bottom - rc.top;
            if (w < 200 || h < 100) return false;
        }
    }

    // Fullscreen guard
    if (g_settings.ignoreFullscreen && IsFullscreenWindow(hwnd))
        return false;

    return true;
}

/// Can this window be maximized?
static bool CanMaximize(HWND hwnd) {
    LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    return (style & WS_MAXIMIZEBOX) != 0;
}

/// Timer callback: maximize the pending window after a short delay.
/// The delay lets the window fully settle after restoring from minimized
/// state, preventing the "covers taskbar" glitch.
static void CALLBACK MaximizeTimerProc(
    HWND /*hWnd*/, UINT /*uMsg*/, UINT_PTR idTimer, DWORD /*dwTime*/)
{
    KillTimer(nullptr, idTimer);
    g_maximizeTimerId = 0;

    HWND hwnd = g_hwndPendingMaximize;
    g_hwndPendingMaximize = nullptr;

    if (!hwnd || !IsWindow(hwnd))         return;
    if (GetForegroundWindow() != hwnd)    return;  // user already moved on
    if (!CanMaximize(hwnd))               return;

    WINDOWPLACEMENT wp = { sizeof(wp) };
    if (!GetWindowPlacement(hwnd, &wp))   return;
    if (wp.showCmd == SW_SHOWMAXIMIZED)   return;  // already maximized

    // Use SetWindowPlacement for an atomic state transition that
    // properly respects the monitor work area (taskbar).
    wp.showCmd = SW_SHOWMAXIMIZED;
    SetWindowPlacement(hwnd, &wp);

    Wh_Log(L"[Monocle] Deferred maximize applied to 0x%08X",
           (unsigned)(ULONG_PTR)hwnd);
}

// ---------------------------------------------------------------------------
// Core monocle actions
// ---------------------------------------------------------------------------

/// EnumWindows callback: minimize every valid app window except `lParam`.
static BOOL CALLBACK MinimizeAllExceptProc(HWND hwnd, LPARAM lParam) {
    HWND except = (HWND)lParam;
    if (hwnd == except)          return TRUE;
    if (IsIconic(hwnd))          return TRUE;  // already minimized
    if (!IsValidAppWindow(hwnd)) return TRUE;

    ShowWindow(hwnd, SW_MINIMIZE);
    return TRUE;
}

static void MinimizeAllExcept(HWND except) {
    EnumWindows(MinimizeAllExceptProc, (LPARAM)except);
}

// ---------------------------------------------------------------------------
// WinEvent callback
// ---------------------------------------------------------------------------

static void CALLBACK WinEventProc(
    HWINEVENTHOOK /*hWinEventHook*/,
    DWORD         event,
    HWND          hwnd,
    LONG          idObject,
    LONG          idChild,
    DWORD         /*idEventThread*/,
    DWORD         /*dwmsEventTime*/)
{
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF)
        return;

    // ── EVENT_SYSTEM_FOREGROUND ─────────────────────────────────────────
    if (event == EVENT_SYSTEM_FOREGROUND) {

        // Focus went to a non-app window (desktop, taskbar, start menu…)
        // → do nothing, let the user see the desktop.
        if (!IsValidAppWindow(hwnd)) {
            Wh_Log(L"[Monocle] Focus → non-app (0x%08X), ignoring",
                   (unsigned)(ULONG_PTR)hwnd);
            return;
        }

        // Same window re-activated → skip.
        if (hwnd == g_hwndLastActive) return;

        Wh_Log(L"[Monocle] Focus → app 0x%08X", (unsigned)(ULONG_PTR)hwnd);

        // 1) Minimize ALL other valid app windows.
        MinimizeAllExcept(hwnd);

        // 2) Force-maximize the focused window (deferred by 50ms).
        //    The delay prevents the "maximize past taskbar" glitch that
        //    occurs when the window is still mid-restore from minimized.
        if (g_settings.maximizeNewWindow && CanMaximize(hwnd)) {
            // Cancel any previous pending maximize.
            if (g_maximizeTimerId) {
                KillTimer(nullptr, g_maximizeTimerId);
                g_maximizeTimerId = 0;
            }
            g_hwndPendingMaximize = hwnd;
            g_maximizeTimerId = SetTimer(nullptr, 0, 50, MaximizeTimerProc);
        }

        // 3) Remember this window.
        g_hwndLastActive = hwnd;
    }
}

// ---------------------------------------------------------------------------
// Worker thread — runs a message loop so the WinEvent hook actually fires
// ---------------------------------------------------------------------------

static DWORD WINAPI WorkerThreadProc(LPVOID /*lpParam*/) {
    Wh_Log(L"[Monocle] Worker thread started (tid %u)", GetCurrentThreadId());

    // Install the event hook on THIS thread (which has a message pump).
    g_hFgHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND,
        EVENT_SYSTEM_FOREGROUND,
        nullptr,
        WinEventProc,
        0, 0,
        WINEVENT_OUTOFCONTEXT
    );

    if (!g_hFgHook) {
        Wh_Log(L"[Monocle] SetWinEventHook failed (err %u)", GetLastError());
        return 1;
    }

    Wh_Log(L"[Monocle] Event hook installed — monocle mode active");

    // ── Message loop ────────────────────────────────────────────────────
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.message == WM_MONOCLE_QUIT) break;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // ── Cleanup ─────────────────────────────────────────────────────────
    if (g_hFgHook) {
        UnhookWinEvent(g_hFgHook);
        g_hFgHook = nullptr;
    }

    Wh_Log(L"[Monocle] Worker thread exiting");
    return 0;
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

static void LoadSettings() {
    g_settings.maximizeNewWindow = Wh_GetIntSetting(L"MaximizeNewWindow");
    g_settings.ignoreFullscreen  = Wh_GetIntSetting(L"IgnoreFullscreen");
}

// ---------------------------------------------------------------------------
// Windhawk lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"[Monocle WM] Initializing (v2.0)...");

    // ── Single-instance guard ───────────────────────────────────────────
    // explorer.exe may have several instances; only one should run monocle.
    g_hMutex = CreateMutexW(nullptr, TRUE,
                            L"Global\\MonocleWM_SingleInstance");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        Wh_Log(L"[Monocle WM] Another instance active — becoming passive");
        if (g_hMutex) { CloseHandle(g_hMutex); g_hMutex = nullptr; }
        g_isOwner = false;
        return TRUE;  // stay loaded but idle
    }
    g_isOwner = true;

    LoadSettings();

    // Snapshot the current foreground window.
    HWND fg = GetForegroundWindow();
    if (IsValidAppWindow(fg)) {
        g_hwndLastActive = fg;
    }

    // ── Spawn worker thread with message loop ───────────────────────────
    g_hThread = CreateThread(
        nullptr, 0, WorkerThreadProc, nullptr, 0, &g_dwThreadId);
    if (!g_hThread) {
        Wh_Log(L"[Monocle WM] CreateThread failed (err %u)", GetLastError());
        return FALSE;
    }

    Wh_Log(L"[Monocle WM] Init complete — worker thread %u", g_dwThreadId);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"[Monocle WM] Shutting down...");

    if (g_isOwner && g_hThread) {
        // Tell the worker thread to exit.
        PostThreadMessageW(g_dwThreadId, WM_MONOCLE_QUIT, 0, 0);
        if (WaitForSingleObject(g_hThread, 3000) == WAIT_TIMEOUT) {
            Wh_Log(L"[Monocle WM] Worker thread didn't exit in time");
        }
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }

    if (g_hMutex) {
        ReleaseMutex(g_hMutex);
        CloseHandle(g_hMutex);
        g_hMutex = nullptr;
    }

    g_hwndLastActive = nullptr;
    Wh_Log(L"[Monocle WM] Shut down cleanly");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"[Monocle WM] Settings changed — reloading");
    LoadSettings();
}
