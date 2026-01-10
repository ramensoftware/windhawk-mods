// ==WindhawkMod==
// @id              taskbar-reorder-right-drag
// @name            Taskbar reorder within/between groups
// @description     Reorder taskbar items within and between groups by dragging with the right mouse button
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshlwapi
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Taskbar reorder within/between groups

Reorder taskbar items within and between groups by dragging with the right mouse
button.

The mod works best with the "Combine taskbar buttons and hide labels" option set
to "Never" in the taskbar settings.

Only Windows 11 is supported. For older Windows versions check out [7+ Taskbar
Tweaker](https://tweaker.ramensoftware.com/).

## Known limitations

* Windows of programs running as administrator cannot be moved to different
  groups.
* Some Store/modern/UWP apps cannot be moved to different groups.

![demonstration](https://i.imgur.com/RgMASfK.gif)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <initguid.h>  // Must appear before propkey.h

#include <propkey.h>
#include <propsys.h>
#include <shlwapi.h>

#include <atomic>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

using namespace winrt::Windows::UI::Xaml;

std::atomic<bool> g_taskbarViewDllLoaded;

bool g_taskItemFilterDisallowAll;
std::atomic<DWORD> g_getPtr_captureForThreadId;
HDPA g_getPtr_lastHdpa;

// https://www.geoffchappell.com/studies/windows/shell/comctl32/api/da/dpa/dpa.htm
typedef struct _DPA {
    int cpItems;
    PVOID* pArray;
    HANDLE hHeap;
    int cpCapacity;
    int cpGrow;
} DPA, *HDPA;

// Right-drag state tracking for taskbar buttons.
struct {
    winrt::weak_ref<UIElement> sourceElement;
    winrt::weak_ref<UIElement> targetElement;
    FrameworkElement dropIndicator = nullptr;
    Controls::Primitives::Popup newGroupPopup = nullptr;
    bool insertAfter = false;
    bool newGroup = false;
} g_rightDragState;

void* QueryViaVtable(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) {
        ptr = (void**)ptr + 1;
    }
    return ptr;
}

BOOL WndSetAppId(HWND hWnd, const WCHAR* pAppId) {
    IPropertyStore* pps;
    HRESULT hr = SHGetPropertyStoreForWindow(hWnd, IID_PPV_ARGS(&pps));
    if (SUCCEEDED(hr)) {
        PROPVARIANT pv;
        if (pAppId) {
            pv.vt = VT_LPWSTR;
            hr = SHStrDup(pAppId, &pv.pwszVal);
        } else {
            PropVariantInit(&pv);
        }

        if (SUCCEEDED(hr)) {
            hr = pps->SetValue(PKEY_AppUserModel_ID, pv);
            if (SUCCEEDED(hr)) {
                hr = pps->Commit();
            }
            PropVariantClear(&pv);
        }

        pps->Release();
    }

    return SUCCEEDED(hr);
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param{
        .proc = proc,
        .procParam = procParam,
    };
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

void* CTaskListWnd_vftable_ITaskListUI;

using CTaskGroup_GetNumItems_t = int(WINAPI*)(PVOID pThis);
CTaskGroup_GetNumItems_t CTaskGroup_GetNumItems;

using CTaskGroup_GetAppID_t = PCWSTR(WINAPI*)(void* pThis);
CTaskGroup_GetAppID_t CTaskGroup_GetAppID;

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(void* pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow;

using CImmersiveTaskItem_GetWindow_t = HWND(WINAPI*)(void* pThis);
CImmersiveTaskItem_GetWindow_t CImmersiveTaskItem_GetWindow;

void* CImmersiveTaskItem_vftable;

HWND GetWindowFromTaskItem(void* taskItem) {
    if (!taskItem) {
        return nullptr;
    }

    if (*(void**)taskItem == CImmersiveTaskItem_vftable) {
        return CImmersiveTaskItem_GetWindow(taskItem);
    }

    return CWindowTaskItem_GetWindow(taskItem);
}

using CTaskListWnd__GetTBGroupFromGroup_t =
    void*(WINAPI*)(void* pThis, void* pTaskGroup, int* pTaskBtnGroupIndex);
CTaskListWnd__GetTBGroupFromGroup_t CTaskListWnd__GetTBGroupFromGroup;

using CTaskBtnGroup_GetGroupType_t = int(WINAPI*)(void* pThis);
CTaskBtnGroup_GetGroupType_t CTaskBtnGroup_GetGroupType;

using CTaskBtnGroup_IndexOfTaskItem_t = int(WINAPI*)(void* pThis,
                                                     void* taskItem);
CTaskBtnGroup_IndexOfTaskItem_t CTaskBtnGroup_IndexOfTaskItem;

using CTaskListWnd_TaskInclusionChanged_t = HRESULT(WINAPI*)(void* pThis,
                                                             void* pTaskGroup,
                                                             void* pTaskItem);
CTaskListWnd_TaskInclusionChanged_t CTaskListWnd_TaskInclusionChanged;

using TaskItemFilter_IsTaskAllowed_t = bool(WINAPI*)(void* pThis,
                                                     void* pTaskItem);
TaskItemFilter_IsTaskAllowed_t TaskItemFilter_IsTaskAllowed_Original;
bool WINAPI TaskItemFilter_IsTaskAllowed_Hook(void* pThis, void* pTaskItem) {
    if (g_taskItemFilterDisallowAll) {
        Wh_Log(L">");
        return false;
    }

    return TaskItemFilter_IsTaskAllowed_Original(pThis, pTaskItem);
}

WCHAR g_clickSentinel[] = L"click-sentinel";
void* g_clickSentinel_TaskGroup = nullptr;
void* g_clickSentinel_TaskItem = nullptr;

using CTaskListWnd_HandleClick_t = HRESULT(WINAPI*)(void* pThis,
                                                    void* taskGroup,
                                                    void* taskItem,
                                                    void** launcherOptions);
CTaskListWnd_HandleClick_t CTaskListWnd_HandleClick_Original;
HRESULT WINAPI CTaskListWnd_HandleClick_Hook(void* pThis,
                                             void* taskGroup,
                                             void* taskItem,
                                             void** launcherOptions) {
    Wh_Log(L"> taskGroup=%p, taskItem=%p, launcherOptions=%p", taskGroup,
           taskItem, launcherOptions);

    if (*launcherOptions == &g_clickSentinel) {
        Wh_Log(L"Click triggered by sentinel");
        g_clickSentinel_TaskGroup = taskGroup;
        g_clickSentinel_TaskItem = taskItem;
        return S_OK;
    }

    return CTaskListWnd_HandleClick_Original(pThis, taskGroup, taskItem,
                                             launcherOptions);
}

using TaskItem_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskItem_ReportClicked_t TaskItem_ReportClicked_Original;

struct TaskGroupAndItem {
    void* taskGroup = nullptr;
    void* taskItem = nullptr;
};

// Triggers the sentinel to capture real task group and task item from
// WindowsUdk task item.
TaskGroupAndItem GetTaskGroupAndItemFromWindowsUdkTaskItem(
    void* windowsUdkTaskItem) {
    g_clickSentinel_TaskGroup = nullptr;
    g_clickSentinel_TaskItem = nullptr;
    TaskItem_ReportClicked_Original(windowsUdkTaskItem, &g_clickSentinel);
    return {g_clickSentinel_TaskGroup, g_clickSentinel_TaskItem};
}

bool MoveTaskInTaskList(HWND hMMTaskListWnd,
                        void* lpMMTaskListLongPtr,
                        void* taskGroup,
                        void* taskItemFrom,
                        void* taskItemTo) {
    void* taskBtnGroup = CTaskListWnd__GetTBGroupFromGroup(lpMMTaskListLongPtr,
                                                           taskGroup, nullptr);
    if (!taskBtnGroup) {
        return false;
    }

    int taskBtnGroupType = CTaskBtnGroup_GetGroupType(taskBtnGroup);
    if (taskBtnGroupType != 1 && taskBtnGroupType != 3) {
        return false;
    }

    g_getPtr_lastHdpa = nullptr;

    g_getPtr_captureForThreadId = GetCurrentThreadId();
    int indexFrom = CTaskBtnGroup_IndexOfTaskItem(taskBtnGroup, taskItemFrom);
    g_getPtr_captureForThreadId = 0;

    if (indexFrom == -1) {
        return false;
    }

    HDPA buttonsArray = g_getPtr_lastHdpa;
    if (!buttonsArray) {
        return false;
    }

    g_getPtr_lastHdpa = nullptr;

    g_getPtr_captureForThreadId = GetCurrentThreadId();
    int indexTo = CTaskBtnGroup_IndexOfTaskItem(taskBtnGroup, taskItemTo);
    g_getPtr_captureForThreadId = 0;

    if (indexTo == -1) {
        return false;
    }

    if (g_getPtr_lastHdpa != buttonsArray) {
        return false;
    }

    void* button = (void*)DPA_DeletePtr(buttonsArray, indexFrom);
    if (!button) {
        return false;
    }

    DPA_InsertPtr(buttonsArray, indexTo, button);

    HWND hThumbnailWnd = GetCapture();
    if (hThumbnailWnd) {
        WCHAR szClassName[32];
        if (GetClassName(hThumbnailWnd, szClassName, ARRAYSIZE(szClassName)) ==
                0 ||
            _wcsicmp(szClassName, L"TaskListThumbnailWnd") != 0) {
            hThumbnailWnd = nullptr;
        }
    }

    if (hThumbnailWnd) {
        SendMessage(hThumbnailWnd, WM_SETREDRAW, FALSE, 0);
    }

    g_taskItemFilterDisallowAll = true;

    void* pThis_ITaskListUI =
        QueryViaVtable(lpMMTaskListLongPtr, CTaskListWnd_vftable_ITaskListUI);

    CTaskListWnd_TaskInclusionChanged(pThis_ITaskListUI, taskGroup,
                                      taskItemFrom);

    g_taskItemFilterDisallowAll = false;

    CTaskListWnd_TaskInclusionChanged(pThis_ITaskListUI, taskGroup,
                                      taskItemFrom);

    if (hThumbnailWnd) {
        SendMessage(hThumbnailWnd, WM_SETREDRAW, TRUE, 0);
        RedrawWindow(hThumbnailWnd, nullptr, nullptr,
                     RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
    }

    return true;
}

HDPA GetTaskItemsArray(void* taskGroup) {
    // This is a horrible hack, but it's the best way I found to get the array
    // of task items from a task group. It relies on the implementation of
    // CTaskGroup::GetNumItems being just this:
    //
    // return DPA_GetPtrCount(this->taskItemsArray);
    //
    // Or in other words:
    //
    // return *(int*)this[taskItemsArrayOffset];
    //
    // Instead of calling it with a real taskGroup object, we call it with an
    // array of pointers to ints. The returned int value is actually the offset
    // to the array member.

    static size_t offset = []() {
        constexpr int kIntArraySize = 256;
        int arrayOfInts[kIntArraySize];
        int* arrayOfIntPtrs[kIntArraySize];
        for (int i = 0; i < kIntArraySize; i++) {
            arrayOfInts[i] = i;
            arrayOfIntPtrs[i] = &arrayOfInts[i];
        }

        return CTaskGroup_GetNumItems(arrayOfIntPtrs);
    }();

    return (HDPA)((void**)taskGroup)[offset];
}

bool MoveTaskInGroup(void* taskGroup,
                     void* taskItemFrom,
                     void* taskItemTo,
                     bool insertAfter) {
    HDPA taskItemsArray = GetTaskItemsArray(taskGroup);
    if (!taskItemsArray) {
        return false;
    }

    int taskItemsCount = taskItemsArray->cpItems;
    void** taskItems = (void**)taskItemsArray->pArray;

    int indexFrom = -1;
    for (int i = 0; i < taskItemsCount; i++) {
        if (taskItems[i] == taskItemFrom) {
            indexFrom = i;
            break;
        }
    }

    if (indexFrom == -1) {
        return false;
    }

    int indexTo = -1;
    for (int i = 0; i < taskItemsCount; i++) {
        if (taskItems[i] == taskItemTo) {
            indexTo = i;
            break;
        }
    }

    if (indexTo == -1) {
        return false;
    }

    // Adjust indexTo based on insertAfter flag:
    // - When dragging right (indexFrom < indexTo): default is "after", so
    //   subtract 1 if insertAfter is false.
    // - When dragging left (indexFrom > indexTo): default is "before", so
    //   add 1 if insertAfter is true.
    if (indexFrom < indexTo && !insertAfter) {
        indexTo--;
    } else if (indexFrom > indexTo && insertAfter) {
        indexTo++;
    }

    // If adjusted indexTo equals indexFrom, no movement needed.
    if (indexTo == indexFrom) {
        return true;
    }

    void* taskItemTemp = taskItems[indexFrom];
    if (indexFrom < indexTo) {
        memmove(&taskItems[indexFrom], &taskItems[indexFrom + 1],
                (indexTo - indexFrom) * sizeof(void*));
    } else {
        memmove(&taskItems[indexTo + 1], &taskItems[indexTo],
                (indexFrom - indexTo) * sizeof(void*));
    }
    taskItems[indexTo] = taskItemTemp;

    auto taskbarEnumProc = [taskGroup, taskItemFrom, taskItemTo](
                               HWND hMMTaskbarWnd, bool secondary) {
        HWND hMMTaskSwWnd;
        if (!secondary) {
            hMMTaskSwWnd = (HWND)GetProp(hMMTaskbarWnd, L"TaskbandHWND");
        } else {
            hMMTaskSwWnd =
                FindWindowEx(hMMTaskbarWnd, nullptr, L"WorkerW", nullptr);
        }

        if (!hMMTaskSwWnd) {
            return;
        }

        HWND hMMTaskListWnd =
            FindWindowEx(hMMTaskSwWnd, nullptr, L"MSTaskListWClass", nullptr);
        if (!hMMTaskListWnd) {
            return;
        }

        void* lpMMTaskListLongPtr = (void*)GetWindowLongPtr(hMMTaskListWnd, 0);

        if (!MoveTaskInTaskList(hMMTaskListWnd, lpMMTaskListLongPtr, taskGroup,
                                taskItemFrom, taskItemTo)) {
            Wh_Log(L"Failed to move task item in taskbar %08X",
                   (DWORD)(DWORD_PTR)hMMTaskListWnd);
        }
    };

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            bool secondary;
            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                secondary = false;
            } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                secondary = true;
            } else {
                return TRUE;
            }

            auto& proc = *reinterpret_cast<decltype(taskbarEnumProc)*>(lParam);
            proc(hWnd, secondary);
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&taskbarEnumProc));

    return true;
}

