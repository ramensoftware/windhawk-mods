// ==WindhawkMod==
// @id              rovan-style
// @name            Rovan Style
// @description     Moves and resizes the windows of selected programs to a set position and size when they open.
// @version         1.0
// @author          Roey Qian
// @github          https://github.com/roeyqian
// @include         *
// @architecture    x86-64
// @architecture    x86
// @compilerOptions -luser32 -ldwmapi -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Rovan Style

## Overview

**Rovan Style** is a [Windhawk](https://windhawk.net/) window layout mod that automatically positions and resizes application windows. When a window of a matching program appears, it is moved to the configured position and size, optionally followed by native Windows vertical maximization, with a separate rule for each program.

## Options

The settings below apply to every program the mod is loaded into. Changes are applied live, without restarting the programs.

1. **Delay after opening (ms):** How long to wait after a window appears before adjusting it, so the program can finish initializing. The default is `300`, and the range is 0–30000.
2. **Startup correction period (ms):** How long after the first adjustment the mod keeps correcting any layout the program restores by itself. The default is `1000`, the range is 0–30000, and `0` adjusts only once.
3. **Apply to already open windows:** Also adjusts matching windows that are already open when the mod is enabled or when the program's own rules change.
4. **Compatibility with programs' position/size saving:** Sends paired move/size begin and end notifications around each adjustment, so programs that save their layout at the end of a manual drag update their own records. Such programs keep opening at that layout even after the mod is disabled.
5. **Program rules:** Gives specific programs their own window layout. Each rule consists of:
   - **Program:** The program to match, as a process name (`notepad.exe`) or a full executable path (`C:\Program Files\Example\Example.exe`). An empty value skips the rule.
   - **Window position (X,Y):** The top-left corner of the window, as two integers, e.g. `100,100` or `-1920,100`.
   - **Window size (Width,Height):** The size of the window, as two non-negative integers, e.g. `1280,800`. A value of `0` keeps the current size in that direction.
   - **Vertical maximize:** Applies the normal position and size first, then enters native Windows vertical maximization.
   - **Include dialogs and tool windows:** Also handles the program's dialogs, tool windows and borderless windows.

## Behavior

A rule matches a program by its process name or full executable path, case-insensitively, and environment variables such as `%ProgramFiles%` are expanded. For example, a rule for `Example.exe` matches that program in any folder, while a rule for `%ProgramFiles%\Example\Example.exe` matches only that executable. Folders and wildcards are not supported. Rules are checked in order and the first valid matching rule wins, so put more specific rules above broader ones.

Positions and sizes are in physical pixels on the whole virtual desktop, where the top-left corner of the primary monitor is `(0, 0)`, and the size is the full window rectangle including the frame and the title bar. Only ordinary top-level windows are handled unless *Include dialogs and tool windows* is turned on, and IME windows are always excluded. The rule is applied after *Delay after opening* and corrected during *Startup correction period*. After that, the window can be moved and resized freely. A window that is hidden and shown again, for example a program that hides to the tray instead of closing, is treated as a new window and adjusted again.

The mod is injected through Windhawk's default `*` target process scope, but it unloads right away from programs without a valid matching rule, and each program adjusts only its own windows. Elevated programs therefore adjust their windows in their own privilege context, without being added to the target process list.

With *Vertical maximize* turned on, Windows controls the top and bottom edges of the window, while X and the width still follow the rule, and Y and the height are kept as the restore position. It uses native Windows vertical maximization, which requires Windows 10, version 1903 or later. When that isn't possible, the normal layout is kept.

## Notes

- Keep Windhawk's default `*` target process scope, and make sure Windhawk can load into the program. Regular installations use a service, while portable mode may require administrator rights for elevated programs.
- A rule with an invalid position or size format is skipped, so windows are never moved by mistake.
- For modern apps, enter the exe that actually owns the window, because some apps use a host process.
- The secure desktop, protected programs, programs that block mod loading, exclusive fullscreen and a program's own minimum or maximum size limits may still prevent adjustment.
- Whether a program actually saves the adjusted layout is up to the program itself, even with *Compatibility with programs' position/size saving* turned on.
- Disabling the mod stops monitoring, and adjusted windows keep their current position and size instead of returning to their original layout.
- Only the x86 and x86-64 architectures are supported.
- This mod is currently developed primarily for Windows 11, version 24H2. Compatibility and operational stability on other Windows versions are not guaranteed at this time, since the mod relies on how Windows itself handles window sizing, snapping and native vertical maximization, which can change between Windows builds.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ApplyDelayMs: 300
  $name: 🔶 Delay after opening (ms)
  $description: >-
    How long to wait after a window appears before adjusting it, so the program can finish initializing. Range 0–30000.
     ✨ It is recommended to increase this for slow-starting programs.
- SettleTimeMs: 1000
  $name: 🔶 Startup correction period (ms)
  $description: >-
    How long to keep correcting any layout the program restores by itself after the first adjustment. Range 0–30000.
     0 means adjust only once.
- ApplyToExistingWindows: true
  $name: 🔷 Apply to already open windows
  $description: >-
    Also adjust matching windows that are already open when the mod is enabled or the program's own rules change.
- NotifySizeMove: true
  $name: 🔷 Compatibility with programs' position/size saving
  $description: >-
    Send paired move/size begin and end notifications while adjusting, so programs that save their layout at the end of a manual drag update their own records.
     ✨ No mouse input is simulated.
- Rules:
  - - Program: ""
      $name: 🔶 Program
      $description: >-
        Entries can be process names or full executable paths, environment variables are supported, for example:
         • notepad.exe
         • C:\Program Files\Example\Example.exe
         • %ProgramFiles%\Example\Example.exe
         Leave empty to skip this rule.
    - Position: "100,100"
      $name: 🔶 Window position (X,Y)
      $description: >-
        Two comma-separated integers, in physical pixels on the whole virtual desktop, for example:
         • 100,100
         • -1920,100 (a monitor to the left of the primary monitor)
    - Size: "1280,800"
      $name: 🔶 Window size (Width,Height)
      $description: >-
        Two comma-separated non-negative integers, in physical pixels and including the window frame, for example 1280,800.
         0 means keep the current size in that direction.
    - VerticalMaximize: false
      $name: 🔷 Vertical maximize
      $description: >-
        Apply the normal position and size first, then enter native Windows vertical maximization. Y and the height are kept as the restore position.
         ❗ Windows 10, version 1903 or later is required, and the window must support system sizing.
    - IncludeDialogs: false
      $name: 🔷 Include dialogs and tool windows
      $description: >-
        Also handle the program's dialogs, tool windows and borderless windows.
         ✨ IME windows are always excluded.
  $name: ⏩ Program Rules
  $description: >-
    Give specific programs their own window layout. Rules are matched in order and the first valid rule wins, up to 256 rules.
     ❗ Keep Windhawk's default target process scope (*), otherwise the rules can't reach the programs they are written for.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <cwchar>
#include <cwctype>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

constexpr int kMaxRules = 256;
constexpr ULONGLONG kRetryIntervalMs = 150;
constexpr ULONGLONG kPlacementWaitMs = 1000;
constexpr UINT kNotificationTimeoutMs = 100;
constexpr ULONGLONG kRuleRetryMs = 2000;

struct Rule {
    std::wstring program;
    bool fullPath;
    int x;
    int y;
    int width;
    int height;
    bool includeDialogs;
    bool verticalMaximize = false;

    bool operator==(const Rule& other) const {
        return program == other.program && fullPath == other.fullPath && x == other.x &&
               y == other.y && width == other.width && height == other.height &&
               includeDialogs == other.includeDialogs &&
               verticalMaximize == other.verticalMaximize;
    }
};

struct Settings {
    std::vector<Rule> rules;
    int delayMs;
    int settleMs;
    bool applyToExisting;
    bool notifySizeMove = true;
};

struct NativeVerticalTask {
    WPARAM id;
    std::atomic<bool> completed{false};
    std::atomic<bool> cancelled{false};
};

struct WindowState {
    DWORD processId;
    DWORD threadId;
    // -1 means observed, but not selected by a rule.
    int ruleIndex = -1;
    ULONGLONG nextAttempt = 0;
    ULONGLONG deadline = 0;
    bool pending = false;
    // Pending, but minimized or cloaked. Not polled; window events wake it.
    bool waitingUntilUsable = false;
    // A short poll after a wake event, in case the state changes just after it.
    ULONGLONG usableWakeDeadline = 0;
    bool restoreRequested = false;
    bool reported = false;
    bool placementPending = false;
    bool sizeMoveStarted = false;
    bool notificationFailed = false;
    ULONGLONG placementDeadline = 0;
    RECT requestedRect{};
    bool preparingVerticalMaximize = false;
    bool normalRectEstablished = false;
    RECT normalRect{};
    bool verticalErrorReported = false;
    bool nativeVerticalAttempted = false;
    std::shared_ptr<NativeVerticalTask> nativeVerticalTask{};
    ULONGLONG ruleRetryDeadline = 0;
    ULONGLONG nextRuleRetry = 0;
    bool retryRuleOnEvent = false;
};

struct DeferredWindowEvent {
    DWORD event;
    HWND hwnd;
    LONG objectId;
    LONG childId;
};

// Only the event-loop thread accesses the window map and its settings snapshot.
struct Controller {
    std::shared_ptr<const Settings> settings;
    std::unordered_map<HWND, WindowState> windows;
    bool processingWindows = false;
    std::vector<DeferredWindowEvent> deferredEvents{};
};

std::mutex g_settingsMutex;
std::shared_ptr<const Settings> g_settings;
HANDLE g_stopEvent = nullptr;
HANDLE g_reloadEvent = nullptr;
HANDLE g_thread = nullptr;
Controller* g_controller = nullptr;
std::atomic<bool> g_stopping{false};
std::wstring g_processPath;
std::wstring g_processName;

// Native vertical maximization runs on each window's own thread through a
// subclass. The controller posts a request; the window thread completes it.
UINT g_nativeVerticalMessage = 0;
std::mutex g_nativeMutex;
std::unordered_map<HWND, std::shared_ptr<NativeVerticalTask>> g_nativeTasks;
std::vector<HWND> g_subclassedWindows;
WPARAM g_nextNativeTaskId = 0;
std::atomic<int> g_nativeCallsInProgress{0};

std::wstring NormalizeProgram(std::wstring value) {
    const size_t first = value.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) {
        return {};
    }
    value = value.substr(first, value.find_last_not_of(L" \t\r\n") - first + 1);
    if (value.size() >= 2 && value.front() == L'"' && value.back() == L'"') {
        value = value.substr(1, value.size() - 2);
    }

    DWORD length = ExpandEnvironmentStringsW(value.c_str(), nullptr, 0);
    if (length > 0 && length <= 32768) {
        std::vector<wchar_t> expanded(length);
        DWORD copied = ExpandEnvironmentStringsW(value.c_str(), expanded.data(), length);
        if (copied > 0 && copied <= length) {
            value.assign(expanded.data());
        }
    }
    std::replace(value.begin(), value.end(), L'/', L'\\');
    if (value.compare(0, 4, L"\\\\?\\") == 0) {
        if (value.compare(4, 4, L"UNC\\") == 0) {
            value = L"\\\\" + value.substr(8);
        } else {
            value.erase(0, 4);
        }
    }
    return value;
}

