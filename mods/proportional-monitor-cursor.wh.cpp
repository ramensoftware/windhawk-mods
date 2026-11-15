// ==WindhawkMod==
// @id              proportional-monitor-cursor
// @name            Proportional cursor between monitors
// @description     Keeps the relative (0-1) cursor position when it moves between monitors of different resolutions, so the bottom of a small monitor matches the bottom of a big one
// @version         1.0.0
// @author          Vitaliy
// @github          https://github.com/vicitacal
// @include         windhawk.exe
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

```
      leaving 1920x1080 at y = 1000        entering 2560x1440
      (1000 / 1079 = 0.93 of the height)   (0.93 * 1439 = y 1334)

      +--------------------+ 0.00          +--------------------+ 0.00
      |                    |               |                    |
      |                    |               |                    |
      |                    |               |                    |
      |                    |               |   without the mod  |
      |                    |               | ->  y stays 1000   | 0.69
      |                    |               |                    |
      | ->  y = 1000       | 0.93          | ->  y = 1334       | 0.93
      +--------------------+ 1.00          |                    |
                                           +--------------------+ 1.00
```

It works for left/right transitions (the Y coordinate is normalized) and for
top/bottom transitions (the X coordinate is normalized).

**Bonus:** with *Cross gaps between monitors* enabled you can also leave the
monitor through a part of the edge that has no neighbour at all (for example the
lower 360 px of a 1440p monitor when the 1080p monitor is aligned to the top).
Windows normally blocks the cursor there; the mod teleports it to the
proportional spot on the monitor in that direction.

## Notes

* The mod needs neither injection nor function hooks, so it runs as a tool mod
  in a dedicated `windhawk.exe` process. Its low-level mouse hook therefore
  can't stall the shell, and Windhawk guarantees a single instance.
* Cursor movement reported as injected (`LLMHF_INJECTED`) is left untouched -
  it can't be told apart from the mod's own `SetCursorPos`. Some setups report
  regular movement that way, RDP sessions, VM guest tools and a few tablets and
  precision touchpads among them; on those the mod stays inactive.
* While an application confines the cursor with `ClipCursor` (most full-screen
  games do) the mod stays out of the way.
* When a window of an elevated process is in the foreground, low-level hooks
  from a non-elevated process don't receive input. That's a UIPI restriction,
  not something the mod can work around.
  
Contact with me: vicitacal11@ya.ru
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
    direction instead of being blocked. Such an edge no longer stops the
    cursor, so an auto-hide taskbar sitting on it can't be revealed by pushing
    into it any more.
- unstickAtEdge: true
  $name: Unstick the cursor at the edge
  $description: >-
    Fallback for the option above, used when the system reports the already
    clamped cursor position. If the cursor stays pinned to the very same edge
    pixel while you keep pushing towards a monitor, it is released. Has no
    effect unless "Cross gaps between monitors" is enabled.
*/
// ==/WindhawkModSettings==

#include <shellapi.h>

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <vector>

struct {
    std::atomic<bool> normalizeVertical;
    std::atomic<bool> normalizeHorizontal;
    std::atomic<bool> crossGaps;
    std::atomic<bool> unstickAtEdge;
} g_settings;

// All of the state below is touched only by the hook thread.
std::vector<RECT> g_monitors;
bool g_monitorsResyncDone;
POINT g_lastPoint;
bool g_lastPointValid;
POINT g_lastDelta;
int g_stuckCount;

HANDLE g_hookThread;
DWORD g_hookThreadId;
HANDLE g_queueReadyEvent;
HHOOK g_mouseHook;

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

// Distance between two half-open 1-D intervals, 0 only when they really
// overlap. Intervals that merely touch - two monitors sharing nothing but a
// corner - come out one apart rather than zero.
int IntervalDistance(int aStart, int aEnd, int bStart, int bEnd) {
    if (bStart >= aEnd) {
        return bStart - aEnd + 1;
    }
    if (aStart >= bEnd) {
        return aStart - bEnd + 1;
    }
    return 0;
}

