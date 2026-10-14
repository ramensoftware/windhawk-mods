// ==WindhawkMod==
// @id              fancy-taskview-cross-monitor-drag
// @name            Fancy Task View Cross-Monitor Drag
// @description     Move windows between monitors from Windows 11 Task View
// @version         0.7.0
// @author          His Fanciness
// @github          https://github.com/HisFanciness
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lole32 -loleaut32 -luuid
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
- Window positions are remembered for the current Explorer session.

Diagnostic log: `%TEMP%\FancyTaskViewMove.log`.
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
- DetailedLogging: false
  $name: Enable detailed logging
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <strsafe.h>
#include <uiautomation.h>
#include <cwchar>

static constexpr UINT kInspect = WM_APP + 0x61;
static constexpr UINT kMove = WM_APP + 0x62;
static constexpr UINT kTrace = WM_APP + 0x63;
static bool g_enableMovement = true;
static bool g_allowMaximized = true;
static int g_geometryMode = 0;  // 0 proportional, 1 pixels, 2 DPI
static bool g_keepEdgeAlignment = false;
static bool g_rememberPerMonitorGeometry = true;
static int g_afterMove = 0;   // 0 stay, 1 close, 2 reopen
static bool g_detailedLogging = false;
static HANDLE g_mouseThread = nullptr;
static DWORD g_mouseThreadId = 0;
static HANDLE g_mouseReady = nullptr;
static HHOOK g_mouseHook = nullptr;  // Only used on mouse thread
static LONG g_mouseStatus = 0;
static HANDLE g_worker = nullptr;
static DWORD g_workerId = 0;
static HANDLE g_workerReady = nullptr;
static WCHAR g_logPath[MAX_PATH] = {};
static LONG g_dragSequence = 0;
static bool g_dragging = false;       // Mouse thread only
static HMONITOR g_startMonitor = nullptr;
static HMONITOR g_lastMonitor = nullptr;
static POINT g_startPt = {};
static DWORD g_startTick = 0;
// The identification result belongs to the ordered UIA worker, not the mouse
// hook. v0.5.1's shared g_candidate caused missed fast drags.
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
    DWORD durationMs;
    POINT dropPoint;
    POINT startPoint;
    DWORD releaseTick;
};

struct Trace {
    WCHAR message[320];
};

// A WH_MOUSE_LL callback must finish promptly. File I/O stays off the
// mouse hook thread to prevent Windows silently timing out our hook.
static void TraceFromMouseHook(const WCHAR* note) {
    Trace* item = new Trace{};
    if (!item) return;
    StringCchCopyW(item->message, ARRAYSIZE(item->message), note);
    if (!PostThreadMessageW(g_workerId, kTrace,
                            reinterpret_cast<WPARAM>(item), 0)) delete item;
}

struct Locate {
    POINT pt;
    HWND found;
};

static BOOL CALLBACK FindTaskView(HWND window, LPARAM p) {
    Locate* lookup = reinterpret_cast<Locate*>(p);
    if (!IsWindowVisible(window)) return TRUE;
    WCHAR cls[80] = {}, text[80] = {};
    if (GetClassNameW(window, cls, ARRAYSIZE(cls)) == 0 ||
        lstrcmpW(cls, L"XamlExplorerHostIslandWindow") != 0 ||
        GetWindowTextW(window, text, ARRAYSIZE(text)) == 0 ||
        lstrcmpW(text, L"Task View") != 0) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    RECT rect = {};
    if (!GetWindowRect(window, &rect) || !PtInRect(&rect, lookup->pt)) return TRUE;
    lookup->found = window;
    return FALSE;
}

static HWND TaskViewAt(POINT pt) {
    Locate lookup = {pt, nullptr};
    EnumWindows(FindTaskView, reinterpret_cast<LPARAM>(&lookup));
    return lookup.found;
}

static void MonitorName(HMONITOR monitor, WCHAR* out, size_t chars) {
    MONITORINFOEXW info = {};
    info.cbSize = sizeof(info);
    if (monitor && GetMonitorInfoW(monitor, &info)) {
        StringCchPrintfW(out, chars, L"%s", info.szDevice);
    } else {
        StringCchCopyW(out, chars, L"unknown");
    }
}

