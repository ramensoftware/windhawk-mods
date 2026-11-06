// ==WindhawkMod==
// @id              keep-windows-clear-of-taskbar
// @name            Keep Windows Clear of Taskbar
// @description     Keeps normal app windows from opening, restoring or being dragged under the taskbar, on any screen edge
// @version         1.0.0
// @author          Goobis
// @github          https://github.com/b2dmx
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -ldwmapi -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Keep Windows Clear of Taskbar

Keep Windows Clear of Taskbar v1.0.0 by Goobis

AI was used to help develop this application

[Support on Ko-fi](https://ko-fi.com/goobis2dmx)

Windows lets app windows open, restore or be dragged so they end up under the
taskbar. This mod keeps every normal window (one with a title bar and a sizing
border) clear of the taskbar, whichever screen edge the taskbar is on: bottom,
top, left or right. A window is corrected when:

- it opens,
- you restore it from maximized,
- an app moves or resizes it,
- you finish dragging or resizing it (can be turned off in Settings).

Windows that are already open are checked too: when the mod starts, and again
whenever the taskbar or the display layout changes (for example when you move
or resize the taskbar, or change resolution or monitors). An auto-hide taskbar
sliding in and out doesn't count, so it never shuffles your windows around.

If a window fits, it is moved just far enough to clear the taskbar. If it is too
big to fit between the taskbar and the opposite screen edge, it is shrunk to
fit; if the app refuses to get that small, it is lined up with the edge next to
the taskbar instead.

Left alone: maximized and fullscreen windows, minimized windows, tool windows,
always-on-top windows, menus, tooltips, windows you have placed across two
monitors, and screen edges without a taskbar or other reserved screen space
(you can still park a window partly off-screen there). Space reserved by docks,
app bars or mods such as Maximized Window Margins counts as reserved too, so
windows are kept clear of it as well.

## Relation to Taskbar Top Window Guard

Taskbar Top Window Guard solves the same problem for a taskbar at the top of the
screen, and adds optional centering of new windows. This mod covers any taskbar
edge, including the bottom, and does not center windows. With a top taskbar,
both mods correct the same windows, so Taskbar Top Window Guard is only worth
adding alongside this one for its centering.

## How it works

The mod runs as a Windhawk tool mod in its own windhawk.exe process. It listens
to the window events Windows already broadcasts to accessibility tools and moves
offending windows asynchronously, so a hung app can never block it. It doesn't
load into, hook or subclass any other process, so it can't crash other apps or
Explorer. If an app keeps fighting the correction, the mod backs off from that
window.

The mod measures where the taskbar actually is, so it also works when another
mod (such as Taskbar auto-hide when maximized) puts the taskbar in auto-hide mode
but keeps it visible.

You may see a window appear at its old size for a split second before it jumps
into place. That is expected: the correction happens right after Windows moves
the window.

Apps running as administrator can't be moved by a normal process, so their
windows are not corrected.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- correctAfterDrag: true
  $name: Also correct windows after I drag or resize them
  $description: Turn off if you want to be able to drag a window under the taskbar yourself. A window you leave under the taskbar is then left alone until it's maximized.
- autoHiddenTaskbar: true
  $name: Keep windows clear of an auto-hidden taskbar while it's showing
  $description: Leave on if another mod keeps an auto-hide taskbar visible. Turn off if your taskbar really auto-hides and you don't want windows to move while it pops up; windows then only avoid the space Windows itself reserves.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <stdio.h>
#include <wchar.h>

#include <atomic>
#include <cstdlib>
#include <unordered_map>
#include <unordered_set>

static const UINT kMsgSettingsChanged = WM_APP + 1;

// Re-check open windows this long after the taskbar or display layout stops
// changing (a taskbar animation or a display change sends a burst of events).
static const UINT_PTR kResweepTimerId = 1;
static const UINT kResweepDelayMs = 400;

// Back-off for apps that undo the correction.
static const DWORD kRetryWindowMs = 2000;
static const int kMaxCorrectionsPerWindow = 3;  // per kRetryWindowMs
static const int kMaxShrinkAttempts = 2;        // per kRetryWindowMs
static const int kMaxBackoffs = 3;              // then stop for good
static const DWORD kQuietResetMs = 60000;       // forget back-offs after this

static std::atomic<bool> g_correctAfterDrag{true};
static std::atomic<bool> g_autoHiddenTaskbar{true};

static HANDLE g_thread = nullptr;
static DWORD g_threadId = 0;
static HANDLE g_readyEvent = nullptr;

// ---------------------------------------------------------------------------
// State below is only touched on the watcher thread (out-of-context WinEvent
// callbacks run on the thread that registered them), so it needs no locking.
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

// Hidden top-level window that receives the work area, display and taskbar
// broadcasts (message-only windows don't get broadcasts).
static HWND g_notifyWindow = nullptr;
static UINT g_taskbarCreatedMsg = 0;

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
    if (IsZoomed(hWnd) || IsIconic(hWnd) || IsHungAppWindow(hWnd)) {
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
    if (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) &&
        cloaked) {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// Pure geometry
// ---------------------------------------------------------------------------

// The area windows must stay inside, and which of its sides are protected
// (have a taskbar or other reserved space). Unprotected sides are the bare
// screen edge and are never enforced.
struct UsableArea {
    RECT rect;
    bool protLeft;
    bool protTop;
    bool protRight;
    bool protBottom;
};

static void InitUsableArea(const RECT& monitor, const RECT& work,
                           UsableArea* area) {
    area->rect = work;
    area->protLeft = work.left > monitor.left;
    area->protTop = work.top > monitor.top;
    area->protRight = work.right < monitor.right;
    area->protBottom = work.bottom < monitor.bottom;
}

// Adds a taskbar rectangle on this monitor. A taskbar slid away by auto-hide
// (almost entirely off-screen) is ignored.
static void ApplyTaskbar(const RECT& monitor, const RECT& t, UsableArea* area) {
    const int monitorWidth = monitor.right - monitor.left;
    const int monitorHeight = monitor.bottom - monitor.top;
    const bool horizontal = (t.right - t.left) > (t.bottom - t.top);
    const int kEdge = 2;
    const int kShowing = 8;

    if (horizontal) {
        if (t.bottom >= monitor.bottom - kEdge &&
            t.top > monitor.top + monitorHeight / 2) {
            if (t.top < monitor.bottom - kShowing) {
                if (t.top < area->rect.bottom) {
                    area->rect.bottom = t.top;
                }
                area->protBottom = true;
            }
        } else if (t.top <= monitor.top + kEdge &&
                   t.bottom < monitor.top + monitorHeight / 2) {
            if (t.bottom > monitor.top + kShowing) {
                if (t.bottom > area->rect.top) {
                    area->rect.top = t.bottom;
                }
                area->protTop = true;
            }
        }
    } else {
        if (t.left <= monitor.left + kEdge &&
            t.right < monitor.left + monitorWidth / 2) {
            if (t.right > monitor.left + kShowing) {
                if (t.right > area->rect.left) {
                    area->rect.left = t.right;
                }
                area->protLeft = true;
            }
        } else if (t.right >= monitor.right - kEdge &&
                   t.left > monitor.left + monitorWidth / 2) {
            if (t.left < monitor.right - kShowing) {
                if (t.left < area->rect.right) {
                    area->rect.right = t.left;
                }
                area->protRight = true;
            }
        }
    }
}

// Fits the span [lo, hi] into [a, b] along one axis, enforcing only protected
// sides. Returns true and fills *outLo/*outHi if something changes.
static bool FitAxis(int lo, int hi, int a, int b, bool protLo, bool protHi,
                    bool allowShrink, int* outLo, int* outHi, bool* shrinks) {
    const bool badLo = protLo && lo < a;
    const bool badHi = protHi && hi > b;
    if (!badLo && !badHi) {
        return false;
    }
    if (hi <= a || lo >= b) {
        return false;  // Not on the usable area at all.
    }
    // Hanging off the bare screen edge before the taskbar side (for example a
    // window's title bar above the top of the screen): parked on purpose.
    if (badHi && !badLo && !protLo && lo < a) {
        return false;
    }

    const int size = hi - lo;
    const int span = b - a;
    int newLo;
    int newHi;

    if (size <= span) {
        newLo = badLo ? a : b - size;
        newHi = newLo + size;
    } else if (badLo && !protHi) {
        newLo = a;  // Too big, but only one side is protected: just slide.
        newHi = a + size;
    } else if (badHi && !protLo) {
        if (allowShrink) {
            newLo = a;
            newHi = b;
            *shrinks = true;
        } else {
            newLo = a;
            newHi = a + size;
        }
    } else {
        // Both sides protected and the window is too big.
        if (allowShrink) {
            newLo = a;
            newHi = b;
            *shrinks = true;
        } else {
            newLo = a;
            newHi = a + size;
        }
    }

    if (newLo == lo && newHi == hi) {
        return false;
    }
    *outLo = newLo;
    *outHi = newHi;
    return true;
}

// win = full window rect, vis = the part you can see (without Windows 10/11's
// invisible resize border). Returns true and fills *out with the corrected
// window rect.
static bool ComputeCorrectedRect(const RECT& win,
                                 const RECT& vis,
                                 const UsableArea& area,
                                 bool allowShrink,
                                 RECT* out,
                                 bool* shrinks) {
    *shrinks = false;
    if (vis.right <= vis.left || vis.bottom <= vis.top) {
        return false;
    }

    int newLeft = vis.left;
    int newRight = vis.right;
    int newTop = vis.top;
    int newBottom = vis.bottom;

    bool changedX = FitAxis(vis.left, vis.right, area.rect.left,
                            area.rect.right, area.protLeft, area.protRight,
                            allowShrink, &newLeft, &newRight, shrinks);
    bool changedY = FitAxis(vis.top, vis.bottom, area.rect.top,
                            area.rect.bottom, area.protTop, area.protBottom,
                            allowShrink, &newTop, &newBottom, shrinks);
    if (!changedX && !changedY) {
        return false;
    }

    out->left = newLeft - (vis.left - win.left);
    out->top = newTop - (vis.top - win.top);
    out->right = newRight + (win.right - vis.right);
    out->bottom = newBottom + (win.bottom - vis.bottom);
    return true;
}

static bool IsFullscreenLike(const RECT& vis, const RECT& monitor) {
    const int kTolerance = 3;
    return std::abs(vis.left - monitor.left) <= kTolerance &&
           std::abs(vis.top - monitor.top) <= kTolerance &&
           std::abs(vis.right - monitor.right) <= kTolerance &&
           std::abs(vis.bottom - monitor.bottom) <= kTolerance;
}

// ---------------------------------------------------------------------------
// Window correction
// ---------------------------------------------------------------------------

// Part of the window hangs past its monitor's edge onto another monitor: the
// user is spanning monitors on purpose.
static bool SpansAnotherMonitor(const RECT& vis, const RECT& monitor) {
    const RECT outside[4] = {
        {vis.left, vis.top, monitor.left, vis.bottom},
        {monitor.right, vis.top, vis.right, vis.bottom},
        {vis.left, vis.top, vis.right, monitor.top},
        {vis.left, monitor.bottom, vis.right, vis.bottom},
    };
    for (const RECT& r : outside) {
        if (r.right > r.left && r.bottom > r.top &&
            MonitorFromRect(&r, MONITOR_DEFAULTTONULL)) {
            return true;
        }
    }
    return false;
}

static UsableArea GetUsableArea(HMONITOR monitor, const MONITORINFO& mi) {
    UsableArea area;
    InitUsableArea(mi.rcMonitor, mi.rcWork, &area);

    // Without auto-hide, the work area already excludes the taskbar. Measuring
    // the taskbar itself only matters when it is in auto-hide mode, which is
    // what this setting is about.
    if (!g_autoHiddenTaskbar) {
        return area;
    }

    auto consider = [&](HWND taskbar) {
        RECT t;
        if (IsWindowVisible(taskbar) &&
            MonitorFromWindow(taskbar, MONITOR_DEFAULTTONEAREST) == monitor &&
            GetWindowRect(taskbar, &t)) {
            ApplyTaskbar(mi.rcMonitor, t, &area);
        }
    };

    if (HWND primary = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        consider(primary);
    }
    for (HWND secondary = nullptr;
         (secondary = FindWindowExW(nullptr, secondary,
                                    L"Shell_SecondaryTrayWnd", nullptr));) {
        consider(secondary);
    }
    return area;
}

enum class Permission { Allow, AllowWithoutShrink, Deny };

static Permission AskPermission(HWND hWnd, bool wantsShrink) {
    if (g_givenUp.count(hWnd)) {
        return Permission::Deny;
    }

    const DWORD now = GetTickCount();
    CorrectionHistory& h = g_history[hWnd];
    if (h.count == 0 || now - h.start > kRetryWindowMs) {
        if (now - h.start > kQuietResetMs) {
            h.backoffs = 0;  // Quiet for a while: earlier fights don't count.
        }
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
        // The app refused to get smaller (minimum size). Remember it.
        g_cannotShrink.insert(hWnd);
        return Permission::AllowWithoutShrink;
    }

    h.count++;
    if (wantsShrink) {
        h.shrinkCount++;
    }
    return Permission::Allow;
}

// Where a window is and where it would have to go. Filled by
// NeedsCorrection.
struct Placement {
    RECT win;
    RECT vis;
    UsableArea area;
    RECT target;
    bool shrinks;
};

// Returns true if the window currently reaches under the taskbar and can be
// corrected.
static bool NeedsCorrection(HWND hWnd, Placement* p) {
    if (!IsCandidateWindow(hWnd)) {
        return false;
    }

    if (!GetWindowRect(hWnd, &p->win)) {
        return false;
    }

    if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS,
                                     &p->vis, sizeof(p->vis)))) {
        p->vis = p->win;
    }

    HMONITOR monitor = MonitorFromRect(&p->vis, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!monitor || !GetMonitorInfoW(monitor, &mi)) {
        return false;
    }

    if (IsFullscreenLike(p->vis, mi.rcMonitor) ||
        SpansAnotherMonitor(p->vis, mi.rcMonitor)) {
        return false;
    }

    p->area = GetUsableArea(monitor, mi);
    const bool allowShrink = !g_cannotShrink.count(hWnd);
    return ComputeCorrectedRect(p->win, p->vis, p->area, allowShrink,
                                &p->target, &p->shrinks);
}

