// ==WindhawkMod==
// @id              taskbar-disable-per-monitor
// @name            Disable taskbar per monitor
// @description     Turns the taskbar off for good on the chosen monitors: it never shows (no hover, Win key, notifications or flashing apps bring it back); taskbars on the other monitors stay as usual
// @version         1.0
// @author          eprilx
// @github          https://github.com/eprilx
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32
// @license         GPL-3.0-only
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Logic that keeps a taskbar visible while auto-hide is on is adapted from
// "Taskbar auto-hide per monitor" by m417z (GPL-3.0):
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-auto-hide-per-monitor.wh.cpp

// ==WindhawkModReadme==
/*
# Disable taskbar per monitor

Pick one or more monitors. Their taskbar is disabled: unlike auto-hide it
never slides back in, whether from the mouse at the screen edge, the Win key,
Win+T / Win+number, flashing app buttons or notifications. Maximized windows on
those monitors use the full screen. Taskbars on every other monitor stay
visible as usual. Works together with **Taskbar multi-tray**.

## Choosing monitors

The **Monitors to disable the taskbar on** setting is a comma-separated list.
Each entry can be:

* `primary`: the primary monitor, wherever it is.
* A number (`1`, `2`, `3`...): the monitor's position in Windows' monitor
  enumeration. **Taskbar multi-tray** uses the same numbering. It may differ
  from the numbers shown in Display Settings, and it can change after
  plugging monitors in or out.
* Any other text: matched (case-insensitive) against part of the monitor's
  device name, model name or interface name, e.g. `GSM5BD2` or `DISPLAY2`.
  This stays correct when the numbering changes.

Examples: `primary` · `1,3` · `primary, GSM5BD2`. An empty list disables
nothing.

To see the number, device name, model and interface name of each monitor:
Advanced tab, set **Debug logging** to **Mod logs**, open **Show log output**,
then save the settings again. Every monitor is logged in a `Monitor` line.

## Requirements and notes

* Windows 11. Tested on build 26300 with a bottom taskbar. "Show my taskbar on all displays" must be on if any of the
  monitors is not the primary one.
* Disable **Taskbar auto-hide per monitor**. It hooks the same functions.
* You don't need to turn on "Automatically hide the taskbar" yourself. If it is
  off, the mod turns it on, and turns it off again when the mod is disabled.
  Don't switch it off in Settings while the mod is running.
* Disabled taskbars are parked just below the bottom edge of their monitor. If
  another monitor sits directly below, the parked taskbar could show on that
  monitor.
* When explorer starts, the disabled taskbars can flicker up for 1-2 s
  before the mod applies its state.
* Only bottom taskbars are handled.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- disableOn: "primary"
  $name: Monitors to disable the taskbar on
  $description: >-
    Comma-separated. Each entry: "primary", a monitor number (1, 2, 3...), or
    part of the monitor's device / model / interface name (see the mod's
    description for how to find them). Example: primary, 3
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <commctrl.h>
#include <shellapi.h>

#include <atomic>
#include <cwctype>
#include <mutex>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// State.

// true -> every hook behaves like stock Windows (used while toggling the
// auto-hide setting and while unloading).
std::atomic<bool> g_pause;

// true -> TrayUI::GetAutoHideFlags reports "no auto-hide". Only set around
// calls made on the taskbar UI thread.
bool g_forceNoAutoHide;

// true while TaskbarHost::Start_System / Start is running.
std::atomic<bool> g_inTaskbarStart;

std::atomic<bool> g_taskbarViewDllLoaded;

HANDLE g_stopEvent;
HANDLE g_applyThread;

constexpr WCHAR kAutoHideEnabledByModValue[] = L"autoHideEnabledByMod";

// Marks taskbar windows this mod has subclassed.
constexpr WCHAR kSubclassedProp[] = L"Windhawk_taskbar-disable-per-monitor";

// Timer on the primary taskbar window: picks up taskbar windows created later
// (monitor plugged in, explorer re-creating secondary taskbars).
constexpr UINT_PTR kRescanTimerId = 0x57484D31;
constexpr UINT kRescanIntervalMs = 2000;

// Sent to the primary taskbar window to start (wParam=1) or stop (wParam=0)
// the rescan timer on its own thread (timers can't be set from another one).
UINT GetRescanTimerMsg() {
    static UINT msg =
        RegisterWindowMessage(L"Windhawk_taskbar-disable-per-monitor_timer");
    return msg;
}

// TrayUI's auto-hide timer (see taskbar-auto-hide-per-monitor).
constexpr UINT_PTR kTrayUITimerHide = 2;

////////////////////////////////////////////////////////////////////////////////
// Settings and monitor matching.

struct MonitorSelector {
    enum class Kind { Primary, Number, Name } kind;
    int number;
    std::wstring name;  // lower case
};

std::mutex g_settingsMutex;
std::vector<MonitorSelector> g_selectors;

std::wstring ToLower(std::wstring s) {
    for (auto& c : s) {
        c = (WCHAR)towlower(c);
    }
    return s;
}

std::wstring Trim(const std::wstring& s) {
    size_t begin = s.find_first_not_of(L" \t");
    if (begin == std::wstring::npos) {
        return L"";
    }
    size_t end = s.find_last_not_of(L" \t");
    return s.substr(begin, end - begin + 1);
}

void LoadSettings() {
    std::vector<MonitorSelector> selectors;

    PCWSTR disableOn = Wh_GetStringSetting(L"disableOn");
    std::wstring list = disableOn ? disableOn : L"";
    Wh_FreeStringSetting(disableOn);

    size_t pos = 0;
    while (pos <= list.size()) {
        size_t comma = list.find(L',', pos);
        if (comma == std::wstring::npos) {
            comma = list.size();
        }

        std::wstring token = ToLower(Trim(list.substr(pos, comma - pos)));
        pos = comma + 1;

        if (token.empty()) {
            continue;
        }

        if (token == L"primary") {
            selectors.push_back({MonitorSelector::Kind::Primary, 0, L""});
        } else if (token.find_first_not_of(L"0123456789") ==
                   std::wstring::npos) {
            selectors.push_back(
                {MonitorSelector::Kind::Number, _wtoi(token.c_str()), L""});
        } else {
            selectors.push_back({MonitorSelector::Kind::Name, 0, token});
        }
    }

    Wh_Log(L"Settings: disableOn=\"%s\", %d entries", list.c_str(),
           (int)selectors.size());

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_selectors = std::move(selectors);
}

struct MonitorEntry {
    HMONITOR monitor;
    int number;  // 1-based EnumDisplayMonitors position
    bool primary;
    RECT rect;
    WCHAR device[CCHDEVICENAME];
    WCHAR model[128];
    WCHAR interfaceName[128];
    bool hidden;
};

std::vector<MonitorEntry> EnumMonitors() {
    std::vector<MonitorEntry> monitors;

    auto proc = [&](HMONITOR monitor) {
        MonitorEntry entry = {};
        entry.monitor = monitor;
        entry.number = (int)monitors.size() + 1;

        MONITORINFOEX mi = {};
        mi.cbSize = sizeof(mi);
        if (GetMonitorInfo(monitor, &mi)) {
            entry.primary = !!(mi.dwFlags & MONITORINFOF_PRIMARY);
            entry.rect = mi.rcMonitor;
            wcscpy_s(entry.device, mi.szDevice);

            DISPLAY_DEVICE dd = {.cb = sizeof(dd)};
            if (EnumDisplayDevices(mi.szDevice, 0, &dd,
                                   EDD_GET_DEVICE_INTERFACE_NAME)) {
                wcscpy_s(entry.model, dd.DeviceString);
                wcscpy_s(entry.interfaceName, dd.DeviceID);
            }
        }

        monitors.push_back(entry);
    };

    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR monitor, HDC, LPRECT, LPARAM lParam) -> BOOL {
            (*reinterpret_cast<decltype(proc)*>(lParam))(monitor);
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&proc));

    return monitors;
}

bool SelectorMatches(const MonitorSelector& selector,
                     const MonitorEntry& entry) {
    switch (selector.kind) {
        case MonitorSelector::Kind::Primary:
            return entry.primary;
        case MonitorSelector::Kind::Number:
            return selector.number == entry.number;
        case MonitorSelector::Kind::Name:
            for (PCWSTR field :
                 {entry.device, entry.model, entry.interfaceName}) {
                if (ToLower(field).find(selector.name) != std::wstring::npos) {
                    return true;
                }
            }
            return false;
    }
    return false;
}

// Hidden monitors are resolved once per kCacheMs (the checks run on every
// taskbar move) and logged whenever the result changes.
constexpr ULONGLONG kCacheMs = 1000;

std::mutex g_monitorCacheMutex;
std::vector<HMONITOR> g_hiddenMonitors;
ULONGLONG g_monitorCacheTime;
bool g_monitorCacheValid;
std::wstring g_monitorCacheSignature;

void InvalidateMonitorCache() {
    std::lock_guard<std::mutex> lock(g_monitorCacheMutex);
    g_monitorCacheValid = false;
    g_monitorCacheSignature.clear();
}

void RefreshMonitorCacheLocked() {
    std::vector<MonitorSelector> selectors;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        selectors = g_selectors;
    }

    auto monitors = EnumMonitors();

    std::vector<HMONITOR> hidden;
    std::wstring signature;
    for (auto& entry : monitors) {
        for (const auto& selector : selectors) {
            if (SelectorMatches(selector, entry)) {
                entry.hidden = true;
                break;
            }
        }

        if (entry.hidden) {
            hidden.push_back(entry.monitor);
        }

        signature += std::to_wstring((ULONG_PTR)entry.monitor) + L":" +
                     std::to_wstring(entry.hidden) + L";";
    }

    if (signature != g_monitorCacheSignature) {
        for (const auto& entry : monitors) {
            Wh_Log(L"Monitor %d (%p): %s%s (%d,%d)-(%d,%d) model=\"%s\" "
                   L"interface=%s -> %s",
                   entry.number, entry.monitor, entry.device,
                   entry.primary ? L" primary" : L"", entry.rect.left,
                   entry.rect.top, entry.rect.right, entry.rect.bottom,
                   entry.model, entry.interfaceName,
                   entry.hidden ? L"DISABLED" : L"enabled");
        }
        g_monitorCacheSignature = signature;
    }

    g_hiddenMonitors = std::move(hidden);
    g_monitorCacheTime = GetTickCount64();
    g_monitorCacheValid = true;
}

bool IsHiddenMonitor(HMONITOR monitor) {
    if (!monitor) {
        return false;
    }

    std::lock_guard<std::mutex> lock(g_monitorCacheMutex);

    if (!g_monitorCacheValid ||
        GetTickCount64() - g_monitorCacheTime >= kCacheMs) {
        RefreshMonitorCacheLocked();
    }

    for (HMONITOR hiddenMonitor : g_hiddenMonitors) {
        if (hiddenMonitor == monitor) {
            return true;
        }
    }
    return false;
}

HMONITOR GetPrimaryMonitor() {
    return MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
}

// The taskbar on this monitor must stay visible (auto-hide off for it).
bool KeepTaskbarShownOnMonitor(HMONITOR monitor) {
    return !g_pause && monitor && !IsHiddenMonitor(monitor);
}

// The taskbar on this monitor must never show.
bool KeepTaskbarHiddenOnMonitor(HMONITOR monitor) {
    return !g_pause && !g_inTaskbarStart && IsHiddenMonitor(monitor);
}

////////////////////////////////////////////////////////////////////////////////
// Taskbar windows.

bool IsTaskbarWnd(HWND hWnd) {
    WCHAR className[32];
    return hWnd && GetClassName(hWnd, className, ARRAYSIZE(className)) &&
           (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
            _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0);
}

bool IsCurrentProcessWnd(HWND hWnd) {
    DWORD dwProcessId;
    return GetWindowThreadProcessId(hWnd, &dwProcessId) &&
           dwProcessId == GetCurrentProcessId();
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            WCHAR className[32];
            if (IsCurrentProcessWnd(hWnd) &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

template <typename F>
void ForEachCurrentProcessTaskbarWnd(F f) {
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            if (IsCurrentProcessWnd(hWnd) && IsTaskbarWnd(hWnd)) {
                (*reinterpret_cast<F*>(lParam))(hWnd);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&f));
}

// Hidden taskbars are parked fully below their monitor, so neither the taskbar
// nor the 2 px strip that auto-hide normally leaves (which is what detects the
// mouse) is on screen. Every move of a bottom-docked taskbar whose monitor is
// hidden is redirected there, so the taskbar stays off screen even if some
// path reveals it.
void ParkTaskbarIfHidden(HWND hWnd, WINDOWPOS* wp) {
    if (wp->flags & SWP_NOMOVE) {
        return;
    }

    int cx = wp->cx;
    int cy = wp->cy;
    if (wp->flags & SWP_NOSIZE) {
        RECT rc;
        if (!GetWindowRect(hWnd, &rc)) {
            return;
        }
        cx = rc.right - rc.left;
        cy = rc.bottom - rc.top;
    }

    RECT target = {wp->x, wp->y, wp->x + cx, wp->y + cy};
    HMONITOR monitor = MonitorFromRect(&target, MONITOR_DEFAULTTONEAREST);
    if (!KeepTaskbarHiddenOnMonitor(monitor)) {
        return;
    }

    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(monitor, &mi)) {
        return;
    }

    // Bottom-docked: its bottom edge is at (shown) or below (hidden) the
    // monitor's bottom edge.
    if (target.bottom >= mi.rcMonitor.bottom &&
        target.top < mi.rcMonitor.bottom) {
        Wh_Log(L"Parking taskbar %p at y=%d instead of %d", hWnd,
               mi.rcMonitor.bottom, wp->y);
        wp->y = mi.rcMonitor.bottom;
    }
}

// A taskbar can be on screen without moving (e.g. it was shown when the mod
// started and never got a reason to hide, like a flashing button). Move such
// taskbars; the move itself is redirected below the monitor.
void ParkVisibleHiddenTaskbar(HWND hWnd) {
    RECT rc;
    if (!IsWindowVisible(hWnd) || !GetWindowRect(hWnd, &rc)) {
        return;
    }

    HMONITOR monitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(mi)};
    if (!KeepTaskbarHiddenOnMonitor(monitor) || !GetMonitorInfo(monitor, &mi)) {
        return;
    }

    if (rc.bottom >= mi.rcMonitor.bottom && rc.top < mi.rcMonitor.bottom) {
        Wh_Log(L"Taskbar %p is on screen, parking it", hWnd);
        SetWindowPos(hWnd, nullptr, rc.left, mi.rcMonitor.bottom, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

void SubclassTaskbarWnd(HWND hWnd);
void HideHiddenTaskbarPopups();

// The taskbar sets its own position in WM_WINDOWPOSCHANGING, so the position is
// adjusted after its handler ran.
LRESULT CALLBACK TaskbarSubclassProc(HWND hWnd,
                                     UINT uMsg,
                                     WPARAM wParam,
                                     LPARAM lParam,
                                     DWORD_PTR dwRefData) {
    if (uMsg == GetRescanTimerMsg()) {
        if (wParam) {
            SetTimer(hWnd, kRescanTimerId, kRescanIntervalMs, nullptr);
        } else {
            KillTimer(hWnd, kRescanTimerId);
        }
        return 0;
    }

    if (uMsg == WM_TIMER && wParam == kRescanTimerId) {
        if (!g_pause) {
            ForEachCurrentProcessTaskbarWnd([](HWND hTaskbarWnd) {
                SubclassTaskbarWnd(hTaskbarWnd);
                ParkVisibleHiddenTaskbar(hTaskbarWnd);
            });
        }
        return 0;
    }

    if (uMsg == WM_NCDESTROY) {
        RemoveProp(hWnd, kSubclassedProp);
    }

    LRESULT ret = DefSubclassProc(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_WINDOWPOSCHANGING:
            ParkTaskbarIfHidden(hWnd, reinterpret_cast<WINDOWPOS*>(lParam));
            break;

        case WM_DISPLAYCHANGE:
            InvalidateMonitorCache();
            break;
    }

    return ret;
}

void SubclassTaskbarWnd(HWND hWnd) {
    if (GetProp(hWnd, kSubclassedProp)) {
        return;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd,
                                                        TaskbarSubclassProc, 0)) {
        Wh_Log(L"Subclassing taskbar window %p failed", hWnd);
        return;
    }

    SetProp(hWnd, kSubclassedProp, (HANDLE)1);
    Wh_Log(L"Subclassed taskbar window %p", hWnd);

    WCHAR className[32];
    if (GetClassName(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"Shell_TrayWnd") == 0) {
        SendMessage(hWnd, GetRescanTimerMsg(), TRUE, 0);
    }
}

void UnsubclassTaskbarWnds() {
    ForEachCurrentProcessTaskbarWnd([](HWND hWnd) {
        if (GetProp(hWnd, kSubclassedProp)) {
            SendMessage(hWnd, GetRescanTimerMsg(), FALSE, 0);
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hWnd, TaskbarSubclassProc);
            RemoveProp(hWnd, kSubclassedProp);
        }
    });
}

// Popups of a taskbar (tooltips, previews, ...) are top-level windows owned by
// the taskbar window. Keyboard focus (Win+T etc.) can still open them over a
// hidden taskbar; keep them hidden.
bool IsOwnedByHiddenTaskbar(HWND hWnd) {
    if (!hWnd) {
        return false;
    }

    HWND hOwner = GetWindow(hWnd, GW_OWNER);
    return hOwner && IsTaskbarWnd(hOwner) &&
           KeepTaskbarHiddenOnMonitor(
               MonitorFromWindow(hOwner, MONITOR_DEFAULTTONEAREST));
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
    if ((uFlags & SWP_SHOWWINDOW) && IsOwnedByHiddenTaskbar(hWnd)) {
        Wh_Log(L"Kept taskbar popup %p hidden (SetWindowPos)", hWnd);
        uFlags &= ~SWP_SHOWWINDOW;
    }

    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (nCmdShow != SW_HIDE && IsOwnedByHiddenTaskbar(hWnd)) {
        Wh_Log(L"Kept taskbar popup %p hidden (ShowWindow %d)", hWnd, nCmdShow);
        return IsWindowVisible(hWnd);
    }

    return ShowWindow_Original(hWnd, nCmdShow);
}

// Hide popups that are already open (e.g. when the mod gets enabled).
void HideHiddenTaskbarPopups() {
    EnumWindows(
        [](HWND hWnd, LPARAM) -> BOOL {
            if (IsCurrentProcessWnd(hWnd) && IsWindowVisible(hWnd) &&
                IsOwnedByHiddenTaskbar(hWnd)) {
                Wh_Log(L"Hiding open taskbar popup %p", hWnd);
                ShowWindow_Original(hWnd, SW_HIDE);
            }
            return TRUE;
        },
        0);
}

////////////////////////////////////////////////////////////////////////////////
// Applying / restoring.

// Turn auto-hide on (remembering whether we did it), or toggle it off/on so
// that every taskbar reloads its auto-hide state through the hooks below.
void ApplyAutoHideState() {
    if (!FindCurrentProcessTaskbarWnd()) {
        return;
    }

    InvalidateMonitorCache();

    ForEachCurrentProcessTaskbarWnd(
        [](HWND hWnd) { SubclassTaskbarWnd(hWnd); });

    APPBARDATA abd = {sizeof(abd)};

    g_pause = true;

    UINT state = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
    if (state & ABS_AUTOHIDE) {
        abd.lParam = state & ~ABS_AUTOHIDE;
        SHAppBarMessage(ABM_SETSTATE, &abd);
    } else {
        Wh_Log(L"Auto-hide was off, enabling it");
        Wh_SetIntValue(kAutoHideEnabledByModValue, 1);
    }

    g_pause = false;

    abd.lParam = state | ABS_AUTOHIDE;
    SHAppBarMessage(ABM_SETSTATE, &abd);

    ForEachCurrentProcessTaskbarWnd(
        [](HWND hWnd) { ParkVisibleHiddenTaskbar(hWnd); });
    HideHiddenTaskbarPopups();
    Wh_Log(L"Applied, initial state=0x%X", state);
}

// Back to stock behavior. Must be called with g_pause == true.
void RestoreAutoHideState() {
    if (!FindCurrentProcessTaskbarWnd()) {
        return;
    }

    APPBARDATA abd = {sizeof(abd)};
    UINT state = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);

    if (Wh_GetIntValue(kAutoHideEnabledByModValue, 0)) {
        Wh_Log(L"Restoring auto-hide off");
        abd.lParam = state & ~ABS_AUTOHIDE;
        SHAppBarMessage(ABM_SETSTATE, &abd);
        Wh_SetIntValue(kAutoHideEnabledByModValue, 0);
    } else if (state & ABS_AUTOHIDE) {
        abd.lParam = state & ~ABS_AUTOHIDE;
        SHAppBarMessage(ABM_SETSTATE, &abd);
        abd.lParam = state;
        SHAppBarMessage(ABM_SETSTATE, &abd);
    }
}

// Waits for this process's taskbar to finish starting, then applies the state.
DWORD WINAPI ApplyThreadProc(LPVOID) {
    constexpr int kPollMs = 250;
    constexpr int kMaxWaitMs = 60000;
    constexpr int kSettleMs = 300;

    for (int waited = 0;; waited += kPollMs) {
        if (FindCurrentProcessTaskbarWnd() && !g_inTaskbarStart) {
            break;
        }
        if (waited >= kMaxWaitMs) {
            // Not the shell process (e.g. a separate folder window process).
            return 0;
        }
        if (WaitForSingleObject(g_stopEvent, kPollMs) != WAIT_TIMEOUT) {
            return 0;
        }
    }

    if (WaitForSingleObject(g_stopEvent, kSettleMs) != WAIT_TIMEOUT) {
        return 0;
    }

    ApplyAutoHideState();
    return 0;
}

void ScheduleApply() {
    static std::atomic<bool> scheduled;
    if (scheduled.exchange(true)) {
        return;
    }

    g_applyThread =
        CreateThread(nullptr, 0, ApplyThreadProc, nullptr, 0, nullptr);
}

////////////////////////////////////////////////////////////////////////////////
// Primary taskbar (TrayUI / Shell_TrayWnd).

bool PrimaryTaskbarHidden() {
    return KeepTaskbarHiddenOnMonitor(GetPrimaryMonitor());
}

bool PrimaryTaskbarShown() {
    return KeepTaskbarShownOnMonitor(GetPrimaryMonitor());
}

using TrayUI_Unhide_t = void(WINAPI*)(void* pThis, int flags, int request);
TrayUI_Unhide_t TrayUI_Unhide_Original;
void WINAPI TrayUI_Unhide_Hook(void* pThis, int flags, int request) {
    if (PrimaryTaskbarHidden()) {
        Wh_Log(L"Blocked TrayUI::Unhide flags=%d request=%d", flags, request);
        return;
    }

    TrayUI_Unhide_Original(pThis, flags, request);
}

using TrayUI__UnhideNow_t = void(WINAPI*)(void* pThis, bool param1);
TrayUI__UnhideNow_t TrayUI__UnhideNow_Original;
void WINAPI TrayUI__UnhideNow_Hook(void* pThis, bool param1) {
    if (PrimaryTaskbarHidden()) {
        Wh_Log(L"Blocked TrayUI::_UnhideNow %d", param1);
        return;
    }

    TrayUI__UnhideNow_Original(pThis, param1);
}

// Polled by the hide timer while the taskbar is shown. Normally false while
// the cursor is over the taskbar or a button is flashing; forced to true so a
// taskbar that got shown anyway hides on the next tick. The telemetry
// argument is a small class passed by value: forwarded untouched.
using TrayUI__ShouldAutoHideTray_t = bool(WINAPI*)(void* pThis,
                                                   ULONG_PTR telemetry);
TrayUI__ShouldAutoHideTray_t TrayUI__ShouldAutoHideTray_Original;
bool WINAPI TrayUI__ShouldAutoHideTray_Hook(void* pThis, ULONG_PTR telemetry) {
    bool ret = TrayUI__ShouldAutoHideTray_Original(pThis, telemetry);

    if (!ret && PrimaryTaskbarHidden()) {
        Wh_Log(L"Forcing _ShouldAutoHideTray -> true");
        return true;
    }

    return ret;
}

// Primary taskbar kept visible: never start hiding it.
using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;
void WINAPI TrayUI__Hide_Hook(void* pThis) {
    if (PrimaryTaskbarShown()) {
        if (HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd()) {
            KillTimer(hTaskbarWnd, kTrayUITimerHide);
        }
        return;
    }

    TrayUI__Hide_Original(pThis);
}

////////////////////////////////////////////////////////////////////////////////
// Auto-hide flag per monitor (adapted from taskbar-auto-hide-per-monitor).

using TrayUI_GetAutoHideFlags_t = DWORD(WINAPI*)(void* pThis);
TrayUI_GetAutoHideFlags_t TrayUI_GetAutoHideFlags_Original;
DWORD WINAPI TrayUI_GetAutoHideFlags_Hook(void* pThis) {
    if (g_forceNoAutoHide) {
        return 0;
    }

    return TrayUI_GetAutoHideFlags_Original(pThis);
}

using CSecondaryTray_GetMonitor_t = HMONITOR(WINAPI*)(void* pThis);
CSecondaryTray_GetMonitor_t CSecondaryTray_GetMonitor_Original;

// Monitor of a CSecondaryTray from the `this` of any of its methods.
// GetMonitor is virtual and expects the pointer to the subobject that declares
// it. Non-virtual methods (_LoadSettings, _Unhide) get the start of the object,
// which on Windows 11 26300 is 0x20 bytes before it (GetMonitor reads
// [this+0x28]; _Unhide reads the same field at [this+0x48]), and calling
// GetMonitor with that pointer returns garbage. Both candidates are tried and
// the one that is a real monitor handle is used.
HMONITOR GetSecondaryTrayMonitor(void* pThis) {
    for (size_t adjust : {0, 0x20}) {
        HMONITOR monitor = CSecondaryTray_GetMonitor_Original(
            reinterpret_cast<BYTE*>(pThis) + adjust);
        MONITORINFO mi = {sizeof(mi)};
        if (monitor && GetMonitorInfo(monitor, &mi)) {
            return monitor;
        }
    }

    return nullptr;
}

using CSecondaryTray__LoadSettings_t = HRESULT(WINAPI*)(void* pThis);
CSecondaryTray__LoadSettings_t CSecondaryTray__LoadSettings_Original;
HRESULT WINAPI CSecondaryTray__LoadSettings_Hook(void* pThis) {
    if (KeepTaskbarShownOnMonitor(GetSecondaryTrayMonitor(pThis))) {
        g_forceNoAutoHide = true;
    }

    HRESULT ret = CSecondaryTray__LoadSettings_Original(pThis);

    g_forceNoAutoHide = false;

    return ret;
}

using CSecondaryTray_CheckSize_t = void(WINAPI*)(void* pThis, int param1);
CSecondaryTray_CheckSize_t CSecondaryTray_CheckSize_Original;
void WINAPI CSecondaryTray_CheckSize_Hook(void* pThis, int param1) {
    if (KeepTaskbarShownOnMonitor(CSecondaryTray_GetMonitor_Original(pThis))) {
        g_forceNoAutoHide = true;
    }

    CSecondaryTray_CheckSize_Original(pThis, param1);

    g_forceNoAutoHide = false;
}

// Secondary taskbar on a hidden monitor: never unhide.
using CSecondaryTray__Unhide_t = void(WINAPI*)(void* pThis,
                                               int flags,
                                               int request);
CSecondaryTray__Unhide_t CSecondaryTray__Unhide_Original;
void WINAPI CSecondaryTray__Unhide_Hook(void* pThis, int flags, int request) {
    if (KeepTaskbarHiddenOnMonitor(GetSecondaryTrayMonitor(pThis))) {
        Wh_Log(L"Blocked CSecondaryTray::_Unhide flags=%d request=%d", flags,
               request);
        return;
    }

    CSecondaryTray__Unhide_Original(pThis, flags, request);
}

// Polled while a secondary taskbar is shown (like TrayUI::_ShouldAutoHideTray
// for the primary one).
using CSecondaryTray_ShouldStayExpanded_t = bool(WINAPI*)(void* pThis);
CSecondaryTray_ShouldStayExpanded_t CSecondaryTray_ShouldStayExpanded_Original;
bool WINAPI CSecondaryTray_ShouldStayExpanded_Hook(void* pThis) {
    bool ret = CSecondaryTray_ShouldStayExpanded_Original(pThis);

    if (ret && KeepTaskbarHiddenOnMonitor(GetSecondaryTrayMonitor(pThis))) {
        Wh_Log(L"Forcing CSecondaryTray::ShouldStayExpanded -> false");
        return false;
    }

    return ret;
}

using TaskbarHost_Start_System_t = void(WINAPI*)(void* pThis);
TaskbarHost_Start_System_t TaskbarHost_Start_System_Original;
void WINAPI TaskbarHost_Start_System_Hook(void* pThis) {
    Wh_Log(L">");

    // Without this, taskbars that must stay visible start stuck hidden. The
    // hidden ones start shown as a side effect; ScheduleApply hides them.
    g_inTaskbarStart = true;
    g_forceNoAutoHide = true;

    TaskbarHost_Start_System_Original(pThis);

    g_forceNoAutoHide = false;
    g_inTaskbarStart = false;

    ScheduleApply();
}

using TaskbarHost_Start_t = void(WINAPI*)(void* pThis);
TaskbarHost_Start_t TaskbarHost_Start_Original;
void WINAPI TaskbarHost_Start_Hook(void* pThis) {
    Wh_Log(L">");

    // Newer builds: TaskbarHost::Start_System does the work.
    if (TaskbarHost_Start_System_Original) {
        TaskbarHost_Start_Original(pThis);
        return;
    }

    g_inTaskbarStart = true;
    g_forceNoAutoHide = true;

    TaskbarHost_Start_Original(pThis);

    g_forceNoAutoHide = false;
    g_inTaskbarStart = false;

    ScheduleApply();
}

using CTray_RecomputeWorkArea_t = int(WINAPI*)(void* pThis,
                                               HMONITOR monitor,
                                               RECT* rect);
CTray_RecomputeWorkArea_t CTray_RecomputeWorkArea_Original;
int WINAPI CTray_RecomputeWorkArea_Hook(void* pThis,
                                        HMONITOR monitor,
                                        RECT* rect) {
    if (KeepTaskbarShownOnMonitor(monitor)) {
        g_forceNoAutoHide = true;
    }

    int ret = CTray_RecomputeWorkArea_Original(pThis, monitor, rect);

    g_forceNoAutoHide = false;

    return ret;
}

using CTray_AppBarSetAutoHideBar_t = BOOL(WINAPI*)(void* pThis,
                                                   HWND hWnd,
                                                   BOOL autoHide,
                                                   UINT edge,
                                                   HMONITOR monitor);
CTray_AppBarSetAutoHideBar_t CTray_AppBarSetAutoHideBar_Original;
BOOL WINAPI CTray_AppBarSetAutoHideBar_Hook(void* pThis,
                                            HWND hWnd,
                                            BOOL autoHide,
                                            UINT edge,
                                            HMONITOR monitor) {
    if (KeepTaskbarShownOnMonitor(monitor)) {
        autoHide = FALSE;
    }

    return CTray_AppBarSetAutoHideBar_Original(pThis, hWnd, autoHide, edge,
                                               monitor);
}

using CTray__OnAppBarMessage_t = UINT_PTR(WINAPI*)(void* pThis,
                                                   COPYDATASTRUCT* cds);
CTray__OnAppBarMessage_t CTray__OnAppBarMessage_Original;
UINT_PTR WINAPI CTray__OnAppBarMessage_Hook(void* pThis, COPYDATASTRUCT* cds) {
    if (g_forceNoAutoHide) {
        return CTray__OnAppBarMessage_Original(pThis, cds);
    }

    if (PrimaryTaskbarShown()) {
        g_forceNoAutoHide = true;
    }

    UINT_PTR ret = CTray__OnAppBarMessage_Original(pThis, cds);

    g_forceNoAutoHide = false;

    return ret;
}

using ViewCoordinator_ShouldTaskbarBeExpanded_t =
    bool(WINAPI*)(void* pThis, HWND hMMTaskbarWnd, bool expanded);
ViewCoordinator_ShouldTaskbarBeExpanded_t
    ViewCoordinator_ShouldTaskbarBeExpanded_Original;
bool WINAPI ViewCoordinator_ShouldTaskbarBeExpanded_Hook(void* pThis,
                                                         HWND hMMTaskbarWnd,
                                                         bool expanded) {
    if (KeepTaskbarShownOnMonitor(
            MonitorFromWindow(hMMTaskbarWnd, MONITOR_DEFAULTTONEAREST))) {
        return true;
    }

    return ViewCoordinator_ShouldTaskbarBeExpanded_Original(
        pThis, hMMTaskbarWnd, expanded);
}

////////////////////////////////////////////////////////////////////////////////
// Hook setup.

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded(unsigned __int64,bool))"},
            &ViewCoordinator_ShouldTaskbarBeExpanded_Original,
            ViewCoordinator_ShouldTaskbarBeExpanded_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, symbolHooks,
                                    ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed (taskbar view module)");
        return false;
    }

    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

bool HookTaskbarSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        // Primary taskbar.
        {
            {LR"(public: virtual void __cdecl TrayUI::Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            &TrayUI_Unhide_Original,
            TrayUI_Unhide_Hook,
        },
        {
            {LR"(public: void __cdecl TrayUI::_UnhideNow(bool))"},
            &TrayUI__UnhideNow_Original,
            TrayUI__UnhideNow_Hook,
            true,
        },
        {
            {
                LR"(public: bool __cdecl TrayUI::_ShouldAutoHideTray(class AutoHideTelemetry::TaskbarAutoHideTelemetry::AutoHideOnTaskbarHide))",
                LR"(public: bool __cdecl TrayUI::_ShouldAutoHideTray(void))",
            },
            &TrayUI__ShouldAutoHideTray_Original,
            TrayUI__ShouldAutoHideTray_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl TrayUI::_Hide(void))"},
            &TrayUI__Hide_Original,
            TrayUI__Hide_Hook,
        },
        {
            {LR"(public: virtual unsigned int __cdecl TrayUI::GetAutoHideFlags(void))"},
            &TrayUI_GetAutoHideFlags_Original,
            TrayUI_GetAutoHideFlags_Hook,
        },
        // Secondary taskbars.
        {
            {LR"(public: virtual struct HMONITOR__ * __cdecl CSecondaryTray::GetMonitor(void))"},
            &CSecondaryTray_GetMonitor_Original,
        },
        {
            {LR"(private: long __cdecl CSecondaryTray::_LoadSettings(void))"},
            &CSecondaryTray__LoadSettings_Original,
            CSecondaryTray__LoadSettings_Hook,
        },
        {
            {LR"(public: virtual void __cdecl CSecondaryTray::CheckSize(int))"},
            &CSecondaryTray_CheckSize_Original,
            CSecondaryTray_CheckSize_Hook,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            &CSecondaryTray__Unhide_Original,
            CSecondaryTray__Unhide_Hook,
            true,
        },
        {
            {LR"(public: virtual bool __cdecl CSecondaryTray::ShouldStayExpanded(void))"},
            &CSecondaryTray_ShouldStayExpanded_Original,
            CSecondaryTray_ShouldStayExpanded_Hook,
            true,
        },
        // Startup.
        {
            {LR"(public: void __cdecl TaskbarHost::Start_System(void))"},
            &TaskbarHost_Start_System_Original,
            TaskbarHost_Start_System_Hook,
            true,  // From 10.0.26100.5074
        },
        {
            {LR"(public: void __cdecl TaskbarHost::Start(void))"},
            &TaskbarHost_Start_Original,
            TaskbarHost_Start_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                    ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed (taskbar.dll)");
        return false;
    }

    return true;
}

bool HookExplorerExeSymbols() {
    HMODULE module = GetModuleHandle(nullptr);

    WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
        {
            {LR"(private: int __cdecl CTray::RecomputeWorkArea(struct HMONITOR__ *,struct tagRECT *))"},
            &CTray_RecomputeWorkArea_Original,
            CTray_RecomputeWorkArea_Hook,
        },
        {
            {LR"(public: virtual int __cdecl CTray::AppBarSetAutoHideBar(struct HWND__ *,int,unsigned int,struct HMONITOR__ *))"},
            &CTray_AppBarSetAutoHideBar_Original,
            CTray_AppBarSetAutoHideBar_Hook,
        },
        {
            {LR"(protected: __int64 __cdecl CTray::_OnAppBarMessage(struct tagCOPYDATASTRUCT *))"},
            &CTray__OnAppBarMessage_Original,
            CTray__OnAppBarMessage_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, explorerExeHooks,
                                    ARRAYSIZE(explorerExeHooks))) {
        Wh_Log(L"HookSymbols failed (explorer.exe)");
        return false;
    }

    return true;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }

    if (!HookTaskbarSymbols()) {
        return FALSE;
    }

    if (!HookExplorerExeSymbols()) {
        return FALSE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        HookTaskbarViewDllSymbols(taskbarViewModule);
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                   LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);

    WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook,
                                   &SetWindowPos_Original);
    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook,
                                   &ShowWindow_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got taskbar view module");
                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    if (FindCurrentProcessTaskbarWnd()) {
        // Mod enabled while the taskbar is running.
        ApplyAutoHideState();
    } else {
        // Explorer is starting: apply once the taskbar exists.
        ScheduleApply();
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
    ApplyAutoHideState();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    if (g_applyThread) {
        WaitForSingleObject(g_applyThread, INFINITE);
        CloseHandle(g_applyThread);
        g_applyThread = nullptr;
    }

    g_pause = true;
    RestoreAutoHideState();
    UnsubclassTaskbarWnds();

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}