using TryGetItemFromContainer_TaskListWindowViewModel_t =
    void*(WINAPI*)(void** output, UIElement* container);
TryGetItemFromContainer_TaskListWindowViewModel_t
    TryGetItemFromContainer_TaskListWindowViewModel_Original;

using TaskListWindowViewModel_get_TaskItem_t = int(WINAPI*)(void* pThis,
                                                            void** taskItem);
TaskListWindowViewModel_get_TaskItem_t
    TaskListWindowViewModel_get_TaskItem_Original;

void* GetWindowsUdkTaskItemFromTaskListButton(UIElement element) {
    winrt::com_ptr<IUnknown> windowViewModel = nullptr;
    TryGetItemFromContainer_TaskListWindowViewModel_Original(
        windowViewModel.put_void(), &element);
    if (!windowViewModel) {
        return nullptr;
    }

    winrt::com_ptr<IUnknown> windowsUdkTaskItem;
    TaskListWindowViewModel_get_TaskItem_Original(
        windowViewModel.get(), windowsUdkTaskItem.put_void());
    return windowsUdkTaskItem.get();
}

void* GetTaskGroupFromElement(UIElement element) {
    void* windowsUdkTaskItem = GetWindowsUdkTaskItemFromTaskListButton(element);
    if (!windowsUdkTaskItem) {
        return nullptr;
    }
    return GetTaskGroupAndItemFromWindowsUdkTaskItem(windowsUdkTaskItem)
        .taskGroup;
}