static void CorrectWindow(HWND hWnd) {
    Placement p;
    if (!NeedsCorrection(hWnd, &p)) {
        return;
    }

    RECT target = p.target;
    bool shrinks = p.shrinks;
    switch (AskPermission(hWnd, shrinks)) {
        case Permission::Deny:
            return;
        case Permission::AllowWithoutShrink:
            if (!ComputeCorrectedRect(p.win, p.vis, p.area, false, &target,
                                      &shrinks)) {
                return;
            }
            break;
        case Permission::Allow:
            break;
    }

    // SWP_ASYNCWINDOWPOS: never wait on the target app.
    SetWindowPos(hWnd, nullptr, target.left, target.top,
                 target.right - target.left, target.bottom - target.top,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER |
                     SWP_ASYNCWINDOWPOS);
}

static BOOL CALLBACK SweepProc(HWND hWnd, LPARAM) {
    if (hWnd != g_movingWindow && !g_userPlaced.count(hWnd)) {
        CorrectWindow(hWnd);
    }
    return TRUE;
}

// Re-check all open windows once things settle. Repeated calls restart the
// timer, so a burst of changes leads to a single sweep.
static void ScheduleResweep() {
    if (g_notifyWindow) {
        SetTimer(g_notifyWindow, kResweepTimerId, kResweepDelayMs, nullptr);
    }
}

