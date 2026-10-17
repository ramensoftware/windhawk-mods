// ==WindhawkMod==
// @id              centered-taskbar-reveal
// @name            Centered Taskbar Reveal
// @description     Hides the taskbar for maximized windows and reveals it only from the center bottom area
// @version         1.1.0
// @author          hominhducdev
// @include         explorer.exe
// @architecture    x86-64
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Centered Taskbar Reveal

This mod hides the Windows taskbar while the foreground window is maximized.

The taskbar appears only when the cursor reaches a configurable area in the
center of the bottom screen edge.

After the taskbar appears, it remains visible while the cursor is over the
taskbar or within a configurable grace area above it.

## Notes

Disable other taskbar auto-hide mods while using this mod, because multiple
mods controlling the same taskbar can conflict.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- revealWidth: 600
  $name: Reveal area width
  $description: Width in pixels of the active reveal area at the center of the bottom edge.

- revealHeight: 6
  $name: Reveal area height
  $description: Height in pixels of the reveal area measured upward from the bottom edge.

- taskbarTopGrace: 20
  $name: Grace area above taskbar
  $description: Extra pixels above the taskbar where the cursor can move without hiding it.

- taskbarSideGrace: 10
  $name: Grace area beside taskbar
  $description: Extra pixels on the left and right side of the taskbar.

- pollingInterval: 30
  $name: Polling interval
  $description: How often the cursor position is checked, in milliseconds.

- revealDelay: 40
  $name: Reveal delay
  $description: Time in milliseconds before the taskbar appears.

- hideDelay: 450
  $name: Hide delay
  $description: Time in milliseconds before the taskbar hides after the cursor leaves it.

- hideOnlyWhenMaximized: true
  $name: Hide only when maximized
  $description: Keep the taskbar visible when the foreground window is not maximized.

- foregroundWindowOnly: true
  $name: Foreground window only
  $description: Only inspect the currently active window.

- primaryMonitorOnly: false
  $name: Primary monitor only
  $description: Apply the behavior only on the primary monitor.
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <atomic>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Settings {
    int revealWidth;
    int revealHeight;

    int taskbarTopGrace;
    int taskbarSideGrace;

    int pollingInterval;
    int revealDelay;
    int hideDelay;

    bool hideOnlyWhenMaximized;
    bool foregroundWindowOnly;
    bool primaryMonitorOnly;
};

Settings g_settings;

std::atomic<bool> g_running = false;
HANDLE g_workerThread = nullptr;

std::mutex g_stateMutex;

struct TaskbarState {
    bool hiddenByMod = false;

    ULONGLONG enteredRevealAreaAt = 0;
    ULONGLONG leftTaskbarAreaAt = 0;
};

std::unordered_map<HWND, TaskbarState> g_taskbarStates;

void LoadSettings() {
    g_settings.revealWidth =
        Wh_GetIntSetting(L"revealWidth");

    g_settings.revealHeight =
        Wh_GetIntSetting(L"revealHeight");

    g_settings.taskbarTopGrace =
        Wh_GetIntSetting(L"taskbarTopGrace");

    g_settings.taskbarSideGrace =
        Wh_GetIntSetting(L"taskbarSideGrace");

    g_settings.pollingInterval =
        Wh_GetIntSetting(L"pollingInterval");

    g_settings.revealDelay =
        Wh_GetIntSetting(L"revealDelay");

    g_settings.hideDelay =
        Wh_GetIntSetting(L"hideDelay");

    g_settings.hideOnlyWhenMaximized =
        Wh_GetIntSetting(L"hideOnlyWhenMaximized");

    g_settings.foregroundWindowOnly =
        Wh_GetIntSetting(L"foregroundWindowOnly");

    g_settings.primaryMonitorOnly =
        Wh_GetIntSetting(L"primaryMonitorOnly");

    if (g_settings.revealWidth < 100) {
        g_settings.revealWidth = 100;
    }

    if (g_settings.revealHeight < 1) {
        g_settings.revealHeight = 1;
    }

    if (g_settings.revealHeight > 100) {
        g_settings.revealHeight = 100;
    }

    if (g_settings.taskbarTopGrace < 0) {
        g_settings.taskbarTopGrace = 0;
    }

    if (g_settings.taskbarTopGrace > 200) {
        g_settings.taskbarTopGrace = 200;
    }

    if (g_settings.taskbarSideGrace < 0) {
        g_settings.taskbarSideGrace = 0;
    }

    if (g_settings.taskbarSideGrace > 200) {
        g_settings.taskbarSideGrace = 200;
    }

    if (g_settings.pollingInterval < 10) {
        g_settings.pollingInterval = 10;
    }

    if (g_settings.pollingInterval > 1000) {
        g_settings.pollingInterval = 1000;
    }

    if (g_settings.revealDelay < 0) {
        g_settings.revealDelay = 0;
    }

    if (g_settings.revealDelay > 5000) {
        g_settings.revealDelay = 5000;
    }

    if (g_settings.hideDelay < 0) {
        g_settings.hideDelay = 0;
    }

    if (g_settings.hideDelay > 10000) {
        g_settings.hideDelay = 10000;
    }
}