bool EqualProgram(const std::wstring& a, const std::wstring& b) {
    return CompareStringOrdinal(a.c_str(), -1, b.c_str(), -1, TRUE) == CSTR_EQUAL;
}

bool ParseIntegerPair(PCWSTR text, int& first, int& second) {
    long long values[2]{};
    const wchar_t* cursor = text;
    for (int i = 0; i < 2; ++i) {
        while (std::iswspace(*cursor)) {
            ++cursor;
        }
        wchar_t* end = nullptr;
        errno = 0;
        values[i] = std::wcstoll(cursor, &end, 10);
        if (end == cursor || errno == ERANGE || values[i] < std::numeric_limits<int>::min() ||
            values[i] > std::numeric_limits<int>::max()) {
            return false;
        }
        cursor = end;
        while (std::iswspace(*cursor)) {
            ++cursor;
        }
        if (i == 0) {
            if (*cursor != L',' && *cursor != L'\uFF0C') {
                return false;
            }
            ++cursor;
        } else if (*cursor != L'\0') {
            return false;
        }
    }
    first = static_cast<int>(values[0]);
    second = static_cast<int>(values[1]);
    return true;
}

std::shared_ptr<const Settings> ReadSettings() {
    auto settings = std::make_shared<Settings>();
    settings->delayMs = std::clamp(Wh_GetIntSetting(L"ApplyDelayMs"), 0, 30000);
    settings->settleMs = std::clamp(Wh_GetIntSetting(L"SettleTimeMs"), 0, 30000);
    settings->applyToExisting = Wh_GetIntSetting(L"ApplyToExistingWindows") != 0;
    settings->notifySizeMove = Wh_GetIntSetting(L"NotifySizeMove") != 0;

    // The settings API returns an empty string for both missing and blank values.
    // Rows added in the settings UI start with a default position, so a slot
    // with both an empty program and an empty position is the end of the list.
    for (int i = 0; i < kMaxRules; ++i) {
        auto text = WindhawkUtils::StringSetting::make(L"Rules[%d].Program", i);
        std::wstring program = NormalizeProgram(text.get());
        if (program.empty()) {
            auto position = WindhawkUtils::StringSetting::make(L"Rules[%d].Position", i);
            if (!*position.get()) {
                break;
            }
            continue;
        }

        const bool fullPath = program.find_first_of(L"\\:") != std::wstring::npos;
        if (program.find_first_of(L"*?") != std::wstring::npos ||
            (fullPath && !(program.size() >= 3 && program[1] == L':' && program[2] == L'\\') &&
             program.compare(0, 2, L"\\\\") != 0)) {
            Wh_Log(L"Rule %d ignored: use an executable name or absolute path", i + 1);
            continue;
        }
        // Keep only this process's rules. The mod has no cross-process window
        // controller, so elevated and standard applications use the same path.
        if (!EqualProgram(program, fullPath ? g_processPath : g_processName)) {
            continue;
        }
        int x = 0, y = 0, width = 0, height = 0;
        auto position = WindhawkUtils::StringSetting::make(L"Rules[%d].Position", i);
        bool positionValid = ParseIntegerPair(position, x, y);
        auto size = WindhawkUtils::StringSetting::make(L"Rules[%d].Size", i);
        bool sizeValid = ParseIntegerPair(size, width, height);
        if (!positionValid || !sizeValid) {
            Wh_Log(L"Rule %d ignored: position must be X,Y and size must be Width,Height", i + 1);
            continue;
        }
        // Keep arithmetic and window-manager coordinates in a practical range.
        if (width < 0 || width > 100000 || height < 0 || height > 100000 || x < -100000 ||
            x > 100000 || y < -100000 || y > 100000) {
            Wh_Log(L"Rule %d ignored: invalid size or out-of-range coordinates", i + 1);
            continue;
        }
        settings->rules.push_back({std::move(program), fullPath, x, y, width, height,
                                   Wh_GetIntSetting(L"Rules[%d].IncludeDialogs", i) != 0,
                                   Wh_GetIntSetting(L"Rules[%d].VerticalMaximize", i) != 0});
    }
    return settings;
}

