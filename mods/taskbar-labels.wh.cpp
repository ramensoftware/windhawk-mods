// ==WindhawkMod==
// @id              taskbar-labels
// @name            Taskbar Labels for Windows 11
// @description     Customize text labels and combining for running programs on the taskbar (Windows 11 only)
// @version         1.3.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWINVER=0x0A00 -loleaut32 -lole32 -lruntimeobject -lwininet
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
- excludedPrograms: [excluded1.exe]
  $name: Excluded programs
  $description: >-
    If the "Show labels, don't combine taskbar buttons" mode is used, labels
    won't be shown for these programs

    If the "Hide labels, don't combine taskbar buttons" mode is used, labels
    will be shown for these programs

    If another mode is used, this list is ignored
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
- leftAndRightPaddingSize: 8
  $name: Left and right padding size
- spaceBetweenIconAndLabel: 8
  $name: Space between icon and label
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

#undef GetCurrentTime

#include <initguid.h>  // must come before knownfolders.h

#include <inspectable.h>
#include <knownfolders.h>
#include <shlobj.h>
#include <wininet.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/base.h>

#include <algorithm>
#include <atomic>
#include <limits>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

// https://stackoverflow.com/a/51274008
template <auto fn>
struct deleter_from_fn {
    template <typename T>
    constexpr void operator()(T* arg) const {
        fn(arg);
    }
};
using string_setting_unique_ptr =
    std::unique_ptr<const WCHAR[], deleter_from_fn<Wh_FreeStringSetting>>;

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
    int leftAndRightPaddingSize;
    int spaceBetweenIconAndLabel;
    string_setting_unique_ptr labelForSingleItem;
    string_setting_unique_ptr labelForMultipleItems;
} g_settings;

WCHAR g_taskbarViewDllPath[MAX_PATH];
std::atomic<bool> g_taskbarViewDllLoaded = false;
std::atomic<bool> g_applyingSettings = false;
std::atomic<bool> g_overrideGroupingMode = false;
std::atomic<bool> g_unloading = false;

bool g_hasNativeLabelsImplementation;

double g_initialTaskbarItemWidth;

UINT_PTR g_invalidateTaskListButtonTimer;
std::unordered_set<FrameworkElement> g_taskListButtonsWithLabelMissing;

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

HWND GetTaskbarWnd() {
    static HWND hTaskbarWnd;

    if (!hTaskbarWnd) {
        HWND hWnd = FindWindow(L"Shell_TrayWnd", nullptr);

        DWORD processId = 0;
        if (hWnd && GetWindowThreadProcessId(hWnd, &processId) &&
            processId == GetCurrentProcessId()) {
            hTaskbarWnd = hWnd;
        }
    }

    return hTaskbarWnd;
}

