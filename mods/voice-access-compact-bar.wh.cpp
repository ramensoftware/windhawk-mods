// ==WindhawkMod==
// @id              voice-access-compact-bar
// @name            Compact Voice Access bar
// @description     Turn the full-width Voice Access bar into a compact window and stop it from reserving space at the top of the screen
// @version         1.0
// @author          jcyrio
// @github          https://github.com/jcyrio
// @include         VoiceAccess.exe
// @architecture    x86-64
// @compilerOptions -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Compact Voice Access bar

By default, Windows 11 Voice Access docks a bar across the full width of the
monitor and reserves that strip of the screen, so every maximized window moves
down to make room. This mod turns the bar into a compact window that floats
near the top-right corner of the monitor, and removes the reserved strip so
other windows can use the whole screen again.

Requires Windows 11 with Voice Access. Keep Windhawk running while Voice Access
is open; the mod attaches a short moment after the bar appears.

## Settings

* **Compact placement**: turn the compact window on or off. When it is off,
  the mod leaves Voice Access exactly as Windows positions it.
* **Window width**: width of the compact window in pixels. The window is kept
  at least 300 pixels wide and never wider than the monitor.
* **Distance from top**: how far below the top edge of the monitor the compact
  window sits, in pixels.

Settings take effect immediately. Disabling the mod, or turning compact
placement off, restores the original full-width bar and its reserved strip.

## Notes

* The mod resizes the outer Voice Access window only. The contents of the bar
  are laid out by Voice Access itself, so a very narrow width may clip some of
  its controls. Widen the window if that happens.
* The mod attaches once per Voice Access launch, after the bar has been docked
  at the top of a monitor. If Voice Access recreates its bar later, for example
  after a display change, restart Voice Access to re-attach.
* Enable Windhawk logging for the mod to see the attach step, the window
  positions it applies, and the result of each work-area change.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- compact: true
  $name: Compact placement
  $description: Show Voice Access as a compact window instead of a full-width docked bar
- width: 900
  $name: Window width
  $description: Width of the compact window in pixels (at least 300)
- top: 100
  $name: Distance from top
  $description: Gap between the top of the monitor and the compact window, in pixels
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <windhawk_api.h>
#include <atomic>
#include <algorithm>

static std::atomic<HWND> g_bar{nullptr};
static std::atomic<bool> g_attached{false};
static std::atomic<bool> g_stopping{false};
static HANDLE g_stopEvent = nullptr;
static HANDLE g_attachThread = nullptr;
static SRWLOCK g_applyLock = SRWLOCK_INIT;
static RECT g_original;
static WNDPROC g_originalProc;
using AppbarFn = decltype(&SHAppBarMessage);
static AppbarFn g_originalAppbar;
static APPBARDATA g_reservation{};
static bool g_haveReservation = false;
static std::atomic<bool> g_released{false};
static std::atomic<bool> g_compact{false};
static std::atomic<int> g_width{900}, g_top{100}, g_logCount{0};

static bool IsBar(HWND window) {
    if (!g_attached.load() || window != g_bar.load() || !IsWindow(window)) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    wchar_t cls[128] = {};
    return pid == GetCurrentProcessId() &&
        GetClassNameW(window, cls, 128) && wcscmp(cls, L"Voice access") == 0;
}

static RECT DesiredRect(HWND window) {
    MONITORINFO monitor{}; monitor.cbSize = sizeof(monitor);
    if (!GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitor))
        return g_original;
    LONG maxWidth = monitor.rcMonitor.right - monitor.rcMonitor.left;
    LONG minWidth = std::min<LONG>(300, maxWidth);
    LONG width = std::clamp<LONG>(g_width.load(), minWidth, maxWidth);
    LONG height = g_original.bottom - g_original.top;
    LONG left = std::max(monitor.rcMonitor.left, monitor.rcMonitor.right - width - 16);
    LONG lowestTop = std::max(monitor.rcMonitor.top, monitor.rcMonitor.bottom - height);
    LONG top = std::clamp<LONG>(monitor.rcMonitor.top + g_top.load(),
                              monitor.rcMonitor.top, lowestTop);
    return RECT{left, top, left + width, top + height};
}

