// ==WindhawkMod==
// @id              keep-windows-above-taskbar
// @name            Keep Windows Above Taskbar
// @description     Pulls normal app windows back above the taskbar when they open, restore or move below it
// @version         2.3.0
// @author          Goobis
// @github          https://github.com/b2dmx
// @license         MIT
// @include         explorer.exe
// @compilerOptions -luser32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Keep Windows Above Taskbar

Keep Windows Above Taskbar v2.3.0 by Goobis

AI was used to help develop this application

Windows lets app windows open, restore or be dragged so they reach under the
taskbar. This mod watches every normal window (one with a title bar and a sizing
border) in every app, and pulls it back above the taskbar when:

- the window opens,
- you restore it from maximized,
- an app moves or resizes it,
- you finish dragging or resizing it (can be turned off in Settings).

Windows that are already open when the mod starts are checked once too.

If a window is taller than the space above the taskbar, it is shrunk to fit. If
the app refuses to get that small, its top is lined up with the top of the
screen instead. Otherwise it is moved up so its bottom edge sits on the taskbar.

Left alone: maximized windows, fullscreen apps and games, minimized windows,
tool windows, always-on-top windows, menus, tooltips, and windows you have
placed across two monitors.

## How it works (and why it is safe)

The mod runs one small background thread inside Explorer and listens to the
window move events Windows already broadcasts to accessibility tools. It does
not load into, hook or subclass any other app, so it cannot crash them. If an
app keeps fighting the correction, the mod backs off from that window.

You may see a window appear at its old size for a split second before it jumps
into place. That is expected: the correction happens right after Windows moves
the window.

Apps running as administrator cannot be moved by a normal process, so their
windows are not corrected.

The mod measures where the taskbar actually is, so it also works when another
mod (or the auto-hide setting) makes Windows treat the whole screen as usable
space. When the taskbar is slid away (auto-hidden), windows are only kept from
hanging off the bottom edge of the screen.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- correctAfterDrag: true
  $name: Also pull windows back up after I drag or resize them
  $description: Turn off if you want to be able to drag a window below the taskbar yourself.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>

#include <atomic>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <wchar.h>

// IDs spelled out so the mod does not depend on SDK header versions.
static const DWORD kDwmExtendedFrameBounds = 9;  // DWMWA_EXTENDED_FRAME_BOUNDS
static const DWORD kDwmCloaked = 14;             // DWMWA_CLOAKED
static const DWORD kEventObjectUncloaked = 0x8018;  // EVENT_OBJECT_UNCLOAKED

// Back-off for apps that undo the correction.
static const DWORD kRetryWindowMs = 2000;
static const int kMaxCorrectionsPerWindow = 3;  // per kRetryWindowMs
static const int kMaxShrinkAttempts = 2;        // per kRetryWindowMs
static const int kMaxBackoffs = 3;              // then stop for good

static std::atomic<bool> g_correctAfterDrag{true};

static HANDLE g_watcherMutex = nullptr;  // only one Explorer runs the watcher
static HANDLE g_stopEvent = nullptr;
static HANDLE g_readyEvent = nullptr;
static HANDLE g_thread = nullptr;
static DWORD g_threadId = 0;

// ---------------------------------------------------------------------------
// State below is only touched on the watcher thread (out-of-context WinEvent
// callbacks are delivered to the thread that registered them), so it needs no
// locking.
// ---------------------------------------------------------------------------
struct CorrectionHistory {
    DWORD start = 0;
    int count = 0;
    int shrinkCount = 0;
    int backoffs = 0;
};

static HWND g_movingWindow = nullptr;
static std::unordered_map<HWND, CorrectionHistory> g_history;
static std::unordered_set<HWND> g_givenUp;
static std::unordered_set<HWND> g_cannotShrink;
static std::unordered_set<HWND> g_userPlaced;

// Returns true for a normal, captioned, resizable top-level window.
static bool IsNormalWindowStyle(DWORD style, DWORD exStyle) {
    if (style & WS_CHILD) {
        return false;
    }
    if ((style & WS_CAPTION) != WS_CAPTION || !(style & WS_THICKFRAME)) {
        return false;
    }
    if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST)) {
        return false;
    }
    return true;
}

static bool IsCandidateWindow(HWND hWnd) {
    if (!IsWindow(hWnd) || !IsWindowVisible(hWnd)) {
        return false;
    }
    if (IsZoomed(hWnd) || IsIconic(hWnd)) {
        return false;
    }

    DWORD style = (DWORD)GetWindowLongW(hWnd, GWL_STYLE);
    DWORD exStyle = (DWORD)GetWindowLongW(hWnd, GWL_EXSTYLE);
    if (!IsNormalWindowStyle(style, exStyle)) {
        return false;
    }

    // Cloaked windows (other virtual desktops, UWP frames not yet shown) are
    // invisible to the user; they are handled when they uncloak.
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hWnd, kDwmCloaked, &cloaked,
                                        sizeof(cloaked))) &&
        cloaked) {
        return false;
    }

    return true;
}

