// ==WindhawkMod==
// @id              taskbar-start-button-corner-fix
// @name            Start Menu Corner Click Fix
// @description     Fixes the issue where clicking in the corner of the taskbar doesn't open the Start menu on multi monitor setups.
// @version         2.0
// @author          Alchemy
// @github          https://github.com/alchemyyy
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Start Menu Corner Click Fix

Fixes the issue where clicking the bottom-left corner of a left-aligned taskbar doesn't reliably open the Start menu when another monitor is adjacent to that edge.

If `openStartWhenButtonUnavailable` is enabled and no Start button is exposed
through UI Automation, the mod presses the Windows key. UI Automation errors
and activation failures don't trigger this fallback.

Windows normally opens a keyboard-triggered Start menu on the primary monitor.
To open the Start menu on the monitor containing the mouse pointer,
install the [Start menu open location](https://windhawk.net/mods/start-menu-open-location) mod and use its default cursor-based monitor setting.

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
  $description: Press the Windows key if the taskbar exposes no Start button through UI Automation
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <objbase.h>
#include <uiautomation.h>

#include <atomic>
#include <unordered_map>

#include <winrt/base.h>

#include <windhawk_utils.h>

namespace {

constexpr wchar_t kInputSiteClassName[] =
    L"Windows.UI.Input.InputSite.WindowClass";
constexpr wchar_t kContentBridgeClassName[] =
    L"Windows.UI.Composition.DesktopWindowContentBridge";
constexpr wchar_t kPrimaryTaskbarClassName[] = L"Shell_TrayWnd";
constexpr wchar_t kSecondaryTaskbarClassName[] = L"Shell_SecondaryTrayWnd";
constexpr wchar_t kStartButtonAutomationID[] = L"StartButton";
constexpr wchar_t kExplorerAdvancedRegistryPath[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
constexpr wchar_t kTaskbarAlignmentRegistryValue[] = L"TaskbarAl";
constexpr int kDefaultEdgeThickness = 5;
constexpr int kDefaultEdgeLength = 15;
constexpr DWORD kLeftTaskbarAlignment = 0;
constexpr DWORD kUIAutomationTimeoutMilliseconds = 1000;

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

using StartButtonCache =
    std::unordered_map<HWND, winrt::com_ptr<IUIAutomationElement>>;

Settings g_settings;
thread_local CornerPointerState g_cornerPointerState;

SRWLOCK g_hookOperationLock = SRWLOCK_INIT;
WNDPROC g_inputSiteWindowProcTarget = nullptr;
WNDPROC g_inputSiteWindowProcOriginal = nullptr;
bool g_inputSiteWindowProcHookPending = false;
bool g_hookLifecycleUnloading = false;
std::atomic<bool> g_initialized{false};

SRWLOCK g_startMenuActivationLock = SRWLOCK_INIT;
HANDLE g_startMenuActivationEvent = nullptr;
HANDLE g_startMenuActivationThread = nullptr;
std::atomic<HWND> g_pendingStartMenuActivation{nullptr};
std::atomic<bool> g_acceptStartMenuActivations{true};

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

// Creates the reusable condition used to find each taskbar's Start button
winrt::com_ptr<IUIAutomationCondition> CreateStartButtonCondition(
    IUIAutomation* automation) {
    BSTR automationID = SysAllocString(kStartButtonAutomationID);
    if (!automationID) {
        Wh_Log(L"Failed to allocate the Start button automation ID");
        return {};
    }

    VARIANT automationIDValue = {};
    automationIDValue.vt = VT_BSTR;
    automationIDValue.bstrVal = automationID;

    winrt::com_ptr<IUIAutomationCondition> condition;
    HRESULT operationResult = automation->CreatePropertyCondition(
        UIA_AutomationIdPropertyId, automationIDValue, condition.put());
    SysFreeString(automationID);

    if (FAILED(operationResult) || !condition) {
        Wh_Log(L"Failed to create the Start button condition (error: "
               L"0x%08X)",
               static_cast<unsigned int>(operationResult));
        return {};
    }

    return condition;
}

void RemoveInvalidCachedStartButtons(StartButtonCache& startButtonCache) {
    StartButtonCache::iterator cachedStartButton =
        startButtonCache.begin();
    while (cachedStartButton != startButtonCache.end()) {
        if (IsTaskbarWindow(cachedStartButton->first)) {
            ++cachedStartButton;
            continue;
        }

        cachedStartButton = startButtonCache.erase(cachedStartButton);
    }
}

StartButtonOperationResult GetTaskbarStartButton(
    IUIAutomation* automation,
    IUIAutomationCondition* condition,
    StartButtonCache& startButtonCache,
    HWND taskbarWindow,
    winrt::com_ptr<IUIAutomationElement>& startButton) {
    startButton = nullptr;

    StartButtonCache::iterator cachedStartButton =
        startButtonCache.find(taskbarWindow);
    if (cachedStartButton != startButtonCache.end()) {
        startButton = cachedStartButton->second;
        return StartButtonOperationResult::Succeeded;
    }

    winrt::com_ptr<IUIAutomationElement> taskbarElement;
    HRESULT operationResult =
        automation->ElementFromHandle(taskbarWindow, taskbarElement.put());
    if (FAILED(operationResult) || !taskbarElement) {
        Wh_Log(L"Failed to get the UI Automation element for taskbar %p "
               L"(error: 0x%08X)",
               taskbarWindow, static_cast<unsigned int>(operationResult));
        return StartButtonOperationResult::Failed;
    }

    operationResult = taskbarElement->FindFirst(
        TreeScope_Descendants, condition, startButton.put());
    if (FAILED(operationResult)) {
        Wh_Log(L"Failed to find the Start button for taskbar %p (error: "
               L"0x%08X)",
               taskbarWindow, static_cast<unsigned int>(operationResult));
        return StartButtonOperationResult::Failed;
    }

    if (!startButton) {
        Wh_Log(L"No Start button is available for taskbar %p",
               taskbarWindow);
        return StartButtonOperationResult::Unavailable;
    }

    startButtonCache.emplace(taskbarWindow, startButton);
    return StartButtonOperationResult::Succeeded;
}

bool TryActivateStartButton(
    const winrt::com_ptr<IUIAutomationElement>& startButton,
    HWND taskbarWindow) {
    winrt::com_ptr<IUIAutomationTogglePattern> togglePattern;
    HRESULT operationResult = startButton->GetCurrentPatternAs(
        UIA_TogglePatternId, __uuidof(IUIAutomationTogglePattern),
        togglePattern.put_void());
    if (SUCCEEDED(operationResult) && togglePattern) {
        operationResult = togglePattern->Toggle();
        if (SUCCEEDED(operationResult)) {
            Wh_Log(L"Toggled the Start button for taskbar %p",
                   taskbarWindow);
            return true;
        }

        Wh_Log(L"Failed to toggle the Start button for taskbar %p (error: "
               L"0x%08X)",
               taskbarWindow, static_cast<unsigned int>(operationResult));
    } else {
        Wh_Log(L"The Start button toggle pattern isn't available for "
               L"taskbar %p (error: 0x%08X)",
               taskbarWindow, static_cast<unsigned int>(operationResult));
    }

    winrt::com_ptr<IUIAutomationInvokePattern> invokePattern;
    operationResult = startButton->GetCurrentPatternAs(
        UIA_InvokePatternId, __uuidof(IUIAutomationInvokePattern),
        invokePattern.put_void());
    if (FAILED(operationResult) || !invokePattern) {
        Wh_Log(L"The Start button invoke pattern isn't available for "
               L"taskbar %p (error: 0x%08X)",
               taskbarWindow, static_cast<unsigned int>(operationResult));
        return false;
    }

    operationResult = invokePattern->Invoke();
    if (FAILED(operationResult)) {
        Wh_Log(L"Failed to invoke the Start button for taskbar %p (error: "
               L"0x%08X)",
               taskbarWindow, static_cast<unsigned int>(operationResult));
        return false;
    }

    Wh_Log(L"Invoked the Start button for taskbar %p", taskbarWindow);
    return true;
}

// Activates the Start button belonging to the specified taskbar
StartButtonOperationResult ActivateTaskbarStartButton(
    IUIAutomation* automation,
    IUIAutomationCondition* condition,
    StartButtonCache& startButtonCache,
    HWND taskbarWindow) {
    RemoveInvalidCachedStartButtons(startButtonCache);
    if (!automation || !condition ||
        !IsTaskbarWindow(taskbarWindow)) {
        return StartButtonOperationResult::Failed;
    }

    bool startButtonWasCached =
        startButtonCache.find(taskbarWindow) != startButtonCache.end();
    winrt::com_ptr<IUIAutomationElement> startButton;
    StartButtonOperationResult lookupResult = GetTaskbarStartButton(
        automation, condition, startButtonCache, taskbarWindow,
        startButton);
    if (lookupResult != StartButtonOperationResult::Succeeded) {
        return lookupResult;
    }

    if (TryActivateStartButton(startButton, taskbarWindow)) {
        return StartButtonOperationResult::Succeeded;
    }

    startButtonCache.erase(taskbarWindow);
    if (!startButtonWasCached) {
        return StartButtonOperationResult::Failed;
    }

    Wh_Log(L"Refreshing the cached Start button for taskbar %p",
           taskbarWindow);
    lookupResult = GetTaskbarStartButton(
        automation, condition, startButtonCache, taskbarWindow,
        startButton);
    if (lookupResult != StartButtonOperationResult::Succeeded) {
        return lookupResult;
    }

    if (TryActivateStartButton(startButton, taskbarWindow)) {
        return StartButtonOperationResult::Succeeded;
    }

    startButtonCache.erase(taskbarWindow);
    return StartButtonOperationResult::Failed;
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

// Processes UI Automation away from the taskbar input thread
DWORD WINAPI StartMenuActivationThreadProc(void* parameter) {
    HANDLE activationEvent = static_cast<HANDLE>(parameter);

    HRESULT COMInitializationResult =
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    struct COMUninitializer {
        HRESULT initializationResult;

        ~COMUninitializer() {
            if (SUCCEEDED(initializationResult)) {
                CoUninitialize();
            }
        }
    } COMScope{COMInitializationResult};

    bool COMInitialized = SUCCEEDED(COMInitializationResult);
    if (!COMInitialized) {
        Wh_Log(L"Failed to initialize COM on the Start menu activation "
               L"thread (error: 0x%08X)",
               static_cast<unsigned int>(COMInitializationResult));
    }

    winrt::com_ptr<IUIAutomation2> automation;
    winrt::com_ptr<IUIAutomationCondition> startButtonCondition;
    if (COMInitialized) {
        automation = winrt::try_create_instance<IUIAutomation2>(
            __uuidof(CUIAutomation8));
        if (!automation) {
            Wh_Log(L"Failed to create the UI Automation instance");
        } else {
            // Bound provider calls so unload can't wait indefinitely
            HRESULT connectionTimeoutResult =
                automation->put_ConnectionTimeout(
                    kUIAutomationTimeoutMilliseconds);
            if (FAILED(connectionTimeoutResult)) {
                Wh_Log(L"Failed to set the UI Automation connection timeout "
                       L"(error: 0x%08X)",
                       static_cast<unsigned int>(connectionTimeoutResult));
            }

            HRESULT transactionTimeoutResult =
                automation->put_TransactionTimeout(
                    kUIAutomationTimeoutMilliseconds);
            if (FAILED(transactionTimeoutResult)) {
                Wh_Log(L"Failed to set the UI Automation transaction timeout "
                       L"(error: 0x%08X)",
                       static_cast<unsigned int>(transactionTimeoutResult));
            }

            startButtonCondition =
                CreateStartButtonCondition(automation.get());
        }
    }

    StartButtonCache startButtonCache;
    while (true) {
        DWORD waitResult =
            WaitForSingleObject(activationEvent, INFINITE);
        if (waitResult != WAIT_OBJECT_0) {
            DWORD error =
                waitResult == WAIT_FAILED ? GetLastError() : ERROR_SUCCESS;
            Wh_Log(L"Start menu activation thread wait failed "
                   L"(result: 0x%08X, error: %lu)",
                   waitResult, error);
            break;
        }

        if (!g_acceptStartMenuActivations.load(
                std::memory_order_acquire)) {
            break;
        }

        HWND taskbarWindow = g_pendingStartMenuActivation.exchange(
            nullptr, std::memory_order_acq_rel);
        if (!taskbarWindow) {
            continue;
        }

        StartButtonOperationResult activationResult =
            ActivateTaskbarStartButton(
                automation.get(), startButtonCondition.get(),
                startButtonCache, taskbarWindow);
        if (activationResult ==
            StartButtonOperationResult::Unavailable) {
            HandleUnavailableStartButton(taskbarWindow);
        }
    }

    return 0;
}

// The caller holds g_startMenuActivationLock exclusively
bool InitializeStartMenuActivationWorkerLocked() {
    if (g_startMenuActivationThread) {
        return true;
    }

    HANDLE activationEvent =
        CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!activationEvent) {
        Wh_Log(L"Failed to create the Start menu activation event "
               L"(error: %lu)",
               GetLastError());
        return false;
    }

    DWORD activationThreadID = 0;
    HANDLE activationThread = CreateThread(
        nullptr, 0, StartMenuActivationThreadProc, activationEvent, 0,
        &activationThreadID);
    if (!activationThread) {
        DWORD error = GetLastError();
        CloseHandle(activationEvent);
        Wh_Log(L"Failed to create the Start menu activation thread "
               L"(error: %lu)",
               error);
        return false;
    }

    g_startMenuActivationEvent = activationEvent;
    g_startMenuActivationThread = activationThread;
    Wh_Log(L"Started the Start menu activation thread %lu",
           activationThreadID);
    return true;
}

void ShutdownStartMenuActivationWorker() {
    HANDLE activationThread = nullptr;
    HANDLE activationEvent = nullptr;
    DWORD error = ERROR_SUCCESS;

    AcquireSRWLockExclusive(&g_startMenuActivationLock);
    g_acceptStartMenuActivations.store(
        false, std::memory_order_release);
    g_pendingStartMenuActivation.store(
        nullptr, std::memory_order_release);
    activationThread = g_startMenuActivationThread;
    activationEvent = g_startMenuActivationEvent;
    if (activationEvent && !SetEvent(activationEvent)) {
        error = GetLastError();
    }
    ReleaseSRWLockExclusive(&g_startMenuActivationLock);

    if (error != ERROR_SUCCESS) {
        Wh_Log(L"Failed to signal the Start menu activation thread to stop "
               L"(error: %lu)",
               error);
    }

    if (activationThread) {
        DWORD waitResult =
            WaitForSingleObject(activationThread, INFINITE);
        if (waitResult != WAIT_OBJECT_0) {
            Wh_Log(L"Failed to wait for the Start menu activation thread "
                   L"(result: 0x%08X, error: %lu)",
                   waitResult, GetLastError());
        }
    }

    AcquireSRWLockExclusive(&g_startMenuActivationLock);
    if (g_startMenuActivationThread) {
        CloseHandle(g_startMenuActivationThread);
        g_startMenuActivationThread = nullptr;
    }
    if (g_startMenuActivationEvent) {
        CloseHandle(g_startMenuActivationEvent);
        g_startMenuActivationEvent = nullptr;
    }
    ReleaseSRWLockExclusive(&g_startMenuActivationLock);
}

bool QueueStartMenuActivation(HWND taskbarWindow) {
    if (!taskbarWindow) {
        Wh_Log(L"Rejected Start menu activation without a taskbar window");
        return false;
    }

    bool activationQueued = false;
    bool activationsAccepted = false;
    DWORD error = ERROR_SUCCESS;

    AcquireSRWLockExclusive(&g_startMenuActivationLock);
    activationsAccepted = g_acceptStartMenuActivations.load(
        std::memory_order_acquire);
    if (activationsAccepted &&
        InitializeStartMenuActivationWorkerLocked()) {
        // Coalesce pending clicks by keeping the newest taskbar
        g_pendingStartMenuActivation.store(
            taskbarWindow, std::memory_order_release);
        if (SetEvent(g_startMenuActivationEvent)) {
            activationQueued = true;
        } else {
            error = GetLastError();
            HWND expectedTaskbarWindow = taskbarWindow;
            g_pendingStartMenuActivation.compare_exchange_strong(
                expectedTaskbarWindow, nullptr,
                std::memory_order_acq_rel);
        }
    }
    ReleaseSRWLockExclusive(&g_startMenuActivationLock);

    if (!activationQueued) {
        if (!activationsAccepted) {
            Wh_Log(L"Rejected Start menu activation for taskbar %p during "
                   L"shutdown",
                   taskbarWindow);
        } else if (error != ERROR_SUCCESS) {
            Wh_Log(L"Failed to queue Start menu activation for taskbar %p "
                   L"(error: %lu)",
                   taskbarWindow, error);
        }
        return false;
    }

    Wh_Log(L"Queued Start menu activation for taskbar %p", taskbarWindow);
    return true;
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
                QueueStartMenuActivation(clickedTaskbarWindow);
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

    ShutdownStartMenuActivationWorker();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitialized");
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    FindAndHookExistingInputSite();
}
