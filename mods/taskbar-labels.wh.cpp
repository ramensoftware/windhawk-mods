// ==WindhawkMod==
// @id              taskbar-labels
// @name            Taskbar Labels for Windows 11
// @description     Customize text labels and combining for running programs on the taskbar (Windows 11 only)
// @version         1.6
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
# Taskbar Labels for Windows 11

Customize text labels and combining for running programs on the taskbar.

The mod improves the native Windows taskbar labels implementation by making all
taskbar items have the same width (optional), adding ellipsis for long labels,
and providing other customization options.

The mod allows to choose one of the four available modes:

![Show labels, don't combine taskbar buttons](https://i.imgur.com/v8Idmjy.png) \
*Show labels, don't combine taskbar buttons (default)*

![Hide labels, combine taskbar buttons](https://i.imgur.com/6Fg5h0d.png) \
*Hide labels, combine taskbar buttons*

![Show labels, combine taskbar buttons](https://i.imgur.com/Y5HA6Xv.png) \
*Show labels, combine taskbar buttons*

![Hide labels, don't combine taskbar buttons](https://i.imgur.com/Buh8KnZ.png) \
*Hide labels, don't combine taskbar buttons*

Only the first two modes are available natively in Windows.

**Older Windows 11 versions:** In older versions, the taskbar can only show
icons for taskbar items, without any text labels. This mod adds text labels
using a custom implementation. The additional modes, and some other options,
aren't available.

Additional customization is available in the settings. For example, you can
choose one of the following running indicator styles:

![Centered, fixed size](https://i.imgur.com/zWxTGRb.png) \
*Centered, fixed size (default)*

![Centered, dynamic size](https://i.imgur.com/YiPSZdI.png) \
*Centered, dynamic size*

![On the left (below the icon)](https://i.imgur.com/7M5x5EJ.png) \
*On the left (below the icon)*

![Full width](https://i.imgur.com/T7YjTfk.png) \
*Full width*

Labels can also be shown or hidden per-program in the settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: labelsWithoutCombining
  $name: Mode
  $description: >-
    Note: When switching to or from the last two modes, restarting explorer
    might be required to fully apply the new configuration
  $options:
  - labelsWithoutCombining: Show labels, don't combine taskbar buttons
  - noLabelsWithCombining: Hide labels, combine taskbar buttons
  - labelsWithCombining: Show labels, combine taskbar buttons
  - noLabelsWithoutCombining: Hide labels, don't combine taskbar buttons
- taskbarItemWidth: 160
  $name: Taskbar item width
  $description: >-
    The width to use when labels are shown, set to 0 to use the Windows adaptive
    width

    Not used on a vertical taskbar, where the items are as wide as the taskbar
- runningIndicatorStyle: centerFixed
  $name: Running indicator style
  $options:
  - centerFixed: Centered, fixed size
  - centerDynamic: Centered, dynamic size
  - left: On the left (below the icon)
  - fullWidth: Full width
- progressIndicatorStyle: sameAsRunningIndicatorStyle
  $name: Progress indicator style
  $options:
  - sameAsRunningIndicatorStyle: Same as running indicator style
  - centerDynamic: Centered, dynamic size
  - fullWidth: Full width
- excludedPrograms: [""]
  $name: Excluded programs
  $description: >-
    If one of the "Show labels" modes is used, labels won't be shown for these
    programs

    If one of the "Hide labels" modes is used, labels will be shown for these
    programs

    Entries can be process names, paths or application IDs, for example:

    mspaint.exe

    C:\Windows\System32\notepad.exe

    Microsoft.WindowsCalculator_8wekyb3d8bbwe!App
- minimumTaskbarItemWidth: 50
  $name: Minimum taskbar item width
  $description: >-
    The minimum width before the taskbar overflows

    Values larger than the Windows minimum width are unsupported and have no
    effect
- maximumTaskbarItemWidth: 176
  $name: Maximum taskbar item width
  $description: >-
    The maximum width, only used for the Windows adaptive width
- fontSize: 12
  $name: Font size
- fontFamily: ""
  $name: Font family
  $description: >-
    For a list of fonts that are shipped with Windows 11, refer to the following
    page:

    https://learn.microsoft.com/en-us/typography/fonts/windows_11_font_list
- textTrimming: characterEllipsis
  $name: Text trimming
  $options:
  - characterEllipsis: Trim at character with ellipsis (...)
  - wordEllipsis: Trim at word with ellipsis (...)
  - clip: Clip at a pixel level (Windows 11 default)
- leftAndRightPaddingSize: 8
  $name: Left and right padding size
- spaceBetweenIconAndLabel: 8
  $name: Space between icon and label
- runningIndicatorHeight: 0
  $name: Running indicator height
  $description: >-
    Set to zero for the default height, set to -1 to hide it
- runningIndicatorVerticalOffset: 0
  $name: Running indicator vertical offset
- alwaysShowThumbnailLabels: false
  $name: Always show thumbnail labels
  $description: >-
    By default, thumbnail labels are shown on hover only if taskbar labels are
    hidden, but that might not be applied for all customizations that this mod
    offers
- labelForSingleItem: "%name%"
  $name: Label for a single item
  $description: >-
    The following variables can be used: %name%, %amount%

    Ignored in newer Windows versions with the built-in taskbar labels
    implementation
- labelForMultipleItems: "[%amount%] %name%"
  $name: Label for multiple items
  $description: >-
    Ignored in newer Windows versions with the built-in taskbar labels
    implementation
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <inspectable.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/base.h>

// The taskbar items live in a WinUI 2 (MUX) ItemsRepeater built on top of
// system XAML. Pull in its projection to enumerate the realized items
// (ItemsSourceView / TryGetElement).
#define WH_WINRT_WINUI2
#include <winrt/Microsoft.UI.Xaml.Controls.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <limits>
#include <string>
#include <unordered_set>

using namespace winrt::Windows::UI::Xaml;

enum class Mode {
    labelsWithoutCombining,
    noLabelsWithCombining,
    labelsWithCombining,
    noLabelsWithoutCombining,
};

enum class IndicatorStyle {
    centerFixed,
    centerDynamic,
    left,
    fullWidth,
};

struct {
    Mode mode;
    int taskbarItemWidth;
    IndicatorStyle runningIndicatorStyle;
    IndicatorStyle progressIndicatorStyle;
    std::unordered_set<std::wstring> excludedPrograms;
    int minimumTaskbarItemWidth;
    int maximumTaskbarItemWidth;
    int fontSize;
    WindhawkUtils::StringSetting fontFamily;
    TextTrimming textTrimming;
    int leftAndRightPaddingSize;
    int spaceBetweenIconAndLabel;
    int runningIndicatorHeight;
    int runningIndicatorVerticalOffset;
    bool alwaysShowThumbnailLabels;
    WindhawkUtils::StringSetting labelForSingleItem;
    WindhawkUtils::StringSetting labelForMultipleItems;
} g_settings;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_searchUxUiDllLoaded;
std::atomic<bool> g_systemTrayDllLoaded;
std::atomic<bool> g_applyingSettings;
std::atomic<bool> g_overrideGroupingMode;
std::atomic<bool> g_unloading;

bool g_hasNativeLabelsImplementation;

double g_initialTaskbarItemWidth;

UINT_PTR g_invalidateTaskListButtonTimer;
std::unordered_set<void*> g_taskListButtonsWithLabelMissing;

WINUSERAPI UINT WINAPI GetDpiForWindow(HWND hwnd);

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (child.Name() == name) {
            return child;
        }
    }

    return nullptr;
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (winrt::get_class_name(child) == className) {
            return child;
        }
    }

    return nullptr;
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

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
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

void RecalculateLabels() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return;
    }

    HWND hReBarWindow32 =
        FindWindowEx(hTaskbarWnd, nullptr, L"ReBarWindow32", nullptr);
    if (!hReBarWindow32) {
        return;
    }

    HWND hMSTaskSwWClass =
        FindWindowEx(hReBarWindow32, nullptr, L"MSTaskSwWClass", nullptr);
    if (!hMSTaskSwWClass) {
        return;
    }

    g_applyingSettings = true;

    // Trigger CTaskBand::_HandleSyncDisplayChange.
    SendMessage(hMSTaskSwWClass, 0x452, 3, 0);

    g_applyingSettings = false;
}

using TaskListButton_get_IsRunning_t = HRESULT(WINAPI*)(void* pThis,
                                                        bool* running);
TaskListButton_get_IsRunning_t TaskListButton_get_IsRunning_Original;

bool TaskListButton_IsRunning(FrameworkElement taskListButtonElement) {
    bool isRunning = false;
    TaskListButton_get_IsRunning_Original(
        winrt::get_abi(
            taskListButtonElement.as<winrt::Windows::Foundation::IUnknown>()),
        &isRunning);
    return isRunning;
}

double CalculateTaskbarItemWidth(FrameworkElement taskbarFrameRepeaterElement,
                                 double minWidth,
                                 double maxWidth) {
    double taskbarFrameRepeaterEndOffset = 0;

    auto rootGridElement =
        Media::VisualTreeHelper::GetParent(taskbarFrameRepeaterElement)
            .as<FrameworkElement>();
    if (rootGridElement && rootGridElement.Name() == L"RootGrid") {
        auto taskbarFrameElement =
            Media::VisualTreeHelper::GetParent(rootGridElement)
                .as<FrameworkElement>();
        if (taskbarFrameElement &&
            taskbarFrameElement.Name() == L"TaskbarFrame") {
            auto containerGridElement =
                Media::VisualTreeHelper::GetParent(taskbarFrameElement)
                    .as<FrameworkElement>();
            if (containerGridElement &&
                winrt::get_class_name(containerGridElement) ==
                    L"Windows.UI.Xaml.Controls.Grid") {
                auto systemTrayFrameElement = FindChildByClassName(
                    containerGridElement, L"SystemTray.SystemTrayFrame");
                if (systemTrayFrameElement) {
                    taskbarFrameRepeaterEndOffset =
                        systemTrayFrameElement.ActualOffset().x;
                }
            }
        }
    }

    // For older versions (pre-KB5022913). Only works correctly on primary
    // monitor.
    if (!taskbarFrameRepeaterEndOffset) {
        HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
        if (!hTaskbarWnd) {
            return minWidth;
        }

        HWND hTrayNotifyWnd =
            FindWindowEx(hTaskbarWnd, nullptr, L"TrayNotifyWnd", nullptr);
        if (!hTrayNotifyWnd) {
            return minWidth;
        }

        RECT rcTrayNotify{};
        if (!GetWindowRect(hTrayNotifyWnd, &rcTrayNotify)) {
            return minWidth;
        }

        MapWindowPoints(HWND_DESKTOP, hTaskbarWnd, (LPPOINT)&rcTrayNotify, 2);

        taskbarFrameRepeaterEndOffset =
            MulDiv(rcTrayNotify.left, 96, GetDpiForWindow(hTrayNotifyWnd));
    }

    auto repeater =
        taskbarFrameRepeaterElement
            .try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>();
    if (!repeater) {
        Wh_Log(L"Not an ItemsRepeater");
        return minWidth;
    }

    bool hasOverflowButton = false;
    int taskListRunningButtonsCount = 0;
    double otherElementsWidth = 0;

    auto itemsSourceView = repeater.ItemsSourceView();
    if (!itemsSourceView) {
        Wh_Log(L"No ItemsSourceView");
        return minWidth;
    }

    int count = itemsSourceView.Count();

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

        auto childOffset = child.ActualOffset();
        auto childWidth = child.ActualWidth();

        if (childOffset.x + childWidth < 0) {
            continue;
        }

        bool isRunningTaskListButton = false;
        if (child.Name() == L"TaskListButton") {
            if (TaskListButton_IsRunning(child)) {
                isRunningTaskListButton = true;
            }
        } else if (child.Name() == L"OverflowButton") {
            hasOverflowButton = true;
        }

        if (isRunningTaskListButton) {
            taskListRunningButtonsCount++;
        } else {
            otherElementsWidth += childWidth;
        }
    }

    if (hasOverflowButton) {
        return minWidth;
    }

    if (taskListRunningButtonsCount == 0) {
        return minWidth;
    }

    double width = (taskbarFrameRepeaterEndOffset - otherElementsWidth) /
                   taskListRunningButtonsCount;

    // Wh_Log(L"(%f-%f) / %d = %f", taskbarFrameRepeaterEndOffset,
    //        otherElementsWidth, taskListRunningButtonsCount, width);

    if (width < minWidth) {
        return minWidth;
    }

    if (width > maxWidth) {
        return maxWidth;
    }

    return width;
}

using CTaskListThumbnailWnd_DisplayUI_t = void*(WINAPI*)(void* pThis,
                                                         void* param1,
                                                         void* param2,
                                                         void* param3,
                                                         DWORD flags);
CTaskListThumbnailWnd_DisplayUI_t CTaskListThumbnailWnd_DisplayUI_Original;
void* WINAPI CTaskListThumbnailWnd_DisplayUI_Hook(void* pThis,
                                                  void* param1,
                                                  void* param2,
                                                  void* param3,
                                                  DWORD flags) {
    Wh_Log(L">");

    if (g_settings.alwaysShowThumbnailLabels) {
        flags |= 0x01;
    }

    void* ret = CTaskListThumbnailWnd_DisplayUI_Original(pThis, param1, param2,
                                                         param3, flags);

    return ret;
}

using CTaskListWnd__GetTBGroupFromGroup_t = void*(WINAPI*)(void* pThis,
                                                           void* taskGroup,
                                                           int* index);
CTaskListWnd__GetTBGroupFromGroup_t CTaskListWnd__GetTBGroupFromGroup_Original;

using CTaskBtnGroup_GetNumItems_t = int(WINAPI*)(void* pThis);
CTaskBtnGroup_GetNumItems_t CTaskBtnGroup_GetNumItems_Original;

using CTaskBtnGroup_GetTaskItem_t = void*(WINAPI*)(void* pThis, int index);
CTaskBtnGroup_GetTaskItem_t CTaskBtnGroup_GetTaskItem_Original;

using CTaskGroup_GetTitleText_t = LONG_PTR(WINAPI*)(void* pThis,
                                                    void* taskItem,
                                                    WCHAR* text,
                                                    int bufferSize);
CTaskGroup_GetTitleText_t CTaskGroup_GetTitleText_Original;

using IconContainer_IsStorageRecreationRequired_t = bool(WINAPI*)(void* pThis,
                                                                  void* param1,
                                                                  int flags);
IconContainer_IsStorageRecreationRequired_t
    IconContainer_IsStorageRecreationRequired_Original;
bool WINAPI IconContainer_IsStorageRecreationRequired_Hook(void* pThis,
                                                           void* param1,
                                                           int flags) {
    if (g_applyingSettings) {
        return true;
    }

    return IconContainer_IsStorageRecreationRequired_Original(pThis, param1,
                                                              flags);
}

int StringCopyTruncated(PWSTR dest,
                        size_t destSize,
                        PCWSTR src,
                        bool* truncated) {
    if (destSize == 0) {
        *truncated = *src;
        return 0;
    }

    size_t i;
    for (i = 0; i < destSize - 1 && *src; i++) {
        *dest++ = *src++;
    }

    *dest = L'\0';
    *truncated = *src;
    return i;
}

int FormatLabel(PCWSTR name,
                int numItems,
                PWSTR buffer,
                size_t bufferSize,
                PCWSTR format) {
    if (bufferSize == 0) {
        return 0;
    }

    WCHAR tempNumberBuffer[16];

    PWSTR bufferStart = buffer;
    PWSTR bufferEnd = bufferStart + bufferSize;
    while (*format && bufferEnd - buffer > 1) {
        if (*format == L'%') {
            PCWSTR srcStr = nullptr;
            size_t formatTokenLen;

            if (wcsncmp(L"%name%", format, sizeof("%name%") - 1) == 0) {
                srcStr = name;
                formatTokenLen = sizeof("%name%") - 1;
            } else if (wcsncmp(L"%amount%", format, sizeof("%amount%") - 1) ==
                       0) {
                swprintf_s(tempNumberBuffer, L"%d", numItems);
                srcStr = tempNumberBuffer;
                formatTokenLen = sizeof("%amount%") - 1;
            }

            if (srcStr) {
                bool truncated;
                buffer += StringCopyTruncated(buffer, bufferEnd - buffer,
                                              srcStr, &truncated);
                if (truncated) {
                    break;
                }

                format += formatTokenLen;
                continue;
            }
        }

        *buffer++ = *format++;
    }

    if (*format && bufferSize >= 4) {
        buffer[-1] = L'.';
        buffer[-2] = L'.';
        buffer[-3] = L'.';
    }

    *buffer = L'\0';

    return buffer - bufferStart;
}

bool g_inGroupChanged;
WCHAR g_taskBtnGroupTitleInGroupChanged[256];

using CTaskListWnd_GroupChanged_t = LONG_PTR(WINAPI*)(void* pThis,
                                                      void* taskGroup,
                                                      int taskGroupProperty);
CTaskListWnd_GroupChanged_t CTaskListWnd_GroupChanged_Original;
LONG_PTR WINAPI CTaskListWnd_GroupChanged_Hook(void* pThis,
                                               void* taskGroup,
                                               int taskGroupProperty) {
    Wh_Log(L">");

    int numItems = 0;
    void* taskItem = nullptr;

    void* taskBtnGroup = CTaskListWnd__GetTBGroupFromGroup_Original(
        (BYTE*)pThis - 0x28, taskGroup, nullptr);
    if (taskBtnGroup) {
        numItems = CTaskBtnGroup_GetNumItems_Original(taskBtnGroup);
        if (numItems == 1) {
            taskItem = CTaskBtnGroup_GetTaskItem_Original(taskBtnGroup, 0);
        }
    }

    WCHAR textBuffer[256] = L"";
    CTaskGroup_GetTitleText_Original(taskGroup, taskItem, textBuffer,
                                     ARRAYSIZE(textBuffer));

    FormatLabel(textBuffer, numItems, g_taskBtnGroupTitleInGroupChanged,
                ARRAYSIZE(g_taskBtnGroupTitleInGroupChanged),
                numItems > 1 ? g_settings.labelForMultipleItems.get()
                             : g_settings.labelForSingleItem.get());

    g_inGroupChanged = true;
    LONG_PTR ret =
        CTaskListWnd_GroupChanged_Original(pThis, taskGroup, taskGroupProperty);
    g_inGroupChanged = false;

    *g_taskBtnGroupTitleInGroupChanged = L'\0';

    return ret;
}

using CTaskListWnd_TaskDestroyed_t = LONG_PTR(WINAPI*)(void* pThis,
                                                       void* taskGroup,
                                                       void* taskItem,
                                                       int taskDestroyedFlags);
CTaskListWnd_TaskDestroyed_t CTaskListWnd_TaskDestroyed_Original;
LONG_PTR WINAPI CTaskListWnd_TaskDestroyed_Hook(void* pThis,
                                                void* taskGroup,
                                                void* taskItem,
                                                int taskDestroyedFlags) {
    Wh_Log(L">");

    LONG_PTR ret = CTaskListWnd_TaskDestroyed_Original(
        pThis, taskGroup, taskItem, taskDestroyedFlags);

    // Trigger CTaskListWnd::GroupChanged to trigger the title change.
    int taskGroupProperty = 4;  // saw this in the debugger
    CTaskListWnd_GroupChanged_Hook(pThis, taskGroup, taskGroupProperty);

    return ret;
}

using CTaskListWnd_TaskDestroyed_2_t = LONG_PTR(WINAPI*)(void* pThis,
                                                         void* taskGroup,
                                                         void* taskItem);
CTaskListWnd_TaskDestroyed_2_t CTaskListWnd_TaskDestroyed_2_Original;
LONG_PTR WINAPI CTaskListWnd_TaskDestroyed_2_Hook(void* pThis,
                                                  void* taskGroup,
                                                  void* taskItem) {
    Wh_Log(L">");

    LONG_PTR ret =
        CTaskListWnd_TaskDestroyed_2_Original(pThis, taskGroup, taskItem);

    // Trigger CTaskListWnd::GroupChanged to trigger the title change.
    int taskGroupProperty = 4;  // saw this in the debugger
    CTaskListWnd_GroupChanged_Hook(pThis, taskGroup, taskGroupProperty);

    return ret;
}

void UpdateTaskListButtonWidth(FrameworkElement taskListButtonElement,
                               double widthToSet,
                               bool showLabels) {
    auto iconPanelElement =
        FindChildByName(taskListButtonElement, L"IconPanel");
    if (!iconPanelElement) {
        return;
    }

    auto iconElement = FindChildByName(iconPanelElement, L"Icon");
    if (!iconElement) {
        return;
    }

    // Reset in case an old version of the mod was installed.
    taskListButtonElement.Width(std::numeric_limits<double>::quiet_NaN());

    iconPanelElement.Width(widthToSet);

    iconElement.HorizontalAlignment(showLabels ? HorizontalAlignment::Left
                                               : HorizontalAlignment::Stretch);

    iconElement.Margin(Thickness{
        .Left = showLabels ? g_settings.leftAndRightPaddingSize : 0.0,
    });

    // Badge for non-UWP apps.
    auto overlayIconElement = FindChildByName(iconPanelElement, L"OverlayIcon");
    if (overlayIconElement) {
        overlayIconElement.Margin(Thickness{
            .Right = showLabels ? (widthToSet - iconElement.ActualWidth() -
                                   g_settings.leftAndRightPaddingSize - 4)
                                : 0.0,
        });
    }

    // Badge for UWP apps.
    auto badgeControlElement =
        FindChildByName(iconPanelElement, L"BadgeControl");
    if (badgeControlElement) {
        badgeControlElement.Margin(Thickness{
            .Right = showLabels ? (widthToSet - iconElement.ActualWidth() -
                                   g_settings.leftAndRightPaddingSize - 4)
                                : 0.0,
        });
    }

    PCWSTR indicatorClassNames[] = {
        L"RunningIndicator",
        L"ProgressIndicator",
    };
    for (auto indicatorClassName : indicatorClassNames) {
        auto indicatorElement =
            FindChildByName(iconPanelElement, indicatorClassName);
        if (!indicatorElement) {
            continue;
        }

        bool isProgressIndicator =
            wcscmp(indicatorClassName, L"ProgressIndicator") == 0;

        IndicatorStyle indicatorStyle = isProgressIndicator
                                            ? g_settings.progressIndicatorStyle
                                            : g_settings.runningIndicatorStyle;

        double minWidth = 0;

        if (!g_unloading) {
            if (indicatorStyle == IndicatorStyle::centerDynamic) {
                minWidth = indicatorElement.Width() * widthToSet /
                           g_initialTaskbarItemWidth;
            } else if (indicatorStyle == IndicatorStyle::fullWidth) {
                minWidth = widthToSet - 6;
            }
        }

        indicatorElement.MinWidth(minWidth);

        if (isProgressIndicator) {
            auto element = indicatorElement;
            if ((element = FindChildByName(element, L"LayoutRoot")) &&
                (element = FindChildByName(element, L"ProgressBarRoot")) &&
                (element = FindChildByClassName(
                     element, L"Windows.UI.Xaml.Controls.Border")) &&
                (element = FindChildByClassName(
                     element, L"Windows.UI.Xaml.Controls.Grid")) &&
                (element = FindChildByName(element, L"ProgressBarTrack"))) {
                element.MinWidth(minWidth);
            }
        }

        indicatorElement.Margin(Thickness{
            .Right = indicatorStyle == IndicatorStyle::left && showLabels
                         ? (widthToSet - iconElement.ActualWidth() -
                            g_settings.leftAndRightPaddingSize * 2 - 4)
                         : 0.0,
        });
    }

    // Don't remove, for some reason it causes a bug - the running indicator
    // ends up being behind the semi-transparent rectangle of the active
    // button. Hide it instead.
    auto windhawkTextControl =
        FindChildByName(iconPanelElement, L"WindhawkText")
            .as<Controls::TextBlock>();
    if (windhawkTextControl) {
        if (!showLabels) {
            windhawkTextControl.Visibility(Visibility::Collapsed);
        }

        windhawkTextControl.Margin(Thickness{
            .Left = g_settings.leftAndRightPaddingSize +
                    iconElement.ActualWidth() +
                    g_settings.spaceBetweenIconAndLabel,
            .Right = static_cast<double>(g_settings.leftAndRightPaddingSize),
            .Bottom = 2,
        });

        if (windhawkTextControl.FontSize() != g_settings.fontSize) {
            windhawkTextControl.FontSize(g_settings.fontSize);
        }

        if (showLabels) {
            windhawkTextControl.Visibility(Visibility::Visible);
        }
    }
}

FrameworkElement GetOrCreateLabelSpacer(Controls::Grid iconPanelElement) {
    auto spacerElement =
        FindChildByName(iconPanelElement, L"WindhawkLabelSpacer");
    if (!spacerElement) {
        Controls::Border spacer;
        spacer.Name(L"WindhawkLabelSpacer");
        spacer.Height(0);
        Controls::Grid::SetColumn(spacer, 1);
        iconPanelElement.Children().Append(spacer);
        spacerElement = spacer;
    }

    return spacerElement;
}

// Don't remove the spacer, for some reason it causes a bug - the running
// indicator ends up being behind the semi-transparent rectangle of the active
// button. Hide it instead.
void HideLabelSpacer(Controls::Grid iconPanelElement) {
    auto spacerElement =
        FindChildByName(iconPanelElement, L"WindhawkLabelSpacer");
    if (spacerElement) {
        spacerElement.Width(0);
    }
}

// The native vertical taskbar switches the buttons to a vertical layout with a
// visual state. A taskbar which is rotated as a whole keeps the horizontal
// layout.
bool IsVerticalTaskListButton(FrameworkElement iconPanelElement) {
    for (const auto& group :
         VisualStateManager::GetVisualStateGroups(iconPanelElement)) {
        if (group.Name() == L"OrientationStates") {
            auto currentState = group.CurrentState();
            return currentState &&
                   currentState.Name() == L"VerticalOrientation";
        }
    }

    return false;
}

void UpdateProgressIndicatorTrack(FrameworkElement progressIndicatorElement,
                                  double minWidth,
                                  double height) {
    auto element = progressIndicatorElement;
    if ((element = FindChildByName(element, L"LayoutRoot")) &&
        (element = FindChildByName(element, L"ProgressBarRoot")) &&
        (element = FindChildByClassName(element,
                                        L"Windows.UI.Xaml.Controls.Border")) &&
        (element =
             FindChildByClassName(element, L"Windows.UI.Xaml.Controls.Grid"))) {
        auto progressBarTrack = FindChildByName(element, L"ProgressBarTrack");
        if (progressBarTrack) {
            element.Height(height);
            progressBarTrack.MinWidth(minWidth);
        }
    }
}

// On a vertical taskbar, the indicators are vertical bars on the left of the
// button. The button height doesn't change with the label, so only the full
// width style, which spans the height, differs from the native size. The
// progress indicator is laid out horizontally at the bottom, and its visual is
// rotated by 270 degrees around its top left corner, so its width is the
// length, and its top is the bottom end.
void UpdateVerticalIndicatorStyle(Controls::Grid iconPanelElement,
                                  FrameworkElement indicatorElement,
                                  bool isProgressIndicator,
                                  IndicatorStyle indicatorStyle,
                                  double firstColumnWidthPixels) {
    // Undo the horizontal style. It replaces the left and right margins, which
    // are natively the same as the top one.
    indicatorElement.ClearValue(FrameworkElement::MaxWidthProperty());

    auto indicatorMargin = indicatorElement.Margin();
    indicatorMargin.Left = indicatorMargin.Top;
    indicatorMargin.Right = indicatorMargin.Top;

    double fullLength = 0;
    if (!g_unloading && indicatorStyle == IndicatorStyle::fullWidth) {
        auto iconPanelPadding = iconPanelElement.Padding();
        fullLength =
            std::fmax(iconPanelElement.ActualHeight() - iconPanelPadding.Top -
                          iconPanelPadding.Bottom - indicatorMargin.Top -
                          indicatorMargin.Bottom,
                      0.0);
    }

    double thickness = g_unloading || !g_settings.runningIndicatorHeight
                           ? 3
                           : std::max(g_settings.runningIndicatorHeight, 0);

    // Positive offsets move the indicator away from the icon, like they move
    // the horizontal indicator down.
    float offsetX =
        g_unloading
            ? 0.0f
            : -static_cast<float>(g_settings.runningIndicatorVerticalOffset);
    float offsetY = 0;

    if (isProgressIndicator) {
        indicatorElement.MinWidth(fullLength);
        indicatorElement.Height(thickness);
        UpdateProgressIndicatorTrack(indicatorElement, fullLength, thickness);

        // A layout wider than the icon column gets clipped to it unless its
        // arrange slot is widened with a negative margin.
        double columnOverflow =
            indicatorMargin.Left + fullLength - firstColumnWidthPixels;
        if (columnOverflow > 0) {
            indicatorMargin.Right = -columnOverflow;
        }

        // Move the bottom end of a full length indicator to the bottom of the
        // layout.
        if (fullLength > 0) {
            offsetY = static_cast<float>(
                std::fmax(thickness, indicatorElement.MinHeight()));
        }
    } else {
        indicatorElement.ClearValue(FrameworkElement::MinWidthProperty());
        indicatorElement.MinHeight(fullLength);
        indicatorElement.Width(thickness);
    }

    indicatorElement.Margin(indicatorMargin);

    auto translation = indicatorElement.Translation();
    if (translation.x != offsetX || translation.y != offsetY) {
        translation.x = offsetX;
        translation.y = offsetY;
        indicatorElement.Translation(translation);
    }
}

void UpdateTaskListButtonWithLabelStyle(FrameworkElement taskListButtonElement,
                                        Controls::Grid iconPanelElement) {
    auto iconElement = FindChildByName(iconPanelElement, L"Icon");
    if (!iconElement) {
        return;
    }

    double taskListButtonWidth = taskListButtonElement.ActualWidth();

    // The icon is collapsed while the default icon placeholder is shown, and
    // this runs before layout when the icon size changes, so the actual width
    // can't be relied on.
    double iconWidth = iconElement.Width();
    if (!(iconWidth > 0)) {
        iconWidth = iconElement.ActualWidth();
    }

    auto columnDefinitions = iconPanelElement.ColumnDefinitions();

    // The layout below is relative to the icon column width. Fall back to the
    // stock width of 40 if the column isn't pixel-sized.
    auto firstColumnWidth = columnDefinitions.GetAt(0).Width();
    auto firstColumnWidthPixels =
        firstColumnWidth.GridUnitType == GridUnitType::Pixel
            ? firstColumnWidth.Value
            : 40.0;

    auto iconPanelPadding = iconPanelElement.Padding();

    bool vertical = IsVerticalTaskListButton(iconPanelElement);

    // A vertical taskbar arranges all buttons as wide as the taskbar, so the
    // label fits the button instead of the item width setting. The layout
    // slot, unlike the actual width, doesn't depend on the label.
    double itemWidth = g_settings.taskbarItemWidth;
    if (vertical) {
        auto taskListButtonMargin = taskListButtonElement.Margin();
        itemWidth = Controls::Primitives::LayoutInformation::GetLayoutSlot(
                        taskListButtonElement)
                        .Width -
                    taskListButtonMargin.Left - taskListButtonMargin.Right;
    }

    double secondColumnWidthPixels = g_unloading ? 0 : itemWidth;
    if (secondColumnWidthPixels > 0) {
        secondColumnWidthPixels -= firstColumnWidthPixels +
                                   iconPanelPadding.Left +
                                   iconPanelPadding.Right;
        if (secondColumnWidthPixels < 1) {
            secondColumnWidthPixels = 1;
        }
    }

    auto labelControlElement =
        FindChildByName(iconPanelElement, L"LabelControl")
            .as<Controls::TextBlock>();

    // In labelsWithCombining mode, pinned items have labels too. Hide them,
    // except on a vertical taskbar, which natively shows labels for pinned
    // items.
    if (g_settings.mode == Mode::labelsWithCombining && !vertical &&
        labelControlElement &&
        !TaskListButton_IsRunning(taskListButtonElement)) {
        secondColumnWidthPixels = 0;
        labelControlElement.Visibility(Visibility::Collapsed);
        labelControlElement = nullptr;
    }

    if (labelControlElement) {
        labelControlElement.Visibility(Visibility::Visible);

        auto horizontalAlignment = g_unloading ? HorizontalAlignment::Center
                                               : HorizontalAlignment::Left;
        if (labelControlElement.HorizontalAlignment() != horizontalAlignment) {
            labelControlElement.HorizontalAlignment(horizontalAlignment);
        }

        auto labelControlMargin = labelControlElement.Margin();
        labelControlMargin.Left =
            g_unloading ? 0
                        : (g_settings.leftAndRightPaddingSize + iconWidth +
                           g_settings.spaceBetweenIconAndLabel -
                           firstColumnWidthPixels);
        labelControlMargin.Right =
            g_unloading ? 0 : (g_settings.leftAndRightPaddingSize - 10);
        labelControlElement.Margin(labelControlMargin);

        double columnWidth;
        if (g_unloading || (!vertical && g_settings.taskbarItemWidth == 0)) {
            columnWidth = std::fmax(
                0.0, (g_unloading ? 176 : g_settings.maximumTaskbarItemWidth) -
                         firstColumnWidthPixels);
            HideLabelSpacer(iconPanelElement);
        } else if (vertical) {
            columnWidth = secondColumnWidthPixels;
            HideLabelSpacer(iconPanelElement);
        } else {
            columnWidth = secondColumnWidthPixels;
            GetOrCreateLabelSpacer(iconPanelElement).Width(columnWidth);
        }

        // The column is auto-sized, and the label's desired width includes its
        // margins, so exclude them to keep the column within columnWidth.
        double maxWidth = std::fmax(0.0, columnWidth - labelControlMargin.Left -
                                             labelControlMargin.Right);
        if (labelControlElement.MaxWidth() != maxWidth) {
            labelControlElement.MaxWidth(maxWidth);
            taskListButtonElement.InvalidateMeasure();
        }

        double fontSize = g_unloading ? 12 : g_settings.fontSize;
        if (labelControlElement.FontSize() != fontSize) {
            labelControlElement.FontSize(fontSize);
        }

        PCWSTR fontFamily = g_unloading ? L"" : g_settings.fontFamily.get();
        if (*fontFamily) {
            if (labelControlElement.FontFamily().Source() != fontFamily) {
                labelControlElement.FontFamily(
                    Markup::XamlBindingHelper::ConvertValue(
                        winrt::xaml_typename<Media::FontFamily>(),
                        winrt::box_value(fontFamily))
                        .as<Media::FontFamily>());
            }
        } else {
            auto labelControlElementDp =
                labelControlElement.as<DependencyObject>();
            if (labelControlElementDp.ReadLocalValue(
                    Controls::TextBlock::FontFamilyProperty()) !=
                DependencyProperty::UnsetValue()) {
                labelControlElementDp.ClearValue(
                    Controls::TextBlock::FontFamilyProperty());
            }
        }

        auto textTrimming =
            g_unloading ? TextTrimming::Clip : g_settings.textTrimming;
        if (labelControlElement.TextTrimming() != textTrimming) {
            labelControlElement.TextTrimming(textTrimming);
        }
    } else {
        HideLabelSpacer(iconPanelElement);
    }

    iconElement.HorizontalAlignment((g_unloading || !labelControlElement)
                                        ? HorizontalAlignment::Stretch
                                        : HorizontalAlignment::Left);

    auto iconMargin = iconElement.Margin();
    iconMargin.Left = (g_unloading || !labelControlElement)
                          ? 0.0
                          : g_settings.leftAndRightPaddingSize;
    // A left margin which leaves less than the icon width in the icon column
    // gets the icon clipped to the column unless its arrange slot is widened
    // with a negative right margin.
    iconMargin.Right =
        -std::fmax(0.0, iconMargin.Left + iconWidth - firstColumnWidthPixels);
    iconElement.Margin(iconMargin);

    for (PCWSTR badgeElementName : {
             // Badge for non-UWP apps.
             L"OverlayIcon",
             // Badge for UWP apps.
             L"BadgeControl",
         }) {
        auto badgeElement = FindChildByName(iconPanelElement, badgeElementName);
        if (badgeElement) {
            badgeElement.Margin(Thickness{
                .Right = (g_unloading || !labelControlElement)
                             ? 0.0
                             : (firstColumnWidthPixels - iconWidth) / 2 -
                                   g_settings.leftAndRightPaddingSize,
            });
        }
    }

    // Within the button padding and the native indicator margins.
    double maxIndicatorWidth =
        std::fmax(taskListButtonWidth - iconPanelPadding.Left -
                      iconPanelPadding.Right - 2,
                  0.0);

    PCWSTR indicatorClassNames[] = {
        L"RunningIndicator",
        L"ProgressIndicator",
    };
    for (auto indicatorClassName : indicatorClassNames) {
        auto indicatorElement =
            FindChildByName(iconPanelElement, indicatorClassName);
        if (!indicatorElement) {
            continue;
        }

        bool isProgressIndicator =
            wcscmp(indicatorClassName, L"ProgressIndicator") == 0;

        IndicatorStyle indicatorStyle =
            g_unloading
                ? IndicatorStyle::left
                : (isProgressIndicator ? g_settings.progressIndicatorStyle
                                       : g_settings.runningIndicatorStyle);

        if (vertical) {
            UpdateVerticalIndicatorStyle(iconPanelElement, indicatorElement,
                                         isProgressIndicator, indicatorStyle,
                                         firstColumnWidthPixels);
            continue;
        }

        if (!isProgressIndicator) {
            // Undo the vertical style.
            indicatorElement.ClearValue(FrameworkElement::MinHeightProperty());
        }

        // Keep the indicator in the icon column, which has a fixed pixel
        // width. The sizes below derive from the button's actual width, and
        // spanning into the auto-sized label column would feed them back into
        // the desired width, which becomes a layout cycle once the taskbar is
        // full and the layout scales buttons down.
        indicatorElement.SetValue(Controls::Grid::ColumnSpanProperty(),
                                  winrt::box_value(1));

        double maxWidth = maxIndicatorWidth;
        indicatorElement.MaxWidth(maxWidth);

        double minWidth = 0;

        double indicatorElementWidth = indicatorElement.Width();
        if (indicatorElementWidth > 0) {
            if (indicatorStyle == IndicatorStyle::centerFixed) {
                // Without this, the indicator isn't centered.
                minWidth = indicatorElementWidth;
            } else if (indicatorStyle == IndicatorStyle::centerDynamic) {
                if (firstColumnWidthPixels > 0) {
                    minWidth = indicatorElementWidth * taskListButtonWidth /
                               firstColumnWidthPixels;
                }
            } else if (indicatorStyle == IndicatorStyle::fullWidth) {
                minWidth = maxWidth;
            }

            if (minWidth > maxWidth) {
                minWidth = maxWidth;
            }
        }

        indicatorElement.MinWidth(minWidth);

        auto indicatorMargin = indicatorElement.Margin();
        indicatorMargin.Left = 0;
        indicatorMargin.Right = 0;
        auto indicatorHorizontalAlignment = HorizontalAlignment::Stretch;
        if (!g_unloading && labelControlElement) {
            if (indicatorStyle == IndicatorStyle::left) {
                // Stretch centers the indicator between the left margin and the
                // column edge, so this puts that midpoint on the icon's center.
                indicatorMargin.Left = iconWidth +
                                       g_settings.leftAndRightPaddingSize * 2 -
                                       firstColumnWidthPixels;
            } else {
                indicatorMargin.Left = (taskListButtonWidth - minWidth) / 2 -
                                       iconPanelPadding.Left;
                indicatorHorizontalAlignment = HorizontalAlignment::Left;
            }
        }

        // An indicator wider than the icon column gets clipped to it unless its
        // arrange slot is widened with negative margins. A stretched indicator
        // is centered in the slot, so widen it on both sides to keep the center
        // in place.
        double indicatorWidth =
            indicatorElementWidth > 0
                ? std::clamp(indicatorElementWidth, minWidth, maxWidth)
                : minWidth;
        double columnOverflow =
            indicatorMargin.Left + indicatorWidth - firstColumnWidthPixels;
        if (columnOverflow > 0) {
            if (indicatorHorizontalAlignment == HorizontalAlignment::Stretch) {
                indicatorMargin.Left -= columnOverflow / 2;
                indicatorMargin.Right = -columnOverflow / 2;
            } else {
                indicatorMargin.Right = -columnOverflow;
            }
        }

        indicatorElement.Margin(indicatorMargin);
        indicatorElement.HorizontalAlignment(indicatorHorizontalAlignment);

        int height = g_unloading || !g_settings.runningIndicatorHeight
                         ? 3
                         : std::max(g_settings.runningIndicatorHeight, 0);
        indicatorElement.Height(height);

        float verticalOffset =
            g_unloading
                ? 0.0f
                : static_cast<float>(g_settings.runningIndicatorVerticalOffset);
        auto translation = indicatorElement.Translation();
        if (translation.x != 0 || translation.y != verticalOffset) {
            translation.x = 0;
            translation.y = verticalOffset;
            indicatorElement.Translation(translation);
        }

        if (isProgressIndicator) {
            UpdateProgressIndicatorTrack(indicatorElement, minWidth, height);
        }
    }
}

void UpdateTaskListButtonCustomizations(
    FrameworkElement taskListButtonElement) {
    auto iconPanelElement =
        FindChildByName(taskListButtonElement, L"IconPanel");
    if (!iconPanelElement) {
        return;
    }

    auto iconElement = FindChildByName(iconPanelElement, L"Icon");
    if (!iconElement) {
        return;
    }

    auto taskbarFrameRepeaterElement =
        Media::VisualTreeHelper::GetParent(taskListButtonElement)
            .as<FrameworkElement>();

    if (!taskbarFrameRepeaterElement ||
        taskbarFrameRepeaterElement.Name() != L"TaskbarFrameRepeater") {
        // Can also be "OverflowFlyoutListRepeater".
        return;
    }

    double taskListButtonWidth = taskListButtonElement.ActualWidth();

    double iconPanelWidth = iconPanelElement.ActualWidth();

    // Check if non-positive or NaN.
    if (!(taskListButtonWidth > 0) || !(iconPanelWidth > 0)) {
        return;
    }

    // Only true with the native labels implementation of Windows.
    auto iconPanelGrid = iconPanelElement.as<Controls::Grid>();
    if (iconPanelGrid.ColumnDefinitions().Size() == 2) {
        UpdateTaskListButtonWithLabelStyle(taskListButtonElement,
                                           iconPanelGrid);
        return;
    }

    if (g_hasNativeLabelsImplementation) {
        // Should never happen.
        Wh_Log(L"Unexpected button properties");
        return;
    }

    if (!g_initialTaskbarItemWidth) {
        g_initialTaskbarItemWidth = iconPanelWidth;
    }

    bool isRunning = TaskListButton_IsRunning(taskListButtonElement);
    bool showLabels = isRunning;
    double minWidth =
        std::fmin(g_initialTaskbarItemWidth,
                  static_cast<double>(g_settings.taskbarItemWidth));

    if (g_unloading) {
        showLabels = false;
        minWidth = g_initialTaskbarItemWidth;
    }

    double widthToSet;

    if (showLabels) {
        widthToSet = CalculateTaskbarItemWidth(
            taskbarFrameRepeaterElement, minWidth, g_settings.taskbarItemWidth);

        if (widthToSet <= iconElement.ActualWidth() +
                              g_settings.leftAndRightPaddingSize * 2 +
                              g_settings.spaceBetweenIconAndLabel + 8) {
            showLabels = false;
        }
    } else {
        widthToSet = minWidth;
    }

    auto windhawkTextControl =
        FindChildByName(iconPanelElement, L"WindhawkText")
            .as<Controls::TextBlock>();
    if (!windhawkTextControl) {
        PCWSTR xaml =
            LR"(
                <TextBlock
                    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
                    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
                    xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
                    xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
                    mc:Ignorable="d"
                    Name="WindhawkText"
                    VerticalAlignment="Center"
                    FontSize="12"
                    TextTrimming="CharacterEllipsis"
                />
            )";

        windhawkTextControl =
            Markup::XamlReader::Load(xaml).as<Controls::TextBlock>();

        windhawkTextControl.FontSize(g_settings.fontSize);

        iconPanelElement.as<Controls::Panel>().Children().Append(
            windhawkTextControl);
    }

    UpdateTaskListButtonWidth(taskListButtonElement, widthToSet, showLabels);

    bool textLabelMissing = false;

    if (!isRunning) {
        // The check is important. Without it, there's an infinite rerendering
        // loop.
        if (windhawkTextControl.Text() != L"") {
            windhawkTextControl.Text(L"");
        }
    } else if (showLabels) {
        if (windhawkTextControl.Text() == L"") {
            textLabelMissing = true;
        }
    }

    if (textLabelMissing && !g_unloading) {
        g_taskListButtonsWithLabelMissing.insert(
            winrt::get_abi(windhawkTextControl));

        g_invalidateTaskListButtonTimer =
            SetTimer(nullptr, g_invalidateTaskListButtonTimer, 200,
                     [](HWND hwnd,  // handle of window for timer messages
                        UINT uMsg,  // WM_TIMER message
                        UINT_PTR idEvent,  // timer identifier
                        DWORD dwTime       // current system time
                     ) {
                         KillTimer(nullptr, g_invalidateTaskListButtonTimer);
                         g_invalidateTaskListButtonTimer = 0;

                         if (!g_taskListButtonsWithLabelMissing.empty()) {
                             g_taskListButtonsWithLabelMissing.clear();
                             RecalculateLabels();
                         }
                     });
    } else if (!textLabelMissing && g_invalidateTaskListButtonTimer) {
        g_taskListButtonsWithLabelMissing.erase(
            winrt::get_abi(windhawkTextControl));
    }
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    Wh_Log(L">");

    TaskListButton_UpdateVisualStates_Original(pThis);

    void* taskListButtonIUnknownPtr = (void**)pThis + 3;
    winrt::Windows::Foundation::IUnknown taskListButtonIUnknown;
    winrt::copy_from_abi(taskListButtonIUnknown, taskListButtonIUnknownPtr);

    auto taskListButtonElement = taskListButtonIUnknown.as<FrameworkElement>();

    UpdateTaskListButtonCustomizations(taskListButtonElement);
}

using TaskListButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateButtonPadding_t
    TaskListButton_UpdateButtonPadding_Original;
void WINAPI TaskListButton_UpdateButtonPadding_Hook(void* pThis) {
    Wh_Log(L">");

    TaskListButton_UpdateButtonPadding_Original(pThis);

    if (g_hasNativeLabelsImplementation) {
        return;
    }

    void* taskListButtonIUnknownPtr = (void**)pThis + 3;
    winrt::Windows::Foundation::IUnknown taskListButtonIUnknown;
    winrt::copy_from_abi(taskListButtonIUnknown, taskListButtonIUnknownPtr);

    auto taskListButtonElement = taskListButtonIUnknown.as<FrameworkElement>();

    UpdateTaskListButtonCustomizations(taskListButtonElement);
}

using TaskListButton_UpdateBadgeSize_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateBadgeSize_t TaskListButton_UpdateBadgeSize_Original;
void WINAPI TaskListButton_UpdateBadgeSize_Hook(void* pThis) {
    Wh_Log(L">");

    TaskListButton_UpdateBadgeSize_Original(pThis);

    if (g_hasNativeLabelsImplementation) {
        return;
    }

    void* taskListButtonIUnknownPtr = (void**)pThis + 3;
    winrt::Windows::Foundation::IUnknown taskListButtonIUnknown;
    winrt::copy_from_abi(taskListButtonIUnknown, taskListButtonIUnknownPtr);

    auto taskListButtonElement = taskListButtonIUnknown.as<FrameworkElement>();

    UpdateTaskListButtonCustomizations(taskListButtonElement);
}

using TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_t =
    void(WINAPI*)(void* pThis);
TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_t
    TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original;
void WINAPI TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Hook(void* pThis) {
    Wh_Log(L">");

    TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original(pThis);

    if (g_hasNativeLabelsImplementation) {
        return;
    }

    void* taskbarFrameIUnknownPtr = (void**)pThis + 3;
    winrt::Windows::Foundation::IUnknown taskbarFrameIUnknown;
    winrt::copy_from_abi(taskbarFrameIUnknown, taskbarFrameIUnknownPtr);

    auto taskbarFrameElement = taskbarFrameIUnknown.as<FrameworkElement>();

    auto taskbarFrameRepeaterContainerElement =
        FindChildByName(taskbarFrameElement, L"RootGrid");
    if (!taskbarFrameRepeaterContainerElement) {
        // For older versions (pre-KB5022913).
        taskbarFrameRepeaterContainerElement =
            FindChildByName(taskbarFrameElement, L"TaskbarFrameBorder");
    }

    if (!taskbarFrameRepeaterContainerElement) {
        return;
    }

    auto taskbarFrameRepeaterElement = FindChildByName(
        taskbarFrameRepeaterContainerElement, L"TaskbarFrameRepeater");
    if (!taskbarFrameRepeaterElement) {
        return;
    }

    auto repeater =
        taskbarFrameRepeaterElement
            .try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>();
    if (!repeater) {
        Wh_Log(L"Not an ItemsRepeater");
        return;
    }

    auto itemsSourceView = repeater.ItemsSourceView();
    if (!itemsSourceView) {
        Wh_Log(L"No ItemsSourceView");
        return;
    }

    int count = itemsSourceView.Count();

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

        if (child.Name() == L"TaskListButton") {
            UpdateTaskListButtonCustomizations(child);
        }
    }
}

using TaskListButton_Icon_t = void(WINAPI*)(void* pThis,
                                            LONG_PTR randomAccessStream);
TaskListButton_Icon_t TaskListButton_Icon_Original;
void WINAPI TaskListButton_Icon_Hook(void* pThis, LONG_PTR randomAccessStream) {
    Wh_Log(L">");

    TaskListButton_Icon_Original(pThis, randomAccessStream);

    if (g_hasNativeLabelsImplementation) {
        return;
    }

    if (!g_inGroupChanged) {
        return;
    }

    void* taskListButtonIUnknownPtr = (void**)pThis + 3;
    winrt::Windows::Foundation::IUnknown taskListButtonIUnknown;
    winrt::copy_from_abi(taskListButtonIUnknown, taskListButtonIUnknownPtr);

    auto taskListButtonElement = taskListButtonIUnknown.as<FrameworkElement>();

    auto iconPanelElement =
        FindChildByName(taskListButtonElement, L"IconPanel");
    if (iconPanelElement) {
        auto windhawkTextControl =
            FindChildByName(iconPanelElement, L"WindhawkText")
                .as<Controls::TextBlock>();
        if (windhawkTextControl) {
            // Avoid setting empty text as it's used for missing titles.
            windhawkTextControl.Text(*g_taskBtnGroupTitleInGroupChanged
                                         ? g_taskBtnGroupTitleInGroupChanged
                                         : L" ");
        }
    }
}

using ITaskbarButton_get_MinScalableWidth_t = HRESULT(WINAPI*)(void* pThis,
                                                               float* minWidth);
ITaskbarButton_get_MinScalableWidth_t
    ITaskbarButton_get_MinScalableWidth_Original;
HRESULT ITaskbarButton_get_MinScalableWidth_Hook(void* pThis, float* minWidth) {
    Wh_Log(L">");

    HRESULT ret = ITaskbarButton_get_MinScalableWidth_Original(pThis, minWidth);

    if (SUCCEEDED(ret) && !g_unloading && g_hasNativeLabelsImplementation &&
        *minWidth > 0) {
        // Allow to create many taskbar items before overflow appears.
        int minimumTaskbarItemWidth = g_settings.minimumTaskbarItemWidth;
        if (minimumTaskbarItemWidth < 1) {
            minimumTaskbarItemWidth = 1;
            Wh_Log(L"minimumTaskbarItemWidth too small, using %d",
                   minimumTaskbarItemWidth);
        }

        if (*minWidth >= minimumTaskbarItemWidth) {
            *minWidth = minimumTaskbarItemWidth;
        } else {
            Wh_Log(L"minimumTaskbarItemWidth too large, using default (%f)",
                   *minWidth);
        }
    }

    return ret;
}

using TaskbarCollapsibleLayout_ArrangeOverride_t =
    HRESULT(WINAPI*)(void* pThis,
                     void* context,
                     winrt::Windows::Foundation::Size finalSize,
                     winrt::Windows::Foundation::Size* resultSize);
TaskbarCollapsibleLayout_ArrangeOverride_t
    TaskbarCollapsibleLayout_ArrangeOverride_Original;
HRESULT WINAPI TaskbarCollapsibleLayout_ArrangeOverride_Hook(
    void* pThis,
    void* context,
    winrt::Windows::Foundation::Size finalSize,
    winrt::Windows::Foundation::Size* resultSize) {
    Wh_Log(L">");

    HRESULT ret = TaskbarCollapsibleLayout_ArrangeOverride_Original(
        pThis, context, finalSize, resultSize);
    if (FAILED(ret)) {
        return ret;
    }

    // The layout skips dragged items, which are moved with a translation, so
    // layout changes inside them stay pending until the drop. A label with
    // ellipsis trimming isn't rendered while its layout is pending, so arrange
    // the items in their current slots.
    winrt::Microsoft::UI::Xaml::Controls::VirtualizingLayoutContext
        layoutContext{nullptr};
    winrt::copy_from_abi(layoutContext, context);
    if (layoutContext.ItemCount() == 0) {
        return ret;
    }

    auto repeaterElement = Media::VisualTreeHelper::GetParent(
        layoutContext.GetOrCreateElementAt(0));
    if (!repeaterElement) {
        return ret;
    }

    int childrenCount =
        Media::VisualTreeHelper::GetChildrenCount(repeaterElement);
    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(repeaterElement, i)
                         .try_as<FrameworkElement>();
        if (child && child.Name() == L"TaskListButton") {
            child.Arrange(
                Controls::Primitives::LayoutInformation::GetLayoutSlot(child));
        }
    }

    return ret;
}

