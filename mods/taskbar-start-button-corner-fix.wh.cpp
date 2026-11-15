// ==WindhawkMod==
// @id              taskbar-start-button-corner-fix
// @name            Start Menu Corner Click Fix
// @description     Fixes the issue where clicking in the corner of the taskbar doesn't open the Start menu on multi monitor setups.
// @version         2.1
// @author          Alchemy
// @github          https://github.com/alchemyyy
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -loleacc
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Start Menu Corner Click Fix

Fixes the issue where clicking the bottom-left corner of a left-aligned taskbar doesn't reliably open the Start menu when another monitor is adjacent to that edge.

The mod presses the taskbar's own Start button through the accessibility interface Explorer provides for it. Start opens the same way as when the button itself is clicked, without first moving focus to the taskbar. If `openStartWhenButtonUnavailable` is enabled and the taskbar has no Start button, the mod presses the Windows key instead. Errors while pressing an existing Start button don't trigger this fallback.

Windows normally opens a keyboard-triggered Start menu on the primary monitor. To open the Start menu on the monitor containing the mouse pointer, install the [Start menu open location](https://windhawk.net/mods/start-menu-open-location) mod and use its default cursor-based monitor setting.

## Requirements

- Windows 11 64-bit (tested on 25H2)
- A left-aligned taskbar
- Windhawk v1.5 or later
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- edgeThickness: 5
  $name: Edge thickness
  $description: Thickness of the L-shaped edge region in pixels at 100% scaling
- edgeLength: 15
  $name: Edge length
  $description: Length of each arm in pixels at 100% scaling
- openStartWhenButtonUnavailable: true
  $name: Open Start when button unavailable
  $description: Press the Windows key if the taskbar has no Start button
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <oleacc.h>

#include <atomic>

#include <winrt/base.h>

#include <windhawk_utils.h>

namespace {

constexpr wchar_t kInputSiteClassName[] =
    L"Windows.UI.Input.InputSite.WindowClass";
constexpr wchar_t kContentBridgeClassName[] =
    L"Windows.UI.Composition.DesktopWindowContentBridge";
constexpr wchar_t kPrimaryTaskbarClassName[] = L"Shell_TrayWnd";
constexpr wchar_t kSecondaryTaskbarClassName[] = L"Shell_SecondaryTrayWnd";
constexpr wchar_t kStartButtonClassName[] = L"Start";
constexpr wchar_t kExplorerAdvancedRegistryPath[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
constexpr wchar_t kTaskbarAlignmentRegistryValue[] = L"TaskbarAl";
constexpr int kDefaultEdgeThickness = 5;
constexpr int kDefaultEdgeLength = 15;
constexpr DWORD kLeftTaskbarAlignment = 0;

struct Settings {
    std::atomic<int> edgeThickness{kDefaultEdgeThickness};
    std::atomic<int> edgeLength{kDefaultEdgeLength};
    std::atomic<bool> openStartWhenButtonUnavailable{true};
};

struct CornerPointerState {
    HWND inputSiteWindow = nullptr;
    HWND taskbarWindow = nullptr;
    UINT32 pointerID = 0;
    bool suppressing = false;
};

enum class StartButtonOperationResult {
    Succeeded,
    Unavailable,
    Failed,
};

Settings g_settings;
thread_local CornerPointerState g_cornerPointerState;

SRWLOCK g_hookOperationLock = SRWLOCK_INIT;
WNDPROC g_inputSiteWindowProcTarget = nullptr;
WNDPROC g_inputSiteWindowProcOriginal = nullptr;
bool g_inputSiteWindowProcHookPending = false;
bool g_hookLifecycleUnloading = false;
std::atomic<bool> g_initialized{false};

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD extendedStyle,
                                           LPCWSTR className,
                                           LPCWSTR windowName,
                                           DWORD style,
                                           int x,
                                           int y,
                                           int width,
                                           int height,
                                           HWND parentWindow,
                                           HMENU menu,
                                           HINSTANCE instance,
                                           void* parameter,
                                           DWORD band);

CreateWindowInBand_t g_createWindowInBandOriginal = nullptr;

int GetPositiveSetting(const std::atomic<int>& setting, int defaultValue) {
    int value = setting.load(std::memory_order_relaxed);
    return value > 0 ? value : defaultValue;
}

int GetDPIScaledSetting(const std::atomic<int>& setting,
                        int defaultValue,
                        HWND window) {
    int value = GetPositiveSetting(setting, defaultValue);
    UINT DPI = GetDpiForWindow(window);
    if (DPI == 0) {
        DPI = USER_DEFAULT_SCREEN_DPI;
    }

    int scaledValue =
        MulDiv(value, static_cast<int>(DPI), USER_DEFAULT_SCREEN_DPI);
    return scaledValue > 0 ? scaledValue : value;
}

bool IsClassName(HWND window, const wchar_t* expectedClassName) {
    wchar_t className[128];
    int length = GetClassNameW(window, className, ARRAYSIZE(className));
    return length > 0 && wcscmp(className, expectedClassName) == 0;
}

bool IsTaskbarWindow(HWND window) {
    if (!window) {
        return false;
    }

    DWORD processID = 0;
    if (!GetWindowThreadProcessId(window, &processID) ||
        processID != GetCurrentProcessId()) {
        return false;
    }

    return IsClassName(window, kPrimaryTaskbarClassName) ||
           IsClassName(window, kSecondaryTaskbarClassName);
}

bool IsTaskbarLeftAligned() {
    DWORD taskbarAlignment = 0;
    DWORD taskbarAlignmentSize = sizeof(taskbarAlignment);
    LSTATUS queryResult = RegGetValueW(
        HKEY_CURRENT_USER, kExplorerAdvancedRegistryPath,
        kTaskbarAlignmentRegistryValue, RRF_RT_REG_DWORD, nullptr,
        &taskbarAlignment, &taskbarAlignmentSize);
    return queryResult == ERROR_SUCCESS &&
           taskbarAlignment == kLeftTaskbarAlignment;
}

bool IsTaskbarInputSite(HWND window) {
    if (!window || !IsClassName(window, kInputSiteClassName)) {
        return false;
    }

    HWND parentWindow = GetParent(window);
    if (!parentWindow ||
        !IsClassName(parentWindow, kContentBridgeClassName)) {
        return false;
    }

    return IsTaskbarWindow(GetAncestor(window, GA_ROOT));
}

bool IsMousePointer(UINT32 pointerID) {
    if (pointerID == 0) {
        return true;
    }

    POINTER_INPUT_TYPE pointerType = PT_POINTER;
    return GetPointerType(pointerID, &pointerType) && pointerType == PT_MOUSE;
}

bool IsInCornerRegion(HWND taskbarWindow, POINT point) {
    RECT taskbarRectangle;
    if (!GetWindowRect(taskbarWindow, &taskbarRectangle) ||
        !PtInRect(&taskbarRectangle, point)) {
        return false;
    }

    HMONITOR taskbarMonitor =
        MonitorFromWindow(taskbarWindow, MONITOR_DEFAULTTONULL);
    HMONITOR pointMonitor = MonitorFromPoint(point, MONITOR_DEFAULTTONULL);
    if (!taskbarMonitor || taskbarMonitor != pointMonitor) {
        return false;
    }

    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(taskbarMonitor, &monitorInfo)) {
        return false;
    }

    POINT adjacentPoint = {
        monitorInfo.rcMonitor.left - 1,
        point.y,
    };
    HMONITOR adjacentMonitor =
        MonitorFromPoint(adjacentPoint, MONITOR_DEFAULTTONULL);
    if (!adjacentMonitor || adjacentMonitor == taskbarMonitor) {
        return false;
    }

    MONITORINFO adjacentMonitorInfo = {};
    adjacentMonitorInfo.cbSize = sizeof(adjacentMonitorInfo);
    if (!GetMonitorInfoW(adjacentMonitor, &adjacentMonitorInfo) ||
        adjacentMonitorInfo.rcMonitor.right != monitorInfo.rcMonitor.left) {
        return false;
    }

    LONG distanceFromLeft = point.x - monitorInfo.rcMonitor.left;
    LONG distanceFromBottom = monitorInfo.rcMonitor.bottom - 1 - point.y;
    int edgeLength = GetDPIScaledSetting(
        g_settings.edgeLength, kDefaultEdgeLength, taskbarWindow);
    if (distanceFromLeft < 0 || distanceFromLeft >= edgeLength ||
        distanceFromBottom < 0 || distanceFromBottom >= edgeLength) {
        return false;
    }

    int edgeThickness = GetDPIScaledSetting(
        g_settings.edgeThickness, kDefaultEdgeThickness, taskbarWindow);
    if (distanceFromLeft >= edgeThickness &&
        distanceFromBottom >= edgeThickness) {
        return false;
    }

    return IsTaskbarLeftAligned();
}

POINT GetPointerPoint(LPARAM parameter) {
    POINT point = {};
    point.x = GET_X_LPARAM(parameter);
    point.y = GET_Y_LPARAM(parameter);
    return point;
}

void ClearCornerPointerState() {
    g_cornerPointerState = {};
}

bool IsActiveCornerPointer(HWND inputSiteWindow, UINT32 pointerID) {
    return g_cornerPointerState.suppressing &&
           g_cornerPointerState.inputSiteWindow == inputSiteWindow &&
           g_cornerPointerState.pointerID == pointerID;
}

// Every taskbar keeps a hidden legacy Start button window. Its accessible
// default action asks Explorer's Start coordinator to toggle Start without
// going through the XAML Start button.
StartButtonOperationResult PressTaskbarStartButton(HWND taskbarWindow) {
    if (!IsTaskbarWindow(taskbarWindow)) {
        Wh_Log(L"Taskbar %p no longer exists", taskbarWindow);
        return StartButtonOperationResult::Failed;
    }

    HWND startButtonWindow =
        FindWindowExW(taskbarWindow, nullptr, kStartButtonClassName, nullptr);
    if (!startButtonWindow) {
        Wh_Log(L"No Start button is available for taskbar %p",
               taskbarWindow);
        return StartButtonOperationResult::Unavailable;
    }

    winrt::com_ptr<IAccessible> startButton;
    HRESULT operationResult = AccessibleObjectFromWindow(
        startButtonWindow, static_cast<DWORD>(OBJID_CLIENT),
        __uuidof(IAccessible), startButton.put_void());
    if (FAILED(operationResult) || !startButton) {
        Wh_Log(L"Failed to get the accessible Start button %p for taskbar %p "
               L"(error: 0x%08X)",
               startButtonWindow, taskbarWindow,
               static_cast<unsigned int>(operationResult));
        return StartButtonOperationResult::Failed;
    }

    VARIANT self = {};
    self.vt = VT_I4;
    self.lVal = CHILDID_SELF;
    operationResult = startButton->accDoDefaultAction(self);
    if (FAILED(operationResult)) {
        Wh_Log(L"Failed to press the Start button %p for taskbar %p "
               L"(error: 0x%08X)",
               startButtonWindow, taskbarWindow,
               static_cast<unsigned int>(operationResult));
        return StartButtonOperationResult::Failed;
    }

    Wh_Log(L"Pressed the Start button %p for taskbar %p", startButtonWindow,
           taskbarWindow);
    return StartButtonOperationResult::Succeeded;
}

bool SendWindowsKeyPress() {
    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_LWIN;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_LWIN;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    UINT inputCount = static_cast<UINT>(ARRAYSIZE(inputs));
    UINT sentInputCount = SendInput(inputCount, inputs, sizeof(INPUT));
    if (sentInputCount != inputCount) {
        Wh_Log(L"Failed to send the Windows key press (sent: %u, error: "
               L"%lu)",
               sentInputCount, GetLastError());
        return false;
    }

    Wh_Log(L"Sent the Windows key press");
    return true;
}

void HandleUnavailableStartButton(HWND taskbarWindow) {
    if (!g_settings.openStartWhenButtonUnavailable.load(
            std::memory_order_relaxed)) {
        Wh_Log(L"Start button unavailable for taskbar %p; Windows key "
               L"fallback is disabled",
               taskbarWindow);
        return;
    }

    Wh_Log(L"Start button unavailable for taskbar %p; falling back to the "
           L"Windows key",
           taskbarWindow);
    SendWindowsKeyPress();
}

LRESULT HandleInputSiteWindowMessage(WNDPROC originalWindowProc,
                                     HWND window,
                                     UINT message,
                                     WPARAM wParam,
                                     LPARAM lParam) {
    if (!originalWindowProc) {
        return DefWindowProcW(window, message, wParam, lParam);
    }

    // The hooked procedure is shared by every InputSite window in Explorer
    switch (message) {
        case WM_POINTERDOWN:
        case WM_POINTERUPDATE:
        case WM_POINTERUP:
        case WM_POINTERCAPTURECHANGED:
        case WM_POINTERLEAVE:
        case WM_CANCELMODE:
        case WM_NCDESTROY:
            break;

        default:
            return originalWindowProc(window, message, wParam, lParam);
    }

    switch (message) {
        case WM_POINTERDOWN: {
            UINT32 pointerID = GET_POINTERID_WPARAM(wParam);
            if (!IS_POINTER_FIRSTBUTTON_WPARAM(wParam) ||
                !IsMousePointer(pointerID)) {
                break;
            }

            HWND taskbarWindow = GetAncestor(window, GA_ROOT);
            if (!IsTaskbarWindow(taskbarWindow)) {
                break;
            }

            ClearCornerPointerState();

            POINT point = GetPointerPoint(lParam);
            if (!IsInCornerRegion(taskbarWindow, point)) {
                break;
            }

            g_cornerPointerState.inputSiteWindow = window;
            g_cornerPointerState.taskbarWindow = taskbarWindow;
            g_cornerPointerState.pointerID = pointerID;
            g_cornerPointerState.suppressing = true;

            Wh_Log(L"Intercepted corner pointer down at (%ld, %ld), "
                   L"pointer %u, InputSite %p, taskbar %p",
                   point.x, point.y, pointerID, window, taskbarWindow);

            return 0;
        }

        case WM_POINTERUPDATE: {
            UINT32 pointerID = GET_POINTERID_WPARAM(wParam);
            if (IsActiveCornerPointer(window, pointerID)) {
                if (!IS_POINTER_FIRSTBUTTON_WPARAM(wParam) ||
                    IS_POINTER_CANCELED_WPARAM(wParam)) {
                    ClearCornerPointerState();
                    break;
                }

                return 0;
            }
            break;
        }

        case WM_POINTERUP: {
            UINT32 pointerID = GET_POINTERID_WPARAM(wParam);
            if (!IsActiveCornerPointer(window, pointerID)) {
                break;
            }

            HWND clickedTaskbarWindow =
                g_cornerPointerState.taskbarWindow;
            POINT point = GetPointerPoint(lParam);
            bool pointerCanceled = IS_POINTER_CANCELED_WPARAM(wParam);
            bool releasedInCorner = !pointerCanceled &&
                IsInCornerRegion(clickedTaskbarWindow, point);
            ClearCornerPointerState();

            if (releasedInCorner) {
                Wh_Log(L"Intercepted corner pointer up at (%ld, %ld), "
                       L"pointer %u",
                       point.x, point.y, pointerID);
                if (PressTaskbarStartButton(clickedTaskbarWindow) ==
                    StartButtonOperationResult::Unavailable) {
                    HandleUnavailableStartButton(clickedTaskbarWindow);
                }
            } else {
                if (pointerCanceled) {
                    Wh_Log(L"Canceled corner click after pointer cancellation");
                } else {
                    Wh_Log(L"Canceled corner click after pointer left the "
                           L"region");
                }
            }

            return 0;
        }

        case WM_POINTERCAPTURECHANGED: {
            UINT32 pointerID = GET_POINTERID_WPARAM(wParam);
            if (IsActiveCornerPointer(window, pointerID)) {
                ClearCornerPointerState();
            }
            break;
        }

        case WM_POINTERLEAVE: {
            UINT32 pointerID = GET_POINTERID_WPARAM(wParam);
            if (IsActiveCornerPointer(window, pointerID)) {
                ClearCornerPointerState();
            }
            break;
        }

        case WM_CANCELMODE:
            if (g_cornerPointerState.inputSiteWindow == window) {
                ClearCornerPointerState();
            }
            break;

        case WM_NCDESTROY:
            if (g_cornerPointerState.inputSiteWindow == window) {
                ClearCornerPointerState();
            }
            break;
    }

    return originalWindowProc(window, message, wParam, lParam);
}

// InputSite rejects subclassed window procedures, so hook its shared procedure
LRESULT CALLBACK InputSiteWindowProc_Hook(HWND window,
                                          UINT message,
                                          WPARAM wParam,
                                          LPARAM lParam) {
    return HandleInputSiteWindowMessage(
        g_inputSiteWindowProcOriginal, window, message, wParam, lParam);
}

// The caller holds g_hookOperationLock exclusively
bool ApplyPendingInputSiteWindowProcHookLocked() {
    if (!g_inputSiteWindowProcHookPending) {
        return true;
    }

    if (g_hookLifecycleUnloading) {
        return false;
    }

    if (!Wh_ApplyHookOperations()) {
        Wh_Log(L"Failed to apply the InputSite window procedure hook");
        return false;
    }

    g_inputSiteWindowProcHookPending = false;
    return true;
}

bool HookInputSiteWindowProc(HWND inputSiteWindow) {
    if (!IsTaskbarInputSite(inputSiteWindow)) {
        return false;
    }

    WNDPROC windowProc = reinterpret_cast<WNDPROC>(
        GetWindowLongPtrW(inputSiteWindow, GWLP_WNDPROC));
    if (!windowProc) {
        Wh_Log(L"Failed to get InputSite window procedure for %p "
               L"(error: %lu)",
               inputSiteWindow, GetLastError());
        return false;
    }

    AcquireSRWLockExclusive(&g_hookOperationLock);
    if (g_hookLifecycleUnloading) {
        ReleaseSRWLockExclusive(&g_hookOperationLock);
        return false;
    }

    if (g_inputSiteWindowProcTarget &&
        g_inputSiteWindowProcTarget != windowProc) {
        Wh_Log(L"Taskbar InputSite window %p has an unexpected window "
               L"procedure %p; the shared procedure %p is already hooked",
               inputSiteWindow, reinterpret_cast<void*>(windowProc),
               reinterpret_cast<void*>(g_inputSiteWindowProcTarget));
        ReleaseSRWLockExclusive(&g_hookOperationLock);
        return false;
    }

    if (g_inputSiteWindowProcTarget) {
        bool hookApplied =
            ApplyPendingInputSiteWindowProcHookLocked();
        ReleaseSRWLockExclusive(&g_hookOperationLock);
        return hookApplied;
    }

    if (!WindhawkUtils::SetFunctionHook(
            windowProc, InputSiteWindowProc_Hook,
            &g_inputSiteWindowProcOriginal)) {
        Wh_Log(L"Failed to configure InputSite window procedure hook "
               L"for %p",
               inputSiteWindow);
        ReleaseSRWLockExclusive(&g_hookOperationLock);
        return false;
    }

    g_inputSiteWindowProcTarget = windowProc;
    g_inputSiteWindowProcHookPending =
        g_initialized.load(std::memory_order_acquire);
    Wh_Log(L"Configured the shared taskbar InputSite window procedure "
           L"hook: window %p, procedure %p",
           inputSiteWindow, reinterpret_cast<void*>(windowProc));

    bool hookApplied = ApplyPendingInputSiteWindowProcHookLocked();
    ReleaseSRWLockExclusive(&g_hookOperationLock);
    return hookApplied;
}

BOOL CALLBACK FindInputSiteCallback(HWND window, LPARAM) {
    if (IsTaskbarInputSite(window)) {
        HookInputSiteWindowProc(window);
    }

    return TRUE;
}

BOOL CALLBACK FindTaskbarCallback(HWND window, LPARAM) {
    if (IsTaskbarWindow(window)) {
        EnumChildWindows(window, FindInputSiteCallback, 0);
    }

    return TRUE;
}

bool FindAndHookExistingInputSite() {
    EnumWindows(FindTaskbarCallback, 0);

    AcquireSRWLockExclusive(&g_hookOperationLock);
    bool hookConfigured = g_inputSiteWindowProcTarget != nullptr;
    bool hookApplied = ApplyPendingInputSiteWindowProcHookLocked();
    ReleaseSRWLockExclusive(&g_hookOperationLock);
    return hookConfigured && hookApplied;
}

HWND WINAPI CreateWindowInBand_Hook(DWORD extendedStyle,
                                    LPCWSTR className,
                                    LPCWSTR windowName,
                                    DWORD style,
                                    int x,
                                    int y,
                                    int width,
                                    int height,
                                    HWND parentWindow,
                                    HMENU menu,
                                    HINSTANCE instance,
                                    void* parameter,
                                    DWORD band) {
    HWND window = g_createWindowInBandOriginal(
        extendedStyle, className, windowName, style, x, y, width, height,
        parentWindow, menu, instance, parameter, band);

    if (window && className && !IS_INTRESOURCE(className) &&
        wcscmp(className, kInputSiteClassName) == 0) {
        HookInputSiteWindowProc(window);
    }

    return window;
}

bool ConfigureCreateWindowInBandHook() {
    HMODULE user32Module = GetModuleHandleW(L"user32.dll");
    if (!user32Module) {
        Wh_Log(L"Failed to get user32.dll");
        return false;
    }

    CreateWindowInBand_t createWindowInBand =
        reinterpret_cast<CreateWindowInBand_t>(
            GetProcAddress(user32Module, "CreateWindowInBand"));
    if (!createWindowInBand) {
        Wh_Log(L"Failed to find CreateWindowInBand");
        return false;
    }

    if (!WindhawkUtils::SetFunctionHook(
            createWindowInBand, CreateWindowInBand_Hook,
            &g_createWindowInBandOriginal)) {
        Wh_Log(L"Failed to configure CreateWindowInBand hook");
        return false;
    }

    return true;
}

void LoadSettings() {
    g_settings.edgeThickness.store(
        Wh_GetIntSetting(L"edgeThickness"), std::memory_order_relaxed);
    g_settings.edgeLength.store(
        Wh_GetIntSetting(L"edgeLength"), std::memory_order_relaxed);
    g_settings.openStartWhenButtonUnavailable.store(
        Wh_GetIntSetting(L"openStartWhenButtonUnavailable") != 0,
        std::memory_order_relaxed);
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing");

    LoadSettings();

    bool creationHookConfigured = ConfigureCreateWindowInBandHook();
    bool inputSiteHookConfigured = FindAndHookExistingInputSite();
    if (!creationHookConfigured && !inputSiteHookConfigured) {
        Wh_Log(L"Failed to configure taskbar input hooks");
        return FALSE;
    }

    g_initialized.store(true, std::memory_order_release);
    Wh_Log(L"Initialized");
    return TRUE;
}

void Wh_ModAfterInit() {
    // Scan again in case the taskbar was recreated during initialization
    FindAndHookExistingInputSite();
}

void Wh_ModBeforeUninit() {
    AcquireSRWLockExclusive(&g_hookOperationLock);
    g_hookLifecycleUnloading = true;
    ReleaseSRWLockExclusive(&g_hookOperationLock);
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitialized");
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    FindAndHookExistingInputSite();
}