std::shared_ptr<const Settings> SettingsSnapshot() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

bool IsCandidate(HWND hwnd) {
    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd) || GetAncestor(hwnd, GA_ROOT) != hwnd ||
        (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD)) {
        return false;
    }
    wchar_t className[128]{};
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    for (PCWSTR excluded : {L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
                            // Input method (IME/TSF) candidate and composition windows.
                            L"IME", L"MSCTFIME UI", L"Microsoft.IME.UIManager.CandidateWindow.Host",
                            L"Windows.UI.Composition.DesktopWindowContentBridge",
                            L"Microsoft.UI.Content.PopupWindowSiteBridge"}) {
        if (lstrcmpiW(className, excluded) == 0) {
            return false;
        }
    }
    return true;
}

// Log why a window was selected so unexpected matches can be identified.
void LogMatch(HWND hwnd, const Rule& rule) {
    wchar_t className[128]{};
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    Wh_Log(L"Matched %s: hwnd=%p class=\"%s\" style=0x%08lX exstyle=0x%08lX owner=%p",
           rule.program.c_str(), hwnd, className,
           static_cast<unsigned long>(GetWindowLongPtrW(hwnd, GWL_STYLE)),
           static_cast<unsigned long>(GetWindowLongPtrW(hwnd, GWL_EXSTYLE)),
           GetWindow(hwnd, GW_OWNER));
}

