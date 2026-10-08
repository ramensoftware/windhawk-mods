// ==WindhawkMod==
// @id              taskbar-split-align
// @name            Taskbar Split Alignment
// @name:zh-CN      开始靠左 · 图标居中
// @description     Pins Start through native taskbar layout, removes its space from the centered group, and keeps the Start menu centered.
// @description:zh-CN 开始按钮固定左侧，其余图标重新居中，开始菜单保持居中。
// @version         1.0.0
// @author          caa-siu-cat
// @github          https://github.com/caa-siu-cat
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// SPDX-License-Identifier: GPL-3.0-only
// Derived from Start button always on the left 1.3.3 by Michael Maltsev (m417z).
// Source: https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-start-button-position.wh.cpp
// Local adaptation: only Start is pinned; no Start menu/search flyout hooks;
// right-click Start remains anchored at the left independently of Start menu placement.

// ==WindhawkModReadme==
/*
# 开始靠左 · 图标居中

开始按钮固定到任务栏最左侧；其余图标由 Windows 原生布局重新居中。
开始菜单仍按系统设置居中。请保持 Windows 的任务栏对齐方式为“居中”。

此版本挂钩原生 Arrange 布局，移除开始按钮占用的居中组宽度。
不使用定时器追赶动画，也不反复清零/叠加 Translation。
只识别 AutomationId=StartButton，不以“最左边的图标”猜测目标。
空间不足时给开始按钮保留位置，避免与应用图标重叠。

不修改任务栏透明度。停用插件即可恢复原生布局。
请勿同时启用旧版或其他移动开始按钮的插件。

基于 Michael Maltsev (m417z) 的 Start button always on the left 1.3.3 改编，
保留 GPL-3.0 许可。仅加载到 explorer.exe，不加载到开始菜单进程。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- diagnostics: false
  $name: 布局诊断
  $description: 将按钮坐标记录到插件本地存储，用于验证增删图标时的稳定性。正常使用请关闭。
*/
// ==/WindhawkModSettings==
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <cstdio>
#include <functional>
#include <limits>
#include <optional>
#include <string>

#include <dwmapi.h>
#include <roapi.h>
#include <winstring.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

// The taskbar items live in a WinUI 2 (MUX) ItemsRepeater built on top of
// system XAML. Pull in its projection to enumerate only the realized items
// (ItemsSourceView / TryGetElement), excluding virtualized cache items.
#define WH_WINRT_WINUI2
#include <winrt/Microsoft.UI.Xaml.Controls.h>

using namespace winrt::Windows::UI::Xaml;

struct {
    bool otherSystemButtonsOnTheLeft;
    bool startMenuOnTheLeft;
    bool searchMenuPositionInAllCases;
    bool diagnostics;
} g_settings;

enum class Target {
    Explorer,
    StartMenuExperienceHost,
};

Target g_target;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_unloading;

thread_local bool g_TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride;
thread_local bool g_inShowStartButtonContextMenu;
thread_local winrt::weak_ref<FrameworkElement> g_diagnosticRepeater;

HWND g_searchMenuWnd;
// The search menu's x before it was moved, or y with a vertical taskbar.
int g_searchMenuOriginalPos;
bool g_searchMenuVertical;
HMONITOR g_searchMenuMonitor;

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

