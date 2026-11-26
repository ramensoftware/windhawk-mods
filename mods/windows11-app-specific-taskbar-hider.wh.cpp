// ==WindhawkMod==
// @id              windows11-app-specific-taskbar-hider
// @name            App Specific Taskbar Hider
// @description     Hides the taskbar and blocks it from being revealed while selected apps are focused or running
// @version         1.3
// @author          Dszae
// @github          https://github.com/Dszae
// @homepage        https://github.com/Dszae/taskbar-fixer
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# App Specific Taskbar Hider

Hides the Windows 11 taskbar and gives the full screen to your app while one
of your chosen applications is focused (or, optionally, just running).

Unlike simply hiding the taskbar window, this mod also blocks the shell from
showing it again, so moving the mouse to the bottom of the screen (for example
to reach a timeline scrollbar in DaVinci Resolve, or a horizontal scrollbar in
Photoshop/InDesign) will not bring the taskbar back or steal focus from your
app.

## How it differs from similar mods

* *Taskbar auto-hide fine tuning* can keep the taskbar from showing (its
  "Never" mode), but always and for all apps, and it needs Windows' auto-hide
  to be on.
* *Taskbar auto-hide when maximized* turns auto-hide on and off based on
  window state, and its program list holds exclusions, not triggers.

This mod is triggered by a list of programs being focused or running. While
none of them is active, the taskbar works exactly as you configured it, with
no auto-hide, and Windows' auto-hide setting is never changed.

## How it works

The mod hooks the APIs the shell uses to show and activate the taskbar
(`ShowWindow`, `SetWindowPos`, `SetForegroundWindow`). While one of your target
apps is active, any attempt to show or focus a taskbar window (primary or
secondary monitor) is blocked. The work area is extended over the taskbar's
edge of each monitor, so maximized windows fill the screen, and the original
work area is restored when the taskbar comes back.

Focus changes are detected instantly via a foreground-window event. A light
background check (twice a second) handles the "running" mode and re-hides the
taskbar if the shell recreates it.

## Usage

1. In the mod's settings, list the EXE filenames (just the filename, not the
   full path) of the apps that should trigger hiding, e.g. `Resolve.exe`,
   `Photoshop.exe`, `FL64.exe`, `InDesign.exe`. The `.exe` extension is
   optional.
2. Choose whether hiding should require the app to be focused, or just
   running (see the "Require focus" setting).
3. Leave Windows' own "Automatically hide the taskbar" setting **off**, so the
   two don't fight each other.
4. The Start menu still opens with the Win key. Opening it switches focus away
   from your app, so the taskbar is shown while it's open (in "Require focus"
   mode). Alt+Tab and Task View don't bring the taskbar back.

To find an app's EXE name, open Task Manager, go to the **Details** tab, and
look at the **Name** column.

Note: the windows of most Microsoft Store (UWP) apps belong to
`ApplicationFrameHost.exe`, not to the app's own EXE, so in "Require focus"
mode they aren't detected by their own name. Regular desktop apps, including
desktop apps installed from the Store, work as expected.

Only the taskbar's edge of the screen is given to apps. Space reserved by
other docked toolbars is kept.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideTaskbar: true
  $name: Enable taskbar hiding
  $description: >-
    Master switch. When enabled, the taskbar is hidden and blocked from being
    revealed, based on the target processes below.
- targetProcesses: [Resolve.exe, Photoshop.exe, FL64.exe, InDesign.exe]
  $name: Target processes
  $description: >-
    EXE filenames (no path) that should trigger taskbar hiding, e.g.
    Resolve.exe or Photoshop.exe.
- requireFocus: true
  $name: Require focus (not just running)
  $description: >-
    If enabled, the taskbar is hidden only while one of the target apps is the
    foreground window. If disabled, it stays hidden as long as a target app is
    running at all.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <tlhelp32.h>

#include <algorithm>
#include <atomic>
#include <string>
#include <vector>

