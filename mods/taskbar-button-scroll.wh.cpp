// ==WindhawkMod==
// @id              taskbar-button-scroll
// @name            Taskbar minimize/restore on scroll
// @description     Minimize/restore by scrolling the mouse wheel over taskbar buttons and thumbnail previews (Windows 11 only)
// @version         1.0.7
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -loleaut32 -lole32 -lruntimeobject
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

Only Windows 11 version 22H2 or newer is currently supported. For older Windows
versions check out [7+ Taskbar Tweaker](https://tweaker.ramensoftware.com/).

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
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <commctrl.h>
#include <windowsx.h>

#undef GetCurrentTime

#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>

#include <atomic>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_set>

using namespace winrt::Windows::UI::Xaml;

struct {
    bool scrollOverTaskbarButtons;
    bool scrollOverThumbnailPreviews;
    bool maximizeAndRestore;
    bool reverseScrollingDirection;
} g_settings;

constexpr UINT_PTR kRefreshTaskbarTimer = 1731020327;

double g_invokingTaskListButtonAutomationInvokeMouseWheelDelta;
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

std::unordered_set<HWND> g_thumbnailWindows;

#pragma region offsets

void* CTaskListWnd__TaskCreated;

size_t OffsetFromAssembly(void* func,
                          size_t defValue,
                          std::string opcode = "mov",
                          int limit = 30) {
    // Example: mov rax, [rcx+0xE0]
    std::regex regex(
        opcode +
        R"( r(?:[a-z]{2}|\d{1,2}), \[r(?:[a-z]{2}|\d{1,2})\+(0x[0-9A-F]+)\])");

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

PVOID* EV_MM_TASKLIST_TASK_ITEM_FILTER(PVOID lp) {
    static size_t offset =
        OffsetFromAssembly(CTaskListWnd__TaskCreated, 0x268, "mov", 40);

    return (PVOID*)((DWORD_PTR)lp + offset);
}

#pragma endregion  // offsets

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

    if (!g_invokingTaskListButtonAutomationInvokeMouseWheelDelta) {
        return CTaskListWnd__HandleClick_Original(
            pThis, taskBtnGroup, taskItemIndex, clickAction, param4, param5);
    }

    short delta = static_cast<short>(
        g_invokingTaskListButtonAutomationInvokeMouseWheelDelta);

    if (g_lastScrollTarget == taskBtnGroup &&
        GetTickCount() - g_lastScrollTime < 1000 * 5) {
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

    if (command &&
        (g_lastScrollTarget != taskBtnGroup || command != g_lastScrollCommand ||
         GetTickCount() - g_lastScrollCommandTime >= 500)) {
        void* taskGroup = CTaskBtnGroup_GetGroup_Original(taskBtnGroup);
        if (taskGroup) {
            // Group types:
            // 1 - Single item or multiple uncombined items
            // 2 - Pinned item
            // 3 - Multiple combined items
            int groupType = CTaskBtnGroup_GetGroupType_Original(taskBtnGroup);
            if (groupType != 2) {
                g_groupMenuCommandThreadId = GetCurrentThreadId();
                g_groupMenuCommandTaskItem =
                    groupType == 3 ? nullptr
                                   : CTaskBtnGroup_GetTaskItem_Original(
                                         taskBtnGroup, taskItemIndex);

                CTaskGroup_GroupMenuCommand_Original(
                    taskGroup, *EV_MM_TASKLIST_TASK_ITEM_FILTER(pThis),
                    command);

                g_groupMenuCommandThreadId = 0;
                g_groupMenuCommandTaskItem = nullptr;
            }
        }

        g_lastScrollCommand = command;
        g_lastScrollCommandTime = GetTickCount();
    }

    g_lastScrollTarget = taskBtnGroup;
    g_lastScrollTime = GetTickCount();
    g_lastScrollDeltaRemainder = delta % WHEEL_DELTA;
}

BOOL CanMinimizeWindow(HWND hWnd) {
    if (IsIconic(hWnd) || !IsWindowEnabled(hWnd))
        return FALSE;

    long lWndStyle = GetWindowLong(hWnd, GWL_STYLE);
    if (!(lWndStyle & WS_MINIMIZEBOX))
        return FALSE;

    if ((lWndStyle & (WS_CAPTION | WS_SYSMENU)) != (WS_CAPTION | WS_SYSMENU))
        return TRUE;

    HMENU hSystemMenu = GetSystemMenu(hWnd, FALSE);
    if (!hSystemMenu)
        return FALSE;

    UINT uMenuState = GetMenuState(hSystemMenu, SC_MINIMIZE, MF_BYCOMMAND);
    if (uMenuState == (UINT)-1)
        return TRUE;

    return ((uMenuState & MF_DISABLED) == FALSE);
}

BOOL CanMaximizeWindow(HWND hWnd) {
    if (!IsWindowEnabled(hWnd))
        return FALSE;

    long lWndStyle = GetWindowLong(hWnd, GWL_STYLE);
    if (!(lWndStyle & WS_MAXIMIZEBOX))
        return FALSE;

    return TRUE;
}

BOOL CanRestoreWindow(HWND hWnd) {
    if (!IsWindowEnabled(hWnd))
        return FALSE;

    long lWndStyle = GetWindowLong(hWnd, GWL_STYLE);
    if (!(lWndStyle & WS_MAXIMIZEBOX))
        return FALSE;

    return TRUE;
}

void SwitchToWindow(HWND hWnd) {
    BOOL bRestore = FALSE;
    HWND hActiveWnd = hWnd;
    HWND hTempWnd = GetLastActivePopup(GetAncestor(hWnd, GA_ROOTOWNER));

    if (hTempWnd && hTempWnd != hWnd && IsWindowVisible(hTempWnd) &&
        IsWindowEnabled(hTempWnd)) {
        HWND hOwnerWnd = GetWindow(hTempWnd, GW_OWNER);

        while (hOwnerWnd && hOwnerWnd != hWnd)
            hOwnerWnd = GetWindow(hOwnerWnd, GW_OWNER);

        if (hOwnerWnd == hWnd) {
            bRestore = IsIconic(hWnd);
            hActiveWnd = hTempWnd;
        }
    }

    if (IsIconic(hActiveWnd) && !IsWindowEnabled(hActiveWnd)) {
        ShowWindowAsync(hWnd, SW_RESTORE);
    } else {
        SwitchToThisWindow(hActiveWnd, TRUE);
        if (bRestore)
            ShowWindowAsync(hWnd, SW_RESTORE);
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

using TaskListButton_AutomationInvoke_t = void(WINAPI*)(void* pThis);
TaskListButton_AutomationInvoke_t TaskListButton_AutomationInvoke_Original;

using TaskListButton_OnPointerWheelChanged_t = int(WINAPI*)(void* pThis,
                                                            void* pArgs);
TaskListButton_OnPointerWheelChanged_t
    TaskListButton_OnPointerWheelChanged_Original;
int TaskListButton_OnPointerWheelChanged_Hook(void* pThis, void* pArgs) {
    Wh_Log(L">");

    auto original = [&]() {
        return TaskListButton_OnPointerWheelChanged_Original(pThis, pArgs);
    };

    if (!g_settings.scrollOverTaskbarButtons) {
        return original();
    }

    winrt::Windows::Foundation::IInspectable taskListButton = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(
            winrt::guid_of<winrt::Windows::Foundation::IInspectable>(),
            winrt::put_abi(taskListButton));

    if (!taskListButton) {
        return original();
    }

    auto className = winrt::get_class_name(taskListButton);
    Wh_Log(L"%s", className.c_str());

    if (className != L"Taskbar.TaskListButton") {
        return original();
    }

    UIElement taskListButtonElement = taskListButton.as<UIElement>();

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return original();
    }

    double delta = args.GetCurrentPoint(taskListButtonElement)
                       .Properties()
                       .MouseWheelDelta();
    if (!delta) {
        return original();
    }

    // Allows to steal focus.
    INPUT input;
    ZeroMemory(&input, sizeof(INPUT));
    SendInput(1, &input, sizeof(INPUT));

    g_invokingTaskListButtonAutomationInvokeMouseWheelDelta = delta;
    TaskListButton_AutomationInvoke_Original(
        (BYTE*)winrt::get_abi(taskListButton) - 0x18);
    g_invokingTaskListButtonAutomationInvokeMouseWheelDelta = 0;

    args.Handled(true);
    return 0;
}

using ExtendedUIXamlRefresh___private_IsEnabled_t = bool(WINAPI*)(void* pThis);
ExtendedUIXamlRefresh___private_IsEnabled_t
    ExtendedUIXamlRefresh___private_IsEnabled_Original;
bool ExtendedUIXamlRefresh___private_IsEnabled_Hook(void* pThis) {
    // The flag breaks the AutomationInvoke functionality, disable it in this
    // flow.
    if (g_invokingTaskListButtonAutomationInvokeMouseWheelDelta) {
        Wh_Log(L">");
        return false;
    }

    return ExtendedUIXamlRefresh___private_IsEnabled_Original(pThis);
}

// wParam - TRUE to subclass, FALSE to unsubclass
// lParam - subclass data
UINT g_subclassRegisteredMsg = RegisterWindowMessage(
    L"Windhawk_SetWindowSubclassFromAnyThread_" WH_MOD_ID);

BOOL SetWindowSubclassFromAnyThread(HWND hWnd,
                                    SUBCLASSPROC pfnSubclass,
                                    UINT_PTR uIdSubclass,
                                    DWORD_PTR dwRefData) {
    struct SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM {
        SUBCLASSPROC pfnSubclass;
        UINT_PTR uIdSubclass;
        DWORD_PTR dwRefData;
        BOOL result;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return FALSE;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        return SetWindowSubclass(hWnd, pfnSubclass, uIdSubclass, dwRefData);
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) WINAPI -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == g_subclassRegisteredMsg && cwp->wParam) {
                    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM* param =
                        (SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM*)cwp->lParam;
                    param->result =
                        SetWindowSubclass(cwp->hwnd, param->pfnSubclass,
                                          param->uIdSubclass, param->dwRefData);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return FALSE;
    }

    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM param;
    param.pfnSubclass = pfnSubclass;
    param.uIdSubclass = uIdSubclass;
    param.dwRefData = dwRefData;
    param.result = FALSE;
    SendMessage(hWnd, g_subclassRegisteredMsg, TRUE, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return param.result;
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
        GetTickCount() - g_lastScrollTime < 1000 * 5) {
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
            g_noDismissHoverUIUntil = GetTickCount64() + 400;

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

using CTaskListThumbnailWnd__HandleContextMenu_t = void(WINAPI*)(void* pThis,
                                                                 POINT point,
                                                                 int param2);
CTaskListThumbnailWnd__HandleContextMenu_t
    CTaskListThumbnailWnd__HandleContextMenu_Original;

using CTaskListThumbnailWnd__RefreshThumbnail_t = void(WINAPI*)(void* pThis,
                                                                int index);
CTaskListThumbnailWnd__RefreshThumbnail_t
    CTaskListThumbnailWnd__RefreshThumbnail_Original;

using CTaskListThumbnailWnd_GetHoverIndex_t = int(WINAPI*)(void* pThis);
CTaskListThumbnailWnd_GetHoverIndex_t
    CTaskListThumbnailWnd_GetHoverIndex_Original;

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
    INPUT input;
    ZeroMemory(&input, sizeof(INPUT));
    SendInput(1, &input, sizeof(INPUT));

    POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

    g_invokingContextMenuWParam = wParam;
    g_thumbnailContextMenuLastIndex = 0;
    CTaskListThumbnailWnd__HandleContextMenu_Original(thumbnail, pt, 0);
    g_invokingContextMenuWParam = 0;

    SetTimer(hWnd, kRefreshTaskbarTimer, 200, 0);

    return true;
}

LRESULT CALLBACK ThumbnailWindowSubclassProc(HWND hWnd,
                                             UINT uMsg,
                                             WPARAM wParam,
                                             LPARAM lParam,
                                             UINT_PTR uIdSubclass,
                                             DWORD_PTR dwRefData) {
    LRESULT result = 0;

    if (uMsg == WM_NCDESTROY || (uMsg == g_subclassRegisteredMsg && !wParam)) {
        RemoveWindowSubclass(hWnd, ThumbnailWindowSubclassProc, 0);
    }

    switch (uMsg) {
        case WM_MOUSEWHEEL:
            if (!OnThumbnailWheelScroll(hWnd, uMsg, wParam, lParam)) {
                result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }
            break;

        case WM_TIMER:
            switch (wParam) {
                case kRefreshTaskbarTimer: {
                    void* thumbnail = (void*)GetWindowLongPtr(hWnd, 0);
                    CTaskListThumbnailWnd__RefreshThumbnail_Original(
                        thumbnail, g_thumbnailContextMenuLastIndex);
                    result = 0;
                    break;
                }

                default: {
                    result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
                    break;
                }
            }
            break;

        case WM_NCDESTROY:
            g_thumbnailWindows.erase(hWnd);

            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            break;

        default:
            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            break;
    }

    return result;
}

void SubclassThumbnailWindow(HWND hWnd) {
    SetWindowSubclassFromAnyThread(hWnd, ThumbnailWindowSubclassProc, 0, 0);
}

void UnsubclassThumbnailWindow(HWND hWnd) {
    SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0);
}

void HandleIdentifiedThumbnailWindow(HWND hWnd) {
    g_thumbnailWindows.insert(hWnd);
    SubclassThumbnailWindow(hWnd);
}

void FindCurrentProcessThumbnailWindows(HWND hTaskbarWnd) {
    DWORD dwProcessId;
    DWORD dwThreadId = GetWindowThreadProcessId(hTaskbarWnd, &dwProcessId);

    EnumThreadWindows(
        dwThreadId,
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            if (_wcsicmp(szClassName, L"TaskListThumbnailWnd") == 0) {
                g_thumbnailWindows.insert(hWnd);
            }

            return TRUE;
        },
        0);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);

    if (!hWnd) {
        return hWnd;
    }

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"TaskListThumbnailWnd") == 0) {
        Wh_Log(L"Thumbnail window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedThumbnailWindow(hWnd);
    }

    return hWnd;
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
                                           LPCWSTR lpClassName,
                                           LPCWSTR lpWindowName,
                                           DWORD dwStyle,
                                           int X,
                                           int Y,
                                           int nWidth,
                                           int nHeight,
                                           HWND hWndParent,
                                           HMENU hMenu,
                                           HINSTANCE hInstance,
                                           PVOID lpParam,
                                           DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
                                    LPCWSTR lpClassName,
                                    LPCWSTR lpWindowName,
                                    DWORD dwStyle,
                                    int X,
                                    int Y,
                                    int nWidth,
                                    int nHeight,
                                    HWND hWndParent,
                                    HMENU hMenu,
                                    HINSTANCE hInstance,
                                    PVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd) {
        return hWnd;
    }

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"TaskListThumbnailWnd") == 0) {
        Wh_Log(L"Thumbnail window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedThumbnailWindow(hWnd);
    }

    return hWnd;
}