// The width that Windows uses for a vertical taskbar with labels.
constexpr double kVerticalTaskbarWithLabelsWidth = 160;

// TaskbarController::UpdateFrameHeight reports the frame size before it sets
// the frame width.
thread_local double g_updateFrameHeightFrameSize;

using TaskbarTelemetry_FrameSizeChanged_t =
    void(WINAPI*)(const double* frameSize);
TaskbarTelemetry_FrameSizeChanged_t TaskbarTelemetry_FrameSizeChanged_Original;
void WINAPI TaskbarTelemetry_FrameSizeChanged_Hook(const double* frameSize) {
    Wh_Log(L">");

    g_updateFrameHeightFrameSize = *frameSize;

    TaskbarTelemetry_FrameSizeChanged_Original(frameSize);
}

// Returns the taskbar frame, and creates it if it doesn't exist yet.
using TaskbarController_View_t = void*(WINAPI*)(void* pThis, void** result);
TaskbarController_View_t TaskbarController_View_Original;

// Sets the frame width to NaN for a horizontal taskbar. A vertical taskbar is
// as wide as the frame size if items are always combined, and wide enough for
// labels otherwise, which only fits the modes that tie labels to combining.
using TaskbarController_UpdateFrameHeight_t = void(WINAPI*)(void* pThis);
TaskbarController_UpdateFrameHeight_t
    TaskbarController_UpdateFrameHeight_Original;
