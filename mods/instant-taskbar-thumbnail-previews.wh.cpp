// ==WindhawkMod==
// @id              instant-taskbar-thumbnail-previews
// @name            Instant Taskbar Thumbnail Previews
// @description     Controls taskbar thumbnail preview show and close behavior.
// @version         1.1.0
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

![https://github.com/user-attachments/assets/ac176503-f162-4da6-8f4d-b6a8dac5cd79]

Adds the ability to configure or remove the delays before Windows 11 shows and closes taskbar thumbnail previews,
as well as the ability to configure mouse actions that close them.

The hover and close delay settings replace the corresponding Windows value, so they can make the transitions either shorter or longer than the stock delays.

Version 1.1 changes the default "Thumbnail close delay" from 1 to 200 milliseconds. Set it to 1 to retain effectively immediate closing when the pointer leaves the thumbnail area.

"Delay after thumbnail removal" keeps the remaining thumbnail previews open after a thumbnail is removed, whether it was closed from the preview or externally, giving the pointer time to reach another preview before the flyout closes. The normal close delay applies when the last thumbnail is removed. Windows reports removal-driven and pointer-driven dismissals through the same transition, so the mod attributes removals using a short event-correlation window.

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
- closeDelayMs: 200
  $name: Thumbnail close delay
  $description: Exact delay after the pointer leaves the thumbnail area. The minimum value is 1, which is effectively immediate.
- thumbnailRemovalDelayMs: 500
  $name: Delay after thumbnail removal
  $description: Delay used when dismissal is attributed to a recent thumbnail removal and at least one preview remains, including when its window closes outside the preview. Attribution uses a short event-correlation window. The minimum value is 1, which is effectively immediate.
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
#include <unordered_map>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.h>

namespace {

constexpr int MINIMUM_DELAY_MS = 1;
constexpr UINT32 MINIMUM_REMAINING_THUMBNAIL_COUNT = 1;
constexpr LONGLONG TIME_SPAN_TICKS_PER_MILLISECOND = 10000LL;
constexpr DWORD MINIMUM_SUPPORTED_WINDOWS_BUILD = 26100;
constexpr DWORD NONEXECUTABLE_LIBRARY_LOAD_FLAGS =
    LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
    LOAD_LIBRARY_AS_IMAGE_RESOURCE;

// Allow deferred layout and pointer events while rejecting later pointer exits
constexpr ULONGLONG THUMBNAIL_REMOVAL_CORRELATION_WINDOW_MS = 150;

std::atomic<LONGLONG> g_hoverDelayTimeSpan{
    MINIMUM_DELAY_MS * TIME_SPAN_TICKS_PER_MILLISECOND};
std::atomic<UINT> g_closeDelayMilliseconds{MINIMUM_DELAY_MS};
std::atomic<UINT> g_thumbnailRemovalDelayMilliseconds{MINIMUM_DELAY_MS};
std::atomic<bool> g_closeOnOutsideClick{false};
std::atomic<bool> g_closeOnStartButtonHover{false};
std::atomic<bool> g_initialized{false};
std::atomic<bool> g_lateHookApplicationReady{false};
std::atomic<bool> g_hookInstallationPending{true};
std::atomic<bool> g_taskbarHooksApplied{false};
std::atomic<bool> g_taskbarViewHooksApplied{false};

enum class HookInstallState {
    waitingForModule,
    queued,
    applied,
    failed,
    notApplicable,
};

SRWLOCK g_hookInstallLock = SRWLOCK_INIT;
HookInstallState g_taskbarHookInstallState =
    HookInstallState::waitingForModule;
HookInstallState g_taskbarViewHookInstallState =
    HookInstallState::waitingForModule;
thread_local bool g_hookInstallationInProgress = false;

class HookInstallationScope {
   public:
    HookInstallationScope() noexcept {
        g_hookInstallationInProgress = true;
    }

    HookInstallationScope(const HookInstallationScope&) = delete;
    HookInstallationScope& operator=(const HookInstallationScope&) = delete;

    ~HookInstallationScope() noexcept {
        g_hookInstallationInProgress = false;
    }
};

enum class LightDismissOwnership {
    unknown,
    unregistered,
    windows,
    preview,
};

using LightDismissOwnershipMap =
    std::unordered_map<void*, LightDismissOwnership>;

std::atomic<bool> g_lightDismissOwnershipTrackingReliable{true};
SRWLOCK g_lightDismissOwnershipLock = SRWLOCK_INIT;
LightDismissOwnershipMap g_lightDismissOwnershipByTaskbarHost;

enum class DismissDelaySource {
    none,
    pointerExit,
    thumbnailRemoval,
};

struct ThumbnailRemovalTrackingState {
    void* activeFlyoutModel;
    void* activeThumbnailItemsCollection;
    void* removalFlyoutModel;
    ULONGLONG removalTick;
};

thread_local DismissDelaySource g_dismissDelaySource =
    DismissDelaySource::none;
thread_local ThumbnailRemovalTrackingState g_thumbnailRemovalTrackingState = {};
thread_local HWND g_activeTaskbarWindow = nullptr;
thread_local bool g_previewLightDismissArmed = false;
thread_local HWND g_previewLightDismissTaskbarWindow = nullptr;
thread_local void* g_previewLightDismissTaskbarHost = nullptr;
thread_local void* g_activeHoverFlyoutController = nullptr;
thread_local void* g_pointerOverFlyoutFrameModel = nullptr;
thread_local void* g_lastUnregisteredTaskbarHost = nullptr;

struct ActiveTaskbarModelState {
    void* hoverFlyoutController;
    HWND taskbarWindow;
    bool isExpanded;
    bool valid;
};

thread_local ActiveTaskbarModelState g_activeTaskbarModelState = {};

struct TaskbarModelStateCapture {
    UINT64 hostWindowID;
    bool isExpanded;
    bool captured;
};

thread_local TaskbarModelStateCapture* g_taskbarModelStateCapture = nullptr;

class TaskbarModelStateCaptureScope {
   public:
    explicit TaskbarModelStateCaptureScope(
        TaskbarModelStateCapture* capture) noexcept
        : previousCapture_(g_taskbarModelStateCapture) {
        g_taskbarModelStateCapture = capture;
    }

    TaskbarModelStateCaptureScope(
        const TaskbarModelStateCaptureScope&) = delete;
    TaskbarModelStateCaptureScope& operator=(
        const TaskbarModelStateCaptureScope&) = delete;

    ~TaskbarModelStateCaptureScope() noexcept {
        g_taskbarModelStateCapture = previousCapture_;
    }

   private:
    TaskbarModelStateCapture* previousCapture_;
};

bool CanTrackActiveHoverFlyout();
bool CanTrackFlyoutPointerState();
bool CanUseOutsideClick();
bool CanUseStartButtonHover();
bool CanUseThumbnailRemoval();
bool HasTaskbarLightDismissSymbols();
bool HasThumbnailRemovalSymbols();

void ClearThumbnailRemovalTag() {
    g_thumbnailRemovalTrackingState.removalFlyoutModel = nullptr;
    g_thumbnailRemovalTrackingState.removalTick = 0;
}

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

using TaskbarHostDestructor_t = void(WINAPI*)(void* object);
TaskbarHostDestructor_t TaskbarHostDestructor_Original;

using TaskbarModelHostWindowID_t = UINT64(WINAPI*)(const void* object);
TaskbarModelHostWindowID_t TaskbarModelHostWindowID_Original;

using TaskbarModelIsExpanded_t = bool(WINAPI*)(const void* object);
TaskbarModelIsExpanded_t TaskbarModelIsExpanded_Original;

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

void SetLightDismissOwnership(
    void* taskbarHost,
    LightDismissOwnership ownership);

LightDismissOwnership GetLightDismissOwnership(void* taskbarHost) {
    if (!taskbarHost ||
        !g_lightDismissOwnershipTrackingReliable.load(
            std::memory_order_acquire)) {
        return LightDismissOwnership::unknown;
    }

    AcquireSRWLockShared(&g_lightDismissOwnershipLock);
    if (!g_lightDismissOwnershipTrackingReliable.load(
            std::memory_order_acquire)) {
        ReleaseSRWLockShared(&g_lightDismissOwnershipLock);
        return LightDismissOwnership::unknown;
    }

    LightDismissOwnershipMap::const_iterator iterator =
        g_lightDismissOwnershipByTaskbarHost.find(taskbarHost);
    LightDismissOwnership ownership =
        iterator == g_lightDismissOwnershipByTaskbarHost.end()
            ? LightDismissOwnership::unknown
            : iterator->second;
    ReleaseSRWLockShared(&g_lightDismissOwnershipLock);
    return ownership;
}

void SeedLightDismissOwnership(
    void* taskbarHost,
    bool taskbarModelIsExpanded) {
    if (!taskbarHost ||
        GetLightDismissOwnership(taskbarHost) !=
            LightDismissOwnership::unknown) {
        return;
    }

    // TaskbarHost::OnIsExpandedChanged uses this property to choose between
    // native registration and unregistration.
    SetLightDismissOwnership(
        taskbarHost,
        taskbarModelIsExpanded ? LightDismissOwnership::windows
                               : LightDismissOwnership::unregistered);
}

void SetLightDismissOwnership(
    void* taskbarHost,
    LightDismissOwnership ownership) {
    if (!taskbarHost ||
        !g_lightDismissOwnershipTrackingReliable.load(
            std::memory_order_acquire)) {
        return;
    }

    bool updateFailed = false;
    AcquireSRWLockExclusive(&g_lightDismissOwnershipLock);
    if (!g_lightDismissOwnershipTrackingReliable.load(
            std::memory_order_acquire)) {
        ReleaseSRWLockExclusive(&g_lightDismissOwnershipLock);
        return;
    }

    try {
        LightDismissOwnershipMap::iterator iterator =
            g_lightDismissOwnershipByTaskbarHost.find(taskbarHost);
        if (iterator == g_lightDismissOwnershipByTaskbarHost.end()) {
            g_lightDismissOwnershipByTaskbarHost.emplace(
                taskbarHost, ownership);
        } else {
            iterator->second = ownership;
        }
    } catch (...) {
        updateFailed = true;
        g_lightDismissOwnershipTrackingReliable.store(
            false, std::memory_order_release);
    }
    ReleaseSRWLockExclusive(&g_lightDismissOwnershipLock);

    if (updateFailed) {
        Wh_Log(L"Failed to track light-dismiss ownership; preview "
               L"registration is disabled");
    }
}

bool TryClaimUnregisteredLightDismiss(void* taskbarHost) {
    if (!taskbarHost ||
        !g_lightDismissOwnershipTrackingReliable.load(
            std::memory_order_acquire)) {
        return false;
    }

    bool claimed = false;
    AcquireSRWLockExclusive(&g_lightDismissOwnershipLock);
    if (!g_lightDismissOwnershipTrackingReliable.load(
            std::memory_order_acquire)) {
        ReleaseSRWLockExclusive(&g_lightDismissOwnershipLock);
        return false;
    }

    LightDismissOwnershipMap::iterator iterator =
        g_lightDismissOwnershipByTaskbarHost.find(taskbarHost);
    if (iterator != g_lightDismissOwnershipByTaskbarHost.end() &&
        iterator->second == LightDismissOwnership::unregistered) {
        iterator->second = LightDismissOwnership::preview;
        claimed = true;
    }
    ReleaseSRWLockExclusive(&g_lightDismissOwnershipLock);
    return claimed;
}

void ForgetLightDismissOwnership(void* taskbarHost) {
    AcquireSRWLockExclusive(&g_lightDismissOwnershipLock);
    g_lightDismissOwnershipByTaskbarHost.erase(taskbarHost);
    ReleaseSRWLockExclusive(&g_lightDismissOwnershipLock);
}

void ClearPreviewLightDismissStateForHost(void* taskbarHost) {
    if (g_previewLightDismissTaskbarHost == taskbarHost) {
        g_previewLightDismissArmed = false;
        g_previewLightDismissTaskbarWindow = nullptr;
        g_previewLightDismissTaskbarHost = nullptr;
    }

    if (g_lastUnregisteredTaskbarHost == taskbarHost) {
        g_lastUnregisteredTaskbarHost = nullptr;
    }
}

void WINAPI TaskbarHostRegisterLightDismiss_Hook(void* object) {
    TaskbarHostRegisterLightDismiss_Original(object);

    if (!g_taskbarHooksApplied.load(std::memory_order_acquire) ||
        !HasTaskbarLightDismissSymbols()) {
        return;
    }

    SetLightDismissOwnership(object, LightDismissOwnership::windows);
    ClearPreviewLightDismissStateForHost(object);
}

void WINAPI TaskbarHostUnregisterLightDismiss_Hook(void* object) {
    TaskbarHostUnregisterLightDismiss_Original(object);

    if (!g_taskbarHooksApplied.load(std::memory_order_acquire) ||
        !HasTaskbarLightDismissSymbols()) {
        return;
    }

    SetLightDismissOwnership(object, LightDismissOwnership::unregistered);
    ClearPreviewLightDismissStateForHost(object);
    g_lastUnregisteredTaskbarHost = object;
}

void WINAPI TaskbarHostDestructor_Hook(void* object) {
    TaskbarHostDestructor_Original(object);
    ClearPreviewLightDismissStateForHost(object);
    ForgetLightDismissOwnership(object);
}

using SetIsPointerOverFlyoutFrame_t =
    void(WINAPI*)(void* object, bool isPointerOver);
SetIsPointerOverFlyoutFrame_t SetIsPointerOverFlyoutFrame_Original;

using HoverFlyoutModelDestructor_t = void(WINAPI*)(void* object);
HoverFlyoutModelDestructor_t HoverFlyoutModelDestructor_Original;

void WINAPI SetIsPointerOverFlyoutFrame_Hook(
    void* object,
    bool isPointerOver) {
    if (CanTrackFlyoutPointerState()) {
        if (isPointerOver) {
            g_pointerOverFlyoutFrameModel = object;
        } else if (g_pointerOverFlyoutFrameModel == object) {
            g_pointerOverFlyoutFrameModel = nullptr;
        }
    }

    SetIsPointerOverFlyoutFrame_Original(object, isPointerOver);
}

void WINAPI HoverFlyoutModelDestructor_Hook(void* object) {
    if (g_thumbnailRemovalTrackingState.removalFlyoutModel == object) {
        ClearThumbnailRemovalTag();
    }

    if (g_thumbnailRemovalTrackingState.activeFlyoutModel == object) {
        g_thumbnailRemovalTrackingState.activeFlyoutModel = nullptr;
        g_thumbnailRemovalTrackingState.activeThumbnailItemsCollection =
            nullptr;
    }

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
    void* previewTaskbarHost = g_previewLightDismissTaskbarHost;
    g_previewLightDismissArmed = false;
    g_previewLightDismissTaskbarWindow = nullptr;
    g_previewLightDismissTaskbarHost = nullptr;

    bool taskbarHostFound = WithTaskbarHost(
        taskbarWindow,
        [previewTaskbarHost](void* taskbarHost) {
            if (taskbarHost != previewTaskbarHost) {
                return false;
            }

            LightDismissOwnership ownership =
                GetLightDismissOwnership(taskbarHost);
            bool ownershipTrackingReliable =
                g_lightDismissOwnershipTrackingReliable.load(
                    std::memory_order_acquire);
            // Native registration hooks clear the thread-local owner first, so
            // it remains authoritative if only the shared map became unusable.
            if (ownership == LightDismissOwnership::preview ||
                (!ownershipTrackingReliable && previewTaskbarHost)) {
                TaskbarHostUnregisterLightDismiss_Original(taskbarHost);
                SetLightDismissOwnership(
                    taskbarHost,
                    LightDismissOwnership::unregistered);
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
        !CanUseOutsideClick()) {
        return;
    }

    // LightDismissAction observes the taskbar InputSite without taking focus.
    HWND taskbarWindow = g_activeTaskbarWindow;
    bool taskbarModelStateAvailable =
        g_activeTaskbarModelState.valid &&
        g_activeTaskbarModelState.hoverFlyoutController ==
            g_activeHoverFlyoutController &&
        g_activeTaskbarModelState.taskbarWindow == taskbarWindow;
    bool taskbarModelIsExpanded =
        g_activeTaskbarModelState.isExpanded;
    void* registeredTaskbarHost = nullptr;
    bool registered = WithTaskbarHost(
        taskbarWindow,
        [taskbarModelStateAvailable,
         taskbarModelIsExpanded,
         &registeredTaskbarHost](void* taskbarHost) {
            if (taskbarModelStateAvailable) {
                SeedLightDismissOwnership(
                    taskbarHost, taskbarModelIsExpanded);
            }

            // Without a semantic snapshot or observed native transition, an
            // unseen host may already contain a Windows action.
            if (!TryClaimUnregisteredLightDismiss(taskbarHost)) {
                return false;
            }

            try {
                // The trampoline bypasses the ownership-tracking hook.
                TaskbarHostRegisterLightDismiss_Original(taskbarHost);
            } catch (...) {
                SetLightDismissOwnership(
                    taskbarHost,
                    LightDismissOwnership::unregistered);
                throw;
            }

            SetLightDismissOwnership(
                taskbarHost,
                LightDismissOwnership::preview);
            if (g_lastUnregisteredTaskbarHost == taskbarHost) {
                g_lastUnregisteredTaskbarHost = nullptr;
            }
            registeredTaskbarHost = taskbarHost;
            return true;
        });
    if (registered) {
        g_previewLightDismissTaskbarWindow = taskbarWindow;
        g_previewLightDismissTaskbarHost = registeredTaskbarHost;
        g_previewLightDismissArmed = true;
    }
}

using CommitDismissFlyout_t = void(WINAPI*)(void* object);
CommitDismissFlyout_t CommitDismissFlyout_Original;

using HoverFlyoutControllerDestructor_t = void(WINAPI*)(void* object);
HoverFlyoutControllerDestructor_t HoverFlyoutControllerDestructor_Original;

void ClearActiveHoverFlyoutController(void* object) {
    if (g_activeHoverFlyoutController == object) {
        DisarmPreviewLightDismiss();
        g_thumbnailRemovalTrackingState = {};
        g_activeHoverFlyoutController = nullptr;
        g_activeTaskbarWindow = nullptr;
        g_activeTaskbarModelState = {};
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

    ClearActiveHoverFlyoutController(hoverFlyoutController);
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

UINT64 WINAPI TaskbarModelHostWindowID_Hook(const void* object) {
    UINT64 hostWindowID = TaskbarModelHostWindowID_Original(object);

    // UpdateFlyoutWindowPosition asks its own TaskbarModel for this value. Use
    // that semantic call to capture the matching per-taskbar expansion state.
    TaskbarModelStateCapture* capture = g_taskbarModelStateCapture;
    if (!capture || capture->captured || !hostWindowID ||
        !TaskbarModelIsExpanded_Original) {
        return hostWindowID;
    }

    // Claim the slot before calling back into taskbar model code.
    capture->captured = true;
    try {
        bool isExpanded = TaskbarModelIsExpanded_Original(object);
        capture->hostWindowID = hostWindowID;
        capture->isExpanded = isExpanded;
    } catch (...) {
        // A failed semantic query leaves ownership unknown and fails closed.
        capture->captured = false;
    }

    return hostWindowID;
}

void WINAPI UpdateFlyoutWindowPosition_Hook(void* object) {
    TaskbarModelStateCapture taskbarModelStateCapture = {};
    {
        TaskbarModelStateCaptureScope captureScope(
            g_initialized.load(std::memory_order_acquire)
                ? &taskbarModelStateCapture
                : nullptr);
        UpdateFlyoutWindowPosition_Original(object);
    }

    if (!g_initialized.load(std::memory_order_acquire) ||
        !CanTrackActiveHoverFlyout()) {
        return;
    }

    HWND taskbarWindow = nullptr;
    if (taskbarModelStateCapture.captured) {
        taskbarWindow = NormalizeTaskbarWindow(
            reinterpret_cast<HWND>(
                taskbarModelStateCapture.hostWindowID));
    }

    bool controllerChanged =
        g_activeHoverFlyoutController != object;
    bool taskbarChanged =
        taskbarWindow &&
        g_activeTaskbarWindow != taskbarWindow;
    bool taskbarModelStateInvalidated =
        taskbarModelStateCapture.captured && !taskbarWindow;
    if (controllerChanged || taskbarChanged ||
        taskbarModelStateInvalidated) {
        DisarmPreviewLightDismiss();
        if (controllerChanged) {
            // The visible-pending model transition precedes the controller's
            // first position update, so its removal state is already current.
            g_pointerOverFlyoutFrameModel = nullptr;
        }

        if (controllerChanged || taskbarModelStateInvalidated) {
            g_activeTaskbarWindow = nullptr;
            g_activeTaskbarModelState = {};
        }
    }

    g_activeHoverFlyoutController = object;
    if (taskbarWindow) {
        g_activeTaskbarWindow = taskbarWindow;
        g_activeTaskbarModelState = {
            object,
            taskbarWindow,
            taskbarModelStateCapture.isExpanded,
            true,
        };
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
    if (!g_closeOnOutsideClick.load(std::memory_order_relaxed) ||
        !CanUseOutsideClick()) {
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
    void* activeHoverFlyoutController =
        g_activeHoverFlyoutController;
    if (!pointerOverFlyoutFrame) {
        DismissActiveHoverFlyoutForOutsideClick();
    }

    TaskbarControllerOnLightDismissTriggered_Original(
        object, taskbarModel, eventArguments);

    // The native action unregisters before raising this event. Rearming after
    // the native handler returns avoids reentrancy without deferred callbacks.
    if (pointerOverFlyoutFrame && activeHoverFlyoutController &&
        g_activeHoverFlyoutController == activeHoverFlyoutController) {
        ArmPreviewLightDismiss();
    }
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
        CanUseStartButtonHover() &&
        IsStartButton(object);

    ExperienceToggleButtonOnPointerEntered_Original(object, eventArguments);

    if (shouldDismiss &&
        g_activeHoverFlyoutController == activeHoverFlyoutController) {
        CommitActiveHoverFlyoutImmediately();
    }
}

using HoverUIItemsCollectionSetTargetItem_t =
    void(WINAPI*)(void* object, const void* targetItem);
HoverUIItemsCollectionSetTargetItem_t
    HoverUIItemsCollectionSetTargetItem_Original;

using HoverUIItemsCollectionDestructor_t = void(WINAPI*)(void* object);
HoverUIItemsCollectionDestructor_t HoverUIItemsCollectionDestructor_Original;

using ThumbnailSourceArraySize_t = UINT32(WINAPI*)(const void* object);
ThumbnailSourceArraySize_t ThumbnailSourceArraySize_Original;

void TrackActiveThumbnailItemsCollection(void* object) {
    if (!CanUseThumbnailRemoval() ||
        !g_thumbnailRemovalTrackingState.activeFlyoutModel) {
        return;
    }

    // SetTargetItem is the closest semantic ownership signal available. A
    // visible-target transition clears the previous collection first.
    g_thumbnailRemovalTrackingState.activeThumbnailItemsCollection = object;
}

void WINAPI HoverUIItemsCollectionSetTargetItem_Hook(
    void* object,
    const void* targetItem) {
    TrackActiveThumbnailItemsCollection(object);
    HoverUIItemsCollectionSetTargetItem_Original(object, targetItem);
}

void WINAPI HoverUIItemsCollectionDestructor_Hook(void* object) {
    if (g_thumbnailRemovalTrackingState.activeThumbnailItemsCollection ==
        object) {
        if (g_thumbnailRemovalTrackingState.removalFlyoutModel ==
            g_thumbnailRemovalTrackingState.activeFlyoutModel) {
            ClearThumbnailRemovalTag();
        }

        g_thumbnailRemovalTrackingState.activeThumbnailItemsCollection =
            nullptr;
    }

    HoverUIItemsCollectionDestructor_Original(object);
}

bool ThumbnailRemovalLeavesPreviewItems(const void* sourceArray) noexcept {
    if (!sourceArray || !ThumbnailSourceArraySize_Original) {
        return false;
    }

    try {
        // The source vector has already removed the item before raising this
        // callback, so its semantic Size getter reports the remaining count.
        return ThumbnailSourceArraySize_Original(sourceArray) >=
               MINIMUM_REMAINING_THUMBNAIL_COUNT;
    } catch (...) {
        return false;
    }
}

using HoverUIItemsCollectionOnSourceArrayChanged_t =
    void(WINAPI*)(void* object,
                  const void* sender,
                  const winrt::Windows::Foundation::Collections::
                      IVectorChangedEventArgs& eventArguments);
HoverUIItemsCollectionOnSourceArrayChanged_t
    HoverUIItemsCollectionOnSourceArrayChanged_Original;

void WINAPI HoverUIItemsCollectionOnSourceArrayChanged_Hook(
    void* object,
    const void* sender,
    const winrt::Windows::Foundation::Collections::
        IVectorChangedEventArgs& eventArguments) {
    if (CanUseThumbnailRemoval()) {
        try {
            winrt::Windows::Foundation::Collections::CollectionChange
                collectionChange = eventArguments.CollectionChange();
            if (collectionChange ==
                    winrt::Windows::Foundation::Collections::
                        CollectionChange::ItemRemoved &&
                object ==
                    g_thumbnailRemovalTrackingState
                        .activeThumbnailItemsCollection &&
                g_thumbnailRemovalTrackingState.activeFlyoutModel) {
                ClearThumbnailRemovalTag();

                if (ThumbnailRemovalLeavesPreviewItems(sender)) {
                    g_thumbnailRemovalTrackingState.removalFlyoutModel =
                        g_thumbnailRemovalTrackingState.activeFlyoutModel;
                    g_thumbnailRemovalTrackingState.removalTick =
                        GetTickCount64();
                }
            }
        } catch (...) {
            Wh_Log(L"Failed to inspect a thumbnail collection change");
        }
    }

    HoverUIItemsCollectionOnSourceArrayChanged_Original(
        object, sender, eventArguments);
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
    if (CanUseThumbnailRemoval()) {
        // A new hover target supersedes an unconsumed removal transition
        ClearThumbnailRemovalTag();
        g_thumbnailRemovalTrackingState.activeThumbnailItemsCollection =
            nullptr;
        g_thumbnailRemovalTrackingState.activeFlyoutModel = object;
    }

    LONGLONG configuredDelayTimeSpan =
        g_hoverDelayTimeSpan.load(std::memory_order_relaxed);
    LONGLONG appliedDelayTimeSpan =
        delayTimeSpan > 0 ? configuredDelayTimeSpan : delayTimeSpan;

    Wh_Log(L"Hover transition: %lld -> %lld TimeSpan ticks",
           delayTimeSpan, appliedDelayTimeSpan);

    TransitionToFlyoutVisiblePendingState_Original(
        object, targetItemKey, appliedDelayTimeSpan);
}

// Limits the MouseHoverTime override to the active preview dismissal source
class DismissTransitionScope {
   public:
    explicit DismissTransitionScope(DismissDelaySource source) noexcept
        : previousSource_(g_dismissDelaySource) {
        g_dismissDelaySource = source;
    }

    DismissTransitionScope(const DismissTransitionScope&) = delete;
    DismissTransitionScope& operator=(const DismissTransitionScope&) = delete;

    ~DismissTransitionScope() noexcept {
        g_dismissDelaySource = previousSource_;
    }

   private:
    DismissDelaySource previousSource_;
};

using TransitionToFlyoutDismissPendingState_t = void(WINAPI*)(void* object);
TransitionToFlyoutDismissPendingState_t
    TransitionToFlyoutDismissPendingState_Original;

void WINAPI TransitionToFlyoutDismissPendingState_Hook(void* object) {
    ULONGLONG thumbnailRemovalTick = 0;
    if (CanUseThumbnailRemoval() &&
        g_thumbnailRemovalTrackingState.removalFlyoutModel == object) {
        thumbnailRemovalTick =
            g_thumbnailRemovalTrackingState.removalTick;
        ClearThumbnailRemovalTag();
    }

    DismissDelaySource source = DismissDelaySource::pointerExit;
    if (thumbnailRemovalTick) {
        ULONGLONG elapsedMilliseconds =
            GetTickCount64() - thumbnailRemovalTick;
        if (elapsedMilliseconds <=
            THUMBNAIL_REMOVAL_CORRELATION_WINDOW_MS) {
            source = DismissDelaySource::thumbnailRemoval;
        } else {
            Wh_Log(L"Thumbnail-removal correlation expired after %llu ms",
                   elapsedMilliseconds);
        }
    }

    DismissTransitionScope dismissTransitionScope(source);
    TransitionToFlyoutDismissPendingState_Original(object);
}

using MouseHoverTime_t = UINT(WINAPI*)(void* object);
MouseHoverTime_t MouseHoverTime_Original;

UINT WINAPI MouseHoverTime_Hook(void* object) {
    UINT systemDelayMilliseconds = MouseHoverTime_Original(object);
    UINT configuredDelayMilliseconds = 0;
    const WCHAR* transitionName = nullptr;
    switch (g_dismissDelaySource) {
        case DismissDelaySource::none:
            return systemDelayMilliseconds;

        case DismissDelaySource::pointerExit:
            configuredDelayMilliseconds =
                g_closeDelayMilliseconds.load(std::memory_order_relaxed);
            transitionName = L"Pointer-exit close";
            break;

        case DismissDelaySource::thumbnailRemoval:
            configuredDelayMilliseconds =
                g_thumbnailRemovalDelayMilliseconds.load(
                    std::memory_order_relaxed);
            transitionName = L"Thumbnail-removal close";
            break;
    }

    Wh_Log(L"%s transition: %u -> %u ms",
           transitionName,
           systemDelayMilliseconds,
           configuredDelayMilliseconds);

    return configuredDelayMilliseconds;
}

bool HasTaskbarLightDismissSymbols() {
    return TaskbarHostRegisterLightDismiss_Original &&
           TaskbarHostUnregisterLightDismiss_Original &&
           TaskbarHostDestructor_Original &&
           CTaskBand_ITaskListWndSite_Vftable &&
           CSecondaryTaskBand_ITaskListWndSite_Vftable &&
           CTaskBandGetTaskbarHost_Original &&
           CSecondaryTaskBandGetTaskbarHost_Original &&
           ReferenceCountBaseDecrement_Original;
}

bool HasActiveHoverFlyoutLifecycleSymbols() {
    return UpdateFlyoutWindowPosition_Original &&
           CommitDismissFlyout_Original &&
           HoverFlyoutControllerDestructor_Original &&
           HideAllHoverFlyouts_Original;
}

bool HasFlyoutPointerTrackingSymbols() {
    return HasActiveHoverFlyoutLifecycleSymbols() &&
           SetIsPointerOverFlyoutFrame_Original &&
           HoverFlyoutModelDestructor_Original;
}

bool HasOutsideClickViewSymbols() {
    return HasFlyoutPointerTrackingSymbols() &&
           TaskbarModelHostWindowID_Original &&
           TaskbarModelIsExpanded_Original &&
           TaskbarControllerOnLightDismissTriggered_Original;
}

bool HasStartButtonHoverSymbols() {
    return HasActiveHoverFlyoutLifecycleSymbols() &&
           ExperienceToggleButtonOnPointerEntered_Original;
}

bool HasThumbnailRemovalSymbols() {
    return HasActiveHoverFlyoutLifecycleSymbols() &&
           TransitionToFlyoutVisiblePendingState_Original &&
           TransitionToFlyoutDismissPendingState_Original &&
           MouseHoverTime_Original &&
           HoverUIItemsCollectionSetTargetItem_Original &&
           HoverUIItemsCollectionDestructor_Original &&
           ThumbnailSourceArraySize_Original &&
           HoverUIItemsCollectionOnSourceArrayChanged_Original &&
           HoverFlyoutModelDestructor_Original;
}

bool CanTrackActiveHoverFlyout() {
    return g_taskbarViewHooksApplied.load(std::memory_order_acquire) &&
           HasActiveHoverFlyoutLifecycleSymbols();
}

bool CanTrackFlyoutPointerState() {
    return g_taskbarViewHooksApplied.load(std::memory_order_acquire) &&
           HasFlyoutPointerTrackingSymbols();
}

bool CanUseOutsideClick() {
    return g_taskbarHooksApplied.load(std::memory_order_acquire) &&
           g_taskbarViewHooksApplied.load(std::memory_order_acquire) &&
           HasTaskbarLightDismissSymbols() &&
           HasOutsideClickViewSymbols();
}

bool CanUseStartButtonHover() {
    return g_taskbarViewHooksApplied.load(std::memory_order_acquire) &&
           HasStartButtonHoverSymbols();
}

bool CanUseThumbnailRemoval() {
    return g_taskbarViewHooksApplied.load(std::memory_order_acquire) &&
           HasThumbnailRemovalSymbols();
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

    int thumbnailRemovalDelayMilliseconds =
        Wh_GetIntSetting(L"thumbnailRemovalDelayMs");
    if (thumbnailRemovalDelayMilliseconds < MINIMUM_DELAY_MS) {
        thumbnailRemovalDelayMilliseconds = MINIMUM_DELAY_MS;
    }

    LONGLONG delayTimeSpan =
        static_cast<LONGLONG>(delayMilliseconds) *
        TIME_SPAN_TICKS_PER_MILLISECOND;
    g_hoverDelayTimeSpan.store(delayTimeSpan, std::memory_order_relaxed);
    g_closeDelayMilliseconds.store(
        static_cast<UINT>(closeDelayMilliseconds),
        std::memory_order_relaxed);
    g_thumbnailRemovalDelayMilliseconds.store(
        static_cast<UINT>(thumbnailRemovalDelayMilliseconds),
        std::memory_order_relaxed);
    g_closeOnOutsideClick.store(
        Wh_GetIntSetting(L"closeOnOutsideClick") != 0,
        std::memory_order_relaxed);
    g_closeOnStartButtonHover.store(
        Wh_GetIntSetting(L"closeOnStartButtonHover") != 0,
        std::memory_order_relaxed);

    Wh_Log(L"Settings: delayMs=%d closeDelayMs=%d "
           L"thumbnailRemovalDelayMs=%d "
           L"closeOnOutsideClick=%d closeOnStartButtonHover=%d",
           delayMilliseconds,
           closeDelayMilliseconds,
           thumbnailRemovalDelayMilliseconds,
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

bool IsSupportedWindowsBuild() {
    using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW* versionInformation);

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll) {
        return true;
    }

    RtlGetVersion_t rtlGetVersion =
        reinterpret_cast<RtlGetVersion_t>(
            GetProcAddress(ntdll, "RtlGetVersion"));
    if (!rtlGetVersion) {
        return true;
    }

    OSVERSIONINFOW versionInformation = {};
    versionInformation.dwOSVersionInfoSize = sizeof(versionInformation);
    if (rtlGetVersion(&versionInformation) < 0) {
        return true;
    }

    return versionInformation.dwMajorVersion > 10 ||
           (versionInformation.dwMajorVersion == 10 &&
            versionInformation.dwBuildNumber >=
                MINIMUM_SUPPORTED_WINDOWS_BUILD);
}

bool IsTaskbarOwnedByAnotherProcess() {
    HWND taskbarWindow = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbarWindow) {
        return false;
    }

    DWORD taskbarProcessId = 0;
    GetWindowThreadProcessId(taskbarWindow, &taskbarProcessId);
    return taskbarProcessId &&
           taskbarProcessId != GetCurrentProcessId();
}

void MarkWaitingHookStatesNotApplicable() {
    if (g_taskbarHookInstallState ==
        HookInstallState::waitingForModule) {
        g_taskbarHookInstallState = HookInstallState::notApplicable;
    }

    if (g_taskbarViewHookInstallState ==
        HookInstallState::waitingForModule) {
        g_taskbarViewHookInstallState = HookInstallState::notApplicable;
    }
}

bool ResolveTaskbarSymbols(HMODULE module) {
    if (!module ||
        g_taskbarHookInstallState !=
            HookInstallState::waitingForModule) {
        return false;
    }

    // Mark the module terminal before symbol work so it is never resolved twice.
    g_taskbarHookInstallState = HookInstallState::failed;

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
        {
            {
                LR"(public: __cdecl TaskbarHost::~TaskbarHost(void))",
            },
            &TaskbarHostDestructor_Original,
            TaskbarHostDestructor_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                    ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"Failed to resolve Taskbar.dll symbols");
        return false;
    }

    g_taskbarHookInstallState = HookInstallState::queued;

    if (!HasTaskbarLightDismissSymbols()) {
        Wh_Log(L"Taskbar light-dismiss support isn't available on this build");
    }

    return true;
}

bool ResolveTaskbarViewSymbols(HMODULE module) {
    if (!module ||
        g_taskbarViewHookInstallState !=
            HookInstallState::waitingForModule) {
        return false;
    }

    // Mark the module terminal before symbol work so it is never resolved twice.
    g_taskbarViewHookInstallState = HookInstallState::failed;

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
                LR"(public: void __cdecl winrt::Taskbar::implementation::HoverUIItemsCollection::SetTargetItem(struct winrt::WindowsUdk::UI::Shell::TaskItem const &))",
            },
            &HoverUIItemsCollectionSetTargetItem_Original,
            HoverUIItemsCollectionSetTargetItem_Hook,
            true,
        },
        {
            {
                LR"(public: virtual __cdecl winrt::Taskbar::implementation::HoverUIItemsCollection::~HoverUIItemsCollection(void))",
            },
            &HoverUIItemsCollectionDestructor_Original,
            HoverUIItemsCollectionDestructor_Hook,
            true,
        },
        {
            {
                LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IObservableVector<struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail>,struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail>::Size(void)const )",
                LR"(public: unsigned int __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IObservableVector<struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail>,struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail>::Size(void)const )",
            },
            &ThumbnailSourceArraySize_Original,
            nullptr,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::HoverUIItemsCollection::OnSourceArrayChanged(struct winrt::Windows::Foundation::Collections::IObservableVector<struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail> const &,struct winrt::Windows::Foundation::Collections::IVectorChangedEventArgs const &))",
            },
            &HoverUIItemsCollectionOnSourceArrayChanged_Original,
            HoverUIItemsCollectionOnSourceArrayChanged_Hook,
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
                LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskbarModel<struct winrt::WindowsUdk::UI::Shell::ITaskbarModel>::HostWindowId(void)const )",
                LR"(public: unsigned __int64 __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskbarModel<struct winrt::WindowsUdk::UI::Shell::ITaskbarModel>::HostWindowId(void)const )",
            },
            &TaskbarModelHostWindowID_Original,
            TaskbarModelHostWindowID_Hook,
            true,
        },
        {
            {
                LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskbarModel3<struct winrt::WindowsUdk::UI::Shell::TaskbarModel>::IsExpanded(void)const )",
                LR"(public: bool __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskbarModel3<struct winrt::WindowsUdk::UI::Shell::TaskbarModel>::IsExpanded(void)const )",
            },
            &TaskbarModelIsExpanded_Original,
            nullptr,
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

    g_taskbarViewHookInstallState = HookInstallState::queued;

    if (!TransitionToFlyoutDismissPendingState_Original ||
        !MouseHoverTime_Original) {
        Wh_Log(L"Thumbnail close-delay symbols aren't available; the "
               L"close-delay override is disabled on this build");
    }

    if (!HasThumbnailRemovalSymbols()) {
        Wh_Log(L"Thumbnail-removal delay isn't available on this build");
    }

    if (g_closeOnOutsideClick.load(std::memory_order_relaxed) &&
        !HasOutsideClickViewSymbols()) {
        Wh_Log(L"Thumbnail light-dismiss symbols aren't available; "
               L"close-on-outside-click is disabled on this build");
    }

    if (g_closeOnStartButtonHover.load(std::memory_order_relaxed) &&
        !HasStartButtonHoverSymbols()) {
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

void PublishHookInstallState() {
    bool taskbarHooksApplied =
        g_taskbarHookInstallState == HookInstallState::applied;
    bool taskbarViewHooksApplied =
        g_taskbarViewHookInstallState == HookInstallState::applied;

    g_taskbarHooksApplied.store(
        taskbarHooksApplied, std::memory_order_release);
    g_taskbarViewHooksApplied.store(
        taskbarViewHooksApplied, std::memory_order_release);
    g_hookInstallationPending.store(
        g_taskbarHookInstallState ==
                HookInstallState::waitingForModule ||
            g_taskbarViewHookInstallState ==
                HookInstallState::waitingForModule,
        std::memory_order_release);
}

bool ResolveLoadedTaskbarSymbols() {
    bool hooksQueued = false;

    HMODULE taskbarModule = GetTaskbarModuleHandle();
    if (taskbarModule &&
        g_taskbarHookInstallState ==
            HookInstallState::waitingForModule) {
        hooksQueued = ResolveTaskbarSymbols(taskbarModule) || hooksQueued;
    }

    HMODULE taskbarViewModule = GetTaskbarViewModuleHandle();
    if (taskbarViewModule &&
        g_taskbarViewHookInstallState ==
            HookInstallState::waitingForModule) {
        hooksQueued =
            ResolveTaskbarViewSymbols(taskbarViewModule) || hooksQueued;
    }

    return hooksQueued;
}

bool ApplyQueuedTaskbarHooks() {
    bool taskbarHooksQueued =
        g_taskbarHookInstallState == HookInstallState::queued;
    bool taskbarViewHooksQueued =
        g_taskbarViewHookInstallState == HookInstallState::queued;
    if (!taskbarHooksQueued && !taskbarViewHooksQueued) {
        PublishHookInstallState();
        return true;
    }

    if (!Wh_ApplyHookOperations()) {
        if (taskbarHooksQueued) {
            g_taskbarHookInstallState = HookInstallState::failed;
        }
        if (taskbarViewHooksQueued) {
            g_taskbarViewHookInstallState = HookInstallState::failed;
        }
        PublishHookInstallState();
        return false;
    }

    if (taskbarHooksQueued) {
        g_taskbarHookInstallState = HookInstallState::applied;
    }
    if (taskbarViewHooksQueued) {
        g_taskbarViewHookInstallState = HookInstallState::applied;
    }
    PublishHookInstallState();
    return true;
}

enum class TaskbarModuleType {
    none,
    taskbar,
    taskbarView,
};

PCWSTR GetFileNamePart(PCWSTR path) {
    if (!path) {
        return nullptr;
    }

    PCWSTR fileName = path;
    for (PCWSTR character = path; *character; character++) {
        if (*character == L'\\' || *character == L'/') {
            fileName = character + 1;
        }
    }

    return fileName;
}

TaskbarModuleType GetTaskbarModuleType(PCWSTR path) {
    PCWSTR fileName = GetFileNamePart(path);
    if (!fileName) {
        return TaskbarModuleType::none;
    }

    if (_wcsicmp(fileName, L"Taskbar.dll") == 0) {
        return TaskbarModuleType::taskbar;
    }

    if (_wcsicmp(fileName, L"Taskbar.View.dll") == 0 ||
        _wcsicmp(fileName, L"ExplorerExtensions.dll") == 0) {
        return TaskbarModuleType::taskbarView;
    }

    return TaskbarModuleType::none;
}

void HandleLoadedModule(
    HMODULE module,
    TaskbarModuleType moduleType) {
    if (!module ||
        !g_lateHookApplicationReady.load(std::memory_order_acquire) ||
        !g_hookInstallationPending.load(std::memory_order_acquire) ||
        !g_initialized.load(std::memory_order_acquire) ||
        g_hookInstallationInProgress ||
        moduleType == TaskbarModuleType::none) {
        return;
    }

    AcquireSRWLockExclusive(&g_hookInstallLock);
    {
        HookInstallationScope hookInstallationScope;
        bool hooksQueued = false;
        if (g_initialized.load(std::memory_order_acquire) &&
            g_lateHookApplicationReady.load(std::memory_order_acquire) &&
            g_hookInstallationPending.load(std::memory_order_acquire)) {
            switch (moduleType) {
                case TaskbarModuleType::taskbar:
                    hooksQueued = ResolveTaskbarSymbols(module);
                    break;

                case TaskbarModuleType::taskbarView:
                    hooksQueued = ResolveTaskbarViewSymbols(module);
                    break;

                case TaskbarModuleType::none:
                    break;
            }

            if (hooksQueued) {
                if (!ApplyQueuedTaskbarHooks()) {
                    Wh_Log(L"Failed to apply late taskbar hooks");
                }
            } else {
                PublishHookInstallState();
            }
        }
    }
    ReleaseSRWLockExclusive(&g_hookInstallLock);
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName,
    HANDLE file,
    DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    if (!(flags & NONEXECUTABLE_LIBRARY_LOAD_FLAGS) &&
        g_lateHookApplicationReady.load(std::memory_order_acquire) &&
        g_hookInstallationPending.load(std::memory_order_acquire)) {
        HandleLoadedModule(module, GetTaskbarModuleType(fileName));
    }
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

using RunFromWindowThreadProcedure = void(WINAPI*)();

struct RUN_FROM_WINDOW_THREAD_PARAMETERS {
    RunFromWindowThreadProcedure procedure;
    bool executed;
};

UINT GetRunFromWindowThreadMessage() {
    static const UINT message =
        RegisterWindowMessageW(
            L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    return message;
}

bool RunFromWindowThread(
    HWND window,
    RunFromWindowThreadProcedure procedure) {
    if (!window || !procedure) {
        return false;
    }

    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) {
        return false;
    }

    UINT message = GetRunFromWindowThreadMessage();
    if (!message) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        procedure();
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                const CWPSTRUCT* message =
                    reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (message->message == GetRunFromWindowThreadMessage() &&
                    message->lParam) {
                    RUN_FROM_WINDOW_THREAD_PARAMETERS* parameters =
                        reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAMETERS*>(
                            message->lParam);
                    try {
                        parameters->procedure();
                        parameters->executed = true;
                    } catch (...) {
                        Wh_Log(L"Taskbar thread operation failed");
                    }
                }
            }

            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr,
        threadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAMETERS parameters = {
        procedure,
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

void WINAPI RefreshOutsideClickTaskbarThreadState() {
    if (g_closeOnOutsideClick.load(std::memory_order_relaxed) &&
        g_activeHoverFlyoutController) {
        ArmPreviewLightDismiss();
    } else {
        DisarmPreviewLightDismiss();
    }

    g_lastUnregisteredTaskbarHost = nullptr;
}

void WINAPI CleanupTaskbarThreadState() {
    RefreshOutsideClickTaskbarThreadState();
    g_thumbnailRemovalTrackingState = {};
    g_activeHoverFlyoutController = nullptr;
    g_activeTaskbarWindow = nullptr;
    g_activeTaskbarModelState = {};
    g_pointerOverFlyoutFrameModel = nullptr;
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
            context->procedure)) {
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

    if (!IsSupportedWindowsBuild()) {
        MarkWaitingHookStatesNotApplicable();
        PublishHookInstallState();
        g_initialized.store(true, std::memory_order_release);
        Wh_Log(L"The Windows 11 24H2 taskbar isn't available on this build");
        return TRUE;
    }

    if (!HookLoadLibraryExW()) {
        return FALSE;
    }

    AcquireSRWLockExclusive(&g_hookInstallLock);
    {
        HookInstallationScope hookInstallationScope;
        ResolveLoadedTaskbarSymbols();
        PublishHookInstallState();
    }
    ReleaseSRWLockExclusive(&g_hookInstallLock);

    g_initialized.store(true, std::memory_order_release);
    Wh_Log(L"Initialized");
    return TRUE;
}

void Wh_ModAfterInit() {
    AcquireSRWLockExclusive(&g_hookInstallLock);
    {
        HookInstallationScope hookInstallationScope;

        // Windhawk applies hooks queued by Wh_ModInit before this callback.
        if (g_taskbarHookInstallState == HookInstallState::queued) {
            g_taskbarHookInstallState = HookInstallState::applied;
        }
        if (g_taskbarViewHookInstallState == HookInstallState::queued) {
            g_taskbarViewHookInstallState = HookInstallState::applied;
        }

        // A concurrent late load will wait on the installation lock.
        // Publishing readiness here closes the gap between the module scan
        // and lock release.
        g_lateHookApplicationReady.store(true, std::memory_order_release);

        if (IsTaskbarOwnedByAnotherProcess()) {
            MarkWaitingHookStatesNotApplicable();
            Wh_Log(L"Taskbar is owned by another process; late taskbar hooks "
                   L"aren't applicable");
        }

        bool hooksQueued = ResolveLoadedTaskbarSymbols();
        if (hooksQueued) {
            if (!ApplyQueuedTaskbarHooks()) {
                Wh_Log(L"Failed to apply taskbar hooks loaded during "
                       L"initialization");
            }
        } else {
            PublishHookInstallState();
        }

        if (g_taskbarViewHookInstallState ==
            HookInstallState::waitingForModule) {
            Wh_Log(L"Taskbar.View isn't loaded yet");
        }
    }
    ReleaseSRWLockExclusive(&g_hookInstallLock);
}

void Wh_ModSettingsChanged() {
    bool previousCloseOnOutsideClick =
        g_closeOnOutsideClick.load(std::memory_order_relaxed);
    LoadSettings();

    HandleLoadedModule(
        GetTaskbarModuleHandle(), TaskbarModuleType::taskbar);
    HandleLoadedModule(
        GetTaskbarViewModuleHandle(), TaskbarModuleType::taskbarView);

    if (previousCloseOnOutsideClick !=
        g_closeOnOutsideClick.load(std::memory_order_relaxed)) {
        RunOnTaskbarThreads(RefreshOutsideClickTaskbarThreadState);
    }
}

void Wh_ModBeforeUninit() {
    g_initialized.store(false, std::memory_order_release);
    g_lateHookApplicationReady.store(false, std::memory_order_release);
    g_closeOnOutsideClick.store(false, std::memory_order_relaxed);
    g_closeOnStartButtonHover.store(false, std::memory_order_relaxed);

    AcquireSRWLockExclusive(&g_hookInstallLock);
    ReleaseSRWLockExclusive(&g_hookInstallLock);

    RunOnTaskbarThreads(CleanupTaskbarThreadState);

    g_taskbarHooksApplied.store(false, std::memory_order_release);
    g_taskbarViewHooksApplied.store(false, std::memory_order_release);
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitialized");
}
