// ==WindhawkMod==
// @id              taskbar-multirow
// @name            Multirow taskbar for Windows 11
// @description     Span taskbar items across multiple rows, just like it was possible before Windows 11
// @version         1.1.4
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
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
# Multirow taskbar for Windows 11

Span taskbar items across multiple rows, just like it was possible before
Windows 11.

## Notes

* The mod doesn't change the taskbar height, it only makes the task list span
  across multiple rows. To change the taskbar height, use the [Taskbar height
  and icon size](https://windhawk.net/mods/taskbar-icon-size) mod.
* To have multiple rows of tray icons, use the [Taskbar tray icon spacing and
  grid](https://windhawk.net/mods/taskbar-notification-icon-spacing) mod.

![Screenshot](https://i.imgur.com/xEK7NhR.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- rows: 2
  $name: Rows
- fullHeightStartButton: true
  $name: Full-height start button
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <functional>
#include <optional>
#include <unordered_map>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

struct {
    int rows;
    bool fullHeightStartButton;
} g_settings;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_unloading;

// Holds a flag for the duration of a scope, so that it's also cleared if the
// function it wraps throws.
class ScopedFlag {
   public:
    ScopedFlag(bool* flag) : m_flag(flag) { *m_flag = true; }
    ~ScopedFlag() { *m_flag = false; }

    ScopedFlag(const ScopedFlag&) = delete;
    ScopedFlag& operator=(const ScopedFlag&) = delete;

   private:
    bool* m_flag;
};

thread_local bool g_inTaskbarCollapsibleLayoutXamlTraits_ArrangeOverride;
thread_local float g_taskbarCollapsibleLayoutHeight;

// The rect each item was last moved to, so that an item re-arranged in its
// slot, e.g. by another mod, isn't moved again. A slot from another layout
// height doesn't count, as the layout's rect for the new height can match it.
struct ArrangedItem {
    winrt::weak_ref<FrameworkElement> element;
    winrt::Windows::Foundation::Rect rect;
    float layoutHeight;
};
std::unordered_map<void*, ArrangedItem> g_arrangedItems;

// Set while the taskbar calculates the drop position of a dragged item, holding
// the horizontal distance between the row the item is dragged over and the
// single row the taskbar lays items out in.
bool g_hasDropPlaceholderOffset;
float g_dropPlaceholderOffset;

struct TaskbarState {
    winrt::weak_ref<XamlRoot> xamlRoot;
    // Sized by GetTaskbarState from the row count setting, which can change on
    // another thread in the meantime. Index it within its own size.
    std::vector<float> rowOffsetAdjustment;
};

std::unordered_map<void*, TaskbarState> g_taskbarState;

// Only one taskbar item can be dragged at a time.
struct {
    bool active;
    int startRow;
    bool hasLastDropX;
    float lastDropX;
} g_dragState;

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

// The taskbar frame doesn't always have an explicit width, in which case
// Width() is NaN and the laid out width has to be used.
double GetTaskbarFrameWidth(FrameworkElement taskbarFrameElement) {
    double width = taskbarFrameElement.Width();
    return std::isnan(width) ? taskbarFrameElement.ActualWidth() : width;
}

// The width a single row of items can span, i.e. the taskbar frame width
// without the part which is occupied by the system tray.
bool GetWidthWithoutExtent(FrameworkElement xamlRootContent, double* result) {
    auto taskbarFrameElement =
        FindChildByName(xamlRootContent, L"TaskbarFrame");
    if (!taskbarFrameElement) {
        return false;
    }

    auto systemTrayFrame =
        FindChildByClassName(xamlRootContent, L"SystemTray.SystemTrayFrame");
    if (!systemTrayFrame) {
        return false;
    }

    *result = GetTaskbarFrameWidth(taskbarFrameElement) -
              systemTrayFrame.ActualWidth();
    return true;
}

TaskbarState* GetTaskbarState(XamlRoot xamlRoot) {
    void* xamlRootAbi = winrt::get_abi(xamlRoot);

    auto [it, inserted] = g_taskbarState.insert(
        {xamlRootAbi, TaskbarState{
                          .xamlRoot = winrt::make_weak(xamlRoot),
                      }});

    if (!inserted && !it->second.xamlRoot.get()) {
        it->second = TaskbarState{
            .xamlRoot = winrt::make_weak(xamlRoot),
        };
    }

    // Update size in case it was just created as an empty vector or in case the
    // settings changed.
    it->second.rowOffsetAdjustment.resize(g_settings.rows);

    return &it->second;
}

// The horizontal offset which items get on their way from the single row the
// taskbar lays out to the given row.
float RowOffsetSum(TaskbarState* taskbarState, int row) {
    float sum = 0;

    size_t count = std::min(static_cast<size_t>(row),
                            taskbarState->rowOffsetAdjustment.size());
    for (size_t i = 0; i < count; i++) {
        sum += taskbarState->rowOffsetAdjustment[i];
    }

    return sum;
}

int ClampRow(int row) {
    if (row < 0) {
        return 0;
    }

    if (row >= g_settings.rows) {
        return g_settings.rows - 1;
    }

    return row;
}

// Expands the bounds of a task list item to span all rows, given the distance
// of the item from the top of the task list.
void ExpandBoundsToAllRows(winrt::Windows::Foundation::Rect* bounds,
                           double offset,
                           double taskListHeight) {
    // Also catches NaN.
    if (g_settings.rows < 2 || !(taskListHeight > 0)) {
        return;
    }

    double rowHeight = taskListHeight / g_settings.rows;
    int row = ClampRow((int)std::lround(offset / rowHeight));
    bounds->Y -= row * rowHeight;
    bounds->Height = taskListHeight;
}

FrameworkElement FindFullHeightStartButton(
    FrameworkElement taskbarFrameRepeater) {
    if (!g_settings.fullHeightStartButton) {
        return nullptr;
    }

    return EnumChildElements(taskbarFrameRepeater, [](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Taskbar.ExperienceToggleButton") {
            return false;
        }

        auto automationId =
            Automation::AutomationProperties::GetAutomationId(child);
        return automationId == L"StartButton";
    });
}

// Moves the rect of a task list item from the single row that the taskbar lays
// items out in to the row that the item is displayed in. An item that moves to
// the beginning of a row sets the horizontal offset of the row's other items,
// unless updateRowOffsets is false.
winrt::Windows::Foundation::Rect MoveToDisplayedRow(
    winrt::Windows::Foundation::Rect rect,
    TaskbarState* taskbarState,
    double widthWithoutExtent,
    double startButtonWidth,
    bool updateRowOffsets) {
    int rows = static_cast<int>(taskbarState->rowOffsetAdjustment.size());
    rect.Height /= rows;
    for (int i = 0; i < rows - 1 && rect.X + rect.Width > widthWithoutExtent;
         i++) {
        rect.X -= widthWithoutExtent;
        if (rect.X <= 0) {
            if (updateRowOffsets) {
                taskbarState->rowOffsetAdjustment[i] =
                    -rect.X + startButtonWidth;
            }

            rect.X = startButtonWidth;
        } else {
            rect.X += taskbarState->rowOffsetAdjustment[i];
        }

        rect.Y += rect.Height;
    }

    return rect;
}

void UpdateTaskbarFrameRepeaterMargin(FrameworkElement taskbarFrameRepeater,
                                      TaskbarState* taskbarState,
                                      double widthWithoutExtent,
                                      bool forceUpdate = false) {
    double desiredMargin = 0;

    if (!g_unloading) {
        // Also catches NaN.
        if (!(widthWithoutExtent > 0)) {
            Wh_Log(L"Skipping, widthWithoutExtent=%f", widthWithoutExtent);
            return;
        }

        desiredMargin = -widthWithoutExtent * (g_settings.rows - 1);

        for (const auto f : taskbarState->rowOffsetAdjustment) {
            desiredMargin += f;
        }

        if (desiredMargin > 0) {
            desiredMargin = 0;
        }
    }

    auto margin = taskbarFrameRepeater.Margin();
    if (forceUpdate) {
        Wh_Log(L"Re-setting margin.Right=%f (widthWithoutExtent=%f)",
               desiredMargin, widthWithoutExtent);
        margin.Right = desiredMargin + 1;
        taskbarFrameRepeater.Margin(margin);
        margin.Right = desiredMargin;
        taskbarFrameRepeater.Margin(margin);
    } else if (margin.Right != desiredMargin) {
        Wh_Log(L"Setting margin.Right=%f (widthWithoutExtent=%f)",
               desiredMargin, widthWithoutExtent);
        margin.Right = desiredMargin;
        taskbarFrameRepeater.Margin(margin);
    }
}

FrameworkElement FindTaskbarFrameRepeater(FrameworkElement xamlRootContent) {
    FrameworkElement child = xamlRootContent;
    if (child &&
        (child = FindChildByClassName(child, L"Taskbar.TaskbarFrame")) &&
        (child = FindChildByName(child, L"RootGrid")) &&
        (child = FindChildByName(child, L"TaskbarFrameRepeater"))) {
        return child;
    }

    return nullptr;
}

bool ApplyStyle(XamlRoot xamlRoot) {
    TaskbarState* taskbarState = GetTaskbarState(xamlRoot);

    auto xamlRootContent = xamlRoot.Content().as<FrameworkElement>();

    auto taskbarFrameRepeater = FindTaskbarFrameRepeater(xamlRootContent);
    if (!taskbarFrameRepeater) {
        return false;
    }

    double widthWithoutExtent;
    if (!GetWidthWithoutExtent(xamlRootContent, &widthWithoutExtent)) {
        return false;
    }

    UpdateTaskbarFrameRepeaterMargin(taskbarFrameRepeater, taskbarState,
                                     widthWithoutExtent, /*forceUpdate=*/true);

    return true;
}

void* CTaskBand_ITaskListWndSite_vftable;

void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#else
#error "Unsupported architecture"
#endif

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    HWND hTaskSwWnd =
        (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                               taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
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

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

void ApplySettingsFromTaskbarThread() {
    Wh_Log(L"Applying settings");

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            XamlRoot xamlRoot = nullptr;
            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                xamlRoot = GetTaskbarXamlRoot(hWnd);
            } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
            } else {
                return TRUE;
            }

            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed");
                return TRUE;
            }

            if (!ApplyStyle(xamlRoot)) {
                Wh_Log(L"ApplyStyle failed");
                return TRUE;
            }

            return TRUE;
        },
        0);

    // Touch a registry value to trigger a watcher for the settings.
    constexpr WCHAR kTempValueName[] = L"_temp_windhawk_" WH_MOD_ID;
    HKEY hSubKey;
    LONG result = RegOpenKeyEx(
        HKEY_CURRENT_USER,
        LR"(SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Advanced)", 0,
        KEY_WRITE, &hSubKey);
    if (result == ERROR_SUCCESS) {
        if (RegSetValueEx(hSubKey, kTempValueName, 0, REG_SZ, (const BYTE*)L"",
                          sizeof(WCHAR)) != ERROR_SUCCESS) {
            Wh_Log(L"Failed to create temp value");
        } else if (RegDeleteValue(hSubKey, kTempValueName) != ERROR_SUCCESS) {
            Wh_Log(L"Failed to remove temp value");
        }

        RegCloseKey(hSubKey);
    } else {
        Wh_Log(L"Failed to open subkey: %d", result);
    }
}

