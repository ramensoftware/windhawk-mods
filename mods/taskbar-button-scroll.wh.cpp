// ==WindhawkMod==
// @id              taskbar-button-scroll
// @name            Taskbar minimize/restore on scroll
// @description     Minimize/restore by scrolling the mouse wheel over taskbar buttons and thumbnail previews
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
# Taskbar minimize/restore on scroll

Minimize/restore by scrolling the mouse wheel over taskbar buttons and thumbnail
previews.

Only Windows 10 64-bit and Windows 11 are supported. For older Windows versions
check out [7+ Taskbar Tweaker](https://tweaker.ramensoftware.com/).

**Note:** To customize the old taskbar on Windows 11 (if using ExplorerPatcher
or a similar tool), enable the relevant option in the mod's settings.

![Demonstration](https://i.imgur.com/rnnwOss.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- scrollOverTaskbarButtons: true
  $name: Scroll over taskbar buttons
- scrollOverThumbnailPreviews: true
  $name: Scroll over thumbnail previews
- maximizeAndRestore: false
  $name: Maximize and restore
  $description: >-
    By default, the mod switches between minimize/restore states on scroll. This
    option switches to three states: minimize/restore/maximize.
- reverseScrollingDirection: false
  $name: Reverse scrolling direction
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
#include <windowsx.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <atomic>
#include <regex>
#include <string>
#include <string_view>

using namespace winrt::Windows::UI::Xaml;

struct {
    bool scrollOverTaskbarButtons;
    bool scrollOverThumbnailPreviews;
    bool maximizeAndRestore;
    bool reverseScrollingDirection;
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

constexpr int kShortDelay = 400;
constexpr int kLongDelay = 5000;
constexpr UINT_PTR kRefreshTaskbarTimer = 1731020327;

int g_pointerWheelEventMouseWheelDelta;
DWORD g_pointerWheelEventMouseWheelTime;
WPARAM g_invokingContextMenuWParam;
int g_thumbnailContextMenuLastIndex;
void* g_lastScrollTarget;
DWORD g_lastScrollTime;
short g_lastScrollDeltaRemainder;
int g_lastScrollCommand;
DWORD g_lastScrollCommandTime;
std::atomic<DWORD> g_groupMenuCommandThreadId;
void* g_groupMenuCommandTaskItem;
ULONGLONG g_noDismissHoverUIUntil;
bool g_captureTaskGroup;
void* g_capturedTaskGroup;

#pragma region offsets

void* CTaskListWnd_SetTaskFilter;

// Only for ExplorerPatcher.
using CTaskListWnd_GetTaskFilterPtr_t = void** (*)(void*);
CTaskListWnd_GetTaskFilterPtr_t CTaskListWnd_GetTaskFilterPtr;

size_t OffsetFromAssemblyRegex(void* func,
                               size_t defValue,
                               std::regex regex,
                               int limit = 30) {
    BYTE* p = (BYTE*)func;
    for (int i = 0; i < limit; i++) {
        WH_DISASM_RESULT result;
        if (!Wh_Disasm(p, &result)) {
            break;
        }

        p += result.length;

        std::string_view s = result.text;
        if (s == "ret") {
            break;
        }

        std::match_results<std::string_view::const_iterator> match;
        if (std::regex_match(s.begin(), s.end(), match, regex)) {
            // Wh_Log(L"%S", result.text);
            return std::stoull(match[1], nullptr, 16);
        }
    }

    Wh_Log(L"Failed for %p", func);
    return defValue;
}

void* Get_TaskItemFilter_For_CTaskListWnd_ITaskListUI(void* pThis_ITaskListUI) {
    static size_t offset =
#if defined(_M_X64)
        OffsetFromAssemblyRegex(CTaskListWnd_SetTaskFilter, 0x1F8,
                                std::regex(R"(add rcx, 0x([0-9a-f]+))",
                                           std::regex_constants::icase),
                                10);
#elif defined(_M_ARM64)
        OffsetFromAssemblyRegex(
            CTaskListWnd_SetTaskFilter, 0x1F8,
            std::regex(R"(add\s+x\d+, x\d+, #0x([0-9a-f]+))",
                       std::regex_constants::icase),
            10);
#else
#error "Unsupported architecture"
#endif

    return *(void**)((DWORD_PTR)pThis_ITaskListUI + offset);
}

#pragma endregion  // offsets

using DwmpActivateLivePreview_t = HRESULT(WINAPI*)(BOOL peekOn,
                                                   HWND hPeekWindow,
                                                   HWND hTopmostWindow,
                                                   UINT peekType,
                                                   void* param5);
DwmpActivateLivePreview_t pDwmpActivateLivePreview;

using CTaskBtnGroup_GetGroup_t = void*(WINAPI*)(void* pThis);
CTaskBtnGroup_GetGroup_t CTaskBtnGroup_GetGroup_Original;

using CTaskBtnGroup_GetGroupType_t = int(WINAPI*)(void* pThis);
CTaskBtnGroup_GetGroupType_t CTaskBtnGroup_GetGroupType_Original;

using CTaskBtnGroup_GetTaskItem_t = void*(WINAPI*)(void* pThis, int index);
CTaskBtnGroup_GetTaskItem_t CTaskBtnGroup_GetTaskItem_Original;

using CTaskGroup_GroupMenuCommand_t = HRESULT(WINAPI*)(void* pThis,
                                                       void* filter,
                                                       int command);
CTaskGroup_GroupMenuCommand_t CTaskGroup_GroupMenuCommand_Original;

void* CTaskListWnd_vftable_CImpWndProc;
void* CTaskListWnd_vftable_ITaskListUI;

using TaskItemThumbnail_ReportClicked_t = int(WINAPI*)(void* pThis,
                                                       void* launcherOptions);
TaskItemThumbnail_ReportClicked_t TaskItemThumbnail_ReportClicked_Original;

using TaskItem_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskItem_ReportClicked_t TaskItem_ReportClicked_Original;

using TaskGroup_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskGroup_ReportClicked_t TaskGroup_ReportClicked_Original;

using TryGetItemFromContainer_TaskItemThumbnailViewModel_t =
    void*(WINAPI*)(void** output, UIElement* container);
TryGetItemFromContainer_TaskItemThumbnailViewModel_t
    TryGetItemFromContainer_TaskItemThumbnailViewModel_Original;

using TaskItemThumbnailViewModel_get_TaskItemThumbnail_t =
    int(WINAPI*)(void* pThis, void** taskItemThumbnail);
TaskItemThumbnailViewModel_get_TaskItemThumbnail_t
    TaskItemThumbnailViewModel_get_TaskItemThumbnail_Original;

using TryGetItemFromContainer_TaskListWindowViewModel_t =
    void*(WINAPI*)(void** output, UIElement* container);
TryGetItemFromContainer_TaskListWindowViewModel_t
    TryGetItemFromContainer_TaskListWindowViewModel_Original;

using TaskListWindowViewModel_get_TaskItem_t = int(WINAPI*)(void* pThis,
                                                            void** taskItem);
TaskListWindowViewModel_get_TaskItem_t
    TaskListWindowViewModel_get_TaskItem_Original;

using TryGetItemFromContainer_TaskListGroupViewModel_t =
    void*(WINAPI*)(void** output, UIElement* container);
TryGetItemFromContainer_TaskListGroupViewModel_t
    TryGetItemFromContainer_TaskListGroupViewModel_Original;

using TaskListGroupViewModel_IsMultiWindow_t = bool(WINAPI*)(void* pThis);
TaskListGroupViewModel_IsMultiWindow_t
    TaskListGroupViewModel_IsMultiWindow_Original;

using ITaskGroup_IsRunning_t = bool(WINAPI*)(void* pThis);
ITaskGroup_IsRunning_t ITaskGroup_IsRunning_Original;
bool WINAPI ITaskGroup_IsRunning_Hook(void* pThis) {
    if (g_captureTaskGroup) {
        Wh_Log(L">");
        g_capturedTaskGroup = *(void**)pThis;
        return false;
    }

    return ITaskGroup_IsRunning_Original(pThis);
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

void* GetWindowsUdkTaskItemFromTaskListButton(UIElement element) {
    winrt::com_ptr<IUnknown> windowViewModel = nullptr;
    TryGetItemFromContainer_TaskListWindowViewModel_Original(
        windowViewModel.put_void(), &element);
    if (!windowViewModel) {
        return nullptr;
    }

    winrt::com_ptr<IUnknown> windowsUdkTaskItem;
    TaskListWindowViewModel_get_TaskItem_Original(
        windowViewModel.get(), windowsUdkTaskItem.put_void());
    return windowsUdkTaskItem.get();
}

void* GetWindowsUdkTaskGroupFromTaskListButton(UIElement element) {
    winrt::com_ptr<IUnknown> groupViewModel = nullptr;
    TryGetItemFromContainer_TaskListGroupViewModel_Original(
        groupViewModel.put_void(), &element);
    if (!groupViewModel) {
        return nullptr;
    }

    g_capturedTaskGroup = nullptr;
    g_captureTaskGroup = true;
    TaskListGroupViewModel_IsMultiWindow_Original((void**)groupViewModel.get() -
                                                  1);
    g_captureTaskGroup = false;
    return g_capturedTaskGroup;
}

void* GetWindowsUdkTaskItemThumbnailFromElement(UIElement element) {
    winrt::com_ptr<IUnknown> thumbnailViewModel = nullptr;
    TryGetItemFromContainer_TaskItemThumbnailViewModel_Original(
        thumbnailViewModel.put_void(), &element);

    if (thumbnailViewModel) {
        winrt::com_ptr<IUnknown> windowsUdkTaskItemThumbnail;
        TaskItemThumbnailViewModel_get_TaskItemThumbnail_Original(
            thumbnailViewModel.get(), windowsUdkTaskItemThumbnail.put_void());
        return windowsUdkTaskItemThumbnail.get();
    }

    return nullptr;
}

constexpr int kScrollCommandOriginal = -1;

int GetScrollCommand(void* scrollTarget) {
    DWORD tickCountNow = GetTickCount();

    if (!g_pointerWheelEventMouseWheelDelta &&
        tickCountNow - g_pointerWheelEventMouseWheelTime < kShortDelay) {
        Wh_Log(L"Too soon after wheel scroll, ignoring event");
        return 0;
    }

    if (tickCountNow - g_pointerWheelEventMouseWheelTime >= kLongDelay) {
        g_pointerWheelEventMouseWheelDelta = 0;
    }

    if (!g_pointerWheelEventMouseWheelDelta) {
        return kScrollCommandOriginal;
    }

    int delta = g_pointerWheelEventMouseWheelDelta;
    g_pointerWheelEventMouseWheelDelta = 0;

    if (g_lastScrollTarget == scrollTarget &&
        tickCountNow - g_lastScrollTime < kLongDelay) {
        delta += g_lastScrollDeltaRemainder;
    }

    int clicks = delta / WHEEL_DELTA;
    Wh_Log(L"%d clicks (delta=%d)", clicks, delta);

    if (g_settings.reverseScrollingDirection) {
        clicks = -clicks;
    }

    int command = 0;

    if (clicks > 0) {
        command = SC_RESTORE;
    } else if (clicks < 0) {
        command = SC_MINIMIZE;
    }

    if (command && g_lastScrollTarget == scrollTarget &&
        command == g_lastScrollCommand &&
        tickCountNow - g_lastScrollCommandTime < kShortDelay) {
        Wh_Log(L"Ignoring rapid event");
        command = 0;
    }

    if (command) {
        g_lastScrollCommand = command;
        g_lastScrollCommandTime = tickCountNow;
    }

    g_lastScrollTarget = scrollTarget;
    g_lastScrollTime = tickCountNow;
    g_lastScrollDeltaRemainder = delta % WHEEL_DELTA;

    return command;
}

void TriggerScrollCommand(void* pThis,
                          void* taskGroup,
                          void* taskItem,
                          int command) {
    g_groupMenuCommandThreadId = GetCurrentThreadId();
    g_groupMenuCommandTaskItem = taskItem;

    void* pThis_CImpWndProc =
        QueryViaVtableBackwards(pThis, CTaskListWnd_vftable_CImpWndProc);

    void* taskFilter;
    if (CTaskListWnd_GetTaskFilterPtr) {
        taskFilter = *CTaskListWnd_GetTaskFilterPtr(pThis_CImpWndProc);
    } else {
        void* pThis_ITaskListUI =
            QueryViaVtable(pThis_CImpWndProc, CTaskListWnd_vftable_ITaskListUI);
        taskFilter =
            Get_TaskItemFilter_For_CTaskListWnd_ITaskListUI(pThis_ITaskListUI);
    }

    Wh_Log(L"Triggering command 0x%04X", command);
    CTaskGroup_GroupMenuCommand_Original(taskGroup, taskFilter, command);

    g_groupMenuCommandThreadId = 0;
    g_groupMenuCommandTaskItem = nullptr;
}

using CTaskListWnd__HandleClick_t = void(WINAPI*)(void* pThis,
                                                  void* taskBtnGroup,
                                                  int taskItemIndex,
                                                  int clickAction,
                                                  int param4,
                                                  int param5);
CTaskListWnd__HandleClick_t CTaskListWnd__HandleClick_Original;
void WINAPI CTaskListWnd__HandleClick_Hook(void* pThis,
                                           void* taskBtnGroup,
                                           int taskItemIndex,
                                           int clickAction,
                                           int param4,
                                           int param5) {
    Wh_Log(L"> clickAction=%d, taskItemIndex=%d", clickAction, taskItemIndex);

    int command = GetScrollCommand(taskBtnGroup);
    switch (command) {
        case 0:
            return;

        case kScrollCommandOriginal:
            return CTaskListWnd__HandleClick_Original(
                pThis, taskBtnGroup, taskItemIndex, clickAction, param4,
                param5);
    }

    void* taskGroup = CTaskBtnGroup_GetGroup_Original(taskBtnGroup);
    if (!taskGroup) {
        Wh_Log(L"No task group");
        return;
    }

    // Group types:
    // 1 - Single item or multiple uncombined items
    // 2 - Pinned item
    // 3 - Multiple combined items
    int groupType = CTaskBtnGroup_GetGroupType_Original(taskBtnGroup);
    if (groupType == 2) {
        Wh_Log(L"Ignoring pinned item");
        return;
    }

    void* taskItem = groupType == 3 ? nullptr
                                    : CTaskBtnGroup_GetTaskItem_Original(
                                          taskBtnGroup, taskItemIndex);

    TriggerScrollCommand(pThis, taskGroup, taskItem, command);
}

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

    int command = GetScrollCommand(taskGroup);
    switch (command) {
        case 0:
            return S_OK;

        case kScrollCommandOriginal:
            return CTaskListWnd_HandleExtendedUIClick_Original(
                pThis, taskGroup, taskItem, launcherOptions);
    }

    // Stop aero peek.
    if (pDwmpActivateLivePreview) {
        pDwmpActivateLivePreview(FALSE, nullptr, nullptr, 3, nullptr);
    }

    TriggerScrollCommand(pThis, taskGroup, taskItem, command);

    return S_OK;
}

BOOL CanMinimizeWindow(HWND hWnd) {
    if (IsIconic(hWnd) || !IsWindowEnabled(hWnd)) {
        return FALSE;
    }

    long lWndStyle = GetWindowLong(hWnd, GWL_STYLE);
    if (!(lWndStyle & WS_MINIMIZEBOX)) {
        return FALSE;
    }

    if ((lWndStyle & (WS_CAPTION | WS_SYSMENU)) != (WS_CAPTION | WS_SYSMENU)) {
        return TRUE;
    }

    HMENU hSystemMenu = GetSystemMenu(hWnd, FALSE);
    if (!hSystemMenu) {
        return FALSE;
    }

    UINT uMenuState = GetMenuState(hSystemMenu, SC_MINIMIZE, MF_BYCOMMAND);
    if (uMenuState == (UINT)-1) {
        return TRUE;
    }

    return ((uMenuState & MF_DISABLED) == FALSE);
}

BOOL CanMaximizeWindow(HWND hWnd) {
    if (!IsWindowEnabled(hWnd)) {
        return FALSE;
    }

    long lWndStyle = GetWindowLong(hWnd, GWL_STYLE);
    if (!(lWndStyle & WS_MAXIMIZEBOX)) {
        return FALSE;
    }

    return TRUE;
}

BOOL CanRestoreWindow(HWND hWnd) {
    if (!IsWindowEnabled(hWnd)) {
        return FALSE;
    }

    long lWndStyle = GetWindowLong(hWnd, GWL_STYLE);
    if (!(lWndStyle & WS_MAXIMIZEBOX)) {
        return FALSE;
    }

    return TRUE;
}

void SwitchToWindow(HWND hWnd) {
    BOOL bRestore = FALSE;
    HWND hActiveWnd = hWnd;
    HWND hTempWnd = GetLastActivePopup(GetAncestor(hWnd, GA_ROOTOWNER));

    if (hTempWnd && hTempWnd != hWnd && IsWindowVisible(hTempWnd) &&
        IsWindowEnabled(hTempWnd)) {
        HWND hOwnerWnd = GetWindow(hTempWnd, GW_OWNER);

        while (hOwnerWnd && hOwnerWnd != hWnd) {
            hOwnerWnd = GetWindow(hOwnerWnd, GW_OWNER);
        }

        if (hOwnerWnd == hWnd) {
            bRestore = IsIconic(hWnd);
            hActiveWnd = hTempWnd;
        }
    }

    if (IsIconic(hActiveWnd) && !IsWindowEnabled(hActiveWnd)) {
        ShowWindowAsync(hWnd, SW_RESTORE);
    } else {
        SwitchToThisWindow(hActiveWnd, TRUE);
        if (bRestore) {
            ShowWindowAsync(hWnd, SW_RESTORE);
        }
    }
}

bool MinimizeWithScroll(HWND hWnd) {
    if (g_settings.maximizeAndRestore && CanRestoreWindow(hWnd) &&
        IsZoomed(hWnd)) {
        SwitchToWindow(hWnd);
        return PostMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    }

    if (CanMinimizeWindow(hWnd)) {
        DWORD dwProcessId;
        GetWindowThreadProcessId(hWnd, &dwProcessId);
        AllowSetForegroundWindow(dwProcessId);
        return PostMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
    }

    return true;
}

bool RestoreWithScroll(HWND hWnd) {
    if (g_settings.maximizeAndRestore && CanMaximizeWindow(hWnd) &&
        !IsIconic(hWnd) && !IsZoomed(hWnd)) {
        SwitchToWindow(hWnd);
        return PostMessage(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
    }

    SwitchToWindow(hWnd);
    return true;
}

using CApi_PostMessageW_t = BOOL(
    WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CApi_PostMessageW_t CApi_PostMessageW_Original;
BOOL WINAPI CApi_PostMessageW_Hook(void* pThis,
                                   HWND hWnd,
                                   UINT Msg,
                                   WPARAM wParam,
                                   LPARAM lParam) {
    if (g_groupMenuCommandThreadId == GetCurrentThreadId() &&
        Msg == WM_SYSCOMMAND) {
        Wh_Log(L">");

        switch (wParam) {
            case SC_MINIMIZE:
                return MinimizeWithScroll(hWnd);

            case SC_RESTORE:
                return RestoreWithScroll(hWnd);
        }
    }

    return CApi_PostMessageW_Original(pThis, hWnd, Msg, wParam, lParam);
}

using CApi_BringWindowToTop_t = BOOL(WINAPI*)(void* pThis, HWND hWnd);
CApi_BringWindowToTop_t CApi_BringWindowToTop_Original;
BOOL WINAPI CApi_BringWindowToTop_Hook(void* pThis, HWND hWnd) {
    if (g_groupMenuCommandThreadId == GetCurrentThreadId()) {
        Wh_Log(L">");

        // This function is being called by CTaskGroup::GroupMenuCommand for the
        // SC_RESTORE command. Calling BringWindowToTop for a window that's not
        // responding causes the taskbar to hang. Skip it.
        return TRUE;
    }

    return CApi_BringWindowToTop_Original(pThis, hWnd);
}

using CTaskItem_IsVisibleOnCurrentVirtualDesktop_t = bool(WINAPI*)(void* pThis);
CTaskItem_IsVisibleOnCurrentVirtualDesktop_t
    CTaskItem_IsVisibleOnCurrentVirtualDesktop_Original;
bool WINAPI CTaskItem_IsVisibleOnCurrentVirtualDesktop_Hook(void* pThis) {
    if (g_groupMenuCommandThreadId == GetCurrentThreadId()) {
        Wh_Log(L">");

        if (g_groupMenuCommandTaskItem) {
            return g_groupMenuCommandTaskItem == pThis;
        }
    }

    return CTaskItem_IsVisibleOnCurrentVirtualDesktop_Original(pThis);
}

using TaskListButton_FlyoutFrame_OnPointerWheelChanged_t =
    int(WINAPI*)(void* pThis, void* pArgs);

int TaskListButton_FlyoutFrame_OnPointerWheelChanged_Hook(
    void* pThis,
    void* pArgs,
    TaskListButton_FlyoutFrame_OnPointerWheelChanged_t originalFunctionPtr) {
    Wh_Log(L">");

    auto original = [=]() { return originalFunctionPtr(pThis, pArgs); };

    if (GetKeyState(VK_CONTROL) < 0) {
        return original();
    }

    UIElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<UIElement>(), winrt::put_abi(element));
    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);
    Wh_Log(L"%s", className.c_str());

    bool isThumbnail = false;
    if (className == L"Taskbar.TaskListButton") {
        if (!g_settings.scrollOverTaskbarButtons ||
            !TryGetItemFromContainer_TaskListWindowViewModel_Original ||
            !TaskListWindowViewModel_get_TaskItem_Original ||
            !TryGetItemFromContainer_TaskListGroupViewModel_Original ||
            !TaskListGroupViewModel_IsMultiWindow_Original ||
            !ITaskGroup_IsRunning_Original ||
            !TaskItem_ReportClicked_Original ||
            !TaskGroup_ReportClicked_Original) {
            return original();
        }
    } else if (className == L"Taskbar.FlyoutFrame") {
        if (!g_settings.scrollOverThumbnailPreviews ||
            !TryGetItemFromContainer_TaskItemThumbnailViewModel_Original ||
            !TaskItemThumbnailViewModel_get_TaskItemThumbnail_Original ||
            !TaskItemThumbnail_ReportClicked_Original) {
            return original();
        }

        isThumbnail = true;
    } else {
        return original();
    }

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return original();
    }

    double delta = args.GetCurrentPoint(element).Properties().MouseWheelDelta();
    if (!delta) {
        return original();
    }

    DWORD now = GetTickCount();
    if (now - g_pointerWheelEventMouseWheelTime >= kLongDelay) {
        g_pointerWheelEventMouseWheelDelta = 0;
    }

    g_pointerWheelEventMouseWheelDelta += delta;
    g_pointerWheelEventMouseWheelTime = GetTickCount();

    void* windowsUdkTaskItemThumbnail = nullptr;
    void* windowsUdkTaskItem = nullptr;
    void* windowsUdkTaskGroup = nullptr;

    if (isThumbnail) {
        // For thumbnails, find the hovered thumbnail element and get its
        // TaskItemThumbnail.
        auto pointerPos = args.GetCurrentPoint(element).Position();
        auto hoveredElement =
            Media::VisualTreeHelper::FindElementsInHostCoordinates(pointerPos,
                                                                   element);
        for (const auto& child : hoveredElement) {
            if (winrt::get_class_name(child) ==
                L"Taskbar.TaskItemThumbnailView") {
                windowsUdkTaskItemThumbnail =
                    GetWindowsUdkTaskItemThumbnailFromElement(
                        child.as<UIElement>());
                break;
            }
        }
    } else {
        windowsUdkTaskItem = GetWindowsUdkTaskItemFromTaskListButton(element);
        if (!windowsUdkTaskItem) {
            windowsUdkTaskGroup =
                GetWindowsUdkTaskGroupFromTaskListButton(element);
        }
    }

    if (windowsUdkTaskItemThumbnail || windowsUdkTaskItem ||
        windowsUdkTaskGroup) {
        Wh_Log(
            L"Triggering click with delta %d, windowsUdkTaskItem=%p, "
            L"windowsUdkTaskGroup=%p, windowsUdkTaskItemThumbnail=%p",
            g_pointerWheelEventMouseWheelDelta, windowsUdkTaskItem,
            windowsUdkTaskGroup, windowsUdkTaskItemThumbnail);

        // Allows to steal focus.
        INPUT input{};
        SendInput(1, &input, sizeof(INPUT));

        if (windowsUdkTaskItemThumbnail) {
            TaskItemThumbnail_ReportClicked_Original(
                (void**)windowsUdkTaskItemThumbnail + 1, nullptr);
        } else if (windowsUdkTaskItem) {
            TaskItem_ReportClicked_Original(windowsUdkTaskItem, nullptr);
        } else if (windowsUdkTaskGroup) {
            TaskGroup_ReportClicked_Original(windowsUdkTaskGroup, nullptr);
        }
    }

    args.Handled(true);
    return 0;
}

TaskListButton_FlyoutFrame_OnPointerWheelChanged_t
    TaskListButton_OnPointerWheelChanged_Original;
int WINAPI TaskListButton_OnPointerWheelChanged_Hook(void* pThis, void* pArgs) {
    return TaskListButton_FlyoutFrame_OnPointerWheelChanged_Hook(
        pThis, pArgs, TaskListButton_OnPointerWheelChanged_Original);
}

TaskListButton_FlyoutFrame_OnPointerWheelChanged_t
    FlyoutFrame_OnPointerWheelChanged_Original;
int WINAPI FlyoutFrame_OnPointerWheelChanged_Hook(void* pThis, void* pArgs) {
    return TaskListButton_FlyoutFrame_OnPointerWheelChanged_Hook(
        pThis, pArgs, FlyoutFrame_OnPointerWheelChanged_Original);
}

using CTaskListWnd_ShowLivePreview_t = HWND(WINAPI*)(void* pThis,
                                                     void* taskItem,
                                                     DWORD flags);
CTaskListWnd_ShowLivePreview_t CTaskListWnd_ShowLivePreview_Original;

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(void* pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow_Original;

using CImmersiveTaskItem_GetWindow_t = HWND(WINAPI*)(void* pThis);
CImmersiveTaskItem_GetWindow_t CImmersiveTaskItem_GetWindow_Original;

void* CImmersiveTaskItem_vftable;

using CTaskListWnd_OnContextMenu_t = void(WINAPI*)(void* pThis,
                                                   POINT point,
                                                   HWND hWnd,
                                                   bool dontDismiss,
                                                   void* taskGroup,
                                                   void* taskItem);
CTaskListWnd_OnContextMenu_t CTaskListWnd_OnContextMenu_Original;
void WINAPI CTaskListWnd_OnContextMenu_Hook(void* pThis,
                                            POINT point,
                                            HWND hWnd,
                                            bool dontDismiss,
                                            void* taskGroup,
                                            void* taskItem) {
    if (!g_invokingContextMenuWParam) {
        return CTaskListWnd_OnContextMenu_Original(
            pThis, point, hWnd, dontDismiss, taskGroup, taskItem);
    }

    short delta = GET_WHEEL_DELTA_WPARAM(g_invokingContextMenuWParam);

    if (g_lastScrollTarget == taskItem &&
        GetTickCount() - g_lastScrollTime < kLongDelay) {
        delta += g_lastScrollDeltaRemainder;
    }

    int clicks = delta / WHEEL_DELTA;
    Wh_Log(L"%d clicks (delta=%d)", clicks, delta);

    if (g_settings.reverseScrollingDirection) {
        clicks = -clicks;
    }

    if (clicks != 0) {
        HWND hTaskItemWnd;
        if (*(void**)taskItem == CImmersiveTaskItem_vftable) {
            hTaskItemWnd = CImmersiveTaskItem_GetWindow_Original(taskItem);
        } else {
            hTaskItemWnd = CWindowTaskItem_GetWindow_Original(taskItem);
        }

        if (hTaskItemWnd) {
            CTaskListWnd_ShowLivePreview_Original(pThis, nullptr, 0);
            g_noDismissHoverUIUntil = GetTickCount64() + kShortDelay;

            KillTimer(hWnd, 2006);

            if (clicks > 0) {
                RestoreWithScroll(hTaskItemWnd);
            } else if (clicks < 0) {
                MinimizeWithScroll(hTaskItemWnd);
            }
        }
    }

    g_lastScrollTarget = taskItem;
    g_lastScrollTime = GetTickCount();
    g_lastScrollDeltaRemainder = delta % WHEEL_DELTA;
}

using CTaskListWnd_DismissHoverUI_t = HRESULT(WINAPI*)(void* pThis);
CTaskListWnd_DismissHoverUI_t CTaskListWnd_DismissHoverUI_Original;
HRESULT WINAPI CTaskListWnd_DismissHoverUI_Hook(void* pThis) {
    if (GetTickCount64() < g_noDismissHoverUIUntil) {
        Wh_Log(L">");
        return 0;
    }

    return CTaskListWnd_DismissHoverUI_Original(pThis);
}

using CTaskListThumbnailWnd_ThumbIndexFromPoint_t =
    int(WINAPI*)(void* pThis, const POINT* pt);
CTaskListThumbnailWnd_ThumbIndexFromPoint_t
    CTaskListThumbnailWnd_ThumbIndexFromPoint_Original;
int WINAPI CTaskListThumbnailWnd_ThumbIndexFromPoint_Hook(void* pThis,
                                                          const POINT* pt) {
    int ret = CTaskListThumbnailWnd_ThumbIndexFromPoint_Original(pThis, pt);

    if (g_invokingContextMenuWParam) {
        Wh_Log(L">");
        g_thumbnailContextMenuLastIndex = ret;
    }

    return ret;
}

using CTaskListWnd_v_WndProc_t = LRESULT(
    WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CTaskListWnd_v_WndProc_t CTaskListWnd_v_WndProc_Original;
LRESULT WINAPI CTaskListWnd_v_WndProc_Hook(void* pThis,
                                           HWND hWnd,
                                           UINT Msg,
                                           WPARAM wParam,
                                           LPARAM lParam) {
    if (Msg == WM_MOUSEWHEEL && g_settings.scrollOverTaskbarButtons &&
        !(GetKeyState(VK_CONTROL) < 0)) {
        short delta = GET_WHEEL_DELTA_WPARAM(wParam);

        DWORD now = GetTickCount();
        if (now - g_pointerWheelEventMouseWheelTime >= kLongDelay) {
            g_pointerWheelEventMouseWheelDelta = 0;
        }

        g_pointerWheelEventMouseWheelDelta += delta;
        g_pointerWheelEventMouseWheelTime = GetTickCount();

        Wh_Log(L"Simulating a mouse click with delta %d",
               g_pointerWheelEventMouseWheelDelta);

        // Allows to steal focus.
        INPUT input{};
        SendInput(1, &input, sizeof(INPUT));

        SetForegroundWindow(GetAncestor(hWnd, GA_ROOT));

        INPUT inputs[] = {
            {.type = INPUT_MOUSE, .mi = {.dwFlags = MOUSEEVENTF_LEFTDOWN}},
            {.type = INPUT_MOUSE, .mi = {.dwFlags = MOUSEEVENTF_LEFTUP}},
        };
        SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));

        return 0;
    }

    LRESULT ret =
        CTaskListWnd_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);

    return ret;
}

using CTaskListThumbnailWnd__HandleContextMenu_t = void(WINAPI*)(void* pThis,
                                                                 POINT point,
                                                                 int param2);
CTaskListThumbnailWnd__HandleContextMenu_t
    CTaskListThumbnailWnd__HandleContextMenu_Original;

using CTaskListThumbnailWnd__RefreshThumbnail_t = void(WINAPI*)(void* pThis,
                                                                int index);
CTaskListThumbnailWnd__RefreshThumbnail_t
    CTaskListThumbnailWnd__RefreshThumbnail_Original;

bool OnThumbnailWheelScroll(HWND hWnd,
                            UINT uMsg,
                            WPARAM wParam,
                            LPARAM lParam) {
    if (!g_settings.scrollOverThumbnailPreviews) {
        return false;
    }

    void* thumbnail = (void*)GetWindowLongPtr(hWnd, 0);
    if (!thumbnail) {
        return false;
    }

    // Allows to steal focus.
    INPUT input{};
    SendInput(1, &input, sizeof(INPUT));

    POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

    g_invokingContextMenuWParam = wParam;
    g_thumbnailContextMenuLastIndex = 0;
    CTaskListThumbnailWnd__HandleContextMenu_Original(thumbnail, pt, 0);
    g_invokingContextMenuWParam = 0;

    SetTimer(hWnd, kRefreshTaskbarTimer, 200, 0);

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
    switch (Msg) {
        case WM_MOUSEWHEEL:
            if (OnThumbnailWheelScroll(hWnd, Msg, wParam, lParam)) {
                return 0;
            }
            break;

        case WM_TIMER:
            switch (wParam) {
                case kRefreshTaskbarTimer: {
                    KillTimer(hWnd, kRefreshTaskbarTimer);
                    void* thumbnail = (void*)GetWindowLongPtr(hWnd, 0);
                    CTaskListThumbnailWnd__RefreshThumbnail_Original(
                        thumbnail, g_thumbnailContextMenuLastIndex);
                    return 0;
                }
            }
            break;
    }

    LRESULT ret = CTaskListThumbnailWnd_v_WndProc_Original(pThis, hWnd, Msg,
                                                           wParam, lParam);

    return ret;
}

void LoadSettings() {
    g_settings.scrollOverTaskbarButtons =
        Wh_GetIntSetting(L"scrollOverTaskbarButtons");
    g_settings.scrollOverThumbnailPreviews =
        Wh_GetIntSetting(L"scrollOverThumbnailPreviews");
    g_settings.maximizeAndRestore = Wh_GetIntSetting(L"maximizeAndRestore");
    g_settings.reverseScrollingDirection =
        Wh_GetIntSetting(L"reverseScrollingDirection");
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerWheelChanged(void *))"},
            &TaskListButton_OnPointerWheelChanged_Original,
            nullptr,  // Both OnPointerWheelChanged can have the same address.
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::FlyoutFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerWheelChanged(void *))"},
            &FlyoutFrame_OnPointerWheelChanged_Original,
            nullptr,  // Both OnPointerWheelChanged can have the same address.
            true,     // New XAML refresh thumbnails.
        },
        {
            {LR"(struct winrt::Taskbar::TaskItemThumbnailViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskItemThumbnailViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskItemThumbnailViewModel_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskItemThumbnailViewModel,struct winrt::Taskbar::ITaskItemThumbnailViewModel>::get_TaskItemThumbnail(void * *))"},
            &TaskItemThumbnailViewModel_get_TaskItemThumbnail_Original,
            nullptr,
            true,
        },
        {
            {LR"(struct winrt::Taskbar::TaskListWindowViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListWindowViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListWindowViewModel_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_TaskItem(void * *))"},
            &TaskListWindowViewModel_get_TaskItem_Original,
            nullptr,
            true,
        },
        {
            {LR"(struct winrt::Taskbar::TaskListGroupViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListGroupViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListGroupViewModel_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::TaskListGroupViewModel::IsMultiWindow(void)const )"},
            &TaskListGroupViewModel_IsMultiWindow_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskGroup<struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::IsRunning(void)const )"},
            &ITaskGroup_IsRunning_Original,
            ITaskGroup_IsRunning_Hook,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    // Only hook second OnPointerWheelChanged if the address is different from
    // the first one.
    bool hookFlyoutFrame_OnPointerWheelChanged_Original =
        FlyoutFrame_OnPointerWheelChanged_Original != nullptr &&
        FlyoutFrame_OnPointerWheelChanged_Original !=
            TaskListButton_OnPointerWheelChanged_Original;

    WindhawkUtils::Wh_SetFunctionHookT(
        TaskListButton_OnPointerWheelChanged_Original,
        TaskListButton_OnPointerWheelChanged_Hook,
        &TaskListButton_OnPointerWheelChanged_Original);

    if (hookFlyoutFrame_OnPointerWheelChanged_Original) {
        WindhawkUtils::Wh_SetFunctionHookT(
            FlyoutFrame_OnPointerWheelChanged_Original,
            FlyoutFrame_OnPointerWheelChanged_Hook,
            &FlyoutFrame_OnPointerWheelChanged_Original);
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
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual struct ITaskGroup * __cdecl CTaskBtnGroup::GetGroup(void))"},
            &CTaskBtnGroup_GetGroup_Original,
        },
        {
            {LR"(public: virtual enum eTBGROUPTYPE __cdecl CTaskBtnGroup::GetGroupType(void))"},
            &CTaskBtnGroup_GetGroupType_Original,
        },
        {
            {LR"(public: virtual struct ITaskItem * __cdecl CTaskBtnGroup::GetTaskItem(int))"},
            &CTaskBtnGroup_GetTaskItem_Original,
        },
        {
            {LR"(public: virtual long __cdecl CTaskGroup::GroupMenuCommand(struct ITaskItemFilter *,int))"},
            &CTaskGroup_GroupMenuCommand_Original,
        },
        {
            {LR"(const CTaskListWnd::`vftable'{for `CImpWndProc'})"},
            &CTaskListWnd_vftable_CImpWndProc,
        },
        {
            {LR"(const CTaskListWnd::`vftable'{for `ITaskListUI'})"},
            &CTaskListWnd_vftable_ITaskListUI,
        },
        {
            {LR"(protected: void __cdecl CTaskListWnd::_HandleClick(struct ITaskBtnGroup *,int,enum CTaskListWnd::eCLICKACTION,int,int))"},
            &CTaskListWnd__HandleClick_Original,
            CTaskListWnd__HandleClick_Hook,
        },
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleExtendedUIClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
            &CTaskListWnd_HandleExtendedUIClick_Original,
            CTaskListWnd_HandleExtendedUIClick_Hook,
            true,  // New XAML refresh thumbnails.
        },
        {
            {LR"(public: virtual bool __cdecl CTaskItem::IsVisibleOnCurrentVirtualDesktop(void))"},
            &CTaskItem_IsVisibleOnCurrentVirtualDesktop_Original,
            CTaskItem_IsVisibleOnCurrentVirtualDesktop_Hook,
        },
        {
            {LR"(public: virtual int __cdecl CApi::PostMessageW(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &CApi_PostMessageW_Original,
            CApi_PostMessageW_Hook,
        },
        {
            {LR"(public: virtual int __cdecl CApi::BringWindowToTop(struct HWND__ *))"},
            &CApi_BringWindowToTop_Original,
            CApi_BringWindowToTop_Hook,
        },
        {
            {
                // Windows 11.
                LR"(public: virtual long __cdecl CTaskListWnd::ShowLivePreview(struct ITaskItem *,unsigned long))",

                // Windows 10.
                LR"(public: virtual void __cdecl CTaskListWnd::ShowLivePreview(struct ITaskItem *,unsigned long))",
            },
            &CTaskListWnd_ShowLivePreview_Original,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            &CWindowTaskItem_GetWindow_Original,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetWindow(void))"},
            &CImmersiveTaskItem_GetWindow_Original,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            &CImmersiveTaskItem_vftable,
        },
        {
            {LR"(public: virtual void __cdecl CTaskListWnd::OnContextMenu(struct tagPOINT,struct HWND__ *,bool,struct ITaskGroup *,struct ITaskItem *))"},
            &CTaskListWnd_OnContextMenu_Original,
            CTaskListWnd_OnContextMenu_Hook,
        },
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::DismissHoverUI(int))"},
            &CTaskListWnd_DismissHoverUI_Original,
            CTaskListWnd_DismissHoverUI_Hook,
        },
        {
            {LR"(public: virtual int __cdecl CTaskListThumbnailWnd::ThumbIndexFromPoint(struct tagPOINT const &)const )"},
            &CTaskListThumbnailWnd_ThumbIndexFromPoint_Original,
            CTaskListThumbnailWnd_ThumbIndexFromPoint_Hook,
        },
        {
            {LR"(protected: virtual __int64 __cdecl CTaskListWnd::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &CTaskListWnd_v_WndProc_Original,
            CTaskListWnd_v_WndProc_Hook,
        },
        {
            {LR"(private: void __cdecl CTaskListThumbnailWnd::_HandleContextMenu(struct tagPOINT,int))"},
            &CTaskListThumbnailWnd__HandleContextMenu_Original,
        },
        {
            {LR"(private: void __cdecl CTaskListThumbnailWnd::_RefreshThumbnail(int))"},
            &CTaskListThumbnailWnd__RefreshThumbnail_Original,
        },
        {
            {LR"(private: virtual __int64 __cdecl CTaskListThumbnailWnd::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &CTaskListThumbnailWnd_v_WndProc_Original,
            CTaskListThumbnailWnd_v_WndProc_Hook,
        },
        // For offsets:
        {
            {LR"(public: virtual void __cdecl CTaskListWnd::SetTaskFilter(struct ITaskItemFilter *))"},
            &CTaskListWnd_SetTaskFilter,
        },
        // For XAML taskbar ReportClicked:
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskItemThumbnail,struct winrt::WindowsUdk::UI::Shell::ITaskItemThumbnail2>::ReportClicked(void *))"},
            &TaskItemThumbnail_ReportClicked_Original,
            nullptr,
            true,  // Only on Windows 11.
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskItem,struct winrt::WindowsUdk::UI::Shell::ITaskItem>::ReportClicked(void *))"},
            &TaskItem_ReportClicked_Original,
            nullptr,
            true,  // Only on Windows 11.
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskGroup,struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::ReportClicked(void *))"},
            &TaskGroup_ReportClicked_Original,
            nullptr,
            true,  // Only on Windows 11.
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
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
        {R"(?GetGroup@CTaskBtnGroup@@UEAAPEAUITaskGroup@@XZ)",
         &CTaskBtnGroup_GetGroup_Original},
        {R"(?GetGroupType@CTaskBtnGroup@@UEAA?AW4eTBGROUPTYPE@@XZ)",
         &CTaskBtnGroup_GetGroupType_Original},
        {R"(?GetTaskItem@CTaskBtnGroup@@UEAAPEAUITaskItem@@H@Z)",
         &CTaskBtnGroup_GetTaskItem_Original},
        {R"(?GroupMenuCommand@CTaskGroup@@UEAAJPEAUITaskItemFilter@@H@Z)",
         &CTaskGroup_GroupMenuCommand_Original},
        {R"(??_7CTaskListWnd@@6BCImpWndProc@@@)",
         &CTaskListWnd_vftable_CImpWndProc},
        {R"(??_7CTaskListWnd@@6BITaskListUI@@@)",
         &CTaskListWnd_vftable_ITaskListUI},
        {R"(?_HandleClick@CTaskListWnd@@IEAAXPEAUITaskBtnGroup@@HW4eCLICKACTION@1@HH@Z)",
         &CTaskListWnd__HandleClick_Original, CTaskListWnd__HandleClick_Hook},
        // {R"()", &CTaskListWnd_HandleExtendedUIClick_Original,
        //  CTaskListWnd_HandleExtendedUIClick_Hook, true},
        {R"(?IsVisibleOnCurrentVirtualDesktop@CTaskItem@@UEAA_NXZ)",
         &CTaskItem_IsVisibleOnCurrentVirtualDesktop_Original,
         CTaskItem_IsVisibleOnCurrentVirtualDesktop_Hook},
        {R"(?PostMessageW@CApi@@UEAAHPEAUHWND__@@I_K_J@Z)",
         &CApi_PostMessageW_Original, CApi_PostMessageW_Hook},
        {R"(?BringWindowToTop@CApi@@UEAAHPEAUHWND__@@@Z)",
         &CApi_BringWindowToTop_Original, CApi_BringWindowToTop_Hook},
        {R"(?ShowLivePreview@CTaskListWnd@@UEAAJPEAUITaskItem@@K@Z)",
         &CTaskListWnd_ShowLivePreview_Original},
        {R"(?GetWindow@CWindowTaskItem@@UEAAPEAUHWND__@@XZ)",
         &CWindowTaskItem_GetWindow_Original},
        {R"(?GetWindow@CImmersiveTaskItem@@UEAAPEAUHWND__@@XZ)",
         &CImmersiveTaskItem_GetWindow_Original},
        {R"(??_7CImmersiveTaskItem@@6BITaskItem@@@)",
         &CImmersiveTaskItem_vftable},
        {R"(?OnContextMenu@CTaskListWnd@@UEAAXUtagPOINT@@PEAUHWND__@@_NPEAUITaskGroup@@PEAUITaskItem@@@Z)",
         &CTaskListWnd_OnContextMenu_Original, CTaskListWnd_OnContextMenu_Hook},
        {R"(?DismissHoverUI@CTaskListWnd@@UEAAJH@Z)",
         &CTaskListWnd_DismissHoverUI_Original,
         CTaskListWnd_DismissHoverUI_Hook},
        {R"(?ThumbIndexFromPoint@CTaskListThumbnailWnd@@UEBAHAEBUtagPOINT@@@Z)",
         &CTaskListThumbnailWnd_ThumbIndexFromPoint_Original,
         CTaskListThumbnailWnd_ThumbIndexFromPoint_Hook},
        {R"(?v_WndProc@CTaskListWnd@@MEAA_JPEAUHWND__@@I_K_J@Z)",
         &CTaskListWnd_v_WndProc_Original, CTaskListWnd_v_WndProc_Hook},
        {R"(?_HandleContextMenu@CTaskListThumbnailWnd@@AEAAXUtagPOINT@@H@Z)",
         &CTaskListThumbnailWnd__HandleContextMenu_Original},
        {R"(?_RefreshThumbnail@CTaskListThumbnailWnd@@AEAAXH@Z)",
         &CTaskListThumbnailWnd__RefreshThumbnail_Original},
        {R"(?v_WndProc@CTaskListThumbnailWnd@@EEAA_JPEAUHWND__@@I_K_J@Z)",
         &CTaskListThumbnailWnd_v_WndProc_Original,
         CTaskListThumbnailWnd_v_WndProc_Hook},
        // For offsets:
        {R"(?CTaskListWnd_GetTaskFilterPtr@@YAPEAXPEAVCTaskListWnd@@@Z)",
         &CTaskListWnd_GetTaskFilterPtr},
        // No need for TaskItemThumbnail_ReportClicked_Original.
        // No need for TaskItem_ReportClicked_Original.
        // No need for TaskGroup_ReportClicked_Original.
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
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            g_taskbarViewDllLoaded = true;
            if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
                return FALSE;
            }
        } else {
            Wh_Log(L"Taskbar view module not loaded yet");
        }

        if (!HookTaskbarSymbols()) {
            return FALSE;
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

    HMODULE dwmapiModule =
        LoadLibraryEx(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (dwmapiModule) {
        pDwmpActivateLivePreview =
            (DwmpActivateLivePreview_t)GetProcAddress(dwmapiModule, (PCSTR)113);
    }

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