void WINAPI TaskbarController_UpdateFrameHeight_Hook(void* pThis) {
    Wh_Log(L">");

    Mode mode = g_settings.mode;
    if (g_unloading || !TaskbarController_View_Original ||
        (mode != Mode::labelsWithCombining &&
         mode != Mode::noLabelsWithoutCombining)) {
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }

    g_updateFrameHeightFrameSize = 0;

    TaskbarController_UpdateFrameHeight_Original(pThis);

    double frameSize = g_updateFrameHeightFrameSize;
    if (frameSize <= 0) {
        return;
    }

    double width = mode == Mode::labelsWithCombining
                       ? kVerticalTaskbarWithLabelsWidth
                       : frameSize;

    // The frame is created before anything calls UpdateFrameHeight.
    FrameworkElement taskbarFrame = nullptr;
    TaskbarController_View_Original(pThis, winrt::put_abi(taskbarFrame));
    if (!taskbarFrame) {
        return;
    }

    // Another mod hooking this function may have replaced the width that
    // Windows set, which is one of these. The NaN width of a horizontal taskbar
    // is neither.
    double currentWidth = taskbarFrame.Width();
    if ((currentWidth == frameSize ||
         currentWidth == kVerticalTaskbarWithLabelsWidth) &&
        currentWidth != width) {
        Wh_Log(L"Vertical taskbar width: %f->%f", currentWidth, width);
        taskbarFrame.Width(width);
    }
}

