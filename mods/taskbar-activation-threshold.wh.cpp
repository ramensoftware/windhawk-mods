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
stationary and invokes Explorer's native taskbar reveal logic on the taskbar UI
thread.

The activation threshold is scaled independently for each monitor's DPI.
Cursor movement is detected through Windows accessibility events rather than
continuous polling, minimizing background activity while the pointer is idle.

## Demonstration

![Taskbar Activation Threshold demonstration](https://raw.githubusercontent.com/themagnificentoofman/taskbar-activation-threshold-assets/main/taskbar-activation-threshold-demo.gif)

## Features

- Configurable activation-band height and hover delay
- Per-monitor DPI scaling
- Multi-monitor support
- Adjustable release delay and activation cooldown
- Optional suppression over fullscreen applications
- Optional suppression while a mouse button is held
- Optional restriction to the primary monitor
- Native timer fallback while the Windows 11 taskbar coordinator is unavailable
- No cursor movement, pointer injection, or taskbar layout modification

## Compatibility

The mod does not modify:

- Taskbar position or geometry
- XAML layout or styles
- Taskbar height or icon size
- Animation duration
- Transparency
- Windows' auto-hide setting

When revealing the taskbar, the mod brings it to the front of its existing
Z-order band without activating it or changing whether Explorer currently
treats it as topmost. This prevents ordinary windows in the same band from
covering the taskbar while preserving fullscreen and third-party Z-order
policies.

The mod is designed to coexist with taskbar styling, sizing, transparency,
animation, clock, label, and icon mods that retain the standard bottom-positioned
Windows 11 taskbar.

## Requirements

- Windows 11
- The standard Windows 11 taskbar
- A bottom-positioned taskbar
- **Automatically hide the taskbar** enabled in Windows, unless
  `requireWindowsAutoHide` is disabled

## Known limitations

- Top-, left-, right-, detached-, and arbitrarily positioned taskbars are not
  supported.
- ExplorerPatcher's legacy Windows 10 taskbar is not supported.
- Mods that disable, replace, or override the taskbar's native expansion logic
  may conflict with this mod. Keyboard-only and never-show auto-hide modes are
  intentionally incompatible with mouse activation.
- Windows updates can rename undocumented taskbar symbols. If this occurs, one
  or more reveal paths may require a symbol update.
- Shortly after Explorer starts, the internal Windows 11 taskbar coordinator
  may not yet have been captured. During this period, the optional native timer
  fallback is used. Coordinator-controlled activation begins automatically once
  the coordinator becomes available.

## Suggested settings

- Activation threshold: `24`
- Hover delay: `75`
- Release delay: `120`
- Activation cooldown: `200`

These defaults provide a noticeably larger activation area while retaining
responsive reveal and hide behavior. The short hover delay prevents accidental
reveals while crossing the bottom edge of an upper monitor in a vertically
stacked layout. Increase the activation threshold for easier access, or reduce
it to more closely match the standard Windows edge trigger.

## Attribution and license

The Windows 11 `ViewCoordinator` symbol names and UI-thread invocation
technique were informed by the GPL-3.0-licensed **Taskbar auto-hide fine
tuning** Windhawk mod by m417z.

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
    Minimum time between reveal checks while the pointer remains in the band.
- requireWindowsAutoHide: true
  $name: Require Windows auto-hide
  $description: >-
    Only activate when Windows reports that taskbar auto-hide is enabled.
    Disable this when another mod manages auto-hide without enabling the
    Windows setting.
- ignoreFullscreenApps: true
  $name: Ignore fullscreen apps
  $description: >-
    Don't reveal the taskbar over borderless or exclusive-fullscreen apps.
- ignoreWhileMouseButtonDown: true
  $name: Ignore while dragging
  $description: >-
    Don't activate while a mouse button is held.
- primaryMonitorOnly: false
  $name: Primary monitor only
  $description: >-
    Only use the activation band on the primary monitor.
- nativeTimerFallback: true
  $name: Native timer fallback
  $description: >-
    If the Windows 11 ViewCoordinator isn't available yet, ask Explorer's
    native taskbar unhide timer to reveal the taskbar. This doesn't move or
    inject the mouse cursor.
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
    std::atomic<bool> nativeTimerFallback{true};
};

Settings g_settings;

std::atomic<bool> g_taskbarViewDllLoaded{false};
std::atomic<unsigned> g_settingsGeneration{0};
std::atomic<DWORD> g_workerThreadId{0};
std::atomic<bool> g_cursorUpdatePosted{false};
std::atomic<HWND> g_mainTaskbarWindow{nullptr};

HANDLE g_stopEvent = nullptr;
HANDLE g_workerThread = nullptr;

// Registered in Wh_ModInit rather than during DLL initialization, avoiding
// user32 calls while the loader lock is held.
UINT g_uiThreadMessage = 0;
UINT g_captureTaskbarObjectMessage = 0;
UINT g_queryTaskbarRectMessage = 0;

// The taskbar-rectangle query can outlive SendMessageTimeoutW. Keeping all
// request and result storage at file scope avoids passing a pointer to a worker
// stack frame into Explorer's UI thread. Only the activation worker initiates
// these queries, and a timed-out request remains pending until the UI thread
// eventually consumes it.
struct StuckRectQueryState {
    std::atomic<bool> pending{false};
    std::atomic<bool> succeeded{false};
    HMONITOR monitor = nullptr;
    RECT rect{};
};

