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

The cursor stays stationary. The mod tracks cursor movement, then asks Explorer
on the taskbar UI thread to use its native taskbar unhide and pointer-over paths.

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
- No cursor injection, taskbar layout changes, Explorer timer manipulation, or
  taskbar Z-order forcing

## Behavior

The activation threshold controls where the taskbar can be revealed. Once this
mod reveals a taskbar, it keeps Explorer's synthetic pointer-over state until
the pointer has left both the activation band and the visible taskbar, then
waits for the configured release delay before clearing it.

With a typical Windows 11 taskbar, the default 24-DIP band is shorter than the
taskbar itself. In that common case the threshold mainly changes the trigger
area; after reveal, the taskbar's own rectangle keeps it open while you use its
buttons. Values larger than the taskbar height additionally let the pointer sit
in the strip above the visible taskbar without collapsing it. Moving directly
from one monitor's activation band to another releases the old monitor
immediately, then applies the new monitor's hover delay instead of stacking both
release and hover delays.

## Compatibility

The mod is intended for the standard bottom-positioned Windows 11 taskbar. It
doesn't modify taskbar geometry, XAML, height, icon size, animation duration,
transparency, the Windows auto-hide setting, or Z-order.

Taskbar Z-order remains Explorer's responsibility. The mod deliberately uses
Explorer's native TrayUI/CSecondaryTray unhide before asserting synthetic
pointer-over, but it never calls `SetWindowPos` or forces `HWND_TOPMOST`.
Unusual shell layering or a Z-order customization can therefore still affect
which window appears above the taskbar.

**Taskbar auto-hide custom activation area** controls the allowed region along
the taskbar, while this mod enlarges the trigger distance perpendicular to the
bottom edge. The implementations solve different trigger problems, but both
hook Explorer's pointer-over path. This mod temporarily holds Explorer's
mouse pointer-leave while it owns an enlarged-band reveal, whereas that mod
observes and filters the same events. Touch/pen leave notifications aren't
suppressed. Installing both can therefore be hook-order sensitive and isn't
guaranteed to combine cleanly. The same mouse pointer-leave interception can
affect **Taskbar auto-hide fine tuning / keyboard-only** state tracking, so
combinations with mods that hook the same callback should be considered
unsupported unless specifically tested.

Mods that replace or intentionally suppress Explorer's native reveal behavior
(such as keyboard-only or never-show mouse modes) are intentionally
incompatible with mouse activation from this mod.

## Requirements

- Windows 11
- Standard Windows 11 taskbar
- Bottom-positioned taskbar
- **Automatically hide the taskbar** enabled, unless **Require Windows
  auto-hide** is disabled

## Known limitations

- Top-, left-, right-, detached-, and arbitrary-position taskbars aren't
  supported.
- ExplorerPatcher's legacy Windows 10 taskbar isn't supported.
- The Windows `ABM_GETSTATE` auto-hide flag is system-wide. With per-monitor
  auto-hide mods, **Require Windows auto-hide** can't determine the auto-hide
  state of an individual monitor.
- Reveal depends on undocumented Windows 11 taskbar symbols, including
  `ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged`,
  `ViewCoordinator::ShouldTaskbarBeExpanded`, and the native TrayUI/secondary
  tray unhide paths. `ShouldTaskbarBeExpanded` is pass-through and is hooked
  only to capture Explorer's coordinator early. A Windows update can require
  symbol updates. Secondary-taskbar symbols are optional so a secondary symbol
  change doesn't disable the primary taskbar.
- Fullscreen suppression checks Windows presentation/exclusive-fullscreen
  state and the topmost eligible application window on the target monitor.
  Background fullscreen windows behind another normal app don't suppress the
  band. Click-through, tool, non-activating, cloaked, desktop, and taskbar
  windows are ignored. Presentation mode is system-wide. Suppression is checked
  before reveal; it doesn't continuously force-hide an already revealed
  taskbar if an app later becomes fullscreen.
- Cursor tracking uses a system-wide `EVENT_OBJECT_LOCATIONCHANGE` WinEvent
  hook because Windows can't filter that hook to `OBJID_CURSOR` at
  registration time. Non-cursor events are discarded immediately. If the hook
  can't be installed, a 150 ms cursor-check timer is used instead.