Controls::Canvas g_dropIndicatorCanvas = nullptr;

Controls::Grid FindTaskbarFrameGrid(FrameworkElement taskbarFrame) {
    int childCount = Media::VisualTreeHelper::GetChildrenCount(taskbarFrame);
    for (int i = 0; i < childCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(taskbarFrame, i);
        if (auto grid = child.try_as<Controls::Grid>()) {
            return grid;
        }
    }
    return nullptr;
}

void EnsureDropIndicatorCanvas() {
    if (!g_dropIndicatorCanvas) {
        Controls::Canvas canvas;
        canvas.IsHitTestVisible(false);
        g_dropIndicatorCanvas = canvas;
    }
}

void ReparentCanvasIfNeeded(Controls::Grid newParent) {
    auto currentParent =
        Media::VisualTreeHelper::GetParent(g_dropIndicatorCanvas);
    if (currentParent != newParent) {
        // Remove from old parent if any.
        if (auto oldParent = currentParent.try_as<Controls::Panel>()) {
            UINT index = 0;
            if (oldParent.Children().IndexOf(g_dropIndicatorCanvas, index)) {
                oldParent.Children().RemoveAt(index);
            }
        }

        // Add to new parent with high z-index.
        newParent.Children().Append(g_dropIndicatorCanvas);
        Controls::Canvas::SetZIndex(g_dropIndicatorCanvas, 10000);
    }
}

