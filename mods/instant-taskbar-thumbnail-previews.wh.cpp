// ==WindhawkMod==
// @id              instant-taskbar-thumbnail-previews
// @name            Instant Taskbar Thumbnail Previews
// @description     Controls taskbar thumbnail preview show and close behavior.
// @version         1.0.0
// @author          Alchemy
// @github          https://github.com/alchemyyy
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Instant Taskbar Thumbnail Previews

Adds the ability to configure or remove the delays before Windows 11 shows and closes taskbar thumbnail previews,
as well as the ability to configure mouse actions that close them.

Both delay settings replace the corresponding Windows value, so they can make the transitions either shorter or longer than the stock delays.

The optional outside-click behavior arms the taskbar's native light-dismiss action while a thumbnail flyout is open.

The optional Start-button hover behavior closes an open thumbnail flyout directly from the Start button's XAML pointer-enter handler.

This mod targets the new Windows 11 taskbar used by Windows 11 24H2 and later.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- delayMs: 1
  $name: Thumbnail hover delay
  $description: Exact delay in milliseconds. The minimum value is 1, which is effectively immediate.
- closeDelayMs: 1
  $name: Thumbnail close delay
  $description: Exact delay after the pointer leaves the thumbnail area. The minimum value is 1, which is effectively immediate.
- closeOnOutsideClick: false
  $name: Close on outside click
  $description: Immediately close open thumbnail previews when clicking outside the taskbar and thumbnail surface.
- closeOnStartButtonHover: false
  $name: Close on Start button hover
  $description: Immediately close open thumbnail previews when hovering over the Start button.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>

#include <atomic>
#include <unordered_set>

#undef GetCurrentTime

#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.h>

#define MINIMUM_DELAY_MS 1
#define TIME_SPAN_TICKS_PER_MILLISECOND 10000LL

namespace {

std::atomic<LONGLONG> g_hoverDelayTimeSpan{
    MINIMUM_DELAY_MS * TIME_SPAN_TICKS_PER_MILLISECOND};
std::atomic<UINT> g_closeDelayMilliseconds{MINIMUM_DELAY_MS};
std::atomic<bool> g_closeOnOutsideClick{false};
std::atomic<bool> g_closeOnStartButtonHover{false};
std::atomic<bool> g_taskbarHooksQueued{false};
std::atomic<bool> g_taskbarViewHooksQueued{false};
std::atomic<bool> g_initialized{false};
std::atomic<bool> g_windowsLightDismissTrackingReliable{true};
SRWLOCK g_windowsLightDismissTaskbarHostsLock = SRWLOCK_INIT;
std::unordered_set<void*> g_windowsLightDismissTaskbarHosts;
thread_local bool g_inDismissTransition = false;
thread_local HWND g_activeTaskbarWindow = nullptr;
thread_local bool g_previewLightDismissArmed = false;
thread_local HWND g_previewLightDismissTaskbarWindow = nullptr;
thread_local UINT_PTR g_previewLightDismissRearmTimer = 0;
thread_local void* g_activeHoverFlyoutController = nullptr;
thread_local void* g_pointerOverFlyoutFrameModel = nullptr;
thread_local void* g_lastUnregisteredTaskbarHost = nullptr;

void* CTaskBand_ITaskListWndSite_Vftable;
void* CSecondaryTaskBand_ITaskListWndSite_Vftable;

using CTaskBandGetTaskbarHost_t =
    void*(WINAPI*)(void* object, void** taskbarHostSharedPointer);
CTaskBandGetTaskbarHost_t CTaskBandGetTaskbarHost_Original;
CTaskBandGetTaskbarHost_t CSecondaryTaskBandGetTaskbarHost_Original;

using ReferenceCountBaseDecrement_t = void(WINAPI*)(void* object);
ReferenceCountBaseDecrement_t ReferenceCountBaseDecrement_Original;

using TaskbarHostRegisterLightDismiss_t = void(WINAPI*)(void* object);
TaskbarHostRegisterLightDismiss_t TaskbarHostRegisterLightDismiss_Original;

using TaskbarHostUnregisterLightDismiss_t = void(WINAPI*)(void* object);
TaskbarHostUnregisterLightDismiss_t TaskbarHostUnregisterLightDismiss_Original;

using TaskbarControllerOnLightDismissTriggered_t = void(WINAPI*)(
    void* object,
    const void* taskbarModel,
    const void* eventArguments);
TaskbarControllerOnLightDismissTriggered_t
    TaskbarControllerOnLightDismissTriggered_Original;

constexpr int TASK_BAND_INTERFACE_SEARCH_LIMIT = 20;

enum class TaskbarWindowType {
    invalid,
    primary,
    secondary,
};

TaskbarWindowType GetTaskbarWindowType(HWND taskbarWindow) {
    WCHAR className[32] = {};
    if (!GetClassNameW(taskbarWindow, className, ARRAYSIZE(className))) {
        return TaskbarWindowType::invalid;
    }

    if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
        return TaskbarWindowType::primary;
    }

    if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        return TaskbarWindowType::secondary;
    }

    return TaskbarWindowType::invalid;
}