namespace {

constexpr DWORD kTickIntervalMs = 500;
constexpr ULONGLONG kWorkAreaRecheckIntervalMs = 1000;
constexpr UINT kSettingChangeTimeoutMs = 100;
constexpr UINT kMsgSettingsChanged = WM_APP + 1;

struct Settings {
    bool hideTaskbar = true;
    bool requireFocus = true;
    std::vector<std::wstring> targetProcesses;
};

// Settings are written by Wh_ModSettingsChanged and read by the worker
// thread, so access is guarded by g_settingsLock.
Settings g_settings;
CRITICAL_SECTION g_settingsLock;

// The single flag the hooks check: while true, any attempt to show/activate
// a taskbar window is blocked. Only the worker thread changes it (and
// Wh_ModUninit, after the worker thread has exited).
std::atomic<bool> g_forceHidden{false};

HANDLE g_workerThread = nullptr;
std::atomic<DWORD> g_workerThreadId{0};
HANDLE g_stopEvent = nullptr;

// Work areas saved before expanding them, restored when the taskbar is shown
// again. Only touched by the worker thread (or by Wh_ModUninit after the
// worker thread has exited).
struct SavedWorkArea {
    RECT monitorRect;
    RECT workRect;
};
std::vector<SavedWorkArea> g_savedWorkAreas;
ULONGLONG g_lastWorkAreaCheck = 0;

// Taskbar windows hidden by this mod. Only these are shown again, so that
// taskbars hidden by something else (e.g. another mod) stay hidden. Same
// thread rules as g_savedWorkAreas.
std::vector<HWND> g_hiddenTaskbars;

// Some calls below (e.g. the work area change broadcast) dispatch incoming
// sent messages, which can run the foreground event callback while an update
// is in progress. These flags make such nested calls run afterwards instead.
// Only used by the worker thread.
bool g_updateInProgress = false;
bool g_updatePending = false;

// ---------------------------------------------------------------------
// Taskbar windows
// ---------------------------------------------------------------------

bool IsTaskbarWindow(HWND hWnd) {
    if (!hWnd) {
        return false;
    }

    WCHAR className[32];
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return _wcsicmp(className, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

// Returns the primary and all secondary taskbars owned by this explorer
// process.
std::vector<HWND> GetTaskbarWindows() {
    std::vector<HWND> result;
    DWORD currentPid = GetCurrentProcessId();

    auto addIfOurs = [&](HWND hWnd) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hWnd, &pid);
        if (pid == currentPid) {
            result.push_back(hWnd);
        }
    };

    HWND hWnd = nullptr;
    while ((hWnd = FindWindowExW(nullptr, hWnd, L"Shell_TrayWnd", nullptr))) {
        addIfOurs(hWnd);
    }

    hWnd = nullptr;
    while ((hWnd = FindWindowExW(nullptr, hWnd, L"Shell_SecondaryTrayWnd",
                                 nullptr))) {
        addIfOurs(hWnd);
    }

    return result;
}

// ---------------------------------------------------------------------
// Work area
// ---------------------------------------------------------------------

// Notifies all windows about the work area change. SPIF_SENDCHANGE isn't used
// with SPI_SETWORKAREA, since it waits for every top-level window and would
// block on a hung window. Hung windows are skipped here instead.
void BroadcastWorkAreaChange() {
    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETWORKAREA, 0,
                        SMTO_ABORTIFHUNG, kSettingChangeTimeoutMs, nullptr);
}

// Expands the work area of each monitor that has a taskbar over the edge
// the taskbar is docked to, so that maximized windows also cover the
// taskbar's area. Space reserved by other appbars (e.g. docks) on other
// edges is kept. Can be called repeatedly: monitors that are already
// expanded are skipped, and the original work area is only saved the first
// time, so that it can be restored later.
void ExpandWorkAreas() {
    g_lastWorkAreaCheck = GetTickCount64();

    bool changed = false;

    for (HWND hTaskbar : GetTaskbarWindows()) {
        RECT taskbarRect;
        if (!GetWindowRect(hTaskbar, &taskbarRect)) {
            continue;
        }

        HMONITOR hMonitor =
            MonitorFromRect(&taskbarRect, MONITOR_DEFAULTTONULL);
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        if (!hMonitor || !GetMonitorInfoW(hMonitor, &mi)) {
            continue;
        }

        // Extend the work area over the taskbar's edge of the monitor.
        RECT newWork = mi.rcWork;
        int taskbarWidth = taskbarRect.right - taskbarRect.left;
        int taskbarHeight = taskbarRect.bottom - taskbarRect.top;
        if (taskbarWidth >= taskbarHeight) {
            int taskbarCenterY = (taskbarRect.top + taskbarRect.bottom) / 2;
            int monitorCenterY = (mi.rcMonitor.top + mi.rcMonitor.bottom) / 2;
            if (taskbarCenterY >= monitorCenterY) {
                newWork.bottom = mi.rcMonitor.bottom;
            } else {
                newWork.top = mi.rcMonitor.top;
            }
        } else {
            int taskbarCenterX = (taskbarRect.left + taskbarRect.right) / 2;
            int monitorCenterX = (mi.rcMonitor.left + mi.rcMonitor.right) / 2;
            if (taskbarCenterX >= monitorCenterX) {
                newWork.right = mi.rcMonitor.right;
            } else {
                newWork.left = mi.rcMonitor.left;
            }
        }

        if (EqualRect(&newWork, &mi.rcWork)) {
            continue;
        }

        bool alreadySaved = false;
        for (const auto& saved : g_savedWorkAreas) {
            if (EqualRect(&saved.monitorRect, &mi.rcMonitor)) {
                alreadySaved = true;
                break;
            }
        }

        if (!alreadySaved) {
            g_savedWorkAreas.push_back({mi.rcMonitor, mi.rcWork});
        }

        if (SystemParametersInfoW(SPI_SETWORKAREA, 0, &newWork, 0)) {
            changed = true;
        }
    }

    if (changed) {
        BroadcastWorkAreaChange();
    }
}

