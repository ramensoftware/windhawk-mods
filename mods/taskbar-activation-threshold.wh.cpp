// ==WindhawkMod==
// @id              taskbar-activation-threshold
// @name            Taskbar Activation Threshold
// @description     Reveal the auto-hidden Windows 11 taskbar from a configurable bottom activation band without moving the cursor
// @version         1.0.0
// @author          themagnificentoofman
// @github          https://github.com/themagnificentoofman
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshell32 -lshcore -ldwmapi
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Activation Threshold

Reveal the auto-hidden Windows 11 taskbar from a configurable activation band
above the bottom edge of the screen.

The cursor stays stationary. The mod tracks cursor movement, asks Explorer to
keep the target taskbar expanded while the pointer remains in the configured
region, and uses Explorer's native taskbar show/hide paths on the taskbar UI
thread.

## Demonstration

![Taskbar Activation Threshold demonstration](https://raw.githubusercontent.com/themagnificentoofman/taskbar-activation-threshold-assets/main/taskbar-activation-threshold-demo.gif)

## Features

- Configurable activation-band height and hover delay
- Per-monitor DPI scaling
- Multi-monitor and secondary-taskbar support
- Adjustable release delay
- Optional fullscreen/presentation suppression
- Optional suppression while a mouse button is held
- Optional primary-monitor-only mode
- No cursor injection, taskbar layout changes, synthetic pointer-over state, or
  taskbar Z-order forcing

## Behavior

The activation threshold controls where the taskbar can be revealed. After the
hover delay, the mod marks that taskbar as "keep shown", calls Explorer's native
unhide path, and asks `ViewCoordinator` to re-evaluate expansion. Explorer's own
pointer-over notifications continue normally; the mod doesn't swallow or forge
them.

The keep-shown policy remains active while the pointer is inside either the
activation band or the visible taskbar. After the pointer leaves both, the
release delay runs. The mod then removes its keep-shown policy and asks Explorer
to resume its normal auto-hide path. Because the state lives only in this mod's
hooks, disabling or unloading the mod can't leave Explorer with a synthetic
pointer-over state latched.

The default band is 12 DIPs. With a typical Windows 11 taskbar this is shorter
than the taskbar itself, so the threshold mainly changes the trigger area; once
revealed, the taskbar's own rectangle keeps it available while you use its
buttons. Values larger than the taskbar height also create a hold-open strip
above the visible taskbar and are intentionally more aggressive.

Moving directly from one monitor's activation band to another releases the old
monitor immediately and starts the new monitor's hover delay instead of stacking
the old release delay with the new hover delay.

## Compatibility

The mod is intended for the standard bottom-positioned Windows 11 taskbar. It
doesn't modify taskbar XAML, height, icon size, animation duration,
transparency, the Windows auto-hide setting, or Z-order.

Taskbar Z-order remains Explorer's responsibility. Reveals go through the native
`TrayUI::Unhide` / `CSecondaryTray::_Unhide` path before the expansion
re-evaluation, but the mod never calls `SetWindowPos` or forces `HWND_TOPMOST`.
This preserves Explorer's normal layering behavior and avoids fighting
always-on-top applications or Z-order customization mods.

**Taskbar auto-hide custom activation area** controls where along the taskbar a
native edge trigger is accepted, while this mod adds trigger depth perpendicular
to the bottom edge. These are closely related catalog features and may be better
consolidated if the maintainer prefers. This implementation no longer hooks or
suppresses `HandleIsPointerOverTaskbarFrameChanged`, so it doesn't hide
pointer-leave notifications from that mod or from **Taskbar auto-hide fine
tuning**. Mods that intentionally replace Explorer's hide/show policy can still
conflict with the keep-shown hooks used here. Release uses Explorer's native
hide timer and expansion reevaluation, so mods that intercept the hide timer or
`TrayUI::_Hide` / `CSecondaryTray::_AutoHide` can alter release behavior.

Mods that deliberately disable mouse-triggered taskbar reveals remain
incompatible with the purpose of this mod.

## Requirements

- Windows 11
- Standard Windows 11 taskbar
- Bottom-positioned taskbar
- **Automatically hide the taskbar** enabled, unless **Require Windows
  auto-hide** is disabled

## Known limitations

- Top-, left-, right-, detached-, floating-, and arbitrary-position taskbars
  aren't supported. Bottom placement is inferred from the live taskbar window
  rectangle; mods that substantially reposition or resize the taskbar can make
  the geometry check reject it.
- ExplorerPatcher's legacy Windows 10 taskbar isn't supported.
- The Windows `ABM_GETSTATE` auto-hide flag is system-wide. With per-monitor
  auto-hide mods, **Require Windows auto-hide** can't determine the auto-hide
  state of an individual monitor.
- Reveal depends on undocumented Windows 11 taskbar symbols, including
  `ViewCoordinator::ShouldTaskbarBeExpanded`, `ViewCoordinator::UpdateIsExpanded`,
  `TrayUI::_Hide` / `TrayUI::Unhide`, and the corresponding secondary-taskbar
  paths. A Windows update can require symbol updates. Secondary-taskbar symbols
  are optional so a secondary symbol change doesn't disable the primary taskbar.
- Fullscreen suppression checks Windows presentation/exclusive-fullscreen state
  and the topmost eligible application window on the target monitor. Windows
  presentation mode is system-wide. Background fullscreen windows behind another
  normal app don't suppress the band. Click-through, tool, non-activating,
  cloaked, desktop, and taskbar windows are ignored. A borderless window that
  covers the monitor is intentionally treated as fullscreen even if the app
  describes that state as maximized. The top-window verdict is cached for up to
  one second to keep fullscreen detection off the cursor hot path. Suppression is
  a pre-reveal gate; it doesn't continuously force-hide a taskbar if an app
  becomes fullscreen after the reveal.
- Cursor tracking uses a system-wide `EVENT_OBJECT_LOCATIONCHANGE` WinEvent hook
  because Windows can't filter it to `OBJID_CURSOR` at registration time. The OS
  still delivers other location-change events to the hook; the callback discards
  them immediately. If the hook can't be installed, a 150 ms cursor-check timer
  is used instead.
- Transient reveal blockers such as a held mouse button, fullscreen app, or a
  taskbar coordinator that hasn't been observed yet are retried at most once per
  second while the pointer remains parked in the band. Settings-only conditions
  and monitors without a supported taskbar are event-driven and aren't polled.

## Attribution and license

The Windows 11 taskbar symbols and keep-shown pattern were informed by
GPL-3.0-licensed Windhawk taskbar mods by m417z, especially **Taskbar auto-hide
when maximized**, **Taskbar auto-hide fine tuning**, and **Taskbar auto-hide
custom activation area**.

This mod is distributed under the GNU General Public License v3.0.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- activationThresholdPx: 12
  $name: Activation threshold
  $description: >-
    Height of the activation band in logical pixels at 96 DPI, scaled for each
    monitor. 8-24 is a practical range for most taskbars. Larger values are more
    aggressive and, once taller than the taskbar, also create a hold-open strip
    above it.
- activationDelayMs: 75
  $name: Hover delay
  $description: >-
    Time the pointer must remain in the activation band before revealing the
    taskbar. A short delay helps avoid accidental reveals when crossing between
    vertically stacked monitors.
- releaseDelayMs: 120
  $name: Release delay
  $description: >-
    Delay before Explorer is allowed to hide the taskbar after the pointer leaves
    both the activation band and the visible taskbar.
- requireWindowsAutoHide: true
  $name: Require Windows auto-hide
  $description: >-
    Only activate when Windows' system-wide auto-hide flag is enabled. Disable
    this if another mod manages auto-hide independently; per-monitor auto-hide
    states can't be distinguished by this Windows flag.
- ignoreFullscreenApps: true
  $name: Ignore fullscreen apps
  $description: >-
    Don't reveal during Windows presentation/exclusive-fullscreen state or when
    the topmost eligible app on the target monitor is fullscreen. Background
    fullscreen apps behind another normal app don't suppress activation.
- ignoreWhileMouseButtonDown: true
  $name: Ignore while dragging
  $description: >-
    Don't activate while a mouse button is held.
- primaryMonitorOnly: false
  $name: Primary monitor only
  $description: >-
    Only use the activation band on the primary monitor.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <dwmapi.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cwchar>
#include <mutex>
#include <unordered_map>

struct Settings {
    std::atomic<int> activationThresholdPx{12};
    std::atomic<int> activationDelayMs{75};
    std::atomic<int> releaseDelayMs{120};
    std::atomic<bool> requireWindowsAutoHide{true};
    std::atomic<bool> ignoreFullscreenApps{true};
    std::atomic<bool> ignoreWhileMouseButtonDown{true};
    std::atomic<bool> primaryMonitorOnly{false};
};

Settings g_settings;

std::atomic<bool> g_taskbarViewDllLoaded{false};
std::atomic<bool> g_viewCoordinatorSupportAvailable{false};
std::atomic<DWORD> g_workerThreadId{0};
std::atomic<bool> g_cursorUpdatePosted{false};
std::atomic<HWND> g_mainTaskbarWindow{nullptr};
std::atomic<bool> g_secondaryTaskbarSupportAvailable{false};
std::atomic<HWND> g_desiredKeptShownTaskbar{nullptr};
std::atomic<HWND> g_appliedKeptShownTaskbar{nullptr};

HANDLE g_stopEvent = nullptr;
HANDLE g_workerThread = nullptr;  // Guarded by g_workerThreadMutex.
std::mutex g_workerThreadMutex;
std::atomic<bool> g_unloading{false};

// Registered in Wh_ModInit rather than during DLL initialization, avoiding
// user32 calls while the loader lock is held.
UINT g_updateTaskbarStateMessage = 0;

constexpr UINT kWorkerCursorChangedMessage = WM_APP + 1;
constexpr UINT kTransientRetryIntervalMs = 1000;
constexpr UINT_PTR kTrayUITimerHide = 2;
constexpr ULONGLONG kFullscreenCacheDurationMs = 1000;

// These containers are accessed from Explorer's taskbar UI thread by the
// hooked taskbar methods and the registered taskbar-state message.
std::unordered_map<HWND, void*> g_trayUiWndProcObjects;
std::unordered_map<HWND, void*> g_secondaryTrayObjects;
std::unordered_map<HWND, void*> g_viewCoordinators;
std::unordered_map<void*, HWND> g_taskbarsKeptShown;

void* g_trayUiVtableIInspectable = nullptr;
void* g_trayUiVtableITrayComponentHost = nullptr;

bool EraseKeptShownForWindow(HWND taskbarWindow) {
    bool erased = false;
    for (auto it = g_taskbarsKeptShown.begin();
         it != g_taskbarsKeptShown.end();) {
        if (it->second == taskbarWindow) {
            it = g_taskbarsKeptShown.erase(it);
            erased = true;
        } else {
            ++it;
        }
    }
    return erased;
}

void ForgetTaskbarState(HWND taskbarWindow) {
    g_viewCoordinators.erase(taskbarWindow);
    EraseKeptShownForWindow(taskbarWindow);

    HWND expected = taskbarWindow;
    g_desiredKeptShownTaskbar.compare_exchange_strong(
        expected, nullptr, std::memory_order_acq_rel);
    expected = taskbarWindow;
    g_appliedKeptShownTaskbar.compare_exchange_strong(
        expected, nullptr, std::memory_order_acq_rel);
}

DWORD WINAPI ActivationWorkerThread(LPVOID);
void ApplyTaskbarKeepShownStateOnUiThread(HWND taskbarWindow);

void LoadSettings() {
    g_settings.activationThresholdPx.store(
        std::clamp(Wh_GetIntSetting(L"activationThresholdPx"), 1, 500),
        std::memory_order_relaxed);

    g_settings.activationDelayMs.store(
        std::clamp(Wh_GetIntSetting(L"activationDelayMs"), 0, 1000),
        std::memory_order_relaxed);

    g_settings.releaseDelayMs.store(
        std::clamp(Wh_GetIntSetting(L"releaseDelayMs"), 0, 5000),
        std::memory_order_relaxed);

    g_settings.requireWindowsAutoHide.store(
        Wh_GetIntSetting(L"requireWindowsAutoHide") != 0,
        std::memory_order_relaxed);

    g_settings.ignoreFullscreenApps.store(
        Wh_GetIntSetting(L"ignoreFullscreenApps") != 0,
        std::memory_order_relaxed);

    g_settings.ignoreWhileMouseButtonDown.store(
        Wh_GetIntSetting(L"ignoreWhileMouseButtonDown") != 0,
        std::memory_order_relaxed);

    g_settings.primaryMonitorOnly.store(
        Wh_GetIntSetting(L"primaryMonitorOnly") != 0,
        std::memory_order_relaxed);
}

void EnsureActivationWorker() {
    if (g_workerThreadId.load(std::memory_order_acquire)) {
        return;
    }

    if (g_unloading.load(std::memory_order_acquire) || !g_stopEvent) {
        return;
    }

    std::lock_guard<std::mutex> guard(g_workerThreadMutex);
    if (g_unloading.load(std::memory_order_relaxed)) {
        return;
    }

    if (g_workerThread) {
        const DWORD state = WaitForSingleObject(g_workerThread, 0);
        if (state == WAIT_TIMEOUT) {
            return;
        }
        if (state == WAIT_OBJECT_0) {
            CloseHandle(g_workerThread);
            g_workerThread = nullptr;
            Wh_Log(L"Restarting activation worker after prior thread exit");
        } else {
            Wh_Log(L"Couldn't query activation worker state: %u",
                   GetLastError());
            return;
        }
    }

    g_workerThread = CreateThread(
        nullptr, 0, ActivationWorkerThread, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        return;
    }

    Wh_Log(L"Started activation worker in the taskbar Explorer process");
}

void QueuePointerStateUpdate() {
    const DWORD workerThreadId =
        g_workerThreadId.load(std::memory_order_acquire);
    if (!workerThreadId ||
        g_cursorUpdatePosted.exchange(true, std::memory_order_acq_rel)) {
        return;
    }

    if (!PostThreadMessageW(workerThreadId,
                            kWorkerCursorChangedMessage,
                            0,
                            0)) {
        g_cursorUpdatePosted.store(false, std::memory_order_release);
        Wh_Log(L"Couldn't queue pointer-state update: %u", GetLastError());
    }
}

enum class TaskbarKind {
    kNone,
    kMain,
    kSecondary,
};

TaskbarKind GetTaskbarKind(HWND window) {
    if (!window) {
        return TaskbarKind::kNone;
    }

    wchar_t className[64];
    if (!GetClassNameW(window, className, ARRAYSIZE(className))) {
        return TaskbarKind::kNone;
    }

    if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
        return TaskbarKind::kMain;
    }
    if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        return TaskbarKind::kSecondary;
    }
    return TaskbarKind::kNone;
}

