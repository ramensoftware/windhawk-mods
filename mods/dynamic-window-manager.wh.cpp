// ==WindhawkMod==
// @id              dynamic-window-manager
// @name            Dynamic Window Manager
// @description     Sizes, maximizes and positions new application windows according to user-defined per-application rules
// @version         1.0.0
// @author          Armaninyow
// @github          https://github.com/armaninyow
// @include         windhawk.exe
// @include         taskmgr.exe
// @compilerOptions -luser32 -ldwmapi -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Dynamic Window Manager

Automatically sizes and positions application windows based on rules that you
define in the mod's settings. Each rule matches an executable by its process
name and specifies whether the window should be maximized, or its target size,
the anchor corner of the monitor to place it at, and the monitor to use.

Example rules:

| Process name  | Maximize | Width | Height | Horizontal anchor | Vertical anchor | Monitor |
|---------------|----------|-------|--------|-------------------|-----------------|---------|
| `chrome.exe`  | No       | 1600  | 1000   | Right             | Top             | 2       |
| `notepad.exe` | No       | 800   | 600    | Left              | Bottom          | 1       |
| `steam.exe`   | Yes      | \-    | \-     | \-                | \-              | 1       |

## Getting the process name

Rules match an application by its executable file name, so the name has to be
exact: `spotify.exe`, not "Spotify".

1. In Windhawk, open Dynamic Window Manager and go to **Advanced** →
   **Debug logging** → **Mod logs**.
2. Click **Show log output**.
3. Open any app you want a rule for. The mod writes a line for every new window
   it sees, with that window's process name, for example
   `New window ... of process 'spotify.exe'`.
4. Type that name into the rule's **Process name** field.
5. Close the log output when you are done.

## Similar mods

