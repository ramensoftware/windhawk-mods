// ==WindhawkMod==
// @id              taskbar-labels
// @name            Taskbar Labels for Windows 11
// @description     Customize text labels and combining for running programs on the taskbar (Windows 11 only)
// @version         1.4.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWINVER=0x0A00 -lole32 -loleaut32 -lruntimeobject
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

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/base.h>

#include <algorithm>
#include <atomic>
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
std::atomic<bool> g_applyingSettings;
std::atomic<bool> g_overrideGroupingMode;
std::atomic<bool> g_unloading;

bool g_hasNativeLabelsImplementation;

double g_initialTaskbarItemWidth;

UINT_PTR g_invalidateTaskListButtonTimer;
std::unordered_set<FrameworkElement> g_taskListButtonsWithLabelMissing;

#if __cplusplus < 202302L
// Missing in older MinGW headers.
DECLARE_HANDLE(CO_MTA_USAGE_COOKIE);
WINOLEAPI CoIncrementMTAUsage(CO_MTA_USAGE_COOKIE* pCookie);
WINOLEAPI CoDecrementMTAUsage(CO_MTA_USAGE_COOKIE Cookie);
#endif

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

// https://gist.github.com/m417z/451dfc2dad88d7ba88ed1814779a26b4
std::wstring GetWindowAppId(HWND hWnd) {
    // {c8900b66-a973-584b-8cae-355b7f55341b}
    constexpr winrt::guid CLSID_StartMenuCacheAndAppResolver{
        0x660b90c8,
        0x73a9,
        0x4b58,
        {0x8c, 0xae, 0x35, 0x5b, 0x7f, 0x55, 0x34, 0x1b}};

    // {de25675a-72de-44b4-9373-05170450c140}
    constexpr winrt::guid IID_IAppResolver_8{
        0xde25675a,
        0x72de,
        0x44b4,
        {0x93, 0x73, 0x05, 0x17, 0x04, 0x50, 0xc1, 0x40}};

    struct IAppResolver_8 : public IUnknown {
       public:
        virtual HRESULT STDMETHODCALLTYPE GetAppIDForShortcut() = 0;
        virtual HRESULT STDMETHODCALLTYPE GetAppIDForShortcutObject() = 0;
        virtual HRESULT STDMETHODCALLTYPE
        GetAppIDForWindow(HWND hWnd,
                          WCHAR** pszAppId,
                          void* pUnknown1,
                          void* pUnknown2,
                          void* pUnknown3) = 0;
        virtual HRESULT STDMETHODCALLTYPE
        GetAppIDForProcess(DWORD dwProcessId,
                           WCHAR** pszAppId,
                           void* pUnknown1,
                           void* pUnknown2,
                           void* pUnknown3) = 0;
    };

    HRESULT hr;
    std::wstring result;

    CO_MTA_USAGE_COOKIE cookie;
    bool mtaUsageIncreased = SUCCEEDED(CoIncrementMTAUsage(&cookie));

    winrt::com_ptr<IAppResolver_8> appResolver;
    hr = CoCreateInstance(CLSID_StartMenuCacheAndAppResolver, nullptr,
                          CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
                          IID_IAppResolver_8, appResolver.put_void());
    if (SUCCEEDED(hr)) {
        WCHAR* pszAppId;
        hr = appResolver->GetAppIDForWindow(hWnd, &pszAppId, nullptr, nullptr,
                                            nullptr);
        if (SUCCEEDED(hr)) {
            result = pszAppId;
            CoTaskMemFree(pszAppId);
        }
    }

    appResolver = nullptr;

    if (mtaUsageIncreased) {
        CoDecrementMTAUsage(cookie);
    }

    return result;
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

void* TaskbarSettings_GroupingMode_Original;

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
                minWidth = taskListButtonWidth - 6;
                if (minWidth < 0) {
                    minWidth = 0;
                }
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
                if (minWidth > 0) {
                    indicatorElement.Dispatcher().TryRunAsync(
                        winrt::Windows::UI::Core::CoreDispatcherPriority::High,
                        [indicatorElement, minWidth]() {
                            indicatorElement.MinWidth(minWidth);
                        });
                }
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
                    (40 - firstColumnWidthPixels) + (iconWidth - 24) +
                    (g_settings.leftAndRightPaddingSize - 8) * 2;
            } else {
                indicatorMargin.Left = (taskListButtonWidth - minWidth) / 2 - 2;
                indicatorHorizontalAlignment = HorizontalAlignment::Left;
            }
        }
        indicatorElement.Margin(indicatorMargin);
        indicatorElement.HorizontalAlignment(indicatorHorizontalAlignment);

        int height = g_unloading || !g_settings.runningIndicatorHeight
                         ? 3
                         : std::max(g_settings.runningIndicatorHeight, 0);
        indicatorElement.Height(height);

        int verticalOffset =
            g_unloading ? 0 : g_settings.runningIndicatorVerticalOffset;
        Media::TranslateTransform verticalOffsetTransform;
        verticalOffsetTransform.Y(verticalOffset);
        indicatorElement.RenderTransform(verticalOffsetTransform);

        if (isProgressIndicator) {
            auto element = indicatorElement;
            if ((element = FindChildByName(element, L"LayoutRoot")) &&
                (element = FindChildByName(element, L"ProgressBarRoot")) &&
                (element = FindChildByClassName(
                     element, L"Windows.UI.Xaml.Controls.Border")) &&
                (element = FindChildByClassName(
                     element, L"Windows.UI.Xaml.Controls.Grid"))) {
                auto progressBarTrack =
                    FindChildByName(element, L"ProgressBarTrack");
                if (progressBarTrack) {
                    element.Height(height);
                    progressBarTrack.MinWidth(minWidth);
                }
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
                     ) {
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

            if (!excluded) {
                std::wstring appId = GetWindowAppId(hWnd);
                LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE,
                              appId.data(), appId.length(), appId.data(),
                              appId.length(), nullptr, nullptr, 0);
                if (g_settings.excludedPrograms.contains(appId.c_str())) {
                    excluded = true;
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
                {LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskbarSettings5<struct winrt::WindowsUdk::UI::Shell::TaskbarSettings>::GroupingMode(void)const )"},
                &TaskbarSettings_GroupingMode_Original,
                nullptr,
                true,
            },
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
                {LR"(public: __cdecl winrt::impl::consume_Taskbar_ITaskbarAppItemViewModel<struct winrt::Taskbar::ITaskbarAppItemViewModel>::HasLabel(void)const )"},
                &ITaskbarAppItemViewModel_HasLabels_Original,
                ITaskbarAppItemViewModel_HasLabels_Hook,
                true,
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

                TaskListWindowViewModel_ITaskbarAppItemViewModel_get_HasLabel_Hook,
                true,
            },
            {
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::get_HasLabel(bool *))"},
                &TaskListGroupViewModel_ITaskbarAppItemViewModel_get_HasLabel_Original,

                TaskListGroupViewModel_ITaskbarAppItemViewModel_get_HasLabel_Hook,
                true,
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

    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
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

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: virtual int __cdecl CTaskListThumbnailWnd::DisplayUI(struct ITaskBtnGroup *,struct ITaskItem *,struct ITaskItem *,unsigned long))"},
            &CTaskListThumbnailWnd_DisplayUI_Original,
            CTaskListThumbnailWnd_DisplayUI_Hook,
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool HookTaskbarDllSymbolsOldImplementation() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
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
            !!TaskbarSettings_GroupingMode_Original;
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
        WindhawkUtils::Wh_SetFunctionHookT(
            pKernelBaseRegGetValueW, RegGetValueW_Hook, &RegGetValueW_Original);
    }

    return TRUE;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!ModInitWithTaskbarView(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
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

    if (g_taskbarViewDllLoaded) {
        ApplySettings();
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