void ApplySettings(HWND hTaskbarWnd) {
    RunFromWindowThread(
        hTaskbarWnd,
        [](void* pParam) -> void { ApplySettingsFromTaskbarThread(); }, 0);
}

// The bounds of the taskbar button whose context menu (jump list) is being
// opened, relative to the XAML root content.
bool g_hasJumpListButtonBounds;
winrt::Windows::Foundation::Rect g_jumpListButtonBounds;

bool g_inCTaskListWnd_ComputeJumpViewPosition;

bool GetBoundsInXamlRoot(UIElement const& uiElement,
                         winrt::Windows::Foundation::Rect* bounds) {
    auto element = uiElement.try_as<FrameworkElement>();
    auto xamlRoot = element ? element.XamlRoot() : nullptr;
    auto xamlRootContent = xamlRoot ? xamlRoot.Content() : nullptr;
    if (!xamlRootContent) {
        return false;
    }

    *bounds = element.TransformToVisual(xamlRootContent)
                  .TransformBounds(winrt::Windows::Foundation::Rect{
                      0, 0, (float)element.ActualWidth(),
                      (float)element.ActualHeight()});
    return true;
}

using TaskbarResources_OnTaskListButtonContextRequested_t =
    void(WINAPI*)(void* pThis, UIElement const& sender, void* args);