StuckRectQueryState g_stuckRectQuery;

enum UiOperation : WPARAM {
    kUiRevealTaskbar = 1,
    kUiClearSyntheticPointerOver = 2,
    kUiClearAllSyntheticPointerOver = 3,
    kUiTriggerNativeUnhideTimer = 4,
    kUiCancelNativeUnhideTimer = 5,
    kUiBringTaskbarToFront = 6,
};

constexpr UINT kWorkerCursorChangedMessage = WM_APP + 1;
constexpr UINT kWorkerSettingsChangedMessage = WM_APP + 2;
constexpr UINT kReleaseRetryMs = 250;
constexpr UINT kFallbackStatusPollMs = 250;
constexpr UINT kFallbackRearmBaseMs = 1000;
constexpr UINT kNativeTimerCleanupMs = 250;
constexpr UINT kSuppressionBackoffMaxMs = 5000;

// These containers are used by taskbar implementation callbacks and by
// operations marshalled to the taskbar UI thread. This matches the threading
// convention used by the maintained taskbar auto-hide mods.
std::unordered_map<HWND, void*> g_trayUiWndProcObjects;
std::unordered_map<HWND, void*> g_secondaryTrayObjects;
std::unordered_map<HWND, void*> g_viewCoordinators;
std::unordered_set<HWND> g_syntheticPointerOverTaskbars;

// Set only while this mod is applying its reason-7 synthetic enter update on
// the taskbar UI thread. This keeps ShouldTaskbarBeExpanded narrowly scoped.
bool g_syntheticExpansionUpdateInProgress = false;

void* g_trayUiVtableITrayComponentHost = nullptr;

void ForgetTaskbarState(HWND taskbarWindow) {
    g_viewCoordinators.erase(taskbarWindow);
    g_syntheticPointerOverTaskbars.erase(taskbarWindow);
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

    g_settings.nativeTimerFallback.store(
        Wh_GetIntSetting(L"nativeTimerFallback") != 0,
        std::memory_order_relaxed);
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
    if (!window || !IsWindow(window)) {
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

bool GetTaskbarStuckRectForMonitor(HMONITOR monitor, RECT* rect) {
    if (!monitor || !rect || !g_queryTaskbarRectMessage ||
        g_stuckRectQuery.pending.load(std::memory_order_acquire)) {
        return false;
    }

    HWND taskbarWindow = FindMainTaskbarWindow();
    if (!taskbarWindow) {
        return false;
    }

    g_stuckRectQuery.monitor = monitor;
    SetRectEmpty(&g_stuckRectQuery.rect);
    g_stuckRectQuery.succeeded.store(false, std::memory_order_relaxed);
    g_stuckRectQuery.pending.store(true, std::memory_order_release);

    DWORD_PTR result = 0;
    SetLastError(ERROR_SUCCESS);
    if (!SendMessageTimeoutW(
            taskbarWindow,
            g_queryTaskbarRectMessage,
            0,
            0,
            SMTO_ABORTIFHUNG | SMTO_BLOCK,
            250,
            &result)) {
        const DWORD error = GetLastError();
        if (error != ERROR_TIMEOUT && error != ERROR_SUCCESS) {
            // The message wasn't left pending, so allow another query.
            g_stuckRectQuery.pending.store(false,
                                           std::memory_order_release);
        }
        return false;
    }

    // If the hook wasn't reached, don't leave the query permanently pending.
    g_stuckRectQuery.pending.store(false, std::memory_order_release);

    if (!result ||
        !g_stuckRectQuery.succeeded.load(std::memory_order_acquire) ||
        IsRectEmpty(&g_stuckRectQuery.rect)) {
        return false;
    }

    *rect = g_stuckRectQuery.rect;
    return true;
}

HWND GetTaskBandWindow() {
    HWND taskbarWindow = FindMainTaskbarWindow();
    if (!taskbarWindow) {
        return nullptr;
    }

    return reinterpret_cast<HWND>(
        GetPropW(taskbarWindow, L"TaskbandHWND"));
}

HMONITOR GetTaskbarMonitor(HWND taskbarWindow) {
    if (HMONITOR monitor = reinterpret_cast<HMONITOR>(
            GetPropW(taskbarWindow, L"TaskbarMonitor"))) {
        return monitor;
    }

    return MonitorFromWindow(taskbarWindow, MONITOR_DEFAULTTONEAREST);
}

bool IsBottomDockedRect(const RECT& taskbarRect,
                        const MONITORINFO& monitorInfo) {
    const int monitorWidth =
        monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
    const int monitorHeight =
        monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
    const int taskbarWidth = taskbarRect.right - taskbarRect.left;
    const int taskbarHeight = taskbarRect.bottom - taskbarRect.top;

    const bool looksHorizontal =
        taskbarWidth >= monitorWidth / 2 && taskbarHeight > 0 &&
        taskbarHeight <= monitorHeight / 2;
    return looksHorizontal &&
           std::abs(taskbarRect.bottom - monitorInfo.rcMonitor.bottom) <= 2;
}

bool IsBottomPositionedTaskbar(HWND taskbarWindow, HMONITOR monitor) {
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return false;
    }

    // Prefer Explorer's own docked rectangle. Unlike the live window geometry,
    // this identifies the configured edge even while the taskbar is hidden.
    RECT stuckRect{};
    if (GetTaskbarStuckRectForMonitor(monitor, &stuckRect)) {
        return IsBottomDockedRect(stuckRect, monitorInfo);
    }

    // Fallback for Windows builds where the optional symbol is unavailable.
    RECT taskbarRect{};
    if (!GetWindowRect(taskbarWindow, &taskbarRect)) {
        return false;
    }

    const int monitorWidth =
        monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
    const int monitorHeight =
        monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
    const int taskbarWidth = taskbarRect.right - taskbarRect.left;
    const int taskbarHeight = taskbarRect.bottom - taskbarRect.top;
    const bool looksHorizontal =
        taskbarWidth >= monitorWidth / 2 && taskbarHeight > 0 &&
        taskbarHeight <= monitorHeight / 2;
    if (!looksHorizontal) {
        return false;
    }

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
    APPBARDATA appBarData{};
    appBarData.cbSize = sizeof(appBarData);

    return (SHAppBarMessage(ABM_GETSTATE, &appBarData) &
            ABS_AUTOHIDE) != 0;
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
        (style & (WS_CAPTION | WS_THICKFRAME)) != 0 ||
        (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) != 0) {
        return false;
    }

    if (MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST) != monitor ||
        IsWindowCloaked(window)) {
        return false;
    }

    RECT windowRect{};
    if (!GetWindowRect(window, &windowRect)) {
        return false;
    }

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return false;
    }

    constexpr int tolerance = 2;
    return windowRect.left <= monitorInfo.rcMonitor.left + tolerance &&
           windowRect.top <= monitorInfo.rcMonitor.top + tolerance &&
           windowRect.right >= monitorInfo.rcMonitor.right - tolerance &&
           windowRect.bottom >= monitorInfo.rcMonitor.bottom - tolerance;
}