void HideDropIndicator() {
    if (g_rightDragState.dropIndicator) {
        g_rightDragState.dropIndicator.Visibility(Visibility::Collapsed);
    }
    if (g_rightDragState.newGroupPopup) {
        g_rightDragState.newGroupPopup.IsOpen(false);
    }
    if (g_dropIndicatorCanvas) {
        g_dropIndicatorCanvas.Visibility(Visibility::Collapsed);
    }
}

void ShowDropIndicator(FrameworkElement targetElement,
                       FrameworkElement taskbarFrame,
                       bool insertAfter,
                       bool crossGroup) {
    if (!targetElement || !taskbarFrame) {
        HideDropIndicator();
        return;
    }

    auto indicatorParent = FindTaskbarFrameGrid(taskbarFrame);
    if (!indicatorParent) {
        HideDropIndicator();
        return;
    }

    EnsureDropIndicatorCanvas();

    if (!g_rightDragState.dropIndicator) {
        Controls::Border indicator;
        indicator.IsHitTestVisible(false);
        g_rightDragState.dropIndicator = indicator;

        g_dropIndicatorCanvas.Children().Append(indicator);
    }

    auto indicator = g_rightDragState.dropIndicator.try_as<Controls::Border>();

    // Hide the new group popup when showing regular indicator.
    if (g_rightDragState.newGroupPopup) {
        g_rightDragState.newGroupPopup.IsOpen(false);
    }

    // Use system accent color for the indicator.
    winrt::Windows::UI::ViewManagement::UISettings uiSettings;
    auto accentColor = uiSettings.GetColorValue(
        winrt::Windows::UI::ViewManagement::UIColorType::Accent);

    ReparentCanvasIfNeeded(indicatorParent);

    // Position the indicator relative to the target element.
    auto targetTransform = targetElement.TransformToVisual(indicatorParent);
    auto targetPos = targetTransform.TransformPoint({0, 0});

    // Get target task group from the target element for cross-group overlay.
    void* targetTaskGroup = nullptr;
    if (crossGroup) {
        targetTaskGroup = GetTaskGroupFromElement(targetElement);
    }

    if (crossGroup && targetTaskGroup) {
        // Cross-group: overlay the entire target group.
        // Find all TaskListButtons in the same group and calculate bounds.
        double minX = targetPos.X;
        double maxX = targetPos.X + targetElement.ActualWidth();
        double minY = targetPos.Y;
        double maxY = targetPos.Y + targetElement.ActualHeight();

        // Enumerate all elements to find TaskListButtons in the same group.
        auto allElements =
            Media::VisualTreeHelper::FindElementsInHostCoordinates(
                winrt::Windows::Foundation::Rect{
                    0, 0, static_cast<float>(taskbarFrame.ActualWidth()),
                    static_cast<float>(taskbarFrame.ActualHeight())},
                taskbarFrame);

        for (const auto& elem : allElements) {
            if (winrt::get_class_name(elem) != L"Taskbar.TaskListButton") {
                continue;
            }

            auto button = elem.try_as<FrameworkElement>();
            if (!button) {
                continue;
            }

            // Check if this button belongs to the same task group.
            if (GetTaskGroupFromElement(button) != targetTaskGroup) {
                continue;
            }

            // Include this button in the bounds.
            auto buttonTransform = button.TransformToVisual(indicatorParent);
            auto buttonPos = buttonTransform.TransformPoint({0, 0});

            minX = (std::min)(minX, static_cast<double>(buttonPos.X));
            maxX = (std::max)(maxX, static_cast<double>(buttonPos.X) +
                                        button.ActualWidth());
            minY = (std::min)(minY, static_cast<double>(buttonPos.Y));
            maxY = (std::max)(maxY, static_cast<double>(buttonPos.Y) +
                                        button.ActualHeight());
        }

        auto overlayColor = accentColor;
        overlayColor.A = 80;  // Semi-transparent.
        indicator.Background(Media::SolidColorBrush(overlayColor));
        indicator.Width(maxX - minX);
        indicator.Height(maxY - minY);
        indicator.CornerRadius(CornerRadiusHelper::FromUniformRadius(4));

        Controls::Canvas::SetLeft(indicator, minX);
        Controls::Canvas::SetTop(indicator, minY);
    } else {
        // Same group: vertical line indicator.
        indicator.Background(Media::SolidColorBrush(accentColor));
        indicator.Width(4);
        indicator.Height(targetElement.ActualHeight());
        indicator.CornerRadius(CornerRadiusHelper::FromUniformRadius(2));

        double indicatorX = targetPos.X;
        if (insertAfter) {
            indicatorX += targetElement.ActualWidth();
        }

        Controls::Canvas::SetLeft(indicator, indicatorX - 2);
        Controls::Canvas::SetTop(indicator, targetPos.Y);
    }

    indicator.Visibility(Visibility::Visible);
    g_dropIndicatorCanvas.Visibility(Visibility::Visible);
}