void LoadSettings() {
    g_settings.scrollOverTaskbarButtons =
        Wh_GetIntSetting(L"scrollOverTaskbarButtons");
    g_settings.scrollOverThumbnailPreviews =
        Wh_GetIntSetting(L"scrollOverThumbnailPreviews");
    g_settings.maximizeAndRestore = Wh_GetIntSetting(L"maximizeAndRestore");
    g_settings.reverseScrollingDirection =
        Wh_GetIntSetting(L"reverseScrollingDirection");
}

bool HookTaskbarViewDllSymbols() {
    WCHAR dllPath[MAX_PATH];
    if (!GetWindowsDirectory(dllPath, ARRAYSIZE(dllPath))) {
        Wh_Log(L"GetWindowsDirectory failed");
        return false;
    }

    wcscat_s(
        dllPath, MAX_PATH,
        LR"(\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\Taskbar.View.dll)");

    HMODULE module =
        LoadLibraryEx(dllPath, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!module) {
        Wh_Log(L"Taskbar view module couldn't be loaded");
        return false;
    }

    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::AutomationInvoke(void))"},
            &TaskListButton_AutomationInvoke_Original,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerWheelChanged(void *))"},
            &TaskListButton_OnPointerWheelChanged_Original,
            TaskListButton_OnPointerWheelChanged_Hook,
        },
        {
            {LR"(public: bool __cdecl wil::details::FeatureImpl<struct __WilExternalFeatureTraits_Feature_ExtendedUIXamlRefresh>::__private_IsEnabled(void))"},
            &ExtendedUIXamlRefresh___private_IsEnabled_Original,
            ExtendedUIXamlRefresh___private_IsEnabled_Hook,
            true,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
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
            {LR"(protected: void __cdecl CTaskListWnd::_HandleClick(struct ITaskBtnGroup *,int,enum CTaskListWnd::eCLICKACTION,int,int))"},
            &CTaskListWnd__HandleClick_Original,
            CTaskListWnd__HandleClick_Hook,
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
            {LR"(public: virtual long __cdecl CTaskListWnd::ShowLivePreview(struct ITaskItem *,unsigned long))"},
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
            {LR"(private: void __cdecl CTaskListThumbnailWnd::_HandleContextMenu(struct tagPOINT,int))"},
            &CTaskListThumbnailWnd__HandleContextMenu_Original,
        },
        {
            {LR"(private: void __cdecl CTaskListThumbnailWnd::_RefreshThumbnail(int))"},
            &CTaskListThumbnailWnd__RefreshThumbnail_Original,
        },
        // For offsets:
        {
            {LR"(protected: long __cdecl CTaskListWnd::_TaskCreated(struct ITaskGroup *,struct ITaskItem *,int))"},
            &CTaskListWnd__TaskCreated,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarViewDllSymbols()) {
        return FALSE;
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

    HMODULE user32Module = LoadLibrary(L"user32.dll");
    if (user32Module) {
        void* pCreateWindowInBand =
            (void*)GetProcAddress(user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            Wh_SetFunctionHook(pCreateWindowInBand,
                               (void*)CreateWindowInBand_Hook,
                               (void**)&CreateWindowInBand_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    DWORD dwProcessId;
    DWORD dwCurrentProcessId = GetCurrentProcessId();

    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    if (hTaskbarWnd && GetWindowThreadProcessId(hTaskbarWnd, &dwProcessId) &&
        dwProcessId == dwCurrentProcessId) {
        FindCurrentProcessThumbnailWindows(hTaskbarWnd);
        for (HWND hWnd : g_thumbnailWindows) {
            Wh_Log(L"Thumbnail window found: %08X", (DWORD)(ULONG_PTR)hWnd);
            SubclassThumbnailWindow(hWnd);
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    for (HWND hWnd : g_thumbnailWindows) {
        UnsubclassThumbnailWindow(hWnd);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