// ReadSettings keeps only this process's rules, and only this process's
// windows are observed, so matching reduces to the dialog/tool window check.
int MatchRule(HWND hwnd, const Settings& settings) {
    // Borderless popups (no title bar and no resizable frame), such as IME
    // panels, are treated like tool windows and need "include dialogs".
    const LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    const bool hasFrame = (style & WS_CAPTION) == WS_CAPTION || (style & WS_THICKFRAME);
    bool isDialogOrTool =
        GetWindow(hwnd, GW_OWNER) != nullptr || !hasFrame ||
        (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE));
    for (size_t i = 0; i < settings.rules.size(); ++i) {
        if (!isDialogOrTool || settings.rules[i].includeDialogs) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool IsSameWindow(HWND hwnd, const WindowState& state) {
    DWORD processId = 0;
    DWORD threadId = GetWindowThreadProcessId(hwnd, &processId);
    return threadId != 0 && threadId == state.threadId && processId == state.processId;
}

bool SendSizeMoveNotification(HWND hwnd, UINT message) {
    DWORD_PTR result = 0;
    // Don't dispatch WinEvent callbacks while holding references into the map.
    // The dedicated controller thread never attaches to a target's input queue.
    return SendMessageTimeoutW(hwnd, message, 0, 0,
                               SMTO_BLOCK | SMTO_ABORTIFHUNG | SMTO_ERRORONEXIT,
                               kNotificationTimeoutMs, &result) != 0;
}

void EndSizeMove(HWND hwnd, WindowState& state) {
    if (!state.sizeMoveStarted) {
        return;
    }
    state.sizeMoveStarted = false;
    if (IsSameWindow(hwnd, state) && !SendSizeMoveNotification(hwnd, WM_EXITSIZEMOVE)) {
        // A timed-out handler may still finish. Queue its matching end rather
        // than leaving a responsive-again application in the synthetic session.
        PostMessageW(hwnd, WM_EXITSIZEMOVE, 0, 0);
        state.notificationFailed = true;
        Wh_Log(L"Size/move end notification timed out or failed for %p", hwnd);
    }
}

void CancelNativeVerticalTask(HWND hwnd, WindowState& state) {
    if (state.nativeVerticalTask) {
        state.nativeVerticalTask->cancelled.store(true, std::memory_order_release);
        std::lock_guard<std::mutex> lock(g_nativeMutex);
        auto found = g_nativeTasks.find(hwnd);
        if (found != g_nativeTasks.end() && found->second == state.nativeVerticalTask) {
            g_nativeTasks.erase(found);
        }
        state.nativeVerticalTask.reset();
    }
}

void ClearWindowStates() {
    for (auto& [hwnd, state] : g_controller->windows) {
        CancelNativeVerticalTask(hwnd, state);
        EndSizeMove(hwnd, state);
    }
    g_controller->windows.clear();
}

void ScheduleWindow(WindowState& state, const Settings& settings) {
    state.pending = true;
    state.waitingUntilUsable = false;
    state.ruleRetryDeadline = 0;
    state.nextAttempt = GetTickCount64() + settings.delayMs;
    state.deadline = state.nextAttempt + settings.settleMs;
}

void RetryWindowRule(HWND hwnd, WindowState& state, const Settings& settings, bool event = false) {
    ULONGLONG now = GetTickCount64();
    if (event && state.retryRuleOnEvent && state.ruleIndex < 0) {
        state.ruleRetryDeadline = now + kRuleRetryMs;
        state.nextRuleRetry = 0;
    }
    if (!state.ruleRetryDeadline || now < state.nextRuleRetry) {
        return;
    }
    if (now > state.ruleRetryDeadline) {
        state.ruleRetryDeadline = 0;
        return;
    }
    state.nextRuleRetry = now + kRetryIntervalMs;
    state.ruleIndex = MatchRule(hwnd, settings);
    if (state.ruleIndex >= 0) {
        LogMatch(hwnd, settings.rules[state.ruleIndex]);
        ScheduleWindow(state, settings);
    }
}

void ObserveWindow(HWND hwnd, bool schedule) {
    if (!g_controller || g_stopping.load() || !IsCandidate(hwnd)) {
        return;
    }
    auto& controller = *g_controller;
    DWORD processId = 0;
    DWORD threadId = GetWindowThreadProcessId(hwnd, &processId);
    if (!threadId || processId != GetCurrentProcessId()) {
        return;
    }
    auto found = controller.windows.find(hwnd);
    if (found != controller.windows.end()) {
        if (found->second.processId == processId && found->second.threadId == threadId) {
            if (schedule && found->second.waitingUntilUsable) {
                // Restore, uncloak and foreground events can arrive just before
                // the window reports the new state. Poll briefly to catch it.
                found->second.usableWakeDeadline = GetTickCount64() + kPlacementWaitMs;
            }
            RetryWindowRule(hwnd, found->second, *controller.settings, schedule);
            return;
        }
        CancelNativeVerticalTask(hwnd, found->second);
        controller.windows.erase(found);
    }
    WindowState state{processId, threadId};
    if (schedule) {
        state.ruleIndex = MatchRule(hwnd, *controller.settings);
        if (state.ruleIndex >= 0) {
            LogMatch(hwnd, controller.settings->rules[state.ruleIndex]);
            ScheduleWindow(state, *controller.settings);
        } else {
            // Do not permanently cache a transient startup style/owner failure.
            // Deliberately skipped existing windows get no retry budget.
            state.ruleRetryDeadline =
                GetTickCount64() + kRuleRetryMs + controller.settings->delayMs;
            state.nextRuleRetry = GetTickCount64() + kRetryIntervalMs;
            state.retryRuleOnEvent = true;
        }
    }
    controller.windows.emplace(hwnd, state);
}

BOOL CALLBACK EnumerateWindow(HWND hwnd, LPARAM schedule) {
    ObserveWindow(hwnd, schedule != 0);
    return !g_stopping.load();
}

void CALLBACK
WindowEvent(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG objectId, LONG childId, DWORD, DWORD) {
    if (!g_controller || g_stopping.load() || !hwnd) {
        return;
    }
    if (g_controller->processingWindows) {
        g_controller->deferredEvents.push_back({event, hwnd, objectId, childId});
        return;
    }
    if (event == EVENT_SYSTEM_FOREGROUND || event == EVENT_SYSTEM_MINIMIZEEND) {
        ObserveWindow(hwnd, true);
        return;
    }
    if (objectId != OBJID_WINDOW || childId != CHILDID_SELF) {
        return;
    }
    if (event == EVENT_OBJECT_DESTROY || event == EVENT_OBJECT_HIDE) {
        auto found = g_controller->windows.find(hwnd);
        if (found != g_controller->windows.end()) {
            CancelNativeVerticalTask(hwnd, found->second);
            EndSizeMove(hwnd, found->second);
            g_controller->windows.erase(found);
        }
    } else if (event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_UNCLOAKED) {
        ObserveWindow(hwnd, true);
    }
}

bool IsCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
           cloaked;
}