HWND NormalizeTaskbarWindow(HWND window) {
    if (!window) {
        return nullptr;
    }

    HWND rootWindow = GetAncestor(window, GA_ROOT);
    if (!rootWindow) {
        rootWindow = window;
    }

    if (GetTaskbarWindowType(rootWindow) ==
        TaskbarWindowType::invalid) {
        return nullptr;
    }

    DWORD processId = 0;
    DWORD threadId =
        GetWindowThreadProcessId(rootWindow, &processId);
    if (processId != GetCurrentProcessId() ||
        threadId != GetCurrentThreadId()) {
        return nullptr;
    }

    return rootWindow;
}

HWND FindTaskbarWindowForCursor() {
    POINT cursorPoint = {};
    if (!GetCursorPos(&cursorPoint)) {
        return nullptr;
    }

    HWND taskbarWindow =
        NormalizeTaskbarWindow(WindowFromPoint(cursorPoint));
    if (taskbarWindow) {
        return taskbarWindow;
    }

    HMONITOR cursorMonitor =
        MonitorFromPoint(cursorPoint, MONITOR_DEFAULTTONULL);
    if (!cursorMonitor) {
        return nullptr;
    }

    struct FIND_TASKBAR_CONTEXT {
        HMONITOR monitor;
        HWND taskbarWindow;
    };

    FIND_TASKBAR_CONTEXT context = {
        cursorMonitor,
        nullptr,
    };
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND window, LPARAM parameter) -> BOOL {
            FIND_TASKBAR_CONTEXT* context =
                reinterpret_cast<FIND_TASKBAR_CONTEXT*>(parameter);
            if (GetTaskbarWindowType(window) !=
                    TaskbarWindowType::invalid &&
                MonitorFromWindow(window, MONITOR_DEFAULTTONULL) ==
                    context->monitor) {
                context->taskbarWindow = window;
                return FALSE;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&context));

    return context.taskbarWindow;
}

class SharedPointerControlBlockGuard {
   public:
    explicit SharedPointerControlBlockGuard(void* controlBlock) noexcept
        : controlBlock_(controlBlock) {}

    SharedPointerControlBlockGuard(
        const SharedPointerControlBlockGuard&) = delete;
    SharedPointerControlBlockGuard& operator=(
        const SharedPointerControlBlockGuard&) = delete;

    ~SharedPointerControlBlockGuard() noexcept {
        if (controlBlock_ && ReferenceCountBaseDecrement_Original) {
            ReferenceCountBaseDecrement_Original(controlBlock_);
        }
    }

   private:
    void* controlBlock_;
};

template <typename Callback>
bool WithTaskbarHost(HWND taskbarWindow, const Callback& callback) {
    TaskbarWindowType taskbarWindowType =
        GetTaskbarWindowType(taskbarWindow);
    if (taskbarWindowType == TaskbarWindowType::invalid) {
        return false;
    }

    DWORD processId = 0;
    DWORD threadId =
        GetWindowThreadProcessId(taskbarWindow, &processId);
    if (processId != GetCurrentProcessId() ||
        threadId != GetCurrentThreadId()) {
        return false;
    }

    void* taskBandInterfaceVftable = nullptr;
    CTaskBandGetTaskbarHost_t getTaskbarHost = nullptr;
    HWND taskSwitchWindow = nullptr;
    switch (taskbarWindowType) {
        case TaskbarWindowType::primary:
            taskBandInterfaceVftable =
                CTaskBand_ITaskListWndSite_Vftable;
            getTaskbarHost = CTaskBandGetTaskbarHost_Original;
            taskSwitchWindow = reinterpret_cast<HWND>(
                GetPropW(taskbarWindow, L"TaskbandHWND"));
            break;

        case TaskbarWindowType::secondary:
            taskBandInterfaceVftable =
                CSecondaryTaskBand_ITaskListWndSite_Vftable;
            getTaskbarHost =
                CSecondaryTaskBandGetTaskbarHost_Original;
            taskSwitchWindow = FindWindowExW(
                taskbarWindow, nullptr, L"WorkerW", nullptr);
            break;

        case TaskbarWindowType::invalid:
            return false;
    }

    if (!taskBandInterfaceVftable || !getTaskbarHost ||
        !ReferenceCountBaseDecrement_Original || !taskSwitchWindow) {
        return false;
    }

    void* taskBand =
        reinterpret_cast<void*>(GetWindowLongPtrW(taskSwitchWindow, 0));
    if (!taskBand) {
        return false;
    }

    void* taskBandForTaskListWindowSite = taskBand;
    int interfaceIndex = 0;
    for (; interfaceIndex < TASK_BAND_INTERFACE_SEARCH_LIMIT;
         interfaceIndex++) {
        void* currentVftable =
            *reinterpret_cast<void**>(taskBandForTaskListWindowSite);
        if (currentVftable == taskBandInterfaceVftable) {
            break;
        }

        taskBandForTaskListWindowSite =
            reinterpret_cast<void**>(taskBandForTaskListWindowSite) + 1;
    }

    if (interfaceIndex == TASK_BAND_INTERFACE_SEARCH_LIMIT) {
        return false;
    }

    void* taskbarHostSharedPointer[2] = {};
    try {
        getTaskbarHost(
            taskBandForTaskListWindowSite, taskbarHostSharedPointer);
    } catch (...) {
        Wh_Log(L"Failed to retrieve the taskbar host");
        return false;
    }

    SharedPointerControlBlockGuard controlBlockGuard(
        taskbarHostSharedPointer[1]);
    if (!taskbarHostSharedPointer[0]) {
        return false;
    }

    try {
        return callback(taskbarHostSharedPointer[0]);
    } catch (...) {
        Wh_Log(L"Taskbar host operation failed");
        return false;
    }
}