TaskbarResources_OnTaskListButtonContextRequested_t
    TaskbarResources_OnTaskListButtonContextRequested_Original;
void WINAPI
TaskbarResources_OnTaskListButtonContextRequested_Hook(void* pThis,
                                                       UIElement const& sender,
                                                       void* args) {
    Wh_Log(L">");

    auto original = [&] {
        TaskbarResources_OnTaskListButtonContextRequested_Original(
            pThis, sender, args);
    };

    if (!GetBoundsInXamlRoot(sender, &g_jumpListButtonBounds)) {
        return original();
    }

    ScopedFlag scopedFlag(&g_hasJumpListButtonBounds);

    original();
}

using TaskListButtonHandlers_HandleContextRequested_t =
    void(WINAPI*)(UIElement const& sender, void* args);
TaskListButtonHandlers_HandleContextRequested_t
    TaskListButtonHandlers_HandleContextRequested_Original;
void WINAPI
TaskListButtonHandlers_HandleContextRequested_Hook(UIElement const& sender,
                                                   void* args) {
    Wh_Log(L">");

    auto original = [&] {
        TaskListButtonHandlers_HandleContextRequested_Original(sender, args);
    };

    if (!GetBoundsInXamlRoot(sender, &g_jumpListButtonBounds)) {
        return original();
    }

    ScopedFlag scopedFlag(&g_hasJumpListButtonBounds);

    original();
}

using CTaskListWnd_ComputeJumpViewPosition_t =
    HRESULT(WINAPI*)(void* pThis,
                     void* taskBtnGroup,
                     int param2,
                     winrt::Windows::Foundation::Point* point,
                     HorizontalAlignment* horizontalAlignment,
                     VerticalAlignment* verticalAlignment);
CTaskListWnd_ComputeJumpViewPosition_t
    CTaskListWnd_ComputeJumpViewPosition_Original;
HRESULT WINAPI CTaskListWnd_ComputeJumpViewPosition_Hook(
    void* pThis,
    void* taskBtnGroup,
    int param2,
    winrt::Windows::Foundation::Point* point,
    HorizontalAlignment* horizontalAlignment,
    VerticalAlignment* verticalAlignment) {
    Wh_Log(L">");

    ScopedFlag scopedFlag(&g_inCTaskListWnd_ComputeJumpViewPosition);

    return CTaskListWnd_ComputeJumpViewPosition_Original(
        pThis, taskBtnGroup, param2, point, horizontalAlignment,
        verticalAlignment);
}

// Classic thumbnails are placed right outside the reported bounds of the button
// group, on the side facing away from the screen edge. The bounds are expanded
// to span all rows, so that thumbnails don't cover other rows.
bool g_inCTaskListThumbnailWnd_DisplayUI;
bool g_inCTaskListThumbnailWnd_LayoutThumbnails;

using CTaskListThumbnailWnd_DisplayUI_t = int(WINAPI*)(void* pThis,
                                                       void* taskBtnGroup,
                                                       void* taskItem,
                                                       void* param3,
                                                       DWORD param4);
CTaskListThumbnailWnd_DisplayUI_t CTaskListThumbnailWnd_DisplayUI_Original;
int WINAPI CTaskListThumbnailWnd_DisplayUI_Hook(void* pThis,
                                                void* taskBtnGroup,
                                                void* taskItem,
                                                void* param3,
                                                DWORD param4) {
    Wh_Log(L">");

    ScopedFlag scopedFlag(&g_inCTaskListThumbnailWnd_DisplayUI);

    return CTaskListThumbnailWnd_DisplayUI_Original(pThis, taskBtnGroup,
                                                    taskItem, param3, param4);
}

using CTaskListThumbnailWnd_LayoutThumbnails_t = void(WINAPI*)(void* pThis);
CTaskListThumbnailWnd_LayoutThumbnails_t
    CTaskListThumbnailWnd_LayoutThumbnails_Original;