// Pure geometry. win = full window rect, vis = the part you can see (without
// Windows 10/11's invisible resize border), work = monitor area above the
// taskbar. Returns true and fills *out with the corrected window rect if the
// visible window reaches below the work area and something can be done.
// *shrinks is set when the correction makes the window shorter.
static bool ComputeCorrectedRect(const RECT& win,
                                 const RECT& vis,
                                 const RECT& work,
                                 bool allowShrink,
                                 RECT* out,
                                 bool* shrinks) {
    *shrinks = false;
    if (vis.right <= vis.left || vis.bottom <= vis.top) {
        return false;
    }
    if (vis.bottom <= work.bottom) {
        return false;  // Already above the taskbar.
    }
    if (vis.top < work.top || vis.top >= work.bottom) {
        return false;  // Parked off-screen or above the top: not our business.
    }

    const int visHeight = vis.bottom - vis.top;
    const int workHeight = work.bottom - work.top;

    int newVisTop;
    int newVisBottom;
    if (visHeight <= workHeight) {
        newVisTop = work.bottom - visHeight;  // Fits: move up.
        newVisBottom = work.bottom;
    } else if (allowShrink) {
        newVisTop = work.top;  // Too tall: shrink to fit.
        newVisBottom = work.bottom;
        *shrinks = true;
    } else {
        // Too tall and the app won't shrink: best effort is top-aligned.
        if (vis.top == work.top) {
            return false;
        }
        newVisTop = work.top;
        newVisBottom = work.top + visHeight;
    }

    const int topMargin = vis.top - win.top;
    const int bottomMargin = win.bottom - vis.bottom;

    out->left = win.left;
    out->right = win.right;
    out->top = newVisTop - topMargin;
    out->bottom = newVisBottom + bottomMargin;
    return true;
}

// The part of the window below its monitor's bottom edge lands on another
// monitor: the user is spanning monitors on purpose (stacked displays).
static bool SpansMonitorBelow(const RECT& vis, const RECT& monitorRect) {
    if (vis.bottom <= monitorRect.bottom) {
        return false;
    }
    RECT below = {vis.left, monitorRect.bottom, vis.right, vis.bottom};
    return MonitorFromRect(&below, MONITOR_DEFAULTTONULL) != nullptr;
}

enum class Permission { Allow, AllowWithoutShrink, Deny };

static Permission AskPermission(HWND hWnd, bool wantsShrink) {
    if (g_givenUp.count(hWnd)) {
        return Permission::Deny;
    }

    const DWORD now = GetTickCount();
    CorrectionHistory& h = g_history[hWnd];
    if (h.count == 0 || now - h.start > kRetryWindowMs) {
        h.start = now;
        h.count = 0;
        h.shrinkCount = 0;
    }

    if (h.count >= kMaxCorrectionsPerWindow) {
        // The app keeps undoing us. Back off until the window calms down;
        // after a few rounds, stop for good (until the user drags it).
        if (h.count == kMaxCorrectionsPerWindow) {
            h.count++;  // count each backoff once
            if (++h.backoffs >= kMaxBackoffs) {
                Wh_Log(L"Window %p keeps undoing the correction, giving up",
                       hWnd);
                g_history.erase(hWnd);
                g_givenUp.insert(hWnd);
            }
        }
        return Permission::Deny;
    }

    if (wantsShrink && h.shrinkCount >= kMaxShrinkAttempts) {
        // The app refused to get shorter (minimum height). Remember it.
        g_cannotShrink.insert(hWnd);
        return Permission::AllowWithoutShrink;
    }

    h.count++;
    if (wantsShrink) {
        h.shrinkCount++;
    }
    return Permission::Allow;
}

// ---------------------------------------------------------------------------
// Taskbar position. Windows' "work area" excludes the taskbar only when the
// taskbar reserves space. With auto-hide (or mods that drive auto-hide), the
// work area is the whole screen even while the taskbar is showing, so the
// taskbar windows themselves are measured too.
// ---------------------------------------------------------------------------
static std::vector<HWND> g_taskbars;
static DWORD g_taskbarsRefreshed = 0;
static const DWORD kTaskbarRefreshMs = 5000;

static BOOL CALLBACK CollectTaskbarsProc(HWND hWnd, LPARAM) {
    wchar_t cls[64];
    if (GetClassNameW(hWnd, cls, 64) &&
        (wcscmp(cls, L"Shell_TrayWnd") == 0 ||
         wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0)) {
        g_taskbars.push_back(hWnd);
    }
    return TRUE;
}