[Center New Windows](https://windhawk.net/mods/center-new-windows)
and
[Window Centralizer](https://windhawk.net/mods/window-centralizer)
also place new windows, but with a single global size or without resizing at
all, so they don't cover per-application rules, sizes and anchors. If either of
them is enabled together with this mod, both will handle the same window.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- rules:
  - - processName: example.exe
      $name: Process name
      $description: >-
        Executable file name of the application, matched case-insensitively,
        for example chrome.exe
    - maximize: false
      $name: Maximize
      $description: >-
        Maximize the window on the selected monitor instead of applying the
        width, height and anchors below
    - width: 1280
      $name: Width
      $description: Target window width in pixels (minimum 100)
    - height: 720
      $name: Height
      $description: Target window height in pixels (minimum 100)
    - horizontalAnchor: left
      $name: Horizontal anchor
      $description: Edge of the monitor work area the window is aligned to
      $options:
      - left: Left
      - center: Center
      - right: Right
    - verticalAnchor: top
      $name: Vertical anchor
      $description: Edge of the monitor work area the window is aligned to
      $options:
      - top: Top
      - center: Center
      - bottom: Bottom
    - monitor: 1
      $name: Monitor
      $description: >-
        Target monitor, 1-based. The first monitor is used when the value is out
        of range
  $name: Window rules
  $description: >-
    One rule per application. A rule is applied once, when a window of that
    application is first shown; afterwards the window can be moved and resized
    freely
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <dwmapi.h>
#include <shellapi.h>
#include <strsafe.h>

#include <algorithm>
#include <atomic>
#include <deque>
#include <mutex>
#include <set>
#include <string>
#include <vector>

namespace {

// Minimum window dimension, in pixels, that a rule can request.
constexpr int kMinimumDimension = 100;

// Default monitor number (1-based) used when a rule doesn't specify one.
constexpr int kDefaultMonitor = 1;

// How long to wait after a window is detected before applying its rule. Some
// applications size their windows themselves right after showing them, so
// applying the rule immediately would often be overridden.
constexpr ULONGLONG kApplyDelayMs = 200;

// How many times a rule is retried while the window is still maximized. The
// restore is asynchronous, so the first pass can't apply the rule.
constexpr int kMaxApplyAttempts = 5;

// DWMWA_EXTENDED_FRAME_BOUNDS, the visible window frame.
constexpr DWORD kExtendedFrameBounds = 9;

// Upper bound for the set of already handled windows before stale entries are
// pruned.
constexpr size_t kMaxTrackedWindows = 512;

enum class HorizontalAnchor {
    Left,
    Center,
    Right,
};

enum class VerticalAnchor {
    Top,
    Center,
    Bottom,
};

struct Rule {
    std::wstring processName;
    bool maximize = false;
    int width = kMinimumDimension;
    int height = kMinimumDimension;
    HorizontalAnchor horizontalAnchor = HorizontalAnchor::Left;
    VerticalAnchor verticalAnchor = VerticalAnchor::Top;
    int monitor = kDefaultMonitor;
};

struct PendingWindow {
    HWND hwnd = nullptr;
    ULONGLONG due = 0;
    int attempts = 0;
};

enum class ApplyResult {
    Done,
    // The rule has to be applied again, because the window was restored and the
    // restore hasn't taken effect yet.
    Retry,
};

// Window rules, replaced as a whole when the settings change. Guarded by
// g_rulesLock because settings can be reloaded from another thread.
std::mutex g_rulesLock;
std::vector<Rule> g_rules;

HANDLE g_workerThread = nullptr;
DWORD g_workerThreadId = 0;
HWINEVENTHOOK g_showHook = nullptr;
HWINEVENTHOOK g_destroyHook = nullptr;
HWINEVENTHOOK g_uncloakHook = nullptr;
HWINEVENTHOOK g_restoreHook = nullptr;
std::atomic<bool> g_unloading{false};

// Whether the worker manages windows of other processes. A worker that runs
// inside an injected application process (Task Manager) manages the windows of
// that process, and only those.
bool g_managesOtherProcesses = true;

// Set when the worker runs inside an injected application process instead of
// the mod's own tool process.
bool g_isInProcessWorker = false;

DWORD g_processId = 0;

// The members below are only accessed from the worker thread.
std::set<HWND> g_seenWindows;
std::deque<PendingWindow> g_pendingWindows;

// Identifier of the worker thread's timer while a rule is waiting, or zero when
// the timer isn't armed. Windows assigns the identifier, so the returned value
// has to be kept.
UINT_PTR g_applyTimerId = 0;

void UpdateApplyTimer();
void CALLBACK ApplyTimerProc(HWND, UINT, UINT_PTR, DWORD);

int ClampDimension(int value) {
    return value < kMinimumDimension ? kMinimumDimension : value;
}

HorizontalAnchor ParseHorizontalAnchor(PCWSTR value) {
    if (value) {
        if (lstrcmpiW(value, L"center") == 0) {
            return HorizontalAnchor::Center;
        }
        if (lstrcmpiW(value, L"right") == 0) {
            return HorizontalAnchor::Right;
        }
    }
    return HorizontalAnchor::Left;
}

VerticalAnchor ParseVerticalAnchor(PCWSTR value) {
    if (value) {
        if (lstrcmpiW(value, L"center") == 0) {
            return VerticalAnchor::Center;
        }
        if (lstrcmpiW(value, L"bottom") == 0) {
            return VerticalAnchor::Bottom;
        }
    }
    return VerticalAnchor::Top;
}

// Builds the setting name locally instead of relying on the printf-style
// formatting of the settings API.
std::wstring SettingName(const wchar_t* format, int index) {
    wchar_t name[128];
    if (FAILED(StringCchPrintfW(name, ARRAYSIZE(name), format, index))) {
        return std::wstring();
    }

    return name;
}

std::wstring ReadStringSetting(const wchar_t* format, int index) {
    std::wstring name = SettingName(format, index);
    if (name.empty()) {
        return std::wstring();
    }

    PCWSTR value = Wh_GetStringSetting(name.c_str());
    std::wstring result = value ? value : L"";
    if (value) {
        Wh_FreeStringSetting(value);
    }

    return result;
}

int ReadIntSetting(const wchar_t* format, int index, int fallback) {
    std::wstring name = SettingName(format, index);
    if (name.empty()) {
        return fallback;
    }

    return Wh_GetIntSetting(name.c_str());
}

void LoadSettings() {
    std::vector<Rule> rules;

    for (int i = 0;; i++) {
        std::wstring processName =
            ReadStringSetting(L"rules[%d].processName", i);

        if (processName.empty()) {
            // The list ends at the first entry without a process name.
            break;
        }

        Rule rule;
        rule.processName = std::move(processName);
        rule.maximize = ReadIntSetting(L"rules[%d].maximize", i, 0) != 0;
        rule.width =
            ClampDimension(ReadIntSetting(L"rules[%d].width", i, 0));
        rule.height =
            ClampDimension(ReadIntSetting(L"rules[%d].height", i, 0));

        std::wstring horizontalAnchor =
            ReadStringSetting(L"rules[%d].horizontalAnchor", i);
        rule.horizontalAnchor = ParseHorizontalAnchor(horizontalAnchor.c_str());

        std::wstring verticalAnchor =
            ReadStringSetting(L"rules[%d].verticalAnchor", i);
        rule.verticalAnchor = ParseVerticalAnchor(verticalAnchor.c_str());

        rule.monitor = ReadIntSetting(L"rules[%d].monitor", i, kDefaultMonitor);
        if (rule.monitor < 1) {
            rule.monitor = kDefaultMonitor;
        }

        rules.push_back(std::move(rule));
    }

    size_t ruleCount = rules.size();

    {
        std::lock_guard<std::mutex> lock(g_rulesLock);
        g_rules = std::move(rules);
    }

    Wh_Log(L"Loaded %zu window rule(s)", ruleCount);
}

// Compares an executable name from a rule with the executable name of a
// process. Both are expected to be bare file names, without a path.
bool ProcessNameMatches(const std::wstring& ruleName,
                        const std::wstring& processName) {
    if (ruleName.empty() || processName.empty()) {
        return false;
    }

    if (lstrcmpiW(ruleName.c_str(), processName.c_str()) == 0) {
        return true;
    }

    // Allow the ".exe" suffix to be omitted in the rule.
    constexpr size_t exeLength = 4;
    if (ruleName.length() <= exeLength ||
        lstrcmpiW(ruleName.c_str() + ruleName.length() - exeLength, L".exe") !=
            0) {
        std::wstring withSuffix = ruleName + L".exe";
        if (lstrcmpiW(withSuffix.c_str(), processName.c_str()) == 0) {
            return true;
        }
    }

    return false;
}

// Returns the file name part of a path.
const wchar_t* FileNameOf(const wchar_t* path) {
    const wchar_t* fileName = path;
    for (const wchar_t* it = path; *it; it++) {
        if (*it == L'\\' || *it == L'/') {
            fileName = it + 1;
        }
    }

    return fileName;
}

// Whether the mod is loaded in Windhawk's own process, which is where the tool
// process is started from. Windhawk also injects the mod into the processes it
// is configured to target, and those run the worker themselves.
bool IsWindhawkProcess() {
    wchar_t path[MAX_PATH];
    if (!GetModuleFileNameW(nullptr, path, ARRAYSIZE(path))) {
        return false;
    }

    return lstrcmpiW(FileNameOf(path), L"windhawk.exe") == 0;
}

// A worker that runs inside an application process can't rely on the tool
// process to handle that application's windows, because they may be elevated.
// The tool process, in turn, manages every window: the windows of an elevated
// application can't be changed from there, and are handled by the worker that
// runs inside that application.
bool ShouldManageWindow(HWND hwnd) {
    if (g_managesOtherProcesses) {
        return true;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    return processId && processId == g_processId;
}

// Retrieves the executable file name of the process that owns the window.
bool GetWindowProcessName(HWND hwnd, std::wstring& processName) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (!processId) {
        return false;
    }

    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return false;
    }

    wchar_t path[MAX_PATH];
    DWORD pathLength = ARRAYSIZE(path);
    BOOL ok = QueryFullProcessImageNameW(process, 0, path, &pathLength);
    CloseHandle(process);

    if (!ok || !pathLength) {
        return false;
    }

    processName.assign(FileNameOf(path));
    return !processName.empty();
}

bool FindRuleForProcess(const std::wstring& processName, Rule* rule) {
    std::lock_guard<std::mutex> lock(g_rulesLock);
    for (const Rule& candidate : g_rules) {
        if (ProcessNameMatches(candidate.processName, processName)) {
            *rule = candidate;
            return true;
        }
    }

    return false;
}

struct MonitorEntry {
    HMONITOR monitor;
    RECT bounds;
};

BOOL CALLBACK EnumMonitorProc(HMONITOR monitor, HDC, LPRECT, LPARAM parameter) {
    auto* monitors = reinterpret_cast<std::vector<MonitorEntry>*>(parameter);

    MONITORINFO monitorInfo;
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (GetMonitorInfoW(monitor, &monitorInfo)) {
        monitors->push_back({monitor, monitorInfo.rcMonitor});
    }

    return TRUE;
}

// Returns the monitor for a 1-based index, following the physical layout of
// the displays. The first monitor is used when the index is out of range.
HMONITOR GetMonitorByIndex(int index) {
    std::vector<MonitorEntry> monitors;
    EnumDisplayMonitors(nullptr, nullptr, EnumMonitorProc,
                        reinterpret_cast<LPARAM>(&monitors));

    if (monitors.empty()) {
        return MonitorFromWindow(nullptr, MONITOR_DEFAULTTOPRIMARY);
    }

    std::sort(monitors.begin(), monitors.end(),
              [](const MonitorEntry& first, const MonitorEntry& second) {
                  if (first.bounds.top != second.bounds.top) {
                      return first.bounds.top < second.bounds.top;
                  }
                  return first.bounds.left < second.bounds.left;
              });

    size_t wanted = static_cast<size_t>(index > 0 ? index - 1 : 0);
    if (wanted >= monitors.size()) {
        wanted = 0;
    }

    return monitors[wanted].monitor;
}

int GetAnchorPosition(HorizontalAnchor anchor, const RECT& workArea, int width) {
    int workWidth = workArea.right - workArea.left;

    switch (anchor) {
        case HorizontalAnchor::Right:
            return workArea.right - width;
        case HorizontalAnchor::Center:
            return workArea.left + (workWidth - width) / 2;
        case HorizontalAnchor::Left:
        default:
            return workArea.left;
    }
}

int GetAnchorPosition(VerticalAnchor anchor, const RECT& workArea, int height) {
    int workHeight = workArea.bottom - workArea.top;

    switch (anchor) {
        case VerticalAnchor::Bottom:
            return workArea.bottom - height;
        case VerticalAnchor::Center:
            return workArea.top + (workHeight - height) / 2;
        case VerticalAnchor::Top:
        default:
            return workArea.top;
    }
}

// Window changes are always requested asynchronously: a synchronous call blocks
// until the owning thread handles it, which never happens for a hung
// application, and would hold up the worker thread, including on unload.
ApplyResult ApplyRule(HWND hwnd, const Rule& rule) {
    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd)) {
        return ApplyResult::Done;
    }

    HMONITOR monitor = GetMonitorByIndex(rule.monitor);

    MONITORINFO monitorInfo;
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return ApplyResult::Done;
    }

    const RECT& workArea = monitorInfo.rcWork;
    int workWidth = workArea.right - workArea.left;
    int workHeight = workArea.bottom - workArea.top;

    // A maximized window can neither take a size nor move to another monitor.
    // Restoring it is asynchronous, so the rule is applied again on a later
    // pass, after the restore has happened and the borders can be measured.
    if (IsZoomed(hwnd)) {
        ShowWindowAsync(hwnd, SW_RESTORE);
        return ApplyResult::Retry;
    }

    if (rule.maximize) {
        // A window is maximized on the monitor it occupies, so it is moved to
        // the target monitor first.
        RECT windowRect = {};
        HMONITOR currentMonitor =
            GetWindowRect(hwnd, &windowRect)
                ? MonitorFromRect(&windowRect, MONITOR_DEFAULTTONULL)
                : nullptr;

        if (currentMonitor != monitor) {
            SetWindowPos(hwnd, nullptr, workArea.left, workArea.top, 0, 0,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER |
                             SWP_NOSIZE | SWP_ASYNCWINDOWPOS);
        }

        ShowWindowAsync(hwnd, SW_MAXIMIZE);
        return ApplyResult::Done;
    }

    // Windows 10 and 11 windows include invisible resize borders in their
    // window rectangle. Measure them, so that the visible frame ends up with the
    // requested size and at the requested position.
    int borderLeft = 0;
    int borderTop = 0;
    int borderRight = 0;
    int borderBottom = 0;

    RECT frameBounds = {};
    RECT windowRect = {};
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, kExtendedFrameBounds,
                                        &frameBounds, sizeof(frameBounds))) &&
        GetWindowRect(hwnd, &windowRect)) {
        borderLeft = frameBounds.left - windowRect.left;
        borderTop = frameBounds.top - windowRect.top;
        borderRight = windowRect.right - frameBounds.right;
        borderBottom = windowRect.bottom - frameBounds.bottom;
    }

    // Only the visible size is clamped to the work area. The window rectangle
    // itself is not, because its invisible resize borders may stick out of the
    // work area; clamping it would shrink the visible frame by the border
    // thickness below the requested size.
    int visibleWidth = std::min(ClampDimension(rule.width), workWidth);
    int visibleHeight = std::min(ClampDimension(rule.height), workHeight);

    int width = visibleWidth + borderLeft + borderRight;
    int height = visibleHeight + borderTop + borderBottom;

    int x = GetAnchorPosition(rule.horizontalAnchor, workArea, visibleWidth) -
            borderLeft;
    int y = GetAnchorPosition(rule.verticalAnchor, workArea, visibleHeight) -
            borderTop;

    // This is ignored for the windows of elevated applications, which a process
    // of a lower privilege level can't change. Nothing is retried in that case,
    // the window simply keeps the size and position the application gave it.
    SetWindowPos(hwnd, nullptr, x, y, width, height,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER |
                     SWP_ASYNCWINDOWPOS);

    return ApplyResult::Done;
}