void WINAPI CTaskListThumbnailWnd_LayoutThumbnails_Hook(void* pThis) {
    Wh_Log(L">");

    ScopedFlag scopedFlag(&g_inCTaskListThumbnailWnd_LayoutThumbnails);

    CTaskListThumbnailWnd_LayoutThumbnails_Original(pThis);
}

// The XAML root of the taskbar that the window belongs to.
XamlRoot GetTaskbarXamlRootOfWindow(HWND hWnd) {
    HWND hTaskbarWnd = GetAncestor(hWnd, GA_ROOT);
    WCHAR className[32];
    if (!hTaskbarWnd ||
        !GetClassName(hTaskbarWnd, className, ARRAYSIZE(className))) {
        return nullptr;
    }

    if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
        return GetTaskbarXamlRoot(hTaskbarWnd);
    }

    if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        return GetSecondaryTaskbarXamlRoot(hTaskbarWnd);
    }

    return nullptr;
}

// The bounds of the task list of the taskbar that the window belongs to,
// relative to the XAML root content.
bool GetTaskListBounds(HWND hWnd, winrt::Windows::Foundation::Rect* bounds) {
    auto xamlRoot = GetTaskbarXamlRootOfWindow(hWnd);
    if (!xamlRoot) {
        return false;
    }

    auto taskbarFrameRepeater =
        FindTaskbarFrameRepeater(xamlRoot.Content().try_as<FrameworkElement>());
    return taskbarFrameRepeater &&
           GetBoundsInXamlRoot(taskbarFrameRepeater, bounds);
}

// Moves the bounds that the taskbar reports for a button group, relative to the
// XAML root content, to the row that the group is displayed in. The taskbar
// converts the bounds from the layout, which places the buttons in a single
// row, as if they were relative to the taskbar frame. Bounds that are already
// within a row aren't moved.
void MoveGroupBoundsToDisplayedRow(HWND hWnd,
                                   winrt::Windows::Foundation::Rect* bounds) {
    if (g_settings.rows < 2) {
        return;
    }

    auto xamlRoot = GetTaskbarXamlRootOfWindow(hWnd);
    auto xamlRootContent =
        xamlRoot ? xamlRoot.Content().try_as<FrameworkElement>() : nullptr;
    if (!xamlRootContent) {
        return;
    }

    auto taskbarFrame =
        FindChildByClassName(xamlRootContent, L"Taskbar.TaskbarFrame");
    auto taskbarFrameRepeater = FindTaskbarFrameRepeater(xamlRootContent);
    winrt::Windows::Foundation::Rect taskbarFrameBounds;
    double widthWithoutExtent;
    if (!taskbarFrame || !taskbarFrameRepeater ||
        !GetBoundsInXamlRoot(taskbarFrame, &taskbarFrameBounds) ||
        !GetWidthWithoutExtent(xamlRootContent, &widthWithoutExtent) ||
        !(widthWithoutExtent > 0)) {
        return;
    }

    auto startButton = FindFullHeightStartButton(taskbarFrameRepeater);
    double startButtonWidth = startButton ? startButton.ActualWidth() : 0;

    winrt::Windows::Foundation::Rect layoutRect = *bounds;
    layoutRect.X -= taskbarFrameBounds.X;

    auto displayedRect = MoveToDisplayedRow(
        layoutRect, GetTaskbarState(xamlRoot), widthWithoutExtent,
        startButtonWidth, /*updateRowOffsets=*/false);
    bounds->X = displayedRect.X + taskbarFrameBounds.X;
}

// The jump list is centered horizontally on the reported bounds of the button
// group, which don't match the displayed buttons: some builds report them as
// if the buttons were laid out in a single row, and a group's buttons can span
// multiple rows. It's centered on the button it's opened for if the jump list
// is opened from the button's context menu request. Otherwise, e.g. for
// Win+Alt+number, the reported bounds are moved to the displayed row.
using ToRawPixelsInDesktopCoordinates_t = RECT(
    WINAPI*)(const winrt::Windows::Foundation::Rect& rect, UINT dpi, HWND hWnd);
ToRawPixelsInDesktopCoordinates_t ToRawPixelsInDesktopCoordinates_Original;
RECT WINAPI ToRawPixelsInDesktopCoordinates_Hook(
    const winrt::Windows::Foundation::Rect& rect,
    UINT dpi,
    HWND hWnd) {
    if (g_unloading) {
        return ToRawPixelsInDesktopCoordinates_Original(rect, dpi, hWnd);
    }

    winrt::Windows::Foundation::Rect newRect = rect;

    if (g_inCTaskListWnd_ComputeJumpViewPosition) {
        if (g_hasJumpListButtonBounds) {
            newRect.X = g_jumpListButtonBounds.X;
            newRect.Width = g_jumpListButtonBounds.Width;
        } else {
            MoveGroupBoundsToDisplayedRow(hWnd, &newRect);
        }
    } else if (g_inCTaskListThumbnailWnd_DisplayUI ||
               g_inCTaskListThumbnailWnd_LayoutThumbnails) {
        winrt::Windows::Foundation::Rect taskListBounds;
        if (GetTaskListBounds(hWnd, &taskListBounds)) {
            ExpandBoundsToAllRows(&newRect, newRect.Y - taskListBounds.Y,
                                  taskListBounds.Height);
        }
    }

    return ToRawPixelsInDesktopCoordinates_Original(newRect, dpi, hWnd);
}