static void Log(const WCHAR* message) {
    if (!g_logPath[0]) return;
    SYSTEMTIME st = {};
    GetLocalTime(&st);
    WCHAR line[1024] = {};
    if (FAILED(StringCchPrintfW(line, ARRAYSIZE(line),
        L"%02u:%02u:%02u.%03u | %s\r\n", st.wHour, st.wMinute,
        st.wSecond, st.wMilliseconds, message))) return;
    int len = WideCharToMultiByte(CP_UTF8, 0, line, -1, nullptr, 0, nullptr, nullptr);
    if (len <= 1 || len > 4096) return;
    char utf8[4096] = {};
    if (!WideCharToMultiByte(CP_UTF8, 0, line, -1, utf8, len, nullptr, nullptr)) return;
    HANDLE file = CreateFileW(g_logPath, FILE_APPEND_DATA,
        FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return;
    DWORD written = 0;
    WriteFile(file, utf8, static_cast<DWORD>(len - 1), &written, nullptr);
    CloseHandle(file);
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
    int n = GetWindowTextW(hwnd, title, ARRAYSIZE(title));
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
        Log(note);
        return nullptr;
    }

    HWND unique = nullptr;
    int distinctMatches = 0;
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
            if (type == 50007 && level <= 4) {
                WindowMatch match = {name, probe.monitor, nullptr, 0};
                EnumWindows(FindTitleMatch, reinterpret_cast<LPARAM>(&match));
                count = match.matches;
                candidate = match.hwnd;
                if (count == 1) {
                    unique = candidate;
                    distinctMatches = 1;
                }
            }
        }
        WCHAR note[360] = {};
        StringCchPrintfW(note, ARRAYSIZE(note),
            L"PROBE #%ld level=%d uiaType=%ld nameLength=%lu "
            L"exactSourceMonitorMatches=%d hwnd=%p",
            probe.sequence, level, static_cast<long>(type), nameLen,
            count, count == 1 ? candidate : nullptr);
        if (g_detailedLogging) Log(note);
        if (name) SysFreeString(name);
        // The first matching ListItem is all we need to move. Avoid the
        // previous six slow parent COM calls on every successful drag.
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
        distinctMatches == 1 ? L"yes" : L"no",
        distinctMatches == 1 ? unique : nullptr);
    if (g_detailedLogging || !unique) Log(result);
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
// Windhawk/Explorer restart or retain a window across process recreation.
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
    if (!IsWindowVisible(host)) return TRUE;
    WCHAR cls[80] = {}, title[80] = {};
    if (!GetClassNameW(host, cls, ARRAYSIZE(cls)) ||
        lstrcmpW(cls, L"XamlExplorerHostIslandWindow") != 0 ||
        !GetWindowTextW(host, title, ARRAYSIZE(title)) ||
        lstrcmpW(title, L"Task View") != 0) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(host, &pid);
    if (pid == GetCurrentProcessId())
        ++(*reinterpret_cast<int*>(context));
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
        Log(L"REFRESH skipped, another mouse interaction already started");
        return;
    }
    int before = TaskViewHostCount();
    if (before == 0) {
        Log(L"REFRESH skipped, Task View already gone");
        return;
    }
    // Only inject hotkeys while Task View's own Explorer process has focus.
    // Otherwise Esc could dismiss an unrelated application's dialog.
    DWORD foregroundPid = 0;
    GetWindowThreadProcessId(GetForegroundWindow(), &foregroundPid);
    if (foregroundPid != GetCurrentProcessId()) {
        Log(L"REFRESH skipped, Task View Explorer process not in foreground");
        return;
    }
    // Do not interfere with user input, held modifiers, or another drag.
    const int heldKeys[] = {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_SHIFT,
                            VK_CONTROL, VK_MENU, VK_LWIN, VK_RWIN};
    for (int key : heldKeys) {
        if (GetAsyncKeyState(key) & 0x8000) {
            Log(L"REFRESH skipped, key/button currently held");
            return;
        }
    }
    if (InterlockedCompareExchange(&g_mousePressed, 0, 0) ||
        sequence != InterlockedCompareExchange(&g_dragSequence, 0, 0)) {
        Log(L"REFRESH skipped, new drag before Esc");
        return;
    }
    if (!KeyStroke(VK_ESCAPE)) {
        Log(L"REFRESH failed, Esc SendInput unsuccessful");
        return;
    }
    int gone = 0;
    for (int i = 0; i < 18; ++i) {
        Sleep(20);
        if (TaskViewHostCount() == 0) { gone = 1; break; }
    }
    if (!gone) {
        Log(L"AFTER_MOVE Escape did not close Task View; no further action");
        return;
    }
    if (afterMove == 1) {
        Log(L"AFTER_MOVE Task View closed");
        return;
    }
    // Only reopen after we verified the original interface disappeared.
    Sleep(65);
    if (InterlockedCompareExchange(&g_mousePressed, 0, 0) ||
        sequence != InterlockedCompareExchange(&g_dragSequence, 0, 0)) {
        Log(L"REFRESH reopen aborted, another gesture started");
        return;
    }
    bool opened = OpenTaskViewShortcut();
    Sleep(140);
    WCHAR note[200] = {};
    StringCchPrintfW(note, ARRAYSIZE(note),
        L"AFTER_MOVE #%ld reopenSent=%s newHosts=%d",
        sequence, opened ? L"yes" : L"no", TaskViewHostCount());
    Log(note);
}