// Enumerates the realized children of an ItemsRepeater by index. Unlike walking
// the visual tree (EnumChildElements), this excludes virtualized cache items,
// which aren't part of the live layout. Returns the first child for which the
// callback returns true, or nullptr.
FrameworkElement EnumRepeaterChildElements(
    FrameworkElement repeaterElement,
    std::function<bool(FrameworkElement)> enumCallback) {
    auto repeater =
        repeaterElement
            .try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>();
    if (!repeater) {
        Wh_Log(L"Not an ItemsRepeater");
        return nullptr;
    }

    auto itemsSourceView = repeater.ItemsSourceView();
    int count = itemsSourceView ? itemsSourceView.Count() : 0;

    for (int index = 0; index < count; index++) {
        auto element = repeater.TryGetElement(index);
        if (!element) {
            // Not realized (virtualized away).
            continue;
        }

        auto child = element.try_as<FrameworkElement>();
        if (!child) {
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
    }

    return nullptr;
}

// Taskbar items are laid out left to right, or top to bottom on a vertical
// taskbar, where "left" means the top. The helpers below work along that axis:
// an item's extent is its size along it, and its leading and trailing margins
// are the ones before and after it.

// The taskbar frame's root grid has a visual state for the side the taskbar is
// docked to. Builds without the vertical taskbar may not set a current state.
bool IsVerticalTaskbar(FrameworkElement taskbarFrameRepeater) {
    auto rootGrid = Media::VisualTreeHelper::GetParent(taskbarFrameRepeater)
                        .try_as<FrameworkElement>();
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

double& LeadingMargin(Thickness& margin, bool vertical) {
    return vertical ? margin.Top : margin.Left;
}

double& TrailingMargin(Thickness& margin, bool vertical) {
    return vertical ? margin.Bottom : margin.Right;
}

// The taskbar system buttons that the mod can pin to the left.
enum class SystemButton {
    None,
    Start,
    Widgets,
    Search,
    TaskView,
};

// Number of SystemButton values, for sizing arrays indexed by SystemButton.
constexpr size_t kSystemButtonCount =
    static_cast<size_t>(SystemButton::TaskView) + 1;

// The left-to-right order of the pinned cluster: start, search, task view,
// widgets. Returns -1 for items that aren't part of the cluster.
int SystemButtonClusterRank(SystemButton button) {
    switch (button) {
        case SystemButton::None:
            return -1;
        case SystemButton::Start:
            return 0;
        case SystemButton::Search:
            return 1;
        case SystemButton::TaskView:
            return 2;
        case SystemButton::Widgets:
            return 3;
    }
}

SystemButton IdentifySystemButton(FrameworkElement element) {
    auto className = winrt::get_class_name(element);

    if (className == L"Taskbar.ExperienceToggleButton") {
        auto automationId =
            Automation::AutomationProperties::GetAutomationId(element);
        if (automationId == L"StartButton") {
            return SystemButton::Start;
        }
        if (automationId == L"TaskViewButton") {
            return SystemButton::TaskView;
        }
    } else if (className == L"Taskbar.AugmentedEntryPointButton") {
        if (element.Name() == L"AugmentedEntryPointButton") {
            return SystemButton::Widgets;
        }
    } else if (className == L"Taskbar.TaskbarExtensionElement") {
        return SystemButton::Search;
    }

    return SystemButton::None;
}

// Whether the given button belongs to the left-pinned cluster: the start button
// always, plus the search and task view buttons when the option is on. The
// widgets button is pinned separately and isn't part of this set.
bool IsPinnedClusterButton(SystemButton button) {
    if (button == SystemButton::Start) {
        return true;
    }

    return g_settings.otherSystemButtonsOnTheLeft &&
           (button == SystemButton::Search || button == SystemButton::TaskView);
}

// The extent a cluster button takes up when it's not collapsed. The actual size
// can't be used: it includes the collapse margin (-extent), so it never drops
// below it and grows on every layout pass. The content child's DesiredSize
// doesn't depend on the button's own margin.
double GetClusterButtonExtent(FrameworkElement element, bool vertical) {
    if (Media::VisualTreeHelper::GetChildrenCount(element) > 0) {
        auto child = Media::VisualTreeHelper::GetChild(element, 0)
                         .try_as<FrameworkElement>();
        if (child) {
            auto size = child.DesiredSize();
            return vertical ? size.Height : size.Width;
        }
    }

    return vertical ? element.ActualHeight() : element.ActualWidth();
}

// The offset at which a pinned cluster button goes: the summed extents of the
// pinned buttons before it in cluster order (start at 0, then search, task
// view, widgets), so it's independent of the order the layout arranges its
// children in.
double ComputePinnedSystemButtonOffset(FrameworkElement taskbarFrameRepeater,
                                       SystemButton target,
                                       bool vertical) {
    int targetRank = SystemButtonClusterRank(target);
    if (targetRank <= 0) {
        return 0;
    }

    double offset = 0;
    EnumRepeaterChildElements(
        taskbarFrameRepeater,
        [&offset, targetRank, vertical](FrameworkElement child) {
            SystemButton button = IdentifySystemButton(child);
            int childRank = SystemButtonClusterRank(button);
            if (childRank >= 0 && childRank < targetRank &&
                IsPinnedClusterButton(button)) {
                offset += GetClusterButtonExtent(child, vertical);
            }
            return false;
        });

    return offset;
}

// Whether the widgets button is where Windows pins it when the taskbar items
// are centered: at the leading edge, so that its offset is just its margin.
bool IsWidgetsButtonPinned(FrameworkElement element) {
    auto margin = element.Margin();
    auto offset = element.ActualOffset();
    return offset.x == margin.Left && offset.y == margin.Top;
}

// Last GetTickCount64() at which each pinned button was collapsed, to throttle
// collapses (see UpdatePinnedSystemButtonMargin). Indexed by SystemButton.
ULONGLONG g_lastButtonCollapseTick[kSystemButtonCount];

// Keeps the pinned cluster (start, plus search and task view when the option is
// on) from overlapping the centered group: a button collapses out of the layout
// when there's room and expands to reserve its extent when crowded. Expansions
// are throttled (see below) to avoid oscillation. Runs on the taskbar thread.
void UpdatePinnedSystemButtonMargin(FrameworkElement element) {
    SystemButton self = IdentifySystemButton(element);
    if (g_unloading || !IsPinnedClusterButton(self)) {
        // Unloading, or the option was turned off after this was scheduled;
        // ApplyStyle restores the margins.
        return;
    }

    auto taskbarFrameRepeater =
        Media::VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
    if (!taskbarFrameRepeater) {
        return;
    }

    bool vertical = IsVerticalTaskbar(taskbarFrameRepeater);

    // Measure the pinned set's total extent and the nearest centered item.
    double pinnedExtent = 0;
    double centeredStart = std::numeric_limits<double>::infinity();
    EnumRepeaterChildElements(
        taskbarFrameRepeater, [&](FrameworkElement child) {
            SystemButton button = IdentifySystemButton(child);
            if (IsPinnedClusterButton(button)) {
                pinnedExtent += GetClusterButtonExtent(child, vertical);
            } else if (button != SystemButton::Widgets) {
                auto offset = child.ActualOffset();
                float start = vertical ? offset.y : offset.x;
                if (start >= 0 && start < centeredStart) {
                    centeredStart = start;
                }
            }
            return false;
        });

    Thickness margin = element.Margin();
    double& trailingMargin = TrailingMargin(margin, vertical);
    double extent = GetClusterButtonExtent(element, vertical);

    double newTrailingMargin;
    if (centeredStart < pinnedExtent) {
        newTrailingMargin = 0;  // expand: reserve this button's extent
    } else if (trailingMargin != 0 || centeredStart > pinnedExtent + extent) {
        // collapse out of the group, once there's room for this button's
        // extent to spare
        newTrailingMargin = -extent;
    } else {
        return;  // already collapsed and not crowded
    }

    if (trailingMargin == newTrailingMargin) {
        return;
    }

    if (newTrailingMargin < trailingMargin) {
        // Collapsing gives up this button's reserved extent and shifts the
        // centered group back, which can immediately make expanding look right
        // again. Throttle collapses to at most once a second per button so it
        // settles in the expanded (non-overlapping) state instead of
        // oscillating.
        ULONGLONG now = GetTickCount64();
        ULONGLONG* lastCollapse =
            &g_lastButtonCollapseTick[static_cast<int>(self)];
        if (now - *lastCollapse < 1000) {
            return;
        }
        *lastCollapse = now;
    }

    trailingMargin = newTrailingMargin;
    element.Margin(margin);
}

// Collapses a pinned cluster button (the start button always, plus the search
// and task view buttons when the option is on) out of the centered group;
// IUIElement_Arrange_Hook positions it. Buttons that aren't pinned, and
// everything while unloading, are restored to the centered group. Runs on the
// taskbar thread.
void ApplyClusterButtonCollapse(FrameworkElement element) {
    SystemButton systemButton = IdentifySystemButton(element);
    if (systemButton != SystemButton::Start &&
        systemButton != SystemButton::Search &&
        systemButton != SystemButton::TaskView) {
        return;
    }

    auto taskbarFrameRepeater =
        Media::VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
    if (!taskbarFrameRepeater) {
        return;
    }

    bool vertical = IsVerticalTaskbar(taskbarFrameRepeater);

    Thickness margin = element.Margin();
    Thickness newMargin = margin;

    // Restore only the collapse applied by the mod. Check both axes, since
    // the taskbar orientation can change.
    newMargin.Right = std::max(newMargin.Right, 0.0);
    newMargin.Bottom = std::max(newMargin.Bottom, 0.0);

    if (IsPinnedClusterButton(systemButton) && !g_unloading) {
        TrailingMargin(newMargin, vertical) =
            -GetClusterButtonExtent(element, vertical);
    }

    if (newMargin == margin) {
        return;
    }

    Wh_Log(
        L"Collapsing system button %d: margin.Right=%.1f, margin.Bottom=%.1f",
        (int)systemButton, newMargin.Right, newMargin.Bottom);
    element.Margin(newMargin);
}

// Pins the widgets button after the start button (or the whole cluster, when
// the option is on) via its leading margin, or restores it while unloading.
// Windows pins it at the leading edge where the start button goes, so it always
// needs nudging. Runs on the taskbar thread.
void ApplyWidgetMargin(FrameworkElement element) {
    auto taskbarFrameRepeater =
        Media::VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
    if (!taskbarFrameRepeater) {
        return;
    }

    bool vertical = IsVerticalTaskbar(taskbarFrameRepeater);

    double leadingMargin = g_unloading ? 0
                                       : ComputePinnedSystemButtonOffset(
                                             taskbarFrameRepeater,
                                             SystemButton::Widgets, vertical);

    Thickness margin = element.Margin();
    Thickness newMargin = margin;
    // Clear the margin applied for the other taskbar orientation.
    LeadingMargin(newMargin, !vertical) = 0;
    LeadingMargin(newMargin, vertical) = leadingMargin;

    if (newMargin != margin) {
        element.Margin(newMargin);
    }
}

// Runs one of the margin updaters on the taskbar thread, deferred off the
// current layout pass (changing margins during arrange would re-enter layout).
void ScheduleOnTaskbarThread(FrameworkElement element,
                             void (*func)(FrameworkElement)) {
    element.Dispatcher().TryRunAsync(
        winrt::Windows::UI::Core::CoreDispatcherPriority::High,
        [element, func]() { func(element); });
}

bool ApplyStyle(XamlRoot xamlRoot) {
    FrameworkElement xamlRootContent =
        xamlRoot.Content().try_as<FrameworkElement>();

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

    auto widgetElement = EnumRepeaterChildElements(
        taskbarFrameRepeater, [](FrameworkElement child) {
            return IdentifySystemButton(child) == SystemButton::Widgets &&
                   IsWidgetsButtonPinned(child);
        });
    if (widgetElement) {
        ApplyWidgetMargin(widgetElement);
    }

    EnumRepeaterChildElements(taskbarFrameRepeater, [](FrameworkElement child) {
        ApplyClusterButtonCollapse(child);
        return false;
    });

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
}

void ApplySettings(HWND hTaskbarWnd) {
    RunFromWindowThread(
        hTaskbarWnd, [](void* pParam) { ApplySettingsFromTaskbarThread(); }, 0);
}

using IUIElement_Arrange_t =
    HRESULT(WINAPI*)(void* pThis, winrt::Windows::Foundation::Rect rect);
IUIElement_Arrange_t IUIElement_Arrange_Original;
HRESULT WINAPI IUIElement_Arrange_Hook(void* pThis,
                                       winrt::Windows::Foundation::Rect rect) {
    Wh_Log(L">");

    auto original = [=] { return IUIElement_Arrange_Original(pThis, rect); };

    if (!g_TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride || g_unloading) {
        return original();
    }

    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));
    if (!element) {
        return original();
    }

    SystemButton systemButton = IdentifySystemButton(element);

    // The widgets button needs repositioning whether or not the option is on,
    // so handle it before the pinned-cluster check below.
    if (systemButton == SystemButton::Widgets) {
        ScheduleOnTaskbarThread(element, ApplyWidgetMargin);
        return original();
    }

    // The pinned cluster (the start button, plus search and task view when the
    // option is on) is moved to the left below. Everything else (the app
    // buttons) is left alone.
    if (!IsPinnedClusterButton(systemButton)) {
        return original();
    }

    auto taskbarFrameRepeater =
        Media::VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
    if (!taskbarFrameRepeater) {
        return original();
    }

    bool vertical = IsVerticalTaskbar(taskbarFrameRepeater);

    // A collapse along the other axis is left over from before the taskbar
    // orientation changed.
    Thickness margin = element.Margin();
    if (TrailingMargin(margin, !vertical) < 0) {
        ScheduleOnTaskbarThread(element, ApplyClusterButtonCollapse);
    }

    // Find the widgets button at its left-pinned position. When present, it
    // sits right of the start button (or cluster) and anchors it against the
    // centered group.
    auto widgetElement = EnumRepeaterChildElements(
        taskbarFrameRepeater, [](FrameworkElement child) {
            return IdentifySystemButton(child) == SystemButton::Widgets &&
                   IsWidgetsButtonPinned(child);
        });

    // Without that anchor, adjust the margin so the start button (or cluster)
    // doesn't overlap the centered group.
    if (!widgetElement) {
        ScheduleOnTaskbarThread(element, UpdatePinnedSystemButtonMargin);
    }

    // Pin it to the left in cluster order: the start button gets offset 0,
    // then search, then task view.
    double offset = ComputePinnedSystemButtonOffset(taskbarFrameRepeater,
                                                    systemButton, vertical);

    Wh_Log(L"Pinning system button %d to %s=%.1f", (int)systemButton,
           vertical ? L"y" : L"x", offset);

    winrt::Windows::Foundation::Rect newRect = rect;
    if (vertical) {
        newRect.Y = offset;
    } else {
        newRect.X = offset;
    }
    if (g_settings.diagnostics) {
        g_diagnosticRepeater = taskbarFrameRepeater;
    }
    return IUIElement_Arrange_Original(pThis, newRect);
}