void RestoreWorkAreas() {
    // Move the list out first, so that it's in a consistent state even if
    // this function is interrupted by a nested update.
    std::vector<SavedWorkArea> savedWorkAreas = std::move(g_savedWorkAreas);
    g_savedWorkAreas.clear();

    bool changed = false;

    for (const auto& saved : savedWorkAreas) {
        // Skip if the monitor layout changed in the meantime; the shell will
        // have recalculated the work area for the new layout anyway.
        HMONITOR hMonitor =
            MonitorFromRect(&saved.monitorRect, MONITOR_DEFAULTTONULL);
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        if (!hMonitor || !GetMonitorInfoW(hMonitor, &mi) ||
            !EqualRect(&mi.rcMonitor, &saved.monitorRect)) {
            continue;
        }

        RECT workRect = saved.workRect;
        if (SystemParametersInfoW(SPI_SETWORKAREA, 0, &workRect, 0)) {
            changed = true;
        }
    }

    if (changed) {
        BroadcastWorkAreaChange();
    }
}

// ---------------------------------------------------------------------
// Target process detection
// ---------------------------------------------------------------------

bool ExeNameMatches(PCWSTR exeName, const std::vector<std::wstring>& targets) {
    for (const auto& target : targets) {
        if (_wcsicmp(exeName, target.c_str()) == 0) {
            return true;
        }
    }
    return false;
}

bool IsTransientShellWindow(HWND hWnd) {
    WCHAR className[64];
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return _wcsicmp(className, L"XamlExplorerHostIslandWindow") == 0 ||
           _wcsicmp(className, L"ForegroundStaging") == 0 ||
           _wcsicmp(className, L"MultitaskingViewFrame") == 0 ||
           _wcsicmp(className, L"TaskSwitcherWnd") == 0 ||
           IsTaskbarWindow(hWnd);
}

enum class FocusState { Unknown, TargetFocused, OtherFocused };

FocusState GetTargetFocusState(const std::vector<std::wstring>& targets) {
    HWND hWnd = GetForegroundWindow();
    if (!hWnd) {
        // No foreground window, e.g. briefly while switching windows. Keep
        // the current state to avoid flicker.
        return FocusState::Unknown;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (!pid) {
        return FocusState::Unknown;
    }

    if (pid == GetCurrentProcessId() && IsTransientShellWindow(hWnd)) {
        // Alt+Tab, Task View and similar shell UI briefly take the focus
        // while switching windows. Keep the current state, otherwise the
        // taskbar would pop up (and the work area would change) on every
        // Alt+Tab between target apps.
        return FocusState::Unknown;
    }

    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProcess) {
        // Probably a protected process, which can't be a target anyway.
        return FocusState::OtherFocused;
    }

    WCHAR path[1024];
    DWORD size = ARRAYSIZE(path);
    bool matches = false;
    if (QueryFullProcessImageNameW(hProcess, 0, path, &size)) {
        PCWSTR fileName = wcsrchr(path, L'\\');
        fileName = fileName ? fileName + 1 : path;
        matches = ExeNameMatches(fileName, targets);
    }

    CloseHandle(hProcess);
    return matches ? FocusState::TargetFocused : FocusState::OtherFocused;
}