void ShowNewGroupIndicator(FrameworkElement taskbarFrame,
                           double cursorX,
                           double cursorY) {
    if (!taskbarFrame) {
        HideDropIndicator();
        return;
    }

    // Create the popup if it doesn't exist.
    if (!g_rightDragState.newGroupPopup) {
        Controls::Border border;
        border.IsHitTestVisible(false);
        border.Padding(ThicknessHelper::FromLengths(8, 4, 8, 4));
        border.CornerRadius(CornerRadiusHelper::FromUniformRadius(4));

        Controls::TextBlock textBlock;
        textBlock.Text(L"New group");
        textBlock.FontSize(12);
        border.Child(textBlock);

        Controls::Primitives::Popup popup;
        popup.IsHitTestVisible(false);
        popup.Child(border);

        g_rightDragState.newGroupPopup = popup;
    }

    auto popup = g_rightDragState.newGroupPopup;
    auto indicator = popup.Child().try_as<Controls::Border>();

    // Use system accent color for the indicator.
    winrt::Windows::UI::ViewManagement::UISettings uiSettings;
    auto accentColor = uiSettings.GetColorValue(
        winrt::Windows::UI::ViewManagement::UIColorType::Accent);
    indicator.Background(Media::SolidColorBrush(accentColor));

    // Set text color to white for contrast with accent background.
    if (auto textBlock = indicator.Child().try_as<Controls::TextBlock>()) {
        textBlock.Foreground(
            Media::SolidColorBrush(winrt::Windows::UI::Colors::White()));
    }

    // Hide the regular drop indicator.
    if (g_rightDragState.dropIndicator) {
        g_rightDragState.dropIndicator.Visibility(Visibility::Collapsed);
    }
    if (g_dropIndicatorCanvas) {
        g_dropIndicatorCanvas.Visibility(Visibility::Collapsed);
    }

    // Set XamlRoot from taskbarFrame (required for popup to work).
    popup.XamlRoot(taskbarFrame.XamlRoot());

    // Position near the cursor and show.
    popup.HorizontalOffset(cursorX + 10);
    popup.VerticalOffset(cursorY - 10);
    popup.IsOpen(true);
}