// Optional, event-driven verification. Records only geometry, never app names
// or window contents. Disabled during normal use; no timer or worker thread.
void RecordLayoutDiagnostic() noexcept {
    if (!g_settings.diagnostics || g_unloading) {
        return;
    }
    try {
        auto repeater = g_diagnosticRepeater.get();
        if (!repeater) {
            return;
        }
        double start = -1, first = std::numeric_limits<double>::infinity();
        double last = 0;
        int count = 0;
        auto root = repeater.XamlRoot();
        EnumRepeaterChildElements(repeater, [&](FrameworkElement item) {
            if (item.Visibility() != Visibility::Visible || item.ActualWidth() <= 0) {
                return false;
            }
            auto button = IdentifySystemButton(item);
            auto point = item.TransformToVisual(root.Content().as<UIElement>())
                             .TransformPoint({0, 0});
            if (button == SystemButton::Start) {
                start = point.X;
            } else if (button != SystemButton::Widgets && point.X >= 0) {
                first = std::min(first, static_cast<double>(point.X));
                last = std::max(last, static_cast<double>(point.X) + item.ActualWidth());
                ++count;
            }
            return false;
        });
        wchar_t path[MAX_PATH];
        if (!Wh_GetModStoragePath(path, ARRAYSIZE(path))) {
            return;
        }
        std::wstring tracePath = std::wstring(path) + L"\\layout-trace.tsv";
        HANDLE file = CreateFileW(tracePath.c_str(), FILE_APPEND_DATA,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                  OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file == INVALID_HANDLE_VALUE) {
            return;
        }
        char row[256];
        int length = snprintf(row, sizeof(row), "%llu\t%.3f\t%.3f\t%.3f\t%.3f\t%d\n",
                              GetTickCount64(), root.Size().Width, start,
                              count ? first : -1, count ? last : -1, count);
        if (length > 0 && length < static_cast<int>(sizeof(row))) {
            DWORD written;
            WriteFile(file, row, length, &written, nullptr);
        }
        CloseHandle(file);
    } catch (...) {
        // Diagnostics must never affect the shell layout.
    }
}