bool IsAnyTargetProcessRunning(const std::vector<std::wstring>& targets) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return false;
    }

    // Only count processes in the current session, so that apps running for
    // other signed-in users (fast user switching, remote sessions) don't
    // hide this user's taskbar.
    DWORD currentSessionId = 0;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &currentSessionId)) {
        CloseHandle(hSnapshot);
        return false;
    }

    bool found = false;
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    if (Process32FirstW(hSnapshot, &pe)) {
        do {
            DWORD sessionId = 0;
            if (ExeNameMatches(pe.szExeFile, targets) &&
                ProcessIdToSessionId(pe.th32ProcessID, &sessionId) &&
                sessionId == currentSessionId) {
                found = true;
                break;
            }
        } while (Process32NextW(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return found;
}

// ---------------------------------------------------------------------
// State
// ---------------------------------------------------------------------

// Hides the taskbars that are currently visible, and remembers them so that
// they can be shown again later.
void HideVisibleTaskbars() {
    for (HWND hTaskbar : GetTaskbarWindows()) {
        if (!IsWindowVisible(hTaskbar)) {
            continue;
        }

        ShowWindowAsync(hTaskbar, SW_HIDE);

        if (std::find(g_hiddenTaskbars.begin(), g_hiddenTaskbars.end(),
                      hTaskbar) == g_hiddenTaskbars.end()) {
            g_hiddenTaskbars.push_back(hTaskbar);
        }
    }
}

void ApplyTaskbarVisibility(bool hide) {
    // The taskbar windows belong to another thread. ShowWindowAsync doesn't
    // wait for that thread, so it can't deadlock if the taskbar thread is
    // busy or waiting for us. The hooks only block *show* attempts while
    // g_forceHidden is set, so hiding always goes through, and showing is
    // only done after g_forceHidden was cleared.
    if (hide) {
        HideVisibleTaskbars();
        ExpandWorkAreas();
    } else {
        // Move the list out first, so that it's in a consistent state even
        // if this function is interrupted by a nested update.
        std::vector<HWND> hiddenTaskbars = std::move(g_hiddenTaskbars);
        g_hiddenTaskbars.clear();
        for (HWND hTaskbar : hiddenTaskbars) {
            if (IsWindow(hTaskbar)) {
                ShowWindowAsync(hTaskbar, SW_SHOWNA);
            }
        }

        RestoreWorkAreas();
    }
}

void UpdateTaskbarStateOnce() {
    // Only the explorer.exe process that owns the taskbar manages it and the
    // work area. Other explorer.exe processes (separate folder window
    // processes, /factory instances) do nothing.
    if (!g_forceHidden && GetTaskbarWindows().empty()) {
        return;
    }

    bool masterEnabled;
    bool requireFocus;
    std::vector<std::wstring> targets;
    {
        EnterCriticalSection(&g_settingsLock);
        masterEnabled = g_settings.hideTaskbar;
        requireFocus = g_settings.requireFocus;
        targets = g_settings.targetProcesses;
        LeaveCriticalSection(&g_settingsLock);
    }

    bool wasHidden = g_forceHidden;
    bool shouldHide = false;
    if (masterEnabled && !targets.empty()) {
        if (requireFocus) {
            switch (GetTargetFocusState(targets)) {
                case FocusState::Unknown:
                    shouldHide = wasHidden;
                    break;
                case FocusState::TargetFocused:
                    shouldHide = true;
                    break;
                case FocusState::OtherFocused:
                    shouldHide = false;
                    break;
            }
        } else {
            shouldHide = IsAnyTargetProcessRunning(targets);
        }
    }

    if (shouldHide != wasHidden) {
        Wh_Log(L"Condition changed, %s taskbar",
               shouldHide ? L"hiding" : L"showing");
        g_forceHidden = shouldHide;
        ApplyTaskbarVisibility(shouldHide);
        return;
    }

    if (shouldHide) {
        // Re-hide taskbars that became visible through a path the hooks
        // don't cover, e.g. a taskbar recreated after a display change.
        HideVisibleTaskbars();

        // Re-expand the work area if it was reset, e.g. after a display
        // change or when a monitor was connected. Rate limited, since
        // changing the work area notifies all windows.
        if (GetTickCount64() - g_lastWorkAreaCheck >=
            kWorkAreaRecheckIntervalMs) {
            ExpandWorkAreas();
        }
    }
}

// Must only be called from the worker thread.
void UpdateTaskbarState() {
    if (g_updateInProgress) {
        g_updatePending = true;
        return;
    }

    g_updateInProgress = true;
    do {
        g_updatePending = false;
        UpdateTaskbarStateOnce();
    } while (g_updatePending);
    g_updateInProgress = false;
}

// ---------------------------------------------------------------------
// API hooks: block anything that would show/activate a taskbar while
// g_forceHidden is set.
// ---------------------------------------------------------------------

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (g_forceHidden && nCmdShow != SW_HIDE && IsTaskbarWindow(hWnd)) {
        // Pretend it worked. The return value is the previous visibility
        // state, which is "hidden".
        return FALSE;
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;

BOOL WINAPI SetWindowPos_Hook(HWND hWnd,
                              HWND hWndInsertAfter,
                              int X,
                              int Y,
                              int cx,
                              int cy,
                              UINT uFlags) {
    if (g_forceHidden && IsTaskbarWindow(hWnd)) {
        uFlags &= ~SWP_SHOWWINDOW;
        uFlags |= SWP_NOACTIVATE | SWP_HIDEWINDOW;
    }
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

using SetForegroundWindow_t = decltype(&SetForegroundWindow);
SetForegroundWindow_t SetForegroundWindow_Original;

BOOL WINAPI SetForegroundWindow_Hook(HWND hWnd) {
    if (g_forceHidden && IsTaskbarWindow(hWnd)) {
        return FALSE;
    }
    return SetForegroundWindow_Original(hWnd);
}

// ---------------------------------------------------------------------
// Worker thread: reacts to foreground changes instantly, and re-evaluates
// periodically for "running" mode and to re-hide recreated taskbars.
// ---------------------------------------------------------------------

void CALLBACK ForegroundWinEventProc(HWINEVENTHOOK,
                                     DWORD,
                                     HWND,
                                     LONG,
                                     LONG,
                                     DWORD,
                                     DWORD) {
    UpdateTaskbarState();
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    HWINEVENTHOOK winEventHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        ForegroundWinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!winEventHook) {
        Wh_Log(L"SetWinEventHook failed, relying on periodic checks only");
    }

    UpdateTaskbarState();

    while (true) {
        DWORD waitResult = MsgWaitForMultipleObjects(
            1, &g_stopEvent, FALSE, kTickIntervalMs, QS_ALLINPUT);

        if (waitResult == WAIT_OBJECT_0) {
            break;
        }

        if (waitResult == WAIT_OBJECT_0 + 1) {
            // Dispatching runs ForegroundWinEventProc for pending events.
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == kMsgSettingsChanged) {
                    UpdateTaskbarState();
                }
                DispatchMessageW(&msg);
            }
        } else if (waitResult == WAIT_TIMEOUT) {
            UpdateTaskbarState();
        } else {
            Wh_Log(L"MsgWaitForMultipleObjects failed: %u", GetLastError());
            break;
        }
    }

    if (winEventHook) {
        UnhookWinEvent(winEventHook);
    }

    return 0;
}

// ---------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------

std::wstring NormalizeExeName(PCWSTR value) {
    std::wstring name = value;

    // Trim whitespace.
    size_t start = name.find_first_not_of(L" \t");
    size_t end = name.find_last_not_of(L" \t");
    if (start == std::wstring::npos) {
        return L"";
    }
    name = name.substr(start, end - start + 1);

    // Strip a path if one was given.
    size_t slash = name.find_last_of(L"\\/");
    if (slash != std::wstring::npos) {
        name = name.substr(slash + 1);
    }

    // Allow omitting the ".exe" extension.
    if (name.size() < 4 || _wcsicmp(name.c_str() + name.size() - 4, L".exe")) {
        name += L".exe";
    }

    return name;
}

void LoadSettings() {
    Settings newSettings;
    newSettings.hideTaskbar = Wh_GetIntSetting(L"hideTaskbar");
    newSettings.requireFocus = Wh_GetIntSetting(L"requireFocus");

    for (int i = 0;; i++) {
        auto value =
            WindhawkUtils::StringSetting::make(L"targetProcesses[%d]", i);
        if (!*value.get()) {
            break;
        }

        std::wstring name = NormalizeExeName(value.get());
        if (!name.empty()) {
            newSettings.targetProcesses.push_back(std::move(name));
        }
    }

    EnterCriticalSection(&g_settingsLock);
    g_settings = std::move(newSettings);
    LeaveCriticalSection(&g_settingsLock);
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L">");

    InitializeCriticalSection(&g_settingsLock);
    LoadSettings();

    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook,
                                   &ShowWindow_Original);
    WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook,
                                   &SetWindowPos_Original);
    WindhawkUtils::SetFunctionHook(SetForegroundWindow,
                                   SetForegroundWindow_Hook,
                                   &SetForegroundWindow_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // Hooks are active at this point, start tracking the target apps.
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        return;
    }

    DWORD threadId = 0;
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0,
                                  &threadId);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        return;
    }

    g_workerThreadId = threadId;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_workerThread) {
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        g_workerThreadId = 0;
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    if (g_forceHidden) {
        g_forceHidden = false;
        ApplyTaskbarVisibility(false);
    }

    DeleteCriticalSection(&g_settingsLock);
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    // Let the worker thread apply the new settings right away.
    if (g_workerThreadId) {
        PostThreadMessageW(g_workerThreadId, kMsgSettingsChanged, 0, 0);
    }
}