// Hover flyouts, such as thumbnails, are placed right outside the union of the
// bounds of their targets, on the side facing away from the screen edge. The
// bounds of each target are expanded to span all rows, so that flyouts don't
// cover other rows.
thread_local bool g_inFlyoutFrame_UpdateFlyoutPosition;

// The position in the task list of the target whose bounds are calculated.
struct TaskListItemPosition {
    double offset;
    double taskListHeight;
};
thread_local std::optional<TaskListItemPosition> g_flyoutTargetPosition;

using IUIElement_TransformToVisual_t = HRESULT(WINAPI*)(void* pThis,
                                                        void* visual,
                                                        void** result);
IUIElement_TransformToVisual_t IUIElement_TransformToVisual_Original;
HRESULT WINAPI IUIElement_TransformToVisual_Hook(void* pThis,
                                                 void* visual,
                                                 void** result) {
    auto original = [=] {
        return IUIElement_TransformToVisual_Original(pThis, visual, result);
    };

    // The flyout gets the bounds of each target with this function, and then
    // unites them with UnionRect.
    if (!g_inFlyoutFrame_UpdateFlyoutPosition || g_unloading) {
        return original();
    }

    g_flyoutTargetPosition.reset();

    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));
    if (!element) {
        return original();
    }

    auto parent = Media::VisualTreeHelper::GetParent(element);
    if (!parent) {
        return original();
    }

    auto taskbarFrameRepeater = parent.try_as<FrameworkElement>();
    if (!taskbarFrameRepeater ||
        taskbarFrameRepeater.Name() != L"TaskbarFrameRepeater") {
        return original();
    }

    g_flyoutTargetPosition = TaskListItemPosition{
        .offset = element.ActualOffset().y,
        .taskListHeight = taskbarFrameRepeater.ActualHeight(),
    };

    return original();
}

using UnionRect_t = winrt::Windows::Foundation::Rect(WINAPI*)(
    const winrt::Windows::Foundation::Rect& rect1,
    const winrt::Windows::Foundation::Rect& rect2);
UnionRect_t UnionRect_Original;
winrt::Windows::Foundation::Rect WINAPI
UnionRect_Hook(const winrt::Windows::Foundation::Rect& rect1,
               const winrt::Windows::Foundation::Rect& rect2) {
    // The flyout passes the bounds of the target as the second rect.
    if (!g_inFlyoutFrame_UpdateFlyoutPosition || !g_flyoutTargetPosition) {
        return UnionRect_Original(rect1, rect2);
    }

    winrt::Windows::Foundation::Rect newRect2 = rect2;
    ExpandBoundsToAllRows(&newRect2, g_flyoutTargetPosition->offset,
                          g_flyoutTargetPosition->taskListHeight);
    g_flyoutTargetPosition.reset();

    return UnionRect_Original(rect1, newRect2);
}

using FlyoutFrame_UpdateFlyoutPosition_t = void(WINAPI*)(void* pThis);
FlyoutFrame_UpdateFlyoutPosition_t FlyoutFrame_UpdateFlyoutPosition_Original;
void WINAPI FlyoutFrame_UpdateFlyoutPosition_Hook(void* pThis) {
    Wh_Log(L">");

    g_flyoutTargetPosition.reset();
    ScopedFlag scopedFlag(&g_inFlyoutFrame_UpdateFlyoutPosition);

    FlyoutFrame_UpdateFlyoutPosition_Original(pThis);
}

using IUIElement_Arrange_t =
    HRESULT(WINAPI*)(void* pThis, winrt::Windows::Foundation::Rect rect);
IUIElement_Arrange_t IUIElement_Arrange_Original;
HRESULT WINAPI IUIElement_Arrange_Hook(void* pThis,
                                       winrt::Windows::Foundation::Rect rect) {
    Wh_Log(L">");

    auto original = [=] { return IUIElement_Arrange_Original(pThis, rect); };

    if (!g_inTaskbarCollapsibleLayoutXamlTraits_ArrangeOverride ||
        g_unloading) {
        return original();
    }

    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));
    if (!element) {
        return original();
    }

    auto parent = Media::VisualTreeHelper::GetParent(element);
    if (!parent) {
        return original();
    }

    auto taskbarFrameRepeater = parent.try_as<FrameworkElement>();
    if (!taskbarFrameRepeater ||
        taskbarFrameRepeater.Name() != L"TaskbarFrameRepeater") {
        return original();
    }

    void* elementAbi = winrt::get_abi(element);
    if (auto it = g_arrangedItems.find(elementAbi);
        it != g_arrangedItems.end() && it->second.rect == rect &&
        it->second.layoutHeight == g_taskbarCollapsibleLayoutHeight) {
        return original();
    }

    auto startButton = FindFullHeightStartButton(taskbarFrameRepeater);
    if (element == startButton) {
        return original();
    }

    double startButtonWidth = startButton ? startButton.ActualWidth() : 0;

    auto xamlRoot = taskbarFrameRepeater.XamlRoot();
    if (!xamlRoot) {
        return original();
    }

    auto xamlRootContent = xamlRoot.Content().try_as<FrameworkElement>();
    if (!xamlRootContent) {
        return original();
    }

    TaskbarState* taskbarState = GetTaskbarState(xamlRoot);

    double widthWithoutExtent;
    if (!GetWidthWithoutExtent(xamlRootContent, &widthWithoutExtent)) {
        return original();
    }

    if (!(widthWithoutExtent > 0)) {
        Wh_Log(L"Skipping, widthWithoutExtent=%f", widthWithoutExtent);
        return original();
    }

    winrt::Windows::Foundation::Rect newRect =
        MoveToDisplayedRow(rect, taskbarState, widthWithoutExtent,
                           startButtonWidth, /*updateRowOffsets=*/true);

    if (newRect.X + newRect.Width > widthWithoutExtent) {
        UpdateTaskbarFrameRepeaterMargin(taskbarFrameRepeater, taskbarState,
                                         widthWithoutExtent);
    }

    // A rect left as is, e.g. with a single row, can be the layout's rect again
    // after a settings change.
    if (newRect != rect) {
        g_arrangedItems[elementAbi] = {winrt::make_weak(element), newRect,
                                       g_taskbarCollapsibleLayoutHeight};
    } else {
        g_arrangedItems.erase(elementAbi);
    }

    return IUIElement_Arrange_Original(pThis, newRect);
}

using TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_t =
    HRESULT(WINAPI*)(void* pThis,
                     void* context,
                     winrt::Windows::Foundation::Size size,
                     winrt::Windows::Foundation::Size* resultSize);
TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_t
    TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original;
HRESULT WINAPI TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook(
    void* pThis,
    void* context,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    Wh_Log(L">");

    [[maybe_unused]] static bool hooked = [] {
        Shapes::Rectangle rectangle;
        IUIElement element = rectangle;

        void** vtable = *(void***)winrt::get_abi(element);
        auto arrange = (IUIElement_Arrange_t)vtable[92];
        auto transformToVisual = (IUIElement_TransformToVisual_t)vtable[98];

        WindhawkUtils::SetFunctionHook(arrange, IUIElement_Arrange_Hook,
                                       &IUIElement_Arrange_Original);
        WindhawkUtils::SetFunctionHook(transformToVisual,
                                       IUIElement_TransformToVisual_Hook,
                                       &IUIElement_TransformToVisual_Original);
        Wh_ApplyHookOperations();
        return true;
    }();

    ScopedFlag scopedFlag(
        &g_inTaskbarCollapsibleLayoutXamlTraits_ArrangeOverride);

    g_taskbarCollapsibleLayoutHeight = size.Height;

    std::erase_if(g_arrangedItems,
                  [](const auto& item) { return !item.second.element.get(); });

    return TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original(
        pThis, context, size, resultSize);
}

// The taskbar keeps laying items out in a single row and tracks a drag in that
// row: the drop position is the dragged item's layout bounds moved by the
// distance the pointer traveled. The pointer travels across the rows the items
// are displayed in, so the drop position has to be moved by the distance
// between the row the pointer is over and the row the drag started in.
struct MultirowMetrics {
    TaskbarState* taskbarState;
    double widthWithoutExtent;
    double rowHeight;
};

bool GetMultirowMetrics(FrameworkElement taskListButtonElement,
                        MultirowMetrics* metrics) {
    if (g_settings.rows < 2) {
        return false;
    }

    auto parent = Media::VisualTreeHelper::GetParent(taskListButtonElement);
    if (!parent) {
        return false;
    }

    auto taskbarFrameRepeater = parent.try_as<FrameworkElement>();
    if (!taskbarFrameRepeater ||
        taskbarFrameRepeater.Name() != L"TaskbarFrameRepeater") {
        return false;
    }

    double rowHeight = taskbarFrameRepeater.ActualHeight() / g_settings.rows;
    // Also catches NaN.
    if (!(rowHeight > 0)) {
        return false;
    }

    auto xamlRoot = taskbarFrameRepeater.XamlRoot();
    if (!xamlRoot) {
        return false;
    }

    auto xamlRootContent = xamlRoot.Content().try_as<FrameworkElement>();
    if (!xamlRootContent) {
        return false;
    }

    double widthWithoutExtent;
    if (!GetWidthWithoutExtent(xamlRootContent, &widthWithoutExtent) ||
        !(widthWithoutExtent > 0)) {
        return false;
    }

    metrics->taskbarState = GetTaskbarState(xamlRoot);
    metrics->widthWithoutExtent = widthWithoutExtent;
    metrics->rowHeight = rowHeight;
    return true;
}

FrameworkElement TaskListButtonElement(void* pThis) {
    FrameworkElement element = nullptr;
    ((IUnknown*)pThis + 3)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));
    return element;
}

using TaskListButton_OnDragStartedGesture_t =
    void(WINAPI*)(void* pThis, winrt::Windows::Foundation::Point point);
TaskListButton_OnDragStartedGesture_t
    TaskListButton_OnDragStartedGesture_Original;
void WINAPI TaskListButton_OnDragStartedGesture_Hook(
    void* pThis,
    winrt::Windows::Foundation::Point point) {
    Wh_Log(L">");

    g_dragState = {};

    auto element = g_unloading ? nullptr : TaskListButtonElement(pThis);
    MultirowMetrics metrics;
    if (element && GetMultirowMetrics(element, &metrics)) {
        g_dragState.active = true;
        g_dragState.startRow = ClampRow(
            (int)std::lround(element.ActualOffset().y / metrics.rowHeight));

        Wh_Log(L"Drag started in row %d (point.Y=%f, rowHeight=%f)",
               g_dragState.startRow, point.Y, metrics.rowHeight);
    }

    TaskListButton_OnDragStartedGesture_Original(pThis, point);
}

using TaskListButton_OnDragCompletedGesture_t = void(WINAPI*)(void* pThis);
TaskListButton_OnDragCompletedGesture_t
    TaskListButton_OnDragCompletedGesture_Original;
void WINAPI TaskListButton_OnDragCompletedGesture_Hook(void* pThis) {
    Wh_Log(L">");

    TaskListButton_OnDragCompletedGesture_Original(pThis);

    g_dragState = {};
}