void CleanupDropIndicator() {
    if (g_rightDragState.dropIndicator) {
        if (g_dropIndicatorCanvas) {
            UINT index = 0;
            if (g_dropIndicatorCanvas.Children().IndexOf(
                    g_rightDragState.dropIndicator, index)) {
                g_dropIndicatorCanvas.Children().RemoveAt(index);
            }
        }
        g_rightDragState.dropIndicator = nullptr;
    }

    if (g_rightDragState.newGroupPopup) {
        g_rightDragState.newGroupPopup.IsOpen(false);
        g_rightDragState.newGroupPopup = nullptr;
    }

    if (g_dropIndicatorCanvas) {
        auto parent = Media::VisualTreeHelper::GetParent(g_dropIndicatorCanvas);
        if (auto panel = parent.try_as<Controls::Panel>()) {
            UINT index = 0;
            if (panel.Children().IndexOf(g_dropIndicatorCanvas, index)) {
                panel.Children().RemoveAt(index);
            }
        }
        g_dropIndicatorCanvas = nullptr;
    }
}

void ClearRightDragState() {
    CleanupDropIndicator();
    g_rightDragState.sourceElement = nullptr;
    g_rightDragState.targetElement = nullptr;
    g_rightDragState.insertAfter = false;
    g_rightDragState.newGroup = false;
}

FrameworkElement FindTaskbarFrameAncestor(UIElement element) {
    auto parent = Media::VisualTreeHelper::GetParent(element);
    while (parent) {
        if (winrt::get_class_name(parent) == L"Taskbar.TaskbarFrame") {
            return parent.try_as<FrameworkElement>();
        }
        parent = Media::VisualTreeHelper::GetParent(parent);
    }
    return nullptr;
}

struct DragTargetInfo {
    FrameworkElement element = nullptr;
    bool crossGroup = false;
    bool insertAfter = false;
};

DragTargetInfo FindDragTargetAtPosition(
    FrameworkElement taskbarFrame,
    winrt::Windows::Foundation::Point position,
    UIElement sourceElement,
    void* sourceTaskGroup) {
    DragTargetInfo result;

    auto elementsAtPos = Media::VisualTreeHelper::FindElementsInHostCoordinates(
        position, taskbarFrame);

    for (const auto& child : elementsAtPos) {
        if (winrt::get_class_name(child) != L"Taskbar.TaskListButton") {
            continue;
        }

        auto targetElement = child.try_as<FrameworkElement>();
        if (!targetElement) {
            continue;
        }

        void* targetWindowsUdkTaskItem =
            GetWindowsUdkTaskItemFromTaskListButton(targetElement);
        if (!targetWindowsUdkTaskItem) {
            continue;
        }

        auto targetResult =
            GetTaskGroupAndItemFromWindowsUdkTaskItem(targetWindowsUdkTaskItem);
        if (!targetResult.taskGroup || !targetResult.taskItem) {
            continue;
        }

        // Determine insert position based on cursor.
        auto targetTransform = targetElement.TransformToVisual(taskbarFrame);
        auto targetPos = targetTransform.TransformPoint({0, 0});
        double targetCenterX = targetPos.X + targetElement.ActualWidth() / 2;
        result.insertAfter = position.X > targetCenterX;

        // Check if same or cross group.
        bool isSameGroup = (targetResult.taskGroup == sourceTaskGroup);
        if (targetElement == sourceElement && isSameGroup) {
            // Skip source element in same group.
            result.element = nullptr;
        } else {
            result.element = targetElement;
            result.crossGroup = !isSameGroup;
        }

        break;
    }

    return result;
}

bool IsInNewGroupArea(FrameworkElement taskbarFrame,
                      winrt::Windows::Foundation::Point position) {
    double maxRightEdge = 0;
    double minY = taskbarFrame.ActualHeight();
    double maxY = 0;

    auto allElements = Media::VisualTreeHelper::FindElementsInHostCoordinates(
        winrt::Windows::Foundation::Rect{
            0, 0, static_cast<float>(taskbarFrame.ActualWidth()),
            static_cast<float>(taskbarFrame.ActualHeight())},
        taskbarFrame);

    for (const auto& elem : allElements) {
        if (winrt::get_class_name(elem) != L"Taskbar.TaskListButton") {
            continue;
        }

        auto button = elem.try_as<FrameworkElement>();
        if (!button) {
            continue;
        }

        auto buttonTransform = button.TransformToVisual(taskbarFrame);
        auto buttonPos = buttonTransform.TransformPoint({0, 0});

        double rightEdge = buttonPos.X + button.ActualWidth();
        if (rightEdge > maxRightEdge) {
            maxRightEdge = rightEdge;
        }

        minY = (std::min)(minY, static_cast<double>(buttonPos.Y));
        maxY = (std::max)(maxY, static_cast<double>(buttonPos.Y) +
                                    button.ActualHeight());
    }

    return position.X > maxRightEdge && maxRightEdge > 0 &&
           position.Y >= minY && position.Y <= maxY;
}