static bool IsTaskbarWindow(HWND hWnd) {
    WCHAR className[32];
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }
    return wcscmp(className, L"Shell_TrayWnd") == 0 ||
           wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

// Where a fully shown taskbar is docked. Auto-hide slides the taskbar in and
// out without changing any of this, so only a real move or resize changes it.
struct TaskbarDock {
    HMONITOR monitor;
    int edge;  // ABE_LEFT, ABE_TOP, ABE_RIGHT or ABE_BOTTOM
    int thickness;
    int length;

    bool operator==(const TaskbarDock& o) const {
        return monitor == o.monitor && edge == o.edge &&
               thickness == o.thickness && length == o.length;
    }
};

static std::unordered_map<HWND, TaskbarDock> g_taskbarDocks;

// Returns false while the taskbar is partly off-screen (hidden or sliding).
static bool GetTaskbarDock(HWND taskbar, TaskbarDock* dock) {
    RECT t;
    if (!IsWindowVisible(taskbar) || !GetWindowRect(taskbar, &t) ||
        t.right <= t.left || t.bottom <= t.top) {
        return false;
    }
    HMONITOR monitor = MonitorFromRect(&t, MONITOR_DEFAULTTONULL);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!monitor || !GetMonitorInfoW(monitor, &mi)) {
        return false;
    }
    const RECT& m = mi.rcMonitor;
    if (t.left < m.left || t.top < m.top || t.right > m.right ||
        t.bottom > m.bottom) {
        return false;
    }

    const int width = t.right - t.left;
    const int height = t.bottom - t.top;
    dock->monitor = monitor;
    if (width > height) {
        dock->edge = (t.top + t.bottom > m.top + m.bottom) ? ABE_BOTTOM : ABE_TOP;
        dock->thickness = height;
        dock->length = width;
    } else {
        dock->edge = (t.left + t.right > m.left + m.right) ? ABE_RIGHT : ABE_LEFT;
        dock->thickness = width;
        dock->length = height;
    }
    return true;
}

