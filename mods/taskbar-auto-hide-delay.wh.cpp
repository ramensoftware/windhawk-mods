// ==WindhawkMod==
// @id              taskbar-auto-hide-delay
// @name            Taskbar auto-hide after a delay
// @description     Hides the auto-hide taskbar a configurable number of seconds after it becomes visible, unless the mouse pointer is over it
// @version         1.0
// @author          nikitor32
// @github          https://github.com/nikitor32
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshell32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Taskbar auto-hide after a delay

Windows 11 (24H2) hides the auto-hide taskbar only when the pointer leaves
the taskbar. If the taskbar becomes visible for another reason — for
example, when a new window or dialog appears — no "pointer left" event ever
fires, and the taskbar stays on screen indefinitely.

This mod watches the taskbars. As soon as a taskbar becomes visible while
the mouse pointer is **not** over it, a countdown starts. When the countdown
expires, the taskbar is hidden again using the taskbar's own hide timer, so
it slides away with the standard animation.

If you move the pointer over the taskbar, the countdown is cancelled so the
taskbar stays available while you interact with it.

The mod does nothing if auto-hide is disabled in the Windows settings.

## Settings

* **Enable delayed auto-hide** — turn the mod on or off.
* **Hide delay (seconds)** — how long the taskbar stays visible before it
  is hidden. Default: 3.

## Compatibility