using IsWindowArranged_t = BOOL(WINAPI*)(HWND);

IsWindowArranged_t NativeArrangementQuery() {
    static auto query = reinterpret_cast<IsWindowArranged_t>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "IsWindowArranged"));
    return query;
}

bool IsArranged(HWND hwnd) {
    auto query = NativeArrangementQuery();
    return query && query(hwnd);
}

bool GetNormalRect(HWND hwnd, RECT& rect) {
    WINDOWPLACEMENT placement{};
    placement.length = sizeof(placement);
    if (!GetWindowPlacement(hwnd, &placement)) {
        return false;
    }
    rect = placement.rcNormalPosition;
    // WINDOWPLACEMENT uses workspace coordinates for ordinary top-level windows.
    // Convert back to physical screen coordinates before comparing rule values.
    if (!(GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)) {
        MONITORINFO info{};
        info.cbSize = sizeof(info);
        if (!GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &info)) {
            return false;
        }
        OffsetRect(&rect, info.rcWork.left - info.rcMonitor.left,
                   info.rcWork.top - info.rcMonitor.top);
    }
    return true;
}

bool IsNativeVerticalMaximized(HWND hwnd) {
    if (!IsArranged(hwnd) || IsZoomed(hwnd) || IsIconic(hwnd)) {
        return false;
    }
    RECT rect{};
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    return GetWindowRect(hwnd, &rect) &&
           GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &info) &&
           rect.top <= info.rcWork.top && rect.bottom >= info.rcWork.bottom;
}

bool NativeVerticalMatchesRule(HWND hwnd, const WindowState& state, const Rule& rule) {
    if (!rule.verticalMaximize || !IsNativeVerticalMaximized(hwnd)) {
        return false;
    }
    RECT normal{}, actual{};
    if (!GetNormalRect(hwnd, normal) || !GetWindowRect(hwnd, &actual)) {
        return false;
    }
    if (state.normalRectEstablished) {
        return EqualRect(&normal, &state.normalRect) && actual.left == normal.left &&
               actual.right - actual.left == normal.right - normal.left;
    }
    return normal.left == rule.x && normal.top == rule.y &&
           (!rule.width || normal.right - normal.left == rule.width) &&
           (!rule.height || normal.bottom - normal.top == rule.height) &&
           actual.left == normal.left && actual.right - actual.left == normal.right - normal.left;
}

// Runs on the window's own thread, before the application's window procedure.
LRESULT CALLBACK NativeVerticalSubclassProc(HWND hwnd,
                                            UINT message,
                                            WPARAM wParam,
                                            LPARAM lParam,
                                            DWORD_PTR) {
    if (message == WM_NCDESTROY) {
        // The subclass wrapper removes itself on WM_NCDESTROY.
        std::lock_guard<std::mutex> lock(g_nativeMutex);
        g_nativeTasks.erase(hwnd);
        g_subclassedWindows.erase(
            std::remove(g_subclassedWindows.begin(), g_subclassedWindows.end(), hwnd),
            g_subclassedWindows.end());
    } else if (message == g_nativeVerticalMessage && g_nativeVerticalMessage) {
        std::shared_ptr<NativeVerticalTask> task;
        {
            std::lock_guard<std::mutex> lock(g_nativeMutex);
            auto found = g_nativeTasks.find(hwnd);
            if (found != g_nativeTasks.end() && found->second->id == wParam) {
                task = std::move(found->second);
                g_nativeTasks.erase(found);
                // Count the call under the lock that hands out the task, so the
                // teardown can't miss a call that has taken its task.
                g_nativeCallsInProgress.fetch_add(1);
            }
        }
        if (task) {
            RECT rect{};
            if (!task->cancelled.load(std::memory_order_acquire) && IsWindowVisible(hwnd) &&
                !IsIconic(hwnd) && GetWindowRect(hwnd, &rect)) {
                // WinUI/Windows App SDK can swallow this message sent to the app.
                // The default handler performs native arrangement and preserves
                // the normal rectangle. The point is computed here, in the
                // window's own DPI context.
                DefWindowProcW(
                    hwnd, WM_NCLBUTTONDBLCLK, HTTOP,
                    MAKELPARAM(static_cast<SHORT>(rect.left + (rect.right - rect.left) / 2),
                               static_cast<SHORT>(rect.top)));
            }
            task->completed.store(true, std::memory_order_release);
            // At teardown this may be the last reference. Release it before the
            // decrement so no task cleanup runs after the teardown stops waiting.
            task.reset();
            g_nativeCallsInProgress.fetch_sub(1);
        }
        return 0;
    }
    return DefSubclassProc(hwnd, message, wParam, lParam);
}