// Called when a taskbar moves or shows. Returns true if it was docked
// somewhere else or resized since it was last fully shown.
static bool TaskbarDockChanged(HWND taskbar) {
    TaskbarDock dock;
    if (!GetTaskbarDock(taskbar, &dock)) {
        return false;
    }
    auto it = g_taskbarDocks.find(taskbar);
    if (it == g_taskbarDocks.end()) {
        // First time seen fully shown: just remember it. A new taskbar after
        // an Explorer restart is handled by TaskbarCreated.
        g_taskbarDocks.emplace(taskbar, dock);
        return false;
    }
    if (it->second == dock) {
        return false;
    }
    it->second = dock;
    return true;
}

static void RecordTaskbarDocks() {
    if (HWND primary = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        TaskbarDockChanged(primary);
    }
    for (HWND secondary = nullptr;
         (secondary = FindWindowExW(nullptr, secondary,
                                    L"Shell_SecondaryTrayWnd", nullptr));) {
        TaskbarDockChanged(secondary);
    }
}

static void ForgetWindow(HWND hWnd) {
    g_taskbarDocks.erase(hWnd);
    g_history.erase(hWnd);
    g_givenUp.erase(hWnd);
    g_cannotShrink.erase(hWnd);
    g_userPlaced.erase(hWnd);
    if (g_movingWindow == hWnd) {
        g_movingWindow = nullptr;
    }
}