bool IsTaskbarHostForWindow(
    HWND taskbarWindow,
    void* taskbarHost) {
    return taskbarHost &&
           WithTaskbarHost(
               taskbarWindow,
               [taskbarHost](void* activeTaskbarHost) {
                   return activeTaskbarHost == taskbarHost;
                });
}

bool WindowsOwnsLightDismissRegistration(void* taskbarHost) {
    if (!taskbarHost ||
        !g_windowsLightDismissTrackingReliable.load(
            std::memory_order_relaxed)) {
        return true;
    }

    AcquireSRWLockShared(&g_windowsLightDismissTaskbarHostsLock);
    bool registered =
        g_windowsLightDismissTaskbarHosts.count(taskbarHost) != 0;
    ReleaseSRWLockShared(&g_windowsLightDismissTaskbarHostsLock);
    return registered;
}

void SetWindowsLightDismissRegistration(
    void* taskbarHost,
    bool registered) {
    if (!taskbarHost) {
        return;
    }

    bool updateFailed = false;
    AcquireSRWLockExclusive(&g_windowsLightDismissTaskbarHostsLock);
    try {
        if (registered) {
            g_windowsLightDismissTaskbarHosts.insert(taskbarHost);
        } else {
            g_windowsLightDismissTaskbarHosts.erase(taskbarHost);
        }
    } catch (...) {
        updateFailed = true;
        g_windowsLightDismissTrackingReliable.store(
            false, std::memory_order_relaxed);
    }
    ReleaseSRWLockExclusive(&g_windowsLightDismissTaskbarHostsLock);

    if (updateFailed) {
        Wh_Log(L"Failed to track Windows light-dismiss ownership");
    }
}

void WINAPI TaskbarHostRegisterLightDismiss_Hook(void* object) {
    TaskbarHostRegisterLightDismiss_Original(object);
    SetWindowsLightDismissRegistration(object, true);
}

void WINAPI TaskbarHostUnregisterLightDismiss_Hook(void* object) {
    bool unregistersPreviewAction =
        g_previewLightDismissArmed &&
        IsTaskbarHostForWindow(
            g_previewLightDismissTaskbarWindow,
            object);

    TaskbarHostUnregisterLightDismiss_Original(object);
    SetWindowsLightDismissRegistration(object, false);

    if (unregistersPreviewAction) {
        g_previewLightDismissArmed = false;
        g_previewLightDismissTaskbarWindow = nullptr;
    }

    g_lastUnregisteredTaskbarHost = object;
}

using SetIsPointerOverFlyoutFrame_t =
    void(WINAPI*)(void* object, bool isPointerOver);
SetIsPointerOverFlyoutFrame_t SetIsPointerOverFlyoutFrame_Original;

using HoverFlyoutModelDestructor_t = void(WINAPI*)(void* object);
HoverFlyoutModelDestructor_t HoverFlyoutModelDestructor_Original;

void WINAPI SetIsPointerOverFlyoutFrame_Hook(
    void* object,
    bool isPointerOver) {
    if (g_closeOnOutsideClick.load(std::memory_order_relaxed)) {
        if (isPointerOver) {
            g_pointerOverFlyoutFrameModel = object;
        } else if (g_pointerOverFlyoutFrameModel == object) {
            g_pointerOverFlyoutFrameModel = nullptr;
        }
    }

    SetIsPointerOverFlyoutFrame_Original(object, isPointerOver);
}

void WINAPI HoverFlyoutModelDestructor_Hook(void* object) {
    if (g_pointerOverFlyoutFrameModel == object) {
        g_pointerOverFlyoutFrameModel = nullptr;
    }

    HoverFlyoutModelDestructor_Original(object);
}

void DisarmPreviewLightDismiss() {
    if (!g_previewLightDismissArmed) {
        return;
    }

    HWND taskbarWindow = g_previewLightDismissTaskbarWindow;
    g_previewLightDismissArmed = false;
    g_previewLightDismissTaskbarWindow = nullptr;

    bool taskbarHostFound = WithTaskbarHost(
        taskbarWindow,
        [](void* taskbarHost) {
            if (!WindowsOwnsLightDismissRegistration(taskbarHost)) {
                TaskbarHostUnregisterLightDismiss_Original(taskbarHost);
            }
            return true;
        });
    if (!taskbarHostFound) {
        Wh_Log(L"Failed to unregister the preview light-dismiss action");
    }
}

void ArmPreviewLightDismiss() {
    if (g_previewLightDismissArmed ||
        !g_initialized.load(std::memory_order_acquire) ||
        !g_closeOnOutsideClick.load(std::memory_order_relaxed) ||
        !g_activeTaskbarWindow ||
        !TaskbarHostRegisterLightDismiss_Original ||
        !TaskbarHostUnregisterLightDismiss_Original ||
        !SetIsPointerOverFlyoutFrame_Original ||
        !HoverFlyoutModelDestructor_Original ||
        !TaskbarControllerOnLightDismissTriggered_Original) {
        return;
    }

    // LightDismissAction observes the taskbar InputSite without taking focus.
    HWND taskbarWindow = g_activeTaskbarWindow;
    bool registered = WithTaskbarHost(
        taskbarWindow,
        [](void* taskbarHost) {
            if (WindowsOwnsLightDismissRegistration(taskbarHost)) {
                return false;
            }

            // The trampoline bypasses the Windows-registration tracking hook.
            TaskbarHostRegisterLightDismiss_Original(taskbarHost);
            return true;
        });
    if (registered) {
        g_previewLightDismissTaskbarWindow = taskbarWindow;
        g_previewLightDismissArmed = true;
    }
}

