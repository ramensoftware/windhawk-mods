// ==WindhawkMod==
// @id              fancy-taskview-cross-monitor-drag
// @name            Fancy Task View Cross-Monitor Drag
// @description     Move windows between monitors from Windows 11 Task View
// @version         0.7.0
// @author          His Fanciness
// @github          https://github.com/HisFanciness
// @include         windhawk.exe
// @compilerOptions -luser32 -lole32 -loleaut32 -luuid -lshell32 -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fancy Task View Cross-Monitor Drag

Move windows between monitors by dragging their thumbnails in Windows 11 Task View (`Win + Tab`).

## Features
- Drag a Task View window thumbnail from one monitor to another.
- Remember window size and position separately for each monitor.
- Configure first-time window sizing and positioning.
- Choose whether Task View stays open, closes, or closes and reopens after a move.

## Limitations
- Task View may continue displaying the original thumbnail placement until reopened.
- Virtual desktop assignments do not change.
- Elevated or protected windows may not accept movement requests.
- Some applications adjust their own dimensions when changing monitors.
- Window positions are remembered while the mod is running.
- Windows with duplicate titles on the source monitor cannot be identified.
- A thumbnail whose name differs from its window title cannot be identified.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- EnableMovement: true
  $name: Enable cross-monitor dragging
- AfterMove: stay
  $name: After moving a window
  $options:
    - stay: Keep Task View open
    - close: Close Task View
    - reopen: Close and reopen Task View
- GeometryChoice: proportional
  $name: Initial window sizing
  $options:
    - proportional: Scale relative to the monitor
    - pixels: Keep pixel dimensions when possible
    - dpi: Scale according to display DPI
- KeepEdgeAlignment: false
  $name: Preserve nearest-edge alignment
- RememberPerMonitorGeometry: true
  $name: Remember window position and size per monitor
- AllowMaximized: true
  $name: Move maximized windows
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <strsafe.h>
#include <uiautomation.h>
#include <cwchar>
#include <atomic>
#include <new>
#include <shellapi.h>
#include <shellscalingapi.h>

static constexpr UINT kInspect = WM_APP + 0x61;
static constexpr UINT kMove = WM_APP + 0x62;
static std::atomic<bool> g_enableMovement = true;
static std::atomic<bool> g_allowMaximized = true;
static std::atomic<int> g_geometryMode = 0;  // 0 proportional, 1 pixels, 2 DPI
static std::atomic<bool> g_keepEdgeAlignment = false;
static std::atomic<bool> g_rememberPerMonitorGeometry = true;
static std::atomic<int> g_afterMove = 0;   // 0 stay, 1 close, 2 reopen
static HANDLE g_mouseThread = nullptr;
static DWORD g_mouseThreadId = 0;
static HANDLE g_mouseReady = nullptr;
static HHOOK g_mouseHook = nullptr;  // Only used on mouse thread
static LONG g_mouseStatus = 0;
static HANDLE g_worker = nullptr;
static DWORD g_workerId = 0;
static HANDLE g_workerReady = nullptr;
static LONG g_dragSequence = 0;
static bool g_dragging = false;       // Mouse thread only
static HMONITOR g_startMonitor = nullptr;
static HMONITOR g_lastMonitor = nullptr;
static POINT g_startPt = {};
static DWORD g_startTick = 0;
static volatile LONG g_mousePressed = 0;

struct Probe {
    POINT point;
    HMONITOR monitor;
    LONG sequence;
};

struct MoveRequest {
    HWND hwnd;
    HMONITOR source;
    HMONITOR destination;
    LONG sequence;
    DWORD releaseTick;
};

static DWORD ShellProcessId() {
    DWORD pid = 0;
    GetWindowThreadProcessId(GetShellWindow(), &pid);
    return pid;
}

static bool IsShellXamlHost(HWND window) {
    if (!window || !IsWindowVisible(window)) return false;
    WCHAR cls[80] = {};
    if (!GetClassNameW(window, cls, ARRAYSIZE(cls)) ||
        lstrcmpW(cls, L"XamlExplorerHostIslandWindow") != 0) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    const DWORD shellPid = ShellProcessId();
    if (!shellPid || pid != shellPid) return false;
    return true;
}

static bool IsTaskViewHost(HWND window, RECT* bounds = nullptr) {
    if (!IsShellXamlHost(window)) return false;
    RECT rect = {};
    MONITORINFO monitor = {sizeof(monitor)};
    if (!GetWindowRect(window, &rect) ||
        !GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONULL), &monitor))
        return false;
    // Task View covers a monitor; Alt+Tab uses the same class but a smaller host.
    if (rect.left > monitor.rcMonitor.left || rect.top > monitor.rcMonitor.top ||
        rect.right < monitor.rcMonitor.right || rect.bottom < monitor.rcMonitor.bottom)
        return false;
    if (bounds) *bounds = rect;
    return true;
}

