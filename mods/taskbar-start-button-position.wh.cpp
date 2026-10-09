// ==WindhawkMod==
// @id              taskbar-start-button-position
// @name            Start button always on the left
// @description     Forces the Start button to be on the left of the taskbar, even when taskbar icons are centered, with an option to also move the search and task view buttons (Windows 11 only)
// @version         1.3.3
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         StartMenuExperienceHost.exe
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject
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
# Start button always on the left

Forces the Start button to be on the left of the taskbar, even when taskbar
icons are centered.

There's also an option to move the search and task view buttons to the left,
keeping only the app icons centered.

With a vertical taskbar, the buttons and the Start menu are moved to the top
instead.

Only Windows 11 is supported.

![Screenshot](https://i.imgur.com/MSKYKbE.png) \
_Start button on the left_

![Screenshot](https://i.imgur.com/SOdWH1P.png) \
_Start button, search and task view buttons on the left_
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- otherSystemButtonsOnTheLeft: false
  $name: Move other system buttons to the left
  $description: >-
    In addition to the Start button, also move the search and task view buttons
    to the left, keeping only the app icons centered.
- startMenuOnTheLeft: true
  $name: Start menu on the left
  $description: >-
    Make the start menu open on the left even if taskbar icons are centered.
- searchMenuPositionInAllCases: false
  $name: Position the search menu in all cases
  $description: >-
    By default, the search menu is only repositioned when it's opened from the
    Start menu, not when it's opened in other ways, such as with the Win+S
    shortcut or the taskbar search icon. Enable this option to reposition it in
    all cases.

    Only applies when the "Start menu on the left" option is enabled.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cstdlib>
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
    return IUIElement_Arrange_Original(pThis, newRect);
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

    g_TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride = true;

    HRESULT ret = TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original(
        pThis, context, size, resultSize);

    g_TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride = false;

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
    if (SUCCEEDED(hr) && !g_unloading && g_settings.startMenuOnTheLeft &&
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

std::wstring GetProcessFileName(DWORD dwProcessId) {
    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
    if (!hProcess) {
        return std::wstring{};
    }

    WCHAR processPath[MAX_PATH];

    DWORD dwSize = ARRAYSIZE(processPath);
    if (!QueryFullProcessImageName(hProcess, 0, processPath, &dwSize)) {
        CloseHandle(hProcess);
        return std::wstring{};
    }

    CloseHandle(hProcess);

    PCWSTR processFileName = wcsrchr(processPath, L'\\');
    if (!processFileName) {
        return std::wstring{};
    }

    processFileName++;
    return processFileName;
}

bool IsStartMenuOpen() {
    bool open = false;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0 ||
                _wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") != 0) {
                return TRUE;
            }

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId)) {
                return TRUE;
            }

            std::wstring processFileName = GetProcessFileName(dwProcessId);
            if (_wcsicmp(processFileName.c_str(),
                         L"StartMenuExperienceHost.exe") != 0) {
                return TRUE;
            }

            // The start menu window stays cloaked while hidden and is uncloaked
            // while shown.
            BOOL cloaked = FALSE;
            if (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked,
                                                sizeof(cloaked))) &&
                !cloaked) {
                *(bool*)lParam = true;
            }

            return FALSE;
        },
        (LPARAM)&open);

    return open;
}

HWND GetTaskbarForMonitor(HMONITOR monitor) {
    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    if (!hTaskbarWnd) {
        return nullptr;
    }

    HMONITOR taskbarMonitor = (HMONITOR)GetProp(hTaskbarWnd, L"TaskbarMonitor");
    if (taskbarMonitor == monitor) {
        return hTaskbarWnd;
    }

    DWORD taskbarThreadId = GetWindowThreadProcessId(hTaskbarWnd, nullptr);
    if (!taskbarThreadId) {
        return nullptr;
    }

    struct EnumData {
        HMONITOR monitor;
        HWND result;
    } enumData = {monitor, nullptr};

    EnumThreadWindows(
        taskbarThreadId,
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& data = *reinterpret_cast<EnumData*>(lParam);

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") != 0) {
                return TRUE;
            }

            HMONITOR taskbarMonitor =
                (HMONITOR)GetProp(hWnd, L"TaskbarMonitor");
            if (taskbarMonitor != data.monitor) {
                return TRUE;
            }

            data.result = hWnd;
            return FALSE;
        },
        reinterpret_cast<LPARAM>(&enumData));

    return enumData.result;
}