bool IsSupportedTaskbarWindow(HWND window) {
    const TaskbarKind kind = GetTaskbarKind(window);
    return kind == TaskbarKind::kMain ||
           (kind == TaskbarKind::kSecondary &&
            g_secondaryTaskbarSupportAvailable.load(std::memory_order_acquire));
}

bool IsCurrentProcessWindow(HWND window) {
    if (!window) {
        return false;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    return processId == GetCurrentProcessId();
}

bool IsMainTaskbarWindow(HWND window) {
    return IsCurrentProcessWindow(window) &&
           GetTaskbarKind(window) == TaskbarKind::kMain;
}

HWND FindMainTaskbarWindow() {
    HWND cached = g_mainTaskbarWindow.load(std::memory_order_relaxed);
    if (IsMainTaskbarWindow(cached)) {
        return cached;
    }

    g_mainTaskbarWindow.store(nullptr, std::memory_order_relaxed);

    HWND taskbarWindow = nullptr;
    EnumWindows(
        [](HWND window, LPARAM parameter) -> BOOL {
            if (IsMainTaskbarWindow(window)) {
                *reinterpret_cast<HWND*>(parameter) = window;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&taskbarWindow));

    if (taskbarWindow) {
        g_mainTaskbarWindow.store(taskbarWindow, std::memory_order_relaxed);
    }
    return taskbarWindow;
}

HMONITOR GetTaskbarMonitor(HWND taskbarWindow) {
    if (HMONITOR monitor = reinterpret_cast<HMONITOR>(
            GetPropW(taskbarWindow, L"TaskbarMonitor"))) {
        MONITORINFO monitorInfo{};
        monitorInfo.cbSize = sizeof(monitorInfo);
        if (GetMonitorInfoW(monitor, &monitorInfo)) {
            return monitor;
        }
    }

    // A display-topology transition can briefly leave TaskbarMonitor pointing
    // at a retired HMONITOR. Fall back to the window's current nearest monitor.
    return MonitorFromWindow(taskbarWindow, MONITOR_DEFAULTTONEAREST);
}

bool LooksLikeHorizontalTaskbarRect(
    const RECT& taskbarRect,
    const MONITORINFO& monitorInfo) {
    const int monitorWidth =
        monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
    const int monitorHeight =
        monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
    const int taskbarWidth = taskbarRect.right - taskbarRect.left;
    const int taskbarHeight = taskbarRect.bottom - taskbarRect.top;

    return taskbarWidth >= monitorWidth / 2 && taskbarHeight > 0 &&
           taskbarHeight <= monitorHeight / 2;
}

bool IsBottomPositionedTaskbar(HWND taskbarWindow, HMONITOR monitor) {
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return false;
    }

    // The auto-hidden taskbar remains a full-width horizontal window whose
    // live rectangle sits at or just below the configured monitor edge. Avoid
    // an extra undocumented symbol and cross-thread query for this check.
    RECT taskbarRect{};
    if (!GetWindowRect(taskbarWindow, &taskbarRect) ||
        !LooksLikeHorizontalTaskbarRect(taskbarRect, monitorInfo)) {
        return false;
    }

    // A visible bottom taskbar ends at the monitor bottom; an auto-hidden one
    // straddles that edge with only a very small strip left onscreen. Scale a
    // two-DIP tolerance for high-DPI monitors while keeping the test tight
    // enough to reject detached taskbars.
    const UINT dpi = GetDpiForWindow(taskbarWindow);
    const int edgeTolerance =
        std::max(2, MulDiv(2, dpi ? dpi : 96, 96));
    return taskbarRect.top <= monitorInfo.rcMonitor.bottom + edgeTolerance &&
           taskbarRect.bottom >= monitorInfo.rcMonitor.bottom - edgeTolerance;
}

struct FindTaskbarContext {
    HMONITOR monitor;
    HWND result;
};

BOOL CALLBACK FindTaskbarForMonitorCallback(HWND window, LPARAM parameter) {
    if (!IsSupportedTaskbarWindow(window)) {
        return TRUE;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    if (processId != GetCurrentProcessId()) {
        return TRUE;
    }

    auto* context = reinterpret_cast<FindTaskbarContext*>(parameter);
    HMONITOR taskbarMonitor = GetTaskbarMonitor(window);

    if (taskbarMonitor == context->monitor &&
        IsBottomPositionedTaskbar(window, taskbarMonitor)) {
        context->result = window;
        return FALSE;
    }

    return TRUE;
}

HWND FindTaskbarForMonitor(HMONITOR monitor) {
    HWND mainTaskbar = FindMainTaskbarWindow();
    if (mainTaskbar &&
        GetTaskbarMonitor(mainTaskbar) == monitor &&
        IsBottomPositionedTaskbar(mainTaskbar, monitor)) {
        return mainTaskbar;
    }

    FindTaskbarContext context{
        .monitor = monitor,
        .result = nullptr,
    };

    EnumWindows(FindTaskbarForMonitorCallback,
                reinterpret_cast<LPARAM>(&context));

    return context.result;
}

bool IsWindowsAutoHideEnabled() {
    APPBARDATA appBarData{};
    appBarData.cbSize = sizeof(appBarData);
    return (SHAppBarMessage(ABM_GETSTATE, &appBarData) & ABS_AUTOHIDE) != 0;
}

bool IsAnyMouseButtonDown() {
    constexpr int mouseButtons[] = {
        VK_LBUTTON,
        VK_RBUTTON,
        VK_MBUTTON,
        VK_XBUTTON1,
        VK_XBUTTON2,
    };

    for (int virtualKey : mouseButtons) {
        if ((GetAsyncKeyState(virtualKey) & 0x8000) != 0) {
            return true;
        }
    }

    return false;
}

bool HasDesktopWindowClass(HWND window) {
    wchar_t className[64];
    if (!window || !GetClassNameW(window, className, ARRAYSIZE(className))) {
        return false;
    }

    return _wcsicmp(className, L"Progman") == 0 ||
           _wcsicmp(className, L"WorkerW") == 0;
}

bool IsShellDesktopWindow(HWND window) {
    return window && IsWindowVisible(window) &&
           (window == GetShellWindow() || GetPropW(window, L"DesktopWindow"));
}

bool IsWindowCloaked(HWND window) {
    BOOL cloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(
               window, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
           cloaked;
}

bool DoesClientAreaCoverMonitor(HWND window,
                                const MONITORINFO& monitorInfo) {
    RECT clientRect{};
    if (!GetClientRect(window, &clientRect)) {
        return false;
    }

    POINT points[2] = {
        {clientRect.left, clientRect.top},
        {clientRect.right, clientRect.bottom},
    };
    SetLastError(ERROR_SUCCESS);
    if (MapWindowPoints(window, nullptr, points, ARRAYSIZE(points)) == 0 &&
        GetLastError() != ERROR_SUCCESS) {
        return false;
    }

    constexpr int tolerance = 2;
    return points[0].x <= monitorInfo.rcMonitor.left + tolerance &&
           points[0].y <= monitorInfo.rcMonitor.top + tolerance &&
           points[1].x >= monitorInfo.rcMonitor.right - tolerance &&
           points[1].y >= monitorInfo.rcMonitor.bottom - tolerance;
}

bool IsFullscreenTopWindow(HWND window,
                           const RECT& windowRect,
                           const MONITORINFO& monitorInfo) {
    constexpr int tolerance = 2;
    if (windowRect.left > monitorInfo.rcMonitor.left + tolerance ||
        windowRect.top > monitorInfo.rcMonitor.top + tolerance ||
        windowRect.right < monitorInfo.rcMonitor.right - tolerance ||
        windowRect.bottom < monitorInfo.rcMonitor.bottom - tolerance) {
        return false;
    }

    const LONG_PTR style = GetWindowLongPtrW(window, GWL_STYLE);
    if ((style & (WS_CAPTION | WS_THICKFRAME)) == 0) {
        return true;
    }

    if (IsZoomed(window)) {
        return false;
    }

    return DoesClientAreaCoverMonitor(window, monitorInfo);
}

bool IsFullscreenOrPresentationNotificationState(HMONITOR monitor) {
    QUERY_USER_NOTIFICATION_STATE state = QUNS_ACCEPTS_NOTIFICATIONS;
    if (FAILED(SHQueryUserNotificationState(&state))) {
        return false;
    }

    if (state == QUNS_PRESENTATION_MODE) {
        return true;
    }

    if (state != QUNS_RUNNING_D3D_FULL_SCREEN) {
        return false;
    }

    HWND foregroundWindow = GetForegroundWindow();
    return foregroundWindow &&
           MonitorFromWindow(foregroundWindow,
                             MONITOR_DEFAULTTONEAREST) == monitor;
}

bool IsFullscreenWindowOnMonitor(HMONITOR monitor) {
    if (IsFullscreenOrPresentationNotificationState(monitor)) {
        return true;
    }

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return false;
    }

    struct Context {
        MONITORINFO monitorInfo;
        bool fullscreen = false;
    } context{monitorInfo};

    EnumWindows(
        [](HWND window, LPARAM parameter) -> BOOL {
            auto* context = reinterpret_cast<Context*>(parameter);

            if (IsShellDesktopWindow(window)) {
                return FALSE;
            }
            if (!IsWindowVisible(window) || IsIconic(window) ||
                HasDesktopWindowClass(window) ||
                GetTaskbarKind(window) != TaskbarKind::kNone) {
                return TRUE;
            }

            const LONG_PTR style = GetWindowLongPtrW(window, GWL_STYLE);
            const LONG_PTR exStyle = GetWindowLongPtrW(window, GWL_EXSTYLE);
            if ((style & WS_CHILD) != 0 ||
                (exStyle & (WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW |
                            WS_EX_NOACTIVATE)) != 0) {
                return TRUE;
            }

            RECT windowRect{};
            RECT intersection{};
            if (!GetWindowRect(window, &windowRect) ||
                !IntersectRect(&intersection,
                               &windowRect,
                               &context->monitorInfo.rcMonitor) ||
                IsWindowCloaked(window)) {
                return TRUE;
            }

            // This is the topmost eligible app intersecting the target monitor.
            // Don't continue into background windows after making this decision.
            context->fullscreen = IsFullscreenTopWindow(
                window, windowRect, context->monitorInfo);
            if (context->fullscreen) {
                DWORD processId = 0;
                GetWindowThreadProcessId(window, &processId);
                Wh_Log(L"Fullscreen top window: hwnd=%p pid=%u",
                       window,
                       processId);
            }
            return FALSE;
        },
        reinterpret_cast<LPARAM>(&context));

    return context.fullscreen;
}

bool IsPointInsideActivationBand(const POINT& pointer,
                                 const MONITORINFO& monitorInfo,
                                 UINT dpiY) {
    const int monitorHeight =
        monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

    const int logicalThreshold =
        g_settings.activationThresholdPx.load(std::memory_order_relaxed);
    const int threshold =
        std::min(MulDiv(logicalThreshold, dpiY, 96), monitorHeight);

    return pointer.x >= monitorInfo.rcMonitor.left &&
           pointer.x < monitorInfo.rcMonitor.right &&
           pointer.y >= monitorInfo.rcMonitor.bottom - threshold &&
           pointer.y < monitorInfo.rcMonitor.bottom;
}

bool IsPointOverTaskbar(HWND taskbarWindow, const POINT& pointer) {
    RECT taskbarRect{};
    return GetWindowRect(taskbarWindow, &taskbarRect) &&
           PtInRect(&taskbarRect, pointer);
}

void* QueryViaVtableDirectional(void* object,
                                  void* targetVtable,
                                  int direction) {
    if (!object || !targetVtable || (direction != 1 && direction != -1)) {
        return nullptr;
    }

    MEMORY_BASIC_INFORMATION memoryInfo{};
    if (!VirtualQuery(object, &memoryInfo, sizeof(memoryInfo)) ||
        memoryInfo.State != MEM_COMMIT ||
        (memoryInfo.Protect & (PAGE_NOACCESS | PAGE_GUARD)) != 0) {
        return nullptr;
    }

    const auto regionStart =
        reinterpret_cast<uintptr_t>(memoryInfo.BaseAddress);
    const auto regionEnd = regionStart + memoryInfo.RegionSize;
    const auto objectAddress = reinterpret_cast<uintptr_t>(object);

    for (size_t i = 0; i < 20; i++) {
        const uintptr_t byteOffset = i * sizeof(void*);
        uintptr_t candidateAddress = 0;
        if (direction > 0) {
            if (objectAddress > UINTPTR_MAX - byteOffset) {
                break;
            }
            candidateAddress = objectAddress + byteOffset;
        } else {
            if (objectAddress < byteOffset) {
                break;
            }
            candidateAddress = objectAddress - byteOffset;
        }

        if (candidateAddress < regionStart ||
            candidateAddress > regionEnd - sizeof(void*)) {
            break;
        }

        auto** candidate = reinterpret_cast<void**>(candidateAddress);
        if (*candidate == targetVtable) {
            return candidate;
        }
    }

    return nullptr;
}

void* QueryViaVtable(void* object, void* targetVtable) {
    return QueryViaVtableDirectional(object, targetVtable, 1);
}

void* QueryViaVtableBackwards(void* object, void* targetVtable) {
    return QueryViaVtableDirectional(object, targetVtable, -1);
}

using TrayUI_Hide_t = void(WINAPI*)(void* trayUiInspectable);
TrayUI_Hide_t TrayUI_Hide_Original;

void WINAPI TrayUI_Hide_Hook(void* trayUiInspectable) {
    auto it = g_taskbarsKeptShown.find(trayUiInspectable);
    if (!g_unloading.load(std::memory_order_acquire) &&
        it != g_taskbarsKeptShown.end() &&
        g_desiredKeptShownTaskbar.load(std::memory_order_acquire) == it->second) {
        KillTimer(it->second, kTrayUITimerHide);
        return;
    }

    TrayUI_Hide_Original(trayUiInspectable);
}

using CSecondaryTray_AutoHide_t =
    void(WINAPI*)(void* secondaryTray, bool parameter);
CSecondaryTray_AutoHide_t CSecondaryTray_AutoHide_Original;

void WINAPI CSecondaryTray_AutoHide_Hook(void* secondaryTray,
                                         bool parameter) {
    auto it = g_taskbarsKeptShown.find(secondaryTray);
    if (!g_unloading.load(std::memory_order_acquire) &&
        it != g_taskbarsKeptShown.end() &&
        g_desiredKeptShownTaskbar.load(std::memory_order_acquire) == it->second) {
        KillTimer(it->second, kTrayUITimerHide);
        return;
    }

    CSecondaryTray_AutoHide_Original(secondaryTray, parameter);
}

using TrayUI_Unhide_t =
    void(WINAPI*)(void* trayComponentHost,
                  int trayUnhideFlags,
                  int unhideRequest);

TrayUI_Unhide_t TrayUI_Unhide_Original;

using CSecondaryTray_Unhide_t =
    void(WINAPI*)(void* secondaryTray,
                  int trayUnhideFlags,
                  int unhideRequest);

CSecondaryTray_Unhide_t CSecondaryTray_Unhide_Original;

using TrayUI_WndProc_t =
    LRESULT(WINAPI*)(void* trayUi,
                     HWND window,
                     UINT message,
                     WPARAM wParam,
                     LPARAM lParam,
                     bool* handled);

TrayUI_WndProc_t TrayUI_WndProc_Original;

void HandleTaskbarEnvironmentMessage(UINT message) {
    if (message == WM_DISPLAYCHANGE || message == WM_DPICHANGED ||
        message == WM_SETTINGCHANGE) {
        EnsureActivationWorker();
        QueuePointerStateUpdate();
    }
}

LRESULT WINAPI TrayUI_WndProc_Hook(
    void* trayUi,
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam,
    bool* handled) {
    if (message == g_updateTaskbarStateMessage) {
        // Record the existing taskbar object's pThis for free: this message is
        // already executing with the correct TrayUI instance on its UI thread.
        g_trayUiWndProcObjects[window] = trayUi;
        g_mainTaskbarWindow.store(window, std::memory_order_relaxed);

        ApplyTaskbarKeepShownStateOnUiThread(window);
        if (handled) {
            *handled = true;
        }
        return 0;
    }

    const bool taskbarCreating = message == WM_NCCREATE;
    if (taskbarCreating) {
        g_trayUiWndProcObjects[window] = trayUi;
        g_mainTaskbarWindow.store(window, std::memory_order_relaxed);
        Wh_Log(L"Captured TrayUI object for taskbar %p", window);
        EnsureActivationWorker();
    } else if (message == WM_NCDESTROY) {
        g_trayUiWndProcObjects.erase(window);
        ForgetTaskbarState(window);
        if (g_mainTaskbarWindow.load(std::memory_order_relaxed) == window) {
            g_mainTaskbarWindow.store(nullptr, std::memory_order_relaxed);
        }
    }

    const LRESULT originalResult = TrayUI_WndProc_Original(
        trayUi,
        window,
        message,
        wParam,
        lParam,
        handled);
    HandleTaskbarEnvironmentMessage(message);
    if (taskbarCreating || message == WM_NCDESTROY) {
        // Re-evaluate after Explorer has processed creation/destruction. The
        // worker message carries no HWND, so handle reuse can't make this stale.
        QueuePointerStateUpdate();
    }
    return originalResult;
}

using CSecondaryTray_WndProc_t =
    LRESULT(WINAPI*)(void* secondaryTray,
                     HWND window,
                     UINT message,
                     WPARAM wParam,
                     LPARAM lParam);

CSecondaryTray_WndProc_t CSecondaryTray_WndProc_Original;

LRESULT WINAPI CSecondaryTray_WndProc_Hook(
    void* secondaryTray,
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam) {
    if (message == g_updateTaskbarStateMessage) {
        // As with the primary taskbar, the first UI operation records the
        // existing CSecondaryTray object without a separate capture round-trip.
        g_secondaryTrayObjects[window] = secondaryTray;

        ApplyTaskbarKeepShownStateOnUiThread(window);
        return 0;
    }

    const bool taskbarCreating = message == WM_NCCREATE;
    if (taskbarCreating) {
        g_secondaryTrayObjects[window] = secondaryTray;
        Wh_Log(L"Captured CSecondaryTray object for taskbar %p", window);
        EnsureActivationWorker();
    } else if (message == WM_NCDESTROY) {
        g_secondaryTrayObjects.erase(window);
        ForgetTaskbarState(window);
    }

    const LRESULT originalResult = CSecondaryTray_WndProc_Original(
        secondaryTray,
        window,
        message,
        wParam,
        lParam);
    HandleTaskbarEnvironmentMessage(message);
    if (taskbarCreating || message == WM_NCDESTROY) {
        QueuePointerStateUpdate();
    }
    return originalResult;
}

bool InvokeNativeShellUnhideOnUiThread(HWND taskbarWindow) {
    if (!taskbarWindow || !IsWindow(taskbarWindow)) {
        return false;
    }

    const TaskbarKind kind = GetTaskbarKind(taskbarWindow);
    if (kind == TaskbarKind::kMain) {
        auto it = g_trayUiWndProcObjects.find(taskbarWindow);
        if (it == g_trayUiWndProcObjects.end() ||
            !TrayUI_Unhide_Original || !g_trayUiVtableITrayComponentHost) {
            return false;
        }

        void* trayComponentHost = QueryViaVtable(
            it->second, g_trayUiVtableITrayComponentHost);
        if (!trayComponentHost) {
            Wh_Log(L"Couldn't locate ITrayComponentHost for taskbar %p",
                   taskbarWindow);
            return false;
        }

        TrayUI_Unhide_Original(trayComponentHost, 0, 0);
        return true;
    }

    if (kind == TaskbarKind::kSecondary) {
        auto it = g_secondaryTrayObjects.find(taskbarWindow);
        if (it == g_secondaryTrayObjects.end() ||
            !CSecondaryTray_Unhide_Original) {
            return false;
        }

        CSecondaryTray_Unhide_Original(it->second, 0, 0);
        return true;
    }

    return false;
}

void RememberViewCoordinator(HWND taskbarWindow, void* viewCoordinator) {
    if (!taskbarWindow || !viewCoordinator) {
        return;
    }

    auto it = g_viewCoordinators.find(taskbarWindow);
    if (it != g_viewCoordinators.end() && it->second == viewCoordinator) {
        return;
    }

    g_viewCoordinators[taskbarWindow] = viewCoordinator;
    Wh_Log(L"Captured ViewCoordinator %p for taskbar %p",
           viewCoordinator,
           taskbarWindow);
}

void* GetViewCoordinator(HWND taskbarWindow) {
    auto it = g_viewCoordinators.find(taskbarWindow);
    return it != g_viewCoordinators.end() ? it->second : nullptr;
}

using ViewCoordinator_ShouldTaskbarBeExpanded_t =
    bool(WINAPI*)(void* viewCoordinator,
                  HWND taskbarWindow,
                  bool expanded);

ViewCoordinator_ShouldTaskbarBeExpanded_t
    ViewCoordinator_ShouldTaskbarBeExpanded_Original;

using ViewCoordinator_UpdateIsExpanded_t =
    void(WINAPI*)(void* viewCoordinator,
                  HWND taskbarWindow,
                  int reason);

ViewCoordinator_UpdateIsExpanded_t ViewCoordinator_UpdateIsExpanded_Original;

bool IsTaskbarKeptShownOnUiThread(HWND taskbarWindow) {
    for (const auto& [object, window] : g_taskbarsKeptShown) {
        (void)object;
        if (window == taskbarWindow) {
            return true;
        }
    }
    return false;
}

bool WINAPI ViewCoordinator_ShouldTaskbarBeExpanded_Hook(
    void* viewCoordinator,
    HWND taskbarWindow,
    bool expanded) {
    RememberViewCoordinator(taskbarWindow, viewCoordinator);

    if (!g_unloading.load(std::memory_order_acquire) &&
        g_desiredKeptShownTaskbar.load(std::memory_order_acquire) ==
            taskbarWindow &&
        IsTaskbarKeptShownOnUiThread(taskbarWindow)) {
        return true;
    }

    return ViewCoordinator_ShouldTaskbarBeExpanded_Original(
        viewCoordinator, taskbarWindow, expanded);
}

bool UpdateViewCoordinatorIsExpandedOnUiThread(HWND taskbarWindow) {
    if (!ViewCoordinator_UpdateIsExpanded_Original) {
        return false;
    }

    void* viewCoordinator = GetViewCoordinator(taskbarWindow);
    if (!viewCoordinator) {
        return false;
    }

    // Reason 7 is ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged.
    // The maintained keep-shown implementation uses it to request a normal
    // expansion-policy reevaluation without modifying pointer-over bookkeeping.
    constexpr int kReasonPointerOverTaskbarFrameChanged = 7;
    ViewCoordinator_UpdateIsExpanded_Original(
        viewCoordinator,
        taskbarWindow,
        kReasonPointerOverTaskbarFrameChanged);
    return true;
}

void* GetKeepShownObjectForTaskbarOnUiThread(HWND taskbarWindow) {
    switch (GetTaskbarKind(taskbarWindow)) {
        case TaskbarKind::kMain: {
            auto it = g_trayUiWndProcObjects.find(taskbarWindow);
            if (it == g_trayUiWndProcObjects.end() ||
                !g_trayUiVtableIInspectable) {
                return nullptr;
            }
            return QueryViaVtableBackwards(
                it->second, g_trayUiVtableIInspectable);
        }

        case TaskbarKind::kSecondary: {
            auto it = g_secondaryTrayObjects.find(taskbarWindow);
            return it != g_secondaryTrayObjects.end() ? it->second : nullptr;
        }

        default:
            return nullptr;
    }
}

void ApplyTaskbarKeepShownStateOnUiThread(HWND taskbarWindow) {
    if (!taskbarWindow || !IsCurrentProcessWindow(taskbarWindow)) {
        return;
    }

    const bool shouldKeepShown =
        !g_unloading.load(std::memory_order_acquire) &&
        g_desiredKeptShownTaskbar.load(std::memory_order_acquire) ==
            taskbarWindow;

    if (shouldKeepShown) {
        void* keepShownObject =
            GetKeepShownObjectForTaskbarOnUiThread(taskbarWindow);
        if (!keepShownObject) {
            Wh_Log(L"Couldn't resolve keep-shown object for taskbar %p",
                   taskbarWindow);
            return;
        }

        // Establish the policy before native Unhide. If Unhide asks
        // ShouldTaskbarBeExpanded synchronously, the hook already returns true.
        // Re-running the native unhide on an idempotent state update also repairs
        // a stale visible/layering state without any manual Z-order changes.
        g_taskbarsKeptShown[keepShownObject] = taskbarWindow;

        if (!InvokeNativeShellUnhideOnUiThread(taskbarWindow)) {
            g_taskbarsKeptShown.erase(keepShownObject);
            Wh_Log(L"Native taskbar unhide failed for taskbar %p",
                   taskbarWindow);
            return;
        }

        if (UpdateViewCoordinatorIsExpandedOnUiThread(taskbarWindow)) {
            g_appliedKeptShownTaskbar.store(taskbarWindow,
                                           std::memory_order_release);
        } else {
            // Native Unhide can itself cause ShouldTaskbarBeExpanded to run. If
            // it didn't, leave the state unconfirmed so the worker retries this
            // idempotent show operation instead of assuming a partial reveal is
            // fully established.
            Wh_Log(L"ViewCoordinator not captured yet for taskbar %p",
                   taskbarWindow);
        }
        return;
    }

    // Release is idempotent. Only poke Explorer's native hide path if this mod
    // actually had keep-shown state for the taskbar; a stale/public registered
    // message must not hide a taskbar that the mod never owned. Timer ID 2 is
    // Explorer's native taskbar hide timer and this is the established Windhawk
    // keep-shown release pattern.
    const bool wasKeptShown = EraseKeptShownForWindow(taskbarWindow);
    const bool wasApplied =
        g_appliedKeptShownTaskbar.load(std::memory_order_acquire) ==
        taskbarWindow;
    if (wasKeptShown || wasApplied) {
        if (IsWindow(taskbarWindow) &&
            !SetTimer(taskbarWindow, kTrayUITimerHide, 0, nullptr)) {
            Wh_Log(L"Couldn't arm Explorer's hide timer for taskbar %p: %u",
                   taskbarWindow,
                   GetLastError());
        }
        (void)UpdateViewCoordinatorIsExpandedOnUiThread(taskbarWindow);
    }

    HWND expected = taskbarWindow;
    g_appliedKeptShownTaskbar.compare_exchange_strong(
        expected, nullptr, std::memory_order_acq_rel);
}

bool PostTaskbarStateUpdate(HWND taskbarWindow) {
    if (!taskbarWindow || !IsSupportedTaskbarWindow(taskbarWindow) ||
        !IsCurrentProcessWindow(taskbarWindow)) {
        return false;
    }

    if (!PostMessageW(taskbarWindow,
                      g_updateTaskbarStateMessage,
                      0,
                      0)) {
        Wh_Log(L"Couldn't post taskbar-state update for %p: %u",
               taskbarWindow,
               GetLastError());
        return false;
    }

    return true;
}

struct WorkerTimer {
    UINT_PTR id = 0;
    ULONGLONG dueAt = 0;
};

struct ActivationWorkerState {
    HWND activeTaskbar = nullptr;
    HMONITOR activeTaskbarMonitor = nullptr;
    HMONITOR activationCandidateMonitor = nullptr;
    WorkerTimer releaseTimer;
    WorkerTimer retryTimer;
    ULONGLONG activationCandidateSince = 0;
    HMONITOR fullscreenCacheMonitor = nullptr;
    ULONGLONG fullscreenCacheTimestamp = 0;
    bool fullscreenCacheValue = false;
};

UINT GetMonitorDpiY(HMONITOR monitor) {
    UINT dpiX = 96;
    UINT dpiY = 96;
    if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        return 96;
    }
    return dpiY;
}

void CancelWorkerTimer(WorkerTimer& timer) {
    if (timer.id) {
        KillTimer(nullptr, timer.id);
    }
    timer.id = 0;
    timer.dueAt = 0;
}

bool ScheduleWorkerTimer(UINT delayMs, WorkerTimer& timer) {
    const UINT effectiveDelay = std::max(delayMs, 1U);
    const ULONGLONG requestedDueAt = GetTickCount64() + effectiveDelay;

    if (timer.id) {
        if (timer.dueAt && timer.dueAt <= requestedDueAt) {
            return true;
        }
        CancelWorkerTimer(timer);
    }

    timer.id = SetTimer(nullptr, 0, effectiveDelay, nullptr);
    if (!timer.id) {
        Wh_Log(L"Failed to create worker timer: %u", GetLastError());
        return false;
    }
    timer.dueAt = requestedDueAt;
    return true;
}

bool ScheduleRetry(ActivationWorkerState& state, UINT delayMs) {
    return ScheduleWorkerTimer(delayMs, state.retryTimer);
}

void ResetActivationCandidate(ActivationWorkerState& state) {
    state.activationCandidateMonitor = nullptr;
    state.activationCandidateSince = 0;
}

void InvalidateFullscreenCache(ActivationWorkerState& state) {
    state.fullscreenCacheMonitor = nullptr;
    state.fullscreenCacheTimestamp = 0;
    state.fullscreenCacheValue = false;
}

bool ReleaseActiveTaskbar(ActivationWorkerState& state,
                          bool scheduleRetry = true) {
    CancelWorkerTimer(state.releaseTimer);

    if (!state.activeTaskbar) {
        return true;
    }

    const HWND taskbarWindow = state.activeTaskbar;
    HWND expected = taskbarWindow;
    g_desiredKeptShownTaskbar.compare_exchange_strong(
        expected, nullptr, std::memory_order_acq_rel);

    if (IsSupportedTaskbarWindow(taskbarWindow) &&
        IsCurrentProcessWindow(taskbarWindow) &&
        !PostTaskbarStateUpdate(taskbarWindow)) {
        if (scheduleRetry) {
            // During normal operation, preserve the current keep-shown intent
            // until a release message can be queued. On worker shutdown, fail
            // open instead: desired stays null so the hooks stop suppressing
            // Explorer even if the UI thread is unavailable.
            g_desiredKeptShownTaskbar.store(taskbarWindow,
                                           std::memory_order_release);
            if (ScheduleWorkerTimer(kTransientRetryIntervalMs,
                                    state.releaseTimer)) {
                return false;
            }

            // A failed retry timer must never turn a transient PostMessage
            // failure into a permanently kept-open taskbar. Drop the intent and
            // fail open; the stale UI-thread map is inert while desired is null.
            g_desiredKeptShownTaskbar.store(nullptr,
                                           std::memory_order_release);
            HWND applied = taskbarWindow;
            g_appliedKeptShownTaskbar.compare_exchange_strong(
                applied, nullptr, std::memory_order_acq_rel);
        }
    }

    state.activeTaskbar = nullptr;
    state.activeTaskbarMonitor = nullptr;
    return true;
}

bool RequestTaskbarReveal(ActivationWorkerState& state,
                          HWND taskbarWindow,
                          HMONITOR monitor) {
    const HWND previousDesired =
        g_desiredKeptShownTaskbar.exchange(taskbarWindow,
                                           std::memory_order_acq_rel);
    if (previousDesired && previousDesired != taskbarWindow) {
        PostTaskbarStateUpdate(previousDesired);
    }

    if (!PostTaskbarStateUpdate(taskbarWindow)) {
        HWND expected = taskbarWindow;
        g_desiredKeptShownTaskbar.compare_exchange_strong(
            expected, nullptr, std::memory_order_acq_rel);
        return false;
    }

    state.activeTaskbar = taskbarWindow;
    state.activeTaskbarMonitor = monitor;
    ResetActivationCandidate(state);

    // Confirmation is asynchronous. If Explorer's taskbar UI thread was busy
    // or the keep-shown object couldn't be resolved, retry idempotently at 1 Hz.
    if (g_appliedKeptShownTaskbar.load(std::memory_order_acquire) !=
        taskbarWindow) {
        ScheduleRetry(state, kTransientRetryIntervalMs);
    }
    return true;
}

bool GetCachedFullscreenState(ActivationWorkerState& state,
                              HMONITOR monitor,
                              ULONGLONG now) {
    if (state.fullscreenCacheMonitor == monitor &&
        now - state.fullscreenCacheTimestamp < kFullscreenCacheDurationMs) {
        return state.fullscreenCacheValue;
    }

    state.fullscreenCacheMonitor = monitor;
    state.fullscreenCacheTimestamp = now;
    state.fullscreenCacheValue = IsFullscreenWindowOnMonitor(monitor);
    return state.fullscreenCacheValue;
}

void ProcessPointerState(ActivationWorkerState& state) {
    if (g_unloading.load(std::memory_order_acquire)) {
        return;
    }

    POINT pointer{};
    if (!GetCursorPos(&pointer)) {
        return;
    }

    HMONITOR pointerMonitor =
        MonitorFromPoint(pointer, MONITOR_DEFAULTTONULL);
    if (!pointerMonitor) {
        (void)ReleaseActiveTaskbar(state);
        CancelWorkerTimer(state.retryTimer);
        ResetActivationCandidate(state);
        return;
    }

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(pointerMonitor, &monitorInfo)) {
        return;
    }

    const ULONGLONG now = GetTickCount64();
    const bool insideActivationBand = IsPointInsideActivationBand(
        pointer, monitorInfo, GetMonitorDpiY(pointerMonitor));

    if (state.activeTaskbar) {
        // UI-thread destruction or a fail-open cleanup can clear the desired
        // keep-shown target before the worker observes it. Don't let a stale or
        // recycled HWND keep the worker logically active after that happens.
        if (g_desiredKeptShownTaskbar.load(std::memory_order_acquire) !=
            state.activeTaskbar) {
            CancelWorkerTimer(state.releaseTimer);
            state.activeTaskbar = nullptr;
            state.activeTaskbarMonitor = nullptr;
        }
    }

    if (state.activeTaskbar) {
        bool activeTaskbarAllowed = true;
        if (g_settings.primaryMonitorOnly.load(std::memory_order_relaxed)) {
            MONITORINFO activeMonitorInfo{};
            activeMonitorInfo.cbSize = sizeof(activeMonitorInfo);
            activeTaskbarAllowed =
                state.activeTaskbarMonitor &&
                GetMonitorInfoW(state.activeTaskbarMonitor, &activeMonitorInfo) &&
                (activeMonitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;
        }

        if (!activeTaskbarAllowed) {
            if (!ReleaseActiveTaskbar(state)) {
                return;
            }
        } else if (!IsSupportedTaskbarWindow(state.activeTaskbar) ||
                   !IsCurrentProcessWindow(state.activeTaskbar)) {
            CancelWorkerTimer(state.releaseTimer);
            state.activeTaskbar = nullptr;
            state.activeTaskbarMonitor = nullptr;
        } else {
            const bool sameMonitorBand =
                insideActivationBand &&
                pointerMonitor == state.activeTaskbarMonitor;
            const bool overTaskbar =
                IsPointOverTaskbar(state.activeTaskbar, pointer);

            if (sameMonitorBand || overTaskbar) {
                CancelWorkerTimer(state.releaseTimer);

                if (g_appliedKeptShownTaskbar.load(
                        std::memory_order_acquire) != state.activeTaskbar) {
                    // RequestTaskbarReveal already queued the first update. While
                    // that update is pending, cursor movement must not flood the
                    // taskbar UI queue with duplicate registered messages.
                    if (!state.retryTimer.id) {
                        (void)PostTaskbarStateUpdate(state.activeTaskbar);
                        (void)ScheduleRetry(state, kTransientRetryIntervalMs);
                    }
                } else {
                    CancelWorkerTimer(state.retryTimer);
                }
                return;
            }

            if (insideActivationBand &&
                pointerMonitor != state.activeTaskbarMonitor) {
                // Direct monitor transitions don't pay the old monitor's
                // release delay before starting the new monitor's hover delay.
                if (!ReleaseActiveTaskbar(state)) {
                    return;
                }
            } else {
                const int releaseDelay =
                    g_settings.releaseDelayMs.load(std::memory_order_relaxed);
                if (releaseDelay > 0 &&
                    ScheduleWorkerTimer(static_cast<UINT>(releaseDelay),
                                        state.releaseTimer)) {
                    return;
                }

                // If the release-delay timer can't be created, fail open and
                // release immediately rather than risking a taskbar that stays
                // kept shown after the pointer has left.
                if (!ReleaseActiveTaskbar(state)) {
                    return;
                }
            }
        }
    }

    if (!insideActivationBand) {
        CancelWorkerTimer(state.retryTimer);
        ResetActivationCandidate(state);
        InvalidateFullscreenCache(state);
        return;
    }

    const int activationDelay =
        g_settings.activationDelayMs.load(std::memory_order_relaxed);
    if (state.activationCandidateMonitor != pointerMonitor) {
        state.activationCandidateMonitor = pointerMonitor;
        state.activationCandidateSince = now;
        if (activationDelay > 0) {
            ScheduleRetry(state, static_cast<UINT>(activationDelay));
            return;
        }
    } else {
        const ULONGLONG elapsed = now - state.activationCandidateSince;
        if (elapsed < static_cast<ULONGLONG>(activationDelay)) {
            ScheduleRetry(state, static_cast<UINT>(activationDelay - elapsed));
            return;
        }
    }

    CancelWorkerTimer(state.retryTimer);

    // Event-driven gates first. Settings changes and taskbar creation already
    // queue a fresh pointer-state update, so these conditions aren't polled.
    if (g_settings.primaryMonitorOnly.load(std::memory_order_relaxed) &&
        (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) == 0) {
        return;
    }

    if (!g_viewCoordinatorSupportAvailable.load(std::memory_order_acquire)) {
        return;
    }

    // A held button and fullscreen state can change while the cursor is parked.
    if (g_settings.ignoreWhileMouseButtonDown.load(std::memory_order_relaxed) &&
        IsAnyMouseButtonDown()) {
        ScheduleRetry(state, kTransientRetryIntervalMs);
        return;
    }

    if (g_settings.requireWindowsAutoHide.load(std::memory_order_relaxed) &&
        !IsWindowsAutoHideEnabled()) {
        return;
    }

    HWND taskbarWindow = FindTaskbarForMonitor(pointerMonitor);
    if (!taskbarWindow) {
        return;
    }

    if (g_settings.ignoreFullscreenApps.load(std::memory_order_relaxed) &&
        GetCachedFullscreenState(state, pointerMonitor, now)) {
        ScheduleRetry(state, kTransientRetryIntervalMs);
        return;
    }

    if (!RequestTaskbarReveal(state, taskbarWindow, pointerMonitor)) {
        ScheduleRetry(state, kTransientRetryIntervalMs);
    }
}

void HandleRetryTimer(ActivationWorkerState& state) {
    CancelWorkerTimer(state.retryTimer);
    ProcessPointerState(state);
}

void HandleReleaseTimer(ActivationWorkerState& state) {
    CancelWorkerTimer(state.releaseTimer);
    if (!state.activeTaskbar) {
        return;
    }

    POINT pointer{};
    bool keepActive = false;
    if (GetCursorPos(&pointer) &&
        IsSupportedTaskbarWindow(state.activeTaskbar) &&
        IsCurrentProcessWindow(state.activeTaskbar)) {
        HMONITOR pointerMonitor =
            MonitorFromPoint(pointer, MONITOR_DEFAULTTONULL);
        MONITORINFO monitorInfo{};
        monitorInfo.cbSize = sizeof(monitorInfo);
        if (pointerMonitor && GetMonitorInfoW(pointerMonitor, &monitorInfo)) {
            const bool inBand = IsPointInsideActivationBand(
                pointer, monitorInfo, GetMonitorDpiY(pointerMonitor));
            keepActive =
                (inBand && pointerMonitor == state.activeTaskbarMonitor) ||
                IsPointOverTaskbar(state.activeTaskbar, pointer);
        }
    }

    if (keepActive) {
        return;
    }

    if (ReleaseActiveTaskbar(state)) {
        ProcessPointerState(state);
    }
}

void CALLBACK CursorWinEventProc(HWINEVENTHOOK,
                                 DWORD event,
                                 HWND,
                                 LONG idObject,
                                 LONG,
                                 DWORD,
                                 DWORD) {
    if (event != EVENT_OBJECT_LOCATIONCHANGE || idObject != OBJID_CURSOR) {
        return;
    }

    QueuePointerStateUpdate();
}

DWORD WINAPI ActivationWorkerThread(LPVOID) {
    // Force creation of this thread's message queue before publishing its ID.
    MSG message{};
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_workerThreadId.store(GetCurrentThreadId(), std::memory_order_release);

    // The hook is system-wide because WinEvent can't filter by object ID at
    // registration time. CursorWinEventProc immediately discards every event
    // except OBJID_CURSOR, avoiding permanent polling while the mouse is idle.
    HWINEVENTHOOK cursorHook = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE,
        EVENT_OBJECT_LOCATIONCHANGE,
        nullptr,
        CursorWinEventProc,
        0,
        0,
        WINEVENT_OUTOFCONTEXT);

    UINT_PTR fallbackPollTimerId = 0;
    if (!cursorHook) {
        Wh_Log(L"SetWinEventHook for cursor movement failed: %u; "
               L"using a 150 ms fallback timer",
               GetLastError());
        fallbackPollTimerId =
            SetTimer(nullptr, 0, 150, nullptr);
        if (!fallbackPollTimerId) {
            Wh_Log(L"Fallback cursor timer creation failed: %u; cursor "
                   L"tracking is unavailable",
                   GetLastError());
        }
    }

    ActivationWorkerState state;
    ProcessPointerState(state);

    for (;;) {
        DWORD waitResult = MsgWaitForMultipleObjects(
            1,
            &g_stopEvent,
            FALSE,
            INFINITE,
            QS_ALLINPUT);

        if (waitResult == WAIT_OBJECT_0) {
            break;
        }

        if (waitResult != WAIT_OBJECT_0 + 1) {
            Wh_Log(L"Activation worker stopped: MsgWaitForMultipleObjects failed: %u; "
                   L"a later shell/environment event will retry",
                   GetLastError());
            break;
        }

        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            switch (message.message) {
                case kWorkerCursorChangedMessage:
                    g_cursorUpdatePosted.store(false,
                                               std::memory_order_release);
                    ProcessPointerState(state);
                    break;

                case WM_TIMER:
                    if (state.releaseTimer.id &&
                        message.wParam == state.releaseTimer.id) {
                        HandleReleaseTimer(state);
                    } else if (state.retryTimer.id &&
                               message.wParam == state.retryTimer.id) {
                        HandleRetryTimer(state);
                    } else if (fallbackPollTimerId &&
                               message.wParam == fallbackPollTimerId) {
                        ProcessPointerState(state);
                    }
                    break;

                default:
                    TranslateMessage(&message);
                    DispatchMessageW(&message);
                    break;
            }
        }
    }

    CancelWorkerTimer(state.retryTimer);
    CancelWorkerTimer(state.releaseTimer);

    // Release the mod-owned keep-shown intent without waiting on Explorer's UI
    // thread. If the posted reconciliation is delayed, the hooks fail open as
    // soon as unloading begins, so no shell state can remain latched.
    if (state.activeTaskbar) {
        (void)ReleaseActiveTaskbar(state, false);
    }

    if (cursorHook) {
        UnhookWinEvent(cursorHook);
    } else if (fallbackPollTimerId) {
        KillTimer(nullptr, fallbackPollTimerId);
    }

    g_cursorUpdatePosted.store(false, std::memory_order_release);
    g_workerThreadId.store(0, std::memory_order_release);
    return 0;
}