bool IsTaskbarWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return false;
    }

    WCHAR className[64]{};

    if (!GetClassNameW(
            hWnd,
            className,
            ARRAYSIZE(className))) {
        return false;
    }

    return _wcsicmp(
               className,
               L"Shell_TrayWnd") == 0 ||
           _wcsicmp(
               className,
               L"Shell_SecondaryTrayWnd") == 0;
}

std::vector<HWND> FindTaskbarWindows() {
    std::vector<HWND> taskbars;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto* result =
                reinterpret_cast<std::vector<HWND>*>(
                    lParam);

            if (IsTaskbarWindow(hWnd)) {
                result->push_back(hWnd);
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&taskbars));

    return taskbars;
}

bool IsPrimaryMonitor(HMONITOR monitor) {
    if (!monitor) {
        return false;
    }

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };

    if (!GetMonitorInfoW(
            monitor,
            &monitorInfo)) {
        return false;
    }

    return (
        monitorInfo.dwFlags &
        MONITORINFOF_PRIMARY) != 0;
}

bool IsWindowEligible(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return false;
    }

    if (!IsWindowVisible(hWnd)) {
        return false;
    }

    if (IsIconic(hWnd)) {
        return false;
    }

    if (hWnd == GetShellWindow() ||
        hWnd == GetDesktopWindow()) {
        return false;
    }

    LONG_PTR style =
        GetWindowLongPtrW(
            hWnd,
            GWL_STYLE);

    if (style & WS_CHILD) {
        return false;
    }

    LONG_PTR extendedStyle =
        GetWindowLongPtrW(
            hWnd,
            GWL_EXSTYLE);

    if (extendedStyle & WS_EX_NOACTIVATE) {
        return false;
    }

    WCHAR className[128]{};

    if (GetClassNameW(
            hWnd,
            className,
            ARRAYSIZE(className))) {
        if (_wcsicmp(
                className,
                L"Progman") == 0 ||
            _wcsicmp(
                className,
                L"WorkerW") == 0 ||
            _wcsicmp(
                className,
                L"Shell_TrayWnd") == 0 ||
            _wcsicmp(
                className,
                L"Shell_SecondaryTrayWnd") == 0) {
            return false;
        }
    }

    return true;
}

bool IsWindowMaximized(HWND hWnd) {
    if (!IsWindowEligible(hWnd)) {
        return false;
    }

    WINDOWPLACEMENT placement{
        .length = sizeof(WINDOWPLACEMENT),
    };

    if (!GetWindowPlacement(
            hWnd,
            &placement)) {
        return false;
    }

    return placement.showCmd ==
           SW_SHOWMAXIMIZED;
}

bool MonitorHasMaximizedWindow(
    HMONITOR targetMonitor) {
    if (!targetMonitor) {
        return false;
    }

    HWND foregroundWindow =
        GetForegroundWindow();

    if (g_settings.foregroundWindowOnly) {
        if (!IsWindowEligible(
                foregroundWindow)) {
            return false;
        }

        HMONITOR foregroundMonitor =
            MonitorFromWindow(
                foregroundWindow,
                MONITOR_DEFAULTTONEAREST);

        if (foregroundMonitor !=
            targetMonitor) {
            return false;
        }

        return IsWindowMaximized(
            foregroundWindow);
    }

    struct EnumData {
        HMONITOR monitor;
        bool found;
    };

    EnumData data{
        .monitor = targetMonitor,
        .found = false,
    };

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto* data =
                reinterpret_cast<EnumData*>(
                    lParam);

            if (!IsWindowEligible(hWnd)) {
                return TRUE;
            }

            HMONITOR windowMonitor =
                MonitorFromWindow(
                    hWnd,
                    MONITOR_DEFAULTTONEAREST);

            if (windowMonitor !=
                data->monitor) {
                return TRUE;
            }

            if (IsWindowMaximized(hWnd)) {
                data->found = true;
                return FALSE;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&data));

    return data.found;
}

bool GetCursorPosition(POINT* point) {
    if (!point) {
        return false;
    }

    return GetCursorPos(point) != FALSE;
}