- Reveal-blocking conditions that can change without cursor movement (a held
  mouse button, fullscreen state, or a coordinator that hasn't been observed
  yet) are retried at most once per second. Settings-only conditions and
  monitors without a supported taskbar are event-driven and aren't polled.

## Attribution and license

The Windows 11 taskbar symbols and native reveal techniques were informed by
GPL-3.0-licensed Windhawk taskbar mods by m417z, including **Taskbar auto-hide
fine tuning** and **Taskbar auto-hide custom activation area**.

This mod is distributed under the GNU General Public License v3.0.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- activationThresholdPx: 24
  $name: Activation threshold
  $description: >-
    Height of the activation band in logical pixels at 96 DPI, scaled for each
    monitor. 12-48 is a practical range for most taskbars; values taller than
    the taskbar also keep it open while the pointer is in the strip above it.
- activationDelayMs: 75
  $name: Hover delay
  $description: >-
    Time the pointer must remain in the activation band before revealing the
    taskbar. A short delay helps avoid accidental reveals when crossing between
    vertically stacked monitors.
- releaseDelayMs: 120
  $name: Release delay
  $description: >-
    Delay before the taskbar is allowed to hide after the pointer leaves both
    the activation band and the visible taskbar.
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
#include <wchar.h>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Settings {
    std::atomic<int> activationThresholdPx{24};
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

HANDLE g_stopEvent = nullptr;
std::atomic<HANDLE> g_workerThread{nullptr};
std::mutex g_workerThreadMutex;
std::atomic<bool> g_unloading{false};

// Registered in Wh_ModInit rather than during DLL initialization, avoiding
// user32 calls while the loader lock is held.
UINT g_uiThreadMessage = 0;
enum UiOperation : WPARAM {
    kUiRevealTaskbar = 1,
    kUiClearSyntheticPointerOver = 2,
    kUiClearAllSyntheticPointerOver = 3,
};

constexpr UINT kWorkerCursorChangedMessage = WM_APP + 1;
constexpr UINT kTransientRetryIntervalMs = 1000;
constexpr UINT kActivationAttemptThrottleMs = 200;

// These containers are used by taskbar implementation callbacks and by
// operations marshalled to the taskbar UI thread. This matches the threading
// convention used by the maintained taskbar auto-hide mods.
std::unordered_map<HWND, void*> g_trayUiWndProcObjects;
std::unordered_map<HWND, void*> g_secondaryTrayObjects;
std::unordered_map<HWND, void*> g_viewCoordinators;
std::unordered_set<HWND> g_syntheticPointerOverTaskbars;

void* g_trayUiVtableITrayComponentHost = nullptr;

void ForgetTaskbarState(HWND taskbarWindow) {
    g_viewCoordinators.erase(taskbarWindow);
    g_syntheticPointerOverTaskbars.erase(taskbarWindow);
}

DWORD WINAPI ActivationWorkerThread(LPVOID);
bool HandleTaskbarUiOperation(HWND taskbarWindow,
                              WPARAM operationValue,
                              LRESULT* result);

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

    HANDLE existing = g_workerThread.load(std::memory_order_acquire);
    if (existing) {
        const DWORD state = WaitForSingleObject(existing, 0);
        if (state == WAIT_TIMEOUT) {
            return;
        }
        if (state == WAIT_OBJECT_0) {
            g_workerThread.store(nullptr, std::memory_order_release);
            CloseHandle(existing);
            Wh_Log(L"Restarting activation worker after prior thread exit");
        } else {
            Wh_Log(L"Couldn't query activation worker state: %u",
                   GetLastError());
            return;
        }
    }

    HANDLE workerThread = CreateThread(
        nullptr, 0, ActivationWorkerThread, nullptr, 0, nullptr);
    if (!workerThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        return;
    }

    g_workerThread.store(workerThread, std::memory_order_release);
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

