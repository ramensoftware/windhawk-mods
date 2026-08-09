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

Reveal the auto-hidden Windows 11 taskbar when the pointer enters a configurable
band above the bottom edge of the screen. The pointer itself is never moved.

![Taskbar Activation Threshold demonstration](https://raw.githubusercontent.com/themagnificentoofman/taskbar-activation-threshold-assets/main/taskbar-activation-threshold-demo.gif)

## Behavior

After the hover delay, the mod asks Explorer to keep the taskbar shown and uses
Explorer's native unhide path. The taskbar stays available while the pointer is
over the visible taskbar or inside the lower part of the activation band. After
the pointer leaves, the release delay runs and Explorer resumes its normal
hide policy. While the hold request is active, Explorer hide requests are
intentionally deferred; leaving the hold area restores stock behavior. Open
Start/tray flyouts are respected: release doesn't force the hide timer when
Explorer still wants the taskbar expanded.

The configured threshold is used for the initial trigger. Once the taskbar is
shown, the hold-open band is capped to the taskbar's own height, so very large
thresholds don't create a floating hold-open strip above the taskbar.

Multi-monitor and secondary taskbars are supported. Moving directly from one
monitor's activation band to another releases the old taskbar immediately and
starts the new monitor's hover delay.

## Compatibility

- Windows 11 with the standard bottom-positioned taskbar.
- **Automatically hide the taskbar** should normally be enabled. The optional
  check uses Windows' system-wide auto-hide flag, so per-monitor auto-hide mods
  can't be distinguished perfectly.
- The mod doesn't move the cursor, synthesize taskbar pointer-over state, change
  taskbar layout/XAML, alter animation speed, or force taskbar Z-order.
  Explorer's native unhide path remains responsible for layering.
- ExplorerPatcher's legacy Windows 10 taskbar and substantially repositioned,
  detached, floating, top, left, or right taskbars aren't supported.
- Fullscreen suppression applies before reveal to Windows presentation/exclusive
  fullscreen state and to the foreground fullscreen app on the target monitor.
  A background fullscreen app doesn't block the band. If an app becomes
  fullscreen after the taskbar is already being held open, the taskbar is
  released when the pointer leaves the hold area.
- **Taskbar auto-hide custom activation area** controls where along the edge a
  native trigger is accepted; this mod adds trigger depth above the edge. These
  are closely related catalog features and may be better consolidated if the
  maintainer prefers. Mods that replace Explorer's hide/show policy can still
  interact with this mod's keep-shown hooks.

## Attribution and license

The Windows 11 taskbar symbols and keep-shown pattern are based on GPL-3.0
Windhawk taskbar mods by m417z, especially **Taskbar auto-hide when maximized**,
**Taskbar auto-hide fine tuning**, and **Taskbar auto-hide custom activation
area**.

This mod is distributed under the GNU General Public License v3.0.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- activationThresholdPx: 12
  $name: Activation threshold
  $description: >-
    Height of the activation band in logical pixels at 96 DPI, scaled per
    monitor. 8-24 is a practical range for most taskbars.
- activationDelayMs: 75
  $name: Hover delay
  $description: >-
    Time the pointer must remain in the activation band before revealing the
    taskbar. A short delay helps prevent accidental reveals.
- releaseDelayMs: 120
  $name: Release delay
  $description: >-
    Delay before the mod releases its keep-shown request after the pointer leaves
    both the visible taskbar and the active hold area.
- requireWindowsAutoHide: true
  $name: Require Windows auto-hide
  $description: >-
    Only activate when Windows' system-wide auto-hide flag is enabled. Disable
    this if another mod manages auto-hide; per-monitor states can't be detected
    through this Windows flag.
- ignoreFullscreenApps: true
  $name: Ignore fullscreen apps
  $description: >-
    Don't reveal during Windows presentation/exclusive-fullscreen state or when
    the foreground app on the target monitor is fullscreen.
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

constexpr UINT kWorkerCursorChangedMessage = WM_APP + 1;
constexpr UINT kTransientRetryMs = 1000;
constexpr UINT_PTR kTrayUITimerHide = 2;
constexpr size_t kMaxInterfaceSlots = 20;

std::atomic<bool> g_unloading{false};
std::atomic<bool> g_taskbarViewDllLoaded{false};
std::atomic<bool> g_viewCoordinatorSupportAvailable{false};
std::atomic<bool> g_secondaryTaskbarSupportAvailable{false};
std::atomic<DWORD> g_workerThreadId{0};
std::atomic<bool> g_cursorUpdatePosted{false};
std::atomic<bool> g_workerStateDirty{true};
std::atomic<HWND> g_mainTaskbarWindow{nullptr};
std::atomic<HWND> g_desiredKeptShownTaskbar{nullptr};

HANDLE g_stopEvent = nullptr;
HANDLE g_workerThread = nullptr;  // Guarded by g_workerThreadMutex.
std::mutex g_workerThreadMutex;
UINT g_updateTaskbarStateMessage = 0;

// Taskbar UI-thread state. Explorer's taskbar hooks run on the shell taskbar
// thread, matching the convention used by the maintained taskbar mods.
std::unordered_map<HWND, void*> g_viewCoordinators;
std::unordered_map<void*, HWND> g_taskbarsKeptShown;

void* g_trayUiVtableIInspectable = nullptr;
void* g_trayUiVtableITrayComponentHost = nullptr;

using TrayUI_Hide_t = void(WINAPI*)(void* trayUiInspectable);
using CSecondaryTray_AutoHide_t =
    void(WINAPI*)(void* secondaryTray, bool parameter);
using TrayUI_Unhide_t =
    void(WINAPI*)(void* trayComponentHost, int trayUnhideFlags, int request);
using CSecondaryTray_Unhide_t =
    void(WINAPI*)(void* secondaryTray, int trayUnhideFlags, int request);
using TrayUI_WndProc_t =
    LRESULT(WINAPI*)(void* trayUi,
                     HWND window,
                     UINT message,
                     WPARAM wParam,
                     LPARAM lParam,
                     bool* handled);
using CSecondaryTray_WndProc_t =
    LRESULT(WINAPI*)(void* secondaryTray,
                     HWND window,
                     UINT message,
                     WPARAM wParam,
                     LPARAM lParam);
using ViewCoordinator_ShouldTaskbarBeExpanded_t =
    bool(WINAPI*)(void* viewCoordinator, HWND taskbarWindow, bool expanded);
using ViewCoordinator_UpdateIsExpanded_t =
    void(WINAPI*)(void* viewCoordinator, HWND taskbarWindow, int reason);

TrayUI_Hide_t TrayUI_Hide_Original;
CSecondaryTray_AutoHide_t CSecondaryTray_AutoHide_Original;
TrayUI_Unhide_t TrayUI_Unhide_Original;
CSecondaryTray_Unhide_t CSecondaryTray_Unhide_Original;
TrayUI_WndProc_t TrayUI_WndProc_Original;
CSecondaryTray_WndProc_t CSecondaryTray_WndProc_Original;
ViewCoordinator_ShouldTaskbarBeExpanded_t
    ViewCoordinator_ShouldTaskbarBeExpanded_Original;
ViewCoordinator_UpdateIsExpanded_t ViewCoordinator_UpdateIsExpanded_Original;

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

DWORD WINAPI ActivationWorkerThread(LPVOID);

void LoadSettings() {
    g_settings.activationThresholdPx =
        std::clamp(Wh_GetIntSetting(L"activationThresholdPx"), 1, 500);
    g_settings.activationDelayMs =
        std::clamp(Wh_GetIntSetting(L"activationDelayMs"), 0, 1000);
    g_settings.releaseDelayMs =
        std::clamp(Wh_GetIntSetting(L"releaseDelayMs"), 0, 5000);
    g_settings.requireWindowsAutoHide =
        Wh_GetIntSetting(L"requireWindowsAutoHide") != 0;
    g_settings.ignoreFullscreenApps =
        Wh_GetIntSetting(L"ignoreFullscreenApps") != 0;
    g_settings.ignoreWhileMouseButtonDown =
        Wh_GetIntSetting(L"ignoreWhileMouseButtonDown") != 0;
    g_settings.primaryMonitorOnly =
        Wh_GetIntSetting(L"primaryMonitorOnly") != 0;
}

void QueuePointerStateUpdate() {
    DWORD threadId = g_workerThreadId.load();
    if (!threadId || g_cursorUpdatePosted.exchange(true)) {
        return;
    }

    if (!PostThreadMessageW(threadId, kWorkerCursorChangedMessage, 0, 0)) {
        g_cursorUpdatePosted = false;
        Wh_Log(L"Couldn't queue pointer-state update: %u", GetLastError());
    }
}

void EnsureActivationWorker() {
    if (g_workerThreadId.load() || g_unloading.load() || !g_stopEvent) {
        return;
    }

    std::lock_guard<std::mutex> guard(g_workerThreadMutex);
    if (g_workerThreadId.load() || g_unloading.load()) {
        return;
    }

    if (g_workerThread) {
        DWORD state = WaitForSingleObject(g_workerThread, 0);
        if (state == WAIT_TIMEOUT) {
            return;
        }
        if (state != WAIT_OBJECT_0) {
            Wh_Log(L"Couldn't query activation worker state: %u",
                   GetLastError());
            return;
        }
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    g_workerThread =
        CreateThread(nullptr, 0, ActivationWorkerThread, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
    }
}

enum class TaskbarKind {
    kNone,
    kMain,
    kSecondary,
};

TaskbarKind GetTaskbarKind(HWND window) {
    wchar_t className[64];
    if (!window ||
        !GetClassNameW(window, className, ARRAYSIZE(className))) {
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

bool IsCurrentProcessWindow(HWND window) {
    DWORD processId = 0;
    return window && GetWindowThreadProcessId(window, &processId) != 0 &&
           processId == GetCurrentProcessId();
}

bool IsSupportedTaskbarWindow(HWND window) {
    TaskbarKind kind = GetTaskbarKind(window);
    return IsCurrentProcessWindow(window) &&
           (kind == TaskbarKind::kMain ||
            (kind == TaskbarKind::kSecondary &&
             g_secondaryTaskbarSupportAvailable.load()));
}

HMONITOR GetTaskbarMonitor(HWND taskbarWindow) {
    if (HMONITOR monitor = reinterpret_cast<HMONITOR>(
            GetPropW(taskbarWindow, L"TaskbarMonitor"))) {
        MONITORINFO info{};
        info.cbSize = sizeof(info);
        if (GetMonitorInfoW(monitor, &info)) {
            return monitor;
        }
    }
    return MonitorFromWindow(taskbarWindow, MONITOR_DEFAULTTONEAREST);
}

struct MonitorData {
    RECT rect{};
    UINT dpiY = 96;
    bool primary = false;
};

struct WorkerTimer {
    UINT_PTR id = 0;
    ULONGLONG dueAt = 0;
};

struct WorkerState {
    HWND activeTaskbar = nullptr;
    HMONITOR activeMonitor = nullptr;
    HMONITOR hoverMonitor = nullptr;
    HMONITOR consumedBandMonitor = nullptr;
    ULONGLONG hoverSince = 0;
    WorkerTimer releaseTimer;
    WorkerTimer retryTimer;
    std::unordered_map<HMONITOR, MonitorData> monitors;
    std::unordered_map<HMONITOR, HWND> taskbars;
};

bool GetMonitorData(WorkerState& state,
                    HMONITOR monitor,
                    MonitorData* data) {
    if (!monitor || !data) {
        return false;
    }

    auto it = state.monitors.find(monitor);
    if (it != state.monitors.end()) {
        *data = it->second;
        return true;
    }

    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) {
        return false;
    }

    UINT dpiX = 96;
    UINT dpiY = 96;
    if (FAILED(GetDpiForMonitor(monitor,
                                MDT_EFFECTIVE_DPI,
                                &dpiX,
                                &dpiY))) {
        dpiY = 96;
    }

    MonitorData value;
    value.rect = info.rcMonitor;
    value.dpiY = dpiY;
    value.primary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;
    state.monitors.emplace(monitor, value);
    *data = value;
    return true;
}

int ScaleDip(int logicalPixels, UINT dpiY) {
    return std::max(1, MulDiv(logicalPixels, dpiY ? dpiY : 96, 96));
}

bool IsPointInBottomBand(const POINT& pointer,
                         const MonitorData& monitor,
                         int logicalHeight) {
    int monitorHeight = monitor.rect.bottom - monitor.rect.top;
    int height = std::min(ScaleDip(logicalHeight, monitor.dpiY),
                          monitorHeight);
    return pointer.x >= monitor.rect.left &&
           pointer.x < monitor.rect.right &&
           pointer.y >= monitor.rect.bottom - height &&
           pointer.y < monitor.rect.bottom;
}

bool IsBottomTaskbar(HWND taskbarWindow,
                     HMONITOR monitor,
                     const MonitorData& data) {
    RECT rect{};
    if (!GetWindowRect(taskbarWindow, &rect)) {
        return false;
    }

    int monitorWidth = data.rect.right - data.rect.left;
    int monitorHeight = data.rect.bottom - data.rect.top;
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    if (width < monitorWidth / 2 || height <= 0 ||
        height > monitorHeight / 2) {
        return false;
    }

    int tolerance = ScaleDip(2, data.dpiY);
    return GetTaskbarMonitor(taskbarWindow) == monitor &&
           rect.top <= data.rect.bottom + tolerance &&
           rect.bottom >= data.rect.bottom - tolerance;
}

HWND FindMainTaskbarWindow() {
    HWND cached = g_mainTaskbarWindow.load();
    if (cached && IsCurrentProcessWindow(cached) &&
        GetTaskbarKind(cached) == TaskbarKind::kMain) {
        return cached;
    }

    HWND result = nullptr;
    EnumWindows(
        [](HWND window, LPARAM parameter) -> BOOL {
            if (IsCurrentProcessWindow(window) &&
                GetTaskbarKind(window) == TaskbarKind::kMain) {
                *reinterpret_cast<HWND*>(parameter) = window;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
    g_mainTaskbarWindow = result;
    return result;
}

struct FindTaskbarContext {
    HMONITOR monitor;
    const MonitorData* data;
    HWND result;
};

HWND FindTaskbarForMonitor(HMONITOR monitor, const MonitorData& data) {
    HWND mainTaskbar = FindMainTaskbarWindow();
    if (mainTaskbar && IsBottomTaskbar(mainTaskbar, monitor, data)) {
        return mainTaskbar;
    }

    FindTaskbarContext context{monitor, &data, nullptr};
    EnumWindows(
        [](HWND window, LPARAM parameter) -> BOOL {
            auto* context =
                reinterpret_cast<FindTaskbarContext*>(parameter);
            if (!IsSupportedTaskbarWindow(window) ||
                GetTaskbarKind(window) != TaskbarKind::kSecondary) {
                return TRUE;
            }
            if (IsBottomTaskbar(window,
                                context->monitor,
                                *context->data)) {
                context->result = window;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&context));
    return context.result;
}

HWND GetCachedTaskbarForMonitor(WorkerState& state,
                                HMONITOR monitor,
                                const MonitorData& data) {
    auto it = state.taskbars.find(monitor);
    if (it != state.taskbars.end()) {
        if (!it->second) {
            return nullptr;
        }
        if (IsSupportedTaskbarWindow(it->second) &&
            IsBottomTaskbar(it->second, monitor, data)) {
            return it->second;
        }
        state.taskbars.erase(it);
    }

    HWND taskbarWindow = FindTaskbarForMonitor(monitor, data);
    state.taskbars[monitor] = taskbarWindow;
    return taskbarWindow;
}

bool IsPointInActiveHoldArea(HWND taskbarWindow,
                             const POINT& pointer,
                             const MonitorData& monitor) {
    RECT taskbarRect{};
    if (!GetWindowRect(taskbarWindow, &taskbarRect)) {
        return false;
    }

    if (PtInRect(&taskbarRect, pointer)) {
        return true;
    }

    int taskbarHeight = std::max(1, static_cast<int>(taskbarRect.bottom - taskbarRect.top));
    int triggerHeight = ScaleDip(g_settings.activationThresholdPx.load(),
                                 monitor.dpiY);
    int holdHeight = std::min(triggerHeight, taskbarHeight);
    return pointer.x >= monitor.rect.left &&
           pointer.x < monitor.rect.right &&
           pointer.y >= monitor.rect.bottom - holdHeight &&
           pointer.y < monitor.rect.bottom;
}

bool IsWindowsAutoHideEnabled() {
    APPBARDATA data{};
    data.cbSize = sizeof(data);
    return (SHAppBarMessage(ABM_GETSTATE, &data) & ABS_AUTOHIDE) != 0;
}

bool IsAnyMouseButtonDown() {
    constexpr int buttons[] = {
        VK_LBUTTON,
        VK_RBUTTON,
        VK_MBUTTON,
        VK_XBUTTON1,
        VK_XBUTTON2,
    };
    for (int button : buttons) {
        if ((GetAsyncKeyState(button) & 0x8000) != 0) {
            return true;
        }
    }
    return false;
}

bool IsWindowCloaked(HWND window) {
    BOOL cloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(
               window, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
           cloaked;
}

bool IsFullscreenForegroundWindow(HMONITOR monitor,
                                  const MonitorData& data) {
    QUERY_USER_NOTIFICATION_STATE notification = QUNS_ACCEPTS_NOTIFICATIONS;
    if (SUCCEEDED(SHQueryUserNotificationState(&notification))) {
        if (notification == QUNS_PRESENTATION_MODE) {
            return true;
        }
        if (notification == QUNS_RUNNING_D3D_FULL_SCREEN) {
            HWND foreground = GetForegroundWindow();
            return foreground &&
                   MonitorFromWindow(foreground,
                                     MONITOR_DEFAULTTONEAREST) == monitor;
        }
    }

    HWND window = GetForegroundWindow();
    if (!window || !IsWindowVisible(window) || IsIconic(window) ||
        MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST) != monitor) {
        return false;
    }

    LONG_PTR style = GetWindowLongPtrW(window, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(window, GWL_EXSTYLE);
    if ((style & WS_CHILD) != 0 ||
        (exStyle & (WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW |
                    WS_EX_NOACTIVATE)) != 0 ||
        window == GetShellWindow() || IsWindowCloaked(window)) {
        return false;
    }

    wchar_t className[64];
    if (GetClassNameW(window, className, ARRAYSIZE(className)) &&
        (_wcsicmp(className, L"Progman") == 0 ||
         _wcsicmp(className, L"WorkerW") == 0 ||
         _wcsicmp(className, L"Shell_TrayWnd") == 0 ||
         _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0)) {
        return false;
    }

    // A styled maximized window fills the work area, which equals the monitor
    // while auto-hide is enabled. Don't mistake that for fullscreen. Borderless
    // windows are intentionally allowed through this guard.
    if ((style & (WS_CAPTION | WS_THICKFRAME)) != 0 && IsZoomed(window)) {
        return false;
    }

    RECT rect{};
    if (FAILED(DwmGetWindowAttribute(window,
                                     DWMWA_EXTENDED_FRAME_BOUNDS,
                                     &rect,
                                     sizeof(rect))) &&
        !GetWindowRect(window, &rect)) {
        return false;
    }

    int tolerance = ScaleDip(2, data.dpiY);
    return rect.left <= data.rect.left + tolerance &&
           rect.top <= data.rect.top + tolerance &&
           rect.right >= data.rect.right - tolerance &&
           rect.bottom >= data.rect.bottom - tolerance;
}

bool IsReadablePointerSlot(void* slot) {
    MEMORY_BASIC_INFORMATION info{};
    return slot && VirtualQuery(slot, &info, sizeof(info)) &&
           info.State == MEM_COMMIT &&
           (info.Protect & (PAGE_NOACCESS | PAGE_GUARD)) == 0;
}

void* FindInterfaceForward(void* object, void* targetVtable) {
    uintptr_t address = reinterpret_cast<uintptr_t>(object);
    for (size_t i = 0; i < kMaxInterfaceSlots; i++) {
        uintptr_t offset = i * sizeof(void*);
        if (address > UINTPTR_MAX - offset) {
            break;
        }
        auto** slot = reinterpret_cast<void**>(address + offset);
        if (!IsReadablePointerSlot(slot)) {
            break;
        }
        if (*slot == targetVtable) {
            return slot;
        }
    }
    return nullptr;
}

void* FindInterfaceBackward(void* object, void* targetVtable) {
    uintptr_t address = reinterpret_cast<uintptr_t>(object);
    for (size_t i = 0; i < kMaxInterfaceSlots; i++) {
        uintptr_t offset = i * sizeof(void*);
        if (address < offset) {
            break;
        }
        auto** slot = reinterpret_cast<void**>(address - offset);
        if (!IsReadablePointerSlot(slot)) {
            break;
        }
        if (*slot == targetVtable) {
            return slot;
        }
    }
    return nullptr;
}

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

void RememberViewCoordinator(HWND taskbarWindow, void* viewCoordinator) {
    if (!taskbarWindow || !viewCoordinator) {
        return;
    }
    auto [it, inserted] =
        g_viewCoordinators.insert_or_assign(taskbarWindow, viewCoordinator);
    (void)it;
    if (inserted) {
        Wh_Log(L"Captured ViewCoordinator %p for taskbar %p",
               viewCoordinator,
               taskbarWindow);
    }
}

void UpdateViewCoordinator(HWND taskbarWindow) {
    if (!ViewCoordinator_UpdateIsExpanded_Original) {
        return;
    }
    auto it = g_viewCoordinators.find(taskbarWindow);
    if (it == g_viewCoordinators.end()) {
        return;
    }

    // ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged reason.
    constexpr int kReasonPointerOverTaskbarFrameChanged = 7;
    ViewCoordinator_UpdateIsExpanded_Original(
        it->second, taskbarWindow, kReasonPointerOverTaskbarFrameChanged);
}

bool WINAPI ViewCoordinator_ShouldTaskbarBeExpanded_Hook(
    void* viewCoordinator,
    HWND taskbarWindow,
    bool expanded) {
    RememberViewCoordinator(taskbarWindow, viewCoordinator);
    if (!g_unloading.load() &&
        g_desiredKeptShownTaskbar.load() == taskbarWindow) {
        return true;
    }
    return ViewCoordinator_ShouldTaskbarBeExpanded_Original(
        viewCoordinator, taskbarWindow, expanded);
}

void WINAPI TrayUI_Hide_Hook(void* trayUiInspectable) {
    auto it = g_taskbarsKeptShown.find(trayUiInspectable);
    if (!g_unloading.load() && it != g_taskbarsKeptShown.end() &&
        g_desiredKeptShownTaskbar.load() == it->second) {
        KillTimer(it->second, kTrayUITimerHide);
        return;
    }
    TrayUI_Hide_Original(trayUiInspectable);
}

void WINAPI CSecondaryTray_AutoHide_Hook(void* secondaryTray,
                                         bool parameter) {
    auto it = g_taskbarsKeptShown.find(secondaryTray);
    if (!g_unloading.load() && it != g_taskbarsKeptShown.end() &&
        g_desiredKeptShownTaskbar.load() == it->second) {
        KillTimer(it->second, kTrayUITimerHide);
        return;
    }
    CSecondaryTray_AutoHide_Original(secondaryTray, parameter);
}

void ReleaseExplorerTaskbar(HWND taskbarWindow) {
    bool explorerWantsExpanded = false;
    auto it = g_viewCoordinators.find(taskbarWindow);
    if (it != g_viewCoordinators.end() &&
        ViewCoordinator_ShouldTaskbarBeExpanded_Original) {
        explorerWantsExpanded =
            ViewCoordinator_ShouldTaskbarBeExpanded_Original(
                it->second, taskbarWindow, true);
    }

    if (!explorerWantsExpanded && IsWindow(taskbarWindow) &&
        !SetTimer(taskbarWindow, kTrayUITimerHide, 0, nullptr)) {
        Wh_Log(L"Couldn't arm Explorer's hide timer for taskbar %p: %u",
               taskbarWindow,
               GetLastError());
    }
    UpdateViewCoordinator(taskbarWindow);
}

bool ApplyPrimaryTaskbarState(void* trayUi, HWND taskbarWindow) {
    bool wantShown = !g_unloading.load() &&
                     g_desiredKeptShownTaskbar.load() == taskbarWindow;
    if (wantShown) {
        if (!TrayUI_Unhide_Original || !g_trayUiVtableIInspectable ||
            !g_trayUiVtableITrayComponentHost) {
            return false;
        }

        void* inspectable =
            FindInterfaceBackward(trayUi, g_trayUiVtableIInspectable);
        void* componentHost =
            FindInterfaceForward(trayUi, g_trayUiVtableITrayComponentHost);
        if (!inspectable || !componentHost) {
            return false;
        }

        g_taskbarsKeptShown[inspectable] = taskbarWindow;
        TrayUI_Unhide_Original(componentHost, 0, 0);
        UpdateViewCoordinator(taskbarWindow);
        return true;
    }

    bool wasKeptShown = EraseKeptShownForWindow(taskbarWindow);
    if (wasKeptShown) {
        ReleaseExplorerTaskbar(taskbarWindow);
    }
    return true;
}

bool ApplySecondaryTaskbarState(void* secondaryTray,
                                HWND taskbarWindow) {
    bool wantShown = !g_unloading.load() &&
                     g_desiredKeptShownTaskbar.load() == taskbarWindow;
    if (wantShown) {
        if (!CSecondaryTray_Unhide_Original) {
            return false;
        }
        g_taskbarsKeptShown[secondaryTray] = taskbarWindow;
        CSecondaryTray_Unhide_Original(secondaryTray, 0, 0);
        UpdateViewCoordinator(taskbarWindow);
        return true;
    }

    bool wasKeptShown = EraseKeptShownForWindow(taskbarWindow);
    if (wasKeptShown) {
        ReleaseExplorerTaskbar(taskbarWindow);
    }
    return true;
}

void ForgetTaskbarWindow(HWND taskbarWindow) {
    g_viewCoordinators.erase(taskbarWindow);
    EraseKeptShownForWindow(taskbarWindow);
    HWND expected = taskbarWindow;
    g_desiredKeptShownTaskbar.compare_exchange_strong(expected, nullptr);
    if (g_mainTaskbarWindow.load() == taskbarWindow) {
        g_mainTaskbarWindow = nullptr;
    }
}

void HandleTaskbarEnvironmentMessage(UINT message) {
    if (message == WM_DISPLAYCHANGE || message == WM_DPICHANGED) {
        g_workerStateDirty = true;
        EnsureActivationWorker();
        QueuePointerStateUpdate();
    } else if (message == WM_SETTINGCHANGE) {
        g_workerStateDirty = true;
        EnsureActivationWorker();
        QueuePointerStateUpdate();
    }
}

LRESULT WINAPI TrayUI_WndProc_Hook(void* trayUi,
                                   HWND window,
                                   UINT message,
                                   WPARAM wParam,
                                   LPARAM lParam,
                                   bool* handled) {
    if (message == g_updateTaskbarStateMessage) {
        g_mainTaskbarWindow = window;
        bool ok = ApplyPrimaryTaskbarState(trayUi, window);
        if (handled) {
            *handled = true;
        }
        return ok ? 1 : 0;
    }

    bool created = message == WM_NCCREATE;
    bool destroyed = message == WM_NCDESTROY;
    if (created) {
        g_mainTaskbarWindow = window;
        EnsureActivationWorker();
    } else if (destroyed) {
        ForgetTaskbarWindow(window);
    }

    LRESULT result = TrayUI_WndProc_Original(
        trayUi, window, message, wParam, lParam, handled);

    HandleTaskbarEnvironmentMessage(message);
    if (created || destroyed) {
        g_workerStateDirty = true;
        QueuePointerStateUpdate();
    }
    return result;
}

LRESULT WINAPI CSecondaryTray_WndProc_Hook(void* secondaryTray,
                                           HWND window,
                                           UINT message,
                                           WPARAM wParam,
                                           LPARAM lParam) {
    if (message == g_updateTaskbarStateMessage) {
        return ApplySecondaryTaskbarState(secondaryTray, window) ? 1 : 0;
    }

    bool created = message == WM_NCCREATE;
    bool destroyed = message == WM_NCDESTROY;
    if (created) {
        EnsureActivationWorker();
    } else if (destroyed) {
        ForgetTaskbarWindow(window);
    }

    LRESULT result = CSecondaryTray_WndProc_Original(
        secondaryTray, window, message, wParam, lParam);

    HandleTaskbarEnvironmentMessage(message);
    if (created || destroyed) {
        g_workerStateDirty = true;
        QueuePointerStateUpdate();
    }
    return result;
}

bool SendTaskbarStateUpdate(HWND taskbarWindow, DWORD timeoutMs) {
    if (!IsSupportedTaskbarWindow(taskbarWindow)) {
        return false;
    }

    DWORD_PTR result = 0;
    if (!SendMessageTimeoutW(
            taskbarWindow,
            g_updateTaskbarStateMessage,
            0,
            0,
            SMTO_ABORTIFHUNG | SMTO_BLOCK | SMTO_ERRORONEXIT,
            timeoutMs,
            &result)) {
        return false;
    }
    return result != 0;
}

void CancelWorkerTimer(WorkerTimer& timer) {
    if (timer.id) {
        KillTimer(nullptr, timer.id);
    }
    timer.id = 0;
    timer.dueAt = 0;
}

bool ScheduleWorkerTimer(UINT delayMs, WorkerTimer& timer) {
    UINT effectiveDelay = std::max(delayMs, 1U);
    ULONGLONG dueAt = GetTickCount64() + effectiveDelay;
    if (timer.id && timer.dueAt <= dueAt) {
        return true;
    }
    CancelWorkerTimer(timer);
    timer.id = SetTimer(nullptr, 0, effectiveDelay, nullptr);
    if (!timer.id) {
        Wh_Log(L"Failed to create worker timer: %u", GetLastError());
        return false;
    }
    timer.dueAt = dueAt;
    return true;
}

void ResetHoverCandidate(WorkerState& state) {
    state.hoverMonitor = nullptr;
    state.hoverSince = 0;
}

void ClearActiveTaskbar(WorkerState& state) {
    state.activeTaskbar = nullptr;
    state.activeMonitor = nullptr;
    CancelWorkerTimer(state.releaseTimer);
}

void ReleaseActiveTaskbar(WorkerState& state) {
    if (!state.activeTaskbar) {
        return;
    }

    HWND taskbarWindow = state.activeTaskbar;
    HWND expected = taskbarWindow;
    g_desiredKeptShownTaskbar.compare_exchange_strong(expected, nullptr);

    if (IsSupportedTaskbarWindow(taskbarWindow) &&
        !SendTaskbarStateUpdate(taskbarWindow, 250)) {
            // Desired state is already null, so hooks fail open even if the UI
            // thread is busy. Queue one best-effort cleanup to erase stale map
            // state and re-arm Explorer's hide policy when the thread recovers.
            PostMessageW(taskbarWindow, g_updateTaskbarStateMessage, 0, 0);
    }

    ClearActiveTaskbar(state);
}

bool RevealTaskbar(WorkerState& state,
                   HWND taskbarWindow,
                   HMONITOR monitor) {
    HWND previous = g_desiredKeptShownTaskbar.exchange(taskbarWindow);
    if (previous && previous != taskbarWindow &&
        IsSupportedTaskbarWindow(previous)) {
        SendTaskbarStateUpdate(previous, 100);
    }

    if (!SendTaskbarStateUpdate(taskbarWindow, 250)) {
        HWND expected = taskbarWindow;
        g_desiredKeptShownTaskbar.compare_exchange_strong(expected, nullptr);
        PostMessageW(taskbarWindow, g_updateTaskbarStateMessage, 0, 0);
        return false;
    }

    state.activeTaskbar = taskbarWindow;
    state.activeMonitor = monitor;
    state.consumedBandMonitor = monitor;
    ResetHoverCandidate(state);
    return true;
}

void ProcessPointerState(WorkerState& state) {
    if (g_unloading.load()) {
        return;
    }

    if (g_workerStateDirty.exchange(false)) {
        state.monitors.clear();
        state.taskbars.clear();
        CancelWorkerTimer(state.releaseTimer);
        CancelWorkerTimer(state.retryTimer);
        ResetHoverCandidate(state);
        state.consumedBandMonitor = nullptr;
    }

    POINT pointer{};
    if (!GetCursorPos(&pointer)) {
        return;
    }

    HMONITOR pointerMonitor =
        MonitorFromPoint(pointer, MONITOR_DEFAULTTONULL);
    if (!pointerMonitor) {
        ReleaseActiveTaskbar(state);
        CancelWorkerTimer(state.retryTimer);
        ResetHoverCandidate(state);
        state.consumedBandMonitor = nullptr;
        return;
    }

    MonitorData monitor;
    if (!GetMonitorData(state, pointerMonitor, &monitor)) {
        return;
    }

    bool inTriggerBand = IsPointInBottomBand(
        pointer, monitor, g_settings.activationThresholdPx.load());

    if (state.activeTaskbar &&
        g_desiredKeptShownTaskbar.load() != state.activeTaskbar) {
        ClearActiveTaskbar(state);
    }

    if (state.activeTaskbar) {
        bool activeAllowed = true;
        if (g_settings.primaryMonitorOnly.load()) {
            MonitorData activeData;
            activeAllowed =
                GetMonitorData(state, state.activeMonitor, &activeData) &&
                activeData.primary;
        }

        if (!activeAllowed || !IsWindow(state.activeTaskbar)) {
            ReleaseActiveTaskbar(state);
        } else if (pointerMonitor == state.activeMonitor &&
                   IsPointInActiveHoldArea(state.activeTaskbar,
                                           pointer,
                                           monitor)) {
            CancelWorkerTimer(state.releaseTimer);
            return;
        } else if (pointerMonitor != state.activeMonitor && inTriggerBand) {
            // Don't stack the old monitor's release delay with the new monitor's
            // hover delay.
            ReleaseActiveTaskbar(state);
        } else {
            int delay = g_settings.releaseDelayMs.load();
            if (delay > 0 &&
                ScheduleWorkerTimer(static_cast<UINT>(delay),
                                    state.releaseTimer)) {
                return;
            }
            ReleaseActiveTaskbar(state);
        }
    }

    if (!inTriggerBand) {
        CancelWorkerTimer(state.retryTimer);
        ResetHoverCandidate(state);
        state.consumedBandMonitor = nullptr;
        return;
    }

    // One reveal per continuous band entry. This prevents a large trigger band
    // from becoming a show/hide loop after the post-reveal hold area is clamped
    // to the taskbar's height.
    if (state.consumedBandMonitor == pointerMonitor) {
        CancelWorkerTimer(state.retryTimer);
        return;
    }

    ULONGLONG now = GetTickCount64();
    int hoverDelay = g_settings.activationDelayMs.load();
    if (state.hoverMonitor != pointerMonitor) {
        state.hoverMonitor = pointerMonitor;
        state.hoverSince = now;
        if (hoverDelay > 0) {
            ScheduleWorkerTimer(static_cast<UINT>(hoverDelay),
                                state.retryTimer);
            return;
        }
    } else if (now - state.hoverSince <
               static_cast<ULONGLONG>(hoverDelay)) {
        ScheduleWorkerTimer(
            static_cast<UINT>(hoverDelay - (now - state.hoverSince)),
            state.retryTimer);
        return;
    }

    CancelWorkerTimer(state.retryTimer);

    // Conditions below either change through events we already observe or are
    // transient. Event-driven blockers consume this band entry so cursor jitter
    // doesn't repeatedly redo work that can't succeed until the environment
    // changes; g_workerStateDirty clears the consumed state when that happens.
    if (!g_viewCoordinatorSupportAvailable.load()) {
        state.consumedBandMonitor = pointerMonitor;
        return;
    }
    if (g_settings.primaryMonitorOnly.load() && !monitor.primary) {
        state.consumedBandMonitor = pointerMonitor;
        return;
    }
    if (g_settings.ignoreWhileMouseButtonDown.load() &&
        IsAnyMouseButtonDown()) {
        ScheduleWorkerTimer(kTransientRetryMs, state.retryTimer);
        return;
    }
    if (g_settings.requireWindowsAutoHide.load() &&
        !IsWindowsAutoHideEnabled()) {
        state.consumedBandMonitor = pointerMonitor;
        return;
    }
    if (g_settings.ignoreFullscreenApps.load() &&
        IsFullscreenForegroundWindow(pointerMonitor, monitor)) {
        ScheduleWorkerTimer(kTransientRetryMs, state.retryTimer);
        return;
    }

    HWND taskbarWindow =
        GetCachedTaskbarForMonitor(state, pointerMonitor, monitor);
    if (!taskbarWindow) {
        state.consumedBandMonitor = pointerMonitor;
        return;
    }

    if (!RevealTaskbar(state, taskbarWindow, pointerMonitor)) {
        ScheduleWorkerTimer(kTransientRetryMs, state.retryTimer);
    }
}

void HandleReleaseTimer(WorkerState& state) {
    CancelWorkerTimer(state.releaseTimer);
    if (!state.activeTaskbar) {
        return;
    }

    POINT pointer{};
    HMONITOR monitor = nullptr;
    MonitorData data;
    bool keepShown = false;
    if (GetCursorPos(&pointer)) {
        monitor = MonitorFromPoint(pointer, MONITOR_DEFAULTTONULL);
        if (monitor == state.activeMonitor &&
            GetMonitorData(state, monitor, &data)) {
            keepShown = IsPointInActiveHoldArea(
                state.activeTaskbar, pointer, data);
        }
    }

    if (keepShown) {
        return;
    }

    ReleaseActiveTaskbar(state);
    ProcessPointerState(state);
}

void CALLBACK CursorWinEventProc(HWINEVENTHOOK,
                                 DWORD event,
                                 HWND,
                                 LONG idObject,
                                 LONG,
                                 DWORD,
                                 DWORD) {
    if (event == EVENT_OBJECT_LOCATIONCHANGE && idObject == OBJID_CURSOR) {
        QueuePointerStateUpdate();
    }
}

DWORD WINAPI ActivationWorkerThread(LPVOID) {
    MSG message{};
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_workerThreadId = GetCurrentThreadId();

    HWINEVENTHOOK cursorHook = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE,
        EVENT_OBJECT_LOCATIONCHANGE,
        nullptr,
        CursorWinEventProc,
        0,
        0,
        WINEVENT_OUTOFCONTEXT);

    UINT_PTR fallbackTimer = 0;
    if (!cursorHook) {
        Wh_Log(L"Cursor WinEvent hook failed: %u; using 150 ms fallback",
               GetLastError());
        fallbackTimer = SetTimer(nullptr, 0, 150, nullptr);
    }

    WorkerState state;
    ProcessPointerState(state);

    bool unexpectedExit = false;
    for (;;) {
        DWORD waitResult = MsgWaitForMultipleObjects(
            1, &g_stopEvent, FALSE, INFINITE, QS_ALLINPUT);
        if (waitResult == WAIT_OBJECT_0) {
            break;
        }
        if (waitResult != WAIT_OBJECT_0 + 1) {
            Wh_Log(L"Activation worker wait failed: %u", GetLastError());
            unexpectedExit = true;
            break;
        }

        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == kWorkerCursorChangedMessage) {
                g_cursorUpdatePosted = false;
                ProcessPointerState(state);
            } else if (message.message == WM_TIMER) {
                if (state.releaseTimer.id &&
                    message.wParam == state.releaseTimer.id) {
                    HandleReleaseTimer(state);
                } else if (state.retryTimer.id &&
                           message.wParam == state.retryTimer.id) {
                    CancelWorkerTimer(state.retryTimer);
                    ProcessPointerState(state);
                } else if (fallbackTimer &&
                           message.wParam == fallbackTimer) {
                    ProcessPointerState(state);
                }
            } else {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
    }

    CancelWorkerTimer(state.releaseTimer);
    CancelWorkerTimer(state.retryTimer);

    if (unexpectedExit && !g_unloading.load() && state.activeTaskbar) {
        HWND taskbarWindow = state.activeTaskbar;
        HWND expected = taskbarWindow;
        g_desiredKeptShownTaskbar.compare_exchange_strong(expected, nullptr);
        PostMessageW(taskbarWindow, g_updateTaskbarStateMessage, 0, 0);
    }

    if (cursorHook) {
        UnhookWinEvent(cursorHook);
    } else if (fallbackTimer) {
        KillTimer(nullptr, fallbackTimer);
    }

    g_cursorUpdatePosted = false;
    g_workerThreadId = 0;
    return 0;
}

bool HookTaskbarViewSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded(unsigned __int64,bool))",
            },
            &ViewCoordinator_ShouldTaskbarBeExpanded_Original,
            ViewCoordinator_ShouldTaskbarBeExpanded_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::UpdateIsExpanded(unsigned __int64,enum TaskbarTipTest::TaskbarExpandCollapseReason))",
            },
            &ViewCoordinator_UpdateIsExpanded_Original,
            nullptr,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks)) ||
        !ViewCoordinator_ShouldTaskbarBeExpanded_Original ||
        !ViewCoordinator_UpdateIsExpanded_Original) {
        Wh_Log(L"Required ViewCoordinator symbols are unavailable");
        return false;
    }

    g_viewCoordinatorSupportAvailable = true;
    return true;
}