// Experience buttons, such as Start, Search and Widgets, get the grouping mode,
// which only affects them on a vertical taskbar. They show labels there unless
// items are always combined.
int GetExperienceButtonGroupingMode(int groupingMode) {
    // Same values as TaskbarGlomLevel: 0 - always combine, 2 - never combine.
    if (!g_unloading) {
        if (g_settings.mode == Mode::labelsWithCombining) {
            return 2;
        } else if (g_settings.mode == Mode::noLabelsWithoutCombining) {
            return 0;
        }
    }

    return groupingMode;
}

using ExperienceToggleButton_TaskbarItemGroupingMode_t =
    void(WINAPI*)(void* pThis, int groupingMode);
ExperienceToggleButton_TaskbarItemGroupingMode_t
    ExperienceToggleButton_TaskbarItemGroupingMode_Original;
void WINAPI ExperienceToggleButton_TaskbarItemGroupingMode_Hook(
    void* pThis,
    int groupingMode) {
    Wh_Log(L">");

    ExperienceToggleButton_TaskbarItemGroupingMode_Original(
        pThis, GetExperienceButtonGroupingMode(groupingMode));
}

// The view model of the widgets button collapses it to an icon on a vertical
// taskbar if items are always combined.
using AugmentedEntryPointViewModel_put_TaskbarItemGroupingMode_t =
    HRESULT(WINAPI*)(void* pThis, int groupingMode);