HWND FindAnyTaskbarWindow() {
    if (HWND mainTaskbar = FindMainTaskbarWindow()) {
        return mainTaskbar;
    }

    HWND taskbarWindow = nullptr;
    EnumWindows(
        [](HWND window, LPARAM parameter) -> BOOL {
            if (IsSupportedTaskbarWindow(window) &&
                IsCurrentProcessWindow(window)) {
                *reinterpret_cast<HWND*>(parameter) = window;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&taskbarWindow));
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

void* QueryViaVtable(void* object, void* targetVtable) {
    if (!object || !targetVtable) {
        return nullptr;
    }

    // The maintained Windhawk taskbar implementation locates the
    // ITrayComponentHost subobject by scanning the object's vtable pointers.
    // Keep the small scan bound used by established Windhawk taskbar mods so
    // an object-layout change can't turn this into a wide memory walk.
    auto** candidate = reinterpret_cast<void**>(object);

    for (size_t i = 0; i < 20; i++, candidate++) {
        if (*candidate == targetVtable) {
            return candidate;
        }
    }

    return nullptr;
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
    if (message == g_uiThreadMessage) {
        // Record the existing taskbar object's pThis for free: this message is
        // already executing with the correct TrayUI instance on its UI thread.
        g_trayUiWndProcObjects[window] = trayUi;
        g_mainTaskbarWindow.store(window, std::memory_order_relaxed);

        LRESULT result = 0;
        if (HandleTaskbarUiOperation(window, wParam, &result)) {
            if (handled) {
                *handled = true;
            }
            return result;
        }
    }

    HandleTaskbarEnvironmentMessage(message);

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
    if (taskbarCreating) {
        // Wake a worker that may be backing off because this monitor previously
        // had no taskbar. Queue only after Explorer has processed WM_NCCREATE.
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
    if (message == g_uiThreadMessage) {
        // As with the primary taskbar, the first UI operation records the
        // existing CSecondaryTray object without a separate capture round-trip.
        g_secondaryTrayObjects[window] = secondaryTray;

        LRESULT result = 0;
        if (HandleTaskbarUiOperation(window, wParam, &result)) {
            return result;
        }
    }

    HandleTaskbarEnvironmentMessage(message);

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
    if (taskbarCreating) {
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

    // A worker waiting for the first coordinator doesn't need to poll until the
    // next retry interval; the capture itself is the event that makes reveal
    // possible.
    QueuePointerStateUpdate();
}

void* GetViewCoordinator(HWND taskbarWindow) {
    auto it = g_viewCoordinators.find(taskbarWindow);
    return it != g_viewCoordinators.end() ? it->second : nullptr;
}

// Undocumented Windows 11 taskbar method. inputDeviceKind 0 corresponds to
// the mouse path used by Windows' taskbar implementation.
using ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_t =
    void(WINAPI*)(void* viewCoordinator,
                  HWND taskbarWindow,
                  bool isPointerOver,
                  int inputDeviceKind);

ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_t
    ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original;

using ViewCoordinator_ShouldTaskbarBeExpanded_t =
    bool(WINAPI*)(void* viewCoordinator,
                  HWND taskbarWindow,
                  bool expanded);

ViewCoordinator_ShouldTaskbarBeExpanded_t
    ViewCoordinator_ShouldTaskbarBeExpanded_Original;

bool WINAPI ViewCoordinator_ShouldTaskbarBeExpanded_Hook(
    void* viewCoordinator,
    HWND taskbarWindow,
    bool expanded) {
    RememberViewCoordinator(taskbarWindow, viewCoordinator);
    return ViewCoordinator_ShouldTaskbarBeExpanded_Original(
        viewCoordinator, taskbarWindow, expanded);
}

void WINAPI
ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Hook(
    void* viewCoordinator,
    HWND taskbarWindow,
    bool isPointerOver,
    int inputDeviceKind) {
    RememberViewCoordinator(taskbarWindow, viewCoordinator);

    if (!isPointerOver && inputDeviceKind == 0 &&
        g_syntheticPointerOverTaskbars.contains(taskbarWindow)) {
        if (g_workerThreadId.load(std::memory_order_acquire)) {
            // Explorer's physical frame is narrower than the configured band.
            // Keep our synthetic enter until the worker applies releaseDelayMs.
            return;
        }

        // Never leave the taskbar latched if the worker unexpectedly stopped.
        g_syntheticPointerOverTaskbars.erase(taskbarWindow);
        Wh_Log(L"Worker unavailable; releasing synthetic state for taskbar %p",
               taskbarWindow);
        EnsureActivationWorker();
    }

    ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original(
        viewCoordinator,
        taskbarWindow,
        isPointerOver,
        inputDeviceKind);
}

bool SetSyntheticPointerOverOnUiThread(HWND taskbarWindow,
                                       bool isPointerOver) {
    if (!isPointerOver && taskbarWindow &&
        !g_syntheticPointerOverTaskbars.contains(taskbarWindow)) {
        return true;
    }

    if (!taskbarWindow || !IsWindow(taskbarWindow) ||
        !ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original) {
        if (taskbarWindow && !IsWindow(taskbarWindow)) {
            ForgetTaskbarState(taskbarWindow);
        }
        return false;
    }

    void* viewCoordinator = GetViewCoordinator(taskbarWindow);
    if (!viewCoordinator) {
        Wh_Log(L"No ViewCoordinator captured yet for taskbar %p",
               taskbarWindow);
        return false;
    }

    if (isPointerOver) {
        const auto [_, inserted] =
            g_syntheticPointerOverTaskbars.insert(taskbarWindow);

        if (!inserted) {
            return true;
        }

        Wh_Log(L"Setting synthetic pointer-over for taskbar %p",
               taskbarWindow);

        // Explorer's native handler performs the corresponding expansion
        // reevaluation internally. Calling it once mirrors the established
        // taskbar auto-hide reveal path without overriding expansion policy.
        ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original(
            viewCoordinator,
            taskbarWindow,
            true,
            0);
    } else {
        if (g_syntheticPointerOverTaskbars.erase(taskbarWindow) == 0) {
            return true;
        }

        Wh_Log(L"Clearing synthetic pointer-over for taskbar %p",
               taskbarWindow);

        ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original(
            viewCoordinator,
            taskbarWindow,
            false,
            0);
    }

    return true;
}

bool RevealTaskbarOnUiThread(HWND taskbarWindow) {
    if (!taskbarWindow || !IsWindow(taskbarWindow) ||
        !ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original) {
        return false;
    }

    // Don't start Explorer's visible unhide until the coordinator is available.
    // This avoids a native-only first reveal that immediately collapses before
    // the worker can take synthetic pointer-over ownership.
    if (!GetViewCoordinator(taskbarWindow)) {
        Wh_Log(L"ViewCoordinator isn't captured yet for taskbar %p",
               taskbarWindow);
        return false;
    }

    // Preserve Explorer's own slide/Z-order transition before asserting the
    // synthetic mouse pointer-over state. This is important for shell layering,
    // but only after we know the matching synthetic ownership can be established.
    if (!InvokeNativeShellUnhideOnUiThread(taskbarWindow)) {
        return false;
    }

    return SetSyntheticPointerOverOnUiThread(taskbarWindow, true);
}

void ClearAllSyntheticPointerOverOnUiThread() {
    std::vector<HWND> taskbars(
        g_syntheticPointerOverTaskbars.begin(),
        g_syntheticPointerOverTaskbars.end());

    for (HWND taskbarWindow : taskbars) {
        SetSyntheticPointerOverOnUiThread(taskbarWindow, false);
    }

    g_syntheticPointerOverTaskbars.clear();
}

bool HandleTaskbarUiOperation(HWND taskbarWindow,
                              WPARAM operationValue,
                              LRESULT* result) {
    if (!result || operationValue < kUiRevealTaskbar ||
        operationValue > kUiClearAllSyntheticPointerOver) {
        return false;
    }

    switch (static_cast<UiOperation>(operationValue)) {
        case kUiRevealTaskbar:
            *result = RevealTaskbarOnUiThread(taskbarWindow) ? 1 : 0;
            return true;

        case kUiClearSyntheticPointerOver:
            *result = SetSyntheticPointerOverOnUiThread(
                          taskbarWindow, false)
                          ? 1
                          : 0;
            return true;

        case kUiClearAllSyntheticPointerOver:
            ClearAllSyntheticPointerOverOnUiThread();
            *result = 1;
            return true;
    }

    return false;
}

enum class UiCallResult {
    kFailed,
    kSucceeded,
    kUncertain,
};

UiCallResult SendUiOperation(UiOperation operation,
                             HWND taskbarWindow,
                             DWORD timeoutMs = 250) {
    HWND destination = taskbarWindow;
    if (operation == kUiClearAllSyntheticPointerOver) {
        destination = FindAnyTaskbarWindow();
    }

    if (!destination || !IsSupportedTaskbarWindow(destination) ||
        !IsCurrentProcessWindow(destination)) {
        return UiCallResult::kFailed;
    }

    DWORD_PTR result = 0;
    SetLastError(ERROR_SUCCESS);
    if (!SendMessageTimeoutW(
            destination,
            g_uiThreadMessage,
            operation,
            0,
            SMTO_ABORTIFHUNG | SMTO_BLOCK | SMTO_ERRORONEXIT,
            timeoutMs,
            &result)) {
        const DWORD error = GetLastError();
        Wh_Log(L"Taskbar UI operation timed out/failed: operation=%u "
               L"taskbar=%p error=%u",
               static_cast<unsigned>(operation),
               destination,
               error);

        // SendMessageTimeout can return after a destination WndProc has started
        // processing but before it returns. In that narrow case the UI-side
        // effect can still complete after this worker resumes. Preserve that
        // distinction so a possible late synthetic enter is always reconciled.
        // See Raymond Chen's documented explanation:
        // https://devblogs.microsoft.com/oldnewthing/20110915-00/?p=9643
        if (error != ERROR_INVALID_WINDOW_HANDLE && IsWindow(destination)) {
            return UiCallResult::kUncertain;
        }
        return UiCallResult::kFailed;
    }

    return result != 0 ? UiCallResult::kSucceeded
                       : UiCallResult::kFailed;
}

struct WorkerTimer {
    UINT_PTR id = 0;
    ULONGLONG dueAt = 0;
};

struct ActivationWorkerState {
    HWND activeTaskbar = nullptr;
    HMONITOR activeTaskbarMonitor = nullptr;
    HMONITOR activationCandidateMonitor = nullptr;
    HMONITOR bandEntryMonitor = nullptr;
    bool armed = true;
    bool uiStateUncertain = false;
    WorkerTimer releaseTimer;
    WorkerTimer retryTimer;
    ULONGLONG activationCandidateSince = 0;
    ULONGLONG lastActivationAttempt = 0;
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

void ScheduleWorkerTimer(UINT delayMs, WorkerTimer& timer) {
    const UINT effectiveDelay = std::max(delayMs, 1U);
    const ULONGLONG requestedDueAt = GetTickCount64() + effectiveDelay;

    if (timer.id) {
        if (timer.dueAt && timer.dueAt <= requestedDueAt) {
            return;
        }
        CancelWorkerTimer(timer);
    }

    timer.id = SetTimer(nullptr, 0, effectiveDelay, nullptr);
    if (!timer.id) {
        Wh_Log(L"Failed to create worker timer: %u", GetLastError());
        return;
    }
    timer.dueAt = requestedDueAt;
}

void ForgetWorkerActiveTaskbar(ActivationWorkerState& state) {
    state.activeTaskbar = nullptr;
    state.activeTaskbarMonitor = nullptr;
    state.armed = true;
    state.uiStateUncertain = false;
}

bool ClearActiveTaskbar(ActivationWorkerState& state,
                        DWORD timeoutMs = 250,
                        bool scheduleRetry = true) {
    CancelWorkerTimer(state.releaseTimer);

    if (!state.activeTaskbar) {
        return true;
    }

    if (!IsSupportedTaskbarWindow(state.activeTaskbar) ||
        !IsCurrentProcessWindow(state.activeTaskbar)) {
        ForgetWorkerActiveTaskbar(state);
        return true;
    }

    const UiCallResult result = SendUiOperation(
        kUiClearSyntheticPointerOver, state.activeTaskbar, timeoutMs);
    if (result != UiCallResult::kSucceeded) {
        if (result == UiCallResult::kUncertain) {
            state.uiStateUncertain = true;
            if (scheduleRetry) {
                ScheduleWorkerTimer(kTransientRetryIntervalMs,
                                    state.retryTimer);
            }
        } else if (scheduleRetry) {
            ScheduleWorkerTimer(kTransientRetryIntervalMs,
                                state.releaseTimer);
        }
        return false;
    }

    ForgetWorkerActiveTaskbar(state);
    return true;
}

void ScheduleRetry(ActivationWorkerState& state, UINT delayMs) {
    ScheduleWorkerTimer(delayMs, state.retryTimer);
}

void ResetActivationCandidate(ActivationWorkerState& state) {
    state.activationCandidateMonitor = nullptr;
    state.activationCandidateSince = 0;
}

void ResetBandEntry(ActivationWorkerState& state) {
    state.bandEntryMonitor = nullptr;
    state.lastActivationAttempt = 0;
    ResetActivationCandidate(state);
}

void ProcessPointerState(ActivationWorkerState& state) {
    POINT pointer{};
    if (!GetCursorPos(&pointer)) {
        return;
    }

    HMONITOR pointerMonitor =
        MonitorFromPoint(pointer, MONITOR_DEFAULTTONULL);
    if (!pointerMonitor) {
        (void)ClearActiveTaskbar(state);
        CancelWorkerTimer(state.retryTimer);
        ResetBandEntry(state);
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
            // A settings change can make an already-owned secondary taskbar
            // ineligible even while the pointer hasn't moved out of its band.
            if (!ClearActiveTaskbar(state)) {
                return;
            }
        } else if (!IsSupportedTaskbarWindow(state.activeTaskbar) ||
                   !IsCurrentProcessWindow(state.activeTaskbar)) {
            CancelWorkerTimer(state.releaseTimer);
            ForgetWorkerActiveTaskbar(state);
        } else {
            const bool sameMonitorBand =
                insideActivationBand &&
                pointerMonitor == state.activeTaskbarMonitor;
            const bool overTaskbar =
                IsPointOverTaskbar(state.activeTaskbar, pointer);

            if (sameMonitorBand || overTaskbar) {
                CancelWorkerTimer(state.releaseTimer);
                // A timed-out UI operation must still be reconciled even if the
                // pointer remains inside the band; otherwise an uncertain no-op
                // can wedge this band entry until the pointer leaves and re-enters.
                if (!state.uiStateUncertain) {
                    CancelWorkerTimer(state.retryTimer);
                }
                return;
            }

            if (state.uiStateUncertain) {
                const int releaseDelay =
                    g_settings.releaseDelayMs.load(std::memory_order_relaxed);
                ScheduleRetry(state,
                              static_cast<UINT>(std::max(releaseDelay, 1)));
                return;
            }

            if (insideActivationBand &&
                pointerMonitor != state.activeTaskbarMonitor) {
                // A direct monitor-to-monitor transition shouldn't pay the old
                // monitor's release delay. Release it now, then let the new
                // monitor apply its own hover delay below.
                if (!ClearActiveTaskbar(state)) {
                    return;
                }
            } else {
                const int releaseDelay =
                    g_settings.releaseDelayMs.load(std::memory_order_relaxed);
                if (releaseDelay > 0) {
                    ScheduleWorkerTimer(static_cast<UINT>(releaseDelay),
                                        state.releaseTimer);
                    return;
                }

                if (!ClearActiveTaskbar(state)) {
                    return;
                }
            }
        }
    }

    if (!insideActivationBand) {
        state.armed = true;
        CancelWorkerTimer(state.retryTimer);
        ResetBandEntry(state);
        return;
    }

    if (state.bandEntryMonitor != pointerMonitor) {
        state.bandEntryMonitor = pointerMonitor;
        state.armed = true;
        state.lastActivationAttempt = 0;
        ResetActivationCandidate(state);
    }

    if (!state.armed) {
        CancelWorkerTimer(state.retryTimer);
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

    const ULONGLONG sinceLastAttempt = now - state.lastActivationAttempt;
    if (sinceLastAttempt < kActivationAttemptThrottleMs) {
        ScheduleRetry(
            state,
            static_cast<UINT>(kActivationAttemptThrottleMs - sinceLastAttempt));
        return;
    }

    state.lastActivationAttempt = now;
    CancelWorkerTimer(state.retryTimer);

    // Cheap, event-driven gates first. These conditions don't need polling:
    // settings changes and taskbar creation already queue a fresh state update.
    if (g_settings.primaryMonitorOnly.load(std::memory_order_relaxed) &&
        (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) == 0) {
        return;
    }

    if (!g_viewCoordinatorSupportAvailable.load(std::memory_order_acquire)) {
        return;
    }

    // Button state can change without cursor movement, so keep a low-frequency
    // retry for this transient condition.
    if (g_settings.ignoreWhileMouseButtonDown.load(std::memory_order_relaxed) &&
        IsAnyMouseButtonDown()) {
        ScheduleRetry(state, kTransientRetryIntervalMs);
        return;
    }

    // ABM_GETSTATE is system-wide and changes through shell/settings events that
    // already wake the worker. Don't poll it while auto-hide is disabled.
    if (g_settings.requireWindowsAutoHide.load(std::memory_order_relaxed) &&
        !IsWindowsAutoHideEnabled()) {
        return;
    }

    // Monitors without a supported taskbar are awakened by taskbar WM_NCCREATE
    // or display changes, so this is also an event-driven no-retry gate.
    HWND taskbarWindow = FindTaskbarForMonitor(pointerMonitor);
    if (!taskbarWindow) {
        return;
    }

    // Fullscreen state can end while the pointer is stationary. One check per
    // second is responsive enough without maintaining a 4 Hz desktop walk.
    if (g_settings.ignoreFullscreenApps.load(std::memory_order_relaxed) &&
        IsFullscreenWindowOnMonitor(pointerMonitor)) {
        ScheduleRetry(state, kTransientRetryIntervalMs);
        return;
    }

    const UiCallResult revealResult =
        SendUiOperation(kUiRevealTaskbar, taskbarWindow, 250);
    if (revealResult == UiCallResult::kSucceeded ||
        revealResult == UiCallResult::kUncertain) {
        state.activeTaskbar = taskbarWindow;
        state.activeTaskbarMonitor = pointerMonitor;
        state.armed = false;
        state.uiStateUncertain = revealResult == UiCallResult::kUncertain;
        ResetActivationCandidate(state);

        if (state.uiStateUncertain) {
            // Don't assume an uncertain reveal succeeded forever. Reconcile it
            // after one second by forcing a confirmed leave, then retry from a
            // clean state if the pointer is still eligible.
            ScheduleRetry(state, kTransientRetryIntervalMs);
        }
        return;
    }

    // A confirmed failure establishes no ownership. Retry at low frequency; if
    // the coordinator is captured in the meantime, its hook also queues an
    // immediate state update.
    ScheduleRetry(state, kTransientRetryIntervalMs);
}

void HandleRetryTimer(ActivationWorkerState& state) {
    CancelWorkerTimer(state.retryTimer);

    if (state.uiStateUncertain && state.activeTaskbar) {
        if (!ClearActiveTaskbar(state, 250, false)) {
            ScheduleRetry(state, kTransientRetryIntervalMs);
            return;
        }
    }

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

    if (ClearActiveTaskbar(state)) {
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

    // Keep unload bounded if Explorer's taskbar UI thread is unresponsive. A
    // single best-effort release is enough here; Wh_ModBeforeUninit also sends
    // one UI-thread-wide cleanup before hooks are removed.
    if (state.activeTaskbar) {
        (void)ClearActiveTaskbar(state, 250, false);
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
                LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged(unsigned __int64,bool,enum winrt::WindowsUdk::UI::Shell::InputDeviceKind))",
            },
            &ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original,
            ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Hook,
            true,  // Checked as a required pair below for a clearer diagnostic.
        },
        {
            {
                LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded(unsigned __int64,bool))",
            },
            &ViewCoordinator_ShouldTaskbarBeExpanded_Original,
            ViewCoordinator_ShouldTaskbarBeExpanded_Hook,
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

    if (!ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original ||
        !ViewCoordinator_ShouldTaskbarBeExpanded_Original) {
        Wh_Log(L"Taskbar reveal path unavailable: required ViewCoordinator "
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
                LR"(const TrayUI::`vftable'{for `ITrayComponentHost'})",
            },
            &g_trayUiVtableITrayComponentHost,
            nullptr,
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
        CSecondaryTray_Unhide_Original && CSecondaryTray_WndProc_Original;
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

    g_uiThreadMessage =
        RegisterWindowMessageW(L"taskbar-activation-threshold.ui");
    if (!g_uiThreadMessage) {
        Wh_Log(L"RegisterWindowMessageW failed for UI operation channel");
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
        // first marshalled UI operation records its WndProc pThis before the
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
        workerThread = g_workerThread.load(std::memory_order_acquire);
    }
    if (workerThread) {
        WaitForSingleObject(workerThread, INFINITE);
    }

    // One bounded cleanup attempt keeps disable/reload responsive even when
    // Explorer's taskbar UI thread is the component that's hung.
    const UiCallResult clearResult =
        SendUiOperation(kUiClearAllSyntheticPointerOver, nullptr, 500);
    if (clearResult != UiCallResult::kSucceeded) {
        Wh_Log(L"Final synthetic pointer-over cleanup wasn't confirmed");
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing");

    HANDLE workerThread = nullptr;
    {
        std::lock_guard<std::mutex> guard(g_workerThreadMutex);
        workerThread = g_workerThread.exchange(nullptr,
                                                std::memory_order_acq_rel);
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