// Distance from a coordinate to a half-open span, 0 if it falls inside.
int DistanceToSpan(LONG value, LONG spanStart, LONG spanEnd) {
    if (value < spanStart) {
        return spanStart - value;
    }
    if (value >= spanEnd) {
        return value - spanEnd + 1;
    }
    return 0;
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

// Picks the best monitor in the given direction, ignoring `from`. A monitor
// whose perpendicular span overlaps the source one wins over a closer one that
// is offset to the side, so that the obvious neighbour is always preferred.
// `cross` is the position along the edge the cursor is leaving through, used to
// break ties instead of leaving them to the enumeration order.
int FindMonitorInDirection(int from, Direction dir, LONG cross) {
    const RECT& src = g_monitors[from];

    int best = -1;
    int bestGap = 0;
    int bestOffAxis = 0;
    int bestCrossDist = 0;

    for (size_t i = 0; i < g_monitors.size(); i++) {
        if ((int)i == from) {
            continue;
        }

        const RECT& cand = g_monitors[i];
        int gap;
        int offAxis;
        int crossDist;

        switch (dir) {
            case kRight:
                if (cand.left < src.right) {
                    continue;
                }
                gap = cand.left - src.right;
                offAxis = IntervalDistance(src.top, src.bottom, cand.top,
                                           cand.bottom);
                crossDist = DistanceToSpan(cross, cand.top, cand.bottom);
                break;
            case kLeft:
                if (cand.right > src.left) {
                    continue;
                }
                gap = src.left - cand.right;
                offAxis = IntervalDistance(src.top, src.bottom, cand.top,
                                           cand.bottom);
                crossDist = DistanceToSpan(cross, cand.top, cand.bottom);
                break;
            case kDown:
                if (cand.top < src.bottom) {
                    continue;
                }
                gap = cand.top - src.bottom;
                offAxis = IntervalDistance(src.left, src.right, cand.left,
                                           cand.right);
                crossDist = DistanceToSpan(cross, cand.left, cand.right);
                break;
            default:  // kUp
                if (cand.bottom > src.top) {
                    continue;
                }
                gap = src.top - cand.bottom;
                offAxis = IntervalDistance(src.left, src.right, cand.left,
                                           cand.right);
                crossDist = DistanceToSpan(cross, cand.left, cand.right);
                break;
        }

        bool better;
        if (best < 0) {
            better = true;
        } else if ((offAxis == 0) != (bestOffAxis == 0)) {
            better = offAxis == 0;
        } else if (gap != bestGap) {
            better = gap < bestGap;
        } else if (offAxis != bestOffAxis) {
            better = offAxis < bestOffAxis;
        } else {
            better = crossDist < bestCrossDist;
        }

        if (better) {
            best = (int)i;
            bestGap = gap;
            bestOffAxis = offAxis;
            bestCrossDist = crossDist;
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
    bool horizontal = dir == kLeft || dir == kRight;

    // Position along the edge the cursor is leaving through.
    LONG cross = horizontal ? std::clamp(desired.y, src.top, src.bottom - 1)
                            : std::clamp(desired.x, src.left, src.right - 1);

    int to = MonitorAt(desired);
    if (to == from) {
        to = -1;
    }

    if (to < 0) {
        // The cursor is heading into a part of the edge with no neighbour.
        if (!g_settings.crossGaps) {
            return false;
        }
        to = FindMonitorInDirection(from, dir, cross);
        if (to < 0) {
            return false;
        }
    }

    const RECT& dst = g_monitors[to];
    POINT target;

    if (horizontal) {
        if (g_settings.normalizeVertical) {
            target.y = MapProportional(cross, src.top, RectHeight(src), dst.top,
                                       RectHeight(dst));
        } else {
            target.y = std::clamp(cross, dst.top, dst.bottom - 1);
        }

        // `overshoot` is measured from the edge of the source monitor, so it
        // only reconstructs the reported position when the two monitors touch
        // along this axis. Whenever the reported position is already valid on
        // the destination monitor - a fast flick across an intermediate
        // monitor, or a gap between the two - keep it as is.
        if (desired.x >= dst.left && desired.x < dst.right) {
            target.x = desired.x;
        } else {
            int inset = std::clamp(overshoot - 1, 0, RectWidth(dst) - 1);
            target.x =
                (dir == kRight) ? dst.left + inset : dst.right - 1 - inset;
        }
    } else {
        if (g_settings.normalizeHorizontal) {
            target.x = MapProportional(cross, src.left, RectWidth(src),
                                       dst.left, RectWidth(dst));
        } else {
            target.x = std::clamp(cross, dst.left, dst.right - 1);
        }

        if (desired.y >= dst.top && desired.y < dst.bottom) {
            target.y = desired.y;
        } else {
            int inset = std::clamp(overshoot - 1, 0, RectHeight(dst) - 1);
            target.y = (dir == kDown) ? dst.top + inset : dst.bottom - 1 - inset;
        }
    }

    target.x = std::clamp(target.x, dst.left, dst.right - 1);
    target.y = std::clamp(target.y, dst.top, dst.bottom - 1);

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

    int left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int right = left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int bottom = top + GetSystemMetrics(SM_CYVIRTUALSCREEN);

    return clip.left > left || clip.top > top || clip.right < right ||
           clip.bottom < bottom;
}

// The cursor sits still at an edge of its monitor while the last movement was
// heading out through that very edge - the system clamped it for us. Returns
// the direction the user is pushing towards.
bool GetStuckDirection(const RECT& src, POINT at, Direction* dir) {
    // Check the axis the last movement was dominated by first, so that a
    // diagonal movement into a corner doesn't pick the wrong edge.
    bool preferX = abs(g_lastDelta.x) >= abs(g_lastDelta.y);

    for (int pass = 0; pass < 2; pass++) {
        if ((pass == 0) == preferX) {
            if (at.x == src.right - 1 && g_lastDelta.x > 0) {
                *dir = kRight;
                return true;
            }
            if (at.x == src.left && g_lastDelta.x < 0) {
                *dir = kLeft;
                return true;
            }
        } else {
            if (at.y == src.bottom - 1 && g_lastDelta.y > 0) {
                *dir = kDown;
                return true;
            }
            if (at.y == src.top && g_lastDelta.y < 0) {
                *dir = kUp;
                return true;
            }
        }
    }

    return false;
}

// The cursor can only move along one axis while it is pinned against an edge,
// so the two axes are remembered independently. Replacing the whole delta would
// let a 1 px wobble at the edge erase the direction the user is pushing in.
void UpdateLastDelta(POINT delta) {
    if (delta.x != 0) {
        g_lastDelta.x = delta.x;
    }
    if (delta.y != 0) {
        g_lastDelta.y = delta.y;
    }
}

// Returns true and fills `result` if the cursor has to be repositioned.
bool HandleMove(POINT desired, POINT* result) {
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
        // The previous position is not on any monitor - the display layout
        // changed behind our back. Resync once; if the point stays off every
        // monitor, don't re-enumerate on every further mouse event.
        if (!g_monitorsResyncDone) {
            g_monitorsResyncDone = true;
            RefreshMonitors();
        }
        return false;
    }

    g_monitorsResyncDone = false;

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
        // Still on the same monitor.
        if (delta.x != 0 || delta.y != 0) {
            UpdateLastDelta(delta);
            g_stuckCount = 0;
            return false;
        }

        if (!g_settings.unstickAtEdge || !g_settings.crossGaps) {
            g_stuckCount = 0;
            return false;
        }

        Direction dir;
        if (!GetStuckDirection(src, desired, &dir)) {
            g_stuckCount = 0;
            return false;
        }

        if (++g_stuckCount < kStuckEventsToUnstick) {
            return false;
        }

        g_stuckCount = 0;
        return ComputeTarget(from, dir, desired, 1, result);
    }

    g_stuckCount = 0;
    UpdateLastDelta(delta);

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
        // the reference point up to date. The cursor was moved for reasons of
        // its own, so the movement history no longer describes anything.
        g_lastPoint = desired;
        g_lastPointValid = true;
        g_lastDelta = {};
        g_stuckCount = 0;
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    POINT target;
    if (HandleMove(desired, &target) && !CursorIsClipped()) {
        Wh_Log(L"(%ld, %ld) -> (%ld, %ld)", desired.x, desired.y, target.x,
               target.y);

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
    // Runs on the hook thread, same as LowLevelMouseProc, so the monitor list
    // can be rebuilt right here instead of on the input path.
    if (uMsg == WM_DISPLAYCHANGE) {
        Wh_Log(L"Display change");
        RefreshMonitors();
        g_monitorsResyncDone = false;
        // Nothing collected under the previous layout still applies.
        g_lastPointValid = false;
        g_lastDelta = {};
        g_stuckCount = 0;
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

DWORD WINAPI HookThreadProc(LPVOID) {
    MakeThreadDpiAware();

    // Force the message queue to be created, and only then let WhTool_ModUninit
    // post to this thread.
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_queueReadyEvent);

    RefreshMonitors();

    HMODULE module = GetCurrentModule();

    WNDCLASS wc = {};
    wc.lpfnWndProc = NotifyWndProc;
    wc.hInstance = module;
    wc.lpszClassName = L"WindhawkProportionalCursorNotify";

    // Never reuse a class a previous instance left behind: its window procedure
    // would point into an unmapped image.
    ATOM classAtom = RegisterClass(&wc);
    HWND notifyWnd = nullptr;
    if (classAtom) {
        // Not a message-only window on purpose: those don't receive the
        // broadcast WM_DISPLAYCHANGE notification.
        notifyWnd = CreateWindowEx(WS_EX_TOOLWINDOW, wc.lpszClassName, L"",
                                   WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
                                   module, nullptr);
        if (!notifyWnd) {
            Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        }
    } else {
        Wh_Log(L"RegisterClass failed: %u", GetLastError());
    }

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

    if (classAtom) {
        UnregisterClass(wc.lpszClassName, module);
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Mod entry points

void LoadSettings() {
    g_settings.normalizeVertical = Wh_GetIntSetting(L"normalizeVertical");
    g_settings.normalizeHorizontal = Wh_GetIntSetting(L"normalizeHorizontal");
    g_settings.crossGaps = Wh_GetIntSetting(L"crossGaps");
    g_settings.unstickAtEdge = Wh_GetIntSetting(L"unstickAtEdge");
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    g_queueReadyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_queueReadyEvent) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        return FALSE;
    }

    g_hookThread =
        CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_hookThreadId);
    if (!g_hookThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseHandle(g_queueReadyEvent);
        g_queueReadyEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_hookThread) {
        // The queue is ready by now, so WM_QUIT can't be lost and the wait
        // below is guaranteed to return.
        WaitForSingleObject(g_queueReadyEvent, INFINITE);
        PostThreadMessage(g_hookThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        g_hookThreadId = 0;
    }

    if (g_queueReadyEvent) {
        CloseHandle(g_queueReadyEvent);
        g_queueReadyEvent = nullptr;
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