// A vertical taskbar window is taller than it's wide, also while auto-hidden.
bool IsVerticalTaskbarOnMonitor(HMONITOR monitor) {
    HWND hTaskbarWnd = GetTaskbarForMonitor(monitor);
    RECT rect;
    if (!hTaskbarWnd || !GetWindowRect(hTaskbarWnd, &rect)) {
        return false;
    }

    return rect.bottom - rect.top > rect.right - rect.left;
}

using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_Original;
HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND hwnd,
                                          DWORD dwAttribute,
                                          LPCVOID pvAttribute,
                                          DWORD cbAttribute) {
    auto original = [=]() {
        return DwmSetWindowAttribute_Original(hwnd, dwAttribute, pvAttribute,
                                              cbAttribute);
    };

    if (dwAttribute != DWMWA_CLOAK || cbAttribute != sizeof(BOOL)) {
        return original();
    }

    BOOL cloak = *(BOOL*)pvAttribute;

    Wh_Log(L"> %08X %s", (DWORD)(DWORD_PTR)hwnd, cloak ? L"cloak" : L"uncloak");

    DWORD processId = 0;
    if (!hwnd || !GetWindowThreadProcessId(hwnd, &processId)) {
        return original();
    }

    std::wstring processFileName = GetProcessFileName(processId);

    enum class DwmTarget {
        SearchHost,
    };
    DwmTarget target;

    if (_wcsicmp(processFileName.c_str(), L"SearchHost.exe") == 0) {
        target = DwmTarget::SearchHost;
    } else {
        return original();
    }

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(monitor, &monitorInfo);

    RECT targetRect;
    if (!GetWindowRect(hwnd, &targetRect)) {
        return original();
    }

    int x = targetRect.left;
    int y = targetRect.top;
    int cx = targetRect.right - targetRect.left;
    int cy = targetRect.bottom - targetRect.top;

    if (target == DwmTarget::SearchHost) {
        // Only change the position along the taskbar: x, or y with a vertical
        // taskbar.
        bool vertical;
        int newPos;

        if (g_settings.startMenuOnTheLeft && !cloak &&
            (g_settings.searchMenuPositionInAllCases || IsStartMenuOpen())) {
            vertical = IsVerticalTaskbarOnMonitor(monitor);
            int pos = vertical ? y : x;
            int workAreaStart =
                vertical ? monitorInfo.rcWork.top : monitorInfo.rcWork.left;

            // Only a window centered on the monitor is moved (within a pixel,
            // to allow for rounding). One positioned by the search box or
            // button, or already moved, stays.
            int monitorStart = vertical ? monitorInfo.rcMonitor.top
                                        : monitorInfo.rcMonitor.left;
            int monitorEnd = vertical ? monitorInfo.rcMonitor.bottom
                                      : monitorInfo.rcMonitor.right;
            int size = vertical ? cy : cx;
            int centeredPos =
                monitorStart + (monitorEnd - monitorStart - size) / 2;
            if (std::abs(pos - centeredPos) > 1) {
                return original();
            }

            newPos = workAreaStart;
            g_searchMenuWnd = hwnd;
            g_searchMenuOriginalPos = pos;
            g_searchMenuVertical = vertical;
            g_searchMenuMonitor = monitor;
        } else {
            if (!g_searchMenuOriginalPos) {
                return original();
            }

            vertical = g_searchMenuVertical;
            newPos = g_searchMenuOriginalPos;
            bool monitorMatches = monitor == g_searchMenuMonitor;

            g_searchMenuWnd = nullptr;
            g_searchMenuOriginalPos = 0;
            g_searchMenuMonitor = nullptr;

            if (!monitorMatches) {
                return original();
            }
        }

        int& pos = vertical ? y : x;
        if (newPos == pos) {
            return original();
        }

        Wh_Log(L"Adjusting search menu %s: %d -> %d", vertical ? L"y" : L"x",
               pos, newPos);

        pos = newPos;
    }

    SetWindowPos(hwnd, nullptr, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);

    return original();
}

