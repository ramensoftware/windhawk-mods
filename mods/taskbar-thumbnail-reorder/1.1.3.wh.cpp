// ==WindhawkMod==
// @id              taskbar-thumbnail-reorder
// @name            Taskbar Thumbnail Reorder
// @description     Reorder taskbar thumbnails with the left mouse button
// @version         1.1.3
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lversion
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
# Taskbar Thumbnail Reorder

Reorder taskbar thumbnails with the left mouse button.

Only Windows 10 64-bit and Windows 11 are supported. For older Windows versions
check out [7+ Taskbar Tweaker](https://tweaker.ramensoftware.com/).

**Note:** To customize the old taskbar on Windows 11 (if using ExplorerPatcher
or a similar tool), enable the relevant option in the mod's settings.

## Known limitations

* When the new thumbnail previews implementation in Windows 11 is used, and when
  labels are shown, each reordering operation causes the thumbnail previews to
  be closed. This seems to be a bug in Windows 11, it also happens when closing
  items with the x button or via middle click. I couldn't find a workaround,
  hopefully it will be fixed by Microsoft in one of the next updates.

![demonstration](https://i.imgur.com/wGGe2RS.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    ExplorerPatcher or a similar tool).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <commctrl.h>
#include <psapi.h>

#include <atomic>
#include <functional>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

using namespace winrt::Windows::UI::Xaml;

struct {
    bool oldTaskbarOnWin11;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

int g_thumbDraggedIndex = -1;
bool g_thumbDragDone;
bool g_taskItemFilterDisallowAll;
DWORD g_taskInclusionChangeLastTickCount;

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

// XAML thumbnails implementation variables.

struct ThumbnailTaskItemMapping {
    winrt::weak_ref<winrt::Windows::Foundation::IInspectable> thumbnail;
    void* taskGroup;
    void* taskItem;
};

std::vector<ThumbnailTaskItemMapping> g_thumbnailTaskItemMapping;

bool g_inHoverFlyoutModel_TargetItemKey;

winrt::weak_ref<winrt::Windows::Foundation::IInspectable>
    g_TaskGroup_Thumbnails;

bool g_reorderingXamlThumbnails;

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
    }

    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

void* QueryViaVtable(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) {
        ptr = (void**)ptr + 1;
    }
    return ptr;
}

void* QueryViaVtableBackwards(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) {
        ptr = (void**)ptr - 1;
    }
    return ptr;
}

void* Query_CTaskListThumbnailWnd_IExtendedUISwitcher(
    void* taskListThumbnailWnd) {
    // This has been correct at least since the first Win10 versions, and is
    // unlikely to ever change.
    return (void**)taskListThumbnailWnd + 2;
}

void* CTaskListWnd_vftable_ITaskListUI;

using CTaskListThumbnailWnd_GetHoverIndex_t = int(WINAPI*)(void* pThis);
CTaskListThumbnailWnd_GetHoverIndex_t CTaskListThumbnailWnd_GetHoverIndex;

using CTaskListThumbnailWnd__GetTaskItem_t = void*(WINAPI*)(void* pThis,
                                                            int index);
CTaskListThumbnailWnd__GetTaskItem_t CTaskListThumbnailWnd__GetTaskItem;

using CTaskListThumbnailWnd_GetTaskGroup_t = void*(WINAPI*)(void* pThis);
CTaskListThumbnailWnd_GetTaskGroup_t CTaskListThumbnailWnd_GetTaskGroup;

using CTaskListThumbnailWnd_TaskReordered_t = void(WINAPI*)(void* pThis,
                                                            void* taskItem);
CTaskListThumbnailWnd_TaskReordered_t CTaskListThumbnailWnd_TaskReordered;