AugmentedEntryPointViewModel_put_TaskbarItemGroupingMode_t
    AugmentedEntryPointViewModel_put_TaskbarItemGroupingMode_Original;
HRESULT WINAPI AugmentedEntryPointViewModel_put_TaskbarItemGroupingMode_Hook(
    void* pThis,
    int groupingMode) {
    Wh_Log(L">");

    return AugmentedEntryPointViewModel_put_TaskbarItemGroupingMode_Original(
        pThis, GetExperienceButtonGroupingMode(groupingMode));
}

// The search button of SearchUx.UI.dll is bound to its view model for the
// grouping mode, which the view model reads from the taskbar settings.
using SearchButtonBase_put_TaskbarItemGroupingMode_t =
    HRESULT(WINAPI*)(void* pThis, int groupingMode);
SearchButtonBase_put_TaskbarItemGroupingMode_t
    SearchButtonBase_put_TaskbarItemGroupingMode_Original;
HRESULT WINAPI SearchButtonBase_put_TaskbarItemGroupingMode_Hook(
    void* pThis,
    int groupingMode) {
    Wh_Log(L">");

    return SearchButtonBase_put_TaskbarItemGroupingMode_Original(
        pThis, GetExperienceButtonGroupingMode(groupingMode));
}

// On a vertical taskbar, the tray uses the compact layout, a single column of
// icons in a narrow frame, if items are always combined, and the detailed
// layout, grids of icons in a frame as wide as a taskbar with labels,
// otherwise. Like the experience buttons, it follows the labels.
using TaskbarSettings_GroupingMode_t = int(WINAPI*)(void* pThis);
TaskbarSettings_GroupingMode_t TaskbarSettings_GroupingMode_Original;
int WINAPI TaskbarSettings_GroupingMode_Hook(void* pThis) {
    Wh_Log(L">");

    return GetExperienceButtonGroupingMode(
        TaskbarSettings_GroupingMode_Original(pThis));
}

bool IsAppIdExcluded(PCWSTR appId) {
    std::wstring appIdUpper = appId;
    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, appIdUpper.data(),
                  static_cast<int>(appIdUpper.length()), appIdUpper.data(),
                  static_cast<int>(appIdUpper.length()), nullptr, nullptr, 0);
    if (!g_settings.excludedPrograms.contains(appIdUpper)) {
        return false;
    }

    Wh_Log(L"Excluding %s", appId);
    return true;
}

bool IsWindowProcessExcluded(HWND hWnd) {
    DWORD resolvedWindowProcessPathLen = 0;
    WCHAR resolvedWindowProcessPath[MAX_PATH];
    WCHAR resolvedWindowProcessPathUpper[MAX_PATH];

    DWORD dwProcessId = 0;
    if (GetWindowThreadProcessId(hWnd, &dwProcessId)) {
        HANDLE hProcess =
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
        if (hProcess) {
            DWORD dwSize = ARRAYSIZE(resolvedWindowProcessPath);
            if (QueryFullProcessImageName(hProcess, 0,
                                          resolvedWindowProcessPath, &dwSize)) {
                resolvedWindowProcessPathLen = dwSize;
            }

            CloseHandle(hProcess);
        }
    }

    if (resolvedWindowProcessPathLen == 0) {
        return false;
    }

    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE,
                  resolvedWindowProcessPath, resolvedWindowProcessPathLen + 1,
                  resolvedWindowProcessPathUpper,
                  resolvedWindowProcessPathLen + 1, nullptr, nullptr, 0);

    bool excluded =
        g_settings.excludedPrograms.contains(resolvedWindowProcessPathUpper);

    if (!excluded) {
        if (PCWSTR programFileNameUpper =
                wcsrchr(resolvedWindowProcessPathUpper, L'\\')) {
            programFileNameUpper++;
            if (*programFileNameUpper &&
                g_settings.excludedPrograms.contains(programFileNameUpper)) {
                excluded = true;
            }
        }
    }

    if (excluded) {
        Wh_Log(L"Excluding %s", resolvedWindowProcessPath);
    }

    return excluded;
}

// WindowsUdk.UI.Shell interfaces, declared up to the last used method. A WinRT
// interface doesn't change once published, so a successful query by IID
// guarantees the layout.

// {b081d9d6-9b45-5363-8a4d-854add6abe7e}
constexpr winrt::guid IID_ITaskItem{
    0xb081d9d6,
    0x9b45,
    0x5363,
    {0x8a, 0x4d, 0x85, 0x4a, 0xdd, 0x6a, 0xbe, 0x7e}};