static void RefreshTaskbarsIfNeeded() {
    const DWORD now = GetTickCount();
    bool stale = g_taskbars.empty() ||
                 now - g_taskbarsRefreshed > kTaskbarRefreshMs;
    if (!stale) {
        for (HWND h : g_taskbars) {
            if (!IsWindow(h)) {
                stale = true;
                break;
            }
        }
    }
    if (stale) {
        g_taskbars.clear();
        EnumWindows(CollectTaskbarsProc, 0);
        g_taskbarsRefreshed = now;
    }
}

// Pure geometry: if a bottom taskbar is showing on this monitor, make sure the
// work area stops at its top edge.
static void ApplyBottomTaskbar(RECT* work,
                               const RECT& monitor,
                               const RECT& taskbar) {
    const int monitorHeight = monitor.bottom - monitor.top;
    const bool isBottom = taskbar.bottom >= monitor.bottom - 2 &&
                          taskbar.top > monitor.top + monitorHeight / 2 &&
                          (taskbar.right - taskbar.left) >
                              (taskbar.bottom - taskbar.top);
    // An auto-hidden taskbar is slid almost entirely off the screen.
    const bool isShowing = taskbar.top < monitor.bottom - 8;
    if (isBottom && isShowing && taskbar.top < work->bottom) {
        work->bottom = taskbar.top;
    }
}

static RECT EffectiveWorkArea(HMONITOR monitor, const MONITORINFO& mi) {
    RECT work = mi.rcWork;
    RefreshTaskbarsIfNeeded();
    for (HWND h : g_taskbars) {
        if (!IsWindowVisible(h) ||
            MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST) != monitor) {
            continue;
        }
        RECT t;
        if (GetWindowRect(h, &t)) {
            ApplyBottomTaskbar(&work, mi.rcMonitor, t);
        }
    }
    return work;
}

static void CorrectWindow(HWND hWnd) {
    if (!IsCandidateWindow(hWnd)) {
        return;
    }

    RECT win;
    if (!GetWindowRect(hWnd, &win)) {
        return;
    }

    RECT vis;
    if (FAILED(DwmGetWindowAttribute(hWnd, kDwmExtendedFrameBounds, &vis,
                                     sizeof(vis)))) {
        vis = win;
    }

    HMONITOR monitor = MonitorFromRect(&vis, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!monitor || !GetMonitorInfoW(monitor, &mi)) {
        return;
    }

    if (SpansMonitorBelow(vis, mi.rcMonitor)) {
        return;
    }

    const RECT work = EffectiveWorkArea(monitor, mi);
    const bool allowShrink = !g_cannotShrink.count(hWnd);
    RECT target;
    bool shrinks;
    if (!ComputeCorrectedRect(win, vis, work, allowShrink, &target,
                              &shrinks)) {
        return;
    }

    switch (AskPermission(hWnd, shrinks)) {
        case Permission::Deny:
            return;
        case Permission::AllowWithoutShrink:
            if (!ComputeCorrectedRect(win, vis, work, false, &target,
                                      &shrinks)) {
                return;
            }
            break;
        case Permission::Allow:
            break;
    }

    // SWP_ASYNCWINDOWPOS: never wait on the target app, so a hung app cannot
    // stall this thread.
    SetWindowPos(hWnd, nullptr, target.left, target.top,
                 target.right - target.left, target.bottom - target.top,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER |
                     SWP_ASYNCWINDOWPOS);
}

static BOOL CALLBACK SweepProc(HWND hWnd, LPARAM) {
    CorrectWindow(hWnd);
    return TRUE;
}

static void ForgetWindow(HWND hWnd) {
    g_history.erase(hWnd);
    g_givenUp.erase(hWnd);
    g_cannotShrink.erase(hWnd);
    g_userPlaced.erase(hWnd);
    if (g_movingWindow == hWnd) {
        g_movingWindow = nullptr;
    }
}

static void CALLBACK WinEventProc(HWINEVENTHOOK hook,
                                  DWORD event,
                                  HWND hWnd,
                                  LONG idObject,
                                  LONG idChild,
                                  DWORD eventThread,
                                  DWORD eventTime) {
    (void)hook;
    (void)eventThread;
    (void)eventTime;

    if (!hWnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }

    switch (event) {
        case EVENT_SYSTEM_MOVESIZESTART:
            g_movingWindow = hWnd;
            return;

        case EVENT_SYSTEM_MOVESIZEEND:
            if (g_movingWindow == hWnd) {
                g_movingWindow = nullptr;
            }
            // A deliberate drag gets a fresh budget.
            g_history.erase(hWnd);
            g_givenUp.erase(hWnd);
            if (g_correctAfterDrag) {
                g_userPlaced.erase(hWnd);
                CorrectWindow(hWnd);
            } else {
                // Respect where the user put it until it is maximized again.
                g_userPlaced.insert(hWnd);
            }
            return;

        case EVENT_OBJECT_DESTROY:
            ForgetWindow(hWnd);
            return;

        case EVENT_OBJECT_SHOW:
        case EVENT_OBJECT_LOCATIONCHANGE:
        case kEventObjectUncloaked:
            if (hWnd == g_movingWindow) {
                return;  // User is mid-drag.
            }
            if (IsZoomed(hWnd)) {
                g_userPlaced.erase(hWnd);
                return;
            }
            if (g_userPlaced.count(hWnd)) {
                return;
            }
            CorrectWindow(hWnd);
            return;
    }
}