static void MoveWindowToMonitor(const MoveRequest& req) {
    // Give native Task View a chance to finish its mouseup handling.
    Sleep(130);
    if (GetTickCount() - req.releaseTick > 4500) {
        Log(L"SKIP stale move request (UI Automation queue was delayed)");
        return;
    }
    if (InterlockedCompareExchange(&g_mousePressed, 0, 0)) {
        Log(L"SKIP another drag began before pending move");
        return;
    }
    WCHAR note[512] = {};
    if (!IsWindow(req.hwnd) || !IsWindowVisible(req.hwnd) ||
        GetWindow(req.hwnd, GW_OWNER) ||
        MonitorFromWindow(req.hwnd, MONITOR_DEFAULTTONULL) != req.source) {
        StringCchPrintfW(note, ARRAYSIZE(note),
            L"SKIP #%ld candidate moved, vanished, or no longer on source", req.sequence);
        Log(note);
        return;
    }
    WCHAR cls[80] = {};
    GetClassNameW(req.hwnd, cls, ARRAYSIZE(cls));
    if (lstrcmpW(cls, L"XamlExplorerHostIslandWindow") == 0) {
        Log(L"SKIP shell's Task View window");
        return;
    }
    MONITORINFO src = {}, dst = {};
    src.cbSize = sizeof(src);
    dst.cbSize = sizeof(dst);
    if (!GetMonitorInfoW(req.source, &src) || !GetMonitorInfoW(req.destination, &dst)) {
        Log(L"SKIP monitor information unavailable");
        return;
    }
    WINDOWPLACEMENT placement = {};
    placement.length = sizeof(placement);
    if (!GetWindowPlacement(req.hwnd, &placement) ||
        placement.showCmd == SW_SHOWMINIMIZED || IsIconic(req.hwnd)) {
        Log(L"SKIP window minimized or placement unavailable");
        return;
    }
    const bool maximized = IsZoomed(req.hwnd) != FALSE;
    if (maximized && !g_allowMaximized) {
        Log(L"SKIP maximized window movement disabled");
        return;
    }
    bool restored = false;
    if (maximized && g_enableMovement) {
        ShowWindowAsync(req.hwnd, SW_RESTORE);
        for (int i = 0; i < 12 && IsZoomed(req.hwnd); ++i) Sleep(25);
        restored = !IsZoomed(req.hwnd);
        if (!restored) { Log(L"SKIP restore timed out"); return; }
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
        Log(L"SKIP invalid work/window dimensions");
        if (restored) ShowWindowAsync(req.hwnd, SW_MAXIMIZE);
        return;
    }
    const UINT srcDpi = GetDpiForWindow(req.hwnd);
    RECT targetRect = {};
    const bool cached = LoadGeometry(req.hwnd, req.destination, &targetRect);
    double scale = 1.0;
    if (!cached) {
        if (g_geometryMode == 0) {
            const double sx = double(dstW) / double(srcW);
            const double sy = double(dstH) / double(srcH);
            scale = sx < sy ? sx : sy;
        } else if (g_geometryMode == 2) {
            // Destination monitor's DPI is only available with Shcore's
            // GetDpiForMonitor; dynamic import avoids hard dependency.
            using GetDpiFn = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
            HMODULE shcore = GetModuleHandleW(L"shcore.dll");
            if (!shcore) shcore = LoadLibraryW(L"shcore.dll");
            UINT dx = 96, dy = 96;
            auto getDpi = shcore ? reinterpret_cast<GetDpiFn>(
                GetProcAddress(shcore, "GetDpiForMonitor")) : nullptr;
            if (getDpi && SUCCEEDED(getDpi(req.destination, 0, &dx, &dy)) && srcDpi)
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
        tx, ty, tw, th, g_geometryMode, scale, cached ? L"yes" : L"no", srcDpi);
    Log(note);
    if (!g_enableMovement) {
        Log(L"DRY_RUN movement disabled");
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
        Log(note);
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
    Log(note);
    if (!ok && error == ERROR_ACCESS_DENIED)
        Log(L"ACCESS_DENIED Windows refused to move the target; often an elevated/protected process. No app-specific override is attempted.");
    if (now == req.destination && ok) {
        SaveGeometry(req.hwnd, req.source, oldRect);
        ApplyAfterMove(req.sequence);
    } else if (ok) {
        Log(L"WARN movement posted but destination monitor was not observed");
    }
}

static DWORD WINAPI AutomationThread(void*) {
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
    Log(note);

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
                else Log(L"SKIP identification unavailable, UIA not initialised");
                delete probe;
            }
        } else if (msg.message == kTrace) {
            Trace* record = reinterpret_cast<Trace*>(msg.wParam);
            if (record) { Log(record->message); delete record; }
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
                    Log(diagnostic);
                } else if (InterlockedCompareExchange(&g_mousePressed, 0, 0)) {
                    Log(L"SKIP a new drag is already in progress");
                } else {
                    req->hwnd = inspectedHwnd;
                    MoveWindowToMonitor(*req);
                }
                inspectedHwnd = nullptr;
                delete req;
            }
        }
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
                    Probe* probe = new Probe{p, g_startMonitor, seq};
                    if (!PostThreadMessageW(g_workerId, kInspect, reinterpret_cast<WPARAM>(probe), 0))
                        delete probe;
                    WCHAR mon[80], note[250];
                    MonitorName(g_startMonitor, mon, ARRAYSIZE(mon));
                    StringCchPrintfW(note, ARRAYSIZE(note),
                        L"DOWN #%ld x=%ld y=%ld monitor=%s", seq, p.x, p.y, mon);
                    if (g_detailedLogging) TraceFromMouseHook(note);
                }
            } else if (g_dragging && msg == WM_MOUSEMOVE) {
                HMONITOR newMon = MonitorFromPoint(p, MONITOR_DEFAULTTONEAREST);
                if (newMon != g_lastMonitor) {
                    WCHAR mon[80], note[250];
                    MonitorName(newMon, mon, ARRAYSIZE(mon));
                    StringCchPrintfW(note, ARRAYSIZE(note),
                        L"CROSS x=%ld y=%ld into=%s taskview=%s",
                        p.x, p.y, mon, TaskViewAt(p) ? L"yes" : L"no");
                    if (g_detailedLogging) TraceFromMouseHook(note);
                    g_lastMonitor = newMon;
                }
            } else if (g_dragging && (msg == WM_LBUTTONUP || msg == WM_RBUTTONDOWN)) {
                HMONITOR dest = MonitorFromPoint(p, MONITOR_DEFAULTTONEAREST);
                WCHAR mon[80], note[300];
                MonitorName(dest, mon, ARRAYSIZE(mon));
                const bool overTaskView = TaskViewAt(p) != nullptr;
                const DWORD elapsed = GetTickCount() - g_startTick;
                StringCchPrintfW(note, ARRAYSIZE(note),
                    L"UP x=%ld y=%ld monitor=%s cross=%s taskview=%s durationMs=%lu",
                    p.x, p.y, mon,
                    dest == g_startMonitor ? L"no" : L"yes",
                    overTaskView ? L"yes" : L"no", elapsed);
                if (g_detailedLogging) TraceFromMouseHook(note);
                const LONG deltaX = p.x - g_startPt.x;
                const LONG deltaY = p.y - g_startPt.y;
                const bool travelled = (deltaX >= 40 || deltaX <= -40 ||
                                        deltaY >= 40 || deltaY <= -40);
                bool validDrop = (msg == WM_LBUTTONUP &&
                    dest != g_startMonitor && overTaskView &&
                    elapsed >= 150 && travelled);
                if (validDrop) {
                    MoveRequest* req = new MoveRequest{nullptr, g_startMonitor, dest,
                        InterlockedCompareExchange(&g_dragSequence, 0, 0),
                        elapsed, p, g_startPt, GetTickCount()};
                    if (!PostThreadMessageW(g_workerId, kMove,
                            reinterpret_cast<WPARAM>(req), 0)) {
                        delete req;
                        TraceFromMouseHook(L"ERROR unable to queue window movement");
                    } else {
                        if (g_detailedLogging) TraceFromMouseHook(L"QUEUE move request queued for post-release processing");
                    }
                } else if (dest != g_startMonitor) {
                    TraceFromMouseHook(L"SKIP release rejected: wrong mouse button, Task View closed, or drag too short");
                }
                g_dragging = false;
                InterlockedExchange(&g_mousePressed, 0);
            }
        }
    }
    return CallNextHookEx(g_mouseHook, code, msg, param);
}

