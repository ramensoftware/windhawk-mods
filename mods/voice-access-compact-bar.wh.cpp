// ==WindhawkMod==
// @id              voice-access-compact-bar
// @name            Compact Voice Access bar
// @description     Turn the full-width Voice Access bar into a compact window and stop it from reserving space at the top of the screen
// @version         1.0
// @author          jcyrio
// @github          https://github.com/jcyrio
// @include         VoiceAccess.exe
// @architecture    x86-64
// @compilerOptions -lshell32 -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Compact Voice Access bar

By default, Windows 11 Voice Access docks a bar across the full width of the
monitor and reserves that strip of the screen, so every maximized window moves
down to make room. This mod turns the bar into a compact window that floats
near the top of the monitor, aligned to the left, center or right, and removes
the reserved strip so other windows can use the whole screen again.

![Before and after](https://raw.githubusercontent.com/jcyrio/windhawk-voice-access-compact-bar/main/screenshot.jpg)

Requires Windows 11 with Voice Access. Keep Windhawk running while Voice Access
is open. The mod takes effect the moment Voice Access docks its bar, so the
full-width bar never flashes on screen, and it follows the bar if Voice Access
recreates it, for example after a display change.

## Settings

* **Compact placement**: turn the compact window on or off. When it is off,
  the mod leaves Voice Access exactly as Windows positions it.
* **Window width**: width of the compact window in pixels at 100% display
  scaling. The value is scaled automatically on high-DPI monitors. The window
  is kept at least 300 pixels wide and never wider than the monitor.
* **Distance from top**: how far below the top edge of the monitor the compact
  window sits, in pixels at 100% display scaling.
* **Horizontal alignment**: whether the compact window sits at the left edge,
  the center or the right edge of the monitor. Pick the side where it covers
  the least of your maximized windows.

Settings take effect immediately. Disabling the mod, or turning compact
placement off, restores the original full-width bar and its reserved strip.

## Notes

* The mod resizes the outer Voice Access window only. The contents of the bar
  are laid out by Voice Access itself, so a very narrow width may clip some of
  its controls. Widen the window if that happens.
* Enable Windhawk logging for the mod to see when it attaches to the bar, the
  window positions it applies, and the result of each work-area change.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- compact: true
  $name: Compact placement
  $description: Show Voice Access as a compact window instead of a full-width docked bar
- width: 900
  $name: Window width
  $description: Width of the compact window in pixels at 100% scaling (at least 300), scaled automatically on high-DPI monitors
- top: 100
  $name: Distance from top
  $description: Gap between the top of the monitor and the compact window, in pixels at 100% scaling
- alignment: right
  $name: Horizontal alignment
  $description: Which side of the monitor the compact window sits on
  $options:
  - left: Left
  - center: Center
  - right: Right
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <atomic>
#include <algorithm>

// The bar window that is currently subclassed, or nullptr.
static std::atomic<HWND> g_bar{nullptr};
static std::atomic<bool> g_stopping{false};
static std::atomic<bool> g_compact{false};
// True while the shell holds a zero-height reservation submitted by this mod.
static std::atomic<bool> g_released{false};
enum class Alignment { Left, Center, Right };
static std::atomic<Alignment> g_alignment{Alignment::Right};
static std::atomic<int> g_width{900}, g_top{100}, g_logCount{0};

// Geometry of the docked bar as Voice Access last requested it. This is both
// the rectangle to restore and the reservation to give back. Guarded by
// g_geometryLock, which is only ever held for a copy, never across a call into
// user32 or the shell.
static SRWLOCK g_geometryLock = SRWLOCK_INIT;
static RECT g_docked{};
static bool g_haveDocked = false;

using SHAppBarMessage_t = decltype(&SHAppBarMessage);
static SHAppBarMessage_t g_originalAppbar;

static void SetDocked(const RECT& rect) {
    AcquireSRWLockExclusive(&g_geometryLock);
    g_docked = rect;
    g_haveDocked = true;
    ReleaseSRWLockExclusive(&g_geometryLock);
}

static bool GetDocked(RECT* rect) {
    AcquireSRWLockShared(&g_geometryLock);
    bool have = g_haveDocked;
    if (have) *rect = g_docked;
    ReleaseSRWLockShared(&g_geometryLock);
    return have;
}

// A top-level "Voice access" window that belongs to this process.
static bool IsBarClass(HWND window) {
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    wchar_t cls[128] = {};
    return pid == GetCurrentProcessId() &&
        GetClassNameW(window, cls, 128) && wcscmp(cls, L"Voice access") == 0;
}

static bool IsAttachedBar(HWND window) {
    return window && window == g_bar.load() && IsWindow(window);
}

// Compact rectangle near the top of the monitor the bar is on, aligned per the
// settings. Pixel settings are given at 100% scaling and scaled to the DPI of
// the bar's monitor.
static RECT DesiredRect(HWND window, const RECT& docked) {
    MONITORINFO monitor{}; monitor.cbSize = sizeof(monitor);
    if (!GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitor))
        return docked;
    UINT dpi = GetDpiForWindow(window);
    if (!dpi) dpi = USER_DEFAULT_SCREEN_DPI;
    auto scale = [dpi](int value) -> LONG {
        return MulDiv(value, dpi, USER_DEFAULT_SCREEN_DPI);
    };
    LONG maxWidth = monitor.rcMonitor.right - monitor.rcMonitor.left;
    LONG minWidth = std::min<LONG>(scale(300), maxWidth);
    LONG width = std::clamp<LONG>(scale(g_width.load()), minWidth, maxWidth);
    LONG height = docked.bottom - docked.top;
    LONG margin = scale(16);
    LONG left;
    switch (g_alignment.load()) {
        case Alignment::Left:
            left = monitor.rcMonitor.left + margin;
            break;
        case Alignment::Center:
            left = monitor.rcMonitor.left + (maxWidth - width) / 2;
            break;
        default:
            left = monitor.rcMonitor.right - width - margin;
            break;
    }
    left = std::clamp<LONG>(left, monitor.rcMonitor.left, monitor.rcMonitor.right - width);
    LONG lowestTop = std::max(monitor.rcMonitor.top, monitor.rcMonitor.bottom - height);
    LONG top = std::clamp<LONG>(monitor.rcMonitor.top + scale(g_top.load()),
                              monitor.rcMonitor.top, lowestTop);
    return RECT{left, top, left + width, top + height};
}

