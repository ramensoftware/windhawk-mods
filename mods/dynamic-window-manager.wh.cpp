// ==WindhawkMod==
// @id              dynamic-window-manager
// @name            Dynamic Window Manager
// @description     Sizes and positions new application windows according to user-defined per-application rules
// @version         1.0.0
// @author          Armaninyow
// @github          https://github.com/armaninyow
// @include         explorer.exe
// @include         taskmgr.exe
// @compilerOptions -luser32 -ldwmapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Dynamic Window Manager

Automatically sizes and positions application windows based on rules that you
define in the mod's settings. Each rule matches an executable by its process
name and specifies the target size, the anchor corner of the monitor to place
the window at, and the monitor to use.

Example rules:

| Process name | Width | Height | Horizontal anchor | Vertical anchor | Monitor |
|--------------|-------|--------|-------------------|-----------------|---------|
| `chrome.exe` | 1600  | 1000   | Right             | Top             | 2       |
| `notepad.exe`| 800   | 600    | Left              | Bottom          | 1       |

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

// How often the worker thread polls for new windows and processes the windows
// that are due for a rule.
constexpr DWORD kPollIntervalMs = 150;

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
    int width = kMinimumDimension;
    int height = kMinimumDimension;
    HorizontalAnchor horizontalAnchor = HorizontalAnchor::Left;
    VerticalAnchor verticalAnchor = VerticalAnchor::Top;
    int monitor = kDefaultMonitor;
};

struct PendingWindow {
    HWND hwnd = nullptr;
    ULONGLONG due = 0;
};

// Window rules, replaced as a whole when the settings change. Guarded by
// g_rulesLock because settings can be reloaded from another thread.
std::mutex g_rulesLock;
std::vector<Rule> g_rules;

HANDLE g_workerThread = nullptr;
DWORD g_workerThreadId = 0;
HWINEVENTHOOK g_showHook = nullptr;
HWINEVENTHOOK g_destroyHook = nullptr;
std::atomic<bool> g_unloading{false};
std::atomic<bool> g_workerStarted{false};
HANDLE g_wakeEvent = nullptr;

// True for the copy of the mod that runs inside explorer. That copy manages the
// windows of every process, while a copy that runs inside an application, such
// as Task Manager, can only manage that application's own windows: Windows
// rejects window changes coming from a process of a lower integrity level.
bool g_managesOtherProcesses = false;

// The members below are only accessed from the worker thread.
std::set<HWND> g_seenWindows;
std::deque<PendingWindow> g_pendingWindows;

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

bool CurrentProcessIsExplorer() {
    wchar_t path[MAX_PATH];
    if (!GetModuleFileNameW(nullptr, path, ARRAYSIZE(path))) {
        return false;
    }

    return lstrcmpiW(FileNameOf(path), L"explorer.exe") == 0;
}