using SearchBoxOnTaskbarSearchAppPositioner_GetAppRectForSearchBoxOnTaskbar_t =
    RECT*(WINAPI*)(void* pThis,
                   RECT* result,
                   const void* monitorInfo,
                   bool rtl,
                   int width,
                   int height,
                   bool fullWidth);
SearchBoxOnTaskbarSearchAppPositioner_GetAppRectForSearchBoxOnTaskbar_t
    SearchBoxOnTaskbarSearchAppPositioner_GetAppRectForSearchBoxOnTaskbar;

using Mirror_IsThreadRTL_t = int(WINAPI*)();
Mirror_IsThreadRTL_t Mirror_IsThreadRTL;

// Depending on how the search app is activated, the search window positioner
// places the window by the search button and then, for a center-aligned
// taskbar, centers it on the monitor, instead of using its usual position, such
// as by the search box. With the search button pinned to the left, the usual
// position is used, which is also where the search app expects the search box
// to be. A vertical taskbar has no search box, and the window stays centered.
// When opened from the Start menu, the window is left as is, to be positioned
// along with the Start menu.
using SearchBoxOnTaskbarSearchAppPositioner_AdjustAppRectForCenterAlignedTaskbar_t =
    void(WINAPI*)(void* pThis,
                  RECT* appRect,
                  const void* monitorInfo,
                  int width);
SearchBoxOnTaskbarSearchAppPositioner_AdjustAppRectForCenterAlignedTaskbar_t
    SearchBoxOnTaskbarSearchAppPositioner_AdjustAppRectForCenterAlignedTaskbar_Original;
void WINAPI
SearchBoxOnTaskbarSearchAppPositioner_AdjustAppRectForCenterAlignedTaskbar_Hook(
    void* pThis,
    RECT* appRect,
    const void* monitorInfo,
    int width) {
    RECT originalRect = *appRect;

    SearchBoxOnTaskbarSearchAppPositioner_AdjustAppRectForCenterAlignedTaskbar_Original(
        pThis, appRect, monitorInfo, width);

    if (!g_settings.otherSystemButtonsOnTheLeft || g_unloading) {
        return;
    }

    // Not centered horizontally, e.g. with a vertical taskbar.
    if (appRect->left == originalRect.left || IsStartMenuOpen()) {
        return;
    }

    RECT rect;
    SearchBoxOnTaskbarSearchAppPositioner_GetAppRectForSearchBoxOnTaskbar(
        pThis, &rect, monitorInfo, Mirror_IsThreadRTL() != 0, width, 0, false);

    // The usual positioning centers the window if it doesn't fit by the search
    // box. The monitor info starts with the monitor rect.
    const RECT* monitorRect = (const RECT*)monitorInfo;
    if (rect.left < monitorRect->left || rect.right > monitorRect->right) {
        return;
    }

    appRect->left = rect.left;
}

bool HookTwinuiPcshellSymbols() {
    HMODULE module = LoadLibraryEx(L"twinui.pcshell.dll", nullptr,
                                   LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load twinui.pcshell.dll");
        return false;
    }

    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK twinuiPcshellHooks[] = {
        {
            {LR"(private: struct tagRECT __cdecl SearchBoxOnTaskbarSearchAppPositioner::GetAppRectForSearchBoxOnTaskbar(struct MonitorInfo const &,bool,int,int,bool))"},
            &SearchBoxOnTaskbarSearchAppPositioner_GetAppRectForSearchBoxOnTaskbar,
        },
        {
            {LR"(int __cdecl Mirror_IsThreadRTL(void))"},
            &Mirror_IsThreadRTL,
        },
        {
            {LR"(private: void __cdecl SearchBoxOnTaskbarSearchAppPositioner::AdjustAppRectForCenterAlignedTaskbar(struct tagRECT *,struct MonitorInfo,int))"},
            &SearchBoxOnTaskbarSearchAppPositioner_AdjustAppRectForCenterAlignedTaskbar_Original,
            SearchBoxOnTaskbarSearchAppPositioner_AdjustAppRectForCenterAlignedTaskbar_Hook,
        },
    };

    return HookSymbols(module, twinuiPcshellHooks,
                       ARRAYSIZE(twinuiPcshellHooks));
}

namespace StartMenuUI {

// Overrides a property's local value, keeping the latest local value set by
// Windows to restore it, or to clear the property if there's none.
template <typename T>
class PropertyOverride {
   public:
    using PropertyGetter = DependencyProperty (*)();