struct TaskViewHost {
    HWND hwnd;
};
// Both WinEvent callbacks and the mouse hook run on the mouse thread.
static TaskViewHost g_hosts[32] = {};
static HWINEVENTHOOK g_hostEvents = nullptr;

static void UpdateTaskViewHost(HWND hwnd) {
    const bool host = IsShellXamlHost(hwnd);
    int freeSlot = -1;
    for (int i = 0; i < ARRAYSIZE(g_hosts); ++i) {
        if (g_hosts[i].hwnd == hwnd) {
            g_hosts[i] = host ? TaskViewHost{hwnd} : TaskViewHost{};
            return;
        }
        if (!g_hosts[i].hwnd && freeSlot == -1) freeSlot = i;
    }
    if (host && freeSlot != -1) g_hosts[freeSlot] = {hwnd};
}

static BOOL CALLBACK CacheTaskViewHost(HWND hwnd, LPARAM) {
    UpdateTaskViewHost(hwnd);
    return TRUE;
}

static void CALLBACK HostEvent(HWINEVENTHOOK, DWORD event, HWND hwnd,
                               LONG object, LONG child, DWORD, DWORD) {
    if (!hwnd || object != OBJID_WINDOW || child != CHILDID_SELF) return;
    if (event == EVENT_OBJECT_HIDE || event == EVENT_OBJECT_DESTROY) {
        for (auto& host : g_hosts) if (host.hwnd == hwnd) host = {};
    } else if (event == EVENT_OBJECT_SHOW) {
        UpdateTaskViewHost(hwnd);
    }
}

static HWND TaskViewAt(POINT pt) {
    // Validate only cached shell hosts; no enumeration, window messages or UIA.
    // SHOW can arrive before the host receives its full monitor-sized bounds.
    for (const auto& host : g_hosts) {
        RECT bounds = {};
        if (host.hwnd && IsTaskViewHost(host.hwnd, &bounds) && PtInRect(&bounds, pt))
            return host.hwnd;
    }
    return nullptr;
}

struct WindowMatch {
    const WCHAR* title;
    HMONITOR requiredMonitor;
    HWND hwnd;
    int matches;
};

static BOOL CALLBACK FindTitleMatch(HWND hwnd, LPARAM context) {
    auto* match = reinterpret_cast<WindowMatch*>(context);
    if (!IsWindowVisible(hwnd) || GetWindow(hwnd, GW_OWNER)) return TRUE;
    WCHAR cls[80] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (lstrcmpW(cls, L"XamlExplorerHostIslandWindow") == 0) return TRUE;
    if (MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL) != match->requiredMonitor)
        return TRUE;
    WCHAR title[1024] = {};
    int n = InternalGetWindowText(hwnd, title, ARRAYSIZE(title));
    if (!n || lstrcmpiW(title, match->title) != 0) return TRUE;
    ++match->matches;
    match->hwnd = hwnd;
    return TRUE;
}

static HWND ResolveAtPoint(IUIAutomation* automation, IUIAutomationTreeWalker* walker,
                           const Probe& probe) {
    // Preserve FIFO semantics even if a newer drag started while queued.
    IUIAutomationElement* current = nullptr;
    HRESULT hr = automation->ElementFromPoint(probe.point, &current);
    if (FAILED(hr) || !current) {
        WCHAR note[180] = {};
        StringCchPrintfW(note, ARRAYSIZE(note), L"PROBE #%ld ElementFromPoint failed hr=0x%08lX",
                         probe.sequence, static_cast<unsigned long>(hr));
        Wh_Log(L"%s", note);
        return nullptr;
    }

    HWND unique = nullptr;
    int totalLevels = 0;
    for (int level = 0; level < 12 && current; ++level) {
        ++totalLevels;
        BSTR name = nullptr;
        CONTROLTYPEID type = 0;
        current->get_CurrentControlType(&type);
        HRESULT nameHr = current->get_CurrentName(&name);
        int count = 0;
        HWND candidate = nullptr;
        unsigned long nameLen = 0;
        if (SUCCEEDED(nameHr) && name && name[0]) {
            nameLen = SysStringLen(name);
            // The clicked point may be a descendant of the card. Accept
            // only the nearest ListItem with a unique EXACT window title.
            // Avoid enumerating all app windows for generic parent panels.
            if (type == UIA_ListItemControlTypeId && level <= 4) {
                WindowMatch match = {name, probe.monitor, nullptr, 0};
                EnumWindows(FindTitleMatch, reinterpret_cast<LPARAM>(&match));
                count = match.matches;
                candidate = match.hwnd;
                if (count == 1) {
                    unique = candidate;
                }
            }
        }
        WCHAR note[360] = {};
        StringCchPrintfW(note, ARRAYSIZE(note),
            L"PROBE #%ld level=%d uiaType=%ld nameLength=%lu "
            L"exactSourceMonitorMatches=%d hwnd=%p",
            probe.sequence, level, static_cast<long>(type), nameLen,
            count, count == 1 ? candidate : nullptr);
        Wh_Log(L"%s", note);
        if (name) SysFreeString(name);
        // Stop after identifying the nearest matching ListItem.
        if (unique) {
            current->Release();
            current = nullptr;
            break;
        }
        IUIAutomationElement* parent = nullptr;
        HRESULT parentHr = walker->GetParentElement(current, &parent);
        current->Release();
        current = nullptr;
        if (FAILED(parentHr)) break;
        current = parent;
    }
    if (current) current->Release();
    WCHAR result[250] = {};
    StringCchPrintfW(result, ARRAYSIZE(result),
        L"PROBE #%ld COMPLETE levels=%d uniqueCandidate=%s hwnd=%p",
        probe.sequence, totalLevels,
        unique != nullptr ? L"yes" : L"no",
        unique != nullptr ? unique : nullptr);
    Wh_Log(L"%s", result);
    return unique;
}