The mod doesn't use hooks, so it's designed to work together with other
taskbar mods such as
[Taskbar auto-hide when maximized](https://windhawk.net/mods/taskbar-auto-hide-when-maximized)
and
[Taskbar auto-hide keyboard only](https://windhawk.net/mods/taskbar-auto-hide-keyboard-only).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enable delayed auto-hide
  $description: If disabled, the mod does nothing.
- hideDelaySeconds: 3
  $name: Hide delay (seconds)
  $description: How long the taskbar stays visible (with the pointer not over it) before it's hidden.
*/
// ==/WindhawkModSettings==

#include <shellapi.h>

#include <algorithm>
#include <atomic>
#include <unordered_map>
#include <vector>

// The settings are read from the polling thread while they can be updated
// from another thread, so atomic types are used to avoid data races.
std::atomic<bool> g_enabled{true};
std::atomic<int> g_hideDelaySeconds{3};

// The taskbar hides itself when its internal "hide" timer (ID 2) fires.
// This is the same trick the official taskbar mods use.
constexpr UINT_PTR kTrayUITimerHide = 2;

// How often we check the taskbar state, in milliseconds.
constexpr DWORD kPollIntervalMs = 300;

std::atomic<bool> g_stopFlag{false};
HANDLE g_threadHandle = nullptr;

// Per-taskbar countdown state.
struct TaskbarState {
    bool countdownActive = false;
    ULONGLONG countdownStart = 0;
};
std::unordered_map<HWND, TaskbarState> g_states;

void LoadSettings() {
    g_enabled.store(Wh_GetIntSetting(L"enabled") != 0);
    int hideDelaySeconds = Wh_GetIntSetting(L"hideDelaySeconds");
    if (hideDelaySeconds < 0) {
        hideDelaySeconds = 0;
    }
    g_hideDelaySeconds.store(hideDelaySeconds);
}

// Only makes sense while the Windows auto-hide option is turned on.
bool IsAutoHideEnabled() {
    APPBARDATA abd = {};
    abd.cbSize = sizeof(abd);
    return (SHAppBarMessage(ABM_GETSTATE, &abd) & ABS_AUTOHIDE) != 0;
}

void CollectTaskbars(std::vector<HWND>* out) {
    out->clear();

    HWND primary = FindWindow(L"Shell_TrayWnd", nullptr);
    if (primary) {
        out->push_back(primary);
    }

    // Taskbars on secondary monitors.
    EnumWindows(
        [](HWND hwnd, LPARAM lParam) -> BOOL {
            WCHAR className[32];
            if (GetClassName(hwnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
                reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hwnd);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(out));
}

// Returns true if enough of the taskbar is on screen to be considered
// visible. When auto-hidden, the taskbar is moved mostly off-screen, so only
// a sliver remains.
bool IsTaskbarShown(HWND hwnd, RECT* rectOut) {
    RECT rc;
    if (!GetWindowRect(hwnd, &rc)) {
        return false;
    }

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfo(monitor, &mi)) {
        return false;
    }

    int visibleTop = (std::max)(rc.top, mi.rcMonitor.top);
    int visibleBottom = (std::min)(rc.bottom, mi.rcMonitor.bottom);
    int visibleHeight = visibleBottom - visibleTop;
    int fullHeight = rc.bottom - rc.top;

    if (fullHeight <= 0) {
        return false;
    }

    // At least half of the taskbar is on screen.
    bool shown = visibleHeight * 2 >= fullHeight;
    if (shown) {
        *rectOut = rc;
    }
    return shown;
}

bool IsPointerOver(const RECT& rc) {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return false;
    }
    return PtInRect(&rc, pt) != 0;
}

void PollOnce() {
    if (!g_enabled.load()) {
        return;
    }

    // If auto-hide is off, never touch the taskbar.
    if (!IsAutoHideEnabled()) {
        g_states.clear();
        return;
    }

    std::vector<HWND> taskbars;
    CollectTaskbars(&taskbars);

    // Forget state for taskbars that no longer exist.
    for (auto it = g_states.begin(); it != g_states.end();) {
        if (std::find(taskbars.begin(), taskbars.end(), it->first) ==
            taskbars.end()) {
            it = g_states.erase(it);
        } else {
            ++it;
        }
    }

    ULONGLONG now = GetTickCount64();
    int hideDelaySeconds = g_hideDelaySeconds.load();
    ULONGLONG delayMs = (ULONGLONG)hideDelaySeconds * 1000;

    for (HWND hwnd : taskbars) {
        RECT rc;
        bool shown = IsTaskbarShown(hwnd, &rc);

        TaskbarState& state = g_states[hwnd];

        // Hidden -> reset the countdown.
        if (!shown) {
            state.countdownActive = false;
            state.countdownStart = 0;
            continue;
        }

        // Pointer over the taskbar -> the user wants to use it; cancel.
        if (IsPointerOver(rc)) {
            state.countdownActive = false;
            state.countdownStart = 0;
            continue;
        }

        // Visible and pointer is elsewhere -> start / continue the countdown.
        if (!state.countdownActive) {
            state.countdownActive = true;
            state.countdownStart = now;
            Wh_Log(L"Taskbar %08X visible, starting %d s countdown",
                   (DWORD)(ULONG_PTR)hwnd, hideDelaySeconds);
        } else if (now - state.countdownStart >= delayMs) {
            Wh_Log(L"Taskbar %08X: hiding after delay", (DWORD)(ULONG_PTR)hwnd);
            SetTimer(hwnd, kTrayUITimerHide, 0, nullptr);
            state.countdownActive = false;
            state.countdownStart = 0;
        }
    }
}

DWORD WINAPI PollThread(LPVOID /*param*/) {
    while (!g_stopFlag.load()) {
        PollOnce();
        Sleep(kPollIntervalMs);
    }
    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");
    LoadSettings();
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    g_stopFlag.store(false);
    g_threadHandle =
        CreateThread(nullptr, 0, PollThread, nullptr, 0, nullptr);
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    g_stopFlag.store(true);
    if (g_threadHandle) {
        WaitForSingleObject(g_threadHandle, 2000);
        CloseHandle(g_threadHandle);
        g_threadHandle = nullptr;
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
    g_states.clear();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    // Note: g_states is intentionally not touched here. It's owned by the
    // polling thread; the new delay is picked up on the next poll anyway.
    LoadSettings();
}