using TaskListButton_UpdateDrag_t =
    void(WINAPI*)(void* pThis, winrt::Windows::Foundation::Point point);
TaskListButton_UpdateDrag_t TaskListButton_UpdateDrag_Original;
void WINAPI
TaskListButton_UpdateDrag_Hook(void* pThis,
                               winrt::Windows::Foundation::Point point) {
    Wh_Log(L">");

    auto original = [=] { TaskListButton_UpdateDrag_Original(pThis, point); };

    if (!g_dragState.active || g_unloading) {
        return original();
    }

    auto element = TaskListButtonElement(pThis);
    MultirowMetrics metrics;
    if (!element || !GetMultirowMetrics(element, &metrics)) {
        return original();
    }

    // Gesture points are relative to the repeater, which spans all rows.
    int row = ClampRow((int)std::floor(point.Y / metrics.rowHeight));
    // Clamped again in case the number of rows changed mid-drag.
    int startRow = ClampRow(g_dragState.startRow);

    g_dropPlaceholderOffset = (row - startRow) * metrics.widthWithoutExtent -
                              (RowOffsetSum(metrics.taskbarState, row) -
                               RowOffsetSum(metrics.taskbarState, startRow));
    ScopedFlag scopedFlag(&g_hasDropPlaceholderOffset);

    return original();
}

using TaskListDragOperation_GetDropPlaceholder_t =
    void*(WINAPI*)(void* pThis,
                   void* result,
                   void* itemBounds,
                   UINT64 itemRange,
                   winrt::Windows::Foundation::Rect dragBounds,
                   int rearrangeDirection,
                   void* groupBoundsCalculator,
                   bool param7);
TaskListDragOperation_GetDropPlaceholder_t
    TaskListDragOperation_GetDropPlaceholder_Original;
void* WINAPI TaskListDragOperation_GetDropPlaceholder_Hook(
    void* pThis,
    void* result,
    void* itemBounds,
    UINT64 itemRange,
    winrt::Windows::Foundation::Rect dragBounds,
    int rearrangeDirection,
    void* groupBoundsCalculator,
    bool param7) {
    Wh_Log(L">");

    if (g_hasDropPlaceholderOffset) {
        dragBounds.X += g_dropPlaceholderOffset;

        // The caller derives the direction from the pointer position, which
        // moves backwards whenever the drop position moves forward into the
        // next row. 1 means forward, 0 means backwards.
        if (g_dragState.hasLastDropX && dragBounds.X != g_dragState.lastDropX) {
            rearrangeDirection = dragBounds.X > g_dragState.lastDropX ? 1 : 0;
        }

        g_dragState.hasLastDropX = true;
        g_dragState.lastDropX = dragBounds.X;
    }

    return TaskListDragOperation_GetDropPlaceholder_Original(
        pThis, result, itemBounds, itemRange, dragBounds, rearrangeDirection,
        groupBoundsCalculator, param7);
}

using TaskbarFrame_SystemTrayExtent_t = void(WINAPI*)(void* pThis,
                                                      double value);
TaskbarFrame_SystemTrayExtent_t TaskbarFrame_SystemTrayExtent_Original;
void WINAPI TaskbarFrame_SystemTrayExtent_Hook(void* pThis, double value) {
    Wh_Log(L"> %f", value);

    TaskbarFrame_SystemTrayExtent_Original(pThis, value);

    FrameworkElement taskbarFrameElement = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarFrameElement));
    if (!taskbarFrameElement) {
        return;
    }

    FrameworkElement taskbarFrameRepeater = nullptr;

    FrameworkElement child = taskbarFrameElement;
    if ((child = FindChildByName(child, L"RootGrid")) &&
        (child = FindChildByName(child, L"TaskbarFrameRepeater"))) {
        taskbarFrameRepeater = child;
    }

    if (!taskbarFrameRepeater) {
        return;
    }

    auto xamlRoot = taskbarFrameRepeater.XamlRoot();

    TaskbarState* taskbarState = GetTaskbarState(xamlRoot);

    double widthWithoutExtent =
        GetTaskbarFrameWidth(taskbarFrameElement) - value;

    UpdateTaskbarFrameRepeaterMargin(taskbarFrameRepeater, taskbarState,
                                     widthWithoutExtent);
}