static LONG ClampLong(LONG value, LONG lower, LONG upper) {
    if (value < lower) return lower;
    if (value > upper) return upper;
    return value;
}

static LONG RoundSigned(double value) {
    return static_cast<LONG>(value >= 0.0 ? value + 0.5 : value - 0.5);
}

static LONG RectWidth(const RECT& r) { return r.right - r.left; }
static LONG RectHeight(const RECT& r) { return r.bottom - r.top; }

// Kept on the single UIA worker thread. It intentionally does not survive a
// mod restart or retain a window across process recreation.
struct SavedGeometry {
    HWND hwnd;
    DWORD pid;
    HMONITOR monitor;
    RECT rect;
    DWORD lastTick;
    bool used;
};
static SavedGeometry g_savedGeometry[128] = {};

static DWORD ProcessIdFor(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return pid;
}

static void SaveGeometry(HWND hwnd, HMONITOR monitor, const RECT& rect) {
    if (!g_rememberPerMonitorGeometry) return;
    const DWORD pid = ProcessIdFor(hwnd);
    if (!pid) return;
    int slot = -1, oldestSlot = 0;
    DWORD oldestAge = 0;
    const DWORD now = GetTickCount();
    for (int i = 0; i < 128; ++i) {
        auto& rec = g_savedGeometry[i];
        if (rec.used && rec.hwnd == hwnd && rec.pid == pid && rec.monitor == monitor) {
            slot = i;
            break;
        }
        if (!rec.used && slot == -1) slot = i;
        DWORD age = now - rec.lastTick;
        if (age > oldestAge) { oldestAge = age; oldestSlot = i; }
    }
    if (slot == -1) slot = oldestSlot;
    g_savedGeometry[slot] = {hwnd, pid, monitor, rect, now, true};
}

static bool LoadGeometry(HWND hwnd, HMONITOR monitor, RECT* output) {
    if (!g_rememberPerMonitorGeometry) return false;
    const DWORD pid = ProcessIdFor(hwnd);
    for (auto& rec : g_savedGeometry) {
        if (rec.used && rec.hwnd == hwnd && rec.pid == pid && rec.monitor == monitor) {
            *output = rec.rect;
            return true;
        }
    }
    return false;
}

static BOOL CALLBACK CountTaskViewHost(HWND host, LPARAM context) {
    if (IsTaskViewHost(host)) ++(*reinterpret_cast<int*>(context));
    return TRUE;
}
static int TaskViewHostCount() {
    int count = 0;
    EnumWindows(CountTaskViewHost, reinterpret_cast<LPARAM>(&count));
    return count;
}

static bool KeyStroke(WORD key) {
    INPUT inputs[2] = {};
    inputs[0].type = inputs[1].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = inputs[1].ki.wVk = key;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    return SendInput(2, inputs, sizeof(INPUT)) == 2;
}
static bool OpenTaskViewShortcut() {
    INPUT inputs[4] = {};
    for (auto& input : inputs) input.type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_LWIN;
    inputs[1].ki.wVk = VK_TAB;
    inputs[2].ki.wVk = VK_TAB;
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].ki.wVk = VK_LWIN;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    return SendInput(4, inputs, sizeof(INPUT)) == 4;
}