void ProcessPendingWindows() {
    ULONGLONG now = GetTickCount64();

    std::vector<PendingWindow> ready;
    for (auto it = g_pendingWindows.begin(); it != g_pendingWindows.end();) {
        if (it->due <= now) {
            ready.push_back(*it);
            it = g_pendingWindows.erase(it);
        } else {
            ++it;
        }
    }

    for (const PendingWindow& pending : ready) {
        HWND hwnd = pending.hwnd;
        if (!IsWindow(hwnd)) {
            continue;
        }

        // Look the rule up again in case the settings changed meanwhile.
        std::wstring processName;
        if (!GetWindowProcessName(hwnd, processName)) {
            continue;
        }

        Rule rule;
        if (!FindRuleForProcess(processName, &rule)) {
            continue;
        }

        if (ApplyRule(hwnd, rule) == ApplyResult::Retry &&
            pending.attempts + 1 < kMaxApplyAttempts) {
            g_pendingWindows.push_back(
                {hwnd, GetTickCount64() + kApplyDelayMs, pending.attempts + 1});
        }
    }
}

// Arms the worker thread's timer for the window that is due first, or stops it
// when there is nothing left to do. The timer is the only polling that happens;
// new windows themselves are reported by the event hooks.
void UpdateApplyTimer() {
    if (g_pendingWindows.empty()) {
        if (g_applyTimerId) {
            KillTimer(nullptr, g_applyTimerId);
            g_applyTimerId = 0;
        }
        return;
    }

    ULONGLONG now = GetTickCount64();
    ULONGLONG earliest = g_pendingWindows.front().due;
    for (const PendingWindow& pending : g_pendingWindows) {
        earliest = std::min(earliest, pending.due);
    }

    UINT delay = earliest > now ? static_cast<UINT>(earliest - now) : 1;

    // A thread timer (created with a NULL window) gets its identifier from
    // Windows, which ignores the requested one and returns the identifier it
    // chose. Keeping that identifier and passing it back resets the existing
    // timer instead of creating another one, and is also what KillTimer needs.
    g_applyTimerId = SetTimer(nullptr, g_applyTimerId, delay, ApplyTimerProc);
}

