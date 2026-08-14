// ==WindhawkMod==
// @id              proportional-monitor-cursor
// @name            Proportional cursor between monitors
// @description     Keeps the relative (0-1) cursor position when it moves between monitors of different resolutions, so the bottom of a small monitor matches the bottom of a big one
// @version         1.0.0
// @author          Vitaliy
// @github          https://github.com/vicitacal
// @include         explorer.exe
// @compilerOptions -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Proportional cursor between monitors

Windows moves the mouse cursor between monitors using *absolute* virtual-desktop
pixels. With monitors of different resolutions (for example 2560x1440 next to
1920x1080) this means that leaving the bottom of the small monitor puts the
cursor somewhere in the middle of the big one - the pointer visually "jumps".

This mod makes the transition **proportional**: the position along the shared
edge is converted to a normalized 0-1 value on the monitor you leave, and
applied to the monitor you enter. The bottom of the small monitor now maps to
the bottom of the big one, the top to the top, the middle to the middle.

It works for left/right transitions (the Y coordinate is normalized) and for
top/bottom transitions (the X coordinate is normalized).

**Bonus:** with *Cross gaps between monitors* enabled you can also leave the
monitor through a part of the edge that has no neighbour at all (for example the
lower 360 px of a 1440p monitor when the 1080p monitor is aligned to the top).
Windows normally blocks the cursor there; the mod teleports it to the
proportional spot on the monitor in that direction.

## Notes

* The mod installs a global low-level mouse hook, therefore it runs in a single
  process - `explorer.exe`. Nothing else is patched.
* Injected cursor movement (remote desktop tools, automation, `SetCursorPos`
  from other apps) is left untouched.
* While an application confines the cursor with `ClipCursor` (most full-screen
  games do) the mod stays out of the way.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- normalizeVertical: true
  $name: Normalize vertical position
  $description: >-
    When the cursor moves left/right between monitors, keep its relative
    height (0 = top edge, 1 = bottom edge) instead of the absolute Y pixel.
- normalizeHorizontal: true
  $name: Normalize horizontal position
  $description: >-
    Same for monitors placed above/below each other: keep the relative
    horizontal position instead of the absolute X pixel.
- crossGaps: true
  $name: Cross gaps between monitors
  $description: >-
    Allow the cursor to leave through a part of the edge that has no neighbour
    monitor. It lands on the proportional spot of the nearest monitor in that
    direction instead of being blocked.
- unstickAtEdge: true
  $name: Unstick the cursor at the edge
  $description: >-
    Fallback for the case above, used when the system reports the already
    clamped cursor position. If the cursor stays pinned to the very same edge
    pixel while you keep pushing towards a monitor, it is released.
- verboseLog: false
  $name: Verbose logging
  $description: Write every performed transition to the Windhawk log.
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <vector>

struct {
    std::atomic<bool> normalizeVertical;
    std::atomic<bool> normalizeHorizontal;
    std::atomic<bool> crossGaps;
    std::atomic<bool> unstickAtEdge;
    std::atomic<bool> verboseLog;
} g_settings;

// All of the state below is touched only by the hook thread.
std::vector<RECT> g_monitors;
bool g_monitorsValid;
POINT g_lastPoint;
bool g_lastPointValid;
POINT g_lastDelta;
int g_stuckCount;

HANDLE g_thread;
DWORD g_threadId;
HHOOK g_mouseHook;
HANDLE g_quitEvent;
HANDLE g_queueReadyEvent;

constexpr int kStuckEventsToUnstick = 3;

////////////////////////////////////////////////////////////////////////////////
// Monitor list

BOOL CALLBACK MonitorEnumProc(HMONITOR monitor, HDC, LPRECT, LPARAM) {
    MONITORINFO mi = {sizeof(MONITORINFO)};
    if (GetMonitorInfo(monitor, &mi)) {
        g_monitors.push_back(mi.rcMonitor);
    }
    return TRUE;
}

void RefreshMonitors() {
    g_monitors.clear();
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, 0);
    g_monitorsValid = true;
}

int RectWidth(const RECT& r) {
    return r.right - r.left;
}

int RectHeight(const RECT& r) {
    return r.bottom - r.top;
}

bool PointInRect(const RECT& r, POINT p) {
    return p.x >= r.left && p.x < r.right && p.y >= r.top && p.y < r.bottom;
}