static void ApplyAfterMove(LONG sequence) {
    const int afterMove = g_afterMove;
    if (afterMove == 0) return;
    if (InterlockedCompareExchange(&g_mousePressed, 0, 0) ||
        sequence != InterlockedCompareExchange(&g_dragSequence, 0, 0)) {
        Wh_Log(L"REFRESH skipped, another mouse interaction already started");
        return;
    }
    int before = TaskViewHostCount();
    if (before == 0) {
        Wh_Log(L"REFRESH skipped, Task View already gone");
        return;
    }
    // Only inject hotkeys while Task View's own Explorer process has focus.
    // Otherwise Esc could dismiss an unrelated application's dialog.
    DWORD foregroundPid = 0;
    GetWindowThreadProcessId(GetForegroundWindow(), &foregroundPid);
    if (!foregroundPid || foregroundPid != ShellProcessId()) {
        Wh_Log(L"REFRESH skipped, Task View Explorer process not in foreground");
        return;
    }
    // Do not interfere with user input, held modifiers, or another drag.
    const int heldKeys[] = {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_SHIFT,
                            VK_CONTROL, VK_MENU, VK_LWIN, VK_RWIN};
    for (int key : heldKeys) {
        if (GetAsyncKeyState(key) & 0x8000) {
            Wh_Log(L"REFRESH skipped, key/button currently held");
            return;
        }
    }
    if (InterlockedCompareExchange(&g_mousePressed, 0, 0) ||
        sequence != InterlockedCompareExchange(&g_dragSequence, 0, 0)) {
        Wh_Log(L"REFRESH skipped, new drag before Esc");
        return;
    }
    if (!KeyStroke(VK_ESCAPE)) {
        Wh_Log(L"REFRESH failed, Esc SendInput unsuccessful");
        return;
    }
    int gone = 0;
    for (int i = 0; i < 18; ++i) {
        Sleep(20);
        if (TaskViewHostCount() == 0) { gone = 1; break; }
    }
    if (!gone) {
        Wh_Log(L"AFTER_MOVE Escape did not close Task View; no further action");
        return;
    }
    if (afterMove == 1) {
        Wh_Log(L"AFTER_MOVE Task View closed");
        return;
    }
    // Only reopen after we verified the original interface disappeared.
    Sleep(65);
    if (InterlockedCompareExchange(&g_mousePressed, 0, 0) ||
        sequence != InterlockedCompareExchange(&g_dragSequence, 0, 0)) {
        Wh_Log(L"REFRESH reopen aborted, another gesture started");
        return;
    }
    bool opened = OpenTaskViewShortcut();
    Sleep(140);
    WCHAR note[200] = {};
    StringCchPrintfW(note, ARRAYSIZE(note),
        L"AFTER_MOVE #%ld reopenSent=%s newHosts=%d",
        sequence, opened ? L"yes" : L"no", TaskViewHostCount());
    Wh_Log(L"%s", note);
}