static LRESULT CALLBACK BarProcHook(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    bool target = IsBar(window) && message == WM_WINDOWPOSCHANGING && lParam;
    WINDOWPOS before{};
    if (target) before = *reinterpret_cast<WINDOWPOS*>(lParam);
    LRESULT result = g_originalProc(window, message, wParam, lParam);
    if (target) {
        auto pos = reinterpret_cast<WINDOWPOS*>(lParam);
        if (g_logCount.fetch_add(1) < 100)
            Wh_Log(L"WINDOWPOS before=(%d,%d,%d,%d flags=%x) after=(%d,%d,%d,%d flags=%x)",
                   before.x, before.y, before.cx, before.cy, before.flags,
                   pos->x, pos->y, pos->cx, pos->cy, pos->flags);
        if (!g_stopping.load() && g_compact.load()) {
            RECT desired = DesiredRect(window);
            pos->x = desired.left;
            pos->y = desired.top;
            pos->cx = desired.right - desired.left;
            pos->cy = desired.bottom - desired.top;
            pos->flags &= ~(SWP_NOMOVE | SWP_NOSIZE);
        }
    }
    return result;
}

struct BarSearch { HWND window = nullptr; bool ambiguous = false; };
static BOOL CALLBACK FindBar(HWND window, LPARAM context) {
    auto search = reinterpret_cast<BarSearch*>(context);
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    wchar_t cls[128] = {};
    if (pid == GetCurrentProcessId() && IsWindowVisible(window) &&
        GetClassNameW(window, cls, 128) && wcscmp(cls, L"Voice access") == 0) {
        if (search->window) { search->ambiguous = true; return FALSE; }
        search->window = window;
    }
    return TRUE;
}

// Preserve the output rectangle Voice Access expects while changing only the
// rectangle submitted to the shell. Leave registration and callbacks intact.
static UINT_PTR WINAPI AppbarHook(DWORD message, PAPPBARDATA data) {
    if (message == ABM_SETPOS && data && data->cbSize == sizeof(APPBARDATA) &&
        IsBar(data->hWnd) && data->uEdge == ABE_TOP && g_haveReservation &&
        !g_stopping.load() && g_compact.load()) {
        APPBARDATA zero = *data;
        zero.rc.bottom = zero.rc.top;
        UINT_PTR result = g_originalAppbar(message, &zero);
        if (result) g_released = true;
        Wh_Log(L"ABM_SETPOS compact: requested height=%ld, reserved height=0, result=%llu",
               data->rc.bottom - data->rc.top, static_cast<unsigned long long>(result));
        return result;
    }
    return g_originalAppbar(message, data);
}

static void ChangeReservation(bool release) {
    HWND bar = g_bar.load();
    if (!g_haveReservation || !IsBar(bar)) return;
    if (!release && !g_released) return;
    APPBARDATA data = g_reservation;
    if (release) data.rc.bottom = data.rc.top;
    UINT_PTR result = g_originalAppbar(ABM_SETPOS, &data);
    if (result) g_released = release;
    MONITORINFO monitor{}; monitor.cbSize = sizeof(monitor);
    GetMonitorInfoW(MonitorFromWindow(bar, MONITOR_DEFAULTTONEAREST), &monitor);
    Wh_Log(L"Reservation release=%d result=%llu workArea=(%ld,%ld,%ld,%ld)",
           int(release), static_cast<unsigned long long>(result),
           monitor.rcWork.left, monitor.rcWork.top, monitor.rcWork.right, monitor.rcWork.bottom);
}

static void ReadSettings() {
    g_width = Wh_GetIntSetting(L"width");
    g_top = Wh_GetIntSetting(L"top");
    g_compact = Wh_GetIntSetting(L"compact") != 0;
}

static void ApplyRect(RECT rect) {
    HWND bar = g_bar.load();
    if (!IsBar(bar)) return;
    SetLastError(0);
    BOOL ok = SetWindowPos(bar, nullptr, rect.left, rect.top,
                         rect.right - rect.left, rect.bottom - rect.top,
                         SWP_NOACTIVATE | SWP_NOZORDER);
    DWORD error = GetLastError();
    RECT actual{};
    GetWindowRect(bar, &actual);
    Wh_Log(L"SetWindowPos=%d error=%lu actual=(%ld,%ld,%ld,%ld)",
           ok, error, actual.left, actual.top,
           actual.right - actual.left, actual.bottom - actual.top);
}