// Index of the monitor containing the point, or -1.
int MonitorAt(POINT p) {
    for (size_t i = 0; i < g_monitors.size(); i++) {
        if (PointInRect(g_monitors[i], p)) {
            return (int)i;
        }
    }
    return -1;
}

// Distance between two 1-D intervals, 0 if they overlap.
int IntervalDistance(int aStart, int aEnd, int bStart, int bEnd) {
    if (bStart >= aEnd) {
        return bStart - aEnd;
    }
    if (aStart >= bEnd) {
        return aStart - bEnd;
    }
    return 0;
}

int Clamp(int value, int low, int high) {
    if (high < low) {
        return low;
    }
    return value < low ? low : (value > high ? high : value);
}

// Maps a coordinate from one monitor edge to another keeping the 0-1 position.
int MapProportional(int value,
                    int fromStart,
                    int fromSize,
                    int toStart,
                    int toSize) {
    if (fromSize <= 1 || toSize <= 1) {
        return toStart;
    }
    return toStart + MulDiv(value - fromStart, toSize - 1, fromSize - 1);
}

////////////////////////////////////////////////////////////////////////////////
// Transition logic

enum Direction { kLeft, kRight, kUp, kDown };

// Picks the closest monitor in the given direction, ignoring `from`.
int FindMonitorInDirection(int from, Direction dir) {
    const RECT& src = g_monitors[from];

    int best = -1;
    int bestScore = 0;

    for (size_t i = 0; i < g_monitors.size(); i++) {
        if ((int)i == from) {
            continue;
        }

        const RECT& cand = g_monitors[i];
        int gap;
        int offAxis;

        switch (dir) {
            case kRight:
                if (cand.left < src.right) {
                    continue;
                }
                gap = cand.left - src.right;
                offAxis =
                    IntervalDistance(src.top, src.bottom, cand.top, cand.bottom);
                break;
            case kLeft:
                if (cand.right > src.left) {
                    continue;
                }
                gap = src.left - cand.right;
                offAxis =
                    IntervalDistance(src.top, src.bottom, cand.top, cand.bottom);
                break;
            case kDown:
                if (cand.top < src.bottom) {
                    continue;
                }
                gap = cand.top - src.bottom;
                offAxis = IntervalDistance(src.left, src.right, cand.left,
                                           cand.right);
                break;
            default:  // kUp
                if (cand.bottom > src.top) {
                    continue;
                }
                gap = src.top - cand.bottom;
                offAxis = IntervalDistance(src.left, src.right, cand.left,
                                           cand.right);
                break;
        }

        int score = gap + offAxis;
        if (best < 0 || score < bestScore) {
            best = (int)i;
            bestScore = score;
        }
    }

    return best;
}

// Computes where the cursor should land after leaving monitor `from` towards
// `dir`. `overshoot` is how far (>= 1 px) past the edge the mouse wanted to go.
bool ComputeTarget(int from,
                   Direction dir,
                   POINT desired,
                   int overshoot,
                   POINT* result) {
    const RECT& src = g_monitors[from];

    int to = MonitorAt(desired);
    if (to == from) {
        to = -1;
    }
    bool throughGap = to < 0;

    if (throughGap) {
        if (!g_settings.crossGaps) {
            return false;
        }
        to = FindMonitorInDirection(from, dir);
        if (to < 0) {
            return false;
        }
    }

    const RECT& dst = g_monitors[to];
    POINT target;

    if (dir == kLeft || dir == kRight) {
        int y = Clamp(desired.y, src.top, src.bottom - 1);
        if (g_settings.normalizeVertical) {
            target.y = MapProportional(y, src.top, RectHeight(src), dst.top,
                                       RectHeight(dst));
        } else {
            target.y = Clamp(y, dst.top, dst.bottom - 1);
        }
        int inset = Clamp(overshoot - 1, 0, RectWidth(dst) - 1);
        target.x = (dir == kRight) ? dst.left + inset : dst.right - 1 - inset;
    } else {
        int x = Clamp(desired.x, src.left, src.right - 1);
        if (g_settings.normalizeHorizontal) {
            target.x = MapProportional(x, src.left, RectWidth(src), dst.left,
                                       RectWidth(dst));
        } else {
            target.x = Clamp(x, dst.left, dst.right - 1);
        }
        int inset = Clamp(overshoot - 1, 0, RectHeight(dst) - 1);
        target.y = (dir == kDown) ? dst.top + inset : dst.bottom - 1 - inset;
    }

    target.x = Clamp(target.x, dst.left, dst.right - 1);
    target.y = Clamp(target.y, dst.top, dst.bottom - 1);

    if (target.x == desired.x && target.y == desired.y) {
        return false;  // Nothing to correct.
    }

    *result = target;
    return true;
}