using CTaskGroup_GetNumItems_t = int(WINAPI*)(PVOID pThis);
CTaskGroup_GetNumItems_t CTaskGroup_GetNumItems;

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
    Wh_Log(L">");

    if (g_taskItemFilterDisallowAll) {
        return false;
    }

    return TaskItemFilter_IsTaskAllowed_Original(pThis, pTaskItem);
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

    if (g_winVersion <= WinVersion::Win10) {
        InvalidateRect(hMMTaskListWnd, nullptr, FALSE);
    } else {
        g_taskInclusionChangeLastTickCount = GetTickCount();

        HWND hThumbnailWnd = GetCapture();
        if (hThumbnailWnd) {
            WCHAR szClassName[32];
            if (GetClassName(hThumbnailWnd, szClassName,
                             ARRAYSIZE(szClassName)) == 0 ||
                _wcsicmp(szClassName, L"TaskListThumbnailWnd") != 0) {
                hThumbnailWnd = nullptr;
            }
        }

        if (hThumbnailWnd) {
            SendMessage(hThumbnailWnd, WM_SETREDRAW, FALSE, 0);
        }

        g_taskItemFilterDisallowAll = true;

        void* pThis_ITaskListUI = QueryViaVtable(
            lpMMTaskListLongPtr, CTaskListWnd_vftable_ITaskListUI);

        CTaskListWnd_TaskInclusionChanged(pThis_ITaskListUI, taskGroup,
                                          taskItemFrom);

        g_taskItemFilterDisallowAll = false;

        CTaskListWnd_TaskInclusionChanged(pThis_ITaskListUI, taskGroup,
                                          taskItemFrom);

        if (hThumbnailWnd) {
            SendMessage(hThumbnailWnd, WM_SETREDRAW, TRUE, 0);
            RedrawWindow(
                hThumbnailWnd, nullptr, nullptr,
                RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
        }
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

bool MoveTaskInGroup(void* taskGroup, void* taskItemFrom, void* taskItemTo) {
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

bool MoveItemsFromThumbnail(void* lpMMThumbnailLongPtr,
                            int indexFrom,
                            int indexTo) {
    Wh_Log(L"> %d->%d", indexFrom, indexTo);

    void* taskItemFrom =
        CTaskListThumbnailWnd__GetTaskItem(lpMMThumbnailLongPtr, indexFrom);
    if (!taskItemFrom) {
        Wh_Log(L"Failed to get task item");
        return false;
    }

    void* taskItemTo =
        CTaskListThumbnailWnd__GetTaskItem(lpMMThumbnailLongPtr, indexTo);
    if (!taskItemTo) {
        Wh_Log(L"Failed to get task item");
        return false;
    }

    if (!MoveTaskInGroup(
            CTaskListThumbnailWnd_GetTaskGroup(lpMMThumbnailLongPtr),
            taskItemFrom, taskItemTo)) {
        Wh_Log(L"Failed to move task item");
        return false;
    }

    CTaskListThumbnailWnd_TaskReordered(lpMMThumbnailLongPtr, taskItemFrom);

    return true;
}

using CTaskListThumbnailWnd_v_WndProc_t = LRESULT(
    WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CTaskListThumbnailWnd_v_WndProc_t CTaskListThumbnailWnd_v_WndProc_Original;
LRESULT WINAPI CTaskListThumbnailWnd_v_WndProc_Hook(void* pThis,
                                                    HWND hWnd,
                                                    UINT Msg,
                                                    WPARAM wParam,
                                                    LPARAM lParam) {
    LRESULT result = 0;
    bool processed = false;

    auto OriginalProc = [pThis](HWND hWnd, UINT Msg, WPARAM wParam,
                                LPARAM lParam) {
        return CTaskListThumbnailWnd_v_WndProc_Original(pThis, hWnd, Msg,
                                                        wParam, lParam);
    };

    switch (Msg) {
        case WM_LBUTTONDOWN:
            result = OriginalProc(hWnd, Msg, wParam, lParam);

            {
                void* lpMMThumbnailLongPtr = (void*)GetWindowLongPtr(hWnd, 0);
                void* lpMMThumbnailLongPtr_IExtendedUISwitcher =
                    Query_CTaskListThumbnailWnd_IExtendedUISwitcher(
                        lpMMThumbnailLongPtr);
                int trackedIndex = CTaskListThumbnailWnd_GetHoverIndex(
                    lpMMThumbnailLongPtr_IExtendedUISwitcher);
                if (trackedIndex >= 0) {
                    g_thumbDragDone = false;
                    g_thumbDraggedIndex = trackedIndex;
                    SetCapture(hWnd);
                }
            }
            break;

        case WM_MOUSEMOVE:
            result = OriginalProc(hWnd, Msg, wParam, lParam);

            if (GetCapture() == hWnd &&
                GetTickCount() - g_taskInclusionChangeLastTickCount > 60) {
                void* lpMMThumbnailLongPtr = (void*)GetWindowLongPtr(hWnd, 0);
                void* lpMMThumbnailLongPtr_IExtendedUISwitcher =
                    Query_CTaskListThumbnailWnd_IExtendedUISwitcher(
                        lpMMThumbnailLongPtr);
                int trackedIndex = CTaskListThumbnailWnd_GetHoverIndex(
                    lpMMThumbnailLongPtr_IExtendedUISwitcher);
                if (trackedIndex >= 0 && trackedIndex != g_thumbDraggedIndex &&
                    MoveItemsFromThumbnail(lpMMThumbnailLongPtr,
                                           g_thumbDraggedIndex, trackedIndex)) {
                    g_thumbDraggedIndex = trackedIndex;
                    g_thumbDragDone = true;
                }
            }
            break;

        case WM_LBUTTONUP:
            if (GetCapture() == hWnd) {
                ReleaseCapture();
                if (g_thumbDragDone) {
                    result = DefWindowProc(hWnd, Msg, wParam, lParam);
                    processed = true;
                }
            }

            if (!processed) {
                result = OriginalProc(hWnd, Msg, wParam, lParam);
            }
            break;

        case WM_TIMER:
            // Aero peek window of hovered thumbnail.
            if (wParam == 2006) {
                if (GetCapture() == hWnd) {
                    KillTimer(hWnd, wParam);
                    result = DefWindowProc(hWnd, Msg, wParam, lParam);
                    processed = true;
                }
            }

            if (!processed) {
                result = OriginalProc(hWnd, Msg, wParam, lParam);
            }
            break;

        default:
            result = OriginalProc(hWnd, Msg, wParam, lParam);
            break;
    }

    return result;
}

using TaskItemThumbnail_TaskItemThumbnail_t = void*(WINAPI*)(void* param1,
                                                             void* param2,
                                                             void* taskGroup,
                                                             void* taskItem,
                                                             void* taskListUi,
                                                             void* param6,
                                                             void* param7,
                                                             bool param8);
TaskItemThumbnail_TaskItemThumbnail_t
    TaskItemThumbnail_TaskItemThumbnail_Original;
void* WINAPI TaskItemThumbnail_TaskItemThumbnail_Hook(void* param1,
                                                      void* param2,
                                                      void* taskGroup,
                                                      void* taskItem,
                                                      void* taskListUi,
                                                      void* param6,
                                                      void* param7,
                                                      bool param8) {
    Wh_Log(L">");

    void* result = TaskItemThumbnail_TaskItemThumbnail_Original(
        param1, param2, taskGroup, taskItem, taskListUi, param6, param7,
        param8);

    winrt::Windows::Foundation::IInspectable obj = nullptr;
    ((IUnknown*)result + 2)
        ->QueryInterface(
            winrt::guid_of<winrt::Windows::Foundation::IInspectable>(),
            winrt::put_abi(obj));

    g_thumbnailTaskItemMapping.push_back(
        ThumbnailTaskItemMapping{obj, taskGroup, taskItem});

    return result;
}

using TaskGroup_Thumbnails_t = void*(WINAPI*)(void* pThis, void* param1);
TaskGroup_Thumbnails_t TaskGroup_Thumbnails_Original;
void* WINAPI TaskGroup_Thumbnails_Hook(void* pThis, void* param1) {
    void* result = TaskGroup_Thumbnails_Original(pThis, param1);

    if (g_inHoverFlyoutModel_TargetItemKey) {
        Wh_Log(L">");

        winrt::Windows::Foundation::IInspectable obj = nullptr;
        (*(IUnknown**)result)
            ->QueryInterface(
                winrt::guid_of<winrt::Windows::Foundation::IInspectable>(),
                winrt::put_abi(obj));

        bool assign = false;

        auto currentObj = g_TaskGroup_Thumbnails.get();
        if (!currentObj) {
            Wh_Log(L"No previous thumbnails object");
            assign = true;
        } else if (currentObj != obj) {
            Wh_Log(L"Different previous thumbnails object");
            assign = true;
        } else {
            Wh_Log(L"Same previous thumbnails object");
        }

        if (assign) {
            g_TaskGroup_Thumbnails = obj;

            // Remove invalid weak pointers.
            std::erase_if(g_thumbnailTaskItemMapping, [](const auto& item) {
                return !item.thumbnail.get();
            });
        }
    }

    return result;
}

using TaskItemThumbnail_Size_t = int(WINAPI*)(void* pThis);
TaskItemThumbnail_Size_t TaskItemThumbnail_Size_Original;

using TaskItemThumbnail_GetAt_t = void*(WINAPI*)(void* pThis,
                                                 void** result,
                                                 int index);
TaskItemThumbnail_GetAt_t TaskItemThumbnail_GetAt_Original;

using CTaskListWnd_HandleExtendedUIClick_t =
    HRESULT(WINAPI*)(void* pThis,
                     void* taskGroup,
                     void* taskItem,
                     void* launcherOptions);
CTaskListWnd_HandleExtendedUIClick_t
    CTaskListWnd_HandleExtendedUIClick_Original;
HRESULT WINAPI CTaskListWnd_HandleExtendedUIClick_Hook(void* pThis,
                                                       void* taskGroup,
                                                       void* taskItem,
                                                       void* launcherOptions) {
    Wh_Log(L">");

    if (g_reorderingXamlThumbnails) {
        g_reorderingXamlThumbnails = false;
        return S_OK;
    }

    return CTaskListWnd_HandleExtendedUIClick_Original(
        pThis, taskGroup, taskItem, launcherOptions);
}

using HoverFlyoutModel_TargetItemKey_t = void(WINAPI*)(void* pThis,
                                                       void* param1);
HoverFlyoutModel_TargetItemKey_t HoverFlyoutModel_TargetItemKey_Original;
void WINAPI HoverFlyoutModel_TargetItemKey_Hook(void* pThis, void* param1) {
    Wh_Log(L">");

    g_inHoverFlyoutModel_TargetItemKey = true;

    HoverFlyoutModel_TargetItemKey_Original(pThis, param1);

    g_inHoverFlyoutModel_TargetItemKey = false;
}

bool IsPointerInsideElement(const UIElement& element,
                            const Input::PointerRoutedEventArgs& args) {
    auto pointerPos = args.GetCurrentPoint(element).Position();

    float width = element.RenderSize().Width;
    float height = element.RenderSize().Height;

    return pointerPos.X >= 0 && pointerPos.X < width && pointerPos.Y >= 0 &&
           pointerPos.Y < height;
}

void MoveItemsFromXAMLThumbnail(int indexFrom, int indexTo) {
    Wh_Log(L"Moving from %d to %d", indexFrom, indexTo);

    auto thumbnails = g_TaskGroup_Thumbnails.get();
    if (!thumbnails) {
        Wh_Log(L"Thumbnails object isn't valid");
        return;
    }

    auto* thumbnailsPtr = winrt::get_abi(thumbnails);

    int thumbnailsSize = TaskItemThumbnail_Size_Original(&thumbnailsPtr);

    if (indexFrom < 0 || indexFrom >= thumbnailsSize) {
        Wh_Log(L"Invalid indexFrom value");
        return;
    }

    if (indexTo < 0 || indexTo >= thumbnailsSize) {
        Wh_Log(L"Invalid indexTo value");
        return;
    }

    winrt::com_ptr<IUnknown> from;
    TaskItemThumbnail_GetAt_Original(&thumbnailsPtr, from.put_void(),
                                     indexFrom);

    winrt::com_ptr<IUnknown> to;
    TaskItemThumbnail_GetAt_Original(&thumbnailsPtr, to.put_void(), indexTo);

    void* taskItemFrom = nullptr;
    void* taskGroupFrom = nullptr;
    void* taskItemTo = nullptr;
    void* taskGroupTo = nullptr;

    for (const auto& iter : g_thumbnailTaskItemMapping) {
        auto thumbnail = iter.thumbnail.get();
        if (!thumbnail) {
            continue;
        }

        void* thumbnailPtr = winrt::get_abi(thumbnail);

        if (thumbnailPtr == from.get()) {
            taskItemFrom = iter.taskItem;
            taskGroupFrom = iter.taskGroup;
        }

        if (thumbnailPtr == to.get()) {
            taskItemTo = iter.taskItem;
            taskGroupTo = iter.taskGroup;
        }
    }

    if (!taskItemFrom || !taskGroupFrom || !taskItemTo || !taskGroupTo) {
        Wh_Log(L"Task item/group not found");
        return;
    }

    if (taskGroupFrom != taskGroupTo) {
        Wh_Log(L"Task group differs");
        return;
    }

    if (!MoveTaskInGroup(taskGroupFrom, taskItemFrom, taskItemTo)) {
        Wh_Log(L"Failed to move task item");
        return;
    }
}

using TaskItemThumbnailList_OnPointerMoved_t = int(WINAPI*)(void* pThis,
                                                            void* pArgs);
TaskItemThumbnailList_OnPointerMoved_t
    TaskItemThumbnailList_OnPointerMoved_Original;
int WINAPI TaskItemThumbnailList_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskItemThumbnailList_OnPointerMoved_Original(pThis, pArgs);
    };

    if (!GetCapture()) {
        g_reorderingXamlThumbnails = false;
        return original();
    }

    if (GetTickCount() - g_taskInclusionChangeLastTickCount <= 60) {
        return original();
    }

    Wh_Log(L">");

    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));

    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);
    Wh_Log(L"%s", className.c_str());

    FrameworkElement taskItemThumbnailListRepeater = nullptr;

    if (className == L"Taskbar.TaskItemThumbnailList") {
        taskItemThumbnailListRepeater =
            FindChildByName(element, L"TaskItemThumbnailListRepeater");
    } else if (className == L"Taskbar.TaskItemThumbnailScrollableList") {
        FrameworkElement child = element;
        if ((child = FindChildByName(
                 child, L"TaskItemThumbnailScrollableListScrollViewer")) &&
            (child = FindChildByName(child, L"Root")) &&
            (child = FindChildByClassName(child,
                                          L"Windows.UI.Xaml.Controls.Grid")) &&
            (child = FindChildByName(child, L"ScrollContentPresenter")) &&
            (child =
                 FindChildByName(child, L"TaskItemThumbnailListRepeater"))) {
            taskItemThumbnailListRepeater = child;
        }
    } else {
        return original();
    }

    if (!taskItemThumbnailListRepeater) {
        Wh_Log(L"TaskItemThumbnailListRepeater not found");
        return original();
    }

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return original();
    }

    int positionPressed = 0;
    int positionHovered = 0;

    EnumChildElements(
        taskItemThumbnailListRepeater,
        [&args, &positionPressed, &positionHovered](FrameworkElement child) {
            auto className = winrt::get_class_name(child);
            if (className != L"Taskbar.TaskItemThumbnailView") {
                Wh_Log(L"Unexpected element of class %s", className.c_str());
                return true;
            }

            auto grid =
                FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid");
            if (!grid) {
                Wh_Log(L"Element has no grid child");
                return true;
            }

            if (positionPressed == 0) {
                VisualState currentState = nullptr;

                for (const auto& v :
                     VisualStateManager::GetVisualStateGroups(grid)) {
                    if (v.Name() == L"CommonStates") {
                        currentState = v.CurrentState();
                        break;
                    }
                }

                if (currentState) {
                    auto currentStateName = currentState.Name();
                    if (currentStateName == L"Pressed" ||
                        currentStateName == L"RequestingAttentionPressed") {
                        positionPressed =
                            Automation::AutomationProperties::GetPositionInSet(
                                child);
                    }
                }
            }

            if (positionHovered == 0 && IsPointerInsideElement(child, args)) {
                positionHovered =
                    Automation::AutomationProperties::GetPositionInSet(child);
            }

            return positionPressed != 0 && positionHovered != 0;
        });

    if (positionPressed != 0 && positionHovered != 0 &&
        positionPressed != positionHovered) {
        g_reorderingXamlThumbnails = true;
        MoveItemsFromXAMLThumbnail(positionPressed - 1, positionHovered - 1);
    }

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
    HMODULE module;
    if (g_winVersion <= WinVersion::Win10) {
        module = GetModuleHandle(nullptr);
    } else {
        module = LoadLibraryEx(L"taskbar.dll", nullptr,
                               LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!module) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return false;
        }
    }

    // Taskbar.dll, explorer.exe
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] =  //
        {
            {
                {LR"(const CTaskListWnd::`vftable'{for `ITaskListUI'})"},
                &CTaskListWnd_vftable_ITaskListUI,
            },
            {
                {LR"(public: virtual int __cdecl CTaskListThumbnailWnd::GetHoverIndex(void)const )"},
                &CTaskListThumbnailWnd_GetHoverIndex,
            },
            {
                {
                    // Windows 11.
                    LR"(private: struct ITaskItem * __cdecl CTaskListThumbnailWnd::_GetTaskItem(int)const )",

                    // Windows 10.
                    LR"(private: struct ITaskItem * __cdecl CTaskListThumbnailWnd::_GetTaskItem(int))",
                },
                &CTaskListThumbnailWnd__GetTaskItem,
            },
            {
                {LR"(public: virtual struct ITaskGroup * __cdecl CTaskListThumbnailWnd::GetTaskGroup(void)const )"},
                &CTaskListThumbnailWnd_GetTaskGroup,
            },
            {
                {LR"(public: virtual void __cdecl CTaskListThumbnailWnd::TaskReordered(struct ITaskItem *))"},
                &CTaskListThumbnailWnd_TaskReordered,
            },
            {
                {LR"(public: virtual int __cdecl CTaskGroup::GetNumItems(void))"},
                &CTaskGroup_GetNumItems,
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
                {LR"(private: virtual __int64 __cdecl CTaskListThumbnailWnd::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
                &CTaskListThumbnailWnd_v_WndProc_Original,
                CTaskListThumbnailWnd_v_WndProc_Hook,
            },
            {
                {LR"(public: __cdecl winrt::WindowsUdk::UI::Shell::implementation::TaskItemThumbnail::TaskItemThumbnail(struct winrt::WindowsUdk::UI::Shell::TaskItem const &,struct ITaskGroup *,struct ITaskItem *,struct ITaskListUI *,struct IWICImagingFactory *,struct ITaskListAcc *,bool))"},
                &TaskItemThumbnail_TaskItemThumbnail_Original,
                TaskItemThumbnail_TaskItemThumbnail_Hook,
                true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
            },
            {
                {LR"(public: struct winrt::Windows::Foundation::Collections::IObservableVector<struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail> __cdecl winrt::WindowsUdk::UI::Shell::implementation::TaskGroup::Thumbnails(void))"},
                &TaskGroup_Thumbnails_Original,
                TaskGroup_Thumbnails_Hook,
                true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
            },
            {
                {LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IObservableVector<struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail>,struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail>::Size(void)const )"},
                &TaskItemThumbnail_Size_Original,
                nullptr,
                true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
            },
            {
                {LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IObservableVector<struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail>,struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail>::GetAt(unsigned int)const )"},
                &TaskItemThumbnail_GetAt_Original,
                nullptr,
                true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
            },
            {
                {LR"(public: virtual long __cdecl CTaskListWnd::HandleExtendedUIClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
                &CTaskListWnd_HandleExtendedUIClick_Original,
                CTaskListWnd_HandleExtendedUIClick_Hook,
                true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
            },
        };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::TargetItemKey(struct winrt::hstring const &))"},
            &HoverFlyoutModel_TargetItemKey_Original,
            HoverFlyoutModel_TargetItemKey_Hook,
            true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskItemThumbnailList,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"},
            &TaskItemThumbnailList_OnPointerMoved_Original,
            TaskItemThumbnailList_OnPointerMoved_Hook,
            true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
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
    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetExplorerVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000) {
                return WinVersion::Win10;
            } else if (build < 26100) {
                return WinVersion::Win11;
            } else {
                return WinVersion::Win11_24H2;
            }
            break;
    }

    return WinVersion::Unsupported;
}