bool EnsureNativeVerticalSubclass(HWND hwnd) {
    {
        std::lock_guard<std::mutex> lock(g_nativeMutex);
        if (std::find(g_subclassedWindows.begin(), g_subclassedWindows.end(), hwnd) !=
            g_subclassedWindows.end()) {
            return true;
        }
    }
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, NativeVerticalSubclassProc, 0)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(g_nativeMutex);
    g_subclassedWindows.push_back(hwnd);
    return true;
}

// Called on the controller thread before it exits, so Wh_ModUninit covers it.
void RemoveNativeVerticalSubclasses() {
    std::vector<HWND> windows;
    {
        std::lock_guard<std::mutex> lock(g_nativeMutex);
        windows.swap(g_subclassedWindows);
        // No new native call can start after this.
        g_nativeTasks.clear();
    }
    // Wait before removing the subclasses, so the removal can't run nested
    // inside the default handler, which can process sent messages.
    while (g_nativeCallsInProgress.load() > 0) {
        Sleep(10);
    }
    // Each removal returns only after the window's thread has handled it, which
    // happens after that thread has left the subclass procedure.
    for (HWND hwnd : windows) {
        DWORD processId = 0;
        if (GetWindowThreadProcessId(hwnd, &processId) && processId == GetCurrentProcessId()) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, NativeVerticalSubclassProc);
        }
    }
}

bool ApplyNativeVerticalMaximize(HWND hwnd, WindowState& state) {
    if (state.nativeVerticalAttempted) {
        if (!state.verticalErrorReported) {
            Wh_Log(L"The system did not vertically maximize %p; keeping its normal layout", hwnd);
            state.verticalErrorReported = true;
        }
        return true;
    }
    if (!NativeArrangementQuery() || !(GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_THICKFRAME)) {
        if (!state.verticalErrorReported) {
            Wh_Log(L"Native vertical maximization is unavailable for %p", hwnd);
            state.verticalErrorReported = true;
        }
        return true;
    }
    DWORD_PTR result = 0;
    if (IsHungAppWindow(hwnd) ||
        !SendMessageTimeoutW(hwnd, WM_NULL, 0, 0, SMTO_BLOCK | SMTO_ABORTIFHUNG | SMTO_ERRORONEXIT,
                             kNotificationTimeoutMs, &result)) {
        return false;
    }
    if (!g_nativeVerticalMessage || !EnsureNativeVerticalSubclass(hwnd)) {
        if (!state.verticalErrorReported) {
            Wh_Log(L"Failed to prepare native vertical maximization for %p", hwnd);
            state.verticalErrorReported = true;
        }
        return true;
    }
    auto task = std::make_shared<NativeVerticalTask>();
    {
        std::lock_guard<std::mutex> lock(g_nativeMutex);
        task->id = ++g_nextNativeTaskId;
        g_nativeTasks[hwnd] = task;
    }
    // Posting never blocks the controller; the window thread handles it.
    if (!PostMessageW(hwnd, g_nativeVerticalMessage, task->id, 0)) {
        std::lock_guard<std::mutex> lock(g_nativeMutex);
        g_nativeTasks.erase(hwnd);
        return true;
    }
    state.nativeVerticalTask = std::move(task);
    state.nativeVerticalAttempted = true;
    return false;
}

// False means a restore is still pending and geometry needs another turn.
bool ApplyRule(HWND hwnd, WindowState& state, const Rule& rule) {
    if (state.nativeVerticalTask) {
        if (!state.nativeVerticalTask->completed.load(std::memory_order_acquire)) {
            return false;
        }
        state.nativeVerticalTask.reset();
    }
    if (NativeVerticalMatchesRule(hwnd, state, rule)) {
        state.nativeVerticalAttempted = false;
        return true;
    }
    if (IsZoomed(hwnd) || IsArranged(hwnd)) {
        if (!state.restoreRequested) {
            // Async calls keep a hung target from blocking Explorer or unloading.
            ShowWindowAsync(hwnd, SW_SHOWNOACTIVATE);
            state.restoreRequested = true;
        }
        return false;
    }
    state.restoreRequested = false;
    RECT rect{};
    if (!GetWindowRect(hwnd, &rect)) {
        return true;
    }
    int width = rule.width ? rule.width : rect.right - rect.left;
    int height = rule.height ? rule.height : rect.bottom - rect.top;
    RECT target{rule.x, rule.y, rule.x + width, rule.y + height};
    // A completed normal-placement transaction may have been clamped by the
    // application. Its accepted normal rectangle is still valid for native snap.
    bool acceptedNormal = state.normalRectEstablished && EqualRect(&rect, &state.normalRect);
    if (EqualRect(&rect, &target) || (rule.verticalMaximize && acceptedNormal)) {
        if (rule.verticalMaximize) {
            state.normalRect = rect;
            state.normalRectEstablished = true;
            return ApplyNativeVerticalMaximize(hwnd, state);
        }
        return true;
    }
    UINT flags = SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_ASYNCWINDOWPOS;
    if (rule.width == 0 && rule.height == 0) {
        flags |= SWP_NOSIZE;
    }
    if (g_controller->settings->notifySizeMove && !state.notificationFailed) {
        // SetWindowPos sends position/size changes, but not the interactive
        // begin/end pair on which several frameworks base layout persistence.
        state.sizeMoveStarted = true;
        if (!SendSizeMoveNotification(hwnd, WM_ENTERSIZEMOVE)) {
            state.notificationFailed = true;
            EndSizeMove(hwnd, state);
            Wh_Log(L"Size/move begin notification timed out or failed for %p", hwnd);
        }
    }
    if (!SetWindowPos(hwnd, nullptr, target.left, target.top, width, height, flags)) {
        EndSizeMove(hwnd, state);
        if (!state.reported) {
            Wh_Log(L"SetWindowPos failed for %p, error %lu", hwnd, GetLastError());
            state.reported = true;
        }
    } else {
        // SWP_ASYNCWINDOWPOS only queues the request. Never send the end
        // notification immediately: an application could save its OLD rectangle.
        state.placementPending = true;
        state.preparingVerticalMaximize = rule.verticalMaximize;
        state.nativeVerticalAttempted = false;
        state.requestedRect = target;
        state.placementDeadline = GetTickCount64() + kPlacementWaitMs;
        if (!state.reported) {
            Wh_Log(L"Requested %s: (%ld, %ld), %d x %d", rule.program.c_str(), target.left,
                   target.top, width, height);
            state.reported = true;
        }
    }
    return !state.preparingVerticalMaximize;
}