void CancelPreviewLightDismissRearm() {
    if (!g_previewLightDismissRearmTimer) {
        return;
    }

    KillTimer(nullptr, g_previewLightDismissRearmTimer);
    g_previewLightDismissRearmTimer = 0;
}

void CALLBACK PreviewLightDismissRearmTimerProc(
    HWND,
    UINT,
    UINT_PTR timerId,
    DWORD) {
    if (g_previewLightDismissRearmTimer != timerId) {
        KillTimer(nullptr, timerId);
        return;
    }

    KillTimer(nullptr, timerId);
    g_previewLightDismissRearmTimer = 0;

    if (g_activeHoverFlyoutController) {
        ArmPreviewLightDismiss();
    }
}

void QueuePreviewLightDismissRearm() {
    if (g_previewLightDismissRearmTimer) {
        return;
    }

    g_previewLightDismissRearmTimer =
        SetTimer(
            nullptr,
            0,
            USER_TIMER_MINIMUM,
            PreviewLightDismissRearmTimerProc);
    if (!g_previewLightDismissRearmTimer) {
        Wh_Log(L"Failed to queue preview light-dismiss rearming");
    }
}

using CommitDismissFlyout_t = void(WINAPI*)(void* object);
CommitDismissFlyout_t CommitDismissFlyout_Original;

using HoverFlyoutControllerDestructor_t = void(WINAPI*)(void* object);
HoverFlyoutControllerDestructor_t HoverFlyoutControllerDestructor_Original;

void ClearActiveHoverFlyoutController(void* object) {
    if (g_activeHoverFlyoutController == object) {
        CancelPreviewLightDismissRearm();
        DisarmPreviewLightDismiss();
        g_activeHoverFlyoutController = nullptr;
        g_activeTaskbarWindow = nullptr;
        g_pointerOverFlyoutFrameModel = nullptr;
    }
}

void CommitActiveHoverFlyoutImmediately() {
    if (!CommitDismissFlyout_Original) {
        return;
    }

    void* hoverFlyoutController = g_activeHoverFlyoutController;
    if (!hoverFlyoutController) {
        return;
    }

    CancelPreviewLightDismissRearm();
    DisarmPreviewLightDismiss();
    g_activeHoverFlyoutController = nullptr;
    g_activeTaskbarWindow = nullptr;
    g_pointerOverFlyoutFrameModel = nullptr;
    CommitDismissFlyout_Original(hoverFlyoutController);
}

void DismissActiveHoverFlyoutForOutsideClick() {
    if (!g_closeOnOutsideClick.load(std::memory_order_relaxed)) {
        return;
    }

    CommitActiveHoverFlyoutImmediately();
}

void WINAPI CommitDismissFlyout_Hook(void* object) {
    ClearActiveHoverFlyoutController(object);
    CommitDismissFlyout_Original(object);
}

using UpdateFlyoutWindowPosition_t = void(WINAPI*)(void* object);
UpdateFlyoutWindowPosition_t UpdateFlyoutWindowPosition_Original;

void WINAPI UpdateFlyoutWindowPosition_Hook(void* object) {
    UpdateFlyoutWindowPosition_Original(object);

    bool trackActiveHoverFlyout =
        g_closeOnOutsideClick.load(std::memory_order_relaxed) ||
        g_closeOnStartButtonHover.load(std::memory_order_relaxed);
    if (!trackActiveHoverFlyout ||
        !g_initialized.load(std::memory_order_acquire) ||
        !CommitDismissFlyout_Original ||
        !HoverFlyoutControllerDestructor_Original) {
        return;
    }

    HWND taskbarWindow = FindTaskbarWindowForCursor();
    bool controllerChanged =
        g_activeHoverFlyoutController != object;
    bool taskbarChanged =
        taskbarWindow &&
        g_activeTaskbarWindow != taskbarWindow;
    if (controllerChanged || taskbarChanged) {
        CancelPreviewLightDismissRearm();
        DisarmPreviewLightDismiss();
        if (controllerChanged) {
            g_activeTaskbarWindow = nullptr;
            g_pointerOverFlyoutFrameModel = nullptr;
        }
    }

    g_activeHoverFlyoutController = object;
    if (taskbarWindow) {
        g_activeTaskbarWindow = taskbarWindow;
    }
    ArmPreviewLightDismiss();
}

using HideAllHoverFlyouts_t = void(WINAPI*)(void* object);
HideAllHoverFlyouts_t HideAllHoverFlyouts_Original;

void WINAPI HideAllHoverFlyouts_Hook(void* object) {
    ClearActiveHoverFlyoutController(object);
    HideAllHoverFlyouts_Original(object);
}

void WINAPI HoverFlyoutControllerDestructor_Hook(void* object) {
    ClearActiveHoverFlyoutController(object);
    HoverFlyoutControllerDestructor_Original(object);
}