struct EXPLORER_PATCHER_HOOK {
    PCSTR symbol;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;

    template <typename Prototype>
    EXPLORER_PATCHER_HOOK(
        PCSTR symbol,
        Prototype** originalFunction,
        std::type_identity_t<Prototype*> hookFunction = nullptr,
        bool optional = false)
        : symbol(symbol),
          pOriginalFunction(reinterpret_cast<void**>(originalFunction)),
          hookFunction(reinterpret_cast<void*>(hookFunction)),
          optional(optional) {}
};

bool HookExplorerPatcherSymbols(HMODULE explorerPatcherModule) {
    if (g_explorerPatcherInitialized.exchange(true)) {
        return true;
    }

    if (g_winVersion >= WinVersion::Win11) {
        g_winVersion = WinVersion::Win10;
    }

    EXPLORER_PATCHER_HOOK hooks[] = {
        {R"(??_7CTaskListWnd@@6BITaskListUI@@@)",
         &CTaskListWnd_vftable_ITaskListUI},
        {R"(?GetHoverIndex@CTaskListThumbnailWnd@@UEBAHXZ)",
         &CTaskListThumbnailWnd_GetHoverIndex},
        {R"(?_GetTaskItem@CTaskListThumbnailWnd@@AEAAPEAUITaskItem@@H@Z)",
         &CTaskListThumbnailWnd__GetTaskItem},
        {R"(?GetTaskGroup@CTaskListThumbnailWnd@@UEBAPEAUITaskGroup@@XZ)",
         &CTaskListThumbnailWnd_GetTaskGroup},
        {R"(?TaskReordered@CTaskListThumbnailWnd@@UEAAXPEAUITaskItem@@@Z)",
         &CTaskListThumbnailWnd_TaskReordered},
        {R"(?GetNumItems@CTaskGroup@@UEAAHXZ)", &CTaskGroup_GetNumItems},
        {R"(?_GetTBGroupFromGroup@CTaskListWnd@@IEAAPEAUITaskBtnGroup@@PEAUITaskGroup@@PEAH@Z)",
         &CTaskListWnd__GetTBGroupFromGroup},
        {R"(?GetGroupType@CTaskBtnGroup@@UEAA?AW4eTBGROUPTYPE@@XZ)",
         &CTaskBtnGroup_GetGroupType},
        {R"(?IndexOfTaskItem@CTaskBtnGroup@@UEAAHPEAUITaskItem@@@Z)",
         &CTaskBtnGroup_IndexOfTaskItem},
        {R"(?TaskInclusionChanged@CTaskListWnd@@UEAAJPEAUITaskGroup@@PEAUITaskItem@@@Z)",
         &CTaskListWnd_TaskInclusionChanged},
        {R"(?IsTaskAllowed@TaskItemFilter@@UEAA_NPEAUITaskItem@@@Z)",
         &TaskItemFilter_IsTaskAllowed_Original,
         TaskItemFilter_IsTaskAllowed_Hook},
        {R"(?v_WndProc@CTaskListThumbnailWnd@@EEAA_JPEAUHWND__@@I_K_J@Z)",
         &CTaskListThumbnailWnd_v_WndProc_Original,
         CTaskListThumbnailWnd_v_WndProc_Hook},
    };

    bool succeeded = true;

    for (const auto& hook : hooks) {
        void* ptr = (void*)GetProcAddress(explorerPatcherModule, hook.symbol);
        if (!ptr) {
            Wh_Log(L"ExplorerPatcher symbol%s doesn't exist: %S",
                   hook.optional ? L" (optional)" : L"", hook.symbol);
            if (!hook.optional) {
                succeeded = false;
            }
            continue;
        }

        if (hook.hookFunction) {
            Wh_SetFunctionHook(ptr, hook.hookFunction, hook.pOriginalFunction);
        } else {
            *hook.pOriginalFunction = ptr;
        }
    }

    if (!succeeded) {
        Wh_Log(L"HookExplorerPatcherSymbols failed");
    } else if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    return succeeded;
}

