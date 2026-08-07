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

Unlike approaches that move or inject the pointer, this mod keeps the cursor
stationary and invokes Explorer's native Windows 11 taskbar pointer-over path
on the taskbar UI thread.

The activation threshold is scaled independently for each monitor's DPI.
Cursor movement is detected through Windows accessibility events rather than
continuous polling, minimizing background activity while the pointer is idle.

## Demonstration

![Taskbar Activation Threshold demonstration](https://raw.githubusercontent.com/themagnificentoofman/taskbar-activation-threshold-assets/main/taskbar-activation-threshold-demo.gif)

## Features

- Configurable activation-band height and hover delay
- Per-monitor DPI scaling
- Multi-monitor and secondary-taskbar support
- Adjustable release delay and activation cooldown
- Optional suppression over fullscreen applications and presentation mode
- Optional suppression while a mouse button is held
- Optional restriction to the primary monitor
- No cursor movement, pointer injection, taskbar layout modification, or
  Explorer timer manipulation

## Compatibility

The mod does not modify:

- Taskbar position or geometry
- XAML layout or styles
- Taskbar height or icon size
- Animation duration
- Transparency
- Windows' auto-hide setting
- Taskbar Z-order

It is designed to coexist with taskbar styling, sizing, transparency,
animation, clock, label, icon, and Z-order mods that retain the standard
Windows 11 taskbar and don't replace its native reveal logic.

## Requirements

- Windows 11
- The standard Windows 11 taskbar
- A bottom-positioned taskbar
- **Automatically hide the taskbar** enabled in Windows, unless
  **Require Windows auto-hide** is disabled in the mod settings

## Known limitations

- Top-, left-, right-, detached-, and arbitrarily positioned taskbars are not
  supported.
- ExplorerPatcher's legacy Windows 10 taskbar is not supported.
- Mods that disable, replace, or override the taskbar's native expansion logic
  can conflict with this mod. Keyboard-only and never-show auto-hide modes are
  intentionally incompatible with mouse activation.
- Reveal depends on the undocumented Windows 11
  `ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged` method. If a
  Windows update renames or changes it, the mod will log that the coordinator
  reveal path is unavailable and will need a symbol update.
- Shortly after Explorer starts, the internal taskbar coordinator may not yet
  have been observed by the mod. Activation retries with bounded backoff while
  the pointer remains in the band and begins working as soon as Explorer emits
  a coordinator pointer-over transition.
- Fullscreen suppression ignores click-through, tool, non-activating, cloaked,
  desktop, and taskbar windows. It is intentionally conservative otherwise: a
  qualifying fullscreen window anywhere above the desktop on the target
  monitor can suppress activation even when another app has focus. A
  borderless custom-chrome window that covers the monitor can therefore be
  treated as fullscreen. Windows presentation mode is reported system-wide, so
  it suppresses activation on every monitor while active.
- Cursor tracking uses a system-wide `EVENT_OBJECT_LOCATIONCHANGE` WinEvent
  hook because Windows can't filter that hook to `OBJID_CURSOR` at
  registration time. Non-cursor events are discarded immediately by the
  callback. If the WinEvent hook can't be installed, the mod falls back to a
  150 ms cursor check.

## Attribution and license

The Windows 11 `ViewCoordinator` symbol and UI-thread invocation technique were
informed by the GPL-3.0-licensed **Taskbar auto-hide fine tuning** Windhawk mod
by m417z.

This mod is distributed under the GNU General Public License v3.0.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- activationThresholdPx: 24
  $name: Activation threshold
  $description: >-
    Height of the activation band in logical pixels at 96 DPI. The value is
    scaled independently for each monitor. Recommended range: 12 to 48.
- activationDelayMs: 75
  $name: Hover delay
  $description: >-
    Time in milliseconds that the pointer must remain in the activation band
    before the taskbar is revealed. A short delay prevents accidental reveals
    while moving between vertically stacked monitors.
- releaseDelayMs: 120
  $name: Release delay
  $description: >-
    Delay before the taskbar is allowed to hide again after the pointer leaves
    both the activation band and the visible taskbar.
- activationCooldownMs: 200
  $name: Activation cooldown
  $description: >-
    Minimum time between reveal attempts while the pointer remains in the
    activation band.
- requireWindowsAutoHide: true
  $name: Require Windows auto-hide
  $description: >-
    Only activate when Windows reports that taskbar auto-hide is enabled.
    Disable this when another mod manages auto-hide without enabling the
    Windows setting.
- ignoreFullscreenApps: true
  $name: Ignore fullscreen apps
  $description: >-
    Don't reveal while Windows presentation mode is active or when a
    qualifying fullscreen app is detected on the target monitor. Click-through,
    tool, non-activating, cloaked, desktop, and taskbar windows are ignored. A
    qualifying background fullscreen window on that monitor can still suppress
    activation.
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
#include <cstdlib>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Settings {
    std::atomic<int> activationThresholdPx{24};
    std::atomic<int> activationDelayMs{75};
    std::atomic<int> releaseDelayMs{120};
    std::atomic<int> activationCooldownMs{200};
    std::atomic<bool> requireWindowsAutoHide{true};
    std::atomic<bool> ignoreFullscreenApps{true};
    std::atomic<bool> ignoreWhileMouseButtonDown{true};
    std::atomic<bool> primaryMonitorOnly{false};
};

Settings g_settings;