static void MoveWindowToMonitor(const MoveRequest& req) {
    // Give native Task View a chance to finish its mouseup handling.
    Sleep(130);
    if (GetTickCount() - req.releaseTick > 4500) {
        Wh_Log(L"SKIP stale move request (UI Automation queue was delayed)");
        return;
    }
    if (InterlockedCompareExchange(&g_mousePressed, 0, 0)) {
        Wh_Log(L"SKIP another drag began before pending move");
        return;
    }
    WCHAR note[512] = {};
    if (!IsWindow(req.hwnd) || !IsWindowVisible(req.hwnd) ||
        GetWindow(req.hwnd, GW_OWNER) ||
        MonitorFromWindow(req.hwnd, MONITOR_DEFAULTTONULL) != req.source) {
        StringCchPrintfW(note, ARRAYSIZE(note),
            L"SKIP #%ld candidate moved, vanished, or no longer on source", req.sequence);
        Wh_Log(L"%s", note);
        return;
    }
    WCHAR cls[80] = {};
    GetClassNameW(req.hwnd, cls, ARRAYSIZE(cls));
    if (lstrcmpW(cls, L"XamlExplorerHostIslandWindow") == 0) {
        Wh_Log(L"SKIP shell's Task View window");
        return;
    }
    MONITORINFO src = {}, dst = {};
    src.cbSize = sizeof(src);
    dst.cbSize = sizeof(dst);
    if (!GetMonitorInfoW(req.source, &src) || !GetMonitorInfoW(req.destination, &dst)) {
        Wh_Log(L"SKIP monitor information unavailable");
        return;
    }
    WINDOWPLACEMENT placement = {};
    placement.length = sizeof(placement);
    if (!GetWindowPlacement(req.hwnd, &placement) ||
        placement.showCmd == SW_SHOWMINIMIZED || IsIconic(req.hwnd)) {
        Wh_Log(L"SKIP window minimized or placement unavailable");
        return;
    }
    const bool maximized = IsZoomed(req.hwnd) != FALSE;
    if (maximized && !g_allowMaximized) {
        Wh_Log(L"SKIP maximized window movement disabled");
        return;
    }
    bool restored = false;
    if (maximized && g_enableMovement) {
        ShowWindowAsync(req.hwnd, SW_RESTORE);
        for (int i = 0; i < 12 && IsZoomed(req.hwnd); ++i) Sleep(25);
        restored = !IsZoomed(req.hwnd);
        if (!restored) { Wh_Log(L"SKIP restore timed out"); return; }
    }
    RECT oldRect = {};
    if (!GetWindowRect(req.hwnd, &oldRect)) {
        if (restored) ShowWindowAsync(req.hwnd, SW_MAXIMIZE);
        return;
    }
    const RECT& sw = src.rcWork;
    const RECT& dw = dst.rcWork;
    const LONG srcW = RectWidth(sw), srcH = RectHeight(sw);
    const LONG dstW = RectWidth(dw), dstH = RectHeight(dw);
    const LONG w = RectWidth(oldRect), h = RectHeight(oldRect);
    if (srcW < 100 || srcH < 100 || dstW < 100 || dstH < 100 ||
        w < 100 || h < 50 || w > 20000 || h > 20000) {
        Wh_Log(L"SKIP invalid work/window dimensions");
        if (restored) ShowWindowAsync(req.hwnd, SW_MAXIMIZE);
        return;
    }
    UINT srcDpi = 96, srcDpiY = 96;
    GetDpiForMonitor(req.source, MDT_EFFECTIVE_DPI, &srcDpi, &srcDpiY);
    RECT targetRect = {};
    const bool cached = LoadGeometry(req.hwnd, req.destination, &targetRect);
    double scale = 1.0;
    if (!cached) {
        if (g_geometryMode == 0) {
            const double sx = double(dstW) / double(srcW);
            const double sy = double(dstH) / double(srcH);
            scale = sx < sy ? sx : sy;
        } else if (g_geometryMode == 2) {
            UINT dx = 96, dy = 96;
            if (SUCCEEDED(GetDpiForMonitor(req.destination, MDT_EFFECTIVE_DPI, &dx, &dy)) && srcDpi)
                scale = double(dx) / double(srcDpi);
        }
        // Both dimensions use one factor: preserve the outer aspect ratio.
        // Clamp only when too large for destination work area.
        const double fitX = double(dstW) / double(w);
        const double fitY = double(dstH) / double(h);
        if (scale > fitX) scale = fitX;
        if (scale > fitY) scale = fitY;
        const LONG newW = ClampLong(RoundSigned(double(w) * scale), 1, dstW);
        const LONG newH = ClampLong(RoundSigned(double(h) * scale), 1, dstH);
        const LONG travelX = srcW - w, travelY = srcH - h;
        double fx = travelX > 0 ? double(oldRect.left - sw.left) / travelX : 0.5;
        double fy = travelY > 0 ? double(oldRect.top - sw.top) / travelY : 0.5;
        if (fx < 0.0) fx = 0.0; if (fx > 1.0) fx = 1.0;
        if (fy < 0.0) fy = 0.0; if (fy > 1.0) fy = 1.0;
        LONG x = dw.left + RoundSigned(fx * (dstW - newW));
        LONG y = dw.top + RoundSigned(fy * (dstH - newH));
        if (g_keepEdgeAlignment) {
            const LONG leftGap = oldRect.left - sw.left;
            const LONG rightGap = sw.right - oldRect.right;
            const LONG topGap = oldRect.top - sw.top;
            const LONG bottomGap = sw.bottom - oldRect.bottom;
            // Keep a custom PowerToys offset from the nearest edge, and
            // keep an approximately centered window centered.
            const LONG xDiff = leftGap > rightGap ? leftGap - rightGap : rightGap - leftGap;
            const LONG yDiff = topGap > bottomGap ? topGap - bottomGap : bottomGap - topGap;
            if (xDiff <= srcW / 12) x = dw.left + (dstW - newW) / 2;
            else if (leftGap <= rightGap) x = dw.left + leftGap;
            else x = dw.right - newW - rightGap;
            if (yDiff <= srcH / 12) y = dw.top + (dstH - newH) / 2;
            else if (topGap <= bottomGap) y = dw.top + topGap;
            else y = dw.bottom - newH - bottomGap;
        }
        targetRect = {x, y, x + newW, y + newH};
    }
    // Cached geometry may come from a previous taskbar/screen configuration.
    LONG tw = RectWidth(targetRect), th = RectHeight(targetRect);
    if (tw > dstW || th > dstH) {
        const double fitX = double(dstW) / double(tw);
        const double fitY = double(dstH) / double(th);
        const double fit = fitX < fitY ? fitX : fitY;
        tw = ClampLong(RoundSigned(tw * fit), 1, dstW);
        th = ClampLong(RoundSigned(th * fit), 1, dstH);
    }
    const LONG tx = ClampLong(targetRect.left, dw.left, dw.right - tw);
    const LONG ty = ClampLong(targetRect.top, dw.top, dw.bottom - th);
    StringCchPrintfW(note, ARRAYSIZE(note),
        L"PLAN #%ld src=(%ld,%ld %ldx%ld) dst=(%ld,%ld %ldx%ld) mode=%d scale=%.3f cached=%s dpi=%u",
        req.sequence, oldRect.left, oldRect.top, w, h,
        tx, ty, tw, th, g_geometryMode.load(), scale, cached ? L"yes" : L"no", srcDpi);
    Wh_Log(L"%s", note);
    if (!g_enableMovement) {
        Wh_Log(L"DRY_RUN movement disabled");
        if (restored) ShowWindowAsync(req.hwnd, SW_MAXIMIZE);
        return;
    }
    const UINT flags = SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER |
                       SWP_ASYNCWINDOWPOS;
    BOOL ok = SetWindowPos(req.hwnd, nullptr, tx, ty, tw, th, flags);
    DWORD error = ok ? 0 : GetLastError();
    Sleep(125); // allow WM_DPICHANGED and the application's resize handler
    RECT after = {};
    if (ok && GetWindowRect(req.hwnd, &after)) {
        const LONG deltaW = RectWidth(after) - tw;
        const LONG deltaH = RectHeight(after) - th;
        const LONG deltaX = after.left - tx;
        const LONG deltaY = after.top - ty;
        StringCchPrintfW(note, ARRAYSIZE(note),
            L"PASS1 #%ld rect=(%ld,%ld %ldx%ld) delta=(%ld,%ld %ldx%ld) dpi=%u",
            req.sequence, after.left, after.top, RectWidth(after), RectHeight(after),
            deltaX, deltaY, deltaW, deltaH, GetDpiForWindow(req.hwnd));
        Wh_Log(L"%s", note);
        // A single bounded second pass can correct unwanted DPI auto-resizing.
        // It cannot override an app's enforced minimum size.
        if (!maximized && (deltaW > 3 || deltaW < -3 || deltaH > 3 || deltaH < -3 ||
                           deltaX > 3 || deltaX < -3 || deltaY > 3 || deltaY < -3)) {
            SetWindowPos(req.hwnd, nullptr, tx, ty, tw, th, flags);
            Sleep(115);
        }
    }
    if (maximized && restored) {
        ShowWindowAsync(req.hwnd, SW_MAXIMIZE);
        Sleep(90);
    }
    const HMONITOR now = MonitorFromWindow(req.hwnd, MONITOR_DEFAULTTONULL);
    RECT observed = {};
    GetWindowRect(req.hwnd, &observed);
    StringCchPrintfW(note, ARRAYSIZE(note),
        L"MOVE #%ld hwnd=%p result=%s error=%lu targetMatch=%s final=(%ld,%ld %ldx%ld) zoomed=%s dpi=%u",
        req.sequence, req.hwnd, ok ? L"posted" : L"failed", error,
        now == req.destination ? L"yes" : L"no",
        observed.left, observed.top, RectWidth(observed), RectHeight(observed),
        IsZoomed(req.hwnd) ? L"yes" : L"no", GetDpiForWindow(req.hwnd));
    Wh_Log(L"%s", note);
    if (!ok && error == ERROR_ACCESS_DENIED)
        Wh_Log(L"ACCESS_DENIED Windows refused to move the target; often an elevated/protected process. No app-specific override is attempted.");
    if (now == req.destination && ok) {
        SaveGeometry(req.hwnd, req.source, oldRect);
        ApplyAfterMove(req.sequence);
    } else if (ok) {
        Wh_Log(L"WARN movement posted but destination monitor was not observed");
    }
}