bool HookTaskbarViewSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded(unsigned __int64,bool))",
            },
            &ViewCoordinator_ShouldTaskbarBeExpanded_Original,
            ViewCoordinator_ShouldTaskbarBeExpanded_Hook,
            true,  // Checked as a required pair below for a clearer diagnostic.
        },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::UpdateIsExpanded(unsigned __int64,enum TaskbarTipTest::TaskbarExpandCollapseReason))",
            },
            &ViewCoordinator_UpdateIsExpanded_Original,
            nullptr,
            true,  // Checked as a required pair below for a clearer diagnostic.
        },
    };

    if (!WindhawkUtils::HookSymbols(
            module,
            symbolHooks,
            ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"Failed to hook Windows 11 ViewCoordinator symbols");
        return false;
    }

    if (!ViewCoordinator_ShouldTaskbarBeExpanded_Original ||
        !ViewCoordinator_UpdateIsExpanded_Original) {
        Wh_Log(L"Taskbar keep-shown path unavailable: required ViewCoordinator "
               L"symbols need updating");
        return false;
    }

    g_viewCoordinatorSupportAvailable.store(true, std::memory_order_release);
    return true;
}

bool HookTaskbarUiThreadSymbol() {
    HMODULE taskbarModule = LoadLibraryExW(
        L"taskbar.dll",
        nullptr,
        LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (!taskbarModule) {
        Wh_Log(L"Couldn't load taskbar.dll: %u", GetLastError());
        return false;
    }

    // taskbar.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                LR"(const TrayUI::`vftable'{for `IInspectable'})",
            },
            &g_trayUiVtableIInspectable,
            nullptr,
        },
        {
            {
                LR"(const TrayUI::`vftable'{for `ITrayComponentHost'})",
            },
            &g_trayUiVtableITrayComponentHost,
            nullptr,
        },
        {
            {
                LR"(public: void __cdecl TrayUI::_Hide(void))",
            },
            &TrayUI_Hide_Original,
            TrayUI_Hide_Hook,
        },
        {
            {
                LR"(private: void __cdecl CSecondaryTray::_AutoHide(bool))",
            },
            &CSecondaryTray_AutoHide_Original,
            CSecondaryTray_AutoHide_Hook,
            true,
        },
        {
            {
                LR"(public: virtual void __cdecl TrayUI::Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))",
            },
            &TrayUI_Unhide_Original,
            nullptr,
        },
        {
            {
                LR"(private: void __cdecl CSecondaryTray::_Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))",
            },
            &CSecondaryTray_Unhide_Original,
            nullptr,
            true,
        },
        {
            {
                LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))",
            },
            &TrayUI_WndProc_Original,
            TrayUI_WndProc_Hook,
        },
        {
            {
                LR"(private: virtual __int64 __cdecl CSecondaryTray::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))",
            },
            &CSecondaryTray_WndProc_Original,
            CSecondaryTray_WndProc_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(
            taskbarModule,
            symbolHooks,
            ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"Failed to hook required native taskbar symbols");
        return false;
    }

    const bool secondaryTaskbarSupportAvailable =
        CSecondaryTray_AutoHide_Original && CSecondaryTray_Unhide_Original &&
        CSecondaryTray_WndProc_Original;
    g_secondaryTaskbarSupportAvailable.store(
        secondaryTaskbarSupportAvailable,
        std::memory_order_release);
    if (!secondaryTaskbarSupportAvailable) {
        Wh_Log(L"Secondary taskbar support unavailable: optional "
               L"CSecondaryTray symbols need updating");
    }

    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");

    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedTaskbarViewModule(HMODULE module,
                                   LPCWSTR libraryFileName) {
    if (g_unloading.load(std::memory_order_acquire)) {
        return;
    }

    if (!module || g_taskbarViewDllLoaded.load(
                       std::memory_order_acquire)) {
        return;
    }

    if (GetTaskbarViewModuleHandle() != module) {
        return;
    }

    if (g_taskbarViewDllLoaded.exchange(
            true,
            std::memory_order_acq_rel)) {
        return;
    }

    Wh_Log(L"Loaded taskbar view module: %s",
           libraryFileName ? libraryFileName : L"(unknown)");

    if (HookTaskbarViewSymbols(module)) {
        Wh_ApplyHookOperations();
        QueuePointerStateUpdate();
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR libraryFileName,
                                   HANDLE file,
                                   DWORD flags) {
    HMODULE module =
        LoadLibraryExW_Original(libraryFileName, file, flags);

    if (module && !((ULONG_PTR)module & 3)) {
        HandleLoadedTaskbarViewModule(module, libraryFileName);
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing");

    g_unloading.store(false, std::memory_order_relaxed);

    g_updateTaskbarStateMessage = RegisterWindowMessageW(
        L"Windhawk_updateTaskbarKeepShown_" WH_MOD_ID);
    if (!g_updateTaskbarStateMessage) {
        Wh_Log(L"RegisterWindowMessageW failed for taskbar-state channel");
        return FALSE;
    }

    LoadSettings();

    if (!HookTaskbarUiThreadSymbol()) {
        return FALSE;
    }

    if (HMODULE taskbarViewModule =
            GetTaskbarViewModuleHandle()) {
        if (!HookTaskbarViewSymbols(taskbarViewModule)) {
            return FALSE;
        }

        g_taskbarViewDllLoaded.store(
            true,
            std::memory_order_release);
    } else {
        Wh_Log(L"Taskbar.View.dll/ExplorerExtensions.dll isn't loaded yet");

        HMODULE kernelBaseModule =
            GetModuleHandleW(L"kernelbase.dll");

        auto loadLibraryExW = reinterpret_cast<decltype(&LoadLibraryExW)>(
            GetProcAddress(kernelBaseModule, "LoadLibraryExW"));

        if (!loadLibraryExW) {
            Wh_Log(L"Couldn't find KernelBase!LoadLibraryExW");
            return FALSE;
        }

        if (!WindhawkUtils::SetFunctionHook(
                loadLibraryExW,
                LoadLibraryExW_Hook,
                &LoadLibraryExW_Original)) {
            Wh_Log(L"Couldn't hook KernelBase!LoadLibraryExW");
            return FALSE;
        }
    }

    g_stopEvent =
        CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"Looking for taskbar windows in this Explorer process");

    // Only the shell Explorer instance registers Shell_TrayWnd. Folder-window
    // Explorer processes never create the activation worker or its system-wide
    // WinEvent hook.
    WNDCLASSW taskbarClass{};
    if (GetClassInfoW(GetModuleHandleW(nullptr),
                      L"Shell_TrayWnd",
                      &taskbarClass)) {
        // Existing taskbar objects don't need a separate object-discovery send: the
        // first posted taskbar-state update records its WndProc pThis before the
        // operation is dispatched. This leaves worker startup independent of
        // taskbar UI-thread responsiveness.
        EnsureActivationWorker();
    } else {
        Wh_Log(L"Shell_TrayWnd class isn't registered/created in this Explorer "
               L"process yet; a later taskbar WM_NCCREATE can start the worker");
    }

    // Retry in case the taskbar view module loaded between Wh_ModInit and hook
    // application.
    if (!g_taskbarViewDllLoaded.load(std::memory_order_acquire)) {
        if (HMODULE taskbarViewModule =
                GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(
                    true,
                    std::memory_order_acq_rel)) {
                Wh_Log(L"Found taskbar view module after initialization");

                if (HookTaskbarViewSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                    QueuePointerStateUpdate();
                }
            }
        }
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"Stopping activation worker");
    g_unloading.store(true, std::memory_order_release);

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    HANDLE workerThread = nullptr;
    {
        std::lock_guard<std::mutex> guard(g_workerThreadMutex);
        workerThread = g_workerThread;
    }
    if (workerThread) {
        WaitForSingleObject(workerThread, INFINITE);
    }

    // Fail open even if the taskbar UI thread is busy: ShouldTaskbarBeExpanded
    // and the hide hooks stop enforcing keep-shown as soon as g_unloading is
    // true. This post only asks Explorer to begin hiding promptly while the
    // hooks are still installed; no persistent shell pointer state exists.
    HWND desired =
        g_desiredKeptShownTaskbar.exchange(nullptr, std::memory_order_acq_rel);
    if (desired) {
        PostTaskbarStateUpdate(desired);
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing");

    HANDLE workerThread = nullptr;
    {
        std::lock_guard<std::mutex> guard(g_workerThreadMutex);
        workerThread = g_workerThread;
        g_workerThread = nullptr;
    }
    if (workerThread) {
        CloseHandle(workerThread);
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    LoadSettings();
    EnsureActivationWorker();
    QueuePointerStateUpdate();
}