std::atomic<bool> g_taskbarViewDllLoaded{false};
std::atomic<unsigned> g_settingsGeneration{0};
std::atomic<DWORD> g_workerThreadId{0};
std::atomic<bool> g_cursorUpdatePosted{false};
std::atomic<HWND> g_mainTaskbarWindow{nullptr};

HANDLE g_stopEvent = nullptr;
std::atomic<HANDLE> g_workerThread{nullptr};
std::mutex g_workerThreadMutex;
std::atomic<bool> g_unloading{false};

// Registered in Wh_ModInit rather than during DLL initialization, avoiding
// user32 calls while the loader lock is held.
UINT g_uiThreadMessage = 0;
UINT g_captureTaskbarObjectMessage = 0;
enum UiOperation : WPARAM {
    kUiRevealTaskbar = 1,
    kUiClearSyntheticPointerOver = 2,
    kUiClearAllSyntheticPointerOver = 3,
};

constexpr UINT kWorkerCursorChangedMessage = WM_APP + 1;
constexpr UINT kWorkerSettingsChangedMessage = WM_APP + 2;
constexpr UINT kWorkerNativePointerLeaveMessage = WM_APP + 3;
constexpr UINT kReleaseRetryMs = 250;
constexpr UINT kSuppressionBackoffMaxMs = 5000;
constexpr ULONGLONG kAutoHideCacheDurationMs = 5000;

std::atomic<bool> g_autoHideCacheValid{false};
std::atomic<bool> g_autoHideCacheValue{false};
std::atomic<ULONGLONG> g_autoHideCacheTimestamp{0};

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
void EnsureActivationWorker();
bool HandleTaskbarUiOperation(HWND taskbarWindow,
                              WPARAM operationValue,
                              LRESULT* result);