static void CALLBACK WinEventProc(HWINEVENTHOOK,
                                  DWORD event,
                                  HWND hWnd,
                                  LONG idObject,
                                  LONG idChild,
                                  DWORD,
                                  DWORD) {
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
                // If the user left it under the taskbar, respect that until
                // it is maximized again. Otherwise keep correcting it as usual.
                Placement p;
                if (NeedsCorrection(hWnd, &p)) {
                    g_userPlaced.insert(hWnd);
                } else {
                    g_userPlaced.erase(hWnd);
                }
            }
            return;

        case EVENT_OBJECT_DESTROY:
            ForgetWindow(hWnd);
            return;

        case EVENT_OBJECT_SHOW:
        case EVENT_OBJECT_LOCATIONCHANGE:
        case EVENT_OBJECT_UNCLOAKED:
            if (IsTaskbarWindow(hWnd)) {
                // In auto-hide mode, moving or resizing the taskbar doesn't
                // change the work area, so watch the taskbar itself. Its
                // auto-hide slides don't count, only a real move or resize.
                if (TaskbarDockChanged(hWnd) && g_autoHiddenTaskbar) {
                    ScheduleResweep();
                }
                return;
            }
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
static const WCHAR kNotifyClassName[] = L"KeepWindowsClearOfTaskbar_Notify";

static LRESULT CALLBACK NotifyWndProc(HWND hWnd,
                                      UINT msg,
                                      WPARAM wParam,
                                      LPARAM lParam) {
    switch (msg) {
        case WM_SETTINGCHANGE:
            if (wParam == SPI_SETWORKAREA) {
                ScheduleResweep();
            }
            return 0;

        case WM_DISPLAYCHANGE:
            ScheduleResweep();
            return 0;

        case WM_TIMER:
            if (wParam == kResweepTimerId) {
                KillTimer(hWnd, kResweepTimerId);
                // Like the startup sweep: with the setting off, windows may
                // have been placed by the user on purpose.
                if (g_correctAfterDrag) {
                    EnumWindows(SweepProc, 0);
                }
                return 0;
            }
            break;

        default:
            if (g_taskbarCreatedMsg && msg == g_taskbarCreatedMsg) {
                // Explorer restarted; the taskbar may be back somewhere else.
                ScheduleResweep();
                return 0;
            }
            break;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

static HINSTANCE GetThisModule() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&NotifyWndProc), &module);
    return module;
}