// Decides whether this copy of the mod is allowed to change a window. Only the
// copy that runs in explorer manages the windows of other processes; a copy
// that runs inside an application is limited to that application's own windows.
bool ShouldManageWindow(HWND hwnd) {
    if (g_managesOtherProcesses) {
        return true;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    return processId && processId == GetCurrentProcessId();
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

void ApplyRule(HWND hwnd, const Rule& rule) {
    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd)) {
        return;
    }

    HMONITOR monitor = GetMonitorByIndex(rule.monitor);

    MONITORINFO monitorInfo;
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return;
    }

    const RECT& workArea = monitorInfo.rcWork;
    int workWidth = workArea.right - workArea.left;
    int workHeight = workArea.bottom - workArea.top;

    // A maximized window can't take the requested size.
    if (IsZoomed(hwnd)) {
        ShowWindow(hwnd, SW_RESTORE);
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

    // This fails for the windows of elevated applications, because Windows does
    // not let a process of a lower integrity level move them. A copy of the mod
    // that runs inside such an application takes care of its window instead,
    // so a failure here is expected and nothing else has to be done about it.
    SetWindowPos(hwnd, nullptr, x, y, width, height,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

void ProcessPendingWindows() {
    ULONGLONG now = GetTickCount64();

    std::vector<HWND> ready;
    while (!g_pendingWindows.empty() && g_pendingWindows.front().due <= now) {
        ready.push_back(g_pendingWindows.front().hwnd);
        g_pendingWindows.pop_front();
    }

    for (HWND hwnd : ready) {
        if (!IsWindow(hwnd)) {
            continue;
        }

        // Look the rule up again in case the settings changed meanwhile.
        std::wstring processName;
        if (!GetWindowProcessName(hwnd, processName)) {
            continue;
        }

        Rule rule;
        if (FindRuleForProcess(processName, &rule)) {
            ApplyRule(hwnd, rule);
        }
    }
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

    g_pendingWindows.push_back({hwnd, GetTickCount64() + kApplyDelayMs});
    Wh_Log(L"Scheduled rule '%s' for window %p", rule.processName.c_str(), hwnd);
}

BOOL CALLBACK FindNewWindowsProc(HWND hwnd, LPARAM) {
    if (IsCandidateWindow(hwnd)) {
        EvaluateWindow(hwnd);
    }

    return TRUE;
}

// Fallback for missed window event notifications.
void PollForNewWindows() {
    EnumWindows(FindNewWindowsProc, 0);
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
    if (g_unloading) {
        return;
    }

    if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }

    if (event == EVENT_OBJECT_DESTROY) {
        g_seenWindows.erase(hwnd);
        return;
    }

    if (event == EVENT_OBJECT_SHOW) {
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

    g_showHook = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, nullptr,
                                 WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    g_destroyHook =
        SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY, nullptr,
                        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    if (g_managesOtherProcesses) {
        // Windows that are already open are out of scope.
        EnumWindows(MarkExistingWindowsProc, 0);
    }

    // The in-application copy deliberately does not mark existing windows: the
    // application may have created its window before the mod was loaded. Only
    // the application's own windows are considered anyway.

    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

    // The loop pumps messages, which is also what delivers the window event
    // callbacks, and polls for new windows between messages. Timers are not used
    // because timer messages are not reliably delivered to this thread.
    MSG msg;
    bool quitting = false;

    while (!quitting) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                quitting = true;
                break;
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (quitting || g_unloading) {
            break;
        }

        PollForNewWindows();
        ProcessPendingWindows();

        HANDLE handles[] = {g_wakeEvent};
        MsgWaitForMultipleObjects(handles[0] ? 1 : 0, handles, FALSE,
                                  kPollIntervalMs, QS_ALLINPUT);
    }

    if (g_showHook) {
        UnhookWinEvent(g_showHook);
        g_showHook = nullptr;
    }

    if (g_destroyHook) {
        UnhookWinEvent(g_destroyHook);
        g_destroyHook = nullptr;
    }

    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = nullptr;
    }

    return 0;
}

}  // namespace

void StartWorkerThread();

BOOL Wh_ModInit() {
    g_managesOtherProcesses = CurrentProcessIsExplorer();

    Wh_Log(L"Init, mode=%s",
           g_managesOtherProcesses ? L"all processes" : L"own process");

    LoadSettings();

    StartWorkerThread();

    return TRUE;
}

void Wh_ModAfterInit() {
    StartWorkerThread();
}

void StartWorkerThread() {
    // The thread is started from both Wh_ModInit and Wh_ModAfterInit, whichever
    // comes first, so that the mod works even if one of them isn't called.
    bool expected = false;
    if (!g_workerStarted.compare_exchange_strong(expected, true)) {
        return;
    }

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0,
                                  &g_workerThreadId);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    g_unloading = true;

    if (g_workerThread) {
        if (g_wakeEvent) {
            SetEvent(g_wakeEvent);
        }

        if (g_workerThreadId) {
            PostThreadMessageW(g_workerThreadId, WM_QUIT, 0, 0);
        }

        WaitForSingleObject(g_workerThread, 3000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        g_workerThreadId = 0;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");

    LoadSettings();
}