static DWORD WINAPI AutomationThread(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    MSG msg = {};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_workerReady);
    HRESULT comHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    bool comOwned = SUCCEEDED(comHr);
    IUIAutomation* automation = nullptr;
    IUIAutomationTreeWalker* walker = nullptr;
    HRESULT hr = FAILED(comHr) ? comHr : CoCreateInstance(
        CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
        IID_IUIAutomation, reinterpret_cast<void**>(&automation));
    if (SUCCEEDED(hr) && automation) hr = automation->get_RawViewWalker(&walker);
    WCHAR note[140] = {};
    StringCchPrintfW(note, ARRAYSIZE(note), L"UIA worker init hr=0x%08lX", static_cast<unsigned long>(hr));
    Wh_Log(L"%s", note);

    HWND inspectedHwnd = nullptr;
    LONG inspectedSequence = -1;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == kInspect) {
            Probe* probe = reinterpret_cast<Probe*>(msg.wParam);
            if (probe) {
                inspectedHwnd = nullptr;
                inspectedSequence = probe->sequence;
                if (SUCCEEDED(hr) && automation && walker)
                    inspectedHwnd = ResolveAtPoint(automation, walker, *probe);
                else Wh_Log(L"SKIP identification unavailable, UIA not initialised");
                delete probe;
            }
        } else if (msg.message == kMove) {
            MoveRequest* req = reinterpret_cast<MoveRequest*>(msg.wParam);
            if (req) {
                // Mouse events enqueue INSPECT before MOVE. FIFO delivery
                // guarantees even a rapid release gets its identification.
                if (req->sequence != inspectedSequence || !inspectedHwnd) {
                    WCHAR diagnostic[200] = {};
                    StringCchPrintfW(diagnostic, ARRAYSIZE(diagnostic),
                        L"SKIP #%ld no identified thumbnail (probeSequence=%ld match=%p)",
                        req->sequence, inspectedSequence, inspectedHwnd);
                    Wh_Log(L"%s", diagnostic);
                } else if (InterlockedCompareExchange(&g_mousePressed, 0, 0)) {
                    Wh_Log(L"SKIP a new drag is already in progress");
                } else {
                    req->hwnd = inspectedHwnd;
                    MoveWindowToMonitor(*req);
                }
                inspectedHwnd = nullptr;
                delete req;
            }
        }
    }
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == kInspect) delete reinterpret_cast<Probe*>(msg.wParam);
        else if (msg.message == kMove) delete reinterpret_cast<MoveRequest*>(msg.wParam);
    }
    if (walker) walker->Release();
    if (automation) automation->Release();
    if (comOwned) CoUninitialize();
    return 0;
}