bool HookTaskbarSymbols() {
    HMODULE module = LoadLibraryExW(
        L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load taskbar.dll: %u", GetLastError());
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(const TrayUI::`vftable'{for `IInspectable'})"},
            &g_trayUiVtableIInspectable,
            nullptr,
        },
        {
            {LR"(const TrayUI::`vftable'{for `ITrayComponentHost'})"},
            &g_trayUiVtableITrayComponentHost,
            nullptr,
        },
        {
            {LR"(public: void __cdecl TrayUI::_Hide(void))"},
            &TrayUI_Hide_Original,
            TrayUI_Hide_Hook,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_AutoHide(bool))"},
            &CSecondaryTray_AutoHide_Original,
            CSecondaryTray_AutoHide_Hook,
            true,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            &TrayUI_Unhide_Original,
            nullptr,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            &CSecondaryTray_Unhide_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))"},
            &TrayUI_WndProc_Original,
            TrayUI_WndProc_Hook,
        },
        {
            {LR"(private: virtual __int64 __cdecl CSecondaryTray::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &CSecondaryTray_WndProc_Original,
            CSecondaryTray_WndProc_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"Required native taskbar symbols are unavailable");
        return false;
    }

    g_secondaryTaskbarSupportAvailable =
        CSecondaryTray_AutoHide_Original &&
        CSecondaryTray_Unhide_Original &&
        CSecondaryTray_WndProc_Original;
    if (!g_secondaryTaskbarSupportAvailable.load()) {
        Wh_Log(L"Secondary taskbar symbols are unavailable");
    }
    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    return module ? module : GetModuleHandleW(L"ExplorerExtensions.dll");
}