using TaskListButton_OnPointerMoved_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskListButton_OnPointerMoved_t TaskListButton_OnPointerMoved_Original;
int WINAPI TaskListButton_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskListButton_OnPointerMoved_Original(pThis, pArgs);
    };

    UIElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<UIElement>(), winrt::put_abi(element));
    if (!element) {
        return original();
    }

    if (winrt::get_class_name(element) != L"Taskbar.TaskListButton") {
        return original();
    }

    auto sourceElement = g_rightDragState.sourceElement.get();
    if (!sourceElement) {
        return original();
    }

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return original();
    }

    auto props = args.GetCurrentPoint(element).Properties();
    if (!props.IsRightButtonPressed() || props.IsLeftButtonPressed()) {
        ClearRightDragState();
        return original();
    }

    FrameworkElement taskbarFrame = FindTaskbarFrameAncestor(element);
    if (!taskbarFrame) {
        return original();
    }

    auto pointerPos = args.GetCurrentPoint(element).Position();
    auto transform = element.TransformToVisual(taskbarFrame);
    auto cursorPos = transform.TransformPoint(pointerPos);

    void* sourceTaskGroup = GetTaskGroupFromElement(sourceElement);
    auto target = FindDragTargetAtPosition(taskbarFrame, cursorPos,
                                           sourceElement, sourceTaskGroup);

    if (target.element) {
        g_rightDragState.targetElement = target.element;
        g_rightDragState.insertAfter = target.insertAfter;
        g_rightDragState.newGroup = false;
        ShowDropIndicator(target.element, taskbarFrame, target.insertAfter,
                          target.crossGroup);
    } else if (IsInNewGroupArea(taskbarFrame, cursorPos)) {
        g_rightDragState.targetElement = nullptr;
        g_rightDragState.newGroup = true;
        ShowNewGroupIndicator(taskbarFrame, cursorPos.X, cursorPos.Y);
    } else {
        g_rightDragState.targetElement = nullptr;
        g_rightDragState.newGroup = false;
        HideDropIndicator();
    }

    return original();
}

using TaskListButton_OnPointerPressed_t = int(WINAPI*)(void* pThis,
                                                       void* pArgs);
TaskListButton_OnPointerPressed_t TaskListButton_OnPointerPressed_Original;
int WINAPI TaskListButton_OnPointerPressed_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskListButton_OnPointerPressed_Original(pThis, pArgs);
    };

    UIElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<UIElement>(), winrt::put_abi(element));
    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskListButton") {
        return original();
    }

    Wh_Log(L">");

    // Get pointer event args.
    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return original();
    }

    auto props = args.GetCurrentPoint(element).Properties();
    bool isRightButtonPressed = props.IsRightButtonPressed();
    bool isLeftButtonPressed = props.IsLeftButtonPressed();

    // Initiate right-drag if right button pressed without left button, and the
    // element is a valid task item.
    if (isRightButtonPressed && !isLeftButtonPressed &&
        GetTaskGroupFromElement(element)) {
        Wh_Log(L"Starting right-drag");
        g_rightDragState.sourceElement = element;
    }

    return original();
}

using TaskListButton_OnPointerReleased_t = int(WINAPI*)(void* pThis,
                                                        void* pArgs);
TaskListButton_OnPointerReleased_t TaskListButton_OnPointerReleased_Original;
int WINAPI TaskListButton_OnPointerReleased_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskListButton_OnPointerReleased_Original(pThis, pArgs);
    };

    winrt::Windows::Foundation::IInspectable element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(
            winrt::guid_of<winrt::Windows::Foundation::IInspectable>(),
            winrt::put_abi(element));
    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskListButton") {
        return original();
    }

    // Return early if no active drag.
    auto sourceElement = g_rightDragState.sourceElement.get();
    if (!sourceElement) {
        return original();
    }

    Wh_Log(L">");

    // Get source task item/group.
    void* sourceWindowsUdkTaskItem =
        GetWindowsUdkTaskItemFromTaskListButton(sourceElement);
    if (!sourceWindowsUdkTaskItem) {
        ClearRightDragState();
        return original();
    }

    auto sourceResult =
        GetTaskGroupAndItemFromWindowsUdkTaskItem(sourceWindowsUdkTaskItem);
    void* sourceTaskItem = sourceResult.taskItem;
    void* sourceTaskGroup = sourceResult.taskGroup;

    // Get target element and retrieve its task item/group.
    auto targetElement = g_rightDragState.targetElement.get();
    void* targetTaskItem = nullptr;
    void* targetTaskGroup = nullptr;
    if (targetElement) {
        void* targetWindowsUdkTaskItem =
            GetWindowsUdkTaskItemFromTaskListButton(targetElement);
        if (targetWindowsUdkTaskItem) {
            auto targetResult = GetTaskGroupAndItemFromWindowsUdkTaskItem(
                targetWindowsUdkTaskItem);
            targetTaskItem = targetResult.taskItem;
            targetTaskGroup = targetResult.taskGroup;
        }
    }

    // Check if we have an active drag with valid source and target.
    if (sourceTaskItem && targetTaskItem) {
        if (sourceTaskGroup == targetTaskGroup) {
            // Same group - reorder within group.
            Wh_Log(
                L"Completing right-drag: moving %p to %p (insertAfter=%d) in "
                L"group %p",
                sourceTaskItem, targetTaskItem, g_rightDragState.insertAfter,
                sourceTaskGroup);

            MoveTaskInGroup(sourceTaskGroup, sourceTaskItem, targetTaskItem,
                            g_rightDragState.insertAfter);
        } else {
            // Different group - change window AppID.
            HWND hWnd = GetWindowFromTaskItem(sourceTaskItem);
            PCWSTR targetAppId = CTaskGroup_GetAppID(targetTaskGroup);
            if (hWnd && targetAppId) {
                Wh_Log(L"Changing AppID of window %p to %s", hWnd, targetAppId);
                WndSetAppId(hWnd, targetAppId);
            }
        }
    } else if (sourceTaskItem && g_rightDragState.newGroup) {
        // Create new group - generate unique AppID.
        HWND hWnd = GetWindowFromTaskItem(sourceTaskItem);
        if (hWnd) {
            GUID guid;
            if (SUCCEEDED(CoCreateGuid(&guid))) {
                WCHAR guidStr[40];
                StringFromGUID2(guid, guidStr, ARRAYSIZE(guidStr));
                WCHAR uniqueAppId[ARRAYSIZE(WH_MOD_ID) + ARRAYSIZE(guidStr)];
                swprintf_s(uniqueAppId, L"%s%s", WH_MOD_ID, guidStr);
                Wh_Log(L"Creating new group for window %p with AppID %s", hWnd,
                       uniqueAppId);
                WndSetAppId(hWnd, uniqueAppId);
            }
        }
    }

    // Clear the drag state.
    ClearRightDragState();

    return original();
}

