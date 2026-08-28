// ==WindhawkMod==
// @id              taskbar-multirow
// @name            Multirow taskbar for Windows 11
// @description     Span taskbar items across multiple rows, just like it was possible before Windows 11
// @version         1.1.3
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
#include <unordered_map>
#include <vector>

#include <windowsx.h>

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

// Set while the taskbar calculates the drop position of a dragged item, holding
// the horizontal distance between the row the item is dragged over and the
// single row the taskbar lays items out in.
thread_local bool g_hasDropPlaceholderOffset;
thread_local float g_dropPlaceholderOffset;

struct TaskbarState {
    winrt::weak_ref<XamlRoot> xamlRoot;
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

    // The vector is sized from a separate read of the row count setting, which
    // can change while the taskbar thread is using it.
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

bool ApplyStyle(XamlRoot xamlRoot) {
    TaskbarState* taskbarState = GetTaskbarState(xamlRoot);

    auto xamlRootContent = xamlRoot.Content().as<FrameworkElement>();

    FrameworkElement taskbarFrameRepeater = nullptr;

    FrameworkElement child = xamlRootContent;
    if (child &&
        (child = FindChildByClassName(child, L"Taskbar.TaskbarFrame")) &&
        (child = FindChildByName(child, L"RootGrid")) &&
        (child = FindChildByName(child, L"TaskbarFrameRepeater"))) {
        taskbarFrameRepeater = child;
    }

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

    HRESULT ret = CTaskListWnd_ComputeJumpViewPosition_Original(
        pThis, taskBtnGroup, param2, point, horizontalAlignment,
        verticalAlignment);

    DWORD messagePos = GetMessagePos();
    POINT pt{
        GET_X_LPARAM(messagePos),
        GET_Y_LPARAM(messagePos),
    };

    point->X = pt.x;

    return ret;
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

    FrameworkElement startButton = nullptr;
    if (g_settings.fullHeightStartButton) {
        startButton =
            EnumChildElements(taskbarFrameRepeater, [](FrameworkElement child) {
                auto childClassName = winrt::get_class_name(child);
                if (childClassName != L"Taskbar.ExperienceToggleButton") {
                    return false;
                }

                auto automationId =
                    Automation::AutomationProperties::GetAutomationId(child);
                return automationId == L"StartButton";
            });
        if (element == startButton) {
            return original();
        }
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

    winrt::Windows::Foundation::Rect newRect = rect;
    newRect.Height /= g_settings.rows;
    for (int i = 0; i < g_settings.rows - 1 &&
                    newRect.X + newRect.Width > widthWithoutExtent;
         i++) {
        newRect.X -= widthWithoutExtent;
        if (newRect.X <= 0) {
            taskbarState->rowOffsetAdjustment[i] =
                -newRect.X + startButtonWidth;
            newRect.X = startButtonWidth;
        } else {
            newRect.X += taskbarState->rowOffsetAdjustment[i];
        }

        newRect.Y += newRect.Height;
    }

    if (newRect.X + newRect.Width > widthWithoutExtent) {
        UpdateTaskbarFrameRepeaterMargin(taskbarFrameRepeater, taskbarState,
                                         widthWithoutExtent);
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

        WindhawkUtils::SetFunctionHook(arrange, IUIElement_Arrange_Hook,
                                       &IUIElement_Arrange_Original);
        Wh_ApplyHookOperations();
        return true;
    }();

    ScopedFlag scopedFlag(
        &g_inTaskbarCollapsibleLayoutXamlTraits_ArrangeOverride);

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
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarCollapsibleLayout,struct winrt::Microsoft::UI::Xaml::Controls::IVirtualizingLayoutOverrides>::ArrangeOverride(void *,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
            &TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original,
            TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook,
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
            {LR"(public: struct winrt::Taskbar::implementation::DropPlaceholder __cdecl winrt::Taskbar::implementation::TaskListDragOperation::GetDropPlaceholder(class std::vector<struct winrt::Windows::Foundation::Rect,class std::allocator<struct winrt::Windows::Foundation::Rect> > const &,struct std::pair<unsigned int,unsigned int>,struct winrt::Windows::Foundation::Rect,enum winrt::Taskbar::implementation::RearrangeDirection,struct winrt::Taskbar::implementation::GroupBoundsCalculator const *,bool))"},
            &TaskListDragOperation_GetDropPlaceholder_Original,
            TaskListDragOperation_GetDropPlaceholder_Hook,
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