bool IsCursorInsideRevealArea(
    HMONITOR monitor) {
    if (!monitor) {
        return false;
    }

    POINT cursorPosition{};

    if (!GetCursorPosition(
            &cursorPosition)) {
        return false;
    }

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };

    if (!GetMonitorInfoW(
            monitor,
            &monitorInfo)) {
        return false;
    }

    RECT monitorRect =
        monitorInfo.rcMonitor;

    int monitorWidth =
        monitorRect.right -
        monitorRect.left;

    int revealWidth =
        g_settings.revealWidth;

    if (revealWidth > monitorWidth) {
        revealWidth = monitorWidth;
    }

    int revealLeft =
        monitorRect.left +
        (monitorWidth - revealWidth) / 2;

    RECT revealRect{
        revealLeft,
        monitorRect.bottom -
            g_settings.revealHeight,
        revealLeft + revealWidth,
        monitorRect.bottom,
    };

    return PtInRect(
               &revealRect,
               cursorPosition) != FALSE;
}

bool IsCursorInsideTaskbarArea(
    HWND taskbarWindow) {
    if (!taskbarWindow ||
        !IsWindow(taskbarWindow) ||
        !IsWindowVisible(taskbarWindow)) {
        return false;
    }

    POINT cursorPosition{};

    if (!GetCursorPosition(
            &cursorPosition)) {
        return false;
    }

    RECT taskbarRect{};

    if (!GetWindowRect(
            taskbarWindow,
            &taskbarRect)) {
        return false;
    }

    taskbarRect.top -=
        g_settings.taskbarTopGrace;

    taskbarRect.left -=
        g_settings.taskbarSideGrace;

    taskbarRect.right +=
        g_settings.taskbarSideGrace;

    return PtInRect(
               &taskbarRect,
               cursorPosition) != FALSE;
}

void HideTaskbar(
    HWND taskbarWindow,
    TaskbarState& state) {
    if (!IsWindow(taskbarWindow)) {
        return;
    }

    if (state.hiddenByMod &&
        !IsWindowVisible(taskbarWindow)) {
        return;
    }

    Wh_Log(
        L"Hiding taskbar: %p",
        taskbarWindow);

    ShowWindow(
        taskbarWindow,
        SW_HIDE);

    state.hiddenByMod = true;
}

void ShowTaskbar(
    HWND taskbarWindow,
    TaskbarState& state) {
    if (!IsWindow(taskbarWindow)) {
        return;
    }

    if (!state.hiddenByMod &&
        IsWindowVisible(taskbarWindow)) {
        return;
    }

    Wh_Log(
        L"Showing taskbar: %p",
        taskbarWindow);

    ShowWindow(
        taskbarWindow,
        SW_SHOWNOACTIVATE);

    SetWindowPos(
        taskbarWindow,
        HWND_TOPMOST,
        0,
        0,
        0,
        0,
        SWP_NOMOVE |
            SWP_NOSIZE |
            SWP_NOACTIVATE |
            SWP_SHOWWINDOW);

    state.hiddenByMod = false;
}

void ResetTimers(
    TaskbarState& state) {
    state.enteredRevealAreaAt = 0;
    state.leftTaskbarAreaAt = 0;
}

void ProcessTaskbar(
    HWND taskbarWindow,
    ULONGLONG currentTime) {
    if (!IsWindow(taskbarWindow)) {
        return;
    }

    HMONITOR monitor =
        MonitorFromWindow(
            taskbarWindow,
            MONITOR_DEFAULTTONEAREST);

    if (!monitor) {
        return;
    }

    TaskbarState& state =
        g_taskbarStates[taskbarWindow];

    if (g_settings.primaryMonitorOnly &&
        !IsPrimaryMonitor(monitor)) {
        ResetTimers(state);

        ShowTaskbar(
            taskbarWindow,
            state);

        return;
    }

    bool shouldHideTaskbar = true;

    if (g_settings.hideOnlyWhenMaximized) {
        shouldHideTaskbar =
            MonitorHasMaximizedWindow(
                monitor);
    }

    if (!shouldHideTaskbar) {
        ResetTimers(state);

        ShowTaskbar(
            taskbarWindow,
            state);

        return;
    }

    bool cursorInsideRevealArea =
        IsCursorInsideRevealArea(
            monitor);

    bool cursorInsideTaskbarArea =
        IsCursorInsideTaskbarArea(
            taskbarWindow);

    /*
     * Khi Taskbar đang ẩn:
     *
     * Chỉ vùng reveal ở chính giữa mép dưới
     * mới được phép làm Taskbar hiện.
     */
    if (state.hiddenByMod) {
        state.leftTaskbarAreaAt = 0;

        if (!cursorInsideRevealArea) {
            state.enteredRevealAreaAt = 0;
            return;
        }

        if (state.enteredRevealAreaAt == 0) {
            state.enteredRevealAreaAt =
                currentTime;
        }

        ULONGLONG elapsed =
            currentTime -
            state.enteredRevealAreaAt;

        if (elapsed >=
            static_cast<ULONGLONG>(
                g_settings.revealDelay)) {
            ShowTaskbar(
                taskbarWindow,
                state);

            ResetTimers(state);
        }

        return;
    }

    /*
     * Khi Taskbar đang hiện:
     *
     * Nếu chuột vẫn nằm trên Taskbar hoặc
     * trong khoảng đệm phía trên, giữ nguyên.
     */
    if (cursorInsideTaskbarArea ||
        cursorInsideRevealArea) {
        state.enteredRevealAreaAt = 0;
        state.leftTaskbarAreaAt = 0;
        return;
    }

    /*
     * Chuột đã rời khỏi cả Taskbar,
     * khoảng đệm và vùng reveal.
     */
    state.enteredRevealAreaAt = 0;

    if (state.leftTaskbarAreaAt == 0) {
        state.leftTaskbarAreaAt =
            currentTime;
    }

    ULONGLONG elapsed =
        currentTime -
        state.leftTaskbarAreaAt;

    if (elapsed >=
        static_cast<ULONGLONG>(
            g_settings.hideDelay)) {
        HideTaskbar(
            taskbarWindow,
            state);

        ResetTimers(state);
    }
}