// True if some application confined the cursor to a part of the desktop.
bool CursorIsClipped() {
    RECT clip;
    if (!GetClipCursor(&clip)) {
        return false;
    }

    RECT desktop = {0, 0, 0, 0};
    for (size_t i = 0; i < g_monitors.size(); i++) {
        const RECT& r = g_monitors[i];
        if (i == 0) {
            desktop = r;
            continue;
        }
        desktop.left = std::min(desktop.left, r.left);
        desktop.top = std::min(desktop.top, r.top);
        desktop.right = std::max(desktop.right, r.right);
        desktop.bottom = std::max(desktop.bottom, r.bottom);
    }

    return clip.left > desktop.left || clip.top > desktop.top ||
           clip.right < desktop.right || clip.bottom < desktop.bottom;
}

// Returns true and fills `result` if the cursor has to be repositioned.
bool HandleMove(POINT desired, POINT* result) {
    if (!g_monitorsValid) {
        RefreshMonitors();
    }

    if (g_monitors.size() < 2) {
        return false;
    }

    if (!g_lastPointValid) {
        g_lastPoint = desired;
        g_lastPointValid = true;
        return false;
    }

    POINT last = g_lastPoint;
    POINT delta = {desired.x - last.x, desired.y - last.y};

    int from = MonitorAt(last);
    if (from < 0) {
        // The previous position is not on any monitor (the display layout
        // changed behind our back), just resync.
        RefreshMonitors();
        return false;
    }

    const RECT& src = g_monitors[from];

    int overshootX = 0;
    if (desired.x >= src.right) {
        overshootX = desired.x - (src.right - 1);
    } else if (desired.x < src.left) {
        overshootX = desired.x - src.left;
    }

    int overshootY = 0;
    if (desired.y >= src.bottom) {
        overshootY = desired.y - (src.bottom - 1);
    } else if (desired.y < src.top) {
        overshootY = desired.y - src.top;
    }

    if (overshootX == 0 && overshootY == 0) {
        // Still on the same monitor. Detect the "pinned to the edge" case: the
        // system already clamped the position for us, so we never see a point
        // outside of the monitor.
        bool sameSpot = delta.x == 0 && delta.y == 0;
        if (sameSpot && g_settings.unstickAtEdge && g_settings.crossGaps) {
            Direction dir = kRight;
            bool atEdge = true;
            if (desired.x == src.right - 1 && g_lastDelta.x > 0) {
                dir = kRight;
            } else if (desired.x == src.left && g_lastDelta.x < 0) {
                dir = kLeft;
            } else if (desired.y == src.bottom - 1 && g_lastDelta.y > 0) {
                dir = kDown;
            } else if (desired.y == src.top && g_lastDelta.y < 0) {
                dir = kUp;
            } else {
                atEdge = false;
            }

            if (atEdge && ++g_stuckCount >= kStuckEventsToUnstick) {
                g_stuckCount = 0;
                if (ComputeTarget(from, dir, desired, 1, result)) {
                    return true;
                }
            }
        } else {
            g_stuckCount = 0;
            if (!sameSpot) {
                g_lastDelta = delta;
            }
        }

        return false;
    }

    g_stuckCount = 0;
    g_lastDelta = delta;

    Direction dir;
    int overshoot;
    if (abs(overshootX) >= abs(overshootY)) {
        dir = overshootX > 0 ? kRight : kLeft;
        overshoot = abs(overshootX);
    } else {
        dir = overshootY > 0 ? kDown : kUp;
        overshoot = abs(overshootY);
    }

    return ComputeTarget(from, dir, desired, overshoot, result);
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION || wParam != WM_MOUSEMOVE) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    const MSLLHOOKSTRUCT* info = (const MSLLHOOKSTRUCT*)lParam;
    POINT desired = info->pt;

    if (info->flags & LLMHF_INJECTED) {
        // Our own SetCursorPos or someone else's - never process it, but keep
        // the reference point up to date.
        g_lastPoint = desired;
        g_lastPointValid = true;
        g_stuckCount = 0;
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    POINT target;
    if (HandleMove(desired, &target) && !CursorIsClipped()) {
        if (g_settings.verboseLog) {
            Wh_Log(L"(%ld, %ld) -> (%ld, %ld)", desired.x, desired.y, target.x,
                   target.y);
        }

        g_lastPoint = target;
        g_lastPointValid = true;
        SetCursorPos(target.x, target.y);
        return 1;  // Swallow the original move.
    }

    g_lastPoint = desired;
    g_lastPointValid = true;
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Hook thread

LRESULT CALLBACK NotifyWndProc(HWND hWnd,
                               UINT uMsg,
                               WPARAM wParam,
                               LPARAM lParam) {
    if (uMsg == WM_DISPLAYCHANGE || uMsg == WM_SETTINGCHANGE ||
        uMsg == WM_DPICHANGED) {
        g_monitorsValid = false;
        g_lastPointValid = false;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HMODULE GetCurrentModule() {
    HMODULE module = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&LowLevelMouseProc, &module);
    return module;
}

void MakeThreadDpiAware() {
    using SetThreadDpiAwarenessContext_t = HANDLE(WINAPI*)(HANDLE);
    HMODULE user32 = GetModuleHandle(L"user32.dll");
    if (!user32) {
        return;
    }
    auto setContext = (SetThreadDpiAwarenessContext_t)GetProcAddress(
        user32, "SetThreadDpiAwarenessContext");
    if (setContext) {
        // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2, so that all coordinates
        // we deal with are real physical pixels.
        setContext((HANDLE)-4);
    }
}

// A global hook must be installed exactly once. `explorer.exe` may run as
// several processes (e.g. when folder windows are launched separately), so only
// the instance that owns the shell takes the job.
bool IsShellProcess() {
    for (int i = 0; i < 120; i++) {
        HWND shellWnd = GetShellWindow();
        if (shellWnd) {
            DWORD pid = 0;
            GetWindowThreadProcessId(shellWnd, &pid);
            return pid == GetCurrentProcessId();
        }

        // The shell window isn't up yet, we were injected very early.
        if (WaitForSingleObject(g_quitEvent, 500) == WAIT_OBJECT_0) {
            return false;
        }
    }

    return true;  // Gave up waiting, assume we're the one.
}

DWORD WINAPI HookThreadProc(LPVOID) {
    MakeThreadDpiAware();

    // Force the message queue to be created, so that Wh_ModUninit can post to
    // this thread at any point from now on.
    MSG msg;
    PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);
    SetEvent(g_queueReadyEvent);

    if (!IsShellProcess()) {
        Wh_Log(L"Not the shell process, the hook is not installed here");
        return 0;
    }

    HMODULE module = GetCurrentModule();

    WNDCLASS wc = {};
    wc.lpfnWndProc = NotifyWndProc;
    wc.hInstance = module;
    wc.lpszClassName = L"WindhawkProportionalCursorNotify";
    RegisterClass(&wc);

    // Not a message-only window on purpose: those don't receive the broadcast
    // WM_DISPLAYCHANGE notification.
    HWND notifyWnd =
        CreateWindowEx(WS_EX_TOOLWINDOW, wc.lpszClassName, L"", WS_POPUP, 0, 0,
                       0, 0, nullptr, nullptr, module, nullptr);

    g_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, module, 0);
    if (!g_mouseHook) {
        Wh_Log(L"SetWindowsHookEx failed: %u", GetLastError());
    }

    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }

    if (notifyWnd) {
        DestroyWindow(notifyWnd);
    }
    UnregisterClass(wc.lpszClassName, module);

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Mod entry points

void LoadSettings() {
    g_settings.normalizeVertical = Wh_GetIntSetting(L"normalizeVertical");
    g_settings.normalizeHorizontal = Wh_GetIntSetting(L"normalizeHorizontal");
    g_settings.crossGaps = Wh_GetIntSetting(L"crossGaps");
    g_settings.unstickAtEdge = Wh_GetIntSetting(L"unstickAtEdge");
    g_settings.verboseLog = Wh_GetIntSetting(L"verboseLog");
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();
    return TRUE;
}

void Wh_ModAfterInit() {
    g_quitEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_queueReadyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_quitEvent || !g_queueReadyEvent) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        return;
    }

    g_thread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_threadId);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_thread) {
        SetEvent(g_quitEvent);
        WaitForSingleObject(g_queueReadyEvent, 2000);
        PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 10000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }

    if (g_quitEvent) {
        CloseHandle(g_quitEvent);
        g_quitEvent = nullptr;
    }
    if (g_queueReadyEvent) {
        CloseHandle(g_queueReadyEvent);
        g_queueReadyEvent = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}