static DWORD WINAPI MouseThread(void*) {
    MSG msg = {};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&MouseHook), &module);
    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseHook, module, 0);
    InterlockedExchange(&g_mouseStatus, g_mouseHook ? 1 : -1);
    SetEvent(g_mouseReady);
    if (!g_mouseHook) return 1;
    Log(L"START v0.7 ordered movement");
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    UnhookWindowsHookEx(g_mouseHook);
    g_mouseHook = nullptr;
    Log(L"STOP v0.7 ordered movement");
    return 0;
}

BOOL Wh_ModInit(void) {
    WCHAR temp[MAX_PATH] = {};
    DWORD n = GetTempPathW(ARRAYSIZE(temp), temp);
    if (n == 0 || n >= ARRAYSIZE(temp) ||
        FAILED(StringCchPrintfW(g_logPath, ARRAYSIZE(g_logPath),
                                L"%sFancyTaskViewMove.log", temp))) return FALSE;
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
    g_detailedLogging = Wh_GetIntSetting(L"DetailedLogging") != 0;
    g_workerReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_mouseReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_workerReady || !g_mouseReady) goto fail;
    g_worker = CreateThread(nullptr, 0, AutomationThread, nullptr, 0, &g_workerId);
    if (!g_worker || WaitForSingleObject(g_workerReady, 5000) != WAIT_OBJECT_0) goto fail;
    g_mouseThread = CreateThread(nullptr, 0, MouseThread, nullptr, 0, &g_mouseThreadId);
    if (!g_mouseThread || WaitForSingleObject(g_mouseReady, 5000) != WAIT_OBJECT_0 ||
        InterlockedCompareExchange(&g_mouseStatus, 0, 0) != 1) goto fail;
    return TRUE;