bool IsExplorerPatcherModule(HMODULE module) {
    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        return false;
    }

    moduleFileName++;

    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) ==
        0) {
        Wh_Log(L"ExplorerPatcher taskbar module: %s", moduleFileName);
        return true;
    }

    return false;
}

bool HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                return HookExplorerPatcherSymbols(hMods[i]);
            }
        }
    }

    return true;
}

void HandleLoadedModuleIfExplorerPatcher(HMODULE module) {
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized) {
        if (IsExplorerPatcherModule(module)) {
            HookExplorerPatcherSymbols(module);
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
        HandleLoadedModuleIfExplorerPatcher(module);
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

void LoadSettings() {
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_winVersion = GetExplorerVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (g_settings.oldTaskbarOnWin11) {
        bool hasWin10Taskbar = g_winVersion < WinVersion::Win11_24H2;

        if (g_winVersion >= WinVersion::Win11) {
            g_winVersion = WinVersion::Win10;
        }

        if (hasWin10Taskbar && !HookTaskbarSymbols()) {
            return FALSE;
        }
    } else if (g_winVersion >= WinVersion::Win11) {
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
        }
    } else {
        if (!HookTaskbarSymbols()) {
            return FALSE;
        }
    }

    if (!HandleLoadedExplorerPatcher()) {
        Wh_Log(L"HandleLoadedExplorerPatcher failed");
        return FALSE;
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

    WindhawkUtils::Wh_SetFunctionHookT(DPA_GetPtr, DPA_GetPtr_Hook,
                                       &DPA_GetPtr_Original);

    g_initialized = true;

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    // Try again in case there's a race between the previous attempt and the
    // LoadLibraryExW hook.
    if (!g_explorerPatcherInitialized) {
        HandleLoadedExplorerPatcher();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;

    return TRUE;
}
