// ==WindhawkMod==
// @id              taskbar-labels-multi-window-only
// @name            Dynamic Taskbar Labels
// @description     Show labels for multi-window taskbar groups on the current virtual desktop
// @version         1.0.0
// @author          amaztony
// @github          https://github.com/amaztony
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// @license         GPL-3.0-only
// ==/WindhawkMod==

// SPDX-License-Identifier: GPL-3.0-only
//
// Derived in part from Taskbar Labels for Windows 11 by m417z:
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-labels.wh.cpp
//
// The XAML Diagnostics integration is based on the demo by m417z:
// https://gist.github.com/m417z/8741e52d8eaad67b47ee365a20070bf8

// ==WindhawkModReadme==
/*
# Dynamic Taskbar Labels

Keeps taskbar buttons uncombined and changes only label visibility:

- A taskbar group with one window on the current virtual desktop shows only
  its icon.
- A taskbar group with two or more windows on the current virtual desktop
  shows each window title.
- Existing buttons update when the group changes and when the Mod is enabled or
  disabled, and after switching virtual desktops.

Grouping follows Windows' internal taskbar `TaskGroup` state. The Mod doesn't
enumerate windows or group by process, path, AppUserModelID, or HWND.

Designed for the Windows 11 XAML taskbar. If required internal symbols aren't
available, unsupported paths keep the Windows-provided label state.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <ocidl.h>
#include <xamlom.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#define WH_WINRT_WINUI2
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#undef WH_WINRT_WINUI2
#include <winrt/base.h>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <optional>
#include <utility>
#include <vector>

namespace Xaml = winrt::Windows::UI::Xaml;
namespace Controls = winrt::Windows::UI::Xaml::Controls;
namespace Core = winrt::Windows::UI::Core;
namespace Muxc = winrt::Microsoft::UI::Xaml::Controls;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_unloading;

// Binding updates and HasLabel evaluation are synchronous. Thread-local state
// associates the button currently being bound with the TaskGroup captured from
// its ViewModel, without caching native TaskGroup pointers beyond that call.
thread_local int g_hasLabelEvaluationDepth;
thread_local uintptr_t g_bindingViewModelIdentity;
thread_local uintptr_t g_bindingTaskGroupIdentity;
thread_local bool g_refreshingTaskListButtons;
thread_local bool g_synchronizingExistingTaskListButton;
thread_local bool g_refreshingVirtualDesktopButtons;

using ITaskbarAppItemViewModel_HasLabels_t = bool(WINAPI*)(void*);
ITaskbarAppItemViewModel_HasLabels_t
    ITaskbarAppItemViewModel_HasLabels_Original;

using HasLabel_t = HRESULT(WINAPI*)(void*, bool*);
HasLabel_t
    TaskListWindowViewModel_ITaskbarAppItemViewModel_get_HasLabel_Original;

using TaskListButtonBindings_Update_t =
    // ITaskbarAppItemViewModel is a non-trivial C++/WinRT value type.
    // MSVC passes it indirectly; the second argument points to the wrapper.
    void(WINAPI*)(void*, void*, int);
TaskListButtonBindings_Update_t
    TaskListButtonBindings_obj3_Update_Original;
TaskListButtonBindings_Update_t
    TaskListButtonBindings_obj6_Update_Original;
TaskListButtonBindings_Update_t
    TaskListButtonBindings_obj9_Update_Original;
TaskListButtonBindings_Update_t
    TaskListButtonBindings_obj12_Update_Original;
TaskListButtonBindings_Update_t
    TaskListButtonBindings_obj15_Update_Original;
TaskListButtonBindings_Update_t
    TaskListButtonBindings_obj18_Update_Original;
TaskListButtonBindings_Update_t
    TaskListButtonBindings_obj21_Update_Original;
TaskListButtonBindings_Update_t
    TaskListButtonBindings_obj24_Update_Original;

using TaskListButton_put_HasLabel_t =
    HRESULT(WINAPI*)(void*, bool);
TaskListButton_put_HasLabel_t
    TaskListButton_put_HasLabel_Original;

using TaskListButton_UpdateVisualStates_t =
    void(WINAPI*)(void*);
TaskListButton_UpdateVisualStates_t
    TaskListButton_UpdateVisualStates_Original;

void* ITaskListWindowViewModel_vftable;
void* TaskListWindowViewModel_ITaskbarAppItemViewModel_vftable;
using GetTaskItem_t = HRESULT(WINAPI*)(void*, void**);
GetTaskItem_t ITaskListWindowViewModel_get_TaskItem;

using ReportClicked_t = int(WINAPI*)(void*, void*);
ReportClicked_t TaskItem_ReportClicked_Original;

// LauncherOptions is passed as a C++ const reference: one pointer on x64.
using HandleClick_t = HRESULT(WINAPI*)(void*, void*, void*, void*);
HandleClick_t CTaskListWnd_HandleClick_Original;

using EnumTaskItems_t = HRESULT(WINAPI*)(void*, void**);
EnumTaskItems_t CTaskGroup_EnumTaskItems_Original;

using EnumTaskItemsNext_t = HRESULT(WINAPI*)(void*, void**);
EnumTaskItemsNext_t CEnumTaskItems_Next_Original;

using IsVisibleOnCurrentVirtualDesktop_t = bool(WINAPI*)(void*);
IsVisibleOnCurrentVirtualDesktop_t
    CTaskItem_IsVisibleOnCurrentVirtualDesktop_Original;
IsVisibleOnCurrentVirtualDesktop_t
    CTaskGroupTaskItem_IsVisibleOnCurrentVirtualDesktop_Original;

using GroupChanged_t = void(WINAPI*)(void*, void*, int);
GroupChanged_t CTaskListWnd_GroupChanged_Original;

using UpdateVirtualDesktopInclusion_t = HRESULT(WINAPI*)(void*);
UpdateVirtualDesktopInclusion_t
    CTaskBand_UpdateVirtualDesktopInclusion_Original;

using RegGetValueW_t = decltype(&RegGetValueW);
RegGetValueW_t RegGetValueW_Original;

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

WCHAR g_taskGroupSentinel[] =
    L"taskbar-labels-multi-window-sentinel";
thread_local bool g_captureTaskGroup;
thread_local void* g_capturedTaskGroup;

constexpr GUID kIUnknownGuid{
    0x00000000,
    0x0000,
    0x0000,
    {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46},
};

struct TrackedTaskListButton {
    // The weak COM identity keeps no taskbar object alive. interfaceOffset
    // reconstructs the original TaskListButton interface for a live identity.
    winrt::weak_ref<winrt::Windows::Foundation::IUnknown> identity;
    uintptr_t identityValue;
    intptr_t interfaceOffset;
    uintptr_t taskGroupIdentity;
};

std::mutex g_trackedTaskListButtonsMutex;
std::vector<TrackedTaskListButton> g_trackedTaskListButtons;

struct TrackedXamlTaskListButton {
    Xaml::FrameworkElement element{nullptr};
    Core::CoreDispatcher dispatcher{nullptr};
    InstanceHandle handle;
    uintptr_t identityValue;
};

std::mutex g_trackedXamlTaskListButtonsMutex;
std::vector<TrackedXamlTaskListButton>
    g_trackedXamlTaskListButtons;
std::atomic<bool> g_xamlDiagnosticsInitialized;

uintptr_t GetComIdentityValue(void* interfacePointer);

class ScopedBindingContext {
   public:
    explicit ScopedBindingContext(void* viewModel)
        : m_previousViewModelIdentity(g_bindingViewModelIdentity),
          m_previousTaskGroupIdentity(g_bindingTaskGroupIdentity) {
        g_bindingViewModelIdentity =
            GetComIdentityValue(viewModel);
        g_bindingTaskGroupIdentity = 0;
    }

    ~ScopedBindingContext() {
        g_bindingViewModelIdentity =
            m_previousViewModelIdentity;
        g_bindingTaskGroupIdentity =
            m_previousTaskGroupIdentity;
    }

   private:
    uintptr_t m_previousViewModelIdentity;
    uintptr_t m_previousTaskGroupIdentity;
};

bool WINAPI ITaskbarAppItemViewModel_HasLabels_Hook(void* pThis) {
    ++g_hasLabelEvaluationDepth;
    bool result = ITaskbarAppItemViewModel_HasLabels_Original(pThis);
    --g_hasLabelEvaluationDepth;
    return result;
}

HRESULT WINAPI CTaskListWnd_HandleClick_Hook(void* pThis,
                                             void* taskGroup,
                                             void* taskItem,
                                             void* launcherOptions) {
    if (g_captureTaskGroup && launcherOptions &&
        (launcherOptions == &g_taskGroupSentinel ||
         *reinterpret_cast<void**>(launcherOptions) ==
             &g_taskGroupSentinel)) {
        g_capturedTaskGroup = taskGroup;
        return S_OK;
    }

    return CTaskListWnd_HandleClick_Original(
        pThis, taskGroup, taskItem, launcherOptions);
}

void* FindTaskListWindowViewModel(void* pThis) {
    constexpr int kMaximumVtableSearch = 16;
    for (int i = 0; i < kMaximumVtableSearch; ++i) {
        if (*reinterpret_cast<void**>(pThis) ==
            ITaskListWindowViewModel_vftable) {
            return pThis;
        }

        pThis = reinterpret_cast<void**>(pThis) - 1;
    }

    return nullptr;
}

void* FindTaskListWindowAppItemViewModel(
    void* interfacePointer) {
    if (!interfacePointer ||
        !TaskListWindowViewModel_ITaskbarAppItemViewModel_vftable) {
        return nullptr;
    }

    IUnknown* identity = nullptr;
    HRESULT result =
        reinterpret_cast<IUnknown*>(interfacePointer)->QueryInterface(
            kIUnknownGuid,
            reinterpret_cast<void**>(&identity));
    if (FAILED(result) || !identity)
        return nullptr;

    constexpr int kMaximumInterfaceSearch = 16;
    void* appItemViewModel = nullptr;
    for (int i = 0; i < kMaximumInterfaceSearch; ++i) {
        void* candidate =
            reinterpret_cast<void**>(identity) + i;
        if (*reinterpret_cast<void**>(candidate) ==
            TaskListWindowViewModel_ITaskbarAppItemViewModel_vftable) {
            appItemViewModel = candidate;
            break;
        }
    }

    identity->Release();
    return appItemViewModel;
}

uintptr_t GetComIdentityValue(void* interfacePointer) {
    if (!interfacePointer)
        return 0;

    IUnknown* identity = nullptr;
    HRESULT result =
        reinterpret_cast<IUnknown*>(interfacePointer)->QueryInterface(
            kIUnknownGuid,
            reinterpret_cast<void**>(&identity));
    if (FAILED(result) || !identity)
        return 0;

    uintptr_t value = reinterpret_cast<uintptr_t>(identity);
    identity->Release();
    return value;
}

void TrackTaskListButton(void* buttonInterface) {
    if (!buttonInterface || !g_bindingTaskGroupIdentity)
        return;

    IUnknown* identityRaw = nullptr;
    HRESULT result =
        reinterpret_cast<IUnknown*>(buttonInterface)->QueryInterface(
            kIUnknownGuid,
            reinterpret_cast<void**>(&identityRaw));
    if (FAILED(result) || !identityRaw) {
        Wh_Log(L"Button identity lookup failed: 0x%08X",
               result);
        return;
    }

    try {
        winrt::Windows::Foundation::IUnknown identity{
            identityRaw, winrt::take_ownership_from_abi};
        auto weakIdentity = winrt::make_weak(identity);
        uintptr_t identityValue =
            reinterpret_cast<uintptr_t>(identityRaw);
        intptr_t interfaceOffset =
            reinterpret_cast<BYTE*>(buttonInterface) -
            reinterpret_cast<BYTE*>(identityRaw);

        std::lock_guard lock(g_trackedTaskListButtonsMutex);
        for (auto& tracked : g_trackedTaskListButtons) {
            if (tracked.identityValue == identityValue) {
                tracked.identity = std::move(weakIdentity);
                tracked.interfaceOffset = interfaceOffset;
                tracked.taskGroupIdentity =
                    g_bindingTaskGroupIdentity;
                return;
            }
        }

        g_trackedTaskListButtons.push_back({
            .identity = std::move(weakIdentity),
            .identityValue = identityValue,
            .interfaceOffset = interfaceOffset,
            .taskGroupIdentity = g_bindingTaskGroupIdentity,
        });
    } catch (winrt::hresult_error const& e) {
        // identity owns identityRaw after construction.
        Wh_Log(L"Button weak reference failed: 0x%08X",
               e.code().value);
    }
}

template <typename Function>
Function FindResolvedVirtualMethod(void* object,
                                   Function resolvedMethod,
                                   size_t maximumVtableSearch) {
    if (!object || !resolvedMethod)
        return nullptr;

    void** vtable = *reinterpret_cast<void***>(object);
    if (!vtable)
        return nullptr;

    void* resolvedAddress =
        reinterpret_cast<void*>(resolvedMethod);
    for (size_t i = 0; i < maximumVtableSearch; ++i) {
        if (vtable[i] == resolvedAddress) {
            return reinterpret_cast<Function>(vtable[i]);
        }
    }

    return nullptr;
}

std::optional<bool> IsTaskItemVisibleOnCurrentVirtualDesktop(
    void* taskItem) {
    // ITaskItem is internal and its ABI changes between Windows releases.
    // Locate a symbol-resolved implementation in the live vtable instead of
    // relying on a hard-coded slot.
    constexpr size_t kMaximumVtableSearch = 96;
    auto isVisible = FindResolvedVirtualMethod(
        taskItem,
        CTaskItem_IsVisibleOnCurrentVirtualDesktop_Original,
        kMaximumVtableSearch);
    if (!isVisible) {
        isVisible = FindResolvedVirtualMethod(
            taskItem,
            CTaskGroupTaskItem_IsVisibleOnCurrentVirtualDesktop_Original,
            kMaximumVtableSearch);
    }

    return isVisible
               ? std::optional<bool>(isVisible(taskItem))
               : std::nullopt;
}

std::optional<int> GetCurrentDesktopTaskGroupItemCount(
    void* taskGroup) {
    if (!taskGroup || !CTaskGroup_EnumTaskItems_Original ||
        !CEnumTaskItems_Next_Original ||
        (!CTaskItem_IsVisibleOnCurrentVirtualDesktop_Original &&
         !CTaskGroupTaskItem_IsVisibleOnCurrentVirtualDesktop_Original)) {
        return std::nullopt;
    }

    auto enumTaskItems = FindResolvedVirtualMethod(
        taskGroup, CTaskGroup_EnumTaskItems_Original, 16);
    if (!enumTaskItems)
        return std::nullopt;

    IUnknown* taskItems = nullptr;
    HRESULT result = enumTaskItems(
        taskGroup, reinterpret_cast<void**>(&taskItems));
    if (FAILED(result) || !taskItems)
        return std::nullopt;

    auto next = FindResolvedVirtualMethod(
        taskItems, CEnumTaskItems_Next_Original, 16);
    if (!next) {
        taskItems->Release();
        return std::nullopt;
    }

    int visibleItemCount = 0;
    while (true) {
        IUnknown* taskItem = nullptr;
        result = next(
            taskItems, reinterpret_cast<void**>(&taskItem));
        if (result == S_FALSE) {
            if (taskItem)
                taskItem->Release();
            break;
        }
        if (FAILED(result) || !taskItem) {
            visibleItemCount = -1;
            break;
        }

        auto isVisible =
            IsTaskItemVisibleOnCurrentVirtualDesktop(taskItem);
        taskItem->Release();
        if (!isVisible.has_value()) {
            visibleItemCount = -1;
            break;
        }

        if (*isVisible && ++visibleItemCount > 1) {
            // Only the single-window/multi-window boundary matters.
            break;
        }
    }

    taskItems->Release();
    if (visibleItemCount < 0)
        return std::nullopt;

    return visibleItemCount;
}

void RefreshTrackedTaskListButtonsForGroup(void* taskGroup,
                                           int itemCount) {
    if (!taskGroup || !TaskListButton_put_HasLabel_Original)
        return;

    struct Target {
        winrt::weak_ref<
            winrt::Windows::Foundation::IUnknown> identity;
        intptr_t interfaceOffset;
    };

    std::vector<Target> targets;
    uintptr_t taskGroupIdentity =
        reinterpret_cast<uintptr_t>(taskGroup);
    {
        std::lock_guard lock(g_trackedTaskListButtonsMutex);
        for (auto const& tracked : g_trackedTaskListButtons) {
            if (tracked.taskGroupIdentity == taskGroupIdentity) {
                targets.push_back({
                    .identity = tracked.identity,
                    .interfaceOffset = tracked.interfaceOffset,
                });
            }
        }
    }

    bool hasLabel = itemCount > 1;
    g_refreshingTaskListButtons = true;
    for (auto const& target : targets) {
        try {
            auto identity = target.identity.get();
            if (!identity)
                continue;

            void* buttonInterface =
                reinterpret_cast<BYTE*>(winrt::get_abi(identity)) +
                target.interfaceOffset;
            HRESULT result =
                TaskListButton_put_HasLabel_Original(
                    buttonInterface, hasLabel);
            if (FAILED(result)) {
                Wh_Log(
                    L"Button label update failed: 0x%08X",
                    result);
            }
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"Button refresh failed: 0x%08X",
                   e.code().value);
        }
    }
    g_refreshingTaskListButtons = false;
}

std::optional<bool> IsWindowViewModelMultiWindow(void* pThis) {
    if (!ITaskListWindowViewModel_vftable ||
        !ITaskListWindowViewModel_get_TaskItem ||
        !TaskItem_ReportClicked_Original ||
        !CTaskListWnd_HandleClick_Original) {
        return std::nullopt;
    }

    void* viewModel = FindTaskListWindowViewModel(pThis);
    if (!viewModel)
        return std::nullopt;

    IUnknown* taskItem = nullptr;
    HRESULT result = ITaskListWindowViewModel_get_TaskItem(
        viewModel, reinterpret_cast<void**>(&taskItem));
    if (FAILED(result) || !taskItem)
        return std::nullopt;

    g_capturedTaskGroup = nullptr;
    g_captureTaskGroup = true;
    // ReportClicked forwards the native TaskGroup to HandleClick. The sentinel
    // makes our HandleClick hook capture it and return before any click occurs.
    TaskItem_ReportClicked_Original(
        taskItem, &g_taskGroupSentinel);
    g_captureTaskGroup = false;
    taskItem->Release();

    void* taskGroup = g_capturedTaskGroup;
    g_capturedTaskGroup = nullptr;
    if (!taskGroup)
        return std::nullopt;

    auto itemCount =
        GetCurrentDesktopTaskGroupItemCount(taskGroup);
    if (!itemCount)
        return std::nullopt;

    if (g_bindingViewModelIdentity &&
        GetComIdentityValue(viewModel) ==
            g_bindingViewModelIdentity) {
        g_bindingTaskGroupIdentity =
            reinterpret_cast<uintptr_t>(taskGroup);
    }
    return *itemCount > 1;
}

Xaml::FrameworkElement GetTaskListButtonElement(
    void* implementationThis) {
    // This is the TaskListButton XAML interface location used by the
    // upstream Taskbar Labels mod from UpdateVisualStates.
    void* taskListButtonIUnknown =
        reinterpret_cast<void**>(implementationThis) + 3;
    winrt::Windows::Foundation::IUnknown unknown;
    winrt::copy_from_abi(unknown, taskListButtonIUnknown);
    return unknown.try_as<Xaml::FrameworkElement>();
}

bool IsTaskbarAppItemViewModel(
    winrt::Windows::Foundation::IInspectable const& candidate) {
    if (!candidate)
        return false;

    auto runtimeClassName = winrt::get_class_name(candidate);
    return runtimeClassName ==
               L"Taskbar.TaskListWindowViewModel" ||
           runtimeClassName ==
               L"Taskbar.TaskListGroupViewModel";
}

winrt::Windows::Foundation::IInspectable
FindTaskbarAppItemViewModel(
    Xaml::FrameworkElement const& taskListButton) {
    constexpr int kMaximumAncestorSearch = 12;
    Xaml::FrameworkElement current = taskListButton;

    for (int i = 0;
         i < kMaximumAncestorSearch && current;
         ++i) {
        if (auto repeater =
                current.try_as<Muxc::ItemsRepeater>()) {
            int index =
                repeater.GetElementIndex(taskListButton);
            if (index >= 0) {
                auto itemsSourceView =
                    repeater.ItemsSourceView();
                if (itemsSourceView) {
                    auto item = itemsSourceView.GetAt(index);
                    if (IsTaskbarAppItemViewModel(item)) {
                        return item;
                    }
                }
            }
        }

        auto dataContext = current.DataContext();
        if (IsTaskbarAppItemViewModel(dataContext)) {
            return dataContext;
        }

        if (auto presenter =
                current.try_as<Controls::ContentPresenter>()) {
            auto content = presenter.Content();
            if (IsTaskbarAppItemViewModel(content)) {
                return content;
            }
        }

        if (auto contentControl =
                current.try_as<Controls::ContentControl>()) {
            auto content = contentControl.Content();
            if (IsTaskbarAppItemViewModel(content)) {
                return content;
            }
        }

        current =
            current.Parent()
                .try_as<Xaml::FrameworkElement>();
    }

    return nullptr;
}

bool IsTaskListWindowViewModel(
    winrt::Windows::Foundation::IInspectable const& viewModel) {
    return viewModel &&
           winrt::get_class_name(viewModel) ==
               L"Taskbar.TaskListWindowViewModel";
}

void SynchronizeTaskListButtonElement(
    Xaml::FrameworkElement const& element) {
    if (g_unloading || !TaskListButton_put_HasLabel_Original)
        return;

    try {
        if (!element)
            return;

        auto viewModel =
            FindTaskbarAppItemViewModel(element);
        if (!viewModel)
            return;

        ScopedBindingContext bindingContext(
            winrt::get_abi(viewModel));

        if (!IsTaskListWindowViewModel(viewModel))
            return;

        auto isMultiWindow =
            IsWindowViewModelMultiWindow(
                winrt::get_abi(viewModel));
        if (isMultiWindow.has_value() &&
            g_bindingTaskGroupIdentity) {
            auto button =
                element.as<
                    winrt::Windows::Foundation::IUnknown>();
            void* buttonInterface = winrt::get_abi(button);
            HRESULT result =
                TaskListButton_put_HasLabel_Original(
                    buttonInterface, *isMultiWindow);
            if (SUCCEEDED(result)) {
                TrackTaskListButton(buttonInterface);
            } else {
                Wh_Log(
                    L"Initial button update failed: 0x%08X",
                    result);
            }
        }

    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Existing button sync failed: 0x%08X",
               e.code().value);
    }
}

void SynchronizeExistingTaskListButton(void* implementationThis) {
    try {
        SynchronizeTaskListButtonElement(
            GetTaskListButtonElement(implementationThis));
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"XAML button lookup failed: 0x%08X",
               e.code().value);
    }
}

void WINAPI TaskListButton_UpdateVisualStates_Hook(
    void* implementationThis) {
    TaskListButton_UpdateVisualStates_Original(implementationThis);
    if (g_unloading)
        return;

    if (g_synchronizingExistingTaskListButton)
        return;

    g_synchronizingExistingTaskListButton = true;
    SynchronizeExistingTaskListButton(implementationThis);
    g_synchronizingExistingTaskListButton = false;
}

uintptr_t GetXamlElementIdentityValue(
    Xaml::FrameworkElement const& element) {
    auto identity =
        element.as<winrt::Windows::Foundation::IUnknown>();
    return reinterpret_cast<uintptr_t>(
        winrt::get_abi(identity));
}

void RegisterXamlTaskListButton(
    InstanceHandle handle,
    Xaml::FrameworkElement const& element) {
    if (!element || element.Name() != L"TaskListButton")
        return;

    uintptr_t identityValue =
        GetXamlElementIdentityValue(element);
    {
        std::lock_guard lock(
            g_trackedXamlTaskListButtonsMutex);
        for (auto& tracked : g_trackedXamlTaskListButtons) {
            if (tracked.identityValue == identityValue) {
                tracked.element = element;
                tracked.dispatcher = element.Dispatcher();
                tracked.handle = handle;
                return;
            }
        }

        g_trackedXamlTaskListButtons.push_back({
            .element = element,
            .dispatcher = element.Dispatcher(),
            .handle = handle,
            .identityValue = identityValue,
        });
    }
}

void UnregisterXamlTaskListButton(InstanceHandle handle) {
    std::lock_guard lock(g_trackedXamlTaskListButtonsMutex);
    for (auto it = g_trackedXamlTaskListButtons.begin();
         it != g_trackedXamlTaskListButtons.end(); ++it) {
        if (it->handle == handle) {
            g_trackedXamlTaskListButtons.erase(it);
            return;
        }
    }
}

void RestoreTaskListButtonElement(
    Xaml::FrameworkElement const& element) {
    if (!element || !TaskListButton_put_HasLabel_Original ||
        !TaskListWindowViewModel_ITaskbarAppItemViewModel_get_HasLabel_Original) {
        return;
    }

    auto viewModel =
        FindTaskbarAppItemViewModel(element);
    if (!viewModel)
        return;

    if (!IsTaskListWindowViewModel(viewModel))
        return;

    void* appItemViewModel =
        FindTaskListWindowAppItemViewModel(
            winrt::get_abi(viewModel));
    if (!appItemViewModel)
        return;

    bool windowsHasLabel = false;
    HRESULT result =
        TaskListWindowViewModel_ITaskbarAppItemViewModel_get_HasLabel_Original(
            appItemViewModel, &windowsHasLabel);
    if (FAILED(result)) {
        Wh_Log(
            L"Restore label query failed: 0x%08X",
            result);
        return;
    }

    auto button =
        element.as<winrt::Windows::Foundation::IUnknown>();
    result =
        TaskListButton_put_HasLabel_Original(
            winrt::get_abi(button), windowsHasLabel);
    if (FAILED(result)) {
        Wh_Log(L"Restore button update failed: 0x%08X",
               result);
        return;
    }
}

enum class XamlButtonOperation {
    Synchronize,
    Restore,
};

void ProcessAllKnownXamlTaskListButtons(
    XamlButtonOperation operation) {
    // XAML Diagnostics callbacks can run on different taskbar UI threads.
    // Marshal each button operation to the dispatcher that owns the element.
    std::vector<TrackedXamlTaskListButton> buttons;
    {
        std::lock_guard lock(
            g_trackedXamlTaskListButtonsMutex);
        buttons = g_trackedXamlTaskListButtons;
    }

    for (auto const& tracked : buttons) {
        if (!tracked.dispatcher)
            continue;

        try {
            auto process =
                [element = tracked.element, operation]() {
                    if (!element)
                        return;

                    if (operation ==
                        XamlButtonOperation::Synchronize) {
                        SynchronizeTaskListButtonElement(
                            element);
                    } else {
                        RestoreTaskListButtonElement(element);
                    }
                };

            if (tracked.dispatcher.HasThreadAccess()) {
                process();
            } else {
                tracked.dispatcher
                    .RunAsync(
                        Core::CoreDispatcherPriority::Normal,
                        process)
                    .get();
            }
        } catch (winrt::hresult_error const& e) {
            Wh_Log(
                L"XAML dispatcher operation failed: 0x%08X",
                e.code().value);
        }
    }
}

void WINAPI CTaskListWnd_GroupChanged_Hook(void* pThis,
                                           void* taskGroup,
                                           int property) {
    CTaskListWnd_GroupChanged_Original(
        pThis, taskGroup, property);
    if (g_unloading || g_refreshingTaskListButtons ||
        !taskGroup) {
        return;
    }

    auto itemCount =
        GetCurrentDesktopTaskGroupItemCount(taskGroup);
    if (!itemCount)
        return;

    RefreshTrackedTaskListButtonsForGroup(
        taskGroup, *itemCount);
}

HRESULT WINAPI CTaskBand_UpdateVirtualDesktopInclusion_Hook(
    void* pThis) {
    HRESULT result =
        CTaskBand_UpdateVirtualDesktopInclusion_Original(pThis);
    if (!g_unloading && SUCCEEDED(result) &&
        g_xamlDiagnosticsInitialized &&
        !g_refreshingVirtualDesktopButtons) {
        // Native TaskItem visibility has now been updated as one batch.
        // Re-capture each live button's TaskGroup and apply the count for the
        // newly current virtual desktop.
        g_refreshingVirtualDesktopButtons = true;
        ProcessAllKnownXamlTaskListButtons(
            XamlButtonOperation::Synchronize);
        g_refreshingVirtualDesktopButtons = false;
    }
    return result;
}

HRESULT WINAPI TaskListButton_put_HasLabel_Hook(
    void* pThis,
    bool hasLabel) {
    HRESULT result =
        TaskListButton_put_HasLabel_Original(pThis, hasLabel);
    if (!g_unloading && !g_refreshingTaskListButtons &&
        g_bindingViewModelIdentity &&
        g_bindingTaskGroupIdentity && SUCCEEDED(result)) {
        TrackTaskListButton(pThis);
    }
    return result;
}

#define DEFINE_TASK_LIST_BUTTON_BINDINGS_UPDATE_HOOK(number)             \
    void WINAPI TaskListButtonBindings_obj##number##_Update_Hook(        \
        void* pThis, void* viewModelArgument, int phase) {                \
        void* viewModel = viewModelArgument                              \
                              ? *reinterpret_cast<void**>(                \
                                    viewModelArgument)                    \
                              : nullptr;                                  \
        ScopedBindingContext bindingContext(viewModel);                   \
        TaskListButtonBindings_obj##number##_Update_Original(            \
            pThis, viewModelArgument, phase);                            \
    }

DEFINE_TASK_LIST_BUTTON_BINDINGS_UPDATE_HOOK(3)
DEFINE_TASK_LIST_BUTTON_BINDINGS_UPDATE_HOOK(6)
DEFINE_TASK_LIST_BUTTON_BINDINGS_UPDATE_HOOK(9)
DEFINE_TASK_LIST_BUTTON_BINDINGS_UPDATE_HOOK(12)
DEFINE_TASK_LIST_BUTTON_BINDINGS_UPDATE_HOOK(15)
DEFINE_TASK_LIST_BUTTON_BINDINGS_UPDATE_HOOK(18)
DEFINE_TASK_LIST_BUTTON_BINDINGS_UPDATE_HOOK(21)
DEFINE_TASK_LIST_BUTTON_BINDINGS_UPDATE_HOOK(24)

#undef DEFINE_TASK_LIST_BUTTON_BINDINGS_UPDATE_HOOK

HRESULT WINAPI TaskListWindowViewModel_get_HasLabel_Hook(
    void* pThis,
    bool* hasLabel) {
    HRESULT result =
        TaskListWindowViewModel_ITaskbarAppItemViewModel_get_HasLabel_Original(
            pThis, hasLabel);
    if (g_unloading || g_hasLabelEvaluationDepth == 0 ||
        FAILED(result) || !hasLabel) {
        return result;
    }

    auto isMultiWindow = IsWindowViewModelMultiWindow(pThis);
    if (isMultiWindow.has_value()) {
        *hasLabel = *isMultiWindow;
    }

    return result;
}

LONG WINAPI RegGetValueW_Hook(HKEY key,
                              LPCWSTR subKey,
                              LPCWSTR value,
                              DWORD flags,
                              LPDWORD type,
                              PVOID data,
                              LPDWORD dataSize) {
    LONG result = RegGetValueW_Original(
        key, subKey, value, flags, type, data, dataSize);

    // Never combine is a global prerequisite. During unload, return the real
    // registry value so the user's taskbar setting can be restored.
    if (!g_unloading && key == HKEY_CURRENT_USER && subKey &&
        value &&
        _wcsicmp(
            subKey,
            LR"(SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Advanced)") ==
            0 &&
        (_wcsicmp(value, L"TaskbarGlomLevel") == 0 ||
         _wcsicmp(value, L"MMTaskbarGlomLevel") == 0) &&
        flags == RRF_RT_REG_DWORD && data && dataSize &&
        *dataSize == sizeof(DWORD)) {
        *reinterpret_cast<DWORD*>(data) = 2;
        if (type) {
            *type = REG_DWORD;
        }
        return ERROR_SUCCESS;
    }

    return result;
}

struct VisualTreeWatcher
    : winrt::implements<VisualTreeWatcher,
                        IVisualTreeServiceCallback2,
                        winrt::non_agile> {
    void SetXamlDiagnostics(
        winrt::com_ptr<IXamlDiagnostics> diagnostics) {
        m_diagnostics = std::move(diagnostics);
    }

    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(
        ParentChildRelation,
        VisualElement element,
        VisualMutationType mutationType) noexcept override {
        if (mutationType == Remove) {
            UnregisterXamlTaskListButton(element.Handle);
            return S_OK;
        }

        if (mutationType != Add || !m_diagnostics)
            return S_OK;

        try {
            ::IInspectable* inspectableRaw = nullptr;
            winrt::check_hresult(
                m_diagnostics->GetIInspectableFromHandle(
                    element.Handle, &inspectableRaw));
            winrt::Windows::Foundation::IInspectable inspectable{
                inspectableRaw, winrt::take_ownership_from_abi};
            auto frameworkElement =
                inspectable.try_as<Xaml::FrameworkElement>();
            if (frameworkElement &&
                frameworkElement.Name() == L"TaskListButton") {
                RegisterXamlTaskListButton(
                    element.Handle, frameworkElement);
            }
            return S_OK;
        } catch (...) {
            HRESULT result = winrt::to_hresult();
            Wh_Log(
                L"Visual tree callback failed: 0x%08X",
                result);
            return result;
        }
    }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(
        InstanceHandle,
        VisualElementState,
        LPCWSTR) noexcept override {
        return S_OK;
    }

   private:
    winrt::com_ptr<IXamlDiagnostics> m_diagnostics;
};

std::mutex g_visualTreeServiceMutex;
winrt::com_ptr<IVisualTreeService3> g_visualTreeService;
winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;

// {D4AA485B-0FAF-4D60-BC81-4D10987AF0F6}
constexpr CLSID kDynamicLabelsTapClsid{
    0xd4aa485b,
    0x0faf,
    0x4d60,
    {0xbc, 0x81, 0x4d, 0x10, 0x98, 0x7a, 0xf0, 0xf6},
};

struct DynamicLabelsTap
    : winrt::implements<DynamicLabelsTap,
                        IObjectWithSite,
                        winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* site) noexcept override {
        try {
            std::unique_lock lock(g_visualTreeServiceMutex);
            if (g_visualTreeService && g_visualTreeWatcher) {
                g_visualTreeService->UnadviseVisualTreeChange(
                    g_visualTreeWatcher.get());
                g_visualTreeWatcher->SetXamlDiagnostics(nullptr);
            }

            g_visualTreeWatcher = nullptr;
            g_visualTreeService = nullptr;
            g_xamlDiagnosticsInitialized = false;
            if (!site)
                return S_OK;

            // InitializeXamlDiagnosticsEx loads the TAP DLL by path. Balance
            // that extra reference so Windhawk can unload the Mod after the
            // visual tree service and watcher are released.
            HMODULE currentModule = nullptr;
            if (GetModuleHandleExW(
                    GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                    reinterpret_cast<LPCWSTR>(
                        &g_taskbarViewDllLoaded),
                    &currentModule)) {
                FreeLibrary(currentModule);
            }

            winrt::com_ptr<IUnknown> siteUnknown;
            siteUnknown.copy_from(site);
            auto visualTreeService =
                siteUnknown.as<IVisualTreeService3>();
            auto watcher =
                winrt::make_self<VisualTreeWatcher>();
            watcher->SetXamlDiagnostics(
                visualTreeService.as<IXamlDiagnostics>());
            winrt::check_hresult(
                visualTreeService->AdviseVisualTreeChange(
                    watcher.get()));

            g_visualTreeService =
                std::move(visualTreeService);
            g_visualTreeWatcher = std::move(watcher);
            g_xamlDiagnosticsInitialized = true;
            lock.unlock();
            ProcessAllKnownXamlTaskListButtons(
                XamlButtonOperation::Synchronize);
            return S_OK;
        } catch (...) {
            HRESULT result = winrt::to_hresult();
            Wh_Log(L"TAP SetSite failed: 0x%08X",
                   result);
            return result;
        }
    }

    HRESULT STDMETHODCALLTYPE GetSite(
        REFIID iid,
        void** site) noexcept override {
        std::lock_guard lock(g_visualTreeServiceMutex);
        if (!g_visualTreeService) {
            *site = nullptr;
            return E_FAIL;
        }

        return g_visualTreeService.as(iid, site);
    }
};

void ShutdownXamlVisualTreeWatcher() {
    std::lock_guard lock(g_visualTreeServiceMutex);
    if (g_visualTreeService && g_visualTreeWatcher) {
        HRESULT result =
            g_visualTreeService->UnadviseVisualTreeChange(
                g_visualTreeWatcher.get());
        if (FAILED(result)) {
            Wh_Log(
                L"Visual tree detach failed: "
                L"0x%08X",
                result);
        }
        g_visualTreeWatcher->SetXamlDiagnostics(nullptr);
    }

    g_visualTreeWatcher = nullptr;
    g_visualTreeService = nullptr;
    g_xamlDiagnosticsInitialized = false;
}

template <typename T>
struct SimpleClassFactory
    : winrt::implements<SimpleClassFactory<T>,
                        IClassFactory,
                        winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE CreateInstance(
        IUnknown* outer,
        REFIID iid,
        void** object) noexcept override {
        if (outer) {
            *object = nullptr;
            return CLASS_E_NOAGGREGATION;
        }

        try {
            return winrt::make<T>().as(iid, object);
        } catch (...) {
            return winrt::to_hresult();
        }
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override {
        return S_OK;
    }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

extern "C" __declspec(dllexport) HRESULT WINAPI
DllGetClassObject(REFCLSID clsid,
                  REFIID iid,
                  void** object) {
    if (clsid != kDynamicLabelsTapClsid) {
        *object = nullptr;
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    try {
        return winrt::make<
                   SimpleClassFactory<DynamicLabelsTap>>()
            .as(iid, object);
    } catch (...) {
        return winrt::to_hresult();
    }
}

extern "C" __declspec(dllexport) HRESULT WINAPI
DllCanUnloadNow() {
    return winrt::get_module_lock() ? S_FALSE : S_OK;
}

#pragma clang diagnostic pop

HRESULT InitializeXamlVisualTreeWatcher() {
    if (g_xamlDiagnosticsInitialized)
        return S_OK;

    HMODULE currentModule = nullptr;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(
                &g_taskbarViewDllLoaded),
            &currentModule)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    WCHAR modulePath[MAX_PATH];
    DWORD modulePathLength = GetModuleFileNameW(
        currentModule, modulePath, ARRAYSIZE(modulePath));
    if (!modulePathLength ||
        modulePathLength == ARRAYSIZE(modulePath)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    HMODULE xamlModule = LoadLibraryExW(
        L"Windows.UI.Xaml.dll", nullptr,
        LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!xamlModule)
        return HRESULT_FROM_WIN32(GetLastError());

    using InitializeXamlDiagnosticsEx_t =
        HRESULT(WINAPI*)(PCWSTR,
                         DWORD,
                         PCWSTR,
                         PCWSTR,
                         CLSID,
                         PCWSTR);
    auto initialize =
        reinterpret_cast<InitializeXamlDiagnosticsEx_t>(
            GetProcAddress(
                xamlModule,
                "InitializeXamlDiagnosticsEx"));
    if (!initialize)
        return HRESULT_FROM_WIN32(GetLastError());

    // The endpoint name is allocated by the XAML framework and isn't exposed
    // through a public lookup API. Follow the same bounded probe used by the
    // upstream taskbar styler and stop as soon as the endpoint exists.
    HRESULT result = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    WCHAR connectionName[64];
    for (int i = 1; i <= 10000; ++i) {
        swprintf_s(
            connectionName,
            ARRAYSIZE(connectionName),
            L"VisualDiagConnection%d",
            i);
        result = initialize(
            connectionName,
            GetCurrentProcessId(),
            L"",
            modulePath,
            kDynamicLabelsTapClsid,
            nullptr);
        if (result != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
            break;
        }
    }
    if (FAILED(result)) {
        Wh_Log(
            L"XAML diagnostics initialization failed: "
            L"0x%08X",
            result);
    }
    return result;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    return module
               ? module
               : GetModuleHandleW(L"ExplorerExtensions.dll");
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK taskbarViewDllHooks[] = {
        {
            {LR"(public: __cdecl winrt::impl::consume_Taskbar_ITaskbarAppItemViewModel<struct winrt::Taskbar::ITaskbarAppItemViewModel>::HasLabel(void)const )"},
            &ITaskbarAppItemViewModel_HasLabels_Original,
            ITaskbarAppItemViewModel_HasLabels_Hook,
            true,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::`vftable')"},
            &ITaskListWindowViewModel_vftable,
            nullptr,
            true,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::`vftable')"},
            &TaskListWindowViewModel_ITaskbarAppItemViewModel_vftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_TaskItem(void * *))"},
            &ITaskListWindowViewModel_get_TaskItem,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::get_HasLabel(bool *))"},
            &TaskListWindowViewModel_ITaskbarAppItemViewModel_get_HasLabel_Original,
            TaskListWindowViewModel_get_HasLabel_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Taskbar::ITaskListButton>::put_HasLabel(bool))"},
            &TaskListButton_put_HasLabel_Original,
            TaskListButton_put_HasLabel_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
            true,
        },
        {
            {LR"(private: virtual void __cdecl winrt::Taskbar::implementation::TaskListButtonResourcesT<struct winrt::Taskbar::implementation::TaskListButtonResources>::TaskListButtonResources_obj3_Bindings::Update_(struct winrt::Taskbar::ITaskbarAppItemViewModel,int))"},
            &TaskListButtonBindings_obj3_Update_Original,
            TaskListButtonBindings_obj3_Update_Hook,
            true,
        },
        {
            {LR"(private: virtual void __cdecl winrt::Taskbar::implementation::TaskListButtonResourcesT<struct winrt::Taskbar::implementation::TaskListButtonResources>::TaskListButtonResources_obj6_Bindings::Update_(struct winrt::Taskbar::ITaskbarAppItemViewModel,int))"},
            &TaskListButtonBindings_obj6_Update_Original,
            TaskListButtonBindings_obj6_Update_Hook,
            true,
        },
        {
            {LR"(private: virtual void __cdecl winrt::Taskbar::implementation::TaskListButtonResourcesT<struct winrt::Taskbar::implementation::TaskListButtonResources>::TaskListButtonResources_obj9_Bindings::Update_(struct winrt::Taskbar::ITaskbarAppItemViewModel,int))"},
            &TaskListButtonBindings_obj9_Update_Original,
            TaskListButtonBindings_obj9_Update_Hook,
            true,
        },
        {
            {LR"(private: virtual void __cdecl winrt::Taskbar::implementation::TaskListButtonResourcesT<struct winrt::Taskbar::implementation::TaskListButtonResources>::TaskListButtonResources_obj12_Bindings::Update_(struct winrt::Taskbar::ITaskbarAppItemViewModel,int))"},
            &TaskListButtonBindings_obj12_Update_Original,
            TaskListButtonBindings_obj12_Update_Hook,
            true,
        },
        {
            {LR"(private: virtual void __cdecl winrt::Taskbar::implementation::TaskListButtonResourcesT<struct winrt::Taskbar::implementation::TaskListButtonResources>::TaskListButtonResources_obj15_Bindings::Update_(struct winrt::Taskbar::ITaskbarAppItemViewModel,int))"},
            &TaskListButtonBindings_obj15_Update_Original,
            TaskListButtonBindings_obj15_Update_Hook,
            true,
        },
        {
            {LR"(private: virtual void __cdecl winrt::Taskbar::implementation::TaskListButtonResourcesT<struct winrt::Taskbar::implementation::TaskListButtonResources>::TaskListButtonResources_obj18_Bindings::Update_(struct winrt::Taskbar::ITaskbarAppItemViewModel,int))"},
            &TaskListButtonBindings_obj18_Update_Original,
            TaskListButtonBindings_obj18_Update_Hook,
            true,
        },
        {
            {LR"(private: virtual void __cdecl winrt::Taskbar::implementation::TaskListButtonResourcesT<struct winrt::Taskbar::implementation::TaskListButtonResources>::TaskListButtonResources_obj21_Bindings::Update_(struct winrt::Taskbar::ITaskbarAppItemViewModel,int))"},
            &TaskListButtonBindings_obj21_Update_Original,
            TaskListButtonBindings_obj21_Update_Hook,
            true,
        },
        {
            {LR"(private: virtual void __cdecl winrt::Taskbar::implementation::TaskListButtonResourcesT<struct winrt::Taskbar::implementation::TaskListButtonResources>::TaskListButtonResources_obj24_Bindings::Update_(struct winrt::Taskbar::ITaskbarAppItemViewModel,int))"},
            &TaskListButtonBindings_obj24_Update_Original,
            TaskListButtonBindings_obj24_Update_Hook,
            true,
        },
    };

    return HookSymbols(
        module,
        taskbarViewDllHooks,
        ARRAYSIZE(taskbarViewDllHooks));
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryExW(
        L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
            &CTaskListWnd_HandleClick_Original,
            CTaskListWnd_HandleClick_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskItem,struct winrt::WindowsUdk::UI::Shell::ITaskItem>::ReportClicked(void *))"},
            &TaskItem_ReportClicked_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual long __cdecl CTaskGroup::EnumTaskItems(struct IEnumTaskItems * *))"},
            &CTaskGroup_EnumTaskItems_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual long __cdecl CEnumTaskItems::Next(struct ITaskItem * *))"},
            &CEnumTaskItems_Next_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual bool __cdecl CTaskItem::IsVisibleOnCurrentVirtualDesktop(void))"},
            &CTaskItem_IsVisibleOnCurrentVirtualDesktop_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual bool __cdecl CTaskGroupTaskItem::IsVisibleOnCurrentVirtualDesktop(void))"},
            &CTaskGroupTaskItem_IsVisibleOnCurrentVirtualDesktop_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual void __cdecl CTaskListWnd::GroupChanged(struct ITaskGroup *,enum winrt::WindowsUdk::UI::Shell::TaskGroupProperty))"},
            &CTaskListWnd_GroupChanged_Original,
            CTaskListWnd_GroupChanged_Hook,
            true,
        },
        {
            {LR"(protected: long __cdecl CTaskBand::_UpdateVirtualDesktopInclusion(void))"},
            &CTaskBand_UpdateVirtualDesktopInclusion_Original,
            CTaskBand_UpdateVirtualDesktopInclusion_Hook,
            true,
        },
    };

    return HookSymbols(
        module,
        taskbarDllHooks,
        ARRAYSIZE(taskbarDllHooks));
}

BOOL ModInitWithTaskbarView(HMODULE module) {
    if (!HookTaskbarViewDllSymbols(module) ||
        !HookTaskbarDllSymbols()) {
        Wh_Log(L"Symbol setup failed");
        return FALSE;
    }

    auto regGetValue = reinterpret_cast<decltype(&RegGetValueW)>(
        GetProcAddress(
            GetModuleHandleW(L"kernelbase.dll"),
            "RegGetValueW"));
    if (!regGetValue ||
        !WindhawkUtils::SetFunctionHook(
            regGetValue,
            RegGetValueW_Hook,
            &RegGetValueW_Original)) {
        Wh_Log(L"RegGetValueW hook failed");
        return FALSE;
    }

    const bool windowGroupCapturePath =
        ITaskListWindowViewModel_vftable &&
        ITaskListWindowViewModel_get_TaskItem &&
        TaskItem_ReportClicked_Original &&
        CTaskListWnd_HandleClick_Original;
    const bool buttonRefreshPath =
        TaskListButton_put_HasLabel_Original &&
        TaskListButton_UpdateVisualStates_Original &&
        CTaskListWnd_GroupChanged_Original;
    const bool currentDesktopCountPath =
        CTaskGroup_EnumTaskItems_Original &&
        CEnumTaskItems_Next_Original &&
        (CTaskItem_IsVisibleOnCurrentVirtualDesktop_Original ||
         CTaskGroupTaskItem_IsVisibleOnCurrentVirtualDesktop_Original);
    const bool virtualDesktopRefreshPath =
        CTaskBand_UpdateVirtualDesktopInclusion_Original;
    const bool bindingInitPath =
        TaskListButtonBindings_obj3_Update_Original ||
        TaskListButtonBindings_obj6_Update_Original ||
        TaskListButtonBindings_obj9_Update_Original ||
        TaskListButtonBindings_obj12_Update_Original ||
        TaskListButtonBindings_obj15_Update_Original ||
        TaskListButtonBindings_obj18_Update_Original ||
        TaskListButtonBindings_obj21_Update_Original ||
        TaskListButtonBindings_obj24_Update_Original;

    if (!windowGroupCapturePath || !buttonRefreshPath ||
        !bindingInitPath || !currentDesktopCountPath ||
        !virtualDesktopRefreshPath) {
        Wh_Log(
            L"Required symbols are incomplete "
            L"(capture=%d, refresh=%d, bindings=%d, "
            L"currentDesktop=%d, desktopRefresh=%d); "
            L"unsupported paths keep Windows label state",
            windowGroupCapturePath,
            buttonRefreshPath,
            bindingInitPath,
            currentDesktopCountPath,
            virtualDesktopRefreshPath);
    }

    return TRUE;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module) {
    if (GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        if (ModInitWithTaskbarView(module)) {
            Wh_ApplyHookOperations();
            InitializeXamlVisualTreeWatcher();
        }
    }
}

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name,
                                   HANDLE file,
                                   DWORD flags) {
    HMODULE module =
        LoadLibraryExW_Original(name, file, flags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module);
    }
    return module;
}

void NotifyTaskbarSettingsChanged() {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbar) {
        Wh_Log(
            L"Taskbar window unavailable");
        return;
    }

    DWORD_PTR messageResult = 0;
    SetLastError(ERROR_SUCCESS);
    if (!SendMessageTimeoutW(
            taskbar,
            WM_SETTINGCHANGE,
            0,
            0,
            SMTO_ABORTIFHUNG | SMTO_BLOCK,
            2000,
            &messageResult)) {
        DWORD error = GetLastError();
        Wh_Log(
            L"Settings notification failed: %u",
            error);
    }
}

BOOL Wh_ModInit() {
    g_unloading = false;
    if (HMODULE module = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        return ModInitWithTaskbarView(module);
    }

    auto loadLibrary =
        reinterpret_cast<decltype(&LoadLibraryExW)>(
            GetProcAddress(
                GetModuleHandleW(L"kernelbase.dll"),
                "LoadLibraryExW"));
    return loadLibrary &&
           WindhawkUtils::SetFunctionHook(
               loadLibrary,
               LoadLibraryExW_Hook,
               &LoadLibraryExW_Original);
}

void Wh_ModAfterInit() {
    if (!g_taskbarViewDllLoaded) {
        if (HMODULE module = GetTaskbarViewModuleHandle()) {
            HandleLoadedModuleIfTaskbarView(module);
        }
    }

    if (g_taskbarViewDllLoaded) {
        InitializeXamlVisualTreeWatcher();
        NotifyTaskbarSettingsChanged();
    }
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    ProcessAllKnownXamlTaskListButtons(
        XamlButtonOperation::Restore);
    ShutdownXamlVisualTreeWatcher();
    {
        std::lock_guard lock(
            g_trackedXamlTaskListButtonsMutex);
        g_trackedXamlTaskListButtons.clear();
    }
    {
        std::lock_guard lock(g_trackedTaskListButtonsMutex);
        g_trackedTaskListButtons.clear();
    }
    NotifyTaskbarSettingsChanged();
}

void Wh_ModUninit() {}