void RemoveDestroyedTaskbars() {
    for (auto iterator =
             g_taskbarStates.begin();
         iterator !=
         g_taskbarStates.end();) {
        if (!IsWindow(iterator->first)) {
            iterator =
                g_taskbarStates.erase(
                    iterator);
        } else {
            ++iterator;
        }
    }
}

void RestoreAllTaskbars() {
    std::lock_guard<std::mutex>
        lock(g_stateMutex);

    std::vector<HWND> taskbars =
        FindTaskbarWindows();

    std::unordered_set<HWND>
        restoredTaskbars;

    for (HWND taskbarWindow :
         taskbars) {
        TaskbarState& state =
            g_taskbarStates[taskbarWindow];

        ShowTaskbar(
            taskbarWindow,
            state);

        restoredTaskbars.insert(
            taskbarWindow);
    }

    for (auto& pair :
         g_taskbarStates) {
        if (restoredTaskbars.contains(
                pair.first)) {
            continue;
        }

        if (IsWindow(pair.first)) {
            ShowWindow(
                pair.first,
                SW_SHOWNOACTIVATE);
        }
    }

    g_taskbarStates.clear();
}

DWORD WINAPI WorkerThreadProc(
    LPVOID parameter) {
    Wh_Log(
        L"Worker thread started");

    while (g_running.load()) {
        {
            std::lock_guard<std::mutex>
                lock(g_stateMutex);

            std::vector<HWND> taskbars =
                FindTaskbarWindows();

            ULONGLONG currentTime =
                GetTickCount64();

            for (HWND taskbarWindow :
                 taskbars) {
                ProcessTaskbar(
                    taskbarWindow,
                    currentTime);
            }

            RemoveDestroyedTaskbars();
        }

        DWORD sleepTime =
            static_cast<DWORD>(
                g_settings.pollingInterval);

        Sleep(sleepTime);
    }

    Wh_Log(
        L"Worker thread stopped");

    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(
        L"Centered Taskbar Reveal initializing");

    LoadSettings();

    g_running.store(true);

    g_workerThread =
        CreateThread(
            nullptr,
            0,
            WorkerThreadProc,
            nullptr,
            0,
            nullptr);

    if (!g_workerThread) {
        DWORD error =
            GetLastError();

        Wh_Log(
            L"CreateThread failed: %u",
            error);

        g_running.store(false);

        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(
        L"Centered Taskbar Reveal unloading");

    g_running.store(false);

    if (g_workerThread) {
        DWORD waitResult =
            WaitForSingleObject(
                g_workerThread,
                5000);

        if (waitResult ==
            WAIT_TIMEOUT) {
            Wh_Log(
                L"Worker thread timeout");
        }

        CloseHandle(
            g_workerThread);

        g_workerThread = nullptr;
    }

    RestoreAllTaskbars();
}

BOOL Wh_ModSettingsChanged(
    BOOL* reloadRequired) {
    Wh_Log(
        L"Settings changed");

    {
        std::lock_guard<std::mutex>
            lock(g_stateMutex);

        LoadSettings();

        for (auto& pair :
             g_taskbarStates) {
            ResetTimers(
                pair.second);
        }
    }

    if (reloadRequired) {
        *reloadRequired = FALSE;
    }

    return TRUE;
}