void LogFullscreenCandidate(HWND window) {
    wchar_t className[128] = L"";
    wchar_t title[256] = L"";
    GetClassNameW(window, className, ARRAYSIZE(className));
    GetWindowTextW(window, title, ARRAYSIZE(title));

    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);

    Wh_Log(L"Fullscreen candidate: hwnd=%p pid=%u class=\"%s\" title=\"%s\"",
           window,
           processId,
           className,
           title);
}

bool IsFullscreenOrPresentationNotificationState(HMONITOR monitor) {
    QUERY_USER_NOTIFICATION_STATE state = QUNS_ACCEPTS_NOTIFICATIONS;
    if (FAILED(SHQueryUserNotificationState(&state))) {
        return false;
    }

    if (state == QUNS_PRESENTATION_MODE) {
        return true;
    }

    if (state != QUNS_BUSY &&
        state != QUNS_RUNNING_D3D_FULL_SCREEN) {
        return false;
    }

    // The notification state is system-wide. Associate fullscreen/busy states
    // with the foreground monitor to avoid suppressing unrelated taskbars.
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

bool IsTaskbarVisiblyRevealed(HWND taskbarWindow, HMONITOR monitor) {
    RECT taskbarRect{};
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetWindowRect(taskbarWindow, &taskbarRect) ||
        !GetMonitorInfoW(monitor, &monitorInfo)) {
        return false;
    }

    RECT intersection{};
    if (!IntersectRect(&intersection, &taskbarRect, &monitorInfo.rcMonitor)) {
        return false;
    }

    // An auto-hidden taskbar normally leaves only a one-pixel edge onscreen.
    return intersection.bottom - intersection.top > 2;
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

using TrayUI_GetStuckRectForMonitor_t =
    bool(WINAPI*)(void* trayUi, HMONITOR monitor, RECT* rect);
TrayUI_GetStuckRectForMonitor_t TrayUI_GetStuckRectForMonitor_Original;

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
    if (message == g_queryTaskbarRectMessage) {
        bool succeeded = false;
        if (g_stuckRectQuery.pending.load(std::memory_order_acquire) &&
            g_stuckRectQuery.monitor &&
            TrayUI_GetStuckRectForMonitor_Original) {
            RECT rect{};
            succeeded = TrayUI_GetStuckRectForMonitor_Original(
                trayUi, g_stuckRectQuery.monitor, &rect);
            if (succeeded) {
                g_stuckRectQuery.rect = rect;
            } else {
                SetRectEmpty(&g_stuckRectQuery.rect);
            }
        }

        g_stuckRectQuery.succeeded.store(succeeded,
                                         std::memory_order_release);
        g_stuckRectQuery.pending.store(false, std::memory_order_release);

        if (handled) {
            *handled = true;
        }
        return succeeded ? 1 : 0;
    }

    if (message == WM_NCCREATE) {
        g_stuckRectQuery.pending.store(false, std::memory_order_release);
        g_trayUiWndProcObjects[window] = trayUi;
        g_mainTaskbarWindow.store(window, std::memory_order_relaxed);
        Wh_Log(L"Captured TrayUI object for taskbar %p", window);
    } else if (message == g_captureTaskbarObjectMessage) {
        g_trayUiWndProcObjects[window] = trayUi;
        g_mainTaskbarWindow.store(window, std::memory_order_relaxed);
        Wh_Log(L"Captured TrayUI object for taskbar %p", window);
        if (handled) {
            *handled = true;
        }
        return 0;
    } else if (message == WM_NCDESTROY) {
        g_trayUiWndProcObjects.erase(window);
        ForgetTaskbarState(window);
        if (g_mainTaskbarWindow.load(std::memory_order_relaxed) == window) {
            g_mainTaskbarWindow.store(nullptr, std::memory_order_relaxed);
            g_stuckRectQuery.pending.store(false,
                                           std::memory_order_release);
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
    if (message == WM_NCCREATE) {
        g_secondaryTrayObjects[window] = secondaryTray;
        Wh_Log(L"Captured CSecondaryTray object for taskbar %p", window);
    } else if (message == g_captureTaskbarObjectMessage) {
        g_secondaryTrayObjects[window] = secondaryTray;
        Wh_Log(L"Captured CSecondaryTray object for taskbar %p", window);
        return 0;
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

// Undocumented Windows 11 taskbar methods.
//
// inputDeviceKind 0 corresponds to the mouse path used by Windows' taskbar
// implementation. Reason 7 is the coordinator's
// IsPointerOverTaskbarFrameChanged update reason.
constexpr int kReasonIsPointerOverTaskbarFrameChanged = 7;

using ViewCoordinator_IsExpanded_t =
    bool(WINAPI*)(void* viewCoordinator,
                  HWND taskbarWindow);

ViewCoordinator_IsExpanded_t
    ViewCoordinator_IsExpanded_Original;

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
}

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

    // HandleIsPointerOverTaskbarFrameChanged isn't sufficient on every Windows
    // 11 build when the physical pointer remains above the real one-pixel
    // taskbar frame. While this mod owns the synthetic pointer-over state,
    // explicitly authorize expansion in the coordinator's decision function.
    if (g_syntheticExpansionUpdateInProgress &&
        g_syntheticPointerOverTaskbars.contains(taskbarWindow)) {
        Wh_Log(L"ShouldTaskbarBeExpanded: forcing true for taskbar %p "
               L"(requested=%d)",
               taskbarWindow,
               expanded);
        return true;
    }

    bool result =
        ViewCoordinator_ShouldTaskbarBeExpanded_Original(
            viewCoordinator,
            taskbarWindow,
            expanded);

    Wh_Log(L"ShouldTaskbarBeExpanded: native result=%d for taskbar %p "
           L"(requested=%d)",
           result,
           taskbarWindow,
           expanded);
    return result;
}

using ViewCoordinator_UpdateIsExpanded_t =
    void(WINAPI*)(void* viewCoordinator,
                  HWND taskbarWindow,
                  int reason);

ViewCoordinator_UpdateIsExpanded_t
    ViewCoordinator_UpdateIsExpanded_Original;

void WINAPI ViewCoordinator_UpdateIsExpanded_Hook(
    void* viewCoordinator,
    HWND taskbarWindow,
    int reason) {
    RememberViewCoordinator(taskbarWindow, viewCoordinator);

    Wh_Log(L"UpdateIsExpanded: taskbar=%p reason=%d synthetic=%d",
           taskbarWindow,
           reason,
           g_syntheticPointerOverTaskbars.contains(taskbarWindow));

    ViewCoordinator_UpdateIsExpanded_Original(
        viewCoordinator,
        taskbarWindow,
        reason);
}

bool SetSyntheticPointerOverOnUiThread(HWND taskbarWindow,
                                       bool isPointerOver) {
    if (!isPointerOver && taskbarWindow &&
        !g_syntheticPointerOverTaskbars.contains(taskbarWindow)) {
        return true;
    }

    if (!taskbarWindow || !IsWindow(taskbarWindow) ||
        !ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original ||
        !ViewCoordinator_UpdateIsExpanded_Original) {
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

        g_syntheticExpansionUpdateInProgress = true;
        ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original(
            viewCoordinator,
            taskbarWindow,
            true,
            0);

        ViewCoordinator_UpdateIsExpanded_Original(
            viewCoordinator,
            taskbarWindow,
            kReasonIsPointerOverTaskbarFrameChanged);
        g_syntheticExpansionUpdateInProgress = false;

        bool expanded = false;
        if (ViewCoordinator_IsExpanded_Original) {
            expanded = ViewCoordinator_IsExpanded_Original(
                viewCoordinator,
                taskbarWindow);
            Wh_Log(L"Coordinator expansion result after synthetic enter: %d",
                   expanded);
        }

        // IsExpanded can be asynchronous. If verification is available and it
        // still reports collapsed, fully undo the synthetic enter before the
        // caller tries the native timer fallback.
        if (ViewCoordinator_IsExpanded_Original && !expanded) {
            g_syntheticPointerOverTaskbars.erase(taskbarWindow);
            ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original(
                viewCoordinator,
                taskbarWindow,
                false,
                0);
            ViewCoordinator_UpdateIsExpanded_Original(
                viewCoordinator,
                taskbarWindow,
                kReasonIsPointerOverTaskbarFrameChanged);
            return false;
        }
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

        ViewCoordinator_UpdateIsExpanded_Original(
            viewCoordinator,
            taskbarWindow,
            kReasonIsPointerOverTaskbarFrameChanged);

        if (ViewCoordinator_IsExpanded_Original) {
            bool expanded = ViewCoordinator_IsExpanded_Original(
                viewCoordinator,
                taskbarWindow);
            Wh_Log(L"Coordinator expansion result after synthetic leave: %d",
                   expanded);
        }
    }

    return true;
}

bool BringTaskbarToFrontWithinCurrentBandOnUiThread(
    HWND taskbarWindow) {
    if (!taskbarWindow || !IsWindow(taskbarWindow)) {
        return false;
    }

    // Reorder only within Explorer's current Z-order band. This brings the
    // taskbar above ordinary windows when appropriate without forcing or
    // clearing WS_EX_TOPMOST, preserving fullscreen behavior and compatibility
    // with mods that intentionally manage the taskbar's Z-order policy.
    const bool isTopmost =
        (GetWindowLongPtrW(taskbarWindow, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
    if (!SetWindowPos(taskbarWindow,
                      isTopmost ? HWND_TOPMOST : HWND_TOP,
                      0,
                      0,
                      0,
                      0,
                      SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                          SWP_NOOWNERZORDER)) {
        Wh_Log(L"Couldn't bring taskbar %p to the front of its Z-order "
               L"band: %u",
               taskbarWindow,
               GetLastError());
        return false;
    }

    Wh_Log(L"Brought taskbar %p to the front of its %s Z-order band",
           taskbarWindow,
           isTopmost ? L"topmost" : L"normal");
    return true;
}

bool RevealTaskbarOnUiThread(HWND taskbarWindow) {
    // Establish coordinator ownership first. If this fails, leave the legacy
    // path untouched and let the caller use Explorer's native timer fallback,
    // avoiding two independent reveal mechanisms for the same attempt.
    const bool coordinatorExpanded =
        SetSyntheticPointerOverOnUiThread(taskbarWindow, true);

    bool shellWindowRevealed = false;
    bool broughtToFront = false;
    if (coordinatorExpanded) {
        // TrayUI::Unhide physically slides Shell_TrayWnd onscreen after the
        // coordinator has established the matching logical expanded state.
        shellWindowRevealed =
            InvokeNativeShellUnhideOnUiThread(taskbarWindow);
        broughtToFront =
            BringTaskbarToFrontWithinCurrentBandOnUiThread(taskbarWindow);
    }

    Wh_Log(L"Reveal result for taskbar %p: shell=%d coordinator=%d "
           L"zorder=%d",
           taskbarWindow,
           shellWindowRevealed,
           coordinatorExpanded,
           broughtToFront);

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

// This window belongs to Explorer's taskbar UI thread, so custom operations
// sent here can safely invoke ViewCoordinator and taskbar window timers.
using CTaskBand_v_WndProc_t =
    LRESULT(WINAPI*)(void* taskBand,
                     HWND window,
                     UINT message,
                     WPARAM wParam,
                     LPARAM lParam);

CTaskBand_v_WndProc_t CTaskBand_v_WndProc_Original;

constexpr UINT_PTR kTrayUITimerUnhide = 3;

bool CancelNativeUnhideTimerOnUiThread(HWND taskbarWindow) {
    return taskbarWindow && IsWindow(taskbarWindow) &&
           KillTimer(taskbarWindow, kTrayUITimerUnhide);
}

bool TriggerNativeUnhideTimerOnUiThread(HWND taskbarWindow) {
    if (!taskbarWindow || !IsWindow(taskbarWindow)) {
        return false;
    }

    // Avoid carrying over a stale repeating timer if Explorer changed how it
    // handles timer ID 3, then arm the established native unhide timer.
    KillTimer(taskbarWindow, kTrayUITimerUnhide);
    UINT_PTR timerResult =
        SetTimer(taskbarWindow, kTrayUITimerUnhide, 1, nullptr);

    if (!timerResult) {
        Wh_Log(L"Native unhide timer failed for taskbar %p: %u",
               taskbarWindow,
               GetLastError());
        return false;
    }

    // Put the hidden taskbar in the correct Z-order before Explorer processes
    // the asynchronous timer. A second reinforcement is sent by the worker
    // after the timer has had time to reveal the window.
    BringTaskbarToFrontWithinCurrentBandOnUiThread(taskbarWindow);

    Wh_Log(L"Requested native unhide timer for taskbar %p",
           taskbarWindow);
    return true;
}

LRESULT WINAPI CTaskBand_v_WndProc_Hook(
    void* taskBand,
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam) {
    if (message == g_uiThreadMessage) {
        const UiOperation operation =
            static_cast<UiOperation>(wParam);
        HWND targetTaskbar =
            reinterpret_cast<HWND>(lParam);

        switch (operation) {
            case kUiRevealTaskbar:
                return RevealTaskbarOnUiThread(targetTaskbar) ? 1 : 0;

            case kUiClearSyntheticPointerOver:
                return SetSyntheticPointerOverOnUiThread(
                           targetTaskbar,
                           false)
                           ? 1
                           : 0;

            case kUiClearAllSyntheticPointerOver:
                ClearAllSyntheticPointerOverOnUiThread();
                return 1;

            case kUiTriggerNativeUnhideTimer:
                return TriggerNativeUnhideTimerOnUiThread(targetTaskbar)
                           ? 1
                           : 0;

            case kUiCancelNativeUnhideTimer:
                return CancelNativeUnhideTimerOnUiThread(targetTaskbar)
                           ? 1
                           : 0;

            case kUiBringTaskbarToFront:
                return BringTaskbarToFrontWithinCurrentBandOnUiThread(
                           targetTaskbar)
                           ? 1
                           : 0;
        }

        return 0;
    }

    return CTaskBand_v_WndProc_Original(
        taskBand,
        window,
        message,
        wParam,
        lParam);
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
    HWND taskBandWindow = GetTaskBandWindow();
    if (!taskBandWindow) {
        return UiOperationResult::kNotSent;
    }

    DWORD_PTR result = 0;
    SetLastError(ERROR_SUCCESS);
    if (!SendMessageTimeoutW(
            taskBandWindow,
            g_uiThreadMessage,
            operation,
            reinterpret_cast<LPARAM>(taskbarWindow),
            SMTO_ABORTIFHUNG | SMTO_BLOCK,
            timeoutMs,
            &result)) {
        const DWORD error = GetLastError();
        Wh_Log(L"UI-thread operation timed out or failed: operation=%u "
               L"error=%u",
               static_cast<unsigned>(operation),
               error);

        // Once SendMessageTimeoutW has begun a cross-thread send, a timeout or
        // an unspecified failure doesn't prove that the target WndProc won't
        // process the message later. Only a vanished target window is treated
        // as definitely not sent; all other failures are conservatively
        // considered indeterminate.
        if (error == ERROR_INVALID_WINDOW_HANDLE) {
            return UiOperationResult::kNotSent;
        }
        return UiOperationResult::kIndeterminate;
    }

    return result != 0 ? UiOperationResult::kCompletedSuccess
                       : UiOperationResult::kCompletedFailure;
}

struct ActivationWorkerState {
    HWND activeTaskbar = nullptr;
    HMONITOR activeTaskbarMonitor = nullptr;
    HWND fallbackTaskbar = nullptr;
    HMONITOR fallbackTaskbarMonitor = nullptr;
    HMONITOR activationCandidateMonitor = nullptr;
    bool armed = true;
    UINT_PTR releaseTimerId = 0;
    UINT_PTR retryTimerId = 0;
    UINT_PTR nativeTimerCleanupTimerId = 0;
    bool releaseRetryPending = false;
    ULONGLONG activationCandidateSince = 0;
    ULONGLONG lastActivationCheck = 0;
    ULONGLONG fallbackRearmNotBefore = 0;
    unsigned suppressionFailures = 0;
    unsigned fallbackHideCount = 0;
    unsigned appliedSettingsGeneration = 0;
};

void CancelWorkerTimer(UINT_PTR& timerId) {
    if (!timerId) {
        return;
    }

    KillTimer(nullptr, timerId);
    timerId = 0;
}

void ScheduleWorkerTimer(UINT delayMs, UINT_PTR& timerId) {
    if (timerId) {
        return;
    }

    // With hWnd == nullptr, Windows allocates and returns the timer ID. The
    // caller stores that returned value and compares WM_TIMER against it.
    timerId = SetTimer(nullptr, 0, std::max(delayMs, 1U), nullptr);
    if (!timerId) {
        Wh_Log(L"Failed to create worker timer: %u", GetLastError());
    }
}

bool ClearActiveTaskbar(ActivationWorkerState& state,
                        DWORD timeoutMs = 250,
                        bool scheduleRetry = true) {
    CancelWorkerTimer(state.releaseTimerId);

    if (state.activeTaskbar && IsWindow(state.activeTaskbar)) {
        const UiOperationResult clearResult =
            SendUiOperation(kUiClearSyntheticPointerOver,
                            state.activeTaskbar,
                            timeoutMs);
        if (!UiOperationSucceeded(clearResult)) {
            // Keep ownership and retry instead of forgetting synthetic state
            // that may still force Explorer's coordinator to remain expanded.
            // An indeterminate result is especially important: the clear may
            // still be waiting in the taskbar UI thread's sent-message queue.
            if (scheduleRetry) {
                state.releaseRetryPending = true;
                ScheduleWorkerTimer(kReleaseRetryMs,
                                    state.releaseTimerId);
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

void CancelFallbackNativeTimer(ActivationWorkerState& state) {
    CancelWorkerTimer(state.nativeTimerCleanupTimerId);
    if (state.fallbackTaskbar && IsWindow(state.fallbackTaskbar)) {
        (void)SendUiOperation(kUiCancelNativeUnhideTimer,
                              state.fallbackTaskbar,
                              500);
    }
}

void ClearFallbackTaskbar(ActivationWorkerState& state) {
    CancelFallbackNativeTimer(state);
    state.fallbackTaskbar = nullptr;
    state.fallbackTaskbarMonitor = nullptr;
    state.armed = true;
}

void ScheduleRetry(ActivationWorkerState& state, UINT delayMs) {
    ScheduleWorkerTimer(delayMs, state.retryTimerId);
}

UINT GetSuppressionRetryDelay(ActivationWorkerState& state,
                              int cooldownMs) {
    state.suppressionFailures =
        std::min(state.suppressionFailures + 1, 8U);
    const UINT base = static_cast<UINT>(std::max(cooldownMs, 200));
    const unsigned shift = std::min(state.suppressionFailures - 1, 4U);
    return std::min(base << shift, kSuppressionBackoffMaxMs);
}

void ScheduleSuppressedRetry(ActivationWorkerState& state,
                             int cooldownMs) {
    ScheduleRetry(state, GetSuppressionRetryDelay(state, cooldownMs));
}

void ResetActivationCandidate(ActivationWorkerState& state) {
    state.activationCandidateMonitor = nullptr;
    state.activationCandidateSince = 0;
}

void ResetFallbackBackoff(ActivationWorkerState& state) {
    state.fallbackHideCount = 0;
    state.fallbackRearmNotBefore = 0;
}

UINT GetFallbackRearmDelay(ActivationWorkerState& state) {
    state.fallbackHideCount = std::min(state.fallbackHideCount + 1, 4U);
    const unsigned shift = state.fallbackHideCount - 1;
    return std::min(kFallbackRearmBaseMs << shift,
                    kSuppressionBackoffMaxMs);
}

void ProcessPointerState(ActivationWorkerState& state) {
    const unsigned settingsGeneration =
        g_settingsGeneration.load(std::memory_order_acquire);
    if (state.appliedSettingsGeneration != settingsGeneration) {
        CancelWorkerTimer(state.retryTimerId);
        if (!ClearActiveTaskbar(state)) {
            // Keep the old generation pending. Once the release retry succeeds,
            // the complete settings reset will run instead of being forgotten.
            return;
        }

        ClearFallbackTaskbar(state);
        ResetActivationCandidate(state);
        ResetFallbackBackoff(state);
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
        ClearFallbackTaskbar(state);
        CancelWorkerTimer(state.retryTimerId);
        ResetActivationCandidate(state);
        ResetFallbackBackoff(state);
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

    if (state.fallbackTaskbar) {
        const bool sameMonitor =
            pointerMonitor == state.fallbackTaskbarMonitor;
        if (!IsWindow(state.fallbackTaskbar) || !insideActivationBand ||
            !sameMonitor) {
            ClearFallbackTaskbar(state);
            CancelWorkerTimer(state.retryTimerId);
            ResetFallbackBackoff(state);
        } else if (IsTaskbarVisiblyRevealed(state.fallbackTaskbar,
                                            state.fallbackTaskbarMonitor)) {
            // Explorer owns the fallback reveal and its matching hide. Poll at
            // a low rate only while this exceptional path remains active.
            ScheduleRetry(state, kFallbackStatusPollMs);
            return;
        } else {
            // Avoid a rapid hide/reveal oscillation if Explorer's timer path
            // hides again before a ViewCoordinator becomes available.
            ClearFallbackTaskbar(state);
            CancelWorkerTimer(state.retryTimerId);
            const UINT rearmDelay = GetFallbackRearmDelay(state);
            state.fallbackRearmNotBefore = now + rearmDelay;
            ScheduleRetry(state, rearmDelay);
            return;
        }
    }

    if (state.activeTaskbar) {
        if (state.releaseRetryPending) {
            if (!ClearActiveTaskbar(state)) {
                return;
            }
        } else if (!IsWindow(state.activeTaskbar)) {
            CancelWorkerTimer(state.releaseTimerId);
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
                CancelWorkerTimer(state.releaseTimerId);
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
                                    state.releaseTimerId);
                return;
            }
        }
    }

    if (!insideActivationBand) {
        state.armed = true;
        state.suppressionFailures = 0;
        CancelWorkerTimer(state.retryTimerId);
        ResetActivationCandidate(state);
        ResetFallbackBackoff(state);
        return;
    }

    if (!state.armed) {
        CancelWorkerTimer(state.retryTimerId);
        return;
    }

    if (now < state.fallbackRearmNotBefore) {
        ScheduleRetry(
            state,
            static_cast<UINT>(state.fallbackRearmNotBefore - now));
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
    CancelWorkerTimer(state.retryTimerId);

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

    state.suppressionFailures = 0;

    const UiOperationResult revealResult =
        SendUiOperation(kUiRevealTaskbar, taskbarWindow);
    if (UiOperationMayHaveRun(revealResult)) {
        if (revealResult == UiOperationResult::kIndeterminate) {
            Wh_Log(L"Taskbar reveal timed out; assuming coordinator ownership "
                   L"until the matching release succeeds: %p",
                   taskbarWindow);
        } else {
            Wh_Log(L"Activated taskbar %p through native "
                   L"shell/coordinator path",
                   taskbarWindow);
        }

        ClearFallbackTaskbar(state);
        ResetFallbackBackoff(state);
        ResetActivationCandidate(state);
        state.activeTaskbar = taskbarWindow;
        state.activeTaskbarMonitor = pointerMonitor;
        state.armed = false;
        return;
    }

    if (g_settings.nativeTimerFallback.load(std::memory_order_relaxed)) {
        const UiOperationResult fallbackResult =
            SendUiOperation(kUiTriggerNativeUnhideTimer, taskbarWindow);
        if (UiOperationMayHaveRun(fallbackResult)) {
            Wh_Log(L"Activated taskbar %p through native timer fallback%s",
                   taskbarWindow,
                   fallbackResult == UiOperationResult::kIndeterminate
                       ? L" (delivery indeterminate)"
                       : L"");

            ResetActivationCandidate(state);
            state.fallbackTaskbar = taskbarWindow;
            state.fallbackTaskbarMonitor = pointerMonitor;
            state.armed = false;
            ScheduleWorkerTimer(kNativeTimerCleanupMs,
                                state.nativeTimerCleanupTimerId);
            ScheduleRetry(state, kFallbackStatusPollMs);
            return;
        }
    }

    Wh_Log(L"Threshold entered, but no reveal backend succeeded");
    ScheduleSuppressedRetry(state, cooldown);
}

void HandleReleaseTimer(ActivationWorkerState& state) {
    CancelWorkerTimer(state.releaseTimerId);

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
               L"using a 50 ms fallback timer",
               GetLastError());
        fallbackPollTimerId =
            SetTimer(nullptr, 0, 50, nullptr);
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

                case WM_TIMER:
                    if (state.releaseTimerId &&
                        message.wParam == state.releaseTimerId) {
                        HandleReleaseTimer(state);
                    } else if (state.retryTimerId &&
                               message.wParam == state.retryTimerId) {
                        CancelWorkerTimer(state.retryTimerId);
                        ProcessPointerState(state);
                    } else if (state.nativeTimerCleanupTimerId &&
                               message.wParam ==
                                   state.nativeTimerCleanupTimerId) {
                        CancelWorkerTimer(
                            state.nativeTimerCleanupTimerId);
                        if (state.fallbackTaskbar &&
                            IsWindow(state.fallbackTaskbar)) {
                            (void)SendUiOperation(
                                kUiCancelNativeUnhideTimer,
                                state.fallbackTaskbar,
                                500);

                            // The timer reveal is asynchronous. Bring the
                            // taskbar forward within its existing Z-order band
                            // after Explorer has moved it onscreen.
                            if (state.fallbackTaskbarMonitor &&
                                IsTaskbarVisiblyRevealed(
                                    state.fallbackTaskbar,
                                    state.fallbackTaskbarMonitor)) {
                                (void)SendUiOperation(
                                    kUiBringTaskbarToFront,
                                    state.fallbackTaskbar,
                                    500);
                            }
                        }
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

    CancelWorkerTimer(state.retryTimerId);
    CancelWorkerTimer(state.nativeTimerCleanupTimerId);
    ClearFallbackTaskbar(state);

    for (int attempt = 0; state.activeTaskbar && attempt < 3; attempt++) {
        if (ClearActiveTaskbar(state, 1000, false)) {
            break;
        }
        Sleep(50);
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
                LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::IsExpanded(unsigned __int64))",
            },
            &ViewCoordinator_IsExpanded_Original,
            nullptr,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged(unsigned __int64,bool,enum winrt::WindowsUdk::UI::Shell::InputDeviceKind))",
            },
            &ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original,
            ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Hook,
            true,
        },
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
            ViewCoordinator_UpdateIsExpanded_Hook,
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
                LR"(public: virtual bool __cdecl TrayUI::GetStuckRectForMonitor(struct HMONITOR__ *,struct tagRECT *))",
            },
            &TrayUI_GetStuckRectForMonitor_Original,
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
            true,
        },
        {
            {
                LR"(private: virtual __int64 __cdecl CSecondaryTray::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))",
            },
            &CSecondaryTray_WndProc_Original,
            CSecondaryTray_WndProc_Hook,
            true,
        },
        {
            {
                LR"(protected: virtual __int64 __cdecl CTaskBand::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))",
            },
            &CTaskBand_v_WndProc_Original,
            CTaskBand_v_WndProc_Hook,
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

    g_uiThreadMessage =
        RegisterWindowMessageW(L"taskbar-activation-threshold.ui");
    g_captureTaskbarObjectMessage =
        RegisterWindowMessageW(L"taskbar-activation-threshold.capture");
    g_queryTaskbarRectMessage =
        RegisterWindowMessageW(L"taskbar-activation-threshold.stuck-rect");

    if (!g_uiThreadMessage || !g_captureTaskbarObjectMessage ||
        !g_queryTaskbarRectMessage) {
        Wh_Log(L"RegisterWindowMessageW failed: ui=%u capture=%u rect=%u",
               g_uiThreadMessage,
               g_captureTaskbarObjectMessage,
               g_queryTaskbarRectMessage);
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
    }

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

    g_stopEvent =
        CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"Starting activation worker");

    CaptureExistingTaskbarObjects();

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

    g_workerThread = CreateThread(
        nullptr,
        0,
        ActivationWorkerThread,
        nullptr,
        0,
        nullptr);

    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        return;
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"Stopping activation worker");

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, INFINITE);
    }

    UiOperationResult clearResult =
        SendUiOperation(kUiClearAllSyntheticPointerOver, nullptr, 1000);
    if (!UiOperationSucceeded(clearResult)) {
        Sleep(50);
        clearResult =
            SendUiOperation(kUiClearAllSyntheticPointerOver, nullptr, 2000);
        if (!UiOperationSucceeded(clearResult)) {
            Wh_Log(L"Final synthetic pointer-over cleanup wasn't confirmed");
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing");

    if (g_workerThread) {
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    g_stuckRectQuery.pending.store(false, std::memory_order_relaxed);
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
