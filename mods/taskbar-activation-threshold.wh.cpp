// ==WindhawkMod==
// @id              taskbar-activation-threshold
// @name            Taskbar Activation Threshold
// @description     Reveal the auto-hidden Windows 11 taskbar from a configurable bottom activation band without moving the cursor
// @version         1.0.0
// @author          themagnificentoofman
// @github          https://github.com/themagnificentoofman
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshell32
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Activation Threshold

Reveals an auto-hidden Windows 11 taskbar when the pointer enters a
configurable activation band above the bottom edge of the monitor.

Unlike version 0.1, this version **never moves the mouse cursor**. It invokes
the Windows 11 taskbar's native pointer-over state on Explorer's UI thread.

## Compatibility approach

The mod doesn't modify:

- Taskbar position or geometry
- XAML layout or styles
- Taskbar height or icon size
- Animation duration
- Transparency
- Windows' auto-hide setting

It is therefore designed to coexist with styling, sizing, transparency,
animation-speed, clock, label, and icon mods which leave the taskbar at the
bottom of the screen.

## Requirements

- Windows 11
- The standard Windows 11 taskbar
- **Automatically hide the taskbar** enabled in Windows, unless
  `requireWindowsAutoHide` is disabled
- Bottom-positioned taskbars

## Known incompatibilities and limitations

- Taskbars moved to the top, left, right, or an arbitrary floating location
  aren't supported.
- ExplorerPatcher's legacy Windows 10 taskbar isn't supported by version 0.2.
- Mods which block or replace the taskbar's native expansion logic can
  conflict. In particular, keyboard-only or never-show auto-hide modes are
  intentionally incompatible with mouse activation.
- Microsoft can rename undocumented taskbar symbols in a Windows update. If
  that happens, the mod will need a symbol update.
- The taskbar's internal coordinator must have initialized before the first
  threshold activation. If the first attempt does nothing immediately after
  Explorer starts, leave and re-enter the activation band.

## Suggested starting values

- Activation threshold: `42`
- Poll interval: `8`
- Release delay: `120`
- Activation cooldown: `250`

## Version 0.2.4 fixes

- Adds the missing shell-window reveal layer. Windows 11's
  `ViewCoordinator::IsExpanded` can report true while the outer
  `Shell_TrayWnd` is still physically hidden below the screen.
- Calls the native `TrayUI::Unhide` or `CSecondaryTray::_Unhide` path before
  setting the Windows 11 synthetic pointer-over state.
- Captures the required taskbar objects after the mod loads, even though their
  normal creation messages occurred before injection.
- Keeps the pointer completely stationary.

## Version 0.2.3 fixes

- Explicitly calls `ViewCoordinator::UpdateIsExpanded` after changing the
  synthetic pointer-over state.
- Adds `ViewCoordinator::IsExpanded` verification to the diagnostic log.
- Falls back to Explorer's native unhide timer if the explicit coordinator
  update still reports the taskbar as collapsed.

## Version 0.2.2 fixes

- Forces `ShouldTaskbarBeExpanded` to return true while this mod owns the
  synthetic pointer-over state.
- Logs the taskbar expansion decision and update reason while diagnostic
  logging is enabled.

## Version 0.2.1 fixes

- Disables fullscreen suppression by default because some borderless or
  frameless maximized applications can resemble fullscreen windows.
- Adds a native taskbar unhide-timer fallback when the Windows 11
  `ViewCoordinator` hasn't been captured yet.
- Adds clearer diagnostic logging for threshold entry and suppression.

## Attribution and license

The Windows 11 `ViewCoordinator` symbol names and UI-thread invocation
technique were informed by the GPL-3.0-licensed **Taskbar auto-hide fine
tuning** Windhawk mod by m417z. Consequently, version 0.2 is distributed under
GPL-3.0.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- activationThresholdPx: 24
  $name: Activation threshold
  $description: >-
    Height in physical screen pixels of the activation band above the bottom
    edge. Recommended range: 12 to 48.
- pollIntervalMs: 8
  $name: Poll interval
  $description: >-
    How often the pointer position is checked, in milliseconds. Recommended:
    8 to 16.
- releaseDelayMs: 120
  $name: Release delay
  $description: >-
    Delay before releasing the synthetic pointer-over state after the pointer
    leaves both the activation band and the visible taskbar.