void ProcessPendingWindows() {
    auto& controller = *g_controller;
    // The native default handler may dispatch window events synchronously.
    // Defer them until no map references or iterators are being used.
    controller.processingWindows = true;
    ULONGLONG now = GetTickCount64();
    for (auto it = controller.windows.begin(); it != controller.windows.end();) {
        HWND hwnd = it->first;
        WindowState& state = it->second;
        DWORD processId = 0;
        DWORD threadId = GetWindowThreadProcessId(hwnd, &processId);
        if (threadId != state.threadId || processId != state.processId || !IsWindowVisible(hwnd)) {
            CancelNativeVerticalTask(hwnd, state);
            EndSizeMove(hwnd, state);
            it = controller.windows.erase(it);
            continue;
        }
        ++it;
        RetryWindowRule(hwnd, state, *controller.settings);
        if (now < state.nextAttempt || g_stopping.load()) {
            continue;
        }
        if (state.placementPending) {
            RECT actual{};
            bool reached = GetWindowRect(hwnd, &actual) && EqualRect(&actual, &state.requestedRect);
            if (!reached && now < state.placementDeadline && !IsIconic(hwnd) && !IsCloaked(hwnd)) {
                continue;
            }
            state.placementPending = false;
            // Also finish for applications that clamp the requested size.
            // Their own final rectangle is the one they should persist.
            EndSizeMove(hwnd, state);
            if (state.preparingVerticalMaximize) {
                state.preparingVerticalMaximize = false;
                state.normalRectEstablished = GetNormalRect(hwnd, state.normalRect);
                state.pending = true;
            } else if (now >= state.deadline) {
                state.pending = false;
            }
        }
        if (!state.pending) {
            continue;
        }
        // Keep waiting while minimized/cloaked; don't bring hidden desktops forward.
        // This isn't polled: the restore and uncloak events wake the loop. Start
        // the adjustment period only when the window actually becomes usable.
        if (IsIconic(hwnd) || IsCloaked(hwnd)) {
            state.waitingUntilUsable = true;
            continue;
        }
        if (state.waitingUntilUsable) {
            state.waitingUntilUsable = false;
            state.usableWakeDeadline = 0;
            state.nextAttempt = now + kRetryIntervalMs;
            state.deadline = state.nextAttempt + controller.settings->settleMs;
            continue;
        }
        // Styles and owners can change after a window first appears. Shell-hosted
        // panels (e.g. the IME candidate window in explorer.exe) can start as
        // ordinary frames, then stay cloaked for a long time before being used.
        // Re-validate the match right before every adjustment.
        int currentRule = MatchRule(hwnd, *controller.settings);
        if (currentRule != state.ruleIndex) {
            wchar_t className[128]{};
            GetClassNameW(hwnd, className, ARRAYSIZE(className));
            Wh_Log(
                L"Rule changed for hwnd=%p class=\"%s\": %d -> %d (style=0x%08lX exstyle=0x%08lX)",
                hwnd, className, state.ruleIndex, currentRule,
                static_cast<unsigned long>(GetWindowLongPtrW(hwnd, GWL_STYLE)),
                static_cast<unsigned long>(GetWindowLongPtrW(hwnd, GWL_EXSTYLE)));
            state.ruleIndex = currentRule;
            if (currentRule < 0) {
                CancelNativeVerticalTask(hwnd, state);
                EndSizeMove(hwnd, state);
                state.pending = false;
                state.placementPending = false;
                state.preparingVerticalMaximize = false;
                state.retryRuleOnEvent = true;
                continue;
            }
        }
        // Notification sends are bounded and use SMTO_BLOCK to prevent reentrancy.
        bool geometryAttempted =
            ApplyRule(hwnd, state, controller.settings->rules[state.ruleIndex]);
        state.nextAttempt = now + kRetryIntervalMs;
        if (now >= state.deadline) {
            // Restoring and establishing a normal layout for vertical maximize
            // need another turn, including with a zero-length correction period.
            if (!geometryAttempted && now < state.deadline + 2000) {
                continue;
            }
            state.pending = false;
        }
    }
    controller.processingWindows = false;
    std::vector<DeferredWindowEvent> events;
    events.swap(controller.deferredEvents);
    for (const auto& event : events) {
        WindowEvent(nullptr, event.event, event.hwnd, event.objectId, event.childId, 0, 0);
    }
}