fail:
    if (g_mouseThread) {
        PostThreadMessageW(g_mouseThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_mouseThread, 2000);
        CloseHandle(g_mouseThread); g_mouseThread = nullptr;
    }
    if (g_worker) {
        PostThreadMessageW(g_workerId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_worker, 2000);
        CloseHandle(g_worker); g_worker = nullptr;
    }
    if (g_workerReady) { CloseHandle(g_workerReady); g_workerReady = nullptr; }
    if (g_mouseReady) { CloseHandle(g_mouseReady); g_mouseReady = nullptr; }
    return FALSE;
}

void Wh_ModUninit(void) {
    if (g_mouseThread) {
        PostThreadMessageW(g_mouseThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_mouseThread, INFINITE);
        CloseHandle(g_mouseThread); g_mouseThread = nullptr;
    }
    if (g_worker) {
        PostThreadMessageW(g_workerId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker); g_worker = nullptr;
    }
    if (g_workerReady) { CloseHandle(g_workerReady); g_workerReady = nullptr; }
    if (g_mouseReady) { CloseHandle(g_mouseReady); g_mouseReady = nullptr; }
}

void Wh_ModSettingsChanged(void) {
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
    g_detailedLogging = Wh_GetIntSetting(L"DetailedLogging") != 0;
}