- activationCooldownMs: 200
  $name: Activation cooldown
  $description: >-
    Minimum time between reveal attempts, in milliseconds.
- requireWindowsAutoHide: true
  $name: Require Windows auto-hide
  $description: >-
    Only activate when Windows reports that taskbar auto-hide is enabled.
    Disable this when another mod manages auto-hide without enabling the
    Windows setting.
- ignoreFullscreenApps: false
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
- diagnosticLogging: false
  $name: Diagnostic logging
  $description: >-
    Write detailed activation and suppression information to the Windhawk log.
    Enable only while troubleshooting.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Settings {
    std::atomic<int> activationThresholdPx{24};
    std::atomic<int> pollIntervalMs{8};
    std::atomic<int> releaseDelayMs{120};
    std::atomic<int> activationCooldownMs{200};
    std::atomic<bool> requireWindowsAutoHide{true};
    std::atomic<bool> ignoreFullscreenApps{false};
    std::atomic<bool> ignoreWhileMouseButtonDown{true};
    std::atomic<bool> primaryMonitorOnly{false};
    std::atomic<bool> nativeTimerFallback{true};
    std::atomic<bool> diagnosticLogging{false};
};

Settings g_settings;

std::atomic<bool> g_taskbarViewDllLoaded{false};
std::atomic<bool> g_settingsChanged{false};

HANDLE g_stopEvent = nullptr;
HANDLE g_workerThread = nullptr;

// A private message used to marshal operations from the polling worker to
// Explorer's taskbar UI thread.
const UINT g_uiThreadMessage =
    RegisterWindowMessageW(L"Windhawk_TaskbarActivationThreshold_" WH_MOD_ID);

// Existing taskbar windows were created before this mod was injected. Sending
// this message through their hooked WndProc captures the relevant `this`
// pointers without restarting Explorer.
const UINT g_captureTaskbarObjectMessage =
    RegisterWindowMessageW(
        L"Windhawk_TaskbarActivationThreshold_Capture_" WH_MOD_ID);

enum UiOperation : WPARAM {
    kUiRevealTaskbar = 1,
    kUiClearSyntheticPointerOver = 2,
    kUiClearAllSyntheticPointerOver = 3,
};

// The following maps are accessed only on Explorer's taskbar UI thread.
// They provide the shell object needed to physically slide Shell_TrayWnd
// onscreen. ViewCoordinator alone only controls the Windows 11 taskbar view's
// logical expanded/collapsed state.
std::unordered_map<HWND, void*> g_trayUiWndProcObjects;
std::unordered_map<HWND, void*> g_secondaryTrayObjects;

void* g_trayUiVtableITrayComponentHost = nullptr;
void* g_secondaryTrayVtableISecondaryTray = nullptr;

// ViewCoordinator mappings are captured by hooks which run inside the taskbar
// implementation. Access is protected because a Windows update could move a
// callback to a different Explorer thread.
SRWLOCK g_viewCoordinatorLock = SRWLOCK_INIT;
std::unordered_map<HWND, void*> g_viewCoordinators;

// Accessed only on the taskbar UI thread.
std::unordered_set<HWND> g_syntheticPointerOverTaskbars;

template <typename T>
T ClampSetting(T value, T minimum, T maximum) {
    return std::clamp(value, minimum, maximum);
}