struct ITaskItem : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_AppId(void** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UniqueId() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WindowId(HWND* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DisplayName(void** value) = 0;
};

// {a152e779-93df-5be2-af2c-d342452b0ce0}
constexpr winrt::guid IID_ITaskGroup{
    0xa152e779,
    0x93df,
    0x5be2,
    {0xaf, 0x2c, 0xd3, 0x42, 0x45, 0x2b, 0x0c, 0xe0}};

struct ITaskGroup : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_AppId(void** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UniqueId() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DisplayName() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AccessibleName() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Icon() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_OverlayIcon() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_OverlayIconDescription() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Badge() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsRequestingAttention() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Progress() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_VisualState() = 0;
    virtual HRESULT STDMETHODCALLTYPE put_VisualState() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Bounds() = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Bounds() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Items(void** value) = 0;
};

// {1f56dcc7-b47b-5d55-9a34-871b82505851}
constexpr winrt::guid IID_ITaskItemPropertyChangedEventArgs{
    0x1f56dcc7,
    0xb47b,
    0x5d55,
    {0x9a, 0x34, 0x87, 0x1b, 0x82, 0x50, 0x58, 0x51}};

struct ITaskItemPropertyChangedEventArgs : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Group(void** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Item(void** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Property(int* value) = 0;
};

// WindowsUdk.UI.Shell.TaskItemProperty.DisplayName
constexpr int kTaskItemPropertyDisplayName = 2;

HWND GetTaskItemWindow(IUnknown* taskItemUnknown) {
    winrt::com_ptr<ITaskItem> taskItem;
    HRESULT hr =
        taskItemUnknown->QueryInterface(IID_ITaskItem, taskItem.put_void());
    if (FAILED(hr)) {
        return nullptr;
    }

    HWND hWnd = nullptr;
    if (FAILED(taskItem->get_WindowId(&hWnd))) {
        return nullptr;
    }

    return hWnd;
}

bool IsTaskItemExcluded(IUnknown* taskItemUnknown) {
    winrt::com_ptr<ITaskItem> taskItem;
    HRESULT hr =
        taskItemUnknown->QueryInterface(IID_ITaskItem, taskItem.put_void());
    if (FAILED(hr)) {
        return false;
    }

    winrt::hstring appId;
    hr = taskItem->get_AppId(winrt::put_abi(appId));
    if (SUCCEEDED(hr) && IsAppIdExcluded(appId.c_str())) {
        return true;
    }

    HWND hWnd = nullptr;
    hr = taskItem->get_WindowId(&hWnd);
    return SUCCEEDED(hr) && hWnd && IsWindowProcessExcluded(hWnd);
}

// Finds another interface of a C++/WinRT object by its vtable. The interface
// pointers are adjacent in the object, so only a few of them are searched, in
// the given direction (1 or -1).
void* FindAdjacentInterface(void* pInterface, void* vtable, int direction) {
    for (int i = 1; i <= 16; i++) {
        void** p = (void**)pInterface + i * direction;
        if (*p == vtable) {
            return p;
        }
    }

    return nullptr;
}

void* ITaskListWindowViewModel_vftable;

using ITaskListWindowViewModel_get_TaskItem_t =
    HRESULT(WINAPI*)(void* pThis, void** taskItem);
ITaskListWindowViewModel_get_TaskItem_t ITaskListWindowViewModel_get_TaskItem;

using TaskListWindowViewModel_ITaskbarAppItemViewModel_get_HasLabel_t =
    HRESULT(WINAPI*)(void* pThis, bool* hasLabels);
TaskListWindowViewModel_ITaskbarAppItemViewModel_get_HasLabel_t
    TaskListWindowViewModel_ITaskbarAppItemViewModel_get_HasLabel_Original;
HRESULT WINAPI
TaskListWindowViewModel_ITaskbarAppItemViewModel_get_HasLabel_Hook(
    void* pThis,
    bool* hasLabels) {
    Wh_Log(L">");

    HRESULT ret =
        TaskListWindowViewModel_ITaskbarAppItemViewModel_get_HasLabel_Original(
            pThis, hasLabels);
    if (g_unloading || FAILED(ret) || !*hasLabels) {
        return ret;
    }

    bool hideLabels = false;
    if (g_settings.mode == Mode::noLabelsWithoutCombining) {
        hideLabels = true;
    }

    if (!g_settings.excludedPrograms.empty() &&
        ITaskListWindowViewModel_vftable &&
        ITaskListWindowViewModel_get_TaskItem) {
        void* pITaskListWindowViewModel =
            FindAdjacentInterface(pThis, ITaskListWindowViewModel_vftable, -1);
        if (pITaskListWindowViewModel) {
            winrt::com_ptr<IUnknown> taskItem;
            HRESULT hr = ITaskListWindowViewModel_get_TaskItem(
                pITaskListWindowViewModel, taskItem.put_void());
            if (SUCCEEDED(hr) && taskItem &&
                IsTaskItemExcluded(taskItem.get())) {
                hideLabels = !hideLabels;
            }
        } else {
            Wh_Log(L"ITaskListWindowViewModel not found");
        }
    }

    if (hideLabels) {
        *hasLabels = false;
    }

    return ret;
}

using TaskListGroupViewModel_ITaskbarAppItemViewModel_get_TaskGroup_t =
    HRESULT(WINAPI*)(void* pThis, void** taskGroup);
TaskListGroupViewModel_ITaskbarAppItemViewModel_get_TaskGroup_t
    TaskListGroupViewModel_ITaskbarAppItemViewModel_get_TaskGroup;

// The windows of a group share its app ID, so only the process of the first
// window is checked in addition to it.
bool IsTaskGroupExcluded(void* pITaskbarAppItemViewModel, bool requireWindows) {
    winrt::com_ptr<IUnknown> taskGroupUnknown;
    HRESULT hr = TaskListGroupViewModel_ITaskbarAppItemViewModel_get_TaskGroup(
        pITaskbarAppItemViewModel, taskGroupUnknown.put_void());
    if (FAILED(hr) || !taskGroupUnknown) {
        return false;
    }

    winrt::com_ptr<ITaskGroup> taskGroup;
    hr = taskGroupUnknown->QueryInterface(IID_ITaskGroup, taskGroup.put_void());
    if (FAILED(hr)) {
        return false;
    }

    // An IVectorView<TaskItem>, which has the same ABI.
    winrt::Windows::Foundation::Collections::IVectorView<
        winrt::Windows::Foundation::IInspectable>
        items;
    hr = taskGroup->get_Items(winrt::put_abi(items));

    HWND hWnd = nullptr;
    if (SUCCEEDED(hr) && items && items.Size() > 0) {
        auto item = items.GetAt(0);
        hWnd = GetTaskItemWindow(static_cast<IUnknown*>(winrt::get_abi(item)));
    }

    if (!hWnd && requireWindows) {
        return false;
    }

    winrt::hstring appId;
    hr = taskGroup->get_AppId(winrt::put_abi(appId));
    if (SUCCEEDED(hr) && IsAppIdExcluded(appId.c_str())) {
        return true;
    }

    return hWnd && IsWindowProcessExcluded(hWnd);
}

using TaskListGroupViewModel_ITaskbarAppItemViewModel_get_HasLabel_t =
    HRESULT(WINAPI*)(void* pThis, bool* hasLabels);
TaskListGroupViewModel_ITaskbarAppItemViewModel_get_HasLabel_t
    TaskListGroupViewModel_ITaskbarAppItemViewModel_get_HasLabel_Original;
HRESULT WINAPI
TaskListGroupViewModel_ITaskbarAppItemViewModel_get_HasLabel_Hook(
    void* pThis,
    bool* hasLabels) {
    Wh_Log(L">");

    HRESULT ret =
        TaskListGroupViewModel_ITaskbarAppItemViewModel_get_HasLabel_Original(
            pThis, hasLabels);
    if (g_unloading || FAILED(ret)) {
        return ret;
    }

    if (g_settings.mode == Mode::labelsWithCombining) {
        *hasLabels = true;
    } else if (g_settings.mode == Mode::noLabelsWithoutCombining) {
        *hasLabels = false;
    }

    // Labels are only added to groups with windows, leaving the labels of
    // pinned items to Windows.
    bool excludedHasLabels = g_settings.mode == Mode::noLabelsWithCombining ||
                             g_settings.mode == Mode::noLabelsWithoutCombining;
    if (*hasLabels != excludedHasLabels &&
        !g_settings.excludedPrograms.empty() &&
        TaskListGroupViewModel_ITaskbarAppItemViewModel_get_TaskGroup &&
        IsTaskGroupExcluded(pThis, /*requireWindows=*/excludedHasLabels)) {
        *hasLabels = excludedHasLabels;
    }

    return ret;
}

void* TaskListWindowViewModel_ITaskbarAppItemViewModel_vftable;
void* TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable;

void** g_taskListWindowViewModelHasLabelSlot;
void** g_taskListGroupViewModelHasLabelSlot;

// Leaves the slot intact if it doesn't hold the expected value, e.g. if another
// mod patched it.
bool SetVtableSlot(void** slot, void* expected, void* value) {
    DWORD oldProtect;
    if (!VirtualProtect(slot, sizeof(*slot), PAGE_READWRITE, &oldProtect)) {
        return false;
    }

    bool set =
        InterlockedCompareExchangePointer(slot, value, expected) == expected;
    VirtualProtect(slot, sizeof(*slot), oldProtect, &oldProtect);
    return set;
}

// The HasLabel getters are hooked in their vtable slots, since the linker can
// fold a getter with other identical ones. For example, get_HasLabel and
// get_IsRunning of TaskListWindowViewModel both return true, and hooking the
// shared code would hide the running indicator along with the label.
void HookHasLabelVtableSlots() {
    auto windowVtable =
        (void**)TaskListWindowViewModel_ITaskbarAppItemViewModel_vftable;
    auto groupVtable =
        (void**)TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable;
    auto windowGetHasLabel = (void*)
        TaskListWindowViewModel_ITaskbarAppItemViewModel_get_HasLabel_Original;
    auto groupGetHasLabel = (void*)
        TaskListGroupViewModel_ITaskbarAppItemViewModel_get_HasLabel_Original;
    if (!windowVtable || !groupVtable || !windowGetHasLabel ||
        !groupGetHasLabel) {
        return;
    }

    auto windowHook = (void*)
        TaskListWindowViewModel_ITaskbarAppItemViewModel_get_HasLabel_Hook;
    auto groupHook = (void*)
        TaskListGroupViewModel_ITaskbarAppItemViewModel_get_HasLabel_Hook;

    // Both vtables are of the same interface. The group getter reads a field,
    // so its address is unique in its vtable, unlike the window getter's. The
    // first 6 slots are of IInspectable.
    for (int i = 6; i < 64; i++) {
        if (groupVtable[i] != groupGetHasLabel) {
            continue;
        }

        if (!SetVtableSlot(&windowVtable[i], windowGetHasLabel, windowHook) ||
            !SetVtableSlot(&groupVtable[i], groupGetHasLabel, groupHook)) {
            Wh_Log(L"Failed to set HasLabel vtable slot %d", i);
            SetVtableSlot(&windowVtable[i], windowHook, windowGetHasLabel);
            return;
        }

        g_taskListWindowViewModelHasLabelSlot = &windowVtable[i];
        g_taskListGroupViewModelHasLabelSlot = &groupVtable[i];
        return;
    }

    Wh_Log(L"HasLabel vtable slot not found");
}

void UnhookHasLabelVtableSlots() {
    if (g_taskListWindowViewModelHasLabelSlot) {
        SetVtableSlot(
            g_taskListWindowViewModelHasLabelSlot,
            (void*)
                TaskListWindowViewModel_ITaskbarAppItemViewModel_get_HasLabel_Hook,
            (void*)
                TaskListWindowViewModel_ITaskbarAppItemViewModel_get_HasLabel_Original);
        g_taskListWindowViewModelHasLabelSlot = nullptr;
    }

    if (g_taskListGroupViewModelHasLabelSlot) {
        SetVtableSlot(
            g_taskListGroupViewModelHasLabelSlot,
            (void*)
                TaskListGroupViewModel_ITaskbarAppItemViewModel_get_HasLabel_Hook,
            (void*)
                TaskListGroupViewModel_ITaskbarAppItemViewModel_get_HasLabel_Original);
        g_taskListGroupViewModelHasLabelSlot = nullptr;
    }
}

using TaskListGroupViewModel_OnPropertyChanged_t =
    void(WINAPI*)(void* pThis, const std::wstring_view& propertyName);
TaskListGroupViewModel_OnPropertyChanged_t
    TaskListGroupViewModel_OnPropertyChanged;

// In the combining modes, an item with a single window is labeled with the
// window title, as in the modes without combining.
bool UseSingleWindowTitles() {
    return !g_unloading && (g_settings.mode == Mode::labelsWithCombining ||
                            g_settings.mode == Mode::noLabelsWithCombining);
}

// Returns an empty string unless the group has exactly one window.
winrt::hstring GetTaskGroupSingleWindowTitle(void* pITaskbarAppItemViewModel) {
    winrt::com_ptr<IUnknown> taskGroupUnknown;
    HRESULT hr = TaskListGroupViewModel_ITaskbarAppItemViewModel_get_TaskGroup(
        pITaskbarAppItemViewModel, taskGroupUnknown.put_void());
    if (FAILED(hr) || !taskGroupUnknown) {
        return {};
    }

    winrt::com_ptr<ITaskGroup> taskGroup;
    hr = taskGroupUnknown->QueryInterface(IID_ITaskGroup, taskGroup.put_void());
    if (FAILED(hr)) {
        return {};
    }

    // An IVectorView<TaskItem>, which has the same ABI.
    winrt::Windows::Foundation::Collections::IVectorView<
        winrt::Windows::Foundation::IInspectable>
        items;
    hr = taskGroup->get_Items(winrt::put_abi(items));
    if (FAILED(hr) || !items || items.Size() != 1) {
        return {};
    }

    auto item = items.GetAt(0);
    winrt::com_ptr<ITaskItem> taskItem;
    hr = static_cast<IUnknown*>(winrt::get_abi(item))
             ->QueryInterface(IID_ITaskItem, taskItem.put_void());
    if (FAILED(hr)) {
        return {};
    }

    winrt::hstring title;
    taskItem->get_DisplayName(winrt::put_abi(title));
    return title;
}

// The ABI getter is hooked to change the label while keeping the accessible
// name, which uses the implementation getter.
using TaskListGroupViewModel_ITaskbarItemViewModel_get_DisplayName_t =
    HRESULT(WINAPI*)(void* pThis, void** value);
TaskListGroupViewModel_ITaskbarItemViewModel_get_DisplayName_t
    TaskListGroupViewModel_ITaskbarItemViewModel_get_DisplayName_Original;
HRESULT WINAPI
TaskListGroupViewModel_ITaskbarItemViewModel_get_DisplayName_Hook(
    void* pThis,
    void** value) {
    Wh_Log(L">");

    HRESULT ret =
        TaskListGroupViewModel_ITaskbarItemViewModel_get_DisplayName_Original(
            pThis, value);
    if (FAILED(ret) || !UseSingleWindowTitles() ||
        !TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable ||
        !TaskListGroupViewModel_ITaskbarAppItemViewModel_get_TaskGroup) {
        return ret;
    }

    // ITaskbarAppItemViewModel follows ITaskbarItemViewModel in the object.
    void* pITaskbarAppItemViewModel = FindAdjacentInterface(
        pThis, TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable, 1);
    if (!pITaskbarAppItemViewModel) {
        Wh_Log(L"ITaskbarAppItemViewModel not found");
        return ret;
    }

    winrt::hstring title =
        GetTaskGroupSingleWindowTitle(pITaskbarAppItemViewModel);
    if (!title.empty()) {
        // Releases the original string.
        winrt::hstring displayName;
        winrt::attach_abi(displayName, *value);

        *value = winrt::detach_abi(title);
    }

    return ret;
}

using TaskListGroupViewModel_OnTaskItemPropertyChanged_t =
    void(WINAPI*)(void* pThis, void* args);
TaskListGroupViewModel_OnTaskItemPropertyChanged_t
    TaskListGroupViewModel_OnTaskItemPropertyChanged_Original;
void WINAPI TaskListGroupViewModel_OnTaskItemPropertyChanged_Hook(void* pThis,
                                                                  void* args) {
    Wh_Log(L">");

    TaskListGroupViewModel_OnTaskItemPropertyChanged_Original(pThis, args);

    // A group doesn't notify about a change of its windows' titles.
    if (!UseSingleWindowTitles() || !TaskListGroupViewModel_OnPropertyChanged) {
        return;
    }

    winrt::com_ptr<ITaskItemPropertyChangedEventArgs> eventArgs;
    HRESULT hr = (*(IUnknown**)args)
                     ->QueryInterface(IID_ITaskItemPropertyChangedEventArgs,
                                      eventArgs.put_void());
    if (FAILED(hr)) {
        return;
    }

    int property;
    if (SUCCEEDED(eventArgs->get_Property(&property)) &&
        property == kTaskItemPropertyDisplayName) {
        TaskListGroupViewModel_OnPropertyChanged(pThis, L"DisplayName");
    }
}

void OnTaskGroupWindowsChanged(void* pThis) {
    // Excluded programs are matched by the group's windows, but a change of
    // the windows isn't followed by a native HasLabel notification.
    if (!g_unloading && !g_settings.excludedPrograms.empty() &&
        TaskListGroupViewModel_OnPropertyChanged) {
        TaskListGroupViewModel_OnPropertyChanged(pThis, L"HasLabel");
    }

    // The label depends on the number of windows.
    if (UseSingleWindowTitles() && TaskListGroupViewModel_OnPropertyChanged) {
        TaskListGroupViewModel_OnPropertyChanged(pThis, L"DisplayName");
    }
}

using TaskListGroupViewModel_OnTaskItemsCollectionChanged_t =
    void(WINAPI*)(void* pThis, void* args);
TaskListGroupViewModel_OnTaskItemsCollectionChanged_t
    TaskListGroupViewModel_OnTaskItemsCollectionChanged_Original;
void WINAPI
TaskListGroupViewModel_OnTaskItemsCollectionChanged_Hook(void* pThis,
                                                         void* args) {
    Wh_Log(L">");

    TaskListGroupViewModel_OnTaskItemsCollectionChanged_Original(pThis, args);

    OnTaskGroupWindowsChanged(pThis);
}

// Hooked instead of OnTaskItemsCollectionChanged on builds where it's inlined
// into the event handler of the taskbar controller, which then calls these
// itself.
using TaskListGroupViewModel_OnWindowAdded_t =
    void(WINAPI*)(void* pThis, unsigned int index);
TaskListGroupViewModel_OnWindowAdded_t
    TaskListGroupViewModel_OnWindowAdded_Original;
void WINAPI TaskListGroupViewModel_OnWindowAdded_Hook(void* pThis,
                                                      unsigned int index) {
    Wh_Log(L">");

    TaskListGroupViewModel_OnWindowAdded_Original(pThis, index);

    OnTaskGroupWindowsChanged(pThis);
}

using TaskListGroupViewModel_OnWindowRemoved_t =
    void(WINAPI*)(void* pThis, unsigned int index);
TaskListGroupViewModel_OnWindowRemoved_t
    TaskListGroupViewModel_OnWindowRemoved_Original;
void WINAPI TaskListGroupViewModel_OnWindowRemoved_Hook(void* pThis,
                                                        unsigned int index) {
    Wh_Log(L">");

    TaskListGroupViewModel_OnWindowRemoved_Original(pThis, index);

    OnTaskGroupWindowsChanged(pThis);
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
        lpValue &&
        (_wcsicmp(lpValue, L"TaskbarGlomLevel") == 0 ||
         _wcsicmp(lpValue, L"MMTaskbarGlomLevel") == 0) &&
        dwFlags == RRF_RT_REG_DWORD && pvData && pcbData &&
        *pcbData == sizeof(DWORD)) {
        Wh_Log(L">");

        DWORD taskbarGlomLevel = (ret == ERROR_SUCCESS) ? *(DWORD*)pvData : 0;
        DWORD taskbarGlomLevelOriginal = taskbarGlomLevel;

        if (!g_unloading) {
            // 0 - Always
            // 1 - When taskbar is full
            // 2 - Never
            if (g_settings.mode == Mode::noLabelsWithCombining ||
                g_settings.mode == Mode::labelsWithCombining) {
                taskbarGlomLevel = 0;
            } else if (taskbarGlomLevel == 0) {
                taskbarGlomLevel = 2;
            }
        }

        if (g_overrideGroupingMode) {
            if (taskbarGlomLevel == 0) {
                taskbarGlomLevel = 2;
            } else {
                taskbarGlomLevel = 0;
            }
        }

        Wh_Log(L"Overriding TaskbarGlomLevel: %u->%u", taskbarGlomLevelOriginal,
               taskbarGlomLevel);
        *(DWORD*)pvData = taskbarGlomLevel;

        if (pdwType) {
            *pdwType = REG_DWORD;
        }

        ret = ERROR_SUCCESS;
    }

    return ret;
}

void* wil_Feature_GetImpl_Original;

using WilFeatureTraits_Feature_29785186_IsEnabled_t =
    bool(WINAPI*)(void* pThis);
WilFeatureTraits_Feature_29785186_IsEnabled_t
    WilFeatureTraits_Feature_29785186_IsEnabled_Original;

void LoadSettings() {
    PCWSTR mode = Wh_GetStringSetting(L"mode");
    g_settings.mode = Mode::labelsWithoutCombining;
    if (wcscmp(mode, L"noLabelsWithCombining") == 0) {
        g_settings.mode = Mode::noLabelsWithCombining;
    } else if (wcscmp(mode, L"labelsWithCombining") == 0) {
        g_settings.mode = Mode::labelsWithCombining;
    } else if (wcscmp(mode, L"noLabelsWithoutCombining") == 0) {
        g_settings.mode = Mode::noLabelsWithoutCombining;
    }
    Wh_FreeStringSetting(mode);

    g_settings.taskbarItemWidth = Wh_GetIntSetting(L"taskbarItemWidth");

    // For compatibility - in previous versions, width -1 was documented to hide
    // labels.
    if (g_settings.taskbarItemWidth == -1) {
        switch (g_settings.mode) {
            case Mode::labelsWithoutCombining:
                g_settings.mode = Mode::noLabelsWithoutCombining;
                break;

            case Mode::labelsWithCombining:
                g_settings.mode = Mode::noLabelsWithCombining;
                break;

            case Mode::noLabelsWithCombining:
            case Mode::noLabelsWithoutCombining:
                break;
        }

        g_settings.taskbarItemWidth = 160;
    }

    PCWSTR runningIndicatorStyle =
        Wh_GetStringSetting(L"runningIndicatorStyle");
    g_settings.runningIndicatorStyle = IndicatorStyle::centerFixed;
    if (wcscmp(runningIndicatorStyle, L"centerDynamic") == 0) {
        g_settings.runningIndicatorStyle = IndicatorStyle::centerDynamic;
    } else if (wcscmp(runningIndicatorStyle, L"left") == 0) {
        g_settings.runningIndicatorStyle = IndicatorStyle::left;
    } else if (wcscmp(runningIndicatorStyle, L"fullWidth") == 0) {
        g_settings.runningIndicatorStyle = IndicatorStyle::fullWidth;
    }
    Wh_FreeStringSetting(runningIndicatorStyle);

    PCWSTR progressIndicatorStyle =
        Wh_GetStringSetting(L"progressIndicatorStyle");
    g_settings.progressIndicatorStyle = g_settings.runningIndicatorStyle;
    if (wcscmp(progressIndicatorStyle, L"centerDynamic") == 0) {
        g_settings.progressIndicatorStyle = IndicatorStyle::centerDynamic;
    } else if (wcscmp(progressIndicatorStyle, L"fullWidth") == 0) {
        g_settings.progressIndicatorStyle = IndicatorStyle::fullWidth;
    }
    Wh_FreeStringSetting(progressIndicatorStyle);

    g_settings.excludedPrograms.clear();

    for (int i = 0;; i++) {
        PCWSTR program = Wh_GetStringSetting(L"excludedPrograms[%d]", i);

        bool hasProgram = *program;
        if (hasProgram) {
            std::wstring programUpper = program;
            LCMapStringEx(
                LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, &programUpper[0],
                static_cast<int>(programUpper.length()), &programUpper[0],
                static_cast<int>(programUpper.length()), nullptr, nullptr, 0);

            g_settings.excludedPrograms.insert(std::move(programUpper));
        }

        Wh_FreeStringSetting(program);

        if (!hasProgram) {
            break;
        }
    }

    g_settings.minimumTaskbarItemWidth =
        Wh_GetIntSetting(L"minimumTaskbarItemWidth");
    g_settings.maximumTaskbarItemWidth =
        Wh_GetIntSetting(L"maximumTaskbarItemWidth");

    g_settings.fontSize = Wh_GetIntSetting(L"fontSize");
    if (g_settings.fontSize < 1) {
        g_settings.fontSize = 1;
    }

    g_settings.fontFamily = WindhawkUtils::StringSetting::make(L"fontFamily");

    PCWSTR textTrimming = Wh_GetStringSetting(L"textTrimming");
    g_settings.textTrimming = TextTrimming::CharacterEllipsis;
    if (wcscmp(textTrimming, L"wordEllipsis") == 0) {
        g_settings.textTrimming = TextTrimming::WordEllipsis;
    } else if (wcscmp(textTrimming, L"clip") == 0) {
        g_settings.textTrimming = TextTrimming::Clip;
    }
    Wh_FreeStringSetting(textTrimming);

    g_settings.leftAndRightPaddingSize =
        Wh_GetIntSetting(L"leftAndRightPaddingSize");
    g_settings.spaceBetweenIconAndLabel =
        Wh_GetIntSetting(L"spaceBetweenIconAndLabel");
    g_settings.runningIndicatorHeight =
        Wh_GetIntSetting(L"runningIndicatorHeight");
    g_settings.runningIndicatorVerticalOffset =
        Wh_GetIntSetting(L"runningIndicatorVerticalOffset");
    g_settings.alwaysShowThumbnailLabels =
        Wh_GetIntSetting(L"alwaysShowThumbnailLabels");
    g_settings.labelForSingleItem =
        WindhawkUtils::StringSetting::make(L"labelForSingleItem");
    g_settings.labelForMultipleItems =
        WindhawkUtils::StringSetting::make(L"labelForMultipleItems");
}

void ApplySettings() {
    if (!g_hasNativeLabelsImplementation) {
        RecalculateLabels();
    } else {
        HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
        if (!hTaskbarWnd) {
            return;
        }

        // Trigger TrayUI::_HandleSettingChange.
        g_overrideGroupingMode = true;
        SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, 0, 0);
        g_overrideGroupingMode = false;

        Sleep(400);

        SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, 0, 0);
    }
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] =  //
        {
            {
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Taskbar::ITaskListButton>::get_IsRunning(bool *))"},
                &TaskListButton_get_IsRunning_Original,
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
                &TaskListButton_UpdateVisualStates_Original,
                TaskListButton_UpdateVisualStates_Hook,
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))"},
                &TaskListButton_UpdateButtonPadding_Original,
                TaskListButton_UpdateButtonPadding_Hook,
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateBadgeSize(void))"},
                &TaskListButton_UpdateBadgeSize_Original,
                TaskListButton_UpdateBadgeSize_Hook,
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::OnTaskbarLayoutChildBoundsChanged(void))"},
                &TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original,
                TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Hook,
            },
            {
                {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::Icon(struct winrt::Windows::Storage::Streams::IRandomAccessStream))"},
                &TaskListButton_Icon_Original,
                TaskListButton_Icon_Hook,
            },
            {
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Taskbar::ITaskbarButton>::get_MinScalableWidth(float *))"},
                &ITaskbarButton_get_MinScalableWidth_Original,
                ITaskbarButton_get_MinScalableWidth_Hook,
                true,
            },
            {
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarCollapsibleLayout,struct winrt::Microsoft::UI::Xaml::Controls::IVirtualizingLayoutOverrides>::ArrangeOverride(void *,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
                &TaskbarCollapsibleLayout_ArrangeOverride_Original,
                TaskbarCollapsibleLayout_ArrangeOverride_Hook,
                true,  // From 10.0.22621.2361.
            },
            {
                {LR"(public: static void __cdecl TaskbarTelemetry::FrameSizeChanged<double const &>(double const &))"},
                &TaskbarTelemetry_FrameSizeChanged_Original,
                TaskbarTelemetry_FrameSizeChanged_Hook,
                true,  // Only needed for the vertical taskbar.
            },
            {
                {LR"(public: struct winrt::Windows::UI::Xaml::FrameworkElement __cdecl winrt::Taskbar::implementation::TaskbarController::View(void))"},
                &TaskbarController_View_Original,
                nullptr,
                true,  // Only needed for the vertical taskbar.
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::UpdateFrameHeight(void))"},
                &TaskbarController_UpdateFrameHeight_Original,
                TaskbarController_UpdateFrameHeight_Hook,
                true,  // Only needed for the vertical taskbar.
            },
            {
                {LR"(public: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::TaskbarItemGroupingMode(enum winrt::WindowsUdk::UI::Shell::TaskbarItemGroupingMode))"},
                &ExperienceToggleButton_TaskbarItemGroupingMode_Original,
                ExperienceToggleButton_TaskbarItemGroupingMode_Hook,
                true,  // Only needed for the vertical taskbar.
            },
            {
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::AugmentedEntryPointViewModel,struct winrt::Taskbar::IAugmentedEntryPointViewModel>::put_TaskbarItemGroupingMode(int))"},
                &AugmentedEntryPointViewModel_put_TaskbarItemGroupingMode_Original,
                AugmentedEntryPointViewModel_put_TaskbarItemGroupingMode_Hook,
                true,  // Only needed for the vertical taskbar.
            },
            {
                {LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::`vftable')"},
                &ITaskListWindowViewModel_vftable,
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
                nullptr,
                true,  // From 10.0.22621.2361.
            },
            {
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::get_HasLabel(bool *))"},
                &TaskListGroupViewModel_ITaskbarAppItemViewModel_get_HasLabel_Original,
                nullptr,
                true,  // From 10.0.22621.2361.
            },
            {
                {LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::`vftable')"},
                &TaskListWindowViewModel_ITaskbarAppItemViewModel_vftable,
                nullptr,
                true,  // From 10.0.22621.2361.
            },
            {
                {LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::`vftable')"},
                &TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable,
                nullptr,
                true,  // From 10.0.22621.2361.
            },
            {
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::get_TaskGroup(void * *))"},
                &TaskListGroupViewModel_ITaskbarAppItemViewModel_get_TaskGroup,
                nullptr,
                true,  // From 10.0.22621.2361.
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListGroupViewModel::OnPropertyChanged(class std::basic_string_view<wchar_t,struct std::char_traits<wchar_t> > const &))"},
                &TaskListGroupViewModel_OnPropertyChanged,
                nullptr,
                true,  // From 10.0.22621.2361.
            },
            {
                {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListGroupViewModel::OnTaskItemsCollectionChanged(struct winrt::WindowsUdk::UI::Shell::TaskItemCollectionChangedEventArgs const &))"},
                &TaskListGroupViewModel_OnTaskItemsCollectionChanged_Original,
                TaskListGroupViewModel_OnTaskItemsCollectionChanged_Hook,
                true,  // From 10.0.22621.2361, inlined in newer builds.
            },
            {
                {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListGroupViewModel::OnWindowAdded(unsigned int))"},
                &TaskListGroupViewModel_OnWindowAdded_Original,
                nullptr,  // Hooked below if needed.
                true,
            },
            {
                {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListGroupViewModel::OnWindowRemoved(unsigned int))"},
                &TaskListGroupViewModel_OnWindowRemoved_Original,
                nullptr,  // Hooked below if needed.
                true,
            },
            {
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarItemViewModel>::get_DisplayName(void * *))"},
                &TaskListGroupViewModel_ITaskbarItemViewModel_get_DisplayName_Original,
                TaskListGroupViewModel_ITaskbarItemViewModel_get_DisplayName_Hook,
                true,  // From 10.0.22621.2361.
            },
            {
                {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListGroupViewModel::OnTaskItemPropertyChanged(struct winrt::WindowsUdk::UI::Shell::TaskItemPropertyChangedEventArgs const &))"},
                &TaskListGroupViewModel_OnTaskItemPropertyChanged_Original,
                TaskListGroupViewModel_OnTaskItemPropertyChanged_Hook,
                true,  // From 10.0.22621.2361.
            },
            {
                {
                    LR"(class wil::details::FeatureImpl<struct __WilExternalFeatureTraits_Feature_29785186> `private: static class wil::details::FeatureImpl<struct __WilExternalFeatureTraits_Feature_29785186> & __cdecl wil::Feature<struct __WilExternalFeatureTraits_Feature_29785186>::GetImpl(void)'::`2'::impl)",

                    // Symbol before update KB5036980:
                    LR"(class wil::details::FeatureImpl<struct __WilFeatureTraits_Feature_29785186> `private: static class wil::details::FeatureImpl<struct __WilFeatureTraits_Feature_29785186> & __cdecl wil::Feature<struct __WilFeatureTraits_Feature_29785186>::GetImpl(void)'::`2'::impl)",
                },
                &wil_Feature_GetImpl_Original,
                nullptr,
                true,
            },
            {
                {
                    LR"(public: bool __cdecl wil::details::FeatureImpl<struct __WilExternalFeatureTraits_Feature_29785186>::__private_IsEnabled(enum wil::ReportingKind))",

                    // Symbols before update KB5036980:
                    LR"(public: bool __cdecl wil::details::FeatureImpl<struct __WilFeatureTraits_Feature_29785186>::__private_IsEnabled(enum wil::ReportingKind))",
                },
                &WilFeatureTraits_Feature_29785186_IsEnabled_Original,
                nullptr,
                true,
            },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    // Where OnTaskItemsCollectionChanged isn't inlined, it calls these, and
    // some builds inline one of them into it instead.
    if (!TaskListGroupViewModel_OnTaskItemsCollectionChanged_Original) {
        if (TaskListGroupViewModel_OnWindowAdded_Original) {
            WindhawkUtils::SetFunctionHook(
                TaskListGroupViewModel_OnWindowAdded_Original,
                TaskListGroupViewModel_OnWindowAdded_Hook,
                &TaskListGroupViewModel_OnWindowAdded_Original);
        }

        if (TaskListGroupViewModel_OnWindowRemoved_Original) {
            WindhawkUtils::SetFunctionHook(
                TaskListGroupViewModel_OnWindowRemoved_Original,
                TaskListGroupViewModel_OnWindowRemoved_Hook,
                &TaskListGroupViewModel_OnWindowRemoved_Original);
        }
    }

    return true;
}