// Poll only while a window still needs work. Window events and the stop and
// reload events wake the loop otherwise.
bool HasPendingWork(const Controller& controller) {
    const ULONGLONG now = GetTickCount64();
    for (const auto& [hwnd, state] : controller.windows) {
        if (state.pending && (!state.waitingUntilUsable || now < state.usableWakeDeadline)) {
            return true;
        }
        if (state.placementPending || state.ruleRetryDeadline) {
            return true;
        }
    }
    return false;
}

// Each selected process owns its controller and only its own windows.
DWORD WINAPI ControllerThread(void*) {
    DPI_AWARENESS_CONTEXT oldDpi =
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    Controller controller{SettingsSnapshot(), {}};
    g_controller = &controller;
    DWORD processId = GetCurrentProcessId();
    HWINEVENTHOOK objectHook = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE, nullptr,
                                               WindowEvent, processId, 0, WINEVENT_OUTOFCONTEXT);
    HWINEVENTHOOK uncloakHook =
        SetWinEventHook(EVENT_OBJECT_UNCLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr, WindowEvent,
                        processId, 0, WINEVENT_OUTOFCONTEXT);
    HWINEVENTHOOK foregroundHook =
        SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, WindowEvent,
                        processId, 0, WINEVENT_OUTOFCONTEXT);
    HWINEVENTHOOK restoreHook =
        SetWinEventHook(EVENT_SYSTEM_MINIMIZEEND, EVENT_SYSTEM_MINIMIZEEND, nullptr, WindowEvent,
                        processId, 0, WINEVENT_OUTOFCONTEXT);
    if (objectHook && uncloakHook && foregroundHook && restoreHook) {
        EnumWindows(EnumerateWindow, controller.settings->applyToExisting);
        HANDLE events[] = {g_stopEvent, g_reloadEvent};
        while (!g_stopping.load()) {
            DWORD timeout =
                HasPendingWork(controller) ? static_cast<DWORD>(kRetryIntervalMs) : INFINITE;
            DWORD result = MsgWaitForMultipleObjectsEx(ARRAYSIZE(events), events, timeout,
                                                       QS_ALLINPUT, MWMO_INPUTAVAILABLE);
            if (result == WAIT_OBJECT_0 || result == WAIT_FAILED) {
                break;
            }
            if (result == WAIT_OBJECT_0 + 1) {
                auto settings = SettingsSnapshot();
                // Saving the settings for another program leaves this process's
                // rules unchanged. Don't move its windows back in that case.
                bool rulesChanged = settings->rules != controller.settings->rules;
                controller.settings = std::move(settings);
                if (rulesChanged) {
                    ClearWindowStates();
                    EnumWindows(EnumerateWindow, controller.settings->applyToExisting);
                }
            }
            MSG message;
            // Bound each batch so a flood of events can't starve stop/reload.
            for (int i = 0; i < 256 && PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE); ++i) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
            ProcessPendingWindows();
        }
    } else {
        Wh_Log(L"SetWinEventHook failed: %lu", GetLastError());
    }
    ClearWindowStates();
    RemoveNativeVerticalSubclasses();
    for (HWINEVENTHOOK hook : {objectHook, uncloakHook, foregroundHook, restoreHook}) {
        if (hook) {
            UnhookWinEvent(hook);
        }
    }
    g_controller = nullptr;
    if (oldDpi) {
        SetThreadDpiAwarenessContext(oldDpi);
    }
    return 0;
}

void StopController() {
    g_stopping.store(true);
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_thread) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}

void CloseControllerEvents() {
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
    }
    if (g_reloadEvent) {
        CloseHandle(g_reloadEvent);
    }
    g_stopEvent = g_reloadEvent = nullptr;
}

bool StartController() {
    if (g_thread) {
        return true;
    }
    g_stopping.store(false);
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_reloadEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (g_stopEvent && g_reloadEvent) {
        g_thread = CreateThread(nullptr, 0, ControllerThread, nullptr, 0, nullptr);
    }
    if (!g_thread) {
        CloseControllerEvents();
        return false;
    }
    return true;
}

}  // namespace

BOOL Wh_ModInit() {
    std::vector<wchar_t> path(32768);
    DWORD length = GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
    if (!length || length >= path.size()) {
        return FALSE;
    }
    g_processPath = NormalizeProgram(std::wstring(path.data(), length));
    size_t slash = g_processPath.find_last_of(L'\\');
    g_processName = g_processPath.substr(slash == std::wstring::npos ? 0 : slash + 1);
    g_settings = ReadSettings();
    // Unload from processes without a matching rule. Windhawk calls Wh_ModInit
    // again in those processes after each settings change.
    if (g_settings->rules.empty()) {
        return FALSE;
    }
    g_nativeVerticalMessage = RegisterWindowMessageW(L"Windhawk_" WH_MOD_ID L"_NativeVertical");
    return StartController();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    auto settings = ReadSettings();
    if (settings->rules.empty()) {
        // The reload calls Wh_ModInit, which unloads the mod from this process.
        *bReload = TRUE;
        return TRUE;
    }
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_settings = std::move(settings);
    }
    if (g_thread) {
        SetEvent(g_reloadEvent);
    } else if (!StartController()) {
        Wh_Log(L"Failed to start this process's window controller");
    }
    return TRUE;
}

void Wh_ModBeforeUninit() {
    StopController();
}

void Wh_ModUninit() {
    StopController();
    CloseControllerEvents();
    g_settings.reset();
}