static LRESULT CALLBACK MouseHook(int code, WPARAM msg, LPARAM param) {
    if (code == HC_ACTION) {
        auto* mouse = reinterpret_cast<const MSLLHOOKSTRUCT*>(param);
        if (mouse) {
            POINT p = mouse->pt;
            if (msg == WM_LBUTTONDOWN && !g_dragging) {
                if (TaskViewAt(p)) {
                    g_dragging = true;
                    g_startPt = p;
                    g_startMonitor = MonitorFromPoint(p, MONITOR_DEFAULTTONEAREST);
                    g_lastMonitor = g_startMonitor;
                    g_startTick = GetTickCount();
                    InterlockedExchange(&g_mousePressed, 1);
                    LONG seq = InterlockedIncrement(&g_dragSequence);
                    Probe* probe = new (std::nothrow) Probe{p, g_startMonitor, seq};
                    if (probe && !PostThreadMessageW(g_workerId, kInspect, reinterpret_cast<WPARAM>(probe), 0))
                        delete probe;
                    Wh_Log(L"DOWN #%ld x=%ld y=%ld", seq, p.x, p.y);
                }
            } else if (g_dragging && msg == WM_MOUSEMOVE) {
                HMONITOR newMon = MonitorFromPoint(p, MONITOR_DEFAULTTONEAREST);
                if (newMon != g_lastMonitor) {
                    Wh_Log(L"CROSS x=%ld y=%ld taskview=%s", p.x, p.y,
                           TaskViewAt(p) ? L"yes" : L"no");
                    g_lastMonitor = newMon;
                }
            } else if (g_dragging && (msg == WM_LBUTTONUP || msg == WM_RBUTTONDOWN)) {
                HMONITOR dest = MonitorFromPoint(p, MONITOR_DEFAULTTONEAREST);
                const bool overTaskView = TaskViewAt(p) != nullptr;
                const DWORD elapsed = GetTickCount() - g_startTick;
                Wh_Log(L"UP x=%ld y=%ld cross=%s taskview=%s durationMs=%lu",
                       p.x, p.y, dest == g_startMonitor ? L"no" : L"yes",
                       overTaskView ? L"yes" : L"no", elapsed);
                const LONG deltaX = p.x - g_startPt.x;
                const LONG deltaY = p.y - g_startPt.y;
                const bool travelled = (deltaX >= 40 || deltaX <= -40 ||
                                        deltaY >= 40 || deltaY <= -40);
                bool validDrop = (msg == WM_LBUTTONUP &&
                    dest != g_startMonitor && overTaskView &&
                    elapsed >= 150 && travelled);
                if (validDrop) {
                    MoveRequest* req = new (std::nothrow) MoveRequest{nullptr, g_startMonitor, dest,
                        InterlockedCompareExchange(&g_dragSequence, 0, 0),
                        GetTickCount()};
                    if (!req || !PostThreadMessageW(g_workerId, kMove,
                            reinterpret_cast<WPARAM>(req), 0)) {
                        delete req;
                        Wh_Log(L"ERROR unable to queue window movement");
                    } else {
                        Wh_Log(L"QUEUE move request queued for post-release processing");
                    }
                } else if (dest != g_startMonitor) {
                    Wh_Log(L"SKIP release rejected: wrong mouse button, Task View closed, or drag too short");
                }
                g_dragging = false;
                InterlockedExchange(&g_mousePressed, 0);
            }
        }
    }
    return CallNextHookEx(g_mouseHook, code, msg, param);
}