void WINAPI TaskbarControllerOnLightDismissTriggered_Hook(
    void* object,
    const void* taskbarModel,
    const void* eventArguments) {
    void* taskbarHost = g_lastUnregisteredTaskbarHost;
    g_lastUnregisteredTaskbarHost = nullptr;
    if (!g_closeOnOutsideClick.load(std::memory_order_relaxed)) {
        TaskbarControllerOnLightDismissTriggered_Original(
            object, taskbarModel, eventArguments);
        return;
    }

    bool eventTargetsActiveTaskbar =
        IsTaskbarHostForWindow(
            g_activeTaskbarWindow,
            taskbarHost);
    if (!eventTargetsActiveTaskbar) {
        TaskbarControllerOnLightDismissTriggered_Original(
            object, taskbarModel, eventArguments);
        return;
    }

    bool pointerOverFlyoutFrame =
        g_pointerOverFlyoutFrameModel != nullptr;
    if (pointerOverFlyoutFrame) {
        QueuePreviewLightDismissRearm();
    } else {
        CancelPreviewLightDismissRearm();
        DismissActiveHoverFlyoutForOutsideClick();
    }

    TaskbarControllerOnLightDismissTriggered_Original(
        object, taskbarModel, eventArguments);
}

bool IsStartButton(void* object) {
    if (!object) {
        return false;
    }

    try {
        IUnknown* innerObject = reinterpret_cast<IUnknown**>(object)[1];
        if (!innerObject) {
            return false;
        }

        winrt::Windows::UI::Xaml::FrameworkElement element = nullptr;
        HRESULT result = innerObject->QueryInterface(
            winrt::guid_of<winrt::Windows::UI::Xaml::FrameworkElement>(),
            winrt::put_abi(element));
        if (FAILED(result) || !element) {
            return false;
        }

        return winrt::Windows::UI::Xaml::Automation::AutomationProperties::
                   GetAutomationId(element) == L"StartButton";
    } catch (...) {
        return false;
    }
}

using ExperienceToggleButtonOnPointerEntered_t =
    void(WINAPI*)(void* object, const void* eventArguments);
ExperienceToggleButtonOnPointerEntered_t
    ExperienceToggleButtonOnPointerEntered_Original;

void WINAPI ExperienceToggleButtonOnPointerEntered_Hook(
    void* object,
    const void* eventArguments) {
    void* activeHoverFlyoutController = g_activeHoverFlyoutController;
    bool shouldDismiss =
        activeHoverFlyoutController &&
        g_closeOnStartButtonHover.load(std::memory_order_relaxed) &&
        IsStartButton(object);

    ExperienceToggleButtonOnPointerEntered_Original(object, eventArguments);

    if (shouldDismiss &&
        g_activeHoverFlyoutController == activeHoverFlyoutController) {
        CommitActiveHoverFlyoutImmediately();
    }
}

// This is std::chrono::duration<__int64, std::ratio<1, 10000000>> by value.
using TransitionToFlyoutVisiblePendingState_t =
    void(WINAPI*)(void* object, void* targetItemKey, LONGLONG delayTimeSpan);
TransitionToFlyoutVisiblePendingState_t
    TransitionToFlyoutVisiblePendingState_Original;

void WINAPI TransitionToFlyoutVisiblePendingState_Hook(
    void* object,
    void* targetItemKey,
    LONGLONG delayTimeSpan) {
    LONGLONG configuredDelayTimeSpan =
        g_hoverDelayTimeSpan.load(std::memory_order_relaxed);
    LONGLONG appliedDelayTimeSpan =
        delayTimeSpan > 0 ? configuredDelayTimeSpan : delayTimeSpan;

    Wh_Log(L"Hover transition: %lld -> %lld TimeSpan ticks",
           delayTimeSpan, appliedDelayTimeSpan);

    TransitionToFlyoutVisiblePendingState_Original(
        object, targetItemKey, appliedDelayTimeSpan);
}

// Limits the MouseHoverTime override to the preview dismissal transition
class DismissTransitionScope {
   public:
    DismissTransitionScope() noexcept
        : previousState_(g_inDismissTransition) {
        g_inDismissTransition = true;
    }

    DismissTransitionScope(const DismissTransitionScope&) = delete;
    DismissTransitionScope& operator=(const DismissTransitionScope&) = delete;

    ~DismissTransitionScope() noexcept {
        g_inDismissTransition = previousState_;
    }

   private:
    bool previousState_;
};

using TransitionToFlyoutDismissPendingState_t = void(WINAPI*)(void* object);
TransitionToFlyoutDismissPendingState_t
    TransitionToFlyoutDismissPendingState_Original;

void WINAPI TransitionToFlyoutDismissPendingState_Hook(void* object) {
    DismissTransitionScope dismissTransitionScope;
    TransitionToFlyoutDismissPendingState_Original(object);
}

using MouseHoverTime_t = UINT(WINAPI*)(void* object);
MouseHoverTime_t MouseHoverTime_Original;

UINT WINAPI MouseHoverTime_Hook(void* object) {
    UINT systemDelayMilliseconds = MouseHoverTime_Original(object);
    if (!g_inDismissTransition) {
        return systemDelayMilliseconds;
    }

    UINT configuredDelayMilliseconds =
        g_closeDelayMilliseconds.load(std::memory_order_relaxed);

    Wh_Log(L"Close transition: %u -> %u ms",
           systemDelayMilliseconds, configuredDelayMilliseconds);

    return configuredDelayMilliseconds;
}