using RegGetValueW_t = decltype(&RegGetValueW);
RegGetValueW_t RegGetValueW_Original;
LONG WINAPI RegGetValueW_Hook(HKEY hkey,
                              LPCWSTR lpSubKey,
                              LPCWSTR lpValue,
                              DWORD dwFlags,
                              LPDWORD pdwType,
                              PVOID pvData,
                              LPDWORD pcbData) {
    LONG ret = RegGetValueW_Original(hkey, lpSubKey, lpValue, dwFlags, pdwType,
                                     pvData, pcbData);

    if (hkey == HKEY_CURRENT_USER && lpSubKey &&
        _wcsicmp(
            lpSubKey,
            LR"(SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Advanced)") ==
            0 &&
        lpValue && _wcsicmp(lpValue, L"TaskbarAl") == 0 &&
        dwFlags == RRF_RT_REG_DWORD && pvData && pcbData &&
        *pcbData == sizeof(DWORD)) {
        Wh_Log(L"> %u", ret);

        if (!g_unloading) {
            Wh_Log(L"Overriding");

            *(DWORD*)pvData = 0;

            if (pdwType) {
                *pdwType = REG_DWORD;
            }

            ret = ERROR_SUCCESS;
        } else {
            Wh_Log(L"Returning original value: %u", *(DWORD*)pvData);
        }
    }

    return ret;
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
        {
            {LR"(protected: long __cdecl CTaskListWnd::_ComputeJumpViewPosition(struct ITaskBtnGroup *,int,struct Windows::Foundation::Point &,enum Windows::UI::Xaml::HorizontalAlignment &,enum Windows::UI::Xaml::VerticalAlignment &)const )"},
            &CTaskListWnd_ComputeJumpViewPosition_Original,
            CTaskListWnd_ComputeJumpViewPosition_Hook,
        },
        {
            {LR"(struct tagRECT __cdecl ToRawPixelsInDesktopCoordinates(struct winrt::Windows::Foundation::Rect const &,unsigned int,struct HWND__ *))"},
            &ToRawPixelsInDesktopCoordinates_Original,
            ToRawPixelsInDesktopCoordinates_Hook,
        },
        {
            {LR"(public: virtual int __cdecl CTaskListThumbnailWnd::DisplayUI(struct ITaskBtnGroup *,struct ITaskItem *,struct ITaskItem *,unsigned long))"},
            &CTaskListThumbnailWnd_DisplayUI_Original,
            CTaskListThumbnailWnd_DisplayUI_Hook,
            true,  // Classic thumbnails, removed in or near 10.0.26100.8491.
        },
        {
            {LR"(public: virtual void __cdecl CTaskListThumbnailWnd::LayoutThumbnails(void))"},
            &CTaskListThumbnailWnd_LayoutThumbnails_Original,
            CTaskListThumbnailWnd_LayoutThumbnails_Hook,
            true,  // Classic thumbnails, removed in or near 10.0.26100.8491.
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
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] =  //
        {
            {
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarCollapsibleLayout,struct winrt::Microsoft::UI::Xaml::Controls::IVirtualizingLayoutOverrides>::ArrangeOverride(void *,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
                &TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original,
                TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook,
            },
            {
                {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarResources::OnTaskListButtonContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"},
                &TaskbarResources_OnTaskListButtonContextRequested_Original,
                TaskbarResources_OnTaskListButtonContextRequested_Hook,
            },
            {
                {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::SystemTrayExtent(double))"},
                &TaskbarFrame_SystemTrayExtent_Original,
                TaskbarFrame_SystemTrayExtent_Hook,
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::OnDragStartedGesture(struct winrt::Windows::Foundation::Point))"},
                &TaskListButton_OnDragStartedGesture_Original,
                TaskListButton_OnDragStartedGesture_Hook,
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::OnDragCompletedGesture(void))"},
                &TaskListButton_OnDragCompletedGesture_Original,
                TaskListButton_OnDragCompletedGesture_Hook,
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateDrag(struct winrt::Windows::Foundation::Point))"},
                &TaskListButton_UpdateDrag_Original,
                TaskListButton_UpdateDrag_Hook,
            },
            {
                {
                    LR"(public: struct winrt::Taskbar::implementation::DropPlaceholder __cdecl winrt::Taskbar::implementation::TaskListDragOperation::GetDropPlaceholder(class std::vector<struct winrt::Windows::Foundation::Rect,class std::allocator<struct winrt::Windows::Foundation::Rect> > const &,struct std::pair<unsigned int,unsigned int>,struct winrt::Windows::Foundation::Rect,enum winrt::Taskbar::implementation::RearrangeDirection,struct winrt::Taskbar::implementation::GroupBoundsCalculator const *,bool))",

                    // Older builds, which lack the last parameter. Passing an
                    // extra argument is harmless with the x64 calling
                    // convention.
                    LR"(public: struct winrt::Taskbar::implementation::DropPlaceholder __cdecl winrt::Taskbar::implementation::TaskListDragOperation::GetDropPlaceholder(class std::vector<struct winrt::Windows::Foundation::Rect,class std::allocator<struct winrt::Windows::Foundation::Rect> > const &,struct std::pair<unsigned int,unsigned int>,struct winrt::Windows::Foundation::Rect,enum winrt::Taskbar::implementation::RearrangeDirection,struct winrt::Taskbar::implementation::GroupBoundsCalculator const *))",
                },
                &TaskListDragOperation_GetDropPlaceholder_Original,
                TaskListDragOperation_GetDropPlaceholder_Hook,
            },
            {
                {LR"(public: static void __cdecl winrt::Taskbar::implementation::TaskListButtonHandlers::HandleContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"},
                &TaskListButtonHandlers_HandleContextRequested_Original,
                TaskListButtonHandlers_HandleContextRequested_Hook,
                true,  // From 10.0.26200.8116.
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::FlyoutFrame::UpdateFlyoutPosition(void))"},
                &FlyoutFrame_UpdateFlyoutPosition_Original,
                FlyoutFrame_UpdateFlyoutPosition_Hook,
                true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
            },
            {
                {LR"(struct winrt::Windows::Foundation::Rect __cdecl UnionRect(struct winrt::Windows::Foundation::Rect const &,struct winrt::Windows::Foundation::Rect const &))"},
                &UnionRect_Original,
                UnionRect_Hook,
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
    g_settings.rows = std::max(Wh_GetIntSetting(L"rows"), 1);
    g_settings.fullHeightStartButton =
        Wh_GetIntSetting(L"fullHeightStartButton");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
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

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseRegGetValueW = (decltype(&RegGetValueW))GetProcAddress(
        kernelBaseModule, "RegGetValueW");
    WindhawkUtils::SetFunctionHook(pKernelBaseRegGetValueW, RegGetValueW_Hook,
                                   &RegGetValueW_Original);

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

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}