// ---------------------------------------------------------------------------
// Watcher thread
// ---------------------------------------------------------------------------
using SetThreadDpiAwarenessContext_t = HANDLE(WINAPI*)(HANDLE);

static void MakeThreadPerMonitorDpiAware() {
    // Load dynamically so the mod does not depend on SDK header versions.
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        return;
    }
    auto setContext = (SetThreadDpiAwarenessContext_t)(void*)GetProcAddress(
        user32, "SetThreadDpiAwarenessContext");
    if (setContext) {
        // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 == (HANDLE)-4
        setContext((HANDLE)(LONG_PTR)-4);
    }
}

static DWORD WINAPI WatcherThread(LPVOID) {
    // Real (physical) pixel coordinates for every window on every monitor.
    MakeThreadPerMonitorDpiAware();

    // Create this thread's message queue before signaling ready, so a
    // WM_QUIT posted at unload is never lost.
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_readyEvent);

    // Several explorer.exe processes may load this mod. Each one waits here;
    // only the mutex owner watches. If the owner's process exits, the mutex is
    // abandoned and the next Explorer takes over.
    HANDLE waitHandles[2] = {g_stopEvent, g_watcherMutex};
    DWORD waitResult =
        WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);
    if (waitResult != WAIT_OBJECT_0 + 1 &&
        waitResult != WAIT_ABANDONED_0 + 1) {
        return 0;  // Stop requested (or wait failed) before we became owner.
    }

    Wh_Log(L"Watcher running in this Explorer process");

    const DWORD flags = WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNTHREAD;
    HWINEVENTHOOK hooks[4] = {
        SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND,
                        nullptr, WinEventProc, 0, 0, flags),
        SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_SHOW, nullptr,
                        WinEventProc, 0, 0, flags),
        SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE,
                        EVENT_OBJECT_LOCATIONCHANGE, nullptr, WinEventProc, 0,
                        0, flags),
        SetWinEventHook(kEventObjectUncloaked, kEventObjectUncloaked, nullptr,
                        WinEventProc, 0, 0, flags),
    };

    for (HWINEVENTHOOK h : hooks) {
        if (!h) {
            Wh_Log(L"SetWinEventHook failed: %u", GetLastError());
        }
    }

    // Check windows that were already open before the mod started. Skipped
    // when the user wants to keep windows wherever they dragged them.
    if (g_correctAfterDrag) {
        EnumWindows(SweepProc, 0);
    }

    // The stop event may already be set if unload raced with ownership.
    if (WaitForSingleObject(g_stopEvent, 0) != WAIT_OBJECT_0) {
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    for (HWINEVENTHOOK h : hooks) {
        if (h) {
            UnhookWinEvent(h);
        }
    }

    g_history.clear();
    g_givenUp.clear();
    g_cannotShrink.clear();
    g_userPlaced.clear();
    g_movingWindow = nullptr;
    g_taskbars.clear();

    ReleaseMutex(g_watcherMutex);
    return 0;
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------
static void LoadSettings() {
    g_correctAfterDrag = Wh_GetIntSetting(L"correctAfterDrag") != 0;
}

static void CloseHandles() {
    if (g_thread) {
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_watcherMutex) {
        CloseHandle(g_watcherMutex);
        g_watcherMutex = nullptr;
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();

    g_watcherMutex =
        CreateMutexW(nullptr, FALSE, L"KeepWindowsAboveTaskbar_Watcher");
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_watcherMutex || !g_stopEvent || !g_readyEvent) {
        Wh_Log(L"Setup failed: %u", GetLastError());
        CloseHandles();
        return TRUE;
    }

    g_thread = CreateThread(nullptr, 0, WatcherThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseHandles();
        return TRUE;
    }

    WaitForSingleObject(g_readyEvent, 5000);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    if (g_thread) {
        SetEvent(g_stopEvent);
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        // Keep asking until the thread is gone: the DLL must not unload while
        // it still runs. The thread never blocks on other apps, so this ends
        // quickly; re-posting covers the case where its queue did not exist
        // yet on the first try.
        while (WaitForSingleObject(g_thread, 100) == WAIT_TIMEOUT) {
            PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        }
    }
    CloseHandles();
}