    explicit PropertyOverride(PropertyGetter property) : m_property(property) {}

    void Set(DependencyObject element, T value) {
        auto property = m_property();
        auto localValue = element.ReadLocalValue(property).try_as<T>();
        if (m_element.get() != element || localValue != m_value) {
            m_element = element;
            m_windowsValue = localValue;
        }

        m_value = value;
        element.SetValue(property, winrt::box_value(value));
    }

    void Restore() {
        auto element = m_element.get();
        m_element = nullptr;
        if (!element) {
            return;
        }

        auto property = m_property();
        if (element.ReadLocalValue(property).try_as<T>() != m_value) {
            return;
        }

        if (m_windowsValue) {
            element.SetValue(property, winrt::box_value(*m_windowsValue));
        } else {
            element.ClearValue(property);
        }
    }

    // Stops overriding, leaving the current value.
    void Release() { m_element = nullptr; }

   private:
    PropertyGetter m_property;
    winrt::weak_ref<DependencyObject> m_element;
    std::optional<T> m_windowsValue;
    T m_value{};
};

// The Start menu is moved to the left, or to the top with a vertical taskbar,
// which has the Start menu next to it. Its position along the other axis is
// left to Windows, which sets it for the side the taskbar is docked to, also
// when the taskbar orientation changes.
bool g_inApplyStyle;
PropertyOverride<double> g_canvasTopOverride{&Controls::Canvas::TopProperty};
PropertyOverride<double> g_canvasLeftOverride{&Controls::Canvas::LeftProperty};
PropertyOverride<VerticalAlignment> g_verticalAlignmentOverride{
    &FrameworkElement::VerticalAlignmentProperty};
PropertyOverride<HorizontalAlignment> g_horizontalAlignmentOverride{
    &FrameworkElement::HorizontalAlignmentProperty};
PropertyOverride<Thickness> g_marginOverride{&FrameworkElement::MarginProperty};
winrt::weak_ref<DependencyObject> g_startSizingFrameWeakRef;
int64_t g_canvasTopPropertyChangedToken;
int64_t g_canvasLeftPropertyChangedToken;
winrt::weak_ref<DependencyObject> g_frameRootWeakRef;
int64_t g_verticalAlignmentPropertyChangedToken;
int64_t g_horizontalAlignmentPropertyChangedToken;
winrt::event_token g_visibilityChangedToken;

HWND GetCoreWnd() {
    struct ENUM_WINDOWS_PARAM {
        HWND* hWnd;
    };

    HWND hWnd = nullptr;
    ENUM_WINDOWS_PARAM param = {&hWnd};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            if (_wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") == 0) {
                *param.hWnd = hWnd;
                return FALSE;
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnd;
}

void ApplyStyle();

void ApplyStyleClassicStartMenu(FrameworkElement content,
                                bool verticalTaskbar) {
    FrameworkElement startSizingFrame =
        FindChildByClassName(content, L"StartDocked.StartSizingFrame");
    if (!startSizingFrame) {
        Wh_Log(L"Failed to find StartDocked.StartSizingFrame");
        return;
    }

    if (g_unloading) {
        g_canvasLeftOverride.Restore();
        g_canvasTopOverride.Restore();
        return;
    }

    constexpr int kStartMenuMargin = 12;

    if (verticalTaskbar) {
        g_canvasLeftOverride.Release();
        Wh_Log(L"Setting Canvas.Top to %d", kStartMenuMargin);
        g_canvasTopOverride.Set(startSizingFrame, kStartMenuMargin);
    } else {
        g_canvasTopOverride.Release();
        Wh_Log(L"Setting Canvas.Left to %d", kStartMenuMargin);
        g_canvasLeftOverride.Set(startSizingFrame, kStartMenuMargin);
    }

    // Subscribe to Canvas.Top and Canvas.Left property changes to apply custom
    // styles right when that happens. Without it, the start menu may end up
    // truncated. A simple reproduction is to open the start menu on different
    // monitors, each with a different resolution/DPI/taskbar side.
    if (!g_startSizingFrameWeakRef.get()) {
        auto startSizingFrameDo = startSizingFrame.as<DependencyObject>();

        g_startSizingFrameWeakRef = startSizingFrameDo;

        g_canvasTopPropertyChangedToken =
            startSizingFrameDo.RegisterPropertyChangedCallback(
                Controls::Canvas::TopProperty(),
                [](DependencyObject sender, DependencyProperty property) {
                    double top =
                        Controls::Canvas::GetTop(sender.as<FrameworkElement>());
                    Wh_Log(L"Canvas.Top changed to %f", top);
                    if (!g_inApplyStyle) {
                        ApplyStyle();
                    }
                });

        g_canvasLeftPropertyChangedToken =
            startSizingFrameDo.RegisterPropertyChangedCallback(
                Controls::Canvas::LeftProperty(),
                [](DependencyObject sender, DependencyProperty property) {
                    double left = Controls::Canvas::GetLeft(
                        sender.as<FrameworkElement>());
                    Wh_Log(L"Canvas.Left changed to %f", left);
                    if (!g_inApplyStyle) {
                        ApplyStyle();
                    }
                });
    }
}

void ApplyStyleRedesignedStartMenu(FrameworkElement content,
                                   bool verticalTaskbar) {
    FrameworkElement frameRoot = FindChildByName(content, L"FrameRoot");
    if (!frameRoot) {
        Wh_Log(L"Failed to find Start menu frame root");
        return;
    }

    if (g_unloading) {
        g_horizontalAlignmentOverride.Restore();
        g_verticalAlignmentOverride.Restore();
        g_marginOverride.Restore();
        return;
    }

    g_marginOverride.Restore();

    if (verticalTaskbar) {
        g_horizontalAlignmentOverride.Release();

        // Keep the vertical margin set by Windows, moved to the bottom so that
        // it doesn't offset the menu from the top.
        auto margin = frameRoot.Margin();
        margin.Bottom += margin.Top;
        margin.Top = 0;
        g_marginOverride.Set(frameRoot, margin);

        g_verticalAlignmentOverride.Set(frameRoot, VerticalAlignment::Top);
    } else {
        g_verticalAlignmentOverride.Release();
        g_horizontalAlignmentOverride.Set(frameRoot, HorizontalAlignment::Left);
    }

    if (!g_frameRootWeakRef.get()) {
        auto frameRootDo = frameRoot.as<DependencyObject>();

        g_frameRootWeakRef = frameRootDo;

        g_verticalAlignmentPropertyChangedToken =
            frameRootDo.RegisterPropertyChangedCallback(
                FrameworkElement::VerticalAlignmentProperty(),
                [](DependencyObject sender, DependencyProperty property) {
                    auto alignment =
                        sender.as<FrameworkElement>().VerticalAlignment();
                    Wh_Log(L"FrameRoot VerticalAlignment changed to %d",
                           static_cast<int>(alignment));
                    if (!g_inApplyStyle) {
                        ApplyStyle();
                    }
                });

        g_horizontalAlignmentPropertyChangedToken =
            frameRootDo.RegisterPropertyChangedCallback(
                FrameworkElement::HorizontalAlignmentProperty(),
                [](DependencyObject sender, DependencyProperty property) {
                    auto alignment =
                        sender.as<FrameworkElement>().HorizontalAlignment();
                    Wh_Log(L"FrameRoot HorizontalAlignment changed to %d",
                           static_cast<int>(alignment));
                    if (!g_inApplyStyle) {
                        ApplyStyle();
                    }
                });
    }
}

void ApplyStyle() {
    g_inApplyStyle = true;

    HWND coreWnd = GetCoreWnd();
    HMONITOR monitor = MonitorFromWindow(coreWnd, MONITOR_DEFAULTTONEAREST);
    bool verticalTaskbar = IsVerticalTaskbarOnMonitor(monitor);

    Wh_Log(L"Applying Start menu style for monitor %p, vertical taskbar: %d",
           monitor, verticalTaskbar);

    auto window = Window::Current();
    FrameworkElement content = window.Content().as<FrameworkElement>();

    winrt::hstring contentClassName = winrt::get_class_name(content);
    Wh_Log(L"Start menu content class name: %s", contentClassName.c_str());

    if (contentClassName == L"Windows.UI.Xaml.Controls.Canvas") {
        ApplyStyleClassicStartMenu(content, verticalTaskbar);
    } else if (contentClassName == L"StartMenu.StartBlendedFlexFrame") {
        ApplyStyleRedesignedStartMenu(content, verticalTaskbar);
    } else {
        Wh_Log(L"Error: Unsupported Start menu content class name");
    }

    g_inApplyStyle = false;
}

void Init() {
    if (g_visibilityChangedToken) {
        return;
    }

    auto window = Window::Current();
    if (!window) {
        return;
    }

    g_visibilityChangedToken = window.VisibilityChanged(
        [](winrt::Windows::Foundation::IInspectable const& sender,
           winrt::Windows::UI::Core::VisibilityChangedEventArgs const& args) {
            Wh_Log(L"Window visibility changed: %d", args.Visible());
            if (args.Visible()) {
                ApplyStyle();
            }
        });

    ApplyStyle();
}

void Uninit() {
    if (!g_visibilityChangedToken) {
        return;
    }

    auto window = Window::Current();
    if (!window) {
        return;
    }

    window.VisibilityChanged(g_visibilityChangedToken);
    g_visibilityChangedToken = {};

    auto startSizingFrameDo = g_startSizingFrameWeakRef.get();
    if (startSizingFrameDo) {
        if (g_canvasTopPropertyChangedToken) {
            startSizingFrameDo.UnregisterPropertyChangedCallback(
                Controls::Canvas::TopProperty(),
                g_canvasTopPropertyChangedToken);
            g_canvasTopPropertyChangedToken = 0;
        }

        if (g_canvasLeftPropertyChangedToken) {
            startSizingFrameDo.UnregisterPropertyChangedCallback(
                Controls::Canvas::LeftProperty(),
                g_canvasLeftPropertyChangedToken);
            g_canvasLeftPropertyChangedToken = 0;
        }
    }

    g_startSizingFrameWeakRef = nullptr;

    auto frameRootDo = g_frameRootWeakRef.get();
    if (frameRootDo) {
        if (g_verticalAlignmentPropertyChangedToken) {
            frameRootDo.UnregisterPropertyChangedCallback(
                FrameworkElement::VerticalAlignmentProperty(),
                g_verticalAlignmentPropertyChangedToken);
            g_verticalAlignmentPropertyChangedToken = 0;
        }

        if (g_horizontalAlignmentPropertyChangedToken) {
            frameRootDo.UnregisterPropertyChangedCallback(
                FrameworkElement::HorizontalAlignmentProperty(),
                g_horizontalAlignmentPropertyChangedToken);
            g_horizontalAlignmentPropertyChangedToken = 0;
        }
    }

    g_frameRootWeakRef = nullptr;

    ApplyStyle();
}

void SettingsChanged() {
    ApplyStyle();
}

using RoGetActivationFactory_t = decltype(&RoGetActivationFactory);
RoGetActivationFactory_t RoGetActivationFactory_Original;
HRESULT WINAPI RoGetActivationFactory_Hook(HSTRING activatableClassId,
                                           REFIID iid,
                                           void** factory) {
    thread_local static bool isInHook;

    if (isInHook) {
        return RoGetActivationFactory_Original(activatableClassId, iid,
                                               factory);
    }

    isInHook = true;

    if (wcscmp(WindowsGetStringRawBuffer(activatableClassId, nullptr),
               L"Windows.UI.Xaml.Hosting.XamlIsland") == 0) {
        try {
            Init();
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"Error %08X", hr);
        }
    }

    HRESULT ret =
        RoGetActivationFactory_Original(activatableClassId, iid, factory);

    isInHook = false;

    return ret;
}

}  // namespace StartMenuUI

void RestoreMenuPositions() {
    if (g_searchMenuWnd && g_searchMenuOriginalPos) {
        HMONITOR monitor =
            MonitorFromWindow(g_searchMenuWnd, MONITOR_DEFAULTTONEAREST);

        RECT rect;
        // The saved position is an absolute coordinate, valid only on the
        // monitor where it was recorded.
        if (monitor == g_searchMenuMonitor &&
            GetWindowRect(g_searchMenuWnd, &rect)) {
            int x = rect.left;
            int y = rect.top;
            int cx = rect.right - rect.left;
            int cy = rect.bottom - rect.top;

            int& pos = g_searchMenuVertical ? y : x;
            if (g_searchMenuOriginalPos != pos) {
                pos = g_searchMenuOriginalPos;
                SetWindowPos(g_searchMenuWnd, nullptr, x, y, cx, cy,
                             SWP_NOZORDER | SWP_NOACTIVATE);
            }
        }

        g_searchMenuWnd = nullptr;
        g_searchMenuOriginalPos = 0;
        g_searchMenuMonitor = nullptr;
    }
}

void LoadSettings() {
    g_settings.otherSystemButtonsOnTheLeft =
        Wh_GetIntSetting(L"otherSystemButtonsOnTheLeft");
    g_settings.startMenuOnTheLeft = Wh_GetIntSetting(L"startMenuOnTheLeft");
    g_settings.searchMenuPositionInAllCases =
        Wh_GetIntSetting(L"searchMenuPositionInAllCases");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_target = Target::Explorer;

    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(nullptr, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            Wh_Log(L"GetModuleFileName failed");
            return FALSE;

        default:
            if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                moduleFileName++;
                if (_wcsicmp(moduleFileName, L"StartMenuExperienceHost.exe") ==
                    0) {
                    g_target = Target::StartMenuExperienceHost;
                }
            } else {
                Wh_Log(L"GetModuleFileName returned an unsupported path");
                return FALSE;
            }
            break;
    }

    if (g_target == Target::StartMenuExperienceHost) {
        if (!g_settings.startMenuOnTheLeft) {
            return FALSE;
        }

        HMODULE winrtModule =
            GetModuleHandle(L"api-ms-win-core-winrt-l1-1-0.dll");
        auto pRoGetActivationFactory =
            (decltype(&RoGetActivationFactory))GetProcAddress(
                winrtModule, "RoGetActivationFactory");
        WindhawkUtils::SetFunctionHook(
            pRoGetActivationFactory, StartMenuUI::RoGetActivationFactory_Hook,
            &StartMenuUI::RoGetActivationFactory_Original);

        return TRUE;
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    if (!HookTwinuiPcshellSymbols()) {
        // The mod can continue without these hooks.
        Wh_Log(L"HookTwinuiPcshellSymbols failed");
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

    HMODULE dwmapiModule =
        LoadLibraryEx(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (dwmapiModule) {
        auto pDwmSetWindowAttribute =
            (decltype(&DwmSetWindowAttribute))GetProcAddress(
                dwmapiModule, "DwmSetWindowAttribute");
        if (pDwmSetWindowAttribute) {
            WindhawkUtils::SetFunctionHook(pDwmSetWindowAttribute,
                                           DwmSetWindowAttribute_Hook,
                                           &DwmSetWindowAttribute_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_target == Target::Explorer) {
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
    } else if (g_target == Target::StartMenuExperienceHost) {
        HWND hCoreWnd = StartMenuUI::GetCoreWnd();
        if (hCoreWnd) {
            Wh_Log(L"Initializing - Found core window");
            RunFromWindowThread(
                hCoreWnd, [](PVOID) { StartMenuUI::Init(); }, nullptr);
        }
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (g_target == Target::Explorer) {
        HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
        if (hTaskbarWnd) {
            ApplySettings(hTaskbarWnd);
        }
    } else if (g_target == Target::StartMenuExperienceHost) {
        HWND hCoreWnd = StartMenuUI::GetCoreWnd();
        if (hCoreWnd) {
            Wh_Log(L"Uninitializing - Found core window");
            RunFromWindowThread(
                hCoreWnd, [](PVOID) { StartMenuUI::Uninit(); }, nullptr);
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_target == Target::Explorer) {
        RestoreMenuPositions();
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    if (g_target == Target::Explorer) {
        RestoreMenuPositions();
    }

    LoadSettings();

    if (g_target == Target::Explorer) {
        HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
        if (hTaskbarWnd) {
            ApplySettings(hTaskbarWnd);
        }
    } else if (g_target == Target::StartMenuExperienceHost) {
        if (!g_settings.startMenuOnTheLeft) {
            return FALSE;
        }

        HWND hCoreWnd = StartMenuUI::GetCoreWnd();
        if (hCoreWnd) {
            Wh_Log(L"Applying settings - Found core window");
            RunFromWindowThread(
                hCoreWnd, [](PVOID) { StartMenuUI::SettingsChanged(); },
                nullptr);
        }
    }

    return TRUE;
}