// The layout of MSVC's std::vector, which the mod's own STL may not match.
struct MsvcRectVector {
    winrt::Windows::Foundation::Rect* first;
    winrt::Windows::Foundation::Rect* last;
    winrt::Windows::Foundation::Rect* end;
};

// The per-item bounds recorded by the taskbar layout, which the taskbar reports
// to the shell, e.g. to anchor the search flyout to the search box. Depending
// on the Windows version, they're the layout slots rather than where the items
// were arranged, so the pinned buttons get their arranged bounds.
using TaskbarFrame_ChildItemBounds_t = MsvcRectVector*(WINAPI*)(void* pThis);
TaskbarFrame_ChildItemBounds_t TaskbarFrame_ChildItemBounds_Original;
MsvcRectVector* WINAPI TaskbarFrame_ChildItemBounds_Hook(void* pThis) {
    MsvcRectVector* bounds = TaskbarFrame_ChildItemBounds_Original(pThis);
    if (g_unloading) {
        return bounds;
    }

    FrameworkElement taskbarFrame = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarFrame));
    if (!taskbarFrame) {
        return bounds;
    }

    FrameworkElement child = taskbarFrame;
    if (!(child = FindChildByName(child, L"RootGrid")) ||
        !(child = FindChildByName(child, L"TaskbarFrameRepeater"))) {
        return bounds;
    }

    auto repeater =
        child.try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>();
    if (!repeater) {
        return bounds;
    }

    // The bounds are indexed by item index.
    int count = static_cast<int>(bounds->last - bounds->first);
    for (int index = 0; index < count; index++) {
        auto element = repeater.TryGetElement(index).try_as<FrameworkElement>();
        if (!element || !IsPinnedClusterButton(IdentifySystemButton(element))) {
            continue;
        }

        auto offset = element.ActualOffset();
        auto size = element.ActualSize();
        bounds->first[index] = {offset.x, offset.y, size.x, size.y};
    }

    return bounds;
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

    // Arrange can be nested. Restore its previous state on every exit.
    const bool previousArrange = g_TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride;
    g_TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride = true;
    struct ArrangeGuard {
        bool previous;
        ~ArrangeGuard() {
            g_TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride = previous;
        }
    } restoreArrange{previousArrange};

    HRESULT ret = TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original(
        pThis, context, size, resultSize);

    RecordLayoutDiagnostic();

    return ret;
}

using ExperienceToggleButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
ExperienceToggleButton_UpdateButtonPadding_t
    ExperienceToggleButton_UpdateButtonPadding_Original;
void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(void* pThis) {
    Wh_Log(L">");

    ExperienceToggleButton_UpdateButtonPadding_Original(pThis);

    if (g_unloading) {
        return;
    }

    FrameworkElement toggleButtonElement = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(toggleButtonElement));
    if (!toggleButtonElement) {
        return;
    }

    auto panelElement =
        FindChildByName(toggleButtonElement, L"ExperienceToggleButtonRootPanel")
            .try_as<Controls::Grid>();
    if (!panelElement) {
        return;
    }

    auto className = winrt::get_class_name(toggleButtonElement);
    if (className == L"Taskbar.ExperienceToggleButton") {
        auto automationId = Automation::AutomationProperties::GetAutomationId(
            toggleButtonElement);
        if (automationId == L"StartButton") {
            // Start button properties differ depending on whether Explorer is
            // started with centered icons or left-aligned icons. This seems to
            // be a bug in Explorer. Compare the start button in these two
            // cases:
            // 1. Left-align in settings, restart Explorer.
            // 2. Center-align in settings, restart Explorer, left-align.
            //
            // You can see that in the second case, the start button lacks the
            // padding on the left.
            //
            // This workaround adds this padding.
            if (panelElement.Width() == 45) {
                panelElement.Width(55);
            }

            if (panelElement.Padding() == Thickness{2, 4, 2, 4}) {
                panelElement.Padding(Thickness{12, 4, 2, 4});
            }
        }
    }
}