void HandleLoadedTaskbarViewModule(HMODULE module) {
    if (!module || g_unloading.load() || g_taskbarViewDllLoaded.load() ||
        GetTaskbarViewModuleHandle() != module) {
        return;
    }

    if (g_taskbarViewDllLoaded.exchange(true)) {
        return;
    }

    if (HookTaskbarViewSymbols(module)) {
        Wh_ApplyHookOperations();
        g_workerStateDirty = true;
        QueuePointerStateUpdate();
    }
}

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName,
                                   HANDLE file,
                                   DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    if (module && !((ULONG_PTR)module & 3)) {
        HandleLoadedTaskbarViewModule(module);
    }
    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing");
    g_unloading = false;
    LoadSettings();

    g_updateTaskbarStateMessage = RegisterWindowMessageW(
        L"Windhawk_UpdateTaskbarState_" WH_MOD_ID);
    if (!g_updateTaskbarStateMessage) {
        return FALSE;
    }

    if (!HookTaskbarSymbols()) {
        return FALSE;
    }

    if (HMODULE module = GetTaskbarViewModuleHandle()) {
        if (!HookTaskbarViewSymbols(module)) {
            return FALSE;
        }
        g_taskbarViewDllLoaded = true;
    } else {
        HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
        auto loadLibraryExW = reinterpret_cast<decltype(&LoadLibraryExW)>(
            GetProcAddress(kernelBase, "LoadLibraryExW"));
        if (!loadLibraryExW ||
            !WindhawkUtils::SetFunctionHook(
                loadLibraryExW,
                LoadLibraryExW_Hook,
                &LoadLibraryExW_Original)) {
            Wh_Log(L"Couldn't hook KernelBase!LoadLibraryExW");
            return FALSE;
        }
    }

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    WNDCLASSW taskbarClass{};
    if (GetClassInfoW(GetModuleHandleW(nullptr),
                      L"Shell_TrayWnd",
                      &taskbarClass)) {
        EnsureActivationWorker();
    }

    // Close the small race where Taskbar.View loads between Wh_ModInit and hook
    // application.
    if (!g_taskbarViewDllLoaded.load()) {
        HandleLoadedTaskbarViewModule(GetTaskbarViewModuleHandle());
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"Stopping activation worker");
    g_unloading = true;
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    HANDLE worker = nullptr;
    {
        std::lock_guard<std::mutex> guard(g_workerThreadMutex);
        worker = g_workerThread;
    }
    if (worker) {
        WaitForSingleObject(worker, INFINITE);
    }

    // Reconcile every current taskbar while hooks are still installed, not only
    // the latest desired HWND. This also cleans up the exceptional case where a
    // prior release cleared desired state but its UI message never arrived.
    g_desiredKeptShownTaskbar = nullptr;
    EnumWindows(
        [](HWND window, LPARAM) -> BOOL {
            if (IsSupportedTaskbarWindow(window) &&
                !SendTaskbarStateUpdate(window, 1000)) {
                Wh_Log(L"Final taskbar-state release wasn't confirmed for %p",
                       window);
            }
            return TRUE;
        },
        0);
}

void Wh_ModUninit() {
    HANDLE worker = nullptr;
    {
        std::lock_guard<std::mutex> guard(g_workerThreadMutex);
        worker = g_workerThread;
        g_workerThread = nullptr;
    }
    if (worker) {
        CloseHandle(worker);
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    g_workerStateDirty = true;
    EnsureActivationWorker();
    QueuePointerStateUpdate();
}