static void CreateNotifyWindow(HINSTANCE instance) {
    WNDCLASSW wc = {};
    wc.lpfnWndProc = NotifyWndProc;
    wc.hInstance = instance;
    wc.lpszClassName = kNotifyClassName;
    if (!RegisterClassW(&wc)) {
        Wh_Log(L"RegisterClass failed: %u", GetLastError());
        return;
    }

    // A regular (never shown) top-level window, so it receives broadcasts.
    g_notifyWindow =
        CreateWindowExW(WS_EX_TOOLWINDOW, kNotifyClassName, L"", WS_POPUP, 0, 0,
                        0, 0, nullptr, nullptr, instance, nullptr);
    if (!g_notifyWindow) {
        Wh_Log(L"CreateWindow failed: %u", GetLastError());
        UnregisterClassW(kNotifyClassName, instance);
        return;
    }

    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
    // Let these through even if this process runs at a higher integrity level
    // than Explorer.
    ChangeWindowMessageFilterEx(g_notifyWindow, WM_SETTINGCHANGE, MSGFLT_ALLOW,
                                nullptr);
    ChangeWindowMessageFilterEx(g_notifyWindow, WM_DISPLAYCHANGE, MSGFLT_ALLOW,
                                nullptr);
    if (g_taskbarCreatedMsg) {
        ChangeWindowMessageFilterEx(g_notifyWindow, g_taskbarCreatedMsg,
                                    MSGFLT_ALLOW, nullptr);
    }
}

static void DestroyNotifyWindow(HINSTANCE instance) {
    if (g_notifyWindow) {
        KillTimer(g_notifyWindow, kResweepTimerId);
        DestroyWindow(g_notifyWindow);
        g_notifyWindow = nullptr;
        UnregisterClassW(kNotifyClassName, instance);
    }
}

static DWORD WINAPI WatcherThread(LPVOID) {
    // Create the message queue before signaling ready, so the WM_QUIT posted
    // at unload is never lost.
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    const HINSTANCE instance = GetThisModule();
    CreateNotifyWindow(instance);

    const DWORD flags = WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS;
    HWINEVENTHOOK hooks[] = {
        SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND,
                        nullptr, WinEventProc, 0, 0, flags),
        SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_SHOW, nullptr,
                        WinEventProc, 0, 0, flags),
        SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE,
                        EVENT_OBJECT_LOCATIONCHANGE, nullptr, WinEventProc, 0,
                        0, flags),
        SetWinEventHook(EVENT_OBJECT_UNCLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr,
                        WinEventProc, 0, 0, flags),
    };
    for (HWINEVENTHOOK h : hooks) {
        if (!h) {
            Wh_Log(L"SetWinEventHook failed: %u", GetLastError());
        }
    }

    SetEvent(g_readyEvent);

    RecordTaskbarDocks();

    // Check windows that were already open before the mod started.
    if (g_correctAfterDrag) {
        EnumWindows(SweepProc, 0);
    }

    BOOL result;
    while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0 && result != -1) {
        if (!msg.hwnd && msg.message == kMsgSettingsChanged) {
            if (g_correctAfterDrag) {
                // Windows the user placed while the setting was off are no
                // longer exempt.
                g_userPlaced.clear();
                EnumWindows(SweepProc, 0);
            }
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    for (HWINEVENTHOOK h : hooks) {
        if (h) {
            UnhookWinEvent(h);
        }
    }

    DestroyNotifyWindow(instance);

    g_history.clear();
    g_givenUp.clear();
    g_cannotShrink.clear();
    g_userPlaced.clear();
    g_taskbarDocks.clear();
    g_movingWindow = nullptr;
    return 0;
}

static void LoadSettings() {
    g_correctAfterDrag = Wh_GetIntSetting(L"correctAfterDrag") != 0;
    g_autoHiddenTaskbar = Wh_GetIntSetting(L"autoHiddenTaskbar") != 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        return FALSE;
    }

    g_thread = CreateThread(nullptr, 0, WatcherThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }

    WaitForSingleObject(g_readyEvent, INFINITE);
    CloseHandle(g_readyEvent);
    g_readyEvent = nullptr;
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    if (g_threadId) {
        PostThreadMessageW(g_threadId, kMsgSettingsChanged, 0, 0);
    }
}

void WhTool_ModUninit() {
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
        g_threadId = 0;
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
