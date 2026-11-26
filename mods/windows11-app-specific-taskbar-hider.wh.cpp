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

## How it works

The mod hooks the APIs the shell uses to show and activate the taskbar
(`ShowWindow`, `SetWindowPos`, `SetForegroundWindow`). While one of your target
apps is active, any attempt to show or focus a taskbar window (primary or
secondary monitor) is blocked. The work area of every monitor is expanded to
the full monitor size, so maximized windows fill the screen, and the original
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
   mode).

To find an app's EXE name, open Task Manager, go to the **Details** tab, and
look at the **Name** column.
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

#include <windows.h>
#include <tlhelp32.h>

#include <atomic>
#include <string>
#include <vector>

namespace {

constexpr DWORD kTickIntervalMs = 500;
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
DWORD g_workerThreadId = 0;
HANDLE g_stopEvent = nullptr;

// Work areas saved before expanding them, restored when the taskbar is shown
// again. Only touched by the worker thread (or by Wh_ModUninit after the
// worker thread has exited).
struct SavedWorkArea {
    RECT monitorRect;
    RECT workRect;
};
std::vector<SavedWorkArea> g_savedWorkAreas;

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

BOOL CALLBACK ExpandWorkAreaMonitorProc(HMONITOR hMonitor,
                                        HDC,
                                        LPRECT,
                                        LPARAM) {
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(hMonitor, &mi)) {
        return TRUE;
    }

    if (!EqualRect(&mi.rcWork, &mi.rcMonitor)) {
        g_savedWorkAreas.push_back({mi.rcMonitor, mi.rcWork});
        SystemParametersInfoW(SPI_SETWORKAREA, 0, &mi.rcMonitor,
                              SPIF_SENDCHANGE);
    }

    return TRUE;
}

void ExpandWorkAreas() {
    g_savedWorkAreas.clear();
    EnumDisplayMonitors(nullptr, nullptr, ExpandWorkAreaMonitorProc, 0);
}

void RestoreWorkAreas() {
    for (const auto& saved : g_savedWorkAreas) {
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
        SystemParametersInfoW(SPI_SETWORKAREA, 0, &workRect, SPIF_SENDCHANGE);
    }

    g_savedWorkAreas.clear();
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

    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProcess) {
        // Probably a protected process, which can't be a target anyway.
        return FocusState::OtherFocused;
    }

    WCHAR path[MAX_PATH];
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

    bool found = false;
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    if (Process32FirstW(hSnapshot, &pe)) {
        do {
            if (ExeNameMatches(pe.szExeFile, targets)) {
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

void ApplyTaskbarVisibility(bool hide) {
    // The hooks only block *show* attempts while g_forceHidden is set, so
    // the calls below go through.
    for (HWND hTaskbar : GetTaskbarWindows()) {
        ShowWindow(hTaskbar, hide ? SW_HIDE : SW_SHOWNA);
    }

    if (hide) {
        ExpandWorkAreas();
    } else {
        RestoreWorkAreas();
    }
}

// Must only be called from the worker thread.
void UpdateTaskbarState() {
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
        for (HWND hTaskbar : GetTaskbarWindows()) {
            if (IsWindowVisible(hTaskbar)) {
                Wh_Log(L"Re-hiding taskbar %p", hTaskbar);
                ShowWindow(hTaskbar, SW_HIDE);
            }
        }
    }
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
        PCWSTR value = Wh_GetStringSetting(L"targetProcesses[%d]", i);
        bool hasValue = value && *value;
        if (hasValue) {
            std::wstring name = NormalizeExeName(value);
            if (!name.empty()) {
                newSettings.targetProcesses.push_back(std::move(name));
            }
        }
        Wh_FreeStringSetting(value);
        if (!hasValue) {
            break;
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

    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook,
                       (void**)&ShowWindow_Original);
    Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook,
                       (void**)&SetWindowPos_Original);
    Wh_SetFunctionHook((void*)SetForegroundWindow,
                       (void*)SetForegroundWindow_Hook,
                       (void**)&SetForegroundWindow_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // Hooks are active at this point, start tracking the target apps.
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0,
                                  &g_workerThreadId);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_workerThread) {
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
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