// The start button context menu is centered over the start button or aligned to
// its leading edge depending on the taskbar alignment (TaskbarFrame's
// Alignment, where Left=0 and Center=1). Both the placement mode and the anchor
// position are derived from it. With the start button forced to the left, the
// menu should align to the button's leading edge, so report left alignment
// while the menu is being shown, letting the taskbar's own left-alignment code
// position the menu.
using TaskbarFrame_get_Alignment_t = HRESULT(WINAPI*)(void* pThis,
                                                      int* alignment);
TaskbarFrame_get_Alignment_t TaskbarFrame_get_Alignment_Original;
HRESULT WINAPI TaskbarFrame_get_Alignment_Hook(void* pThis, int* alignment) {
    HRESULT hr = TaskbarFrame_get_Alignment_Original(pThis, alignment);
    if (SUCCEEDED(hr) && !g_unloading && 
        g_inShowStartButtonContextMenu) {
        *alignment = 0;  // TaskbarAlignment::Left
    }

    return hr;
}

// The alignment above is read while the context menu coroutine resumes (after
// the menu items are fetched asynchronously), not during the initial call, so
// bracket the override around the whole resume.
using ShowStartButtonContextMenuResumeCoro_t = void(WINAPI*)(void* coroFrame);
ShowStartButtonContextMenuResumeCoro_t
    ShowStartButtonContextMenuResumeCoro_Original;