void LoadSettings() {
    int delayMilliseconds = Wh_GetIntSetting(L"delayMs");
    if (delayMilliseconds < MINIMUM_DELAY_MS) {
        delayMilliseconds = MINIMUM_DELAY_MS;
    }

    int closeDelayMilliseconds = Wh_GetIntSetting(L"closeDelayMs");
    if (closeDelayMilliseconds < MINIMUM_DELAY_MS) {
        closeDelayMilliseconds = MINIMUM_DELAY_MS;
    }

    LONGLONG delayTimeSpan =
        static_cast<LONGLONG>(delayMilliseconds) *
        TIME_SPAN_TICKS_PER_MILLISECOND;
    g_hoverDelayTimeSpan.store(delayTimeSpan, std::memory_order_relaxed);
    g_closeDelayMilliseconds.store(
        static_cast<UINT>(closeDelayMilliseconds),
        std::memory_order_relaxed);
    g_closeOnOutsideClick.store(
        Wh_GetIntSetting(L"closeOnOutsideClick") != 0,
        std::memory_order_relaxed);
    g_closeOnStartButtonHover.store(
        Wh_GetIntSetting(L"closeOnStartButtonHover") != 0,
        std::memory_order_relaxed);

    Wh_Log(L"Settings: delayMs=%d closeDelayMs=%d "
           L"closeOnOutsideClick=%d closeOnStartButtonHover=%d",
           delayMilliseconds,
           closeDelayMilliseconds,
           g_closeOnOutsideClick.load(std::memory_order_relaxed),
           g_closeOnStartButtonHover.load(std::memory_order_relaxed));
}

HMODULE GetTaskbarModuleHandle() {
    return GetModuleHandleW(L"Taskbar.dll");
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }

    return module;
}

bool HookTaskbarSymbols(HMODULE module) {
    if (!module) {
        return false;
    }

    bool expected = false;
    if (!g_taskbarHooksQueued.compare_exchange_strong(
            expected, true, std::memory_order_relaxed)) {
        return true;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {
                LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})",
            },
            &CTaskBand_ITaskListWndSite_Vftable,
            nullptr,
            true,
        },
        {
            {
                LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})",
            },
            &CSecondaryTaskBand_ITaskListWndSite_Vftable,
            nullptr,
            true,
        },
        {
            {
                LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )",
            },
            &CTaskBandGetTaskbarHost_Original,
            nullptr,
            true,
        },
        {
            {
                LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )",
            },
            &CSecondaryTaskBandGetTaskbarHost_Original,
            nullptr,
            true,
        },
        {
            {
                LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))",
            },
            &ReferenceCountBaseDecrement_Original,
            nullptr,
            true,
        },
        {
            {
                LR"(private: void __cdecl TaskbarHost::RegisterLightDismiss(void))",
            },
            &TaskbarHostRegisterLightDismiss_Original,
            TaskbarHostRegisterLightDismiss_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl TaskbarHost::UnregisterLightDismiss(void))",
            },
            &TaskbarHostUnregisterLightDismiss_Original,
            TaskbarHostUnregisterLightDismiss_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                    ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"Failed to resolve Taskbar.dll symbols");
        return false;
    }

    if (!TaskbarHostRegisterLightDismiss_Original ||
        !TaskbarHostUnregisterLightDismiss_Original ||
        !CTaskBand_ITaskListWndSite_Vftable ||
        !CSecondaryTaskBand_ITaskListWndSite_Vftable ||
        !CTaskBandGetTaskbarHost_Original ||
        !CSecondaryTaskBandGetTaskbarHost_Original ||
        !ReferenceCountBaseDecrement_Original) {
        Wh_Log(L"Taskbar light-dismiss support isn't available on this build");
    }

    return true;
}