using DPA_GetPtr_t = decltype(&DPA_GetPtr);
DPA_GetPtr_t DPA_GetPtr_Original;
PVOID WINAPI DPA_GetPtr_Hook(HDPA hdpa, INT_PTR i) {
    if (g_getPtr_captureForThreadId == GetCurrentThreadId()) {
        g_getPtr_lastHdpa = hdpa;
    }

    return DPA_GetPtr_Original(hdpa, i);
}

bool HookTaskbarSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskListWnd::`vftable'{for `ITaskListUI'})"},
            &CTaskListWnd_vftable_ITaskListUI,
        },
        {
            {LR"(public: virtual int __cdecl CTaskGroup::GetNumItems(void))"},
            &CTaskGroup_GetNumItems,
        },
        {
            {LR"(public: virtual unsigned short const * __cdecl CTaskGroup::GetAppID(void))"},
            &CTaskGroup_GetAppID,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            &CWindowTaskItem_GetWindow,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetWindow(void))"},
            &CImmersiveTaskItem_GetWindow,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            &CImmersiveTaskItem_vftable,
        },
        {
            {LR"(protected: struct ITaskBtnGroup * __cdecl CTaskListWnd::_GetTBGroupFromGroup(struct ITaskGroup *,int *))"},
            &CTaskListWnd__GetTBGroupFromGroup,
        },
        {
            {LR"(public: virtual enum eTBGROUPTYPE __cdecl CTaskBtnGroup::GetGroupType(void))"},
            &CTaskBtnGroup_GetGroupType,
        },
        {
            {LR"(public: virtual int __cdecl CTaskBtnGroup::IndexOfTaskItem(struct ITaskItem *))"},
            &CTaskBtnGroup_IndexOfTaskItem,
        },
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::TaskInclusionChanged(struct ITaskGroup *,struct ITaskItem *))"},
            &CTaskListWnd_TaskInclusionChanged,
        },
        {
            {LR"(public: virtual bool __cdecl TaskItemFilter::IsTaskAllowed(struct ITaskItem *))"},
            &TaskItemFilter_IsTaskAllowed_Original,
            TaskItemFilter_IsTaskAllowed_Hook,
        },
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
            &CTaskListWnd_HandleClick_Original,
            CTaskListWnd_HandleClick_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskItem,struct winrt::WindowsUdk::UI::Shell::ITaskItem>::ReportClicked(void *))"},
            &TaskItem_ReportClicked_Original,
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(struct winrt::Taskbar::TaskListWindowViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListWindowViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListWindowViewModel_Original,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_TaskItem(void * *))"},
            &TaskListWindowViewModel_get_TaskItem_Original,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"},
            &TaskListButton_OnPointerMoved_Original,
            TaskListButton_OnPointerMoved_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerPressed(void *))"},
            &TaskListButton_OnPointerPressed_Original,
            TaskListButton_OnPointerPressed_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerReleased(void *))"},
            &TaskListButton_OnPointerReleased_Original,
            TaskListButton_OnPointerReleased_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

void LoadSettings() {
    // None for now...
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarSymbols()) {
        return FALSE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    WindhawkUtils::SetFunctionHook(DPA_GetPtr, DPA_GetPtr_Hook,
                                   &DPA_GetPtr_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        RunFromWindowThread(
            hTaskbarWnd, [](void*) { ClearRightDragState(); }, nullptr);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        RunFromWindowThread(
            hTaskbarWnd, [](void*) { ClearRightDragState(); }, nullptr);
    }

    LoadSettings();
}