void LoadSettings() {
    g_settings.activationThresholdPx.store(
        ClampSetting(Wh_GetIntSetting(L"activationThresholdPx"), 1, 500),
        std::memory_order_relaxed);

    g_settings.pollIntervalMs.store(
        ClampSetting(Wh_GetIntSetting(L"pollIntervalMs"), 1, 1000),
        std::memory_order_relaxed);

    g_settings.releaseDelayMs.store(
        ClampSetting(Wh_GetIntSetting(L"releaseDelayMs"), 0, 5000),
        std::memory_order_relaxed);

    g_settings.activationCooldownMs.store(
        ClampSetting(Wh_GetIntSetting(L"activationCooldownMs"), 0, 10000),
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

    g_settings.diagnosticLogging.store(
        Wh_GetIntSetting(L"diagnosticLogging") != 0,
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

HWND FindMainTaskbarWindow() {
    HWND result = nullptr;
    EnumWindows(
        [](HWND window, LPARAM parameter) -> BOOL {
            auto* result = reinterpret_cast<HWND*>(parameter);

            DWORD processId = 0;
            GetWindowThreadProcessId(window, &processId);

            if (processId != GetCurrentProcessId()) {
                return TRUE;
            }

            wchar_t className[64];
            if (GetClassNameW(window, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *result = window;
                return FALSE;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));

    return result;
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

bool IsBottomPositionedTaskbar(HWND taskbarWindow, HMONITOR monitor) {
    RECT taskbarRect{};
    if (!GetWindowRect(taskbarWindow, &taskbarRect)) {
        return false;
    }

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return false;
    }

    const int monitorWidth =
        monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
    const int monitorHeight =
        monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
    const int taskbarWidth = taskbarRect.right - taskbarRect.left;
    const int taskbarHeight = taskbarRect.bottom - taskbarRect.top;

    // Reject vertical and detached taskbar windows. Visual "floating taskbar"
    // styles normally retain a full-width Shell_TrayWnd and remain compatible.
    const bool looksHorizontal =
        taskbarWidth >= monitorWidth / 2 &&
        taskbarHeight > 0 &&
        taskbarHeight <= monitorHeight / 2;

    if (!looksHorizontal) {
        return false;
    }

    const int tolerance = std::max(16, taskbarHeight + 16);

    // Handles both the visible and auto-hidden window positions.
    const bool nearBottom =
        std::abs(taskbarRect.bottom - monitorInfo.rcMonitor.bottom) <=
            tolerance ||
        std::abs(taskbarRect.top - monitorInfo.rcMonitor.bottom) <=
            tolerance ||
        (taskbarRect.top < monitorInfo.rcMonitor.bottom &&
         taskbarRect.bottom >= monitorInfo.rcMonitor.bottom - 2);

    return nearBottom;
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

bool IsDesktopWindow(HWND window) {
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

bool IsFullscreenWindowOnMonitor(HMONITOR monitor) {
    HWND foregroundWindow = GetForegroundWindow();

    if (!foregroundWindow || IsIconic(foregroundWindow) ||
        IsDesktopWindow(foregroundWindow)) {
        return false;
    }

    if (MonitorFromWindow(foregroundWindow, MONITOR_DEFAULTTONEAREST) !=
        monitor) {
        return false;
    }

    const LONG_PTR style =
        GetWindowLongPtrW(foregroundWindow, GWL_STYLE);

    // A regular maximized desktop window normally retains caption/frame
    // styles. Borderless and exclusive-fullscreen windows normally don't.
    if ((style & WS_CHILD) != 0 ||
        (style & (WS_CAPTION | WS_THICKFRAME)) != 0) {
        return false;
    }

    RECT windowRect{};
    if (!GetWindowRect(foregroundWindow, &windowRect)) {
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

bool IsPointInsideActivationBand(const POINT& pointer,
                                 const MONITORINFO& monitorInfo) {
    const int monitorHeight =
        monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

    const int threshold = std::min(
        g_settings.activationThresholdPx.load(std::memory_order_relaxed),
        monitorHeight);

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
        if (!IsBadReadPtr(candidate, sizeof(void*)) &&
            *candidate == targetVtable) {
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
    if (message == WM_NCCREATE ||
        message == g_captureTaskbarObjectMessage) {
        g_trayUiWndProcObjects[window] = trayUi;

        if (g_settings.diagnosticLogging.load(
                std::memory_order_relaxed)) {
            Wh_Log(L"Captured TrayUI object for taskbar %p", window);
        }
    } else if (message == WM_NCDESTROY) {
        g_trayUiWndProcObjects.erase(window);
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
    if (message == WM_NCCREATE ||
        message == g_captureTaskbarObjectMessage) {
        g_secondaryTrayObjects[window] = secondaryTray;

        if (g_settings.diagnosticLogging.load(
                std::memory_order_relaxed)) {
            Wh_Log(L"Captured CSecondaryTray object for taskbar %p",
                   window);
        }
    } else if (message == WM_NCDESTROY) {
        g_secondaryTrayObjects.erase(window);
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
            !TrayUI_Unhide_Original) {
            if (g_settings.diagnosticLogging.load(
                    std::memory_order_relaxed)) {
                Wh_Log(L"Native main-taskbar unhide unavailable: "
                       L"object=%d function=%d",
                       it != g_trayUiWndProcObjects.end(),
                       TrayUI_Unhide_Original != nullptr);
            }

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

        if (g_settings.diagnosticLogging.load(
                std::memory_order_relaxed)) {
            Wh_Log(L"Called TrayUI::Unhide for taskbar %p",
                   taskbarWindow);
        }

        return true;
    }

    if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        auto it = g_secondaryTrayObjects.find(taskbarWindow);

        if (it == g_secondaryTrayObjects.end() ||
            !CSecondaryTray_Unhide_Original) {
            if (g_settings.diagnosticLogging.load(
                    std::memory_order_relaxed)) {
                Wh_Log(L"Native secondary-taskbar unhide unavailable: "
                       L"object=%d function=%d",
                       it != g_secondaryTrayObjects.end(),
                       CSecondaryTray_Unhide_Original != nullptr);
            }

            return false;
        }

        CSecondaryTray_Unhide_Original(it->second, 0, 0);

        if (g_settings.diagnosticLogging.load(
                std::memory_order_relaxed)) {
            Wh_Log(L"Called CSecondaryTray::_Unhide for taskbar %p",
                   taskbarWindow);
        }

        return true;
    }

    return false;
}

void RememberViewCoordinator(HWND taskbarWindow, void* viewCoordinator) {
    if (!taskbarWindow || !viewCoordinator) {
        return;
    }

    AcquireSRWLockExclusive(&g_viewCoordinatorLock);
    g_viewCoordinators[taskbarWindow] = viewCoordinator;
    ReleaseSRWLockExclusive(&g_viewCoordinatorLock);
}

void ForgetInvalidViewCoordinator(HWND taskbarWindow) {
    AcquireSRWLockExclusive(&g_viewCoordinatorLock);
    g_viewCoordinators.erase(taskbarWindow);
    ReleaseSRWLockExclusive(&g_viewCoordinatorLock);
}

void* GetViewCoordinator(HWND taskbarWindow) {
    void* viewCoordinator = nullptr;

    AcquireSRWLockShared(&g_viewCoordinatorLock);

    auto it = g_viewCoordinators.find(taskbarWindow);
    if (it != g_viewCoordinators.end()) {
        viewCoordinator = it->second;
    }

    ReleaseSRWLockShared(&g_viewCoordinatorLock);
    return viewCoordinator;
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
    if (g_syntheticPointerOverTaskbars.contains(taskbarWindow)) {
        if (g_settings.diagnosticLogging.load(
                std::memory_order_relaxed)) {
            Wh_Log(L"ShouldTaskbarBeExpanded: forcing true for taskbar %p "
                   L"(requested=%d)",
                   taskbarWindow,
                   expanded);
        }

        return true;
    }

    bool result =
        ViewCoordinator_ShouldTaskbarBeExpanded_Original(
            viewCoordinator,
            taskbarWindow,
            expanded);

    if (g_settings.diagnosticLogging.load(
            std::memory_order_relaxed)) {
        Wh_Log(L"ShouldTaskbarBeExpanded: native result=%d for taskbar %p "
               L"(requested=%d)",
               result,
               taskbarWindow,
               expanded);
    }

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

    if (g_settings.diagnosticLogging.load(
            std::memory_order_relaxed)) {
        Wh_Log(L"UpdateIsExpanded: taskbar=%p reason=%d synthetic=%d",
               taskbarWindow,
               reason,
               g_syntheticPointerOverTaskbars.contains(taskbarWindow));
    }

    ViewCoordinator_UpdateIsExpanded_Original(
        viewCoordinator,
        taskbarWindow,
        reason);
}

bool SetSyntheticPointerOverOnUiThread(HWND taskbarWindow,
                                      bool isPointerOver) {
    if (!taskbarWindow || !IsWindow(taskbarWindow) ||
        !ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original ||
        !ViewCoordinator_UpdateIsExpanded_Original) {
        if (taskbarWindow && !IsWindow(taskbarWindow)) {
            ForgetInvalidViewCoordinator(taskbarWindow);
            g_syntheticPointerOverTaskbars.erase(taskbarWindow);
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

        // First update the coordinator's pointer-over state.
        ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original(
            viewCoordinator,
            taskbarWindow,
            true,
            0);

        // Some Windows 11 builds don't immediately reevaluate expansion when
        // HandleIsPointerOverTaskbarFrameChanged is invoked programmatically.
        // Force the same reason-7 state recomputation used by the taskbar.
        ViewCoordinator_UpdateIsExpanded_Original(
            viewCoordinator,
            taskbarWindow,
            kReasonIsPointerOverTaskbarFrameChanged);

        bool expanded = false;
        if (ViewCoordinator_IsExpanded_Original) {
            expanded = ViewCoordinator_IsExpanded_Original(
                viewCoordinator,
                taskbarWindow);
        }

        if (g_settings.diagnosticLogging.load(
                std::memory_order_relaxed)) {
            Wh_Log(L"Coordinator expansion result after synthetic enter: %d",
                   expanded);
        }

        // Return false when verification is available and the coordinator is
        // still collapsed. The caller can then try the native timer fallback.
        if (ViewCoordinator_IsExpanded_Original && !expanded) {
            g_syntheticPointerOverTaskbars.erase(taskbarWindow);
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

        if (g_settings.diagnosticLogging.load(
                std::memory_order_relaxed) &&
            ViewCoordinator_IsExpanded_Original) {
            bool expanded = ViewCoordinator_IsExpanded_Original(
                viewCoordinator,
                taskbarWindow);

            Wh_Log(L"Coordinator expansion result after synthetic leave: %d",
                   expanded);
        }
    }

    return true;
}


bool RevealTaskbarOnUiThread(HWND taskbarWindow) {
    // The maintained Windhawk taskbar implementation invokes both paths:
    // TrayUI::Unhide physically slides Shell_TrayWnd onscreen, while the
    // Windows 11 ViewCoordinator establishes the logical expanded state.
    const bool shellWindowRevealed =
        InvokeNativeShellUnhideOnUiThread(taskbarWindow);

    const bool coordinatorExpanded =
        SetSyntheticPointerOverOnUiThread(
            taskbarWindow,
            true);

    if (g_settings.diagnosticLogging.load(
            std::memory_order_relaxed)) {
        Wh_Log(L"Reveal result for taskbar %p: shell=%d coordinator=%d",
               taskbarWindow,
               shellWindowRevealed,
               coordinatorExpanded);
    }

    return shellWindowRevealed || coordinatorExpanded;
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
// sent here can safely invoke ViewCoordinator.
using CTaskBand_v_WndProc_t =
    LRESULT(WINAPI*)(void* taskBand,
                     HWND window,
                     UINT message,
                     WPARAM wParam,
                     LPARAM lParam);

CTaskBand_v_WndProc_t CTaskBand_v_WndProc_Original;

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
                return RevealTaskbarOnUiThread(targetTaskbar)
                           ? 1
                           : 0;

            case kUiClearSyntheticPointerOver:
                return SetSyntheticPointerOverOnUiThread(
                           targetTaskbar,
                           false)
                           ? 1
                           : 0;

            case kUiClearAllSyntheticPointerOver:
                ClearAllSyntheticPointerOverOnUiThread();
                return 1;
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

bool SendUiOperation(UiOperation operation, HWND taskbarWindow) {
    HWND taskBandWindow = GetTaskBandWindow();
    if (!taskBandWindow) {
        return false;
    }

    DWORD_PTR result = 0;

    if (!SendMessageTimeoutW(
            taskBandWindow,
            g_uiThreadMessage,
            operation,
            reinterpret_cast<LPARAM>(taskbarWindow),
            SMTO_ABORTIFHUNG | SMTO_BLOCK,
            250,
            &result)) {
        Wh_Log(L"UI-thread operation timed out or failed: %u",
               GetLastError());
        return false;
    }

    return result != 0;
}


constexpr UINT_PTR kTrayUITimerHide = 2;
constexpr UINT_PTR kTrayUITimerUnhide = 3;

bool TriggerNativeUnhideTimer(HWND taskbarWindow) {
    if (!taskbarWindow || !IsWindow(taskbarWindow)) {
        return false;
    }

    // Explorer's native taskbar hover path uses timer ID 3 for unhide. SetTimer
    // associates the timer with the taskbar window, so WM_TIMER is handled on
    // Explorer's taskbar thread. This does not move, spoof, or inject the mouse.
    UINT_PTR timerResult =
        SetTimer(taskbarWindow, kTrayUITimerUnhide, 1, nullptr);

    if (!timerResult) {
        Wh_Log(L"Native unhide timer failed for taskbar %p: %u",
               taskbarWindow,
               GetLastError());
        return false;
    }

    if (g_settings.diagnosticLogging.load(std::memory_order_relaxed)) {
        Wh_Log(L"Requested native unhide timer for taskbar %p",
               taskbarWindow);
    }

    return true;
}

DWORD WINAPI ActivationWorkerThread(LPVOID) {
    HWND activeTaskbar = nullptr;
    bool armed = true;
    ULONGLONG lastActivationAttempt = 0;
    ULONGLONG outsideSince = 0;

    while (true) {
        const int pollInterval =
            g_settings.pollIntervalMs.load(std::memory_order_relaxed);

        if (WaitForSingleObject(g_stopEvent, pollInterval) ==
            WAIT_OBJECT_0) {
            break;
        }

        if (g_settingsChanged.exchange(false,
                                       std::memory_order_relaxed)) {
            if (activeTaskbar) {
                SendUiOperation(kUiClearSyntheticPointerOver,
                                activeTaskbar);
            }

            activeTaskbar = nullptr;
            armed = true;
            outsideSince = 0;
        }

        POINT pointer{};
        if (!GetCursorPos(&pointer)) {
            continue;
        }

        HMONITOR pointerMonitor =
            MonitorFromPoint(pointer, MONITOR_DEFAULTTONULL);

        if (!pointerMonitor) {
            if (activeTaskbar) {
                SendUiOperation(kUiClearSyntheticPointerOver,
                                activeTaskbar);
                activeTaskbar = nullptr;
            }

            armed = true;
            outsideSince = 0;
            continue;
        }

        MONITORINFO monitorInfo{};
        monitorInfo.cbSize = sizeof(monitorInfo);

        if (!GetMonitorInfoW(pointerMonitor, &monitorInfo)) {
            continue;
        }

        const bool insideActivationBand =
            IsPointInsideActivationBand(pointer, monitorInfo);

        if (activeTaskbar) {
            if (!IsWindow(activeTaskbar)) {
                activeTaskbar = nullptr;
                armed = true;
                outsideSince = 0;
                continue;
            }

            const bool overVisibleTaskbar =
                IsPointOverTaskbar(activeTaskbar, pointer);

            if (insideActivationBand || overVisibleTaskbar) {
                outsideSince = 0;
                continue;
            }

            const ULONGLONG now = GetTickCount64();

            if (!outsideSince) {
                outsideSince = now;
                continue;
            }

            const int releaseDelay =
                g_settings.releaseDelayMs.load(
                    std::memory_order_relaxed);

            if (now - outsideSince <
                static_cast<ULONGLONG>(releaseDelay)) {
                continue;
            }

            SendUiOperation(kUiClearSyntheticPointerOver,
                            activeTaskbar);

            activeTaskbar = nullptr;
            armed = true;
            outsideSince = 0;
            continue;
        }

        if (!insideActivationBand) {
            armed = true;
            continue;
        }

        if (g_settings.diagnosticLogging.load(
                std::memory_order_relaxed) &&
            armed) {
            Wh_Log(L"Pointer entered activation band: x=%d y=%d bottom=%d",
                   pointer.x,
                   pointer.y,
                   monitorInfo.rcMonitor.bottom);
        }

        if (!armed) {
            continue;
        }

        if (g_settings.primaryMonitorOnly.load(
                std::memory_order_relaxed) &&
            (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) == 0) {
            continue;
        }

        if (g_settings.requireWindowsAutoHide.load(
                std::memory_order_relaxed) &&
            !IsWindowsAutoHideEnabled()) {
            continue;
        }

        if (g_settings.ignoreWhileMouseButtonDown.load(
                std::memory_order_relaxed) &&
            IsAnyMouseButtonDown()) {
            continue;
        }

        if (g_settings.ignoreFullscreenApps.load(
                std::memory_order_relaxed) &&
            IsFullscreenWindowOnMonitor(pointerMonitor)) {
            if (g_settings.diagnosticLogging.load(
                    std::memory_order_relaxed)) {
                Wh_Log(L"Activation suppressed: fullscreen app detected");
            }
            continue;
        }

        const ULONGLONG now = GetTickCount64();
        const int cooldown =
            g_settings.activationCooldownMs.load(
                std::memory_order_relaxed);

        if (now - lastActivationAttempt <
            static_cast<ULONGLONG>(cooldown)) {
            continue;
        }

        lastActivationAttempt = now;

        HWND taskbarWindow =
            FindTaskbarForMonitor(pointerMonitor);

        if (!taskbarWindow) {
            if (g_settings.diagnosticLogging.load(
                    std::memory_order_relaxed)) {
                Wh_Log(L"Activation suppressed: no bottom taskbar found");
            }
            continue;
        }

        bool activated =
            SendUiOperation(kUiRevealTaskbar,
                            taskbarWindow);

        if (activated) {
            Wh_Log(L"Activated taskbar %p through native shell/coordinator "
                   L"path",
                   taskbarWindow);

            activeTaskbar = taskbarWindow;
            armed = false;
            outsideSince = 0;
            continue;
        }

        if (g_settings.nativeTimerFallback.load(
                std::memory_order_relaxed) &&
            TriggerNativeUnhideTimer(taskbarWindow)) {
            Wh_Log(L"Activated taskbar %p through native timer fallback",
                   taskbarWindow);

            // No synthetic pointer-over state was created, so Windows owns the
            // normal hide behavior. Rearm only after the pointer leaves the
            // configured activation band.
            armed = false;
            outsideSince = 0;
            continue;
        }

        if (g_settings.diagnosticLogging.load(
                std::memory_order_relaxed)) {
            Wh_Log(L"Threshold entered, but no reveal backend succeeded");
        }
    }

    if (activeTaskbar) {
        SendUiOperation(kUiClearSyntheticPointerOver,
                        activeTaskbar);
    }

    return 0;
}

bool HookTaskbarViewSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::IsExpanded(unsigned __int64))",
            },
            &ViewCoordinator_IsExpanded_Original,
            nullptr,
        },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged(unsigned __int64,bool,enum winrt::WindowsUdk::UI::Shell::InputDeviceKind))",
            },
            &ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original,
            ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Hook,
        },
        {
            {
                LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded(unsigned __int64,bool))",
            },
            &ViewCoordinator_ShouldTaskbarBeExpanded_Original,
            ViewCoordinator_ShouldTaskbarBeExpanded_Hook,
        },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::UpdateIsExpanded(unsigned __int64,enum TaskbarTipTest::TaskbarExpandCollapseReason))",
            },
            &ViewCoordinator_UpdateIsExpanded_Original,
            ViewCoordinator_UpdateIsExpanded_Hook,
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
                LR"(const CSecondaryTray::`vftable'{for `ISecondaryTray'})",
            },
            &g_secondaryTrayVtableISecondaryTray,
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
                SendMessageW(window,
                             g_captureTaskbarObjectMessage,
                             0,
                             0);
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
    } else {
        g_taskbarViewDllLoaded.store(false, std::memory_order_relaxed);
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR libraryFileName,
                                   HANDLE file,
                                   DWORD flags) {
    HMODULE module =
        LoadLibraryExW_Original(libraryFileName, file, flags);

    if (module) {
        HandleLoadedTaskbarViewModule(module, libraryFileName);
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing version 0.2.4");

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
                } else {
                    g_taskbarViewDllLoaded.store(
                        false,
                        std::memory_order_relaxed);
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

    SendUiOperation(kUiClearAllSyntheticPointerOver, nullptr);
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

    AcquireSRWLockExclusive(&g_viewCoordinatorLock);
    g_viewCoordinators.clear();
    ReleaseSRWLockExclusive(&g_viewCoordinatorLock);

    g_trayUiWndProcObjects.clear();
    g_secondaryTrayObjects.clear();
    g_syntheticPointerOverTaskbars.clear();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");

    LoadSettings();
    g_settingsChanged.store(true, std::memory_order_relaxed);
}