void InvalidateAutoHideCache() {
    g_autoHideCacheValid.store(false, std::memory_order_release);
}

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

    g_settings.activationCooldownMs.store(
        std::clamp(Wh_GetIntSetting(L"activationCooldownMs"), 0, 10000),
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
    if (g_unloading.load(std::memory_order_acquire) || !g_stopEvent ||
        g_workerThread.load(std::memory_order_acquire)) {
        return;
    }

    std::lock_guard<std::mutex> guard(g_workerThreadMutex);
    if (g_unloading.load(std::memory_order_relaxed) ||
        g_workerThread.load(std::memory_order_relaxed)) {
        return;
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

bool IsTaskbarClass(HWND window) {
    if (!window) {
        return false;
    }

    wchar_t className[64];
    if (!GetClassNameW(window, className, ARRAYSIZE(className))) {
        return false;
    }

    return _wcsicmp(className, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0;
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
    if (!IsCurrentProcessWindow(window)) {
        return false;
    }

    wchar_t className[64];
    return GetClassNameW(window, className, ARRAYSIZE(className)) &&
           _wcsicmp(className, L"Shell_TrayWnd") == 0;
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
            if (IsTaskbarClass(window) && IsCurrentProcessWindow(window)) {
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
        return monitor;
    }

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

    const int taskbarHeight = taskbarRect.bottom - taskbarRect.top;
    const int tolerance = std::max(16, taskbarHeight + 16);
    return std::abs(taskbarRect.bottom - monitorInfo.rcMonitor.bottom) <=
               tolerance ||
           std::abs(taskbarRect.top - monitorInfo.rcMonitor.bottom) <=
               tolerance ||
           (taskbarRect.top < monitorInfo.rcMonitor.bottom &&
            taskbarRect.bottom >= monitorInfo.rcMonitor.bottom - 2);
}

struct FindTaskbarContext {
    HMONITOR monitor;
    HWND result;
};

BOOL CALLBACK FindTaskbarForMonitorCallback(HWND window, LPARAM parameter) {
    if (!IsTaskbarClass(window)) {
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
    const ULONGLONG now = GetTickCount64();
    if (g_autoHideCacheValid.load(std::memory_order_acquire)) {
        const ULONGLONG cachedAt =
            g_autoHideCacheTimestamp.load(std::memory_order_relaxed);
        if (now - cachedAt < kAutoHideCacheDurationMs) {
            return g_autoHideCacheValue.load(std::memory_order_relaxed);
        }
    }

    APPBARDATA appBarData{};
    appBarData.cbSize = sizeof(appBarData);
    const bool enabled =
        (SHAppBarMessage(ABM_GETSTATE, &appBarData) & ABS_AUTOHIDE) != 0;

    g_autoHideCacheValue.store(enabled, std::memory_order_relaxed);
    g_autoHideCacheTimestamp.store(now, std::memory_order_relaxed);
    g_autoHideCacheValid.store(true, std::memory_order_release);
    return enabled;
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
    if (!window) {
        return false;
    }

    wchar_t className[64];
    if (!GetClassNameW(window, className, ARRAYSIZE(className))) {
        return false;
    }

    return _wcsicmp(className, L"Progman") == 0 ||
           _wcsicmp(className, L"WorkerW") == 0;
}

bool IsShellDesktopWindow(HWND window) {
    return HasDesktopWindowClass(window) && IsCurrentProcessWindow(window);
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

bool IsFullscreenCandidateOnMonitor(HWND window, HMONITOR monitor) {
    if (!window || !IsWindowVisible(window) || IsIconic(window) ||
        HasDesktopWindowClass(window) || IsTaskbarClass(window)) {
        return false;
    }

    const LONG_PTR style = GetWindowLongPtrW(window, GWL_STYLE);
    const LONG_PTR exStyle = GetWindowLongPtrW(window, GWL_EXSTYLE);

    // Tool, click-through, wallpaper, and other non-activating surfaces can be
    // full-monitor windows without representing a fullscreen application.
    if ((style & WS_CHILD) != 0 ||
        (exStyle & (WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW |
                    WS_EX_NOACTIVATE)) != 0) {
        return false;
    }

    // Keep the expensive DWM query after the cheap style and monitor checks.
    if (MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST) != monitor ||
        IsWindowCloaked(window)) {
        return false;
    }

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return false;
    }

    RECT windowRect{};
    if (!GetWindowRect(window, &windowRect)) {
        return false;
    }

    constexpr int tolerance = 2;
    const bool windowCoversMonitor =
        windowRect.left <= monitorInfo.rcMonitor.left + tolerance &&
        windowRect.top <= monitorInfo.rcMonitor.top + tolerance &&
        windowRect.right >= monitorInfo.rcMonitor.right - tolerance &&
        windowRect.bottom >= monitorInfo.rcMonitor.bottom - tolerance;
    if (!windowCoversMonitor) {
        return false;
    }

    // Borderless windows are the common fullscreen case. Some Store/UWP apps
    // and browsers retain caption/frame style bits in fullscreen; for those,
    // the client area must cover the monitor after excluding maximized windows.
    if ((style & (WS_CAPTION | WS_THICKFRAME)) == 0) {
        return true;
    }

    // A maximized window fills the work area, which equals the monitor while
    // auto-hide is enabled. Chromium, Electron, Terminal, File Explorer, and
    // other custom-frame apps draw their title bar into the client area and
    // would otherwise be indistinguishable from fullscreen here.
    if (IsZoomed(window)) {
        return false;
    }

    return DoesClientAreaCoverMonitor(window, monitorInfo);
}

void LogFullscreenCandidate(HWND window) {
    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    Wh_Log(L"Fullscreen candidate: hwnd=%p pid=%u", window, processId);
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

    // The notification state is system-wide. Associate exclusive D3D
    // fullscreen with the foreground monitor to avoid suppressing unrelated
    // taskbars. QUNS_BUSY is deliberately not treated as fullscreen because it
    // can mean that Presentation Settings are active without a fullscreen app.
    HWND foregroundWindow = GetForegroundWindow();
    return foregroundWindow &&
           MonitorFromWindow(foregroundWindow,
                             MONITOR_DEFAULTTONEAREST) == monitor;
}

bool IsFullscreenWindowOnMonitor(HMONITOR monitor) {
    // This complements the per-monitor window walk. It catches exclusive D3D,
    // fullscreen Store/UWP apps, and presentation mode, which don't always
    // expose a borderless top-level window that can be recognized reliably.
    if (IsFullscreenOrPresentationNotificationState(monitor)) {
        return true;
    }

    struct Context {
        HMONITOR monitor;
        bool found;
    } context{monitor, false};

    // EnumWindows walks top-level windows from top to bottom in Z order. Once
    // the desktop host is reached, all remaining windows are background or
    // wallpaper surfaces and must not suppress taskbar activation.
    EnumWindows(
        [](HWND window, LPARAM parameter) -> BOOL {
            auto* context = reinterpret_cast<Context*>(parameter);

            if (IsShellDesktopWindow(window)) {
                return FALSE;
            }

            if (IsFullscreenCandidateOnMonitor(window, context->monitor)) {
                LogFullscreenCandidate(window);
                context->found = true;
                return FALSE;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&context));

    return context.found;
}

bool IsPointInsideActivationBand(const POINT& pointer,
                                 HMONITOR monitor,
                                 const MONITORINFO& monitorInfo) {
    const int monitorHeight =
        monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

    UINT dpiX = 96;
    UINT dpiY = 96;
    if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        dpiY = 96;
    }

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
    // Keep a conservative bound to avoid walking arbitrary memory if Microsoft
    // changes the object layout.
    auto** candidate = reinterpret_cast<void**>(object);

    for (size_t i = 0; i < 256; i++, candidate++) {
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

LRESULT WINAPI TrayUI_WndProc_Hook(
    void* trayUi,
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam,
    bool* handled) {
    if (message == g_uiThreadMessage) {
        LRESULT result = 0;
        if (HandleTaskbarUiOperation(window, wParam, &result)) {
            if (handled) {
                *handled = true;
            }
            return result;
        }
    }

    if (message == WM_NCCREATE) {
        g_trayUiWndProcObjects[window] = trayUi;
        g_mainTaskbarWindow.store(window, std::memory_order_relaxed);
        Wh_Log(L"Captured TrayUI object for taskbar %p", window);
        EnsureActivationWorker();
    } else if (message == g_captureTaskbarObjectMessage) {
        g_trayUiWndProcObjects[window] = trayUi;
        g_mainTaskbarWindow.store(window, std::memory_order_relaxed);
        Wh_Log(L"Captured TrayUI object for taskbar %p", window);
        EnsureActivationWorker();
        if (handled) {
            *handled = true;
        }
        return 0;
    } else if (message == WM_SETTINGCHANGE) {
        InvalidateAutoHideCache();
    } else if (message == WM_NCDESTROY) {
        g_trayUiWndProcObjects.erase(window);
        ForgetTaskbarState(window);
        if (g_mainTaskbarWindow.load(std::memory_order_relaxed) == window) {
            g_mainTaskbarWindow.store(nullptr, std::memory_order_relaxed);
        }
    }

    return TrayUI_WndProc_Original(
        trayUi,
        window,
        message,
        wParam,
        lParam,
        handled);
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
        LRESULT result = 0;
        if (HandleTaskbarUiOperation(window, wParam, &result)) {
            return result;
        }
    }

    if (message == WM_NCCREATE) {
        g_secondaryTrayObjects[window] = secondaryTray;
        Wh_Log(L"Captured CSecondaryTray object for taskbar %p", window);
        EnsureActivationWorker();
    } else if (message == g_captureTaskbarObjectMessage) {
        g_secondaryTrayObjects[window] = secondaryTray;
        Wh_Log(L"Captured CSecondaryTray object for taskbar %p", window);
        EnsureActivationWorker();
        return 0;
    } else if (message == WM_SETTINGCHANGE) {
        InvalidateAutoHideCache();
    } else if (message == WM_NCDESTROY) {
        g_secondaryTrayObjects.erase(window);
        ForgetTaskbarState(window);
    }

    return CSecondaryTray_WndProc_Original(
        secondaryTray,
        window,
        message,
        wParam,
        lParam);
}

bool InvokeNativeShellUnhideOnUiThread(HWND taskbarWindow) {
    if (!taskbarWindow || !IsWindow(taskbarWindow)) {
        return false;
    }

    wchar_t className[64];
    if (!GetClassNameW(taskbarWindow,
                       className,
                       ARRAYSIZE(className))) {
        return false;
    }

    if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
        auto it = g_trayUiWndProcObjects.find(taskbarWindow);

        if (it == g_trayUiWndProcObjects.end() ||
            !TrayUI_Unhide_Original ||
            !g_trayUiVtableITrayComponentHost) {
            Wh_Log(L"Native main-taskbar unhide unavailable: "
                   L"object=%d vtable=%d function=%d",
                   it != g_trayUiWndProcObjects.end(),
                   g_trayUiVtableITrayComponentHost != nullptr,
                   TrayUI_Unhide_Original != nullptr);
            return false;
        }

        void* trayComponentHost =
            QueryViaVtable(it->second,
                           g_trayUiVtableITrayComponentHost);

        if (!trayComponentHost) {
            Wh_Log(L"Couldn't locate ITrayComponentHost for taskbar %p",
                   taskbarWindow);
            return false;
        }

        TrayUI_Unhide_Original(trayComponentHost, 0, 0);
        Wh_Log(L"Called TrayUI::Unhide for taskbar %p", taskbarWindow);
        return true;
    }

    if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        auto it = g_secondaryTrayObjects.find(taskbarWindow);

        if (it == g_secondaryTrayObjects.end() ||
            !CSecondaryTray_Unhide_Original) {
            Wh_Log(L"Native secondary-taskbar unhide unavailable: "
                   L"object=%d function=%d",
                   it != g_secondaryTrayObjects.end(),
                   CSecondaryTray_Unhide_Original != nullptr);
            return false;
        }

        CSecondaryTray_Unhide_Original(it->second, 0, 0);
        Wh_Log(L"Called CSecondaryTray::_Unhide for taskbar %p",
               taskbarWindow);
        return true;
    }

    return false;
}

void RememberViewCoordinator(HWND taskbarWindow, void* viewCoordinator) {
    if (!taskbarWindow || !viewCoordinator) {
        return;
    }

    g_viewCoordinators[taskbarWindow] = viewCoordinator;
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

void WINAPI
ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Hook(
    void* viewCoordinator,
    HWND taskbarWindow,
    bool isPointerOver,
    int inputDeviceKind) {
    RememberViewCoordinator(taskbarWindow, viewCoordinator);

    ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original(
        viewCoordinator,
        taskbarWindow,
        isPointerOver,
        inputDeviceKind);

    // Calls made by this mod invoke the original function directly, so a false
    // value reaching this hook is Explorer's own pointer-leave transition. If
    // it superseded our synthetic enter, relinquish UI-thread ownership and
    // tell the worker to re-arm rather than waiting for a redundant release.
    if (!isPointerOver &&
        g_syntheticPointerOverTaskbars.erase(taskbarWindow) != 0) {
        Wh_Log(L"Explorer cleared synthetic pointer-over for taskbar %p",
               taskbarWindow);

        const DWORD workerThreadId =
            g_workerThreadId.load(std::memory_order_acquire);
        if (workerThreadId &&
            !PostThreadMessageW(
                workerThreadId,
                kWorkerNativePointerLeaveMessage,
                reinterpret_cast<WPARAM>(taskbarWindow),
                0)) {
            Wh_Log(L"Couldn't notify activation worker about Explorer's "
                   L"pointer leave for taskbar %p: %u",
                   taskbarWindow,
                   GetLastError());
        }
    }
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
    // Establish coordinator ownership first. If no coordinator has been
    // captured yet, the worker retries with bounded backoff.
    const bool coordinatorExpanded =
        SetSyntheticPointerOverOnUiThread(taskbarWindow, true);

    bool shellWindowRevealed = false;
    if (coordinatorExpanded) {
        // Keep the native shell unhide call for the physical Shell_TrayWnd /
        // Shell_SecondaryTrayWnd slide, but leave Explorer's Z-order policy
        // untouched.
        shellWindowRevealed =
            InvokeNativeShellUnhideOnUiThread(taskbarWindow);
    }

    Wh_Log(L"Reveal result for taskbar %p: shell=%d coordinator=%d",
           taskbarWindow,
           shellWindowRevealed,
           coordinatorExpanded);

    return coordinatorExpanded;
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
    if (!result) {
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

enum class UiOperationResult {
    kNotSent,
    kCompletedFailure,
    kCompletedSuccess,
    kIndeterminate,
};

bool UiOperationSucceeded(UiOperationResult result) {
    return result == UiOperationResult::kCompletedSuccess;
}

bool UiOperationMayHaveRun(UiOperationResult result) {
    return result == UiOperationResult::kCompletedSuccess ||
           result == UiOperationResult::kIndeterminate;
}

UiOperationResult SendUiOperation(UiOperation operation,
                                  HWND taskbarWindow,
                                  DWORD timeoutMs = 250) {
    HWND destination = taskbarWindow;
    if (operation == kUiClearAllSyntheticPointerOver) {
        destination = FindAnyTaskbarWindow();
    }

    if (!destination || !IsTaskbarClass(destination) ||
        !IsCurrentProcessWindow(destination)) {
        return UiOperationResult::kNotSent;
    }

    DWORD_PTR result = 0;
    SetLastError(ERROR_SUCCESS);
    if (!SendMessageTimeoutW(
            destination,
            g_uiThreadMessage,
            operation,
            0,
            SMTO_ABORTIFHUNG | SMTO_BLOCK,
            timeoutMs,
            &result)) {
        const DWORD error = GetLastError();
        Wh_Log(L"Taskbar UI operation timed out or failed: operation=%u "
               L"taskbar=%p error=%u",
               static_cast<unsigned>(operation),
               destination,
               error);

        // A timeout doesn't cancel a cross-thread send. Treat it as possibly
        // delivered so the worker retains ownership and still sends the
        // matching release. A vanished window is the only definite no-send.
        if (error == ERROR_INVALID_WINDOW_HANDLE || !IsWindow(destination)) {
            return UiOperationResult::kNotSent;
        }
        return UiOperationResult::kIndeterminate;
    }

    return result != 0 ? UiOperationResult::kCompletedSuccess
                       : UiOperationResult::kCompletedFailure;
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
    WorkerTimer releaseTimer;
    WorkerTimer retryTimer;
    bool releaseRetryPending = false;
    ULONGLONG activationCandidateSince = 0;
    ULONGLONG lastActivationCheck = 0;
    unsigned suppressionFailures = 0;
    unsigned appliedSettingsGeneration = 0;
};

void CancelWorkerTimer(WorkerTimer& timer) {
    if (!timer.id) {
        timer.dueAt = 0;
        return;
    }

    KillTimer(nullptr, timer.id);
    timer.id = 0;
    timer.dueAt = 0;
}

void ScheduleWorkerTimer(UINT delayMs, WorkerTimer& timer) {
    const UINT effectiveDelay = std::max(delayMs, 1U);
    const ULONGLONG requestedDueAt = GetTickCount64() + effectiveDelay;

    if (timer.id) {
        // Keep an existing timer when it will fire no later than the new
        // request. If the new deadline is earlier, replace the timer so a
        // long suppression backoff can't delay a short hover/cooldown retry.
        if (timer.dueAt && timer.dueAt <= requestedDueAt) {
            return;
        }
        CancelWorkerTimer(timer);
    }

    // With hWnd == nullptr, Windows allocates and returns the timer ID. The
    // caller stores that returned value and compares WM_TIMER against it.
    timer.id = SetTimer(nullptr, 0, effectiveDelay, nullptr);
    if (!timer.id) {
        timer.dueAt = 0;
        Wh_Log(L"Failed to create worker timer: %u", GetLastError());
        return;
    }

    timer.dueAt = requestedDueAt;
}

bool ClearActiveTaskbar(ActivationWorkerState& state,
                        DWORD timeoutMs = 250,
                        bool scheduleRetry = true) {
    CancelWorkerTimer(state.releaseTimer);

    if (state.activeTaskbar) {
        const UiOperationResult clearResult =
            SendUiOperation(kUiClearSyntheticPointerOver,
                            state.activeTaskbar,
                            timeoutMs);

        if (clearResult == UiOperationResult::kNotSent) {
            // The HWND is no longer a taskbar in this Explorer process. Its
            // coordinator and synthetic state were already discarded by the
            // taskbar WM_NCDESTROY hook, so retrying could loop forever if the
            // numeric handle was recycled for another window.
            Wh_Log(L"Release target is no longer a taskbar: %p",
                   state.activeTaskbar);
        } else if (!UiOperationSucceeded(clearResult)) {
            // Completed failure or timeout means the taskbar might still own
            // synthetic state. Keep worker ownership and retry rather than
            // allowing Explorer's coordinator to remain forced open.
            if (scheduleRetry) {
                state.releaseRetryPending = true;
                ScheduleWorkerTimer(kReleaseRetryMs,
                                    state.releaseTimer);
            }
            return false;
        }
    }

    state.releaseRetryPending = false;
    state.activeTaskbar = nullptr;
    state.activeTaskbarMonitor = nullptr;
    state.armed = true;
    return true;
}

void ScheduleRetry(ActivationWorkerState& state, UINT delayMs) {
    ScheduleWorkerTimer(delayMs, state.retryTimer);
}

UINT AdvanceSuppressionBackoff(ActivationWorkerState& state,
                                   int cooldownMs) {
    state.suppressionFailures =
        std::min(state.suppressionFailures + 1, 8U);
    const UINT base = static_cast<UINT>(std::max(cooldownMs, 200));
    const unsigned shift = std::min(state.suppressionFailures - 1, 4U);
    return std::min(base << shift, kSuppressionBackoffMaxMs);
}

void ScheduleSuppressedRetry(ActivationWorkerState& state,
                             int cooldownMs) {
    ScheduleRetry(state, AdvanceSuppressionBackoff(state, cooldownMs));
}

void ResetActivationCandidate(ActivationWorkerState& state) {
    state.activationCandidateMonitor = nullptr;
    state.activationCandidateSince = 0;
}

void ResetBandEntry(ActivationWorkerState& state) {
    state.bandEntryMonitor = nullptr;
    ResetActivationCandidate(state);
}

void ProcessPointerState(ActivationWorkerState& state) {
    const unsigned settingsGeneration =
        g_settingsGeneration.load(std::memory_order_acquire);
    if (state.appliedSettingsGeneration != settingsGeneration) {
        CancelWorkerTimer(state.retryTimer);
        if (!ClearActiveTaskbar(state)) {
            // Keep the old generation pending. Once the release retry succeeds,
            // the complete settings reset will run instead of being forgotten.
            return;
        }

        ResetBandEntry(state);
        state.lastActivationCheck = 0;
        state.suppressionFailures = 0;
        state.appliedSettingsGeneration = settingsGeneration;
    }

    POINT pointer{};
    if (!GetCursorPos(&pointer)) {
        return;
    }

    HMONITOR pointerMonitor =
        MonitorFromPoint(pointer, MONITOR_DEFAULTTONULL);
    if (!pointerMonitor) {
        if (!ClearActiveTaskbar(state)) {
            return;
        }
        CancelWorkerTimer(state.retryTimer);
        ResetBandEntry(state);
        state.suppressionFailures = 0;
        return;
    }

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(pointerMonitor, &monitorInfo)) {
        return;
    }

    const ULONGLONG now = GetTickCount64();
    const bool insideActivationBand =
        IsPointInsideActivationBand(pointer, pointerMonitor, monitorInfo);

    if (state.activeTaskbar) {
        if (state.releaseRetryPending) {
            if (!ClearActiveTaskbar(state)) {
                return;
            }
        } else if (!IsWindow(state.activeTaskbar)) {
            CancelWorkerTimer(state.releaseTimer);
            state.releaseRetryPending = false;
            state.activeTaskbar = nullptr;
            state.activeTaskbarMonitor = nullptr;
            state.armed = true;
        } else {
            const bool overVisibleTaskbar =
                IsPointOverTaskbar(state.activeTaskbar, pointer);

            if ((insideActivationBand &&
                 pointerMonitor == state.activeTaskbarMonitor) ||
                overVisibleTaskbar) {
                CancelWorkerTimer(state.releaseTimer);
                return;
            }

            const int releaseDelay =
                g_settings.releaseDelayMs.load(std::memory_order_relaxed);
            if (releaseDelay == 0) {
                if (!ClearActiveTaskbar(state)) {
                    return;
                }
            } else {
                state.releaseRetryPending = false;
                ScheduleWorkerTimer(static_cast<UINT>(releaseDelay),
                                    state.releaseTimer);
                return;
            }
        }
    }

    if (!insideActivationBand) {
        state.armed = true;
        state.suppressionFailures = 0;
        CancelWorkerTimer(state.retryTimer);
        ResetBandEntry(state);
        return;
    }

    // Entering the activation band on another monitor is a new activation
    // opportunity even if the pointer never left the combined bottom edge.
    if (state.bandEntryMonitor != pointerMonitor) {
        state.bandEntryMonitor = pointerMonitor;
        state.armed = true;
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
        const ULONGLONG candidateElapsed =
            now - state.activationCandidateSince;
        if (candidateElapsed < static_cast<ULONGLONG>(activationDelay)) {
            ScheduleRetry(
                state,
                static_cast<UINT>(
                    static_cast<ULONGLONG>(activationDelay) -
                    candidateElapsed));
            return;
        }
    }

    const int cooldown =
        g_settings.activationCooldownMs.load(std::memory_order_relaxed);
    const ULONGLONG elapsed = now - state.lastActivationCheck;
    if (elapsed < static_cast<ULONGLONG>(cooldown)) {
        ScheduleRetry(
            state,
            static_cast<UINT>(
                static_cast<ULONGLONG>(cooldown) - elapsed));
        return;
    }

    state.lastActivationCheck = now;
    CancelWorkerTimer(state.retryTimer);

    Wh_Log(L"Pointer remained in activation band: x=%d y=%d bottom=%d",
           pointer.x,
           pointer.y,
           monitorInfo.rcMonitor.bottom);

    if (g_settings.primaryMonitorOnly.load(std::memory_order_relaxed) &&
        (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) == 0) {
        ScheduleSuppressedRetry(state, cooldown);
        return;
    }

    if (g_settings.requireWindowsAutoHide.load(
            std::memory_order_relaxed) &&
        !IsWindowsAutoHideEnabled()) {
        ScheduleSuppressedRetry(state, cooldown);
        return;
    }

    if (g_settings.ignoreWhileMouseButtonDown.load(
            std::memory_order_relaxed) &&
        IsAnyMouseButtonDown()) {
        ScheduleSuppressedRetry(state, cooldown);
        return;
    }

    if (g_settings.ignoreFullscreenApps.load(std::memory_order_relaxed) &&
        IsFullscreenWindowOnMonitor(pointerMonitor)) {
        Wh_Log(L"Activation suppressed: fullscreen app detected");
        ScheduleSuppressedRetry(state, cooldown);
        return;
    }

    HWND taskbarWindow = FindTaskbarForMonitor(pointerMonitor);
    if (!taskbarWindow) {
        Wh_Log(L"Activation suppressed: no bottom taskbar found");
        ScheduleSuppressedRetry(state, cooldown);
        return;
    }

    const UiOperationResult revealResult =
        SendUiOperation(kUiRevealTaskbar, taskbarWindow, 250);
    if (UiOperationMayHaveRun(revealResult)) {
        state.suppressionFailures = 0;
        if (revealResult == UiOperationResult::kIndeterminate) {
            Wh_Log(L"Taskbar reveal timed out; assuming coordinator ownership "
                   L"until the matching release succeeds: %p",
                   taskbarWindow);
        } else {
            Wh_Log(L"Activated taskbar %p through native "
                   L"shell/coordinator path",
                   taskbarWindow);
        }

        ResetActivationCandidate(state);
        state.activeTaskbar = taskbarWindow;
        state.activeTaskbarMonitor = pointerMonitor;
        state.armed = false;
        return;
    }

    Wh_Log(L"Threshold entered, but coordinator reveal is not available yet");
    ScheduleSuppressedRetry(state, cooldown);
}

void HandleNativePointerLeave(ActivationWorkerState& state,
                              HWND taskbarWindow) {
    if (!taskbarWindow || state.activeTaskbar != taskbarWindow) {
        return;
    }

    CancelWorkerTimer(state.releaseTimer);
    CancelWorkerTimer(state.retryTimer);
    state.releaseRetryPending = false;
    state.activeTaskbar = nullptr;
    state.activeTaskbarMonitor = nullptr;
    state.armed = true;
    state.suppressionFailures = 0;
    ResetActivationCandidate(state);

    Wh_Log(L"Re-arming after Explorer pointer leave for taskbar %p",
           taskbarWindow);
    ProcessPointerState(state);
}

void HandleReleaseTimer(ActivationWorkerState& state) {
    CancelWorkerTimer(state.releaseTimer);

    if (!state.activeTaskbar) {
        return;
    }

    if (state.releaseRetryPending) {
        if (!ClearActiveTaskbar(state)) {
            return;
        }
        ProcessPointerState(state);
        return;
    }

    POINT pointer{};
    bool keepActive = false;

    if (GetCursorPos(&pointer) && IsWindow(state.activeTaskbar)) {
        HMONITOR pointerMonitor =
            MonitorFromPoint(pointer, MONITOR_DEFAULTTONULL);
        MONITORINFO monitorInfo{};
        monitorInfo.cbSize = sizeof(monitorInfo);

        if (pointerMonitor &&
            GetMonitorInfoW(pointerMonitor, &monitorInfo)) {
            const bool insideActivationBand =
                IsPointInsideActivationBand(
                    pointer,
                    pointerMonitor,
                    monitorInfo);
            const bool overVisibleTaskbar =
                IsPointOverTaskbar(state.activeTaskbar, pointer);

            keepActive =
                (insideActivationBand &&
                 pointerMonitor == state.activeTaskbarMonitor) ||
                overVisibleTaskbar;
        }
    }

    if (keepActive) {
        return;
    }

    if (!ClearActiveTaskbar(state)) {
        return;
    }
    ProcessPointerState(state);
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

    DWORD workerThreadId =
        g_workerThreadId.load(std::memory_order_relaxed);
    if (!workerThreadId ||
        g_cursorUpdatePosted.exchange(true, std::memory_order_relaxed)) {
        return;
    }

    if (!PostThreadMessageW(workerThreadId,
                            kWorkerCursorChangedMessage,
                            0,
                            0)) {
        g_cursorUpdatePosted.store(false, std::memory_order_relaxed);
    }
}

DWORD WINAPI ActivationWorkerThread(LPVOID) {
    // Force creation of this thread's message queue before publishing its ID.
    MSG message{};
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_workerThreadId.store(GetCurrentThreadId(), std::memory_order_relaxed);

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
    }

    ActivationWorkerState state;
    ProcessPointerState(state);

    bool stop = false;
    while (!stop) {
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
            Wh_Log(L"Activation worker wait failed: %u", GetLastError());
            break;
        }

        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT) {
                stop = true;
                break;
            }

            switch (message.message) {
                case kWorkerCursorChangedMessage:
                    g_cursorUpdatePosted.store(false,
                                               std::memory_order_relaxed);
                    ProcessPointerState(state);
                    break;

                case kWorkerSettingsChangedMessage:
                    ProcessPointerState(state);
                    break;

                case kWorkerNativePointerLeaveMessage:
                    HandleNativePointerLeave(
                        state,
                        reinterpret_cast<HWND>(message.wParam));
                    break;

                case WM_TIMER:
                    if (state.releaseTimer.id &&
                        message.wParam == state.releaseTimer.id) {
                        HandleReleaseTimer(state);
                    } else if (state.retryTimer.id &&
                               message.wParam == state.retryTimer.id) {
                        CancelWorkerTimer(state.retryTimer);
                        ProcessPointerState(state);
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

    // Keep unload bounded if Explorer's taskbar UI thread is unresponsive. A
    // single best-effort release is sufficient; an indeterminate send may still
    // be delivered later, and a real pointer transition repairs stale state.
    if (state.activeTaskbar) {
        (void)ClearActiveTaskbar(state, 250, false);
    }

    if (cursorHook) {
        UnhookWinEvent(cursorHook);
    } else if (fallbackPollTimerId) {
        KillTimer(nullptr, fallbackPollTimerId);
    }

    g_cursorUpdatePosted.store(false, std::memory_order_relaxed);
    g_workerThreadId.store(0, std::memory_order_relaxed);
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
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(
            module,
            symbolHooks,
            ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"Failed to hook Windows 11 ViewCoordinator symbols");
        return false;
    }

    if (!ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original) {
        Wh_Log(L"Taskbar reveal path unavailable: required ViewCoordinator "
               L"symbol needs updating");
        return false;
    }

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
            true,
        },
        {
            {
                LR"(public: virtual void __cdecl TrayUI::Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))",
            },
            &TrayUI_Unhide_Original,
            nullptr,
            true,
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
        },
    };

    if (!WindhawkUtils::HookSymbols(
            taskbarModule,
            symbolHooks,
            ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"Failed to hook native taskbar symbols");
        return false;
    }

    return true;
}

void CaptureExistingTaskbarObjects() {
    EnumWindows(
        [](HWND window, LPARAM) -> BOOL {
            if (!IsTaskbarClass(window)) {
                return TRUE;
            }

            DWORD processId = 0;
            GetWindowThreadProcessId(window, &processId);

            if (processId == GetCurrentProcessId()) {
                DWORD_PTR result = 0;
                SetLastError(ERROR_SUCCESS);
                if (!SendMessageTimeoutW(
                        window,
                        g_captureTaskbarObjectMessage,
                        0,
                        0,
                        SMTO_ABORTIFHUNG | SMTO_BLOCK,
                        500,
                        &result)) {
                    Wh_Log(L"Taskbar object capture timed out or failed for "
                           L"%p: %u",
                           window,
                           GetLastError());
                }
            }

            return TRUE;
        },
        0);
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
    if (!module || g_taskbarViewDllLoaded.load(
                       std::memory_order_relaxed)) {
        return;
    }

    if (GetTaskbarViewModuleHandle() != module) {
        return;
    }

    if (g_taskbarViewDllLoaded.exchange(
            true,
            std::memory_order_relaxed)) {
        return;
    }

    Wh_Log(L"Loaded taskbar view module: %s",
           libraryFileName ? libraryFileName : L"(unknown)");

    if (HookTaskbarViewSymbols(module)) {
        Wh_ApplyHookOperations();
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
    InvalidateAutoHideCache();

    g_uiThreadMessage =
        RegisterWindowMessageW(L"taskbar-activation-threshold.ui");
    g_captureTaskbarObjectMessage =
        RegisterWindowMessageW(L"taskbar-activation-threshold.capture");
    if (!g_uiThreadMessage || !g_captureTaskbarObjectMessage) {
        Wh_Log(L"RegisterWindowMessageW failed: ui=%u capture=%u",
               g_uiThreadMessage,
               g_captureTaskbarObjectMessage);
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
            std::memory_order_relaxed);
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

        WindhawkUtils::SetFunctionHook(
            loadLibraryExW,
            LoadLibraryExW_Hook,
            &LoadLibraryExW_Original);
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
    // Explorer processes skip the capture pass and never create the activation
    // worker or its system-wide WinEvent hook.
    WNDCLASSW taskbarClass{};
    if (GetClassInfoW(GetModuleHandleW(nullptr),
                      L"Shell_TrayWnd",
                      &taskbarClass)) {
        CaptureExistingTaskbarObjects();
    } else {
        Wh_Log(L"No Shell_TrayWnd class in this Explorer process; worker not "
               L"started");
    }

    // Retry in case the taskbar view module loaded between Wh_ModInit and hook
    // application.
    if (!g_taskbarViewDllLoaded.load(std::memory_order_relaxed)) {
        if (HMODULE taskbarViewModule =
                GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(
                    true,
                    std::memory_order_relaxed)) {
                Wh_Log(L"Found taskbar view module after initialization");

                if (HookTaskbarViewSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
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
    const UiOperationResult clearResult =
        SendUiOperation(kUiClearAllSyntheticPointerOver, nullptr, 500);
    if (!UiOperationSucceeded(clearResult) &&
        clearResult != UiOperationResult::kNotSent) {
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

    InvalidateAutoHideCache();
    g_viewCoordinators.clear();
    g_trayUiWndProcObjects.clear();
    g_secondaryTrayObjects.clear();
    g_syntheticPointerOverTaskbars.clear();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");

    LoadSettings();
    g_settingsGeneration.fetch_add(1, std::memory_order_release);

    DWORD workerThreadId =
        g_workerThreadId.load(std::memory_order_relaxed);
    if (workerThreadId) {
        PostThreadMessageW(workerThreadId,
                           kWorkerSettingsChangedMessage,
                           0,
                           0);
    }
}