bool HookSearchUxUiDllSymbols(HMODULE module) {
    // SearchUx.UI.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SearchUx::SearchUI::implementation::SearchButtonBase,struct winrt::SearchUx::SearchUI::ISearchButtonBase>::put_TaskbarItemGroupingMode(int))"},
            &SearchButtonBase_put_TaskbarItemGroupingMode_Original,
            SearchButtonBase_put_TaskbarItemGroupingMode_Hook,
            true,  // Only needed for the vertical taskbar.
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool HookSystemTrayDllSymbols(HMODULE module) {
    // SystemTray.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskbarSettings5<struct winrt::WindowsUdk::UI::Shell::TaskbarSettings>::GroupingMode(void)const )"},
            &TaskbarSettings_GroupingMode_Original,
            TaskbarSettings_GroupingMode_Hook,
            true,  // Only needed for the vertical taskbar.
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

HMODULE GetSearchUxUiModuleHandle() {
    return GetModuleHandle(L"SearchUx.UI.dll");
}

// Older builds have the tray in Taskbar.View.dll, where the taskbar reads its
// own grouping mode with the same function, so it's not hooked there.
HMODULE GetSystemTrayModuleHandle() {
    return GetModuleHandle(L"SystemTray.dll");
}

BOOL ModInitWithTaskbarView(HMODULE taskbarViewModule);

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (ModInitWithTaskbarView(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

void HandleLoadedModuleIfSearchUxUi(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_searchUxUiDllLoaded && GetSearchUxUiModuleHandle() == module &&
        !g_searchUxUiDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookSearchUxUiDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

void HandleLoadedModuleIfSystemTray(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_systemTrayDllLoaded && GetSystemTrayModuleHandle() == module &&
        !g_systemTrayDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookSystemTrayDllSymbols(module)) {
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
        HandleLoadedModuleIfSearchUxUi(module, lpLibFileName);
        HandleLoadedModuleIfSystemTray(module, lpLibFileName);
    }

    return module;
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
            {LR"(public: virtual int __cdecl CTaskListThumbnailWnd::DisplayUI(struct ITaskBtnGroup *,struct ITaskItem *,struct ITaskItem *,unsigned long))"},
            &CTaskListThumbnailWnd_DisplayUI_Original,
            CTaskListThumbnailWnd_DisplayUI_Hook,
            true,  // Classic thumbnails, removed in or near 10.0.26100.8491.
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool HookTaskbarDllSymbolsOldImplementation() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(protected: struct ITaskBtnGroup * __cdecl CTaskListWnd::_GetTBGroupFromGroup(struct ITaskGroup *,int *))"},
            &CTaskListWnd__GetTBGroupFromGroup_Original,
        },
        {
            {LR"(public: virtual int __cdecl CTaskBtnGroup::GetNumItems(void))"},
            &CTaskBtnGroup_GetNumItems_Original,
        },
        {
            {LR"(public: virtual struct ITaskItem * __cdecl CTaskBtnGroup::GetTaskItem(int))"},
            &CTaskBtnGroup_GetTaskItem_Original,
        },
        {
            {LR"(public: virtual long __cdecl CTaskGroup::GetTitleText(struct ITaskItem *,unsigned short *,int))"},
            &CTaskGroup_GetTitleText_Original,
        },
        {
            {LR"(public: virtual bool __cdecl IconContainer::IsStorageRecreationRequired(class CCoSimpleArray<unsigned int,4294967294,class CSimpleArrayStandardCompareHelper<unsigned int> > const &,enum IconContainerFlags))"},
            &IconContainer_IsStorageRecreationRequired_Original,
            IconContainer_IsStorageRecreationRequired_Hook,
        },
        {
            {LR"(public: virtual void __cdecl CTaskListWnd::GroupChanged(struct ITaskGroup *,enum winrt::WindowsUdk::UI::Shell::TaskGroupProperty))"},
            &CTaskListWnd_GroupChanged_Original,
            CTaskListWnd_GroupChanged_Hook,
        },
        {
            // An older variant, see the newer variant below.
            {LR"(public: virtual long __cdecl CTaskListWnd::TaskDestroyed(struct ITaskGroup *,struct ITaskItem *,enum TaskDestroyedFlags))"},
            &CTaskListWnd_TaskDestroyed_Original,
            CTaskListWnd_TaskDestroyed_Hook,
            true,
        },
        {
            // A newer variant seen in insider builds.
            {LR"(public: virtual long __cdecl CTaskListWnd::TaskDestroyed(struct ITaskGroup *,struct ITaskItem *))"},
            &CTaskListWnd_TaskDestroyed_2_Original,
            CTaskListWnd_TaskDestroyed_2_Hook,
            true,
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

BOOL ModInitWithTaskbarView(HMODULE taskbarViewModule) {
    if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
        return FALSE;
    }

    if (wil_Feature_GetImpl_Original &&
        WilFeatureTraits_Feature_29785186_IsEnabled_Original) {
        g_hasNativeLabelsImplementation =
            WilFeatureTraits_Feature_29785186_IsEnabled_Original(
                wil_Feature_GetImpl_Original);
    } else {
        g_hasNativeLabelsImplementation =
            !!TaskListWindowViewModel_ITaskbarAppItemViewModel_get_HasLabel_Original;
    }

    if (!g_hasNativeLabelsImplementation) {
        if (!HookTaskbarDllSymbolsOldImplementation()) {
            return FALSE;
        }
    } else {
        if (!HookTaskbarDllSymbols()) {
            return FALSE;
        }

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseRegGetValueW = (decltype(&RegGetValueW))GetProcAddress(
            kernelBaseModule, "RegGetValueW");
        WindhawkUtils::SetFunctionHook(
            pKernelBaseRegGetValueW, RegGetValueW_Hook, &RegGetValueW_Original);
    }

    // Last, so that a failed init doesn't leave the slots pointing to the
    // hooks.
    HookHasLabelVtableSlots();

    return TRUE;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    bool delayLoadingNeeded = false;

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!ModInitWithTaskbarView(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
        delayLoadingNeeded = true;
    }

    if (HMODULE searchUxUiModule = GetSearchUxUiModuleHandle()) {
        g_searchUxUiDllLoaded = true;
        HookSearchUxUiDllSymbols(searchUxUiModule);
    } else {
        Wh_Log(L"Search UX UI module not loaded yet");
        delayLoadingNeeded = true;
    }

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        g_systemTrayDllLoaded = true;
        HookSystemTrayDllSymbols(systemTrayModule);
    } else {
        Wh_Log(L"System tray module not loaded yet");
        delayLoadingNeeded = true;
    }

    if (delayLoadingNeeded) {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                if (ModInitWithTaskbarView(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    if (!g_searchUxUiDllLoaded) {
        if (HMODULE searchUxUiModule = GetSearchUxUiModuleHandle()) {
            if (!g_searchUxUiDllLoaded.exchange(true)) {
                Wh_Log(L"Got SearchUx.UI.dll");

                if (HookSearchUxUiDllSymbols(searchUxUiModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    if (!g_systemTrayDllLoaded) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayDllLoaded.exchange(true)) {
                Wh_Log(L"Got SystemTray.dll");

                if (HookSystemTrayDllSymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    if (g_taskbarViewDllLoaded) {
        ApplySettings();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    // Before the unload, to let calls which already read the slots return.
    UnhookHasLabelVtableSlots();

    if (g_taskbarViewDllLoaded) {
        ApplySettings();

        // This is required to give time for taskbar buttons of UWP apps to
        // update the layout.
        Sleep(400);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    // The hooks read the settings on the taskbar thread.
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd ||
        !RunFromWindowThread(
            hTaskbarWnd, [](PVOID) { LoadSettings(); }, nullptr)) {
        LoadSettings();
    }

    if (g_taskbarViewDllLoaded) {
        ApplySettings();
    }
}