static DWORD WINAPI MouseThread(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    MSG msg = {};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_hostEvents = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE,
        nullptr, HostEvent, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (g_hostEvents) EnumWindows(CacheTaskViewHost, 0);
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&MouseHook), &module);
    if (g_hostEvents)
        g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseHook, module, 0);
    InterlockedExchange(&g_mouseStatus, g_mouseHook ? 1 : -1);
    SetEvent(g_mouseReady);
    if (!g_mouseHook) {
        if (g_hostEvents) UnhookWinEvent(g_hostEvents);
        g_hostEvents = nullptr;
        return 1;
    }
    Wh_Log(L"START ordered movement");
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    UnhookWindowsHookEx(g_mouseHook);
    g_mouseHook = nullptr;
    UnhookWinEvent(g_hostEvents);
    g_hostEvents = nullptr;
    for (auto& host : g_hosts) host = {};
    Wh_Log(L"STOP ordered movement");
    return 0;
}

static void LoadSettings() {
    g_enableMovement = Wh_GetIntSetting(L"EnableMovement") != 0;
    g_allowMaximized = Wh_GetIntSetting(L"AllowMaximized") != 0;
    PCWSTR choice = Wh_GetStringSetting(L"GeometryChoice");
    g_geometryMode = (choice && lstrcmpiW(choice, L"pixels") == 0) ? 1 :
                     (choice && lstrcmpiW(choice, L"dpi") == 0) ? 2 : 0;
    if (choice) Wh_FreeStringSetting(choice);
    g_keepEdgeAlignment = Wh_GetIntSetting(L"KeepEdgeAlignment") != 0;
    g_rememberPerMonitorGeometry = Wh_GetIntSetting(L"RememberPerMonitorGeometry") != 0;
    PCWSTR after = Wh_GetStringSetting(L"AfterMove");
    g_afterMove = (after && lstrcmpiW(after, L"close") == 0) ? 1 :
                  (after && lstrcmpiW(after, L"reopen") == 0) ? 2 : 0;
    if (after) Wh_FreeStringSetting(after);
}

static void StopThreads() {
    // Readiness is signalled only after each thread's message queue exists.
    // Stop the producer first, then join the consumer before unloading code.
    if (g_mouseThread) {
        PostThreadMessageW(g_mouseThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_mouseThread, INFINITE);
        CloseHandle(g_mouseThread);
        g_mouseThread = nullptr;
        g_mouseThreadId = 0;
    }
    if (g_worker) {
        PostThreadMessageW(g_workerId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
        g_workerId = 0;
    }
    if (g_workerReady) { CloseHandle(g_workerReady); g_workerReady = nullptr; }
    if (g_mouseReady) { CloseHandle(g_mouseReady); g_mouseReady = nullptr; }
    g_dragging = false;
    InterlockedExchange(&g_mousePressed, 0);
}

BOOL WhTool_ModInit(void) {
    LoadSettings();
    g_workerReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_mouseReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_workerReady || !g_mouseReady) goto fail;
    g_worker = CreateThread(nullptr, 0, AutomationThread, nullptr, 0, &g_workerId);
    if (!g_worker) goto fail;
    WaitForSingleObject(g_workerReady, INFINITE);
    g_mouseThread = CreateThread(nullptr, 0, MouseThread, nullptr, 0, &g_mouseThreadId);
    if (!g_mouseThread) goto fail;
    WaitForSingleObject(g_mouseReady, INFINITE);
    if (InterlockedCompareExchange(&g_mouseStatus, 0, 0) != 1) goto fail;
    return TRUE;
fail:
    StopThreads();
    return FALSE;
}

void WhTool_ModUninit(void) {
    StopThreads();
}

void WhTool_ModSettingsChanged(void) {
    LoadSettings();
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