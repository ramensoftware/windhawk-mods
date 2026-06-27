// ==WindhawkMod==
// @id              taskbar-start-button-position
// @name            Start button always on the left
// @description     Forces the Start button to be on the left of the taskbar, even when taskbar icons are centered, with an option to also move the search and task view buttons (Windows 11 only)
// @version         1.3.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         StartMenuExperienceHost.exe
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject -lshcore
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

Only Windows 11 is supported.

![Screenshot](https://i.imgur.com/MSKYKbE.png) \
_Start button on the left_

![Screenshot](https://i.imgur.com/SOdWH1P.png) \
_Start button, search and task view buttons on the left_
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- startMenuOnTheLeft: true
  $name: Start menu on the left
  $description: >-
    Make the start menu open on the left even if taskbar icons are centered.
- otherSystemButtonsOnTheLeft: false
  $name: Move other system buttons to the left
  $description: >-
    In addition to the start button, also move the search and task view buttons
    to the left, keeping only the app icons centered.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <functional>
#include <string>

#include <dwmapi.h>
#include <roapi.h>
#include <winstring.h>

#undef GetCurrentTime

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
    bool startMenuOnTheLeft;
    bool otherSystemButtonsOnTheLeft;
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
int g_searchMenuOriginalX;
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

// The width of a cluster button. The search button's own ActualWidth is
// unreliable while collapsed (the negative right margin reinforces its old
// width), so use its content child's DesiredSize. Start and task view are
// fixed-width, so ActualWidth is fine.
double GetClusterButtonWidth(FrameworkElement element) {
    if (IdentifySystemButton(element) == SystemButton::Search &&
        Media::VisualTreeHelper::GetChildrenCount(element) > 0) {
        auto child = Media::VisualTreeHelper::GetChild(element, 0)
                         .try_as<FrameworkElement>();
        if (child) {
            return child.DesiredSize().Width;
        }
    }

    return element.ActualWidth();
}

// The X at which a pinned cluster button goes: the summed widths of the buttons
// before it in cluster order (start at 0, then search, task view, widgets), so
// it's independent of the order the layout arranges its children in.
double ComputePinnedSystemButtonX(FrameworkElement taskbarFrameRepeater,
                                  SystemButton target) {
    int targetRank = SystemButtonClusterRank(target);
    if (targetRank <= 0) {
        return 0;
    }

    double x = 0;
    EnumRepeaterChildElements(
        taskbarFrameRepeater, [&x, targetRank](FrameworkElement child) {
            int childRank =
                SystemButtonClusterRank(IdentifySystemButton(child));
            if (childRank >= 0 && childRank < targetRank) {
                x += GetClusterButtonWidth(child);
            }
            return false;
        });

    return x;
}

// Last GetTickCount64() at which each pinned button was collapsed, to throttle
// collapses (see UpdatePinnedSystemButtonMargin). Indexed by SystemButton.
ULONGLONG g_lastButtonCollapseTick[kSystemButtonCount];

// Keeps the pinned cluster (start, plus search and task view when the option is
// on) from overlapping the centered group: a button collapses out of the layout
// when there's room and expands to reserve its width when crowded. Expansions
// are throttled (see below) to avoid oscillation. Runs on the taskbar thread.
void UpdatePinnedSystemButtonMargin(FrameworkElement element) {
    SystemButton self = IdentifySystemButton(element);
    if (g_unloading || !IsPinnedClusterButton(self)) {
        // Unloading, or the option was turned off after this was scheduled;
        // ApplyStyle restores the margins.
        return;
    }

    auto taskbarFrameRepeater =
        Media::VisualTreeHelper::GetParent(element).as<FrameworkElement>();
    if (!taskbarFrameRepeater) {
        return;
    }

    // Measure the pinned set's total width and the nearest centered item.
    double pinnedWidth = 0;
    double centeredLeftX = std::numeric_limits<double>::infinity();
    EnumRepeaterChildElements(
        taskbarFrameRepeater, [&](FrameworkElement child) {
            SystemButton button = IdentifySystemButton(child);
            if (IsPinnedClusterButton(button)) {
                pinnedWidth += GetClusterButtonWidth(child);
            } else if (button != SystemButton::Widgets) {
                auto offset = child.ActualOffset();
                if (offset.x >= 0 && offset.x < centeredLeftX) {
                    centeredLeftX = offset.x;
                }
            }
            return false;
        });

    Thickness margin = element.Margin();

    double newRight;
    if (centeredLeftX < pinnedWidth) {
        newRight = 0;  // expand: reserve this button's width
    } else if (margin.Right != 0 || centeredLeftX > pinnedWidth + 44) {
        newRight =
            -GetClusterButtonWidth(element);  // collapse out of the group
    } else {
        return;  // already collapsed and not crowded
    }

    if (margin.Right == newRight) {
        return;
    }

    if (newRight < margin.Right) {
        // Collapsing gives up this button's reserved width and shifts the
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

    margin.Right = newRight;
    element.Margin(margin);
}

// Pins the widgets button to the right of the start button (or the whole
// cluster, when the option is on) via its left margin. Windows left-pins it at
// the far left where the start button goes, so it always needs nudging right.
// Runs on the taskbar thread.
void UpdateWidgetLeftMargin(FrameworkElement element) {
    if (g_unloading) {
        // ApplyStyle restores the margin on unload; don't fight it.
        return;
    }

    auto taskbarFrameRepeater =
        Media::VisualTreeHelper::GetParent(element).as<FrameworkElement>();
    if (!taskbarFrameRepeater) {
        return;
    }

    double left = g_settings.otherSystemButtonsOnTheLeft
                      ? ComputePinnedSystemButtonX(taskbarFrameRepeater,
                                                   SystemButton::Widgets)
                      : 44;

    Thickness margin = element.Margin();
    if (margin.Left != left) {
        margin.Left = left;
        element.Margin(margin);
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
            auto childClassName = winrt::get_class_name(child);
            if (childClassName != L"Taskbar.AugmentedEntryPointButton") {
                return false;
            }

            if (child.Name() != L"AugmentedEntryPointButton") {
                return false;
            }

            auto margin = child.Margin();

            auto offset = child.ActualOffset();
            if (offset.x != margin.Left || offset.y != 0) {
                return false;
            }

            return true;
        });
    if (widgetElement) {
        auto margin = widgetElement.Margin();
        if (g_unloading) {
            margin.Left = 0;
        } else if (g_settings.otherSystemButtonsOnTheLeft) {
            // Pin the widgets button at the end of the cluster, after the task
            // view button, instead of right after the start button.
            margin.Left = ComputePinnedSystemButtonX(taskbarFrameRepeater,
                                                     SystemButton::Widgets);
        } else {
            margin.Left = 44;
        }
        widgetElement.Margin(margin);
    }

    // Collapse the pinned cluster buttons - the start button always, plus the
    // search and task view buttons when the option is on - so they're excluded
    // from the centered group; their left positioning happens in
    // IUIElement_Arrange_Hook. Buttons that aren't pinned, and everything while
    // unloading, are restored to the centered group.
    EnumRepeaterChildElements(taskbarFrameRepeater, [](FrameworkElement child) {
        SystemButton systemButton = IdentifySystemButton(child);
        switch (systemButton) {
            case SystemButton::Start:
            case SystemButton::Search:
            case SystemButton::TaskView: {
                Thickness margin = child.Margin();
                double width = GetClusterButtonWidth(child);
                if (IsPinnedClusterButton(systemButton) && !g_unloading) {
                    margin.Right = -width;
                } else if (margin.Right < 0) {
                    // Restore only the collapse applied by the mod.
                    margin.Right = 0;
                } else {
                    break;
                }
                Wh_Log(
                    L"Collapsing system button %d: width=%.1f, "
                    L"margin.Right=%.1f",
                    (int)systemButton, width, margin.Right);
                child.Margin(margin);
                break;
            }
            default:
                break;
        }

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
        ScheduleOnTaskbarThread(element, UpdateWidgetLeftMargin);
        return original();
    }

    // The pinned cluster (the start button, plus search and task view when the
    // option is on) is moved to the left below. Everything else (the app
    // buttons) is left alone.
    if (!IsPinnedClusterButton(systemButton)) {
        return original();
    }

    auto taskbarFrameRepeater =
        Media::VisualTreeHelper::GetParent(element).as<FrameworkElement>();

    // Find the widgets button at its left-pinned position (offset matches its
    // margin). When present, it sits right of the start button (or cluster) and
    // anchors it against the centered group.
    auto widgetElement = EnumRepeaterChildElements(
        taskbarFrameRepeater, [](FrameworkElement child) {
            if (IdentifySystemButton(child) != SystemButton::Widgets) {
                return false;
            }

            auto margin = child.Margin();
            auto offset = child.ActualOffset();
            return offset.x == margin.Left && offset.y == 0;
        });

    // Without that anchor, adjust the margin so the start button (or cluster)
    // doesn't overlap the centered group.
    if (!widgetElement) {
        ScheduleOnTaskbarThread(element, UpdatePinnedSystemButtonMargin);
    }

    // Pin it to the left in cluster order: the start button gets
    // X = 0, then search, then task view.
    double x = ComputePinnedSystemButtonX(taskbarFrameRepeater, systemButton);

    Wh_Log(L"Pinning system button %d to x=%.1f", (int)systemButton, x);

    winrt::Windows::Foundation::Rect newRect = rect;
    newRect.X = x;
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

// The start button context menu is centered over the start button or aligned
// to its leading edge depending on the taskbar alignment (TaskbarFrame's
// Alignment, where Left=0 and Center=1). Both the placement mode and the anchor
// position are derived from it. With the start button forced to the left, the
// menu should align to the button's leading edge, so report left alignment
// while the menu is being shown, letting the taskbar's own left-alignment code
// position the menu.
using TaskbarFrame_Alignment_t = int(WINAPI*)(void* pThis);
TaskbarFrame_Alignment_t TaskbarFrame_Alignment_Original;
int WINAPI TaskbarFrame_Alignment_Hook(void* pThis) {
    if (!g_unloading && g_settings.startMenuOnTheLeft &&
        g_inShowStartButtonContextMenu) {
        return 0;  // TaskbarAlignment::Left
    }

    return TaskbarFrame_Alignment_Original(pThis);
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
            {LR"(public: enum winrt::WindowsUdk::UI::Shell::TaskbarAlignment __cdecl winrt::Taskbar::implementation::TaskbarFrame::Alignment(void)const )"},
            &TaskbarFrame_Alignment_Original,
            TaskbarFrame_Alignment_Hook,
        },
        {
            {LR"(static  winrt::Taskbar::implementation::ContextMenus::ShowStartButtonContextMenuAsync$_ResumeCoro$1())"},
            &ShowStartButtonContextMenuResumeCoro_Original,
            ShowStartButtonContextMenuResumeCoro_Hook,
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
        // Only change x.
        int xNew;

        if (g_settings.startMenuOnTheLeft && !cloak && IsStartMenuOpen()) {
            // Not centered or already changed.
            if (x == monitorInfo.rcWork.left) {
                return original();
            }

            xNew = monitorInfo.rcWork.left;
            g_searchMenuWnd = hwnd;
            g_searchMenuOriginalX = x;
            g_searchMenuMonitor = monitor;
        } else {
            if (!g_searchMenuOriginalX) {
                return original();
            }

            xNew = g_searchMenuOriginalX;
            bool monitorMatches = monitor == g_searchMenuMonitor;

            g_searchMenuWnd = nullptr;
            g_searchMenuOriginalX = 0;
            g_searchMenuMonitor = nullptr;

            if (!monitorMatches) {
                return original();
            }
        }

        if (xNew == x) {
            return original();
        }

        Wh_Log(L"Adjusting search menu: %d -> %d", x, xNew);

        x = xNew;
    }

    SetWindowPos(hwnd, nullptr, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);

    return original();
}

namespace StartMenuUI {

bool g_inApplyStyle;
std::optional<double> g_previousCanvasLeft;
winrt::weak_ref<DependencyObject> g_startSizingFrameWeakRef;
int64_t g_canvasTopPropertyChangedToken;
int64_t g_canvasLeftPropertyChangedToken;
std::optional<HorizontalAlignment> g_previousHorizontalAlignment;
winrt::weak_ref<DependencyObject> g_frameRootWeakRef;
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

void ApplyStyleClassicStartMenu(FrameworkElement content, HMONITOR monitor) {
    FrameworkElement startSizingFrame =
        FindChildByClassName(content, L"StartDocked.StartSizingFrame");
    if (!startSizingFrame) {
        Wh_Log(L"Failed to find StartDocked.StartSizingFrame");
        return;
    }

    if (g_unloading) {
        if (g_previousCanvasLeft.has_value()) {
            Wh_Log(L"Restoring Canvas.Left to %f",
                   g_previousCanvasLeft.value());
            Controls::Canvas::SetLeft(startSizingFrame,
                                      g_previousCanvasLeft.value());
        }
    } else {
        if (!g_previousCanvasLeft.has_value()) {
            double canvasLeft = Controls::Canvas::GetLeft(startSizingFrame);
            // The value might be zero when not yet initialized.
            if (canvasLeft) {
                g_previousCanvasLeft = canvasLeft;
            }
        }

        constexpr int kStartMenuMargin = 12;

        double newLeft = kStartMenuMargin;

        Wh_Log(L"Setting Canvas.Left to %f", newLeft);
        Controls::Canvas::SetLeft(startSizingFrame, newLeft);

        // Subscribe to Canvas.Top and Canvas.Left property changes to apply
        // custom styles right when that happens. Without it, the start menu may
        // end up truncated. A simple reproduction is to open the start menu on
        // different monitors, each with a different resolution/DPI/taskbar
        // side.
        if (!g_startSizingFrameWeakRef.get()) {
            auto startSizingFrameDo = startSizingFrame.as<DependencyObject>();

            g_startSizingFrameWeakRef = startSizingFrameDo;

            g_canvasTopPropertyChangedToken =
                startSizingFrameDo.RegisterPropertyChangedCallback(
                    Controls::Canvas::TopProperty(),
                    [](DependencyObject sender, DependencyProperty property) {
                        double top = Controls::Canvas::GetTop(
                            sender.as<FrameworkElement>());
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
}

void ApplyStyleRedesignedStartMenu(FrameworkElement content) {
    FrameworkElement frameRoot = FindChildByName(content, L"FrameRoot");
    if (!frameRoot) {
        Wh_Log(L"Failed to find Start menu frame root");
        return;
    }

    if (g_unloading) {
        frameRoot.HorizontalAlignment(g_previousHorizontalAlignment.value_or(
            HorizontalAlignment::Center));
    } else {
        if (!g_previousHorizontalAlignment) {
            g_previousHorizontalAlignment = frameRoot.HorizontalAlignment();
        }

        frameRoot.HorizontalAlignment(HorizontalAlignment::Left);

        if (!g_frameRootWeakRef.get()) {
            auto frameRootDo = frameRoot.as<DependencyObject>();

            g_frameRootWeakRef = frameRootDo;

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
}

void ApplyStyle() {
    g_inApplyStyle = true;

    HWND coreWnd = GetCoreWnd();
    HMONITOR monitor = MonitorFromWindow(coreWnd, MONITOR_DEFAULTTONEAREST);

    Wh_Log(L"Applying Start menu style for monitor %p", monitor);

    auto window = Window::Current();
    FrameworkElement content = window.Content().as<FrameworkElement>();

    winrt::hstring contentClassName = winrt::get_class_name(content);
    Wh_Log(L"Start menu content class name: %s", contentClassName.c_str());

    if (contentClassName == L"Windows.UI.Xaml.Controls.Canvas") {
        ApplyStyleClassicStartMenu(content, monitor);
    } else if (contentClassName == L"StartMenu.StartBlendedFlexFrame") {
        ApplyStyleRedesignedStartMenu(content);
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
    if (g_searchMenuWnd && g_searchMenuOriginalX) {
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

            if (g_searchMenuOriginalX != x) {
                x = g_searchMenuOriginalX;
                SetWindowPos(g_searchMenuWnd, nullptr, x, y, cx, cy,
                             SWP_NOZORDER | SWP_NOACTIVATE);
            }
        }

        g_searchMenuWnd = nullptr;
        g_searchMenuOriginalX = 0;
        g_searchMenuMonitor = nullptr;
    }
}

void LoadSettings() {
    g_settings.startMenuOnTheLeft = Wh_GetIntSetting(L"startMenuOnTheLeft");
    g_settings.otherSystemButtonsOnTheLeft =
        Wh_GetIntSetting(L"otherSystemButtonsOnTheLeft");
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