static LRESULT CALLBACK BarSubclassProc(HWND window, UINT message, WPARAM wParam,
                                        LPARAM lParam, DWORD_PTR) {
    // Let Voice Access handle the message first, then override the result.
    LRESULT result = DefSubclassProc(window, message, wParam, lParam);
    if (message == WM_WINDOWPOSCHANGING && lParam && !g_stopping.load() && g_compact.load()) {
        RECT docked{};
        if (GetDocked(&docked)) {
            auto pos = reinterpret_cast<WINDOWPOS*>(lParam);
            RECT desired = DesiredRect(window, docked);
            if (g_logCount.fetch_add(1) < 100)
                Wh_Log(L"WINDOWPOS requested=(%d,%d,%d,%d flags=%x) applied=(%ld,%ld,%ld,%ld)",
                       pos->x, pos->y, pos->cx, pos->cy, pos->flags,
                       desired.left, desired.top,
                       desired.right - desired.left, desired.bottom - desired.top);
            pos->x = desired.left;
            pos->y = desired.top;
            pos->cx = desired.right - desired.left;
            pos->cy = desired.bottom - desired.top;
            pos->flags &= ~(SWP_NOMOVE | SWP_NOSIZE);
        }
    } else if (message == WM_NCDESTROY) {
        // The subclass itself is removed by WindhawkUtils. Forget the window so
        // the next ABM_SETPOS from a recreated bar attaches again.
        HWND expected = window;
        if (g_bar.compare_exchange_strong(expected, nullptr))
            Wh_Log(L"Voice Access bar %p destroyed; waiting for a new one", window);
    }
    return result;
}

// Subclass the bar if it is not attached yet. Returns true when `window` is
// the attached bar afterwards.
static bool Attach(HWND window, const RECT& docked) {
    HWND expected = nullptr;
    if (!g_bar.compare_exchange_strong(expected, window)) return expected == window;
    SetDocked(docked);
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(window, BarSubclassProc, 0)) {
        Wh_Log(L"Failed to subclass Voice Access bar %p; error=%lu", window, GetLastError());
        g_bar = nullptr;
        return false;
    }
    // Wh_ModBeforeUninit may have checked g_bar before it was claimed above.
    // It sets g_stopping first, so one of the two sides always removes the
    // subclass; removing it twice is harmless.
    if (g_stopping.load()) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(window, BarSubclassProc);
        g_bar = nullptr;
        return false;
    }
    Wh_Log(L"Attached to Voice Access bar %p; docked=(%ld,%ld,%ld,%ld) compact=%d",
           window, docked.left, docked.top, docked.right, docked.bottom,
           int(g_compact.load()));
    return true;
}