bool HookTaskbarViewSymbols(HMODULE module) {
    if (!module) {
        return false;
    }

    bool expected = false;
    if (!g_taskbarViewHooksQueued.compare_exchange_strong(
            expected, true, std::memory_order_relaxed)) {
        return true;
    }

    bool closeOnOutsideClick =
        g_closeOnOutsideClick.load(std::memory_order_relaxed);
    bool closeOnStartButtonHover =
        g_closeOnStartButtonHover.load(std::memory_order_relaxed);

    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::TransitionToFlyoutVisiblePendingState(struct winrt::hstring,class std::chrono::duration<__int64,struct std::ratio<1,10000000> >))",
                LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::TransitionToFlyoutVisiblePendingState(struct winrt::hstring,class std::chrono::duration<long long,struct std::ratio<1,10000000> >))",
            },
            &TransitionToFlyoutVisiblePendingState_Original,
            TransitionToFlyoutVisiblePendingState_Hook,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::TransitionToFlyoutDismissPendingState(void))",
            },
            &TransitionToFlyoutDismissPendingState_Original,
            TransitionToFlyoutDismissPendingState_Hook,
            true,
        },
        {
            {
                LR"(public: __cdecl winrt::impl::consume_Windows_UI_ViewManagement_IUISettings<struct winrt::Windows::UI::ViewManagement::IUISettings>::MouseHoverTime(void)const )",
                LR"(public: unsigned int __cdecl winrt::impl::consume_Windows_UI_ViewManagement_IUISettings<struct winrt::Windows::UI::ViewManagement::IUISettings>::MouseHoverTime(void)const )",
            },
            &MouseHoverTime_Original,
            MouseHoverTime_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutController::CommitDismissFlyout(void))",
            },
            &CommitDismissFlyout_Original,
            CommitDismissFlyout_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutController::UpdateFlyoutWindowPosition(void))",
            },
            &UpdateFlyoutWindowPosition_Original,
            UpdateFlyoutWindowPosition_Hook,
            true,
        },
        {
            {
                LR"(public: virtual __cdecl winrt::Taskbar::implementation::HoverFlyoutController::~HoverFlyoutController(void))",
            },
            &HoverFlyoutControllerDestructor_Original,
            HoverFlyoutControllerDestructor_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutController::HideAllHoverFlyouts(void))",
            },
            &HideAllHoverFlyouts_Original,
            HideAllHoverFlyouts_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::SetIsPointerOverFlyoutFrame(bool))",
            },
            &SetIsPointerOverFlyoutFrame_Original,
            SetIsPointerOverFlyoutFrame_Hook,
            true,
        },
        {
            {
                LR"(public: virtual __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::~HoverFlyoutModel(void))",
            },
            &HoverFlyoutModelDestructor_Original,
            HoverFlyoutModelDestructor_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::OnLightDismissTriggered(struct winrt::WindowsUdk::UI::Shell::TaskbarModel const &,struct winrt::Windows::Foundation::IInspectable const &))",
            },
            &TaskbarControllerOnLightDismissTriggered_Original,
            TaskbarControllerOnLightDismissTriggered_Hook,
            true,
        },
        {
            {
                LR"(public: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::OnPointerEntered(struct winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const &))",
            },
            &ExperienceToggleButtonOnPointerEntered_Original,
            ExperienceToggleButtonOnPointerEntered_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(
            module,
            taskbarViewHooks,
            ARRAYSIZE(taskbarViewHooks))) {
        Wh_Log(L"Failed to resolve Taskbar.View symbols");
        return false;
    }

    if (!TransitionToFlyoutDismissPendingState_Original ||
        !MouseHoverTime_Original) {
        Wh_Log(L"Thumbnail close-delay symbols aren't available; instant "
               L"close is disabled on this build");
    }

    if (closeOnOutsideClick &&
        (!TaskbarControllerOnLightDismissTriggered_Original ||
         !UpdateFlyoutWindowPosition_Original ||
         !SetIsPointerOverFlyoutFrame_Original ||
         !HoverFlyoutModelDestructor_Original ||
         !HoverFlyoutControllerDestructor_Original ||
         !CommitDismissFlyout_Original)) {
        Wh_Log(L"Thumbnail light-dismiss symbols aren't available; "
               L"close-on-outside-click is disabled on this build");
    }

    if (closeOnStartButtonHover &&
        (!ExperienceToggleButtonOnPointerEntered_Original ||
         !UpdateFlyoutWindowPosition_Original ||
         !CommitDismissFlyout_Original ||
         !HoverFlyoutControllerDestructor_Original)) {
        Wh_Log(L"Start-button hover symbols aren't available; "
               L"close-on-Start-hover is disabled on this build");
    }

    WCHAR modulePath[MAX_PATH] = {};
    if (!GetModuleFileNameW(module, modulePath, ARRAYSIZE(modulePath))) {
        wcscpy_s(modulePath, L"(unknown module)");
    }

    Wh_Log(L"Taskbar.View hooks queued for %s", modulePath);
    return true;
}

void HandleLoadedModule(HMODULE module) {
    if (!module) {
        return;
    }

    bool hooksQueued = false;
    if (!g_taskbarHooksQueued.load(std::memory_order_relaxed) &&
        GetTaskbarModuleHandle() == module) {
        hooksQueued = HookTaskbarSymbols(module);
    }

    if (!g_taskbarViewHooksQueued.load(std::memory_order_relaxed) &&
        GetTaskbarViewModuleHandle() == module) {
        hooksQueued = HookTaskbarViewSymbols(module) || hooksQueued;
    }

    if (hooksQueued &&
        g_initialized.load(std::memory_order_relaxed) &&
        !Wh_ApplyHookOperations()) {
        Wh_Log(L"Failed to apply late taskbar hooks");
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName,
    HANDLE file,
    DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    HandleLoadedModule(module);
    return module;
}

bool HookLoadLibraryExW() {
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelBase) {
        Wh_Log(L"kernelbase.dll isn't loaded");
        return false;
    }

    LoadLibraryExW_t loadLibraryExW =
        reinterpret_cast<LoadLibraryExW_t>(
            GetProcAddress(kernelBase, "LoadLibraryExW"));
    if (!loadLibraryExW) {
        Wh_Log(L"LoadLibraryExW wasn't found");
        return false;
    }

    if (!WindhawkUtils::SetFunctionHook(
            loadLibraryExW,
            LoadLibraryExW_Hook,
            &LoadLibraryExW_Original)) {
        Wh_Log(L"Failed to hook LoadLibraryExW");
        return false;
    }

    return true;
}

using RunFromWindowThreadProcedure = void(WINAPI*)(void* parameter);

struct RunFromWindowThreadParameters {
    RunFromWindowThreadProcedure procedure;
    void* parameter;
    bool executed;
};

