// ==WindhawkMod==
// @id              instant-taskbar-thumbnail-previews
// @name            Instant Taskbar Thumbnail Previews
// @description     Controls taskbar thumbnail preview show and close behavior.
// @version         1.2.1
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

![Demo](https://raw.githubusercontent.com/alchemyyy/windhawk-mods/assets/instant-taskbar-thumbnail-previews-demo.gif)

Adds the ability to configure or remove the delays before Windows 11 shows and closes taskbar thumbnail previews,
as well as the ability to configure mouse actions that close them.

The hover and close delay settings replace the corresponding Windows value, so they can make the transitions either shorter or longer than the stock delays.

### Aim boundary feature (as of version 1.2)

This feature makes flicking to thumbnail previews more ergonomic. 

It does this by suppressing taskbar icon hovers closing the currently open previews and possibly opening their own, under certain conditions.

The trick is tracking the *motion* of the cursor. If the cursor is not moving towards any of the thumbnail previews, then we don't suppress taskbar icon hover events.

![Demo](https://raw.githubusercontent.com/alchemyyy/windhawk-mods/refs/heads/assets/instant-taskbar-thumbnails-aim-boundary-explanation.png)


## Changelog 
### Version 1.2.1

* Fixed the hover delay, the aim boundary, and "Close on outside click" not working on recent Windows 11 taskbar updates.

### Version 1.2

* New feature: Aim boundary
    * Keeps open thumbnail previews attached to their taskbar button while the pointer travels toward them.
    * Not compatible with vertical taskbars.

### Version 1.1

* Changed the default "Thumbnail close delay" from 1 to 200 milliseconds.
    * Set it to 1 to retain effectively immediate closing when the pointer leaves the thumbnail area.
* New setting: "Delay after thumbnail removal"
    * controls how long the remaining thumbnail previews stay open after one is removed, whether it was closed from the preview or externally. The normal close delay applies when the last thumbnail is removed.

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
  $description: How long the remaining previews stay open after one is removed, including when its window closes outside the preview. The normal close delay applies when none remain. The minimum value is 1, which is effectively immediate.
- closeOnOutsideClick: false
  $name: Close on outside click
  $description: Immediately close open thumbnail previews when clicking outside the taskbar and thumbnail surface.
- closeOnStartButtonHover: false
  $name: Close on Start button hover
  $description: Immediately close open thumbnail previews when hovering over the Start button.
- aimTowardPreviews: false
  $name: Keep previews while moving toward them (aim boundary)
  $description: While the pointer moves from a taskbar button toward its open thumbnail previews, ignore other taskbar buttons it crosses and don't start closing the previews. Not compatible with vertical taskbars.
- aimAnchorHeightPercent: 100
  $name: Aim boundary height on the button (%)
  $description: Where the two aim boundary lines meet the button's left and right edges, as a percentage of the button's height measured down from its top edge.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>

#include <algorithm>
#include <atomic>
#include <mutex>
#include <unordered_map>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

#include <string_view>

namespace {

constexpr int MINIMUM_DELAY_MS = 1;
constexpr LONGLONG TIME_SPAN_TICKS_PER_MILLISECOND = 10000LL;
constexpr DWORD MINIMUM_SUPPORTED_WINDOWS_BUILD = 26100;
constexpr DWORD NONEXECUTABLE_LIBRARY_LOAD_FLAGS =
    LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
    LOAD_LIBRARY_AS_IMAGE_RESOURCE;

// Allow deferred layout and pointer events while rejecting later pointer exits
constexpr ULONGLONG THUMBNAIL_REMOVAL_CORRELATION_WINDOW_MS = 150;

// Pointer-aim tracking samples the cursor while a thumbnail flyout is open.
// Direction is measured across at least the minimum motion distance, and the
// pointer counts as at rest once it drifts slower than the minimum drift.
constexpr UINT HOVER_AIM_SAMPLE_INTERVAL_MS = USER_TIMER_MINIMUM;
constexpr LONG HOVER_AIM_MINIMUM_MOTION_PIXELS = 3;
constexpr double HOVER_AIM_MINIMUM_DRIFT_PIXELS_PER_SECOND = 0.01;
constexpr ULONGLONG HOVER_AIM_MOTION_WINDOW_MS = static_cast<ULONGLONG>(
    HOVER_AIM_MINIMUM_MOTION_PIXELS * 1000.0 /
    HOVER_AIM_MINIMUM_DRIFT_PIXELS_PER_SECOND);
// Unique among the timers on the taskbar window the tracking timer uses
constexpr UINT_PTR HOVER_AIM_TIMER_ID = 0x41494D54;
constexpr size_t HOVER_AIM_CURSOR_SAMPLE_COUNT = 32;
constexpr size_t HOVER_AIM_DEFERRED_EVENT_CAPACITY = 32;
constexpr size_t HOVER_AIM_TARGET_CAPACITY = 8;
// FlyoutFrame's template parts are a few levels below the frame
constexpr int HOVER_AIM_TEMPLATE_SEARCH_DEPTH = 8;
constexpr int DEFAULT_AIM_ANCHOR_HEIGHT_PERCENT = 100;

// winrt::WindowsUdk::UI::Shell::TaskItemVisualState values that
// HoverFlyoutModel::OnVisualStateStateChanged treats as pointer exit and enter.
constexpr INT32 TASK_ITEM_VISUAL_STATE_NORMAL = 0;
constexpr INT32 TASK_ITEM_VISUAL_STATE_POINTER_OVER = 1;

std::atomic<UINT> g_hoverDelayMilliseconds{MINIMUM_DELAY_MS};
std::atomic<UINT> g_closeDelayMilliseconds{MINIMUM_DELAY_MS};
std::atomic<UINT> g_thumbnailRemovalDelayMilliseconds{MINIMUM_DELAY_MS};
std::atomic<bool> g_closeOnOutsideClick{false};
std::atomic<bool> g_closeOnStartButtonHover{false};
std::atomic<bool> g_aimTowardPreviews{true};
std::atomic<int> g_aimAnchorHeightPercent{DEFAULT_AIM_ANCHOR_HEIGHT_PERCENT};
std::atomic<bool> g_initialized{false};
std::atomic<bool> g_taskbarModuleHandled{false};
std::atomic<bool> g_taskbarViewModuleHandled{false};

enum class LightDismissOwnership {
    unknown,
    unregistered,
    windows,
    preview,
};

using LightDismissOwnershipMap =
    std::unordered_map<void*, LightDismissOwnership>;

std::atomic<bool> g_lightDismissOwnershipTrackingReliable{true};
std::mutex g_lightDismissOwnershipMutex;
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
// The HoverFlyoutModel whose OnPointerEnteredItemWithHoverFlyout is running
thread_local void* g_hoverDelayFlyoutModel = nullptr;
thread_local ThumbnailRemovalTrackingState g_thumbnailRemovalTrackingState = {};
thread_local HWND g_activeTaskbarWindow = nullptr;
thread_local bool g_previewLightDismissArmed = false;
thread_local HWND g_previewLightDismissTaskbarWindow = nullptr;
thread_local void* g_previewLightDismissTaskbarHost = nullptr;
thread_local void* g_activeHoverFlyoutController = nullptr;
thread_local void* g_pointerOverFlyoutFrameModel = nullptr;
thread_local void* g_lastUnregisteredTaskbarHost = nullptr;

// Window IDs UpdateFlyoutWindowPosition asks the taskbar model and the flyout
// for while it positions the flyout
struct WindowIDCapture {
    UINT64 windowID;
    bool captured;
    bool active;
};

// Kept by value since MSVC exceptions unwind mod frames without destructors
// A skipped restore then leaves stale state, never a dangling stack pointer
thread_local WindowIDCapture g_hostWindowCapture = {};
thread_local WindowIDCapture g_flyoutWindowCapture = {};

// RegisterLightDismiss keeps an action the host already holds and skips a
// frame outside the tree; it subscribes to Dismissed only on an action it
// has just created.
struct LightDismissRegistrationProbe {
    bool actionCreated;
    bool active;
};

thread_local LightDismissRegistrationProbe g_lightDismissRegistrationProbe =
    {};

struct HoverAimCursorSample {
    POINT point;
    ULONGLONG tick;
};

enum class DeferredHoverAimEventKind : UINT8 {
    visualStateChange,
    dismiss,
};

struct DeferredHoverAimEvent {
    DeferredHoverAimEventKind kind;
    // Owned HSTRING of the taskbar item key; null for dismiss events
    void* itemKey;
    bool hasHoverFlyout;
    INT32 previousState;
    INT32 currentState;
};

// Plain data only: thread_local destructors would outlive the mod module
struct HoverAimState {
    // Owned IWeakReference pointers to the flyout frame, its targets, and the
    // preview card found in the frame's template
    void* frameWeakReference;
    void* targetWeakReferences[HOVER_AIM_TARGET_CAPACITY];
    size_t targetCount;
    void* cardWeakReference;
    HWND flyoutWindow;
    HWND timerWindow;
    HoverAimCursorSample cursorSamples[HOVER_AIM_CURSOR_SAMPLE_COUNT];
    size_t nextCursorSample;
    size_t cursorSampleCount;
    void* deferredModel;
    DeferredHoverAimEvent deferredEvents[HOVER_AIM_DEFERRED_EVENT_CAPACITY];
    size_t deferredEventCount;
    bool flushing;
};

thread_local HoverAimState g_hoverAimState = {};

bool HasActiveHoverFlyoutLifecycleSymbols();
bool HasFlyoutPointerTrackingSymbols();
bool HasOutsideClickSymbols();
bool HasStartButtonHoverSymbols();
bool HasTaskbarLightDismissSymbols();
bool HasThumbnailRemovalSymbols();
bool HasHoverAimSymbols();
void StartHoverAimTracking();
void StopHoverAimTracking();
void FlushDeferredHoverAimEvents(const WCHAR* reason);
void DiscardDeferredHoverAimEvents();
void ReleaseHoverAimTargets();
bool TryHoldHoverAimDismiss(void* model);

void ClearThumbnailRemovalTag() {
    g_thumbnailRemovalTrackingState.removalFlyoutModel = nullptr;
    g_thumbnailRemovalTrackingState.removalTick = 0;
}

// Every visible-pending transition, inlined or not, starts a new hover
// target, which supersedes an unconsumed removal transition.
void BeginHoverTarget(void* flyoutModel) {
    if (!HasThumbnailRemovalSymbols()) {
        return;
    }

    ClearThumbnailRemovalTag();
    g_thumbnailRemovalTrackingState.activeThumbnailItemsCollection = nullptr;
    g_thumbnailRemovalTrackingState.activeFlyoutModel = flyoutModel;
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

// Returns the event token through the hidden pointer, as MSVC does for every
// member function that returns a class.
using LightDismissActionDismissed_t = void*(WINAPI*)(const void* object,
                                                     void* eventToken,
                                                     const void* handler);
LightDismissActionDismissed_t LightDismissActionDismissed_Original;

using TaskbarHostDestructor_t = void(WINAPI*)(void* object);
TaskbarHostDestructor_t TaskbarHostDestructor_Original;

// Taskbar.dll implements the WindowsUdk TaskbarModel and TaskbarFlyout, so
// their ABI getters see every Taskbar.View.dll call, whether it goes through a
// consume wrapper or, with newer C++/WinRT versions, straight to the vtable.
using TaskbarModelGetHostWindowID_t =
    HRESULT(WINAPI*)(void* object, UINT64* hostWindowID);
TaskbarModelGetHostWindowID_t TaskbarModelGetHostWindowID_Original;

using TaskbarFlyoutGetWindowID_t =
    HRESULT(WINAPI*)(void* object, UINT64* windowID);
TaskbarFlyoutGetWindowID_t TaskbarFlyoutGetWindowID_Original;

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
    getTaskbarHost(taskBandForTaskListWindowSite, taskbarHostSharedPointer);

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

LightDismissOwnership GetLightDismissOwnership(void* taskbarHost) {
    if (!taskbarHost ||
        !g_lightDismissOwnershipTrackingReliable.load(
            std::memory_order_acquire)) {
        return LightDismissOwnership::unknown;
    }

    std::lock_guard<std::mutex> lock(g_lightDismissOwnershipMutex);
    if (!g_lightDismissOwnershipTrackingReliable.load(
            std::memory_order_acquire)) {
        return LightDismissOwnership::unknown;
    }

    LightDismissOwnershipMap::const_iterator iterator =
        g_lightDismissOwnershipByTaskbarHost.find(taskbarHost);
    return iterator == g_lightDismissOwnershipByTaskbarHost.end()
               ? LightDismissOwnership::unknown
               : iterator->second;
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
    {
        std::lock_guard<std::mutex> lock(g_lightDismissOwnershipMutex);
        if (!g_lightDismissOwnershipTrackingReliable.load(
                std::memory_order_acquire)) {
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
    }

    if (updateFailed) {
        Wh_Log(L"Failed to track light-dismiss ownership; preview "
               L"registration is disabled");
    }
}

void ForgetLightDismissOwnership(void* taskbarHost) {
    std::lock_guard<std::mutex> lock(g_lightDismissOwnershipMutex);
    g_lightDismissOwnershipByTaskbarHost.erase(taskbarHost);
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

void* WINAPI LightDismissActionDismissed_Hook(const void* object,
                                              void* eventToken,
                                              const void* handler) {
    void* result =
        LightDismissActionDismissed_Original(object, eventToken, handler);
    if (g_lightDismissRegistrationProbe.active) {
        g_lightDismissRegistrationProbe.actionCreated = true;
    }

    return result;
}

// Calls the trampoline, which bypasses the ownership-tracking hook, and
// returns whether the call created the host's action.
bool RegisterLightDismissCreatingAction(void* taskbarHost) {
    LightDismissRegistrationProbe previousProbe =
        g_lightDismissRegistrationProbe;
    g_lightDismissRegistrationProbe = {false, true};
    TaskbarHostRegisterLightDismiss_Original(taskbarHost);
    bool actionCreated = g_lightDismissRegistrationProbe.actionCreated;
    g_lightDismissRegistrationProbe = previousProbe;
    return actionCreated;
}

void WINAPI TaskbarHostRegisterLightDismiss_Hook(void* object) {
    if (!HasTaskbarLightDismissSymbols()) {
        TaskbarHostRegisterLightDismiss_Original(object);
        return;
    }

    bool previewActionHeld =
        g_previewLightDismissTaskbarHost == object ||
        GetLightDismissOwnership(object) == LightDismissOwnership::preview;
    // Windows keeps a held preview action as its own.
    if (RegisterLightDismissCreatingAction(object) || previewActionHeld) {
        SetLightDismissOwnership(object, LightDismissOwnership::windows);
    }

    ClearPreviewLightDismissStateForHost(object);
}

void WINAPI TaskbarHostUnregisterLightDismiss_Hook(void* object) {
    TaskbarHostUnregisterLightDismiss_Original(object);

    if (!HasTaskbarLightDismissSymbols()) {
        return;
    }

    SetLightDismissOwnership(object, LightDismissOwnership::unregistered);
    ClearPreviewLightDismissStateForHost(object);
    g_lastUnregisteredTaskbarHost = object;
}

void WINAPI TaskbarHostDestructor_Hook(void* object) {
    // Clean up last: teardown may re-add this host via the unregister hook
    TaskbarHostDestructor_Original(object);
    ClearPreviewLightDismissStateForHost(object);
    ForgetLightDismissOwnership(object);
}

// Older builds reach these through HoverFlyoutModel::SetIsPointerOverFlyoutFrame,
// which tail-calls them; newer builds inline that setter into the controller.
using HoverFlyoutModelOnPointerEnteredFlyoutFrame_t =
    void(WINAPI*)(void* object);
HoverFlyoutModelOnPointerEnteredFlyoutFrame_t
    HoverFlyoutModelOnPointerEnteredFlyoutFrame_Original;

using HoverFlyoutModelOnPointerExitedFlyoutFrame_t =
    void(WINAPI*)(void* object);
HoverFlyoutModelOnPointerExitedFlyoutFrame_t
    HoverFlyoutModelOnPointerExitedFlyoutFrame_Original;

using HoverFlyoutModelDestructor_t = void(WINAPI*)(void* object);
HoverFlyoutModelDestructor_t HoverFlyoutModelDestructor_Original;

void WINAPI HoverFlyoutModelOnPointerEnteredFlyoutFrame_Hook(void* object) {
    if (HasFlyoutPointerTrackingSymbols()) {
        g_pointerOverFlyoutFrameModel = object;
    }

    // Held hover changes precede the frame entry, matching the native order
    // of item exit, dismiss-pending, then frame entry cancelling it.
    FlushDeferredHoverAimEvents(L"pointer reached the previews");
    HoverFlyoutModelOnPointerEnteredFlyoutFrame_Original(object);
}

void WINAPI HoverFlyoutModelOnPointerExitedFlyoutFrame_Hook(void* object) {
    if (g_pointerOverFlyoutFrameModel == object) {
        g_pointerOverFlyoutFrameModel = nullptr;
    }

    HoverFlyoutModelOnPointerExitedFlyoutFrame_Original(object);
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

    if (g_hoverAimState.deferredModel == object) {
        DiscardDeferredHoverAimEvents();
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
        !HasOutsideClickSymbols() ||
        !g_lightDismissOwnershipTrackingReliable.load(
            std::memory_order_acquire)) {
        return;
    }

    // LightDismissAction observes the taskbar InputSite without taking focus.
    HWND taskbarWindow = g_activeTaskbarWindow;
    void* registeredTaskbarHost = nullptr;
    bool registered = WithTaskbarHost(
        taskbarWindow,
        [&registeredTaskbarHost](void* taskbarHost) {
            LightDismissOwnership ownership =
                GetLightDismissOwnership(taskbarHost);
            // A Windows action already raises the event the previews close on.
            if (ownership == LightDismissOwnership::windows) {
                return false;
            }

            // Registration creates an action only when the host holds none,
            // and one the hooks never saw is Windows'. TaskbarModel::IsExpanded
            // can't tell: the desktop taskbar reports expanded without
            // registering one.
            if (!RegisterLightDismissCreatingAction(taskbarHost)) {
                if (ownership == LightDismissOwnership::unknown) {
                    SetLightDismissOwnership(
                        taskbarHost,
                        LightDismissOwnership::windows);
                }

                return false;
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
        g_pointerOverFlyoutFrameModel = nullptr;
        StopHoverAimTracking();
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
    FlushDeferredHoverAimEvents(L"previews closed");
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
    // Replayed after the close so the model sees the pointer's current item
    FlushDeferredHoverAimEvents(L"previews closed");
}

using UpdateFlyoutWindowPosition_t = void(WINAPI*)(void* object);
UpdateFlyoutWindowPosition_t UpdateFlyoutWindowPosition_Original;

void CaptureWindowID(WindowIDCapture& capture,
                     HRESULT result,
                     const UINT64* windowID) {
    if (capture.active && !capture.captured && SUCCEEDED(result) &&
        *windowID) {
        capture.captured = true;
        capture.windowID = *windowID;
    }
}

HRESULT WINAPI TaskbarModelGetHostWindowID_Hook(void* object,
                                                UINT64* hostWindowID) {
    HRESULT result =
        TaskbarModelGetHostWindowID_Original(object, hostWindowID);

    // UpdateFlyoutWindowPosition asks its own TaskbarModel for the taskbar
    // window the flyout belongs to.
    CaptureWindowID(g_hostWindowCapture, result, hostWindowID);
    return result;
}

HRESULT WINAPI TaskbarFlyoutGetWindowID_Hook(void* object, UINT64* windowID) {
    HRESULT result = TaskbarFlyoutGetWindowID_Original(object, windowID);

    // UpdateFlyoutWindowPosition asks its TaskbarFlyout for the window it
    // sizes to the monitor and hosts the flyout frame in.
    CaptureWindowID(g_flyoutWindowCapture, result, windowID);
    return result;
}

void WINAPI UpdateFlyoutWindowPosition_Hook(void* object) {
    WindowIDCapture previousHostWindowCapture = g_hostWindowCapture;
    WindowIDCapture previousFlyoutWindowCapture = g_flyoutWindowCapture;
    bool initialized = g_initialized.load(std::memory_order_acquire);
    g_hostWindowCapture = {0, false, initialized};
    g_flyoutWindowCapture = {0, false, initialized};
    UpdateFlyoutWindowPosition_Original(object);
    WindowIDCapture hostWindowCapture = g_hostWindowCapture;
    WindowIDCapture flyoutWindowCapture = g_flyoutWindowCapture;
    g_hostWindowCapture = previousHostWindowCapture;
    g_flyoutWindowCapture = previousFlyoutWindowCapture;

    if (!g_initialized.load(std::memory_order_acquire) ||
        !HasActiveHoverFlyoutLifecycleSymbols()) {
        return;
    }

    HWND taskbarWindow = nullptr;
    if (hostWindowCapture.captured) {
        taskbarWindow = NormalizeTaskbarWindow(
            reinterpret_cast<HWND>(hostWindowCapture.windowID));
    }

    bool controllerChanged =
        g_activeHoverFlyoutController != object;
    bool taskbarChanged =
        taskbarWindow &&
        g_activeTaskbarWindow != taskbarWindow;
    bool taskbarWindowInvalidated =
        hostWindowCapture.captured && !taskbarWindow;
    if (controllerChanged || taskbarChanged ||
        taskbarWindowInvalidated) {
        DisarmPreviewLightDismiss();
        if (controllerChanged) {
            // The visible-pending model transition precedes the controller's
            // first position update, so its removal state is already current.
            g_pointerOverFlyoutFrameModel = nullptr;
        }

        if (controllerChanged || taskbarWindowInvalidated) {
            g_activeTaskbarWindow = nullptr;
        }
    }

    g_activeHoverFlyoutController = object;
    if (taskbarWindow) {
        g_activeTaskbarWindow = taskbarWindow;
    }

    if (controllerChanged) {
        g_hoverAimState.flyoutWindow = nullptr;
    }

    if (flyoutWindowCapture.captured) {
        HWND flyoutWindow =
            reinterpret_cast<HWND>(flyoutWindowCapture.windowID);
        g_hoverAimState.flyoutWindow =
            IsWindow(flyoutWindow) ? flyoutWindow : nullptr;
    }

    StartHoverAimTracking();
    ArmPreviewLightDismiss();
}

using HideAllHoverFlyouts_t = void(WINAPI*)(void* object);
HideAllHoverFlyouts_t HideAllHoverFlyouts_Original;

void WINAPI HideAllHoverFlyouts_Hook(void* object) {
    ClearActiveHoverFlyoutController(object);
    HideAllHoverFlyouts_Original(object);
    FlushDeferredHoverAimEvents(L"previews hidden");
}

void WINAPI HoverFlyoutControllerDestructor_Hook(void* object) {
    ClearActiveHoverFlyoutController(object);
    // The owned model may be destroyed with the controller
    DiscardDeferredHoverAimEvents();
    HoverFlyoutControllerDestructor_Original(object);
}

void WINAPI TaskbarControllerOnLightDismissTriggered_Hook(
    void* object,
    const void* taskbarModel,
    const void* eventArguments) {
    void* taskbarHost = g_lastUnregisteredTaskbarHost;
    g_lastUnregisteredTaskbarHost = nullptr;
    if (!g_closeOnOutsideClick.load(std::memory_order_relaxed) ||
        !HasOutsideClickSymbols()) {
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
        HasStartButtonHoverSymbols() &&
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
    if (!HasThumbnailRemovalSymbols() ||
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

bool ThumbnailRemovalLeavesPreviewItems(const void* sourceArray) {
    if (!sourceArray || !ThumbnailSourceArraySize_Original) {
        return false;
    }

    // The source vector has already removed the item before raising this
    // callback, so its semantic Size getter reports the remaining count.
    return ThumbnailSourceArraySize_Original(sourceArray) > 0;
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
    if (HasThumbnailRemovalSymbols()) {
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
    BeginHoverTarget(object);

    LONGLONG configuredDelayTimeSpan =
        static_cast<LONGLONG>(
            g_hoverDelayMilliseconds.load(std::memory_order_relaxed)) *
        TIME_SPAN_TICKS_PER_MILLISECOND;
    LONGLONG appliedDelayTimeSpan =
        delayTimeSpan > 0 ? configuredDelayTimeSpan : delayTimeSpan;

    Wh_Log(L"Hover transition: %lld -> %lld TimeSpan ticks",
           delayTimeSpan, appliedDelayTimeSpan);

    TransitionToFlyoutVisiblePendingState_Original(
        object, targetItemKey, appliedDelayTimeSpan);
}

// Windows times its visible-pending transition from MouseHoverTime here.
// Newer builds inline that transition for a first hover instead of calling
// TransitionToFlyoutVisiblePendingState.
using HoverFlyoutModelOnPointerEnteredItemWithHoverFlyout_t =
    void(WINAPI*)(void* object, const void* itemKey);
HoverFlyoutModelOnPointerEnteredItemWithHoverFlyout_t
    HoverFlyoutModelOnPointerEnteredItemWithHoverFlyout_Original;

void WINAPI HoverFlyoutModelOnPointerEnteredItemWithHoverFlyout_Hook(
    void* object,
    const void* itemKey) {
    // Limits the MouseHoverTime override to this handler
    // MSVC exceptions skip mod cleanup, so reset instead of restoring
    g_hoverDelayFlyoutModel = object;
    HoverFlyoutModelOnPointerEnteredItemWithHoverFlyout_Original(object,
                                                                 itemKey);
    g_hoverDelayFlyoutModel = nullptr;
}

using TransitionToFlyoutDismissPendingState_t = void(WINAPI*)(void* object);
TransitionToFlyoutDismissPendingState_t
    TransitionToFlyoutDismissPendingState_Original;

void WINAPI TransitionToFlyoutDismissPendingState_Hook(void* object) {
    ULONGLONG thumbnailRemovalTick = 0;
    if (HasThumbnailRemovalSymbols() &&
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

    // Pointer exits are held while the pointer heads toward the flyout. Any
    // other close must reach the model after the hover changes held so far.
    if (source == DismissDelaySource::pointerExit) {
        if (TryHoldHoverAimDismiss(object)) {
            return;
        }
    } else {
        FlushDeferredHoverAimEvents(L"thumbnail removal");
    }

    // Limits the MouseHoverTime override to this transition
    // MSVC exceptions skip mod cleanup, so reset instead of restoring
    g_dismissDelaySource = source;
    TransitionToFlyoutDismissPendingState_Original(object);
    g_dismissDelaySource = DismissDelaySource::none;
}

using MouseHoverTime_t = UINT(WINAPI*)(void* object);
MouseHoverTime_t MouseHoverTime_Original;

UINT WINAPI MouseHoverTime_Hook(void* object) {
    UINT systemDelayMilliseconds = MouseHoverTime_Original(object);
    UINT configuredDelayMilliseconds = 0;
    const WCHAR* transitionName = nullptr;
    switch (g_dismissDelaySource) {
        case DismissDelaySource::none:
            if (!g_hoverDelayFlyoutModel) {
                return systemDelayMilliseconds;
            }

            // The handler reads it only to start a visible-pending transition
            BeginHoverTarget(g_hoverDelayFlyoutModel);
            configuredDelayMilliseconds =
                g_hoverDelayMilliseconds.load(std::memory_order_relaxed);
            transitionName = L"Hover";
            break;

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

using HoverFlyoutModelOnVisualStateStateChanged_t =
    void(WINAPI*)(void* object,
                  const winrt::hstring& itemKey,
                  bool hasHoverFlyout,
                  INT32 previousState,
                  INT32 currentState);
HoverFlyoutModelOnVisualStateStateChanged_t
    HoverFlyoutModelOnVisualStateStateChanged_Original;

// Reports whether the model has a target item, including pending states
using HoverFlyoutModelIsFlyoutVisible_t = bool(WINAPI*)(const void* object);
HoverFlyoutModelIsFlyoutVisible_t HoverFlyoutModelIsFlyoutVisible_Original;

// The std::vector<winrt::weak_ref<FrameworkElement>> is passed by value, so
// the callee receives the address of the caller's temporary.
using FlyoutFrameTargets_t = void(WINAPI*)(void* object, void* targets);
FlyoutFrameTargets_t FlyoutFrameTargets_Original;

struct WeakReferenceVector {
    void** first;
    void** last;
    void** end;
};

struct HoverAimBounds {
    double left;
    double top;
    double right;
    double bottom;
};

struct HoverAimGeometry {
    double buttonLeft;
    double buttonRight;
    double anchorY;
    double flyoutLeft;
    double flyoutRight;
    double flyoutBottom;
};

enum class HoverAimResult {
    aiming,
    unavailable,
    holdCapacityReached,
    mouseButtonDown,
    atRest,
    noGeometry,
    notBelowPreviews,
    outsideBoundaries,
};

struct HoverAimEvaluation {
    HoverAimGeometry geometry;
    POINT cursor;
    double motionX;
    double motionY;
    bool hasCursor;
    bool hasMotion;
    bool hasGeometry;
};

const WCHAR* DescribeHoverAimResult(HoverAimResult result) {
    switch (result) {
        case HoverAimResult::aiming:
            return L"pointer aiming at the previews";
        case HoverAimResult::unavailable:
            return L"aim tracking inactive";
        case HoverAimResult::holdCapacityReached:
            return L"too many held changes";
        case HoverAimResult::mouseButtonDown:
            return L"mouse button down";
        case HoverAimResult::atRest:
            return L"pointer at rest";
        case HoverAimResult::noGeometry:
            return L"button or preview bounds unavailable";
        case HoverAimResult::notBelowPreviews:
            return L"pointer not below the previews";
        case HoverAimResult::outsideBoundaries:
            return L"motion outside the boundary lines";
    }

    return L"unknown";
}

void ReleaseAbiReference(void*& reference) {
    if (reference) {
        static_cast<winrt::impl::unknown_abi*>(reference)->Release();
        reference = nullptr;
    }
}

// The object must start with an IUnknown-compatible vtable. FlyoutFrame's
// first vtable is its implements<> IUnknown.
void* CreateWeakReference(void* object) {
    void* source = nullptr;
    if (static_cast<winrt::impl::unknown_abi*>(object)->QueryInterface(
            winrt::impl::guid_v<winrt::impl::IWeakReferenceSource>,
            &source) < 0 ||
        !source) {
        return nullptr;
    }

    winrt::impl::IWeakReference* weakReference = nullptr;
    int32_t result =
        static_cast<winrt::impl::IWeakReferenceSource*>(source)
            ->GetWeakReference(&weakReference);
    static_cast<winrt::impl::unknown_abi*>(source)->Release();
    return result >= 0 ? weakReference : nullptr;
}

winrt::Windows::UI::Xaml::FrameworkElement ResolveFrameworkElement(
    void* weakReference) {
    winrt::Windows::UI::Xaml::FrameworkElement element = nullptr;
    if (weakReference) {
        static_cast<winrt::impl::IWeakReference*>(weakReference)->Resolve(
            winrt::guid_of<winrt::Windows::UI::Xaml::FrameworkElement>(),
            winrt::put_abi(element));
    }

    return element;
}

void ReleaseHoverAimTargets() {
    HoverAimState& state = g_hoverAimState;
    ReleaseAbiReference(state.frameWeakReference);
    ReleaseAbiReference(state.cardWeakReference);
    for (size_t index = 0; index < state.targetCount; index++) {
        ReleaseAbiReference(state.targetWeakReferences[index]);
    }

    state.targetCount = 0;
}

void TrackFlyoutFrameTargets(void* frame, const void* targets) {
    ReleaseHoverAimTargets();

    HoverAimState& state = g_hoverAimState;
    state.frameWeakReference = CreateWeakReference(frame);
    if (!state.frameWeakReference) {
        return;
    }

    const WeakReferenceVector* vector =
        static_cast<const WeakReferenceVector*>(targets);
    for (void** target = vector->first;
         target != vector->last &&
         state.targetCount < HOVER_AIM_TARGET_CAPACITY;
         target++) {
        if (!*target) {
            continue;
        }

        static_cast<winrt::impl::unknown_abi*>(*target)->AddRef();
        state.targetWeakReferences[state.targetCount++] = *target;
    }
}

void WINAPI FlyoutFrameTargets_Hook(void* object, void* targets) {
    // Read before the original moves the vector into the frame
    if (HasHoverAimSymbols() && targets) {
        TrackFlyoutFrameTargets(object, targets);
    }

    FlyoutFrameTargets_Original(object, targets);
}

HWND GetXamlContentWindow(HWND hostWindow) {
    HWND contentWindow = FindWindowExW(
        hostWindow,
        nullptr,
        L"Windows.UI.Composition.DesktopWindowContentBridge",
        nullptr);
    return contentWindow ? contentWindow : hostWindow;
}

bool GetElementScreenBounds(
    const winrt::Windows::UI::Xaml::FrameworkElement& element,
    HWND hostWindow,
    HoverAimBounds* bounds) {
    double width = element.ActualWidth();
    double height = element.ActualHeight();
    if (!(width > 0) || !(height > 0)) {
        return false;
    }

    winrt::Windows::UI::Xaml::XamlRoot xamlRoot = element.XamlRoot();
    if (!xamlRoot) {
        return false;
    }

    // The native flyout placement converts window pixels with this scale
    double scale = xamlRoot.RasterizationScale();
    if (!(scale > 0)) {
        return false;
    }

    winrt::Windows::Foundation::Rect elementBounds =
        element.TransformToVisual(nullptr).TransformBounds(
            winrt::Windows::Foundation::Rect{
                0.0f,
                0.0f,
                static_cast<float>(width),
                static_cast<float>(height),
            });

    POINT origin = {};
    if (!ClientToScreen(GetXamlContentWindow(hostWindow), &origin)) {
        return false;
    }

    bounds->left = origin.x + elementBounds.X * scale;
    bounds->top = origin.y + elementBounds.Y * scale;
    bounds->right = bounds->left + elementBounds.Width * scale;
    bounds->bottom = bounds->top + elementBounds.Height * scale;
    return true;
}

winrt::Windows::UI::Xaml::FrameworkElement FindNamedDescendant(
    const winrt::Windows::UI::Xaml::DependencyObject& parent,
    std::wstring_view name,
    int depth) {
    using winrt::Windows::UI::Xaml::Media::VisualTreeHelper;

    int32_t childCount = VisualTreeHelper::GetChildrenCount(parent);
    for (int32_t index = 0; index < childCount; index++) {
        winrt::Windows::UI::Xaml::DependencyObject child =
            VisualTreeHelper::GetChild(parent, index);
        winrt::Windows::UI::Xaml::FrameworkElement element =
            child.try_as<winrt::Windows::UI::Xaml::FrameworkElement>();
        if (element && std::wstring_view(element.Name()) == name) {
            return element;
        }

        if (depth > 1) {
            winrt::Windows::UI::Xaml::FrameworkElement found =
                FindNamedDescendant(child, name, depth - 1);
            if (found) {
                return found;
            }
        }
    }

    return nullptr;
}

// FlyoutFrame fills its monitor-sized window. Its template positions the
// visible preview card, HoverFlyoutGrid, on HoverFlyoutCanvas.
winrt::Windows::UI::Xaml::FrameworkElement GetHoverAimPreviewCard() {
    HoverAimState& state = g_hoverAimState;
    winrt::Windows::UI::Xaml::FrameworkElement card =
        ResolveFrameworkElement(state.cardWeakReference);
    if (card) {
        return card;
    }

    winrt::Windows::UI::Xaml::FrameworkElement frame =
        ResolveFrameworkElement(state.frameWeakReference);
    if (!frame) {
        return nullptr;
    }

    card = FindNamedDescendant(frame, L"HoverFlyoutGrid",
                               HOVER_AIM_TEMPLATE_SEARCH_DEPTH);
    if (card) {
        ReleaseAbiReference(state.cardWeakReference);
        state.cardWeakReference = CreateWeakReference(winrt::get_abi(card));
        return card;
    }

    // Without the template part, the thumbnail list content is the closest
    // element to the previews. It can be replaced, so it isn't cached.
    winrt::Windows::UI::Xaml::Controls::ContentControl contentControl =
        frame.try_as<winrt::Windows::UI::Xaml::Controls::ContentControl>();
    if (!contentControl) {
        return nullptr;
    }

    return contentControl.Content()
        .try_as<winrt::Windows::UI::Xaml::FrameworkElement>();
}

bool GetHoverAimGeometry(HoverAimGeometry* geometry) {
    const HoverAimState& state = g_hoverAimState;
    HWND taskbarWindow = g_activeTaskbarWindow;
    if (!taskbarWindow || !state.flyoutWindow ||
        !state.frameWeakReference || !state.targetCount) {
        return false;
    }

    try {
        winrt::Windows::UI::Xaml::FrameworkElement card =
            GetHoverAimPreviewCard();
        HoverAimBounds flyoutBounds = {};
        if (!card ||
            !GetElementScreenBounds(card, state.flyoutWindow,
                                    &flyoutBounds)) {
            return false;
        }

        HoverAimBounds buttonBounds = {};
        bool buttonFound = false;
        for (size_t index = 0; index < state.targetCount; index++) {
            winrt::Windows::UI::Xaml::FrameworkElement target =
                ResolveFrameworkElement(state.targetWeakReferences[index]);
            HoverAimBounds targetBounds = {};
            if (!target ||
                !GetElementScreenBounds(target, taskbarWindow,
                                        &targetBounds)) {
                continue;
            }

            if (!buttonFound) {
                buttonBounds = targetBounds;
                buttonFound = true;
                continue;
            }

            buttonBounds.left = std::min(buttonBounds.left, targetBounds.left);
            buttonBounds.top = std::min(buttonBounds.top, targetBounds.top);
            buttonBounds.right =
                std::max(buttonBounds.right, targetBounds.right);
            buttonBounds.bottom =
                std::max(buttonBounds.bottom, targetBounds.bottom);
        }

        if (!buttonFound) {
            return false;
        }

        double anchorFraction =
            g_aimAnchorHeightPercent.load(std::memory_order_relaxed) / 100.0;
        geometry->buttonLeft = buttonBounds.left;
        geometry->buttonRight = buttonBounds.right;
        geometry->anchorY =
            buttonBounds.top +
            (buttonBounds.bottom - buttonBounds.top) * anchorFraction;
        geometry->flyoutLeft = flyoutBounds.left;
        geometry->flyoutRight = flyoutBounds.right;
        geometry->flyoutBottom = flyoutBounds.bottom;
        return true;
    } catch (...) {
        return false;
    }
}

// Only position changes are stored. While the pointer stays put, the newest
// sample's time keeps advancing, so it records when the pointer last stood
// there rather than when it arrived.
bool RecordHoverAimCursorSample(POINT* cursor, ULONGLONG* tick) {
    POINT point;
    if (!GetCursorPos(&point)) {
        return false;
    }

    HoverAimState& state = g_hoverAimState;
    ULONGLONG now = GetTickCount64();
    HoverAimCursorSample& newest =
        state.cursorSamples[(state.nextCursorSample +
                             HOVER_AIM_CURSOR_SAMPLE_COUNT - 1) %
                            HOVER_AIM_CURSOR_SAMPLE_COUNT];
    if (state.cursorSampleCount && newest.point.x == point.x &&
        newest.point.y == point.y) {
        newest.tick = now;
    } else {
        state.cursorSamples[state.nextCursorSample] = {point, now};
        state.nextCursorSample =
            (state.nextCursorSample + 1) % HOVER_AIM_CURSOR_SAMPLE_COUNT;
        if (state.cursorSampleCount < HOVER_AIM_CURSOR_SAMPLE_COUNT) {
            state.cursorSampleCount++;
        }
    }

    *cursor = point;
    *tick = now;
    return true;
}

// Motion is measured from the newest position at least the minimum motion
// distance away. If the pointer left it longer ago than that distance takes at
// the minimum drift speed, the pointer is at rest and has no direction.
bool GetRecentHoverAimMotion(POINT cursor,
                             ULONGLONG now,
                             double* motionX,
                             double* motionY) {
    const HoverAimState& state = g_hoverAimState;
    for (size_t age = 1; age < state.cursorSampleCount; age++) {
        const HoverAimCursorSample& sample =
            state.cursorSamples[(state.nextCursorSample +
                                 HOVER_AIM_CURSOR_SAMPLE_COUNT - 1 - age) %
                                HOVER_AIM_CURSOR_SAMPLE_COUNT];
        if (now - sample.tick > HOVER_AIM_MOTION_WINDOW_MS) {
            break;
        }

        LONG deltaX = cursor.x - sample.point.x;
        LONG deltaY = cursor.y - sample.point.y;
        if (deltaX * deltaX + deltaY * deltaY >=
            HOVER_AIM_MINIMUM_MOTION_PIXELS * HOVER_AIM_MINIMUM_MOTION_PIXELS) {
            *motionX = deltaX;
            *motionY = deltaY;
            return true;
        }
    }

    return false;
}

// Screen y grows downward. The cone opens upward from the button between the
// left boundary (button left edge to flyout bottom-left corner) and the right
// boundary (button right edge to flyout bottom-right corner).
bool IsMotionInsideHoverAimCone(double motionX,
                                double motionY,
                                const HoverAimGeometry& geometry) {
    double leftX = geometry.flyoutLeft - geometry.buttonLeft;
    double leftY = geometry.flyoutBottom - geometry.anchorY;
    double rightX = geometry.flyoutRight - geometry.buttonRight;
    double rightY = geometry.flyoutBottom - geometry.anchorY;
    if (leftY >= 0 || rightY >= 0) {
        return false;
    }

    return leftX * motionY - leftY * motionX >= 0 &&
           motionX * rightY - motionY * rightX >= 0;
}

bool IsAnyMouseButtonDown() {
    return ((GetAsyncKeyState(VK_LBUTTON) | GetAsyncKeyState(VK_RBUTTON) |
             GetAsyncKeyState(VK_MBUTTON)) &
            0x8000) != 0;
}

HoverAimResult EvaluateHoverAim(HoverAimEvaluation* evaluation) {
    ULONGLONG now;
    if (!RecordHoverAimCursorSample(&evaluation->cursor, &now)) {
        return HoverAimResult::unavailable;
    }

    evaluation->hasCursor = true;
    if (!g_initialized.load(std::memory_order_acquire) ||
        !g_aimTowardPreviews.load(std::memory_order_relaxed) ||
        !HasHoverAimSymbols() || !g_activeHoverFlyoutController) {
        return HoverAimResult::unavailable;
    }

    if (IsAnyMouseButtonDown()) {
        return HoverAimResult::mouseButtonDown;
    }

    if (!GetRecentHoverAimMotion(evaluation->cursor, now,
                                 &evaluation->motionX,
                                 &evaluation->motionY)) {
        return HoverAimResult::atRest;
    }

    evaluation->hasMotion = true;
    if (!GetHoverAimGeometry(&evaluation->geometry)) {
        return HoverAimResult::noGeometry;
    }

    evaluation->hasGeometry = true;
    if (evaluation->cursor.y <= evaluation->geometry.flyoutBottom) {
        return HoverAimResult::notBelowPreviews;
    }

    if (!IsMotionInsideHoverAimCone(evaluation->motionX,
                                    evaluation->motionY,
                                    evaluation->geometry)) {
        return HoverAimResult::outsideBoundaries;
    }

    return HoverAimResult::aiming;
}

void LogHoverAimEvaluation(const WCHAR* action,
                           const WCHAR* transitionName,
                           HoverAimResult result,
                           const HoverAimEvaluation& evaluation) {
    WCHAR cursor[48] = L"";
    WCHAR motion[48] = L"";
    WCHAR geometry[128] = L"";
    if (evaluation.hasCursor) {
        swprintf_s(cursor, ARRAYSIZE(cursor), L" cursor (%ld, %ld)",
                   evaluation.cursor.x, evaluation.cursor.y);
    }

    if (evaluation.hasMotion) {
        swprintf_s(motion, ARRAYSIZE(motion), L" motion (%.0f, %.0f)",
                   evaluation.motionX, evaluation.motionY);
    }

    if (evaluation.hasGeometry) {
        const HoverAimGeometry& bounds = evaluation.geometry;
        swprintf_s(geometry, ARRAYSIZE(geometry),
                   L" button %.0f-%.0f anchor %.0f previews %.0f-%.0f "
                   L"bottom %.0f",
                   bounds.buttonLeft, bounds.buttonRight, bounds.anchorY,
                   bounds.flyoutLeft, bounds.flyoutRight, bounds.flyoutBottom);
    }

    Wh_Log(L"%s %s (%s):%s%s%s", action, transitionName,
           DescribeHoverAimResult(result), cursor, motion, geometry);
}

void ReleaseDeferredHoverAimEvents(DeferredHoverAimEvent* events,
                                   size_t count) {
    for (size_t index = 0; index < count; index++) {
        if (events[index].itemKey) {
            winrt::hstring itemKey;
            winrt::attach_abi(itemKey, events[index].itemKey);
            events[index].itemKey = nullptr;
        }
    }
}

void DiscardDeferredHoverAimEvents() {
    HoverAimState& state = g_hoverAimState;
    ReleaseDeferredHoverAimEvents(state.deferredEvents,
                                  state.deferredEventCount);
    state.deferredEventCount = 0;
    state.deferredModel = nullptr;
}

bool IsSameItemKey(void* first, void* second) {
    winrt::hstring firstKey;
    winrt::hstring secondKey;
    winrt::copy_from_abi(firstKey, first);
    winrt::copy_from_abi(secondKey, second);
    return firstKey == secondKey;
}

// Per item, replay only its net effect: the first held change if it left an
// item the model already considered hovered, and the last held change if the
// pointer still rests on that item. Items merely crossed on the way vanish.
bool ShouldReplayDeferredVisualStateChange(
    const DeferredHoverAimEvent* events,
    size_t count,
    size_t index) {
    size_t firstIndex = index;
    size_t lastIndex = index;
    for (size_t other = 0; other < count; other++) {
        if (events[other].kind !=
                DeferredHoverAimEventKind::visualStateChange ||
            !IsSameItemKey(events[other].itemKey, events[index].itemKey)) {
            continue;
        }

        firstIndex = std::min(firstIndex, other);
        lastIndex = std::max(lastIndex, other);
    }

    INT32 currentState = events[index].currentState;
    return (index == firstIndex &&
            currentState == TASK_ITEM_VISUAL_STATE_NORMAL) ||
           (index == lastIndex &&
            currentState == TASK_ITEM_VISUAL_STATE_POINTER_OVER);
}

void FlushDeferredHoverAimEvents(const WCHAR* reason) {
    HoverAimState& state = g_hoverAimState;
    if (state.flushing || !state.deferredEventCount) {
        return;
    }

    // Detach first: replayed transitions may reach hooks that hold again
    void* model = state.deferredModel;
    size_t count = state.deferredEventCount;
    DeferredHoverAimEvent events[HOVER_AIM_DEFERRED_EVENT_CAPACITY];
    std::copy(state.deferredEvents, state.deferredEvents + count, events);
    state.deferredEventCount = 0;
    state.deferredModel = nullptr;

    // MSVC exceptions skip mod cleanup, so reset instead of restoring
    state.flushing = true;
    size_t replayedCount = 0;
    for (size_t index = 0; index < count; index++) {
        const DeferredHoverAimEvent& event = events[index];
        if (event.kind == DeferredHoverAimEventKind::dismiss) {
            TransitionToFlyoutDismissPendingState_Hook(model);
            replayedCount++;
            continue;
        }

        if (!ShouldReplayDeferredVisualStateChange(events, count, index)) {
            continue;
        }

        winrt::hstring itemKey;
        winrt::copy_from_abi(itemKey, event.itemKey);
        HoverFlyoutModelOnVisualStateStateChanged_Original(
            model, itemKey, event.hasHoverFlyout, event.previousState,
            event.currentState);
        replayedCount++;
    }

    state.flushing = false;
    ReleaseDeferredHoverAimEvents(events, count);
    Wh_Log(L"Applied %zu of %zu held hover changes (%s)", replayedCount,
           count, reason);
}

// Decides whether a pointer transition is held while the pointer aims at the
// flyout. Otherwise the transitions held so far happened first, so they are
// released before the caller lets this one through.
bool TryHoldHoverAimTransition(void* model, const WCHAR* transitionName) {
    HoverAimState& state = g_hoverAimState;
    if (state.flushing) {
        return false;
    }

    HoverAimEvaluation evaluation = {};
    HoverAimResult result = HoverAimResult::unavailable;
    if (HasHoverAimSymbols() &&
        (!state.deferredModel || state.deferredModel == model) &&
        HoverFlyoutModelIsFlyoutVisible_Original(model)) {
        result = state.deferredEventCount < HOVER_AIM_DEFERRED_EVENT_CAPACITY
                     ? EvaluateHoverAim(&evaluation)
                     : HoverAimResult::holdCapacityReached;
    }

    if (result == HoverAimResult::aiming) {
        if (!state.deferredEventCount) {
            LogHoverAimEvaluation(L"Holding hover changes from",
                                  transitionName, result, evaluation);
        }

        state.deferredModel = model;
        return true;
    }

    if (state.deferredEventCount) {
        FlushDeferredHoverAimEvents(DescribeHoverAimResult(result));
    } else if (result != HoverAimResult::unavailable) {
        LogHoverAimEvaluation(L"Not holding", transitionName, result,
                              evaluation);
    }

    return false;
}

bool TryHoldHoverAimDismiss(void* model) {
    if (!TryHoldHoverAimTransition(model, L"close")) {
        return false;
    }

    HoverAimState& state = g_hoverAimState;
    if (state.deferredEventCount &&
        state.deferredEvents[state.deferredEventCount - 1].kind ==
            DeferredHoverAimEventKind::dismiss) {
        return true;
    }

    state.deferredEvents[state.deferredEventCount++] = {
        DeferredHoverAimEventKind::dismiss,
        nullptr,
        false,
        0,
        0,
    };
    return true;
}

void WINAPI HoverFlyoutModelOnVisualStateStateChanged_Hook(
    void* object,
    const winrt::hstring& itemKey,
    bool hasHoverFlyout,
    INT32 previousState,
    INT32 currentState) {
    const WCHAR* transitionName = nullptr;
    if (currentState == TASK_ITEM_VISUAL_STATE_NORMAL) {
        transitionName = L"item exit";
    } else if (currentState == TASK_ITEM_VISUAL_STATE_POINTER_OVER) {
        transitionName = previousState == TASK_ITEM_VISUAL_STATE_NORMAL
                             ? L"item enter"
                             : L"item hover";
    }

    if (transitionName) {
        if (TryHoldHoverAimTransition(object, transitionName)) {
            winrt::hstring ownedItemKey = itemKey;
            HoverAimState& state = g_hoverAimState;
            state.deferredEvents[state.deferredEventCount++] = {
                DeferredHoverAimEventKind::visualStateChange,
                winrt::detach_abi(ownedItemKey),
                hasHoverFlyout,
                previousState,
                currentState,
            };
            return;
        }
    } else {
        // Presses aren't pointer travel; held changes happened before them
        FlushDeferredHoverAimEvents(L"item pressed");
    }

    HoverFlyoutModelOnVisualStateStateChanged_Original(
        object, itemKey, hasHoverFlyout, previousState, currentState);
}

void CALLBACK HoverAimTimerProc(HWND window,
                                UINT message,
                                UINT_PTR timer,
                                DWORD time) {
    // Samples the cursor outside the taskbar too, where no pointer events
    // reach this thread, and releases held changes once aiming ends.
    if (!g_hoverAimState.deferredEventCount) {
        POINT cursor;
        ULONGLONG tick;
        RecordHoverAimCursorSample(&cursor, &tick);
        return;
    }

    HoverAimEvaluation evaluation = {};
    HoverAimResult result = EvaluateHoverAim(&evaluation);
    if (result != HoverAimResult::aiming) {
        FlushDeferredHoverAimEvents(DescribeHoverAimResult(result));
    }
}

void StartHoverAimTracking() {
    HoverAimState& state = g_hoverAimState;
    HWND taskbarWindow = g_activeTaskbarWindow;
    if (state.timerWindow || !taskbarWindow ||
        !g_aimTowardPreviews.load(std::memory_order_relaxed) ||
        !HasHoverAimSymbols()) {
        return;
    }

    // A window timer with a callback is dispatched without reaching the
    // taskbar's window procedure.
    state.cursorSampleCount = 0;
    state.nextCursorSample = 0;
    if (!SetTimer(taskbarWindow, HOVER_AIM_TIMER_ID,
                  HOVER_AIM_SAMPLE_INTERVAL_MS, HoverAimTimerProc)) {
        Wh_Log(L"Failed to start pointer-aim tracking: %lu", GetLastError());
        return;
    }

    state.timerWindow = taskbarWindow;
}

void StopHoverAimTracking() {
    HoverAimState& state = g_hoverAimState;
    if (state.timerWindow) {
        KillTimer(state.timerWindow, HOVER_AIM_TIMER_ID);
        state.timerWindow = nullptr;
    }

    state.flyoutWindow = nullptr;
}

// Each feature requires every optional symbol it touches to have resolved
// That includes the destructor hooks that clear the pointers it tracks
bool HasTaskbarLightDismissSymbols() {
    return TaskbarHostRegisterLightDismiss_Original &&
           TaskbarHostUnregisterLightDismiss_Original &&
           LightDismissActionDismissed_Original &&
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
           HoverFlyoutModelOnPointerEnteredFlyoutFrame_Original &&
           HoverFlyoutModelOnPointerExitedFlyoutFrame_Original &&
           HoverFlyoutModelDestructor_Original;
}

bool HasStartButtonHoverSymbols() {
    return HasActiveHoverFlyoutLifecycleSymbols() &&
           ExperienceToggleButtonOnPointerEntered_Original;
}

bool HasThumbnailRemovalSymbols() {
    return TransitionToFlyoutVisiblePendingState_Original &&
           TransitionToFlyoutDismissPendingState_Original &&
           MouseHoverTime_Original &&
           HoverFlyoutModelOnPointerEnteredItemWithHoverFlyout_Original &&
           HoverUIItemsCollectionSetTargetItem_Original &&
           HoverUIItemsCollectionDestructor_Original &&
           ThumbnailSourceArraySize_Original &&
           HoverUIItemsCollectionOnSourceArrayChanged_Original &&
           HoverFlyoutModelDestructor_Original;
}

bool HasOutsideClickSymbols() {
    return HasTaskbarLightDismissSymbols() &&
           HasFlyoutPointerTrackingSymbols() &&
           TaskbarModelGetHostWindowID_Original &&
           TaskbarControllerOnLightDismissTriggered_Original;
}

bool HasHoverAimSymbols() {
    return HasActiveHoverFlyoutLifecycleSymbols() &&
           HoverFlyoutModelDestructor_Original &&
           HoverFlyoutModelOnPointerEnteredFlyoutFrame_Original &&
           TaskbarModelGetHostWindowID_Original &&
           TaskbarFlyoutGetWindowID_Original &&
           TransitionToFlyoutDismissPendingState_Original &&
           HoverFlyoutModelOnVisualStateStateChanged_Original &&
           HoverFlyoutModelIsFlyoutVisible_Original &&
           FlyoutFrameTargets_Original;
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

    g_hoverDelayMilliseconds.store(
        static_cast<UINT>(delayMilliseconds),
        std::memory_order_relaxed);
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

    int aimAnchorHeightPercent = Wh_GetIntSetting(L"aimAnchorHeightPercent");
    aimAnchorHeightPercent = std::clamp(aimAnchorHeightPercent, 0, 100);
    g_aimTowardPreviews.store(
        Wh_GetIntSetting(L"aimTowardPreviews") != 0,
        std::memory_order_relaxed);
    g_aimAnchorHeightPercent.store(aimAnchorHeightPercent,
                                   std::memory_order_relaxed);

    Wh_Log(L"Settings: delayMs=%d closeDelayMs=%d "
           L"thumbnailRemovalDelayMs=%d "
           L"closeOnOutsideClick=%d closeOnStartButtonHover=%d "
           L"aimTowardPreviews=%d aimAnchorHeightPercent=%d",
           delayMilliseconds,
           closeDelayMilliseconds,
           thumbnailRemovalDelayMilliseconds,
           g_closeOnOutsideClick.load(std::memory_order_relaxed),
           g_closeOnStartButtonHover.load(std::memory_order_relaxed),
           g_aimTowardPreviews.load(std::memory_order_relaxed),
           aimAnchorHeightPercent);
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

bool ResolveTaskbarSymbols(HMODULE module) {
    if (!module || g_taskbarModuleHandled.exchange(true)) {
        return false;
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
        {
            {
                LR"(public: __cdecl winrt::impl::consume_Windows_UI_Internal_Input_ILightDismissAction<struct winrt::Windows::UI::Internal::Input::ILightDismissAction>::Dismissed(struct winrt::Windows::Foundation::TypedEventHandler<struct winrt::Windows::UI::Internal::Input::LightDismissAction,struct winrt::Windows::UI::Internal::Input::LightDismissEventArgs> const &)const )",
                LR"(public: struct winrt::event_token __cdecl winrt::impl::consume_Windows_UI_Internal_Input_ILightDismissAction<struct winrt::Windows::UI::Internal::Input::ILightDismissAction>::Dismissed(struct winrt::Windows::Foundation::TypedEventHandler<struct winrt::Windows::UI::Internal::Input::LightDismissAction,struct winrt::Windows::UI::Internal::Input::LightDismissEventArgs> const &)const )",
            },
            &LightDismissActionDismissed_Original,
            LightDismissActionDismissed_Hook,
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
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskbarModel,struct winrt::WindowsUdk::UI::Shell::ITaskbarModel>::get_HostWindowId(unsigned __int64 *))",
            },
            &TaskbarModelGetHostWindowID_Original,
            TaskbarModelGetHostWindowID_Hook,
            true,
        },
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskbarFlyout,struct winrt::WindowsUdk::UI::Shell::ITaskbarFlyout>::get_WindowId(unsigned __int64 *))",
            },
            &TaskbarFlyoutGetWindowID_Original,
            TaskbarFlyoutGetWindowID_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                    ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"Failed to resolve Taskbar.dll symbols");
        g_taskbarModuleHandled.store(false);
        return false;
    }

    if (!HasTaskbarLightDismissSymbols()) {
        Wh_Log(L"Taskbar light-dismiss support isn't available on this build");
    }

    return true;
}

bool ResolveTaskbarViewSymbols(HMODULE module) {
    if (!module || g_taskbarViewModuleHandled.exchange(true)) {
        return false;
    }

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
                LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::OnPointerEnteredItemWithHoverFlyout(struct winrt::hstring const &))",
            },
            &HoverFlyoutModelOnPointerEnteredItemWithHoverFlyout_Original,
            HoverFlyoutModelOnPointerEnteredItemWithHoverFlyout_Hook,
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
                LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::OnPointerEnteredFlyoutFrame(void))",
            },
            &HoverFlyoutModelOnPointerEnteredFlyoutFrame_Original,
            HoverFlyoutModelOnPointerEnteredFlyoutFrame_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::OnPointerExitedFlyoutFrame(void))",
            },
            &HoverFlyoutModelOnPointerExitedFlyoutFrame_Original,
            HoverFlyoutModelOnPointerExitedFlyoutFrame_Hook,
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
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::OnVisualStateStateChanged(struct winrt::hstring const &,bool,enum winrt::WindowsUdk::UI::Shell::TaskItemVisualState,enum winrt::WindowsUdk::UI::Shell::TaskItemVisualState))",
            },
            &HoverFlyoutModelOnVisualStateStateChanged_Original,
            HoverFlyoutModelOnVisualStateStateChanged_Hook,
            true,
        },
        {
            {
                LR"(public: bool __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::IsFlyoutVisible(void)const )",
            },
            &HoverFlyoutModelIsFlyoutVisible_Original,
            nullptr,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::FlyoutFrame::Targets(class std::vector<struct winrt::weak_ref<struct winrt::Windows::UI::Xaml::FrameworkElement>,class std::allocator<struct winrt::weak_ref<struct winrt::Windows::UI::Xaml::FrameworkElement> > >))",
            },
            &FlyoutFrameTargets_Original,
            FlyoutFrameTargets_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(
            module,
            taskbarViewHooks,
            ARRAYSIZE(taskbarViewHooks))) {
        Wh_Log(L"Failed to resolve Taskbar.View symbols");
        g_taskbarViewModuleHandled.store(false);
        return false;
    }

    if (!TransitionToFlyoutDismissPendingState_Original ||
        !MouseHoverTime_Original) {
        Wh_Log(L"Thumbnail close-delay symbols aren't available; the "
               L"close-delay override is disabled on this build");
    }

    if (!HoverFlyoutModelOnPointerEnteredItemWithHoverFlyout_Original ||
        !MouseHoverTime_Original) {
        Wh_Log(L"Item-hover delay symbols aren't available; the hover-delay "
               L"override misses inlined first-hover transitions on this "
               L"build");
    }

    if (!HasThumbnailRemovalSymbols()) {
        Wh_Log(L"Thumbnail-removal delay isn't available on this build");
    }

    // Taskbar.dll's TaskbarHost loads this module, so Taskbar.dll is handled.
    if (g_closeOnOutsideClick.load(std::memory_order_relaxed) &&
        !HasOutsideClickSymbols()) {
        Wh_Log(L"Thumbnail light-dismiss symbols aren't available; "
               L"close-on-outside-click is disabled on this build");
    }

    if (g_closeOnStartButtonHover.load(std::memory_order_relaxed) &&
        !HasStartButtonHoverSymbols()) {
        Wh_Log(L"Start-button hover symbols aren't available; "
               L"close-on-Start-hover is disabled on this build");
    }

    if (g_aimTowardPreviews.load(std::memory_order_relaxed) &&
        !HasHoverAimSymbols()) {
        Wh_Log(L"Pointer-aim symbols aren't available; keeping previews "
               L"while moving toward them is disabled on this build");
    }

    WCHAR modulePath[MAX_PATH] = {};
    if (!GetModuleFileNameW(module, modulePath, ARRAYSIZE(modulePath))) {
        wcscpy_s(modulePath, L"(unknown module)");
    }

    Wh_Log(L"Taskbar.View hooks queued for %s", modulePath);
    return true;
}

bool ResolveLoadedTaskbarSymbols() {
    bool hooksQueued = false;

    HMODULE taskbarModule = GetTaskbarModuleHandle();
    if (taskbarModule) {
        hooksQueued = ResolveTaskbarSymbols(taskbarModule) || hooksQueued;
    }

    HMODULE taskbarViewModule = GetTaskbarViewModuleHandle();
    if (taskbarViewModule) {
        hooksQueued =
            ResolveTaskbarViewSymbols(taskbarViewModule) || hooksQueued;
    }

    return hooksQueued;
}

void ApplyQueuedTaskbarHooks() {
    if (!Wh_ApplyHookOperations()) {
        Wh_Log(L"Failed to apply late taskbar hooks");
    }
}

void HandleLoadedModule(HMODULE module) {
    if (!module ||
        !g_initialized.load(std::memory_order_acquire)) {
        return;
    }

    bool hooksQueued = false;
    if (!g_taskbarModuleHandled.load() &&
        GetTaskbarModuleHandle() == module) {
        hooksQueued = ResolveTaskbarSymbols(module);
    }

    if (!g_taskbarViewModuleHandled.load() &&
        GetTaskbarViewModuleHandle() == module) {
        hooksQueued = ResolveTaskbarViewSymbols(module) || hooksQueued;
    }

    if (hooksQueued) {
        ApplyQueuedTaskbarHooks();
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName,
    HANDLE file,
    DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    if (!(flags & NONEXECUTABLE_LIBRARY_LOAD_FLAGS)) {
        HandleLoadedModule(module);
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
                    if (!parameters->executed) {
                        parameters->executed = true;
                        try {
                            parameters->procedure();
                        } catch (...) {
                            parameters->executed = false;
                            Wh_Log(L"Taskbar thread operation failed");
                        }
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
    // Hand held hover changes back to the model before the hooks go away
    FlushDeferredHoverAimEvents(L"mod unloading");
    DiscardDeferredHoverAimEvents();
    StopHoverAimTracking();
    ReleaseHoverAimTargets();
    g_thumbnailRemovalTrackingState = {};
    g_activeHoverFlyoutController = nullptr;
    g_activeTaskbarWindow = nullptr;
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
        Wh_Log(L"The Windows 11 24H2 taskbar isn't available on this build");
        return FALSE;
    }

    if (!HookLoadLibraryExW()) {
        return FALSE;
    }

    // Taskbar.dll only backs the optional outside-click feature
    // A failure is logged and retried later instead of failing initialization
    HMODULE taskbarModule = GetTaskbarModuleHandle();
    if (taskbarModule) {
        ResolveTaskbarSymbols(taskbarModule);
    }

    HMODULE taskbarViewModule = GetTaskbarViewModuleHandle();
    if (taskbarViewModule &&
        !ResolveTaskbarViewSymbols(taskbarViewModule)) {
        return FALSE;
    }

    g_initialized.store(true, std::memory_order_release);
    Wh_Log(L"Initialized");
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_initialized.load(std::memory_order_acquire)) {
        return;
    }

    // Retry in case a module loaded after the initial scan but before the
    // LoadLibraryExW hook became active.
    if (ResolveLoadedTaskbarSymbols()) {
        ApplyQueuedTaskbarHooks();
    }

    if (!g_taskbarViewModuleHandled.load()) {
        Wh_Log(L"Taskbar.View isn't loaded yet");
    }
}

void Wh_ModSettingsChanged() {
    bool previousCloseOnOutsideClick =
        g_closeOnOutsideClick.load(std::memory_order_relaxed);
    LoadSettings();

    if (g_initialized.load(std::memory_order_acquire) &&
        ResolveLoadedTaskbarSymbols()) {
        ApplyQueuedTaskbarHooks();
    }

    if (previousCloseOnOutsideClick !=
        g_closeOnOutsideClick.load(std::memory_order_relaxed)) {
        RunOnTaskbarThreads(RefreshOutsideClickTaskbarThreadState);
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