void RecalculateLabels() {
    HWND hTaskbarWnd = GetTaskbarWnd();
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

// {0BD894F2-EDFC-5DDF-A166-2DB14BBFDF35}
constexpr winrt::guid IItemsRepeater{
    0x0BD894F2,
    0xEDFC,
    0x5DDF,
    {0xA1, 0x66, 0x2D, 0xB1, 0x4B, 0xBF, 0xDF, 0x35}};

int ItemsRepeater_GetElementIndex(FrameworkElement taskbarFrameRepeaterElement,
                                  UIElement element) {
    winrt::Windows::Foundation::IUnknown pThis = nullptr;
    taskbarFrameRepeaterElement.as(IItemsRepeater, winrt::put_abi(pThis));

    using GetElementIndex_t =
        HRESULT(WINAPI*)(void* pThis, void* element, void* index);

    void** vtable = *(void***)winrt::get_abi(pThis);
    auto GetElementIndex = (GetElementIndex_t)vtable[19];

    int index = -1;
    GetElementIndex(winrt::get_abi(pThis), winrt::get_abi(element), &index);

    return index;
}

FrameworkElement ItemsRepeater_TryGetElement(
    FrameworkElement taskbarFrameRepeaterElement,
    int index) {
    winrt::Windows::Foundation::IUnknown pThis = nullptr;
    taskbarFrameRepeaterElement.as(IItemsRepeater, winrt::put_abi(pThis));

    using TryGetElement_t =
        HRESULT(WINAPI*)(void* pThis, int index, void** uiElement);

    void** vtable = *(void***)winrt::get_abi(pThis);
    auto TryGetElement = (TryGetElement_t)vtable[20];

    void* uiElement = nullptr;
    TryGetElement(winrt::get_abi(pThis), index, &uiElement);

    return UIElement{uiElement, winrt::take_ownership_from_abi}
        .try_as<FrameworkElement>();
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
        HWND hTaskbarWnd = GetTaskbarWnd();
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

    bool hasOverflowButton = false;
    int taskListRunningButtonsCount = 0;
    double otherElementsWidth = 0;

    for (auto panelChild :
         taskbarFrameRepeaterElement.as<Controls::Panel>().Children()) {
        int index = ItemsRepeater_GetElementIndex(taskbarFrameRepeaterElement,
                                                  panelChild);
        if (index < 0) {
            continue;
        }

        auto child =
            ItemsRepeater_TryGetElement(taskbarFrameRepeaterElement, index);
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

void UpdateTaskListButtonWithLabelStyle(
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

    double taskListButtonWidth = taskListButtonElement.ActualWidth();
    double iconWidth = iconElement.ActualWidth();

    auto columnDefinitions =
        iconPanelElement.as<Controls::Grid>().ColumnDefinitions();

    auto firstColumnWidth = columnDefinitions.GetAt(0).Width();
    auto firstColumnWidthPixels =
        firstColumnWidth.GridUnitType == GridUnitType::Pixel
            ? firstColumnWidth.Value
            : 0.0;

    double secondColumnWidthPixels =
        g_unloading ? 0 : g_settings.taskbarItemWidth;
    if (secondColumnWidthPixels > 0) {
        secondColumnWidthPixels -= firstColumnWidthPixels;
        if (secondColumnWidthPixels < 1) {
            secondColumnWidthPixels = 1;
        }
    }

    auto labelControlElement =
        FindChildByName(iconPanelElement, L"LabelControl")
            .as<Controls::TextBlock>();

    if (secondColumnWidthPixels > 0 && labelControlElement) {
        // In labelsWithCombining mode, pinned items have labels too. Hide them.
        if (g_settings.mode == Mode::labelsWithCombining &&
            !TaskListButton_IsRunning(taskListButtonElement)) {
            secondColumnWidthPixels = 0;
            labelControlElement.Visibility(Visibility::Collapsed);
            labelControlElement = nullptr;
        }

        columnDefinitions.GetAt(1).Width(GridLength({
            .Value = secondColumnWidthPixels,
            .GridUnitType = GridUnitType::Pixel,
        }));
    } else {
        columnDefinitions.GetAt(1).Width(GridLength({
            .Value = 1,
            .GridUnitType = GridUnitType::Auto,
        }));
    }

    if (labelControlElement) {
        labelControlElement.Visibility(Visibility::Visible);

        auto horizontalAlignment = g_unloading ? HorizontalAlignment::Center
                                               : HorizontalAlignment::Left;
        if (labelControlElement.HorizontalAlignment() != horizontalAlignment) {
            labelControlElement.HorizontalAlignment(horizontalAlignment);
        }

        if (g_unloading) {
            labelControlElement.MaxWidth(
                std::max(0.0, 176 - firstColumnWidthPixels));
        } else if (g_settings.taskbarItemWidth == 0) {
            labelControlElement.MaxWidth(std::max(
                0.0,
                g_settings.maximumTaskbarItemWidth - firstColumnWidthPixels));
        } else {
            labelControlElement.MaxWidth(
                std::numeric_limits<double>::infinity());
        }

        auto textTrimming =
            g_unloading ? TextTrimming::Clip : TextTrimming::CharacterEllipsis;
        if (labelControlElement.TextTrimming() != textTrimming) {
            labelControlElement.TextTrimming(textTrimming);
        }

        auto labelControlMargin = labelControlElement.Margin();
        labelControlMargin.Left =
            g_unloading ? 0
                        : (iconWidth - 24 + g_settings.leftAndRightPaddingSize -
                           8 + g_settings.spaceBetweenIconAndLabel - 8);
        labelControlMargin.Right =
            g_unloading ? 0 : (g_settings.leftAndRightPaddingSize - 10);
        labelControlElement.Margin(labelControlMargin);

        double fontSize = g_unloading ? 12 : g_settings.fontSize;
        if (labelControlElement.FontSize() != fontSize) {
            labelControlElement.FontSize(fontSize);
        }
    }

    iconElement.HorizontalAlignment((g_unloading || !labelControlElement)
                                        ? HorizontalAlignment::Stretch
                                        : HorizontalAlignment::Left);

    auto iconMargin = iconElement.Margin();
    iconMargin.Left = (g_unloading || !labelControlElement)
                          ? 0.0
                          : g_settings.leftAndRightPaddingSize;
    iconMargin.Right = 0;
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
                             : 16 - g_settings.leftAndRightPaddingSize +
                                   (24 - iconWidth),
            });
        }
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

        IndicatorStyle indicatorStyle =
            g_unloading
                ? IndicatorStyle::left
                : (isProgressIndicator ? g_settings.progressIndicatorStyle
                                       : g_settings.runningIndicatorStyle);

        if (indicatorStyle == IndicatorStyle::left) {
            indicatorElement.SetValue(Controls::Grid::ColumnSpanProperty(),
                                      winrt::box_value(1));
        } else {
            indicatorElement.SetValue(Controls::Grid::ColumnSpanProperty(),
                                      winrt::box_value(2));
        }

        double minWidth = 0;

        if (indicatorStyle == IndicatorStyle::centerFixed) {
            // Without this, the indicator isn't centered.
            minWidth = indicatorElement.Width();
        } else if (indicatorStyle == IndicatorStyle::centerDynamic) {
            if (firstColumnWidthPixels > 0) {
                minWidth = indicatorElement.Width() * taskListButtonWidth /
                           firstColumnWidthPixels;
            }
        } else if (indicatorStyle == IndicatorStyle::fullWidth) {
            minWidth = taskListButtonWidth - 6;
            if (minWidth < 0) {
                minWidth = 0;
            }
        }

        // High values of maximumTaskbarItemWidth together with a fullWidth
        // indicator can crash the process due to a refresh loop. Use this as a
        // workaround.
        if (g_settings.taskbarItemWidth == 0 &&
            indicatorStyle == IndicatorStyle::fullWidth) {
            double currentMinWidth = indicatorElement.MinWidth();
            if (minWidth != currentMinWidth) {
                indicatorElement.MinWidth(0);
                indicatorElement.Dispatcher().TryRunAsync(
                    winrt::Windows::UI::Core::CoreDispatcherPriority::High,
                    [indicatorElement, minWidth]() {
                        indicatorElement.MinWidth(minWidth);
                    });
            }
        } else {
            indicatorElement.MinWidth(minWidth);
        }

        auto indicatorMargin = indicatorElement.Margin();
        indicatorMargin.Left = 0;
        indicatorMargin.Right = 0;
        auto indicatorHorizontalAlignment = HorizontalAlignment::Stretch;
        if (!g_unloading && labelControlElement) {
            if (indicatorStyle == IndicatorStyle::left) {
                indicatorMargin.Left =
                    iconWidth - 24 +
                    (g_settings.leftAndRightPaddingSize - 8) * 2;
            } else {
                indicatorMargin.Left = (taskListButtonWidth - minWidth) / 2 - 2;
                indicatorHorizontalAlignment = HorizontalAlignment::Left;
            }
        }
        indicatorElement.Margin(indicatorMargin);
        indicatorElement.HorizontalAlignment(indicatorHorizontalAlignment);

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
    auto columnDefinitions =
        iconPanelElement.as<Controls::Grid>().ColumnDefinitions();
    if (columnDefinitions.Size() == 2) {
        UpdateTaskListButtonWithLabelStyle(taskListButtonElement);
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
        std::min(g_initialTaskbarItemWidth,
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
        g_taskListButtonsWithLabelMissing.insert(windhawkTextControl);

        g_invalidateTaskListButtonTimer =
            SetTimer(nullptr, g_invalidateTaskListButtonTimer, 200,
                     [](HWND hwnd,  // handle of window for timer messages
                        UINT uMsg,  // WM_TIMER message
                        UINT_PTR idEvent,  // timer identifier
                        DWORD dwTime       // current system time
                        ) WINAPI {
                         KillTimer(nullptr, g_invalidateTaskListButtonTimer);
                         g_invalidateTaskListButtonTimer = 0;

                         if (!g_taskListButtonsWithLabelMissing.empty()) {
                             g_taskListButtonsWithLabelMissing.clear();
                             RecalculateLabels();
                         }
                     });
    } else if (!textLabelMissing && g_invalidateTaskListButtonTimer) {
        g_taskListButtonsWithLabelMissing.erase(windhawkTextControl);
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

    for (int i = 0;; i++) {
        auto child =
            ItemsRepeater_TryGetElement(taskbarFrameRepeaterElement, i);
        if (!child) {
            break;
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

using TaskbarSettings_GroupingMode_t = DWORD(WINAPI*)(void* pThis);
TaskbarSettings_GroupingMode_t TaskbarSettings_GroupingMode_Original;
DWORD WINAPI TaskbarSettings_GroupingMode_Hook(void* pThis) {
    Wh_Log(L">");

    DWORD ret = TaskbarSettings_GroupingMode_Original(pThis);

    if (!g_unloading) {
        // 0 - Always
        // 1 - When taskbar is full
        // 2 - Never
        if (g_settings.mode == Mode::noLabelsWithCombining ||
            g_settings.mode == Mode::labelsWithCombining) {
            ret = 0;
        } else if (ret == 0) {
            ret = 2;
        }
    }

    if (g_overrideGroupingMode) {
        if (ret == 0) {
            ret = 2;
        } else {
            ret = 0;
        }
    }

    return ret;
}

using TaskListButton_MinScalableWidth_t = float(WINAPI*)(void* pThis);
TaskListButton_MinScalableWidth_t TaskListButton_MinScalableWidth_Original;
// RDX is expected to be preserved.
[[clang::preserve_most]]
float TaskListButton_MinScalableWidth_Hook(void* pThis) {
    Wh_Log(L">");

    float ret = TaskListButton_MinScalableWidth_Original(pThis);

    if (!g_unloading && g_hasNativeLabelsImplementation && ret > 0) {
        // Allow to create many taskbar items before overflow appears.
        int minimumTaskbarItemWidth = g_settings.minimumTaskbarItemWidth;
        if (minimumTaskbarItemWidth < 44) {
            minimumTaskbarItemWidth = 44;
            Wh_Log(L"minimumTaskbarItemWidth too small, using %d",
                   minimumTaskbarItemWidth);
        }

        if (ret >= minimumTaskbarItemWidth) {
            ret = minimumTaskbarItemWidth;
        } else {
            Wh_Log(L"minimumTaskbarItemWidth too large, using default (%f)",
                   ret);
        }
    }

    return ret;
}

bool g_inITaskbarAppItemViewModel_HasLabels;

using ITaskbarAppItemViewModel_HasLabels_t = bool(WINAPI*)(void* pThis);
ITaskbarAppItemViewModel_HasLabels_t
    ITaskbarAppItemViewModel_HasLabels_Original;
bool WINAPI ITaskbarAppItemViewModel_HasLabels_Hook(void* pThis) {
    Wh_Log(L">");

    g_inITaskbarAppItemViewModel_HasLabels = true;

    bool ret = ITaskbarAppItemViewModel_HasLabels_Original(pThis);

    g_inITaskbarAppItemViewModel_HasLabels = false;

    return ret;
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
    if (g_unloading || !g_inITaskbarAppItemViewModel_HasLabels || FAILED(ret) ||
        !*hasLabels) {
        return ret;
    }

    bool hideLabels = false;
    if (g_settings.mode == Mode::noLabelsWithoutCombining) {
        hideLabels = true;
    }

    if (!g_settings.excludedPrograms.empty() &&
        ITaskListWindowViewModel_vftable &&
        ITaskListWindowViewModel_get_TaskItem) {
        PVOID pITaskListWindowViewModel = pThis;
        while (*(PVOID*)pITaskListWindowViewModel !=
               ITaskListWindowViewModel_vftable) {
            pITaskListWindowViewModel = (PVOID*)pITaskListWindowViewModel - 1;
        }

        HWND hWnd = nullptr;

        winrt::com_ptr<IUnknown> taskItem;
        HRESULT hr = ITaskListWindowViewModel_get_TaskItem(
            pITaskListWindowViewModel, taskItem.put_void());
        if (SUCCEEDED(hr) && taskItem) {
            // public: virtual int __cdecl winrt::impl::produce<struct
            // winrt::WindowsUdk::UI::Shell::implementation::TaskItem, struct
            // winrt::WindowsUdk::UI::Shell::ITaskItem>::get_WindowId(unsigned
            // __int64 *)
            using ITaskItem_get_WindowId_t =
                HRESULT(WINAPI*)(void* pThis, HWND* hWnd);

            void** vtable = *(void***)taskItem.get();
            auto ITaskItem_get_WindowId = (ITaskItem_get_WindowId_t)vtable[8];

            hr = ITaskItem_get_WindowId(taskItem.get(), &hWnd);
        }

        if (SUCCEEDED(hr) && hWnd) {
            DWORD resolvedWindowProcessPathLen = 0;
            WCHAR resolvedWindowProcessPath[MAX_PATH];
            WCHAR resolvedWindowProcessPathUpper[MAX_PATH];

            DWORD dwProcessId = 0;
            if (GetWindowThreadProcessId(hWnd, &dwProcessId)) {
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
                                              FALSE, dwProcessId);
                if (hProcess) {
                    DWORD dwSize = ARRAYSIZE(resolvedWindowProcessPath);
                    if (QueryFullProcessImageName(
                            hProcess, 0, resolvedWindowProcessPath, &dwSize)) {
                        resolvedWindowProcessPathLen = dwSize;
                    }

                    CloseHandle(hProcess);
                }
            }

            if (resolvedWindowProcessPathLen > 0) {
                LCMapStringEx(
                    LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE,
                    resolvedWindowProcessPath, resolvedWindowProcessPathLen + 1,
                    resolvedWindowProcessPathUpper,
                    resolvedWindowProcessPathLen + 1, nullptr, nullptr, 0);
            } else {
                *resolvedWindowProcessPath = L'\0';
                *resolvedWindowProcessPathUpper = L'\0';
            }

            bool excluded = false;

            if (!excluded && resolvedWindowProcessPathLen > 0 &&
                g_settings.excludedPrograms.contains(
                    resolvedWindowProcessPathUpper)) {
                excluded = true;
            }

            if (!excluded) {
                if (PCWSTR programFileNameUpper =
                        wcsrchr(resolvedWindowProcessPathUpper, L'\\')) {
                    programFileNameUpper++;
                    if (*programFileNameUpper &&
                        g_settings.excludedPrograms.contains(
                            programFileNameUpper)) {
                        excluded = true;
                    }
                }
            }

            if (excluded) {
                Wh_Log(L"Excluding %s", resolvedWindowProcessPath);
                hideLabels = !hideLabels;
            }
        }
    }

    if (hideLabels) {
        *hasLabels = false;
    }

    return ret;
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
    if (g_unloading || !g_inITaskbarAppItemViewModel_HasLabels || FAILED(ret)) {
        return ret;
    }

    if (g_settings.mode == Mode::labelsWithCombining) {
        *hasLabels = true;
    } else if (g_settings.mode == Mode::noLabelsWithoutCombining) {
        *hasLabels = false;
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

    g_settings.leftAndRightPaddingSize =
        Wh_GetIntSetting(L"leftAndRightPaddingSize");
    g_settings.spaceBetweenIconAndLabel =
        Wh_GetIntSetting(L"spaceBetweenIconAndLabel");
    g_settings.labelForSingleItem.reset(
        Wh_GetStringSetting(L"labelForSingleItem"));
    g_settings.labelForMultipleItems.reset(
        Wh_GetStringSetting(L"labelForMultipleItems"));
}

void ApplySettings() {
    if (!g_hasNativeLabelsImplementation) {
        RecalculateLabels();
    } else {
        HWND hTaskbarWnd = GetTaskbarWnd();
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

struct SYMBOL_HOOK {
    std::vector<std::wstring_view> symbols;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;
};

bool HookSymbols(HMODULE module,
                 const SYMBOL_HOOK* symbolHooks,
                 size_t symbolHooksCount,
                 bool cacheOnly = false) {
    const WCHAR cacheVer = L'1';
    const WCHAR cacheSep = L'#';
    constexpr size_t cacheMaxSize = 10240;

    WCHAR moduleFilePath[MAX_PATH];
    if (!GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        Wh_Log(L"GetModuleFileName failed");
        return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        Wh_Log(L"GetModuleFileName returned unsupported path");
        return false;
    }

    moduleFileName++;

    WCHAR cacheBuffer[cacheMaxSize + 1];
    std::wstring cacheStrKey = std::wstring(L"symbol-cache-") + moduleFileName;
    Wh_GetStringValue(cacheStrKey.c_str(), cacheBuffer, ARRAYSIZE(cacheBuffer));

    std::wstring_view cacheBufferView(cacheBuffer);

    // https://stackoverflow.com/a/46931770
    auto splitStringView = [](std::wstring_view s, WCHAR delimiter) {
        size_t pos_start = 0, pos_end;
        std::wstring_view token;
        std::vector<std::wstring_view> res;

        while ((pos_end = s.find(delimiter, pos_start)) !=
               std::wstring_view::npos) {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + 1;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    };

    auto cacheParts = splitStringView(cacheBufferView, cacheSep);

    std::vector<bool> symbolResolved(symbolHooksCount, false);
    std::wstring newSystemCacheStr;

    auto onSymbolResolved = [symbolHooks, symbolHooksCount, &symbolResolved,
                             &newSystemCacheStr,
                             module](std::wstring_view symbol, void* address) {
        for (size_t i = 0; i < symbolHooksCount; i++) {
            if (symbolResolved[i]) {
                continue;
            }

            bool match = false;
            for (auto hookSymbol : symbolHooks[i].symbols) {
                if (hookSymbol == symbol) {
                    match = true;
                    break;
                }
            }

            if (!match) {
                continue;
            }

            if (symbolHooks[i].hookFunction) {
                Wh_SetFunctionHook(address, symbolHooks[i].hookFunction,
                                   symbolHooks[i].pOriginalFunction);
                Wh_Log(L"Hooked %p: %.*s", address, symbol.length(),
                       symbol.data());
            } else {
                *symbolHooks[i].pOriginalFunction = address;
                Wh_Log(L"Found %p: %.*s", address, symbol.length(),
                       symbol.data());
            }

            symbolResolved[i] = true;

            newSystemCacheStr += cacheSep;
            newSystemCacheStr += symbol;
            newSystemCacheStr += cacheSep;
            newSystemCacheStr +=
                std::to_wstring((ULONG_PTR)address - (ULONG_PTR)module);

            break;
        }
    };

    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)module;
    IMAGE_NT_HEADERS* header =
        (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
    auto timeStamp = std::to_wstring(header->FileHeader.TimeDateStamp);
    auto imageSize = std::to_wstring(header->OptionalHeader.SizeOfImage);

    newSystemCacheStr += cacheVer;
    newSystemCacheStr += cacheSep;
    newSystemCacheStr += timeStamp;
    newSystemCacheStr += cacheSep;
    newSystemCacheStr += imageSize;

    if (cacheParts.size() >= 3 &&
        cacheParts[0] == std::wstring_view(&cacheVer, 1) &&
        cacheParts[1] == timeStamp && cacheParts[2] == imageSize) {
        for (size_t i = 3; i + 1 < cacheParts.size(); i += 2) {
            auto symbol = cacheParts[i];
            auto address = cacheParts[i + 1];
            if (address.length() == 0) {
                continue;
            }

            void* addressPtr =
                (void*)(std::stoull(std::wstring(address), nullptr, 10) +
                        (ULONG_PTR)module);

            onSymbolResolved(symbol, addressPtr);
        }

        for (size_t i = 0; i < symbolHooksCount; i++) {
            if (symbolResolved[i] || !symbolHooks[i].optional) {
                continue;
            }

            size_t noAddressMatchCount = 0;
            for (size_t j = 3; j + 1 < cacheParts.size(); j += 2) {
                auto symbol = cacheParts[j];
                auto address = cacheParts[j + 1];
                if (address.length() != 0) {
                    continue;
                }

                for (auto hookSymbol : symbolHooks[i].symbols) {
                    if (hookSymbol == symbol) {
                        noAddressMatchCount++;
                        break;
                    }
                }
            }

            if (noAddressMatchCount == symbolHooks[i].symbols.size()) {
                Wh_Log(L"Optional symbol %d doesn't exist (from cache)", i);

                symbolResolved[i] = true;

                for (auto hookSymbol : symbolHooks[i].symbols) {
                    newSystemCacheStr += cacheSep;
                    newSystemCacheStr += hookSymbol;
                    newSystemCacheStr += cacheSep;
                }
            }
        }

        if (std::all_of(symbolResolved.begin(), symbolResolved.end(),
                        [](bool b) { return b; })) {
            return true;
        }
    }

    Wh_Log(L"Couldn't resolve all symbols from cache");

    if (cacheOnly) {
        return false;
    }

    WH_FIND_SYMBOL findSymbol;
    HANDLE findSymbolHandle = Wh_FindFirstSymbol(module, nullptr, &findSymbol);
    if (!findSymbolHandle) {
        Wh_Log(L"Wh_FindFirstSymbol failed");
        return false;
    }

    do {
        onSymbolResolved(findSymbol.symbol, findSymbol.address);
    } while (Wh_FindNextSymbol(findSymbolHandle, &findSymbol));

    Wh_FindCloseSymbol(findSymbolHandle);

    for (size_t i = 0; i < symbolHooksCount; i++) {
        if (symbolResolved[i]) {
            continue;
        }

        if (!symbolHooks[i].optional) {
            Wh_Log(L"Unresolved symbol: %d", i);
            return false;
        }

        Wh_Log(L"Optional symbol %d doesn't exist", i);

        for (auto hookSymbol : symbolHooks[i].symbols) {
            newSystemCacheStr += cacheSep;
            newSystemCacheStr += hookSymbol;
            newSystemCacheStr += cacheSep;
        }
    }

    if (newSystemCacheStr.length() <= cacheMaxSize) {
        Wh_SetStringValue(cacheStrKey.c_str(), newSystemCacheStr.c_str());
    } else {
        Wh_Log(L"Cache is too large (%zu)", newSystemCacheStr.length());
    }

    return true;
}

std::optional<std::wstring> GetUrlContent(PCWSTR lpUrl) {
    HINTERNET hOpenHandle = InternetOpen(
        L"WindhawkMod", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hOpenHandle) {
        return std::nullopt;
    }

    HINTERNET hUrlHandle =
        InternetOpenUrl(hOpenHandle, lpUrl, nullptr, 0,
                        INTERNET_FLAG_NO_AUTH | INTERNET_FLAG_NO_CACHE_WRITE |
                            INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI |
                            INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD,
                        0);
    if (!hUrlHandle) {
        InternetCloseHandle(hOpenHandle);
        return std::nullopt;
    }

    DWORD dwStatusCode = 0;
    DWORD dwStatusCodeSize = sizeof(dwStatusCode);
    if (!HttpQueryInfo(hUrlHandle,
                       HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                       &dwStatusCode, &dwStatusCodeSize, nullptr) ||
        dwStatusCode != 200) {
        InternetCloseHandle(hUrlHandle);
        InternetCloseHandle(hOpenHandle);
        return std::nullopt;
    }

    LPBYTE pUrlContent = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, 0x400);
    if (!pUrlContent) {
        InternetCloseHandle(hUrlHandle);
        InternetCloseHandle(hOpenHandle);
        return std::nullopt;
    }

    DWORD dwNumberOfBytesRead;
    InternetReadFile(hUrlHandle, pUrlContent, 0x400, &dwNumberOfBytesRead);
    DWORD dwLength = dwNumberOfBytesRead;

    while (dwNumberOfBytesRead) {
        LPBYTE pNewUrlContent = (LPBYTE)HeapReAlloc(
            GetProcessHeap(), 0, pUrlContent, dwLength + 0x400);
        if (!pNewUrlContent) {
            InternetCloseHandle(hUrlHandle);
            InternetCloseHandle(hOpenHandle);
            HeapFree(GetProcessHeap(), 0, pUrlContent);
            return std::nullopt;
        }

        pUrlContent = pNewUrlContent;
        InternetReadFile(hUrlHandle, pUrlContent + dwLength, 0x400,
                         &dwNumberOfBytesRead);
        dwLength += dwNumberOfBytesRead;
    }

    InternetCloseHandle(hUrlHandle);
    InternetCloseHandle(hOpenHandle);

    // Assume UTF-8.
    int charsNeeded = MultiByteToWideChar(CP_UTF8, 0, (PCSTR)pUrlContent,
                                          dwLength, nullptr, 0);
    std::wstring unicodeContent(charsNeeded, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, (PCSTR)pUrlContent, dwLength,
                        unicodeContent.data(), unicodeContent.size());

    HeapFree(GetProcessHeap(), 0, pUrlContent);

    return unicodeContent;
}

bool HookSymbolsWithOnlineCacheFallback(HMODULE module,
                                        const SYMBOL_HOOK* symbolHooks,
                                        size_t symbolHooksCount) {
    constexpr WCHAR kModIdForCache[] = L"taskbar-labels";

    if (HookSymbols(module, symbolHooks, symbolHooksCount,
                    /*cacheOnly=*/true)) {
        return true;
    }

    Wh_Log(L"HookSymbols() from cache failed, trying to get an online cache");

    WCHAR moduleFilePath[MAX_PATH];
    DWORD moduleFilePathLen =
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath));
    if (!moduleFilePathLen || moduleFilePathLen == ARRAYSIZE(moduleFilePath)) {
        Wh_Log(L"GetModuleFileName failed");
        return false;
    }

    PWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        Wh_Log(L"GetModuleFileName returned unsupported path");
        return false;
    }

    moduleFileName++;

    DWORD moduleFileNameLen =
        moduleFilePathLen - (moduleFileName - moduleFilePath);

    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_LOWERCASE, moduleFileName,
                  moduleFileNameLen, moduleFileName, moduleFileNameLen, nullptr,
                  nullptr, 0);

    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)module;
    IMAGE_NT_HEADERS* header =
        (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
    auto timeStamp = std::to_wstring(header->FileHeader.TimeDateStamp);
    auto imageSize = std::to_wstring(header->OptionalHeader.SizeOfImage);

    std::wstring cacheStrKey =
#if defined(_M_IX86)
        L"symbol-x86-cache-";
#elif defined(_M_X64)
        L"symbol-cache-";
#else
#error "Unsupported architecture"
#endif
    cacheStrKey += moduleFileName;

    std::wstring onlineCacheUrl =
        L"https://ramensoftware.github.io/windhawk-mod-symbol-cache/";
    onlineCacheUrl += kModIdForCache;
    onlineCacheUrl += L'/';
    onlineCacheUrl += cacheStrKey;
    onlineCacheUrl += L'/';
    onlineCacheUrl += timeStamp;
    onlineCacheUrl += L'-';
    onlineCacheUrl += imageSize;
    onlineCacheUrl += L".txt";

    Wh_Log(L"Looking for an online cache at %s", onlineCacheUrl.c_str());

    auto onlineCache = GetUrlContent(onlineCacheUrl.c_str());
    if (onlineCache) {
        Wh_SetStringValue(cacheStrKey.c_str(), onlineCache->c_str());
    } else {
        Wh_Log(L"Failed to get online cache");
    }

    return HookSymbols(module, symbolHooks, symbolHooksCount);
}

bool GetTaskbarViewDllPath(WCHAR path[MAX_PATH]) {
    WCHAR szWindowsDirectory[MAX_PATH];
    if (!GetWindowsDirectory(szWindowsDirectory,
                             ARRAYSIZE(szWindowsDirectory))) {
        Wh_Log(L"GetWindowsDirectory failed");
        return false;
    }

    // Windows 11 version 22H2.
    wcscpy_s(path, MAX_PATH, szWindowsDirectory);
    wcscat_s(
        path, MAX_PATH,
        LR"(\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\Taskbar.View.dll)");
    if (GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES) {
        return true;
    }

    // Windows 11 version 21H2.
    wcscpy_s(path, MAX_PATH, szWindowsDirectory);
    wcscat_s(
        path, MAX_PATH,
        LR"(\SystemApps\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\ExplorerExtensions.dll)");
    if (GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES) {
        return true;
    }

    return false;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    SYMBOL_HOOK symbolHooks[] =  //
        {
            {
                {
                    LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Taskbar::ITaskListButton>::get_IsRunning(bool *))",
                    LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Taskbar::ITaskListButton>::get_IsRunning(bool * __ptr64) __ptr64)",
                },
                (void**)&TaskListButton_get_IsRunning_Original,
            },
            {
                {
                    LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))",
                    LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void) __ptr64)",
                },
                (void**)&TaskListButton_UpdateVisualStates_Original,
                (void*)TaskListButton_UpdateVisualStates_Hook,
            },
            {
                {
                    LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))",
                    LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void) __ptr64)",
                },
                (void**)&TaskListButton_UpdateButtonPadding_Original,
                (void*)TaskListButton_UpdateButtonPadding_Hook,
            },
            {
                {
                    LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateBadgeSize(void))",
                    LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateBadgeSize(void) __ptr64)",
                },
                (void**)&TaskListButton_UpdateBadgeSize_Original,
                (void*)TaskListButton_UpdateBadgeSize_Hook,
            },
            {
                {
                    LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::OnTaskbarLayoutChildBoundsChanged(void))",
                    LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::OnTaskbarLayoutChildBoundsChanged(void) __ptr64)",
                },
                (void**)&TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original,
                (void*)TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Hook,
            },
            {
                {
                    LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::Icon(struct winrt::Windows::Storage::Streams::IRandomAccessStream))",
                    LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::Icon(struct winrt::Windows::Storage::Streams::IRandomAccessStream) __ptr64)",
                },
                (void**)&TaskListButton_Icon_Original,
                (void*)TaskListButton_Icon_Hook,
            },
            {
                {
                    LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskbarSettings5<struct winrt::WindowsUdk::UI::Shell::TaskbarSettings>::GroupingMode(void)const )",
                    LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskbarSettings5<struct winrt::WindowsUdk::UI::Shell::TaskbarSettings>::GroupingMode(void)const __ptr64)",
                },
                (void**)&TaskbarSettings_GroupingMode_Original,
                (void*)TaskbarSettings_GroupingMode_Hook,
                true,
            },
            {
                {
                    LR"(public: float __cdecl winrt::Taskbar::implementation::TaskListButton::MinScalableWidth(void))",
                    LR"(public: float __cdecl winrt::Taskbar::implementation::TaskListButton::MinScalableWidth(void) __ptr64)",
                },
                (void**)&TaskListButton_MinScalableWidth_Original,
                (void*)TaskListButton_MinScalableWidth_Hook,
                true,
            },
            {
                {
                    LR"(public: __cdecl winrt::impl::consume_Taskbar_ITaskbarAppItemViewModel<struct winrt::Taskbar::ITaskbarAppItemViewModel>::HasLabel(void)const )",
                    LR"(public: __cdecl winrt::impl::consume_Taskbar_ITaskbarAppItemViewModel<struct winrt::Taskbar::ITaskbarAppItemViewModel>::HasLabel(void)const __ptr64)",
                },
                (void**)&ITaskbarAppItemViewModel_HasLabels_Original,
                (void*)ITaskbarAppItemViewModel_HasLabels_Hook,
                true,
            },
            {
                {
                    LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::`vftable')",
                },
                (void**)&ITaskListWindowViewModel_vftable,
                nullptr,
                true,
            },
            {
                {
                    LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_TaskItem(void * *))",
                    LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_TaskItem(void * __ptr64 * __ptr64) __ptr64)",
                },
                (void**)&ITaskListWindowViewModel_get_TaskItem,
                nullptr,
                true,
            },
            {
                {
                    LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::get_HasLabel(bool *))",
                    LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::get_HasLabel(bool * __ptr64) __ptr64)",
                },
                (void**)&TaskListWindowViewModel_ITaskbarAppItemViewModel_get_HasLabel_Original,
                (void*)
                    TaskListWindowViewModel_ITaskbarAppItemViewModel_get_HasLabel_Hook,
                true,
            },
            {
                {
                    LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::get_HasLabel(bool *))",
                    LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::get_HasLabel(bool * __ptr64) __ptr64)",
                },
                (void**)&TaskListGroupViewModel_ITaskbarAppItemViewModel_get_HasLabel_Original,
                (void*)
                    TaskListGroupViewModel_ITaskbarAppItemViewModel_get_HasLabel_Hook,
                true,
            },
            {
                {
                    LR"(class wil::details::FeatureImpl<struct __WilExternalFeatureTraits_Feature_29785186> `private: static class wil::details::FeatureImpl<struct __WilExternalFeatureTraits_Feature_29785186> & __cdecl wil::Feature<struct __WilExternalFeatureTraits_Feature_29785186>::GetImpl(void)'::`2'::impl)",

                    // Symbol before update KB5036980:
                    LR"(class wil::details::FeatureImpl<struct __WilFeatureTraits_Feature_29785186> `private: static class wil::details::FeatureImpl<struct __WilFeatureTraits_Feature_29785186> & __cdecl wil::Feature<struct __WilFeatureTraits_Feature_29785186>::GetImpl(void)'::`2'::impl)",
                },
                (void**)&wil_Feature_GetImpl_Original,
                nullptr,
                true,
            },
            {
                {
                    LR"(public: bool __cdecl wil::details::FeatureImpl<struct __WilExternalFeatureTraits_Feature_29785186>::__private_IsEnabled(enum wil::ReportingKind))",
                    LR"(public: bool __cdecl wil::details::FeatureImpl<struct __WilExternalFeatureTraits_Feature_29785186>::__private_IsEnabled(enum wil::ReportingKind) __ptr64)",

                    // Symbols before update KB5036980:
                    LR"(public: bool __cdecl wil::details::FeatureImpl<struct __WilFeatureTraits_Feature_29785186>::__private_IsEnabled(enum wil::ReportingKind))",
                    LR"(public: bool __cdecl wil::details::FeatureImpl<struct __WilFeatureTraits_Feature_29785186>::__private_IsEnabled(enum wil::ReportingKind) __ptr64)",
                },
                (void**)&WilFeatureTraits_Feature_29785186_IsEnabled_Original,
                nullptr,
                true,
            },
        };

    return HookSymbolsWithOnlineCacheFallback(module, symbolHooks,
                                              ARRAYSIZE(symbolHooks));
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {
                LR"(protected: struct ITaskBtnGroup * __cdecl CTaskListWnd::_GetTBGroupFromGroup(struct ITaskGroup *,int *))",
                LR"(protected: struct ITaskBtnGroup * __ptr64 __cdecl CTaskListWnd::_GetTBGroupFromGroup(struct ITaskGroup * __ptr64,int * __ptr64) __ptr64)",
            },
            (void**)&CTaskListWnd__GetTBGroupFromGroup_Original,
        },
        {
            {
                LR"(public: virtual int __cdecl CTaskBtnGroup::GetNumItems(void))",
                LR"(public: virtual int __cdecl CTaskBtnGroup::GetNumItems(void) __ptr64)",
            },
            (void**)&CTaskBtnGroup_GetNumItems_Original,
        },
        {
            {
                LR"(public: virtual struct ITaskItem * __cdecl CTaskBtnGroup::GetTaskItem(int))",
                LR"(public: virtual struct ITaskItem * __ptr64 __cdecl CTaskBtnGroup::GetTaskItem(int) __ptr64)",
            },
            (void**)&CTaskBtnGroup_GetTaskItem_Original,
        },
        {
            {
                LR"(public: virtual long __cdecl CTaskGroup::GetTitleText(struct ITaskItem *,unsigned short *,int))",
                LR"(public: virtual long __cdecl CTaskGroup::GetTitleText(struct ITaskItem * __ptr64,unsigned short * __ptr64,int) __ptr64)",
            },
            (void**)&CTaskGroup_GetTitleText_Original,
        },
        {
            {
                LR"(public: virtual bool __cdecl IconContainer::IsStorageRecreationRequired(class CCoSimpleArray<unsigned int,4294967294,class CSimpleArrayStandardCompareHelper<unsigned int> > const &,enum IconContainerFlags))",
                LR"(public: virtual bool __cdecl IconContainer::IsStorageRecreationRequired(class CCoSimpleArray<unsigned int,4294967294,class CSimpleArrayStandardCompareHelper<unsigned int> > const & __ptr64,enum IconContainerFlags) __ptr64)",
            },
            (void**)&IconContainer_IsStorageRecreationRequired_Original,
            (void*)IconContainer_IsStorageRecreationRequired_Hook,
        },
        {
            {
                LR"(public: virtual void __cdecl CTaskListWnd::GroupChanged(struct ITaskGroup *,enum winrt::WindowsUdk::UI::Shell::TaskGroupProperty))",
                LR"(public: virtual void __cdecl CTaskListWnd::GroupChanged(struct ITaskGroup * __ptr64,enum winrt::WindowsUdk::UI::Shell::TaskGroupProperty) __ptr64)",
            },
            (void**)&CTaskListWnd_GroupChanged_Original,
            (void*)CTaskListWnd_GroupChanged_Hook,
        },
        {
            // An older variant, see the newer variant below.
            {
                LR"(public: virtual long __cdecl CTaskListWnd::TaskDestroyed(struct ITaskGroup *,struct ITaskItem *,enum TaskDestroyedFlags))",
                LR"(public: virtual long __cdecl CTaskListWnd::TaskDestroyed(struct ITaskGroup * __ptr64,struct ITaskItem * __ptr64,enum TaskDestroyedFlags) __ptr64)",
            },
            (void**)&CTaskListWnd_TaskDestroyed_Original,
            (void*)CTaskListWnd_TaskDestroyed_Hook,
            true,
        },
        {
            // A newer variant seen in insider builds.
            {
                LR"(public: virtual long __cdecl CTaskListWnd::TaskDestroyed(struct ITaskGroup *,struct ITaskItem *))",
                LR"(public: virtual long __cdecl CTaskListWnd::TaskDestroyed(struct ITaskGroup * __ptr64,struct ITaskItem * __ptr64) __ptr64)",
            },
            (void**)&CTaskListWnd_TaskDestroyed_2_Original,
            (void*)CTaskListWnd_TaskDestroyed_2_Hook,
            true,
        },
    };

    return HookSymbolsWithOnlineCacheFallback(module, taskbarDllHooks,
                                              ARRAYSIZE(taskbarDllHooks));
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
            !!TaskbarSettings_GroupingMode_Original;
    }

    if (!g_hasNativeLabelsImplementation) {
        if (!HookTaskbarDllSymbols()) {
            return FALSE;
        }
    }

    return TRUE;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module || g_unloading) {
        return module;
    }

    if (!g_taskbarViewDllLoaded &&
        _wcsicmp(g_taskbarViewDllPath, lpLibFileName) == 0 &&
        !g_taskbarViewDllLoaded.exchange(true) &&
        ModInitWithTaskbarView(module)) {
        Wh_ApplyHookOperations();
        ApplySettings();
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!GetTaskbarViewDllPath(g_taskbarViewDllPath)) {
        Wh_Log(L"Taskbar view module not found");
        return FALSE;
    }

    HMODULE taskbarViewModule = LoadLibraryEx(g_taskbarViewDllPath, nullptr,
                                              LOAD_WITH_ALTERED_SEARCH_PATH);
    if (taskbarViewModule) {
        g_taskbarViewDllLoaded = true;
        return ModInitWithTaskbarView(taskbarViewModule);
    }

    Wh_Log(L"Taskbar view module not loaded yet");

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    FARPROC pKernelBaseLoadLibraryExW =
        GetProcAddress(kernelBaseModule, "LoadLibraryExW");
    Wh_SetFunctionHook((void*)pKernelBaseLoadLibraryExW,
                       (void*)LoadLibraryExW_Hook,
                       (void**)&LoadLibraryExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_taskbarViewDllLoaded) {
        ApplySettings();
    } else {
        HMODULE taskbarViewModule = LoadLibraryEx(
            g_taskbarViewDllPath, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
        if (taskbarViewModule && !g_taskbarViewDllLoaded.exchange(true) &&
            ModInitWithTaskbarView(taskbarViewModule)) {
            Wh_ApplyHookOperations();
            ApplySettings();
        }
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

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

    LoadSettings();

    if (g_taskbarViewDllLoaded) {
        ApplySettings();
    }
}