UINT GetRunFromWindowThreadMessage() {
    static const UINT message =
        RegisterWindowMessageW(
            L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    return message;
}

LRESULT CALLBACK RunFromWindowThreadHook(
    int code,
    WPARAM wParam,
    LPARAM lParam) {
    if (code == HC_ACTION) {
        const CWPSTRUCT* windowMessage =
            reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (windowMessage->message ==
                GetRunFromWindowThreadMessage() &&
            windowMessage->lParam) {
            RunFromWindowThreadParameters* parameters =
                reinterpret_cast<RunFromWindowThreadParameters*>(
                    windowMessage->lParam);
            if (!parameters->executed) {
                parameters->executed = true;
                parameters->procedure(parameters->parameter);
            }
        }
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

bool RunFromWindowThread(
    HWND window,
    RunFromWindowThreadProcedure procedure,
    void* parameter) {
    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) {
        return false;
    }

    UINT message = GetRunFromWindowThreadMessage();
    if (!message) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        procedure(parameter);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        RunFromWindowThreadHook,
        nullptr,
        threadId);
    if (!hook) {
        return false;
    }

    RunFromWindowThreadParameters parameters = {
        procedure,
        parameter,
        false,
    };
    SendMessageW(
        window,
        message,
        0,
        reinterpret_cast<LPARAM>(&parameters));
    UnhookWindowsHookEx(hook);
    return parameters.executed;
}

void WINAPI DisableOutsideClickTaskbarThreadState(void*) {
    CancelPreviewLightDismissRearm();
    DisarmPreviewLightDismiss();
    g_pointerOverFlyoutFrameModel = nullptr;
    g_lastUnregisteredTaskbarHost = nullptr;
}

void WINAPI CleanupTaskbarThreadState(void*) {
    DisableOutsideClickTaskbarThreadState(nullptr);
    g_activeHoverFlyoutController = nullptr;
    g_activeTaskbarWindow = nullptr;
}

struct RUN_TASKBAR_THREAD_OPERATION_CONTEXT {
    RunFromWindowThreadProcedure procedure;
};

BOOL CALLBACK RunTaskbarThreadOperationCallback(
    HWND window,
    LPARAM parameter) {
    RUN_TASKBAR_THREAD_OPERATION_CONTEXT* context =
        reinterpret_cast<RUN_TASKBAR_THREAD_OPERATION_CONTEXT*>(
            parameter);
    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    if (processId == GetCurrentProcessId() &&
        GetTaskbarWindowType(window) !=
            TaskbarWindowType::invalid &&
        !RunFromWindowThread(
            window,
            context->procedure,
            nullptr)) {
        Wh_Log(
            L"Failed to update taskbar thread %lu",
            GetWindowThreadProcessId(window, nullptr));
    }

    return TRUE;
}

void RunOnTaskbarThreads(RunFromWindowThreadProcedure procedure) {
    RUN_TASKBAR_THREAD_OPERATION_CONTEXT context = {
        procedure,
    };
    EnumWindows(
        RunTaskbarThreadOperationCallback,
        reinterpret_cast<LPARAM>(&context));
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing");
    LoadSettings();

    if (!HookLoadLibraryExW()) {
        return FALSE;
    }

    HMODULE taskbarModule = GetTaskbarModuleHandle();
    if (taskbarModule && !HookTaskbarSymbols(taskbarModule)) {
        Wh_Log(L"Taskbar.dll light-dismiss hooks weren't installed");
    }

    HMODULE taskbarViewModule = GetTaskbarViewModuleHandle();
    if (taskbarViewModule && !HookTaskbarViewSymbols(taskbarViewModule)) {
        return FALSE;
    }

    g_initialized.store(true, std::memory_order_release);
    Wh_Log(L"Initialized");
    return TRUE;
}

void Wh_ModAfterInit() {
    bool hooksQueued = false;

    if (!g_taskbarHooksQueued.load(std::memory_order_relaxed)) {
        HMODULE taskbarModule = GetTaskbarModuleHandle();
        if (taskbarModule) {
            hooksQueued = HookTaskbarSymbols(taskbarModule);
        }
    }

    if (!g_taskbarViewHooksQueued.load(std::memory_order_relaxed)) {
        HMODULE taskbarViewModule = GetTaskbarViewModuleHandle();
        if (taskbarViewModule) {
            hooksQueued =
                HookTaskbarViewSymbols(taskbarViewModule) || hooksQueued;
        } else {
            Wh_Log(L"Taskbar.View isn't loaded yet");
        }
    }

    if (hooksQueued && !Wh_ApplyHookOperations()) {
        Wh_Log(L"Failed to apply taskbar hooks in Wh_ModAfterInit");
    }
}

void Wh_ModSettingsChanged() {
    bool previousCloseOnOutsideClick =
        g_closeOnOutsideClick.load(std::memory_order_relaxed);
    LoadSettings();

    if (previousCloseOnOutsideClick &&
        !g_closeOnOutsideClick.load(std::memory_order_relaxed)) {
        RunOnTaskbarThreads(
            DisableOutsideClickTaskbarThreadState);
    }
}

void Wh_ModBeforeUninit() {
    g_initialized.store(false, std::memory_order_release);
    g_closeOnOutsideClick.store(false, std::memory_order_relaxed);
    g_closeOnStartButtonHover.store(false, std::memory_order_relaxed);
    RunOnTaskbarThreads(CleanupTaskbarThreadState);
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitialized");
}