// Windhawk can load this DLL before Voice Access creates any windows.
// Do not reject initialization merely because the bar is not present yet.
static DWORD WINAPI AttachWhenReady(void*) {
    while (WaitForSingleObject(g_stopEvent, 250) == WAIT_TIMEOUT) {
        BarSearch search;
        EnumWindows(FindBar, reinterpret_cast<LPARAM>(&search));
        if (!search.window || search.ambiguous) continue;
        RECT original{};
        MONITORINFO monitor{}; monitor.cbSize = sizeof(monitor);
        if (!GetWindowRect(search.window, &original) ||
            !GetMonitorInfoW(MonitorFromWindow(search.window, MONITOR_DEFAULTTONEAREST), &monitor))
            continue;
        LONG height = original.bottom - original.top;
        // Capture the real, initialized docked bar, not a startup placeholder.
        if (height <= 0 || height >= monitor.rcMonitor.bottom - monitor.rcMonitor.top ||
            original.left != monitor.rcMonitor.left || original.top != monitor.rcMonitor.top ||
            original.right != monitor.rcMonitor.right ||
            monitor.rcWork.top != monitor.rcMonitor.top + height) continue;

        auto proc = reinterpret_cast<void*>(GetWindowLongPtrW(search.window, GWLP_WNDPROC));
        if (!proc || !Wh_SetFunctionHook(proc, reinterpret_cast<void*>(BarProcHook),
                                        reinterpret_cast<void**>(&g_originalProc)) ||
            !Wh_ApplyHookOperations()) {
            Wh_Log(L"Failed to attach the window-procedure hook of the bar.");
            return 0;
        }
        g_bar = search.window;
        g_original = original;
        g_reservation = APPBARDATA{};
        g_reservation.cbSize = sizeof(g_reservation);
        g_reservation.hWnd = search.window;
        g_reservation.uEdge = ABE_TOP;
        g_reservation.rc = RECT{monitor.rcMonitor.left, monitor.rcMonitor.top,
                                monitor.rcMonitor.right, monitor.rcWork.top};
        g_haveReservation = true;
        // Publish the immutable original rectangle and reservation to the hooks.
        AcquireSRWLockExclusive(&g_applyLock);
        g_attached = true;
        Wh_Log(L"Attached to Voice Access bar %p; compact=%d",
               search.window, int(g_compact.load()));
        if (!g_stopping.load() && g_compact.load()) {
            ApplyRect(DesiredRect(search.window));
            ChangeReservation(true);
        }
        ReleaseSRWLockExclusive(&g_applyLock);
        return 0;
    }
    return 0;
}

BOOL Wh_ModInit() {
    g_bar = nullptr;
    g_attached = false;
    g_stopping = false;
    g_haveReservation = false;
    g_released = false;
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) return FALSE;
    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(SHAppBarMessage),
                            reinterpret_cast<void*>(AppbarHook),
                            reinterpret_cast<void**>(&g_originalAppbar))) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    ReadSettings();
    Wh_Log(L"Waiting for the Voice Access bar to initialize; compact=%d", int(g_compact.load()));
    return TRUE;
}

void Wh_ModAfterInit() {
    // This callback runs after Windhawk has installed the initial shell API hook.
    g_attachThread = CreateThread(nullptr, 0, AttachWhenReady, nullptr, 0, nullptr);
    if (!g_attachThread) Wh_Log(L"Could not start automatic attachment; error=%lu", GetLastError());
}

void Wh_ModSettingsChanged() {
    AcquireSRWLockExclusive(&g_applyLock);
    ReadSettings();
    if (g_attached.load() && !g_stopping.load()) {
        ApplyRect(g_compact.load() ? DesiredRect(g_bar.load()) : g_original);
        ChangeReservation(g_compact.load());
    }
    ReleaseSRWLockExclusive(&g_applyLock);
}

void Wh_ModBeforeUninit() {
    g_stopping = true;
    g_compact = false;
    if (g_stopEvent) SetEvent(g_stopEvent);
    // Join our thread before Windhawk removes hooks and unloads this DLL.
    if (g_attachThread) {
        WaitForSingleObject(g_attachThread, INFINITE);
        CloseHandle(g_attachThread);
        g_attachThread = nullptr;
    }
    if (g_attached.load()) {
        ApplyRect(g_original);
        ChangeReservation(false);
    }
    g_attached = false;
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}