void WINAPI ShowStartButtonContextMenuResumeCoro_Hook(void* coroFrame) {
    Wh_Log(L">");

    bool prev = g_inShowStartButtonContextMenu;
    g_inShowStartButtonContextMenu = true;
    ShowStartButtonContextMenuResumeCoro_Original(coroFrame);
    g_inShowStartButtonContextMenu = prev;
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
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
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
            {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateButtonPadding(void))"},
            &ExperienceToggleButton_UpdateButtonPadding_Original,
            ExperienceToggleButton_UpdateButtonPadding_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Taskbar::ITaskbarFrame>::get_Alignment(int *))"},
            &TaskbarFrame_get_Alignment_Original,
            TaskbarFrame_get_Alignment_Hook,
        },
        {
            {LR"(static  winrt::Taskbar::implementation::ContextMenus::ShowStartButtonContextMenuAsync$_ResumeCoro$1())"},
            &ShowStartButtonContextMenuResumeCoro_Original,
            ShowStartButtonContextMenuResumeCoro_Hook,
        },
        {
            {LR"(public: class std::vector<struct winrt::Windows::Foundation::Rect,class std::allocator<struct winrt::Windows::Foundation::Rect> > const & __cdecl winrt::Taskbar::implementation::TaskbarFrame::ChildItemBounds(void)const )"},
            &TaskbarFrame_ChildItemBounds_Original,
            TaskbarFrame_ChildItemBounds_Hook,
            true,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
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
    g_settings.otherSystemButtonsOnTheLeft = false;
    g_settings.startMenuOnTheLeft = false;
    g_settings.searchMenuPositionInAllCases = false;
    g_settings.diagnostics = Wh_GetIntSetting(L"diagnostics") != 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_target = Target::Explorer;
    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"Taskbar symbols unavailable; leaving native layout unchanged");
        return FALSE;
    }
    if (HMODULE module = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(module)) {
            return FALSE;
        }
    } else {
        HMODULE kernelBase = GetModuleHandle(L"kernelbase.dll");
        auto loadLibrary = (decltype(&LoadLibraryExW))GetProcAddress(kernelBase, "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(loadLibrary, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
    }
    Wh_Log(L"Stable split alignment initialized; Start menu remains system-centered");
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_taskbarViewDllLoaded) {
        if (HMODULE module = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true) && HookTaskbarViewDllSymbols(module)) {
                Wh_ApplyHookOperations();
            }
        }
    }
    if (HWND taskbar = FindCurrentProcessTaskbarWnd()) {
        ApplySettings(taskbar);
    }
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    if (HWND taskbar = FindCurrentProcessTaskbarWnd()) {
        ApplySettings(taskbar);
    }
}

void Wh_ModUninit() {}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    // Reload gives all taskbar-thread callbacks a consistent settings snapshot.
    *reload = TRUE;
    return TRUE;
}