void CALLBACK ApplyTimerProc(HWND, UINT, UINT_PTR, DWORD) {
    ProcessPendingWindows();
    UpdateApplyTimer();
}

// Decides whether a window is a candidate for a rule at all.
bool IsCandidateWindow(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) {
        return false;
    }

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

    // Child windows are not top-level application windows.
    if (style & WS_CHILD) {
        return false;
    }

    // Tool windows are floating helpers, not application windows.
    if (exStyle & WS_EX_TOOLWINDOW) {
        return false;
    }

    // The window has to have a title bar.
    if (!(style & WS_CAPTION)) {
        return false;
    }

    // Fixed-size dialogs are left alone.
    if ((style & WS_DLGFRAME) && !(style & WS_THICKFRAME)) {
        return false;
    }

    return true;
}

// Looks at a window that has just appeared and schedules it if it matches a
// rule. Windows are only ever considered once.
void EvaluateWindow(HWND hwnd) {
    if (!ShouldManageWindow(hwnd)) {
        return;
    }

    if (g_seenWindows.find(hwnd) != g_seenWindows.end()) {
        return;
    }

    if (!IsCandidateWindow(hwnd)) {
        // Not eligible yet. The window is not remembered, so that it is picked
        // up later if it becomes visible or changes its style.
        return;
    }

    if (g_seenWindows.size() >= kMaxTrackedWindows) {
        for (auto it = g_seenWindows.begin(); it != g_seenWindows.end();) {
            if (!IsWindow(*it)) {
                it = g_seenWindows.erase(it);
            } else {
                ++it;
            }
        }
    }

    g_seenWindows.insert(hwnd);

    std::wstring processName;
    if (!GetWindowProcessName(hwnd, processName)) {
        return;
    }

    // Logged for every new window, so that the log output can be used to find
    // the process name to put in a rule.
    Wh_Log(L"New window %p of process '%s'", hwnd, processName.c_str());

    Rule rule;
    if (!FindRuleForProcess(processName, &rule)) {
        return;
    }

    g_pendingWindows.push_back({hwnd, GetTickCount64() + kApplyDelayMs, 0});
    UpdateApplyTimer();

    Wh_Log(L"Scheduled rule '%s' for window %p", rule.processName.c_str(), hwnd);
}

