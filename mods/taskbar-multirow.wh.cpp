// ==WindhawkMod==
// @id              taskbar-multirow
// @name            Multirow taskbar for Windows 11
// @description     Span taskbar items across multiple rows, just like it was possible before Windows 11
// @version         1.2
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
Windows 11. On a vertical taskbar, items span across multiple columns instead.

## Notes

* The mod doesn't change the taskbar height, it only makes the task list span
  across multiple rows. To change the taskbar height, use the [Taskbar height
  and icon size](https://windhawk.net/mods/taskbar-icon-size) mod.
* Similarly, the mod doesn't change the width of a vertical taskbar. To make
  room for multiple columns, change the taskbar width with the [Vertical Taskbar
  for Windows 11](https://windhawk.net/mods/taskbar-vertical) mod.
* Multiple columns require the native vertical taskbar, which is available in
  newer Windows 11 builds.
* To have multiple rows of tray icons, use the [Taskbar tray icon spacing and
  grid](https://windhawk.net/mods/taskbar-notification-icon-spacing) mod.

![Screenshot](https://i.imgur.com/xEK7NhR.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- rows: 2
  $name: Rows
  $description: On a vertical taskbar, this is the number of columns.
- fullHeightStartButton: true
  $name: Full-height start button
  $description: On a vertical taskbar, the start button spans the full width.
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

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

// A vertical taskbar lays items out in a single column instead of a single row.
// It's handled the same way with the axes swapped, so its columns are called
// rows here. The length of an element is its size along a row, and its
// thickness is its size across rows.

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
thread_local winrt::Windows::Foundation::Size g_taskbarCollapsibleLayoutSize;

// The rect each item was last moved to, so that an item re-arranged in its
// slot, e.g. by another mod, isn't moved again. A slot from another layout
// thickness doesn't count, as the layout's rect for the new thickness can match
// it.
struct ArrangedItem {
    winrt::weak_ref<FrameworkElement> element;
    winrt::Windows::Foundation::Rect rect;
    float layoutThickness;
};
std::unordered_map<void*, ArrangedItem> g_arrangedItems;

// XAML stores a slot with layout rounding, which can move each value by up to
// half a physical pixel.
bool IsSameSlot(winrt::Windows::Foundation::Rect a,
                winrt::Windows::Foundation::Rect b) {
    return std::abs(a.X - b.X) < 1 && std::abs(a.Y - b.Y) < 1 &&
           std::abs(a.Width - b.Width) < 1 && std::abs(a.Height - b.Height) < 1;
}

// Set while the taskbar calculates the drop position of a dragged item, holding
// the distance along the row between the row the item is dragged over and the
// single row the taskbar lays items out in.
bool g_hasDropPlaceholderOffset;
float g_dropPlaceholderOffset;

struct TaskbarState {
    winrt::weak_ref<XamlRoot> xamlRoot;
    // Kept up to date by GetTaskbarState, as the taskbar can change its
    // orientation at runtime.
    bool vertical;
    // Sized by GetTaskbarState from the row count setting, which can change on
    // another thread in the meantime. Index it within its own size.
    std::vector<float> rowOffsetAdjustment;
};

std::unordered_map<void*, TaskbarState> g_taskbarState;

// Only one taskbar item can be dragged at a time.
struct {
    bool active;
    bool vertical;
    int startRow;
    bool hasLastDropPosition;
    float lastDropPosition;
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

// The root grid of the taskbar frame has a visual state for the side the
// taskbar is docked to. Builds without the vertical taskbar may not set a
// current state.
bool IsVerticalTaskbarFrame(FrameworkElement taskbarFrameElement) {
    auto rootGrid = FindChildByName(taskbarFrameElement, L"RootGrid");
    if (!rootGrid) {
        return false;
    }

    for (const auto& group :
         VisualStateManager::GetVisualStateGroups(rootGrid)) {
        if (group.Name() == L"DockingStates") {
            auto currentState = group.CurrentState();
            if (!currentState) {
                return false;
            }

            auto name = currentState.Name();
            return name == L"DockedLeft" || name == L"DockedRight";
        }
    }

    return false;
}

double GetElementLength(FrameworkElement element, bool vertical) {
    return vertical ? element.ActualHeight() : element.ActualWidth();
}

double GetElementThickness(FrameworkElement element, bool vertical) {
    return vertical ? element.ActualWidth() : element.ActualHeight();
}

// Lets a rect of a vertical taskbar be handled like a rect of a horizontal one,
// and the other way around.
winrt::Windows::Foundation::Rect SwapAxesIfVertical(
    winrt::Windows::Foundation::Rect rect,
    bool vertical) {
    return vertical ? winrt::Windows::Foundation::Rect{rect.Y, rect.X,
                                                       rect.Height, rect.Width}
                    : rect;
}

// The taskbar frame doesn't always have an explicit length, in which case the
// property is NaN and the laid out length has to be used.
double GetTaskbarFrameLength(FrameworkElement taskbarFrameElement,
                             bool vertical) {
    double length =
        vertical ? taskbarFrameElement.Height() : taskbarFrameElement.Width();
    return std::isnan(length) ? GetElementLength(taskbarFrameElement, vertical)
                              : length;
}

// The length a single row of items can span, i.e. the taskbar frame length
// without the part which is occupied by the system tray.
bool GetLengthWithoutExtent(FrameworkElement xamlRootContent,
                            bool vertical,
                            double* result) {
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

    *result = GetTaskbarFrameLength(taskbarFrameElement, vertical) -
              GetElementLength(systemTrayFrame, vertical);
    return true;
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

// The root element of a taskbar item, e.g. the icon panel of a taskbar button.
FrameworkElement GetItemContent(FrameworkElement item) {
    if (Media::VisualTreeHelper::GetChildrenCount(item) == 0) {
        return nullptr;
    }

    return Media::VisualTreeHelper::GetChild(item, 0)
        .try_as<FrameworkElement>();
}

// The column that holds the icon, if the content of an item is the icon panel
// of a taskbar button.
Controls::ColumnDefinition GetItemContentIconColumn(FrameworkElement content) {
    auto iconPanel = content.try_as<Controls::Grid>();
    if (!iconPanel || iconPanel.Name() != L"IconPanel") {
        return nullptr;
    }

    auto columns = iconPanel.ColumnDefinitions();
    return columns.Size() > 0 ? columns.GetAt(0) : nullptr;
}

// Being arranged in a column doesn't make the content of an item narrower: a
// button without a label has an icon panel and an icon column as wide as the
// taskbar, and a label only gets trimmed to the available width. The maximum
// widths apply to all of these, and take precedence over the widths that the
// taskbar sets.
void LimitItemContentWidth(FrameworkElement item, double maxWidth) {
    auto content = GetItemContent(item);
    if (!content) {
        return;
    }

    if (content.MaxWidth() != maxWidth) {
        content.MaxWidth(maxWidth);
    }

    if (auto iconColumn = GetItemContentIconColumn(content)) {
        auto padding = content.as<Controls::Grid>().Padding();
        double iconColumnMaxWidth =
            std::max(maxWidth - padding.Left - padding.Right, 0.0);
        if (iconColumn.MaxWidth() != iconColumnMaxWidth) {
            iconColumn.MaxWidth(iconColumnMaxWidth);
        }
    }
}

void ClearItemContentWidthLimits(FrameworkElement taskbarFrameRepeater) {
    EnumChildElements(taskbarFrameRepeater, [](FrameworkElement item) {
        if (auto content = GetItemContent(item)) {
            content.ClearValue(FrameworkElement::MaxWidthProperty());

            if (auto iconColumn = GetItemContentIconColumn(content)) {
                iconColumn.ClearValue(
                    Controls::ColumnDefinition::MaxWidthProperty());
            }
        }

        return false;
    });
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

    auto xamlRootContent = xamlRoot.Content().try_as<FrameworkElement>();
    auto taskbarFrameElement =
        xamlRootContent ? FindChildByName(xamlRootContent, L"TaskbarFrame")
                        : nullptr;
    if (taskbarFrameElement &&
        it->second.vertical != IsVerticalTaskbarFrame(taskbarFrameElement)) {
        it->second.vertical = !it->second.vertical;
        Wh_Log(L"Taskbar orientation changed, vertical=%d",
               it->second.vertical);

        // Nothing that was applied for the other orientation carries over.
        it->second.rowOffsetAdjustment.clear();

        if (auto repeater = FindTaskbarFrameRepeater(xamlRootContent)) {
            auto margin = repeater.Margin();
            margin.Right = 0;
            margin.Bottom = 0;
            repeater.Margin(margin);

            ClearItemContentWidthLimits(repeater);
        }
    }

    // Update size in case it was just created as an empty vector or in case the
    // settings changed.
    it->second.rowOffsetAdjustment.resize(g_settings.rows);

    return &it->second;
}

// The offset along the row which items get on their way from the single row the
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
// of the item from the start of the first row.
void ExpandBoundsToAllRows(winrt::Windows::Foundation::Rect* bounds,
                           double offset,
                           double taskListThickness,
                           bool vertical) {
    // Also catches NaN.
    if (g_settings.rows < 2 || !(taskListThickness > 0)) {
        return;
    }

    double rowThickness = taskListThickness / g_settings.rows;
    int row = ClampRow((int)std::lround(offset / rowThickness));

    auto rect = SwapAxesIfVertical(*bounds, vertical);
    rect.Y -= row * rowThickness;
    rect.Height = taskListThickness;
    *bounds = SwapAxesIfVertical(rect, vertical);
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

// Moves the rect of a task list item, with the axes of a horizontal taskbar,
// from the single row that the taskbar lays items out in to the row that the
// item is displayed in. An item that moves to the beginning of a row sets the
// offset along the row of the row's other items, unless updateRowOffsets is
// false.
winrt::Windows::Foundation::Rect MoveToDisplayedRow(
    winrt::Windows::Foundation::Rect rect,
    TaskbarState* taskbarState,
    double lengthWithoutExtent,
    double startButtonLength,
    bool updateRowOffsets) {
    int rows = static_cast<int>(taskbarState->rowOffsetAdjustment.size());
    rect.Height /= rows;
    for (int i = 0; i < rows - 1 && rect.X + rect.Width > lengthWithoutExtent;
         i++) {
        rect.X -= lengthWithoutExtent;
        if (rect.X <= 0) {
            if (updateRowOffsets) {
                taskbarState->rowOffsetAdjustment[i] =
                    -rect.X + startButtonLength;
            }

            rect.X = startButtonLength;
        } else {
            rect.X += taskbarState->rowOffsetAdjustment[i];
        }

        rect.Y += rect.Height;
    }

    return rect;
}

void UpdateTaskbarFrameRepeaterMargin(FrameworkElement taskbarFrameRepeater,
                                      TaskbarState* taskbarState,
                                      double lengthWithoutExtent,
                                      bool forceUpdate = false) {
    double desiredMargin = 0;

    if (!g_unloading) {
        // Also catches NaN.
        if (!(lengthWithoutExtent > 0)) {
            Wh_Log(L"Skipping, lengthWithoutExtent=%f", lengthWithoutExtent);
            return;
        }

        desiredMargin = -lengthWithoutExtent * (g_settings.rows - 1);

        for (const auto f : taskbarState->rowOffsetAdjustment) {
            desiredMargin += f;
        }

        if (desiredMargin > 0) {
            desiredMargin = 0;
        }
    }

    auto margin = taskbarFrameRepeater.Margin();
    double& marginEnd = taskbarState->vertical ? margin.Bottom : margin.Right;
    if (forceUpdate) {
        Wh_Log(L"Re-setting margin end=%f (lengthWithoutExtent=%f)",
               desiredMargin, lengthWithoutExtent);
        marginEnd = desiredMargin + 1;
        taskbarFrameRepeater.Margin(margin);
        marginEnd = desiredMargin;
        taskbarFrameRepeater.Margin(margin);
    } else if (marginEnd != desiredMargin) {
        Wh_Log(L"Setting margin end=%f (lengthWithoutExtent=%f)", desiredMargin,
               lengthWithoutExtent);
        marginEnd = desiredMargin;
        taskbarFrameRepeater.Margin(margin);
    }
}

bool ApplyStyle(XamlRoot xamlRoot) {
    TaskbarState* taskbarState = GetTaskbarState(xamlRoot);

    auto xamlRootContent = xamlRoot.Content().as<FrameworkElement>();

    auto taskbarFrameRepeater = FindTaskbarFrameRepeater(xamlRootContent);
    if (!taskbarFrameRepeater) {
        return false;
    }

    // Arranging the items applies the limits again, unless they no longer apply
    // with the new settings or the mod is unloading.
    if (taskbarState->vertical) {
        ClearItemContentWidthLimits(taskbarFrameRepeater);
    }

    double lengthWithoutExtent;
    if (!GetLengthWithoutExtent(xamlRootContent, taskbarState->vertical,
                                &lengthWithoutExtent)) {
        return false;
    }

    UpdateTaskbarFrameRepeaterMargin(taskbarFrameRepeater, taskbarState,
                                     lengthWithoutExtent, /*forceUpdate=*/true);

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

bool IsTaskbarOfWindowVertical(HWND hWnd) {
    auto xamlRoot = GetTaskbarXamlRootOfWindow(hWnd);
    return xamlRoot && GetTaskbarState(xamlRoot)->vertical;
}

// The bounds of the task list of the taskbar that the window belongs to,
// relative to the XAML root content.
bool GetTaskListBounds(HWND hWnd,
                       winrt::Windows::Foundation::Rect* bounds,
                       bool* vertical) {
    auto xamlRoot = GetTaskbarXamlRootOfWindow(hWnd);
    if (!xamlRoot) {
        return false;
    }

    auto taskbarFrameRepeater =
        FindTaskbarFrameRepeater(xamlRoot.Content().try_as<FrameworkElement>());
    if (!taskbarFrameRepeater ||
        !GetBoundsInXamlRoot(taskbarFrameRepeater, bounds)) {
        return false;
    }

    *vertical = GetTaskbarState(xamlRoot)->vertical;
    return true;
}

// Moves the bounds that the taskbar reports for a button group, relative to the
// XAML root content, to the row that the group is displayed in. The taskbar
// converts the bounds from the layout, which places the buttons in a single
// row, as if they were relative to the taskbar frame. Bounds that are already
// within a row aren't moved. Only their position along the row changes, since
// they already span all rows.
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

    TaskbarState* taskbarState = GetTaskbarState(xamlRoot);
    bool vertical = taskbarState->vertical;

    auto taskbarFrame =
        FindChildByClassName(xamlRootContent, L"Taskbar.TaskbarFrame");
    auto taskbarFrameRepeater = FindTaskbarFrameRepeater(xamlRootContent);
    winrt::Windows::Foundation::Rect taskbarFrameBounds;
    double lengthWithoutExtent;
    if (!taskbarFrame || !taskbarFrameRepeater ||
        !GetBoundsInXamlRoot(taskbarFrame, &taskbarFrameBounds) ||
        !GetLengthWithoutExtent(xamlRootContent, vertical,
                                &lengthWithoutExtent) ||
        !(lengthWithoutExtent > 0)) {
        return;
    }

    auto startButton = FindFullHeightStartButton(taskbarFrameRepeater);
    double startButtonLength =
        startButton ? GetElementLength(startButton, vertical) : 0;

    auto layoutRect = SwapAxesIfVertical(*bounds, vertical);
    float frameStart = vertical ? taskbarFrameBounds.Y : taskbarFrameBounds.X;
    layoutRect.X -= frameStart;

    auto displayedRect =
        MoveToDisplayedRow(layoutRect, taskbarState, lengthWithoutExtent,
                           startButtonLength, /*updateRowOffsets=*/false);

    layoutRect.X = displayedRect.X + frameStart;
    *bounds = SwapAxesIfVertical(layoutRect, vertical);
}

// The jump list is placed along the taskbar by the reported bounds of the
// button group, which don't match the displayed buttons: some builds report
// them as if the buttons were laid out in a single row, and a group's buttons
// can span multiple rows. It's placed by the button it's opened for if the jump
// list is opened from the button's context menu request. Otherwise, e.g. for
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
            if (IsTaskbarOfWindowVertical(hWnd)) {
                newRect.Y = g_jumpListButtonBounds.Y;
                newRect.Height = g_jumpListButtonBounds.Height;
            } else {
                newRect.X = g_jumpListButtonBounds.X;
                newRect.Width = g_jumpListButtonBounds.Width;
            }
        } else {
            MoveGroupBoundsToDisplayedRow(hWnd, &newRect);
        }
    } else if (g_inCTaskListThumbnailWnd_DisplayUI ||
               g_inCTaskListThumbnailWnd_LayoutThumbnails) {
        winrt::Windows::Foundation::Rect taskListBounds;
        bool vertical;
        if (GetTaskListBounds(hWnd, &taskListBounds, &vertical)) {
            ExpandBoundsToAllRows(
                &newRect,
                vertical ? newRect.X - taskListBounds.X
                         : newRect.Y - taskListBounds.Y,
                vertical ? taskListBounds.Width : taskListBounds.Height,
                vertical);
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
    double taskListThickness;
    bool vertical;
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

    auto xamlRoot = taskbarFrameRepeater.XamlRoot();
    if (!xamlRoot) {
        return original();
    }

    bool vertical = GetTaskbarState(xamlRoot)->vertical;
    auto offset = element.ActualOffset();

    g_flyoutTargetPosition = TaskListItemPosition{
        .offset = vertical ? offset.x : offset.y,
        .taskListThickness =
            GetElementThickness(taskbarFrameRepeater, vertical),
        .vertical = vertical,
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
                          g_flyoutTargetPosition->taskListThickness,
                          g_flyoutTargetPosition->vertical);
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

    auto xamlRoot = taskbarFrameRepeater.XamlRoot();
    if (!xamlRoot) {
        return original();
    }

    auto xamlRootContent = xamlRoot.Content().try_as<FrameworkElement>();
    if (!xamlRootContent) {
        return original();
    }

    TaskbarState* taskbarState = GetTaskbarState(xamlRoot);
    bool vertical = taskbarState->vertical;

    float layoutThickness = vertical ? g_taskbarCollapsibleLayoutSize.Width
                                     : g_taskbarCollapsibleLayoutSize.Height;

    void* elementAbi = winrt::get_abi(element);
    if (auto it = g_arrangedItems.find(elementAbi);
        it != g_arrangedItems.end() && IsSameSlot(it->second.rect, rect) &&
        it->second.layoutThickness == layoutThickness) {
        return original();
    }

    auto startButton = FindFullHeightStartButton(taskbarFrameRepeater);
    if (element == startButton) {
        return original();
    }

    double startButtonLength =
        startButton ? GetElementLength(startButton, vertical) : 0;

    double lengthWithoutExtent;
    if (!GetLengthWithoutExtent(xamlRootContent, vertical,
                                &lengthWithoutExtent)) {
        return original();
    }

    if (!(lengthWithoutExtent > 0)) {
        Wh_Log(L"Skipping, lengthWithoutExtent=%f", lengthWithoutExtent);
        return original();
    }

    int rows = static_cast<int>(taskbarState->rowOffsetAdjustment.size());
    if (vertical && rows > 1 && layoutThickness > 0) {
        LimitItemContentWidth(element, layoutThickness / rows);
    }

    winrt::Windows::Foundation::Rect displayedRect = MoveToDisplayedRow(
        SwapAxesIfVertical(rect, vertical), taskbarState, lengthWithoutExtent,
        startButtonLength, /*updateRowOffsets=*/true);

    if (displayedRect.X + displayedRect.Width > lengthWithoutExtent) {
        UpdateTaskbarFrameRepeaterMargin(taskbarFrameRepeater, taskbarState,
                                         lengthWithoutExtent);
    }

    winrt::Windows::Foundation::Rect newRect =
        SwapAxesIfVertical(displayedRect, vertical);

    // A rect left as is, e.g. with a single row, can be the layout's rect again
    // after a settings change.
    if (newRect != rect) {
        g_arrangedItems[elementAbi] = {winrt::make_weak(element), newRect,
                                       layoutThickness};
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

    g_taskbarCollapsibleLayoutSize = size;

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
    double lengthWithoutExtent;
    double rowThickness;
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

    auto xamlRoot = taskbarFrameRepeater.XamlRoot();
    if (!xamlRoot) {
        return false;
    }

    auto xamlRootContent = xamlRoot.Content().try_as<FrameworkElement>();
    if (!xamlRootContent) {
        return false;
    }

    TaskbarState* taskbarState = GetTaskbarState(xamlRoot);

    double rowThickness =
        GetElementThickness(taskbarFrameRepeater, taskbarState->vertical) /
        g_settings.rows;
    // Also catches NaN.
    if (!(rowThickness > 0)) {
        return false;
    }

    double lengthWithoutExtent;
    if (!GetLengthWithoutExtent(xamlRootContent, taskbarState->vertical,
                                &lengthWithoutExtent) ||
        !(lengthWithoutExtent > 0)) {
        return false;
    }

    metrics->taskbarState = taskbarState;
    metrics->lengthWithoutExtent = lengthWithoutExtent;
    metrics->rowThickness = rowThickness;
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
        bool vertical = metrics.taskbarState->vertical;
        auto offset = element.ActualOffset();

        g_dragState.active = true;
        g_dragState.vertical = vertical;
        g_dragState.startRow = ClampRow((int)std::lround(
            (vertical ? offset.x : offset.y) / metrics.rowThickness));

        Wh_Log(L"Drag started in row %d (point=%f,%f, rowThickness=%f)",
               g_dragState.startRow, point.X, point.Y, metrics.rowThickness);
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
    float rowPosition = metrics.taskbarState->vertical ? point.X : point.Y;
    int row = ClampRow((int)std::floor(rowPosition / metrics.rowThickness));
    // Clamped again in case the number of rows changed mid-drag.
    int startRow = ClampRow(g_dragState.startRow);

    g_dropPlaceholderOffset = (row - startRow) * metrics.lengthWithoutExtent -
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
        float& dropPosition =
            g_dragState.vertical ? dragBounds.Y : dragBounds.X;
        dropPosition += g_dropPlaceholderOffset;

        // The caller derives the direction from the pointer position, which
        // moves backwards whenever the drop position moves forward into the
        // next row. 1 means forward, 0 means backwards.
        if (g_dragState.hasLastDropPosition &&
            dropPosition != g_dragState.lastDropPosition) {
            rearrangeDirection =
                dropPosition > g_dragState.lastDropPosition ? 1 : 0;
        }

        g_dragState.hasLastDropPosition = true;
        g_dragState.lastDropPosition = dropPosition;
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

    bool vertical = IsVerticalTaskbarFrame(taskbarFrameElement);

    // A frame without a XAML root has no state yet, and no row offsets to
    // apply.
    TaskbarState detachedState{.vertical = vertical};
    auto xamlRoot = taskbarFrameRepeater.XamlRoot();
    TaskbarState* taskbarState =
        xamlRoot ? GetTaskbarState(xamlRoot) : &detachedState;

    double lengthWithoutExtent =
        GetTaskbarFrameLength(taskbarFrameElement, vertical) - value;

    UpdateTaskbarFrameRepeaterMargin(taskbarFrameRepeater, taskbarState,
                                     lengthWithoutExtent);
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