// Voice Access docks its bar with ABM_SETPOS. That call is the signal to
// attach, the source of the docked geometry, and, in compact mode, the place
// to hand the shell a zero-height reservation. The rectangle returned to Voice
// Access is left untouched so that it positions the bar as usual; the subclass
// then moves it to the compact rectangle.
static UINT_PTR WINAPI AppbarHook(DWORD message, PAPPBARDATA data) {
    if (message == ABM_SETPOS && data && data->cbSize == sizeof(APPBARDATA) &&
        data->uEdge == ABE_TOP && !g_stopping.load() && IsBarClass(data->hWnd) &&
        Attach(data->hWnd, data->rc)) {
        SetDocked(data->rc);
        if (g_compact.load()) {
            APPBARDATA zero = *data;
            zero.rc.bottom = zero.rc.top;
            UINT_PTR result = g_originalAppbar(message, &zero);
            g_released = true;
            Wh_Log(L"ABM_SETPOS compact: requested height=%ld, reserved height=0, result=%llu",
                   data->rc.bottom - data->rc.top, static_cast<unsigned long long>(result));
            return result;
        }
    }
    return g_originalAppbar(message, data);
}

// Release (zero height) or restore the reserved strip of the attached bar.
static void ChangeReservation(bool release) {
    HWND bar = g_bar.load();
    RECT docked{};
    if (!IsAttachedBar(bar) || !GetDocked(&docked)) return;
    if (!release && !g_released.load()) return;
    APPBARDATA data{};
    data.cbSize = sizeof(data);
    data.hWnd = bar;
    data.uEdge = ABE_TOP;
    data.rc = docked;
    if (release) data.rc.bottom = data.rc.top;
    UINT_PTR result = g_originalAppbar(ABM_SETPOS, &data);
    g_released = release;
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
    PCWSTR alignment = Wh_GetStringSetting(L"alignment");
    if (wcscmp(alignment, L"left") == 0) g_alignment = Alignment::Left;
    else if (wcscmp(alignment, L"center") == 0) g_alignment = Alignment::Center;
    else g_alignment = Alignment::Right;
    Wh_FreeStringSetting(alignment);
}

static void ApplyRect(RECT rect) {
    HWND bar = g_bar.load();
    if (!IsAttachedBar(bar)) return;
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

struct BarSearch { HWND window = nullptr; bool ambiguous = false; };
static BOOL CALLBACK FindBar(HWND window, LPARAM context) {
    auto search = reinterpret_cast<BarSearch*>(context);
    if (IsWindowVisible(window) && IsBarClass(window)) {
        if (search->window) { search->ambiguous = true; return FALSE; }
        search->window = window;
    }
    return TRUE;
}

BOOL Wh_ModInit() {
    g_bar = nullptr;
    g_stopping = false;
    g_released = false;
    AcquireSRWLockExclusive(&g_geometryLock);
    g_haveDocked = false;
    ReleaseSRWLockExclusive(&g_geometryLock);
    if (!WindhawkUtils::SetFunctionHook(SHAppBarMessage, AppbarHook, &g_originalAppbar)) {
        Wh_Log(L"Failed to hook SHAppBarMessage");
        return FALSE;
    }
    ReadSettings();
    return TRUE;
}

void Wh_ModAfterInit() {
    // Voice Access may already be running with its bar docked, in which case
    // no ABM_SETPOS is coming. The docked window rectangle is the reservation.
    BarSearch search;
    EnumWindows(FindBar, reinterpret_cast<LPARAM>(&search));
    if (!search.window || search.ambiguous) {
        Wh_Log(L"No docked bar yet; waiting for Voice Access to dock it");
        return;
    }
    RECT docked{};
    if (!GetWindowRect(search.window, &docked) || !Attach(search.window, docked)) return;
    if (!g_stopping.load() && g_compact.load()) {
        ApplyRect(DesiredRect(search.window, docked));
        ChangeReservation(true);
    }
}

void Wh_ModSettingsChanged() {
    ReadSettings();
    if (g_stopping.load()) return;
    HWND bar = g_bar.load();
    RECT docked{};
    if (!IsAttachedBar(bar) || !GetDocked(&docked)) return;
    ApplyRect(g_compact.load() ? DesiredRect(bar, docked) : docked);
    ChangeReservation(g_compact.load());
}

void Wh_ModBeforeUninit() {
    g_stopping = true;
    g_compact = false;
    HWND bar = g_bar.load();
    RECT docked{};
    if (IsAttachedBar(bar) && GetDocked(&docked)) {
        ChangeReservation(false);
        ApplyRect(docked);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(bar, BarSubclassProc);
    }
    g_bar = nullptr;
}