// Remembers all currently open windows, so that they are not affected by the
// rules. Only windows that appear after the mod is loaded are handled.
BOOL CALLBACK MarkExistingWindowsProc(HWND hwnd, LPARAM) {
    g_seenWindows.insert(hwnd);
    return TRUE;
}

void CALLBACK WinEventProc(HWINEVENTHOOK,
                           DWORD event,
                           HWND hwnd,
                           LONG idObject,
                           LONG idChild,
                           DWORD,
                           DWORD) {
    if (g_unloading || !hwnd) {
        return;
    }

    // A window that was shown minimized only becomes a candidate once it is
    // restored.
    if (event == EVENT_SYSTEM_MINIMIZEEND) {
        EvaluateWindow(hwnd);
        return;
    }

    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }

    if (event == EVENT_OBJECT_DESTROY) {
        g_seenWindows.erase(hwnd);
        return;
    }

    if (event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_UNCLOAKED) {
        EvaluateWindow(hwnd);
    }
}

// Makes sure that the coordinates that the mod deals with are physical pixels.
void SetPerMonitorDpiAwareness() {
    using SetThreadDpiAwarenessContext_t = HANDLE(WINAPI*)(HANDLE);

    static auto setThreadDpiAwarenessContext =
        reinterpret_cast<SetThreadDpiAwarenessContext_t>(GetProcAddress(
            GetModuleHandleW(L"user32.dll"), "SetThreadDpiAwarenessContext"));

    if (setThreadDpiAwarenessContext) {
        // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
        setThreadDpiAwarenessContext(reinterpret_cast<HANDLE>(-4));
    }
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    SetPerMonitorDpiAwareness();

    // The message queue has to exist before WhTool_ModUninit can post WM_QUIT to
    // this thread.
    MSG msg;
    PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);

    g_showHook = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, nullptr,
                                 WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    g_destroyHook =
        SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY, nullptr,
                        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    g_uncloakHook =
        SetWinEventHook(EVENT_OBJECT_UNCLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr,
                        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    g_restoreHook =
        SetWinEventHook(EVENT_SYSTEM_MINIMIZEEND, EVENT_SYSTEM_MINIMIZEEND,
                        nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    // Windows that are already open are out of scope.
    EnumWindows(MarkExistingWindowsProc, 0);

    while (!g_unloading && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_applyTimerId) {
        KillTimer(nullptr, g_applyTimerId);
        g_applyTimerId = 0;
    }

    HWINEVENTHOOK* hooks[] = {&g_showHook, &g_destroyHook, &g_uncloakHook,
                              &g_restoreHook};
    for (HWINEVENTHOOK* hook : hooks) {
        if (*hook) {
            UnhookWinEvent(*hook);
            *hook = nullptr;
        }
    }

    return 0;
}

}  // namespace

BOOL WhTool_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0,
                                  &g_workerThreadId);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Settings changed");

    LoadSettings();
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");

    g_unloading = true;

    if (g_workerThread) {
        if (g_workerThreadId) {
            PostThreadMessageW(g_workerThreadId, WM_QUIT, 0, 0);
        }

        // The worker never waits for another thread to respond, so the join is
        // short and always completes.
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        g_workerThreadId = 0;
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

    // Windhawk starts the tool process from its own process, and that process
    // runs at the user's privilege level. The mod is also injected into Task
    // Manager, whose windows are elevated and can't be changed from there, so
    // the worker runs inside Task Manager and manages its windows.
    if (!IsWindhawkProcess()) {
        g_managesOtherProcesses = false;
        g_isInProcessWorker = true;
        g_processId = GetCurrentProcessId();
        if (!WhTool_ModInit()) {
            return FALSE;
        }

        return TRUE;
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

    // The tool process exists only to host the worker, so it exits here. The
    // worker that runs inside an injected application process must leave that
    // process running.
    if (!g_isInProcessWorker) {
        ExitProcess(0);
    }
}
