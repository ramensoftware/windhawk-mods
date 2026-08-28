// ==WindhawkMod==
// @id              native-taskbar-ungrouping
// @name            Native Taskbar Ungrouping
// @description     Give every window its own native Windows 11 taskbar button without changing the taskbar's native look.
// @version         2.1.0
// @author          kamkie
// @github          https://github.com/kamkie
// @homepage        https://github.com/kamkie/native-taskbar-ungrouping
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32
// @license         GPL-3.0
// ==/WindhawkMod==

// Derived from taskbar-grouping 1.3.10 by Michael Maltsev (m417z), GPL-3.0:
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-grouping.wh.cpp
// This version is limited to the native Windows 11 vertical taskbar, forces
// its compact combine presentation in memory, removes configurable grouping,
// Windows 10 and ExplorerPatcher paths, and always keeps split buttons adjacent.

// ==WindhawkModReadme==
/*
# Native Taskbar Ungrouping

Keeps Windows in its native **combine** presentation mode — preserving the
native icon-only taskbar layout, including the compact 48-DIP vertical
taskbar — while splitting the underlying task model so each newly opened
window receives a separate native button.

The mod doesn't modify taskbar XAML, labels, frame dimensions, indicators,
progress, badges, padding, Start, tray, or appbar geometry. Windows continues
to own all taskbar presentation and interaction.

Behavior is intentionally fixed:

- Running windows replace matching pinned items.
- Each newly opened window is assigned a separate task group and placed next to
  windows from the same application.
- Application icons, pins, launching, and jump lists retain their normal
  identity.
- No exclusions, custom groups, placement modes, or cosmetic settings.
- The old/ExplorerPatcher taskbar is not targeted.

After enabling the mod, close and reopen existing application windows so they
are resolved into separate groups.

This mod is derived from **Disable grouping on the taskbar** 1.3.10 by Michael
Maltsev (`m417z`). This version removes its settings and legacy-taskbar paths,
activates only for Microsoft's native vertical taskbar, and keeps the compact
combine presentation automatically. Both projects are licensed under GPL-3.0.

Tested on Windows 11 25H2 build 26200.9278 (x64).
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <shellapi.h>
#include <shlobj.h>
#include <winrt/base.h>

#include <atomic>
#include <functional>

struct RESOLVEDWINDOW {
    HWND hButtonWnd;
    WCHAR szPathStr[MAX_PATH];
    WCHAR szAppIdStr[MAX_PATH];
    ITEMIDLIST* pAppItemIdList;
    HWND hThumbInsertBeforeWnd;
    HWND hThumbParentWnd;
    BOOL bSetPinnableAndLaunchable;
    BOOL bSetThumbFlag;
};

std::atomic<bool> g_unloading;
std::atomic<int> g_lastTaskbarEdge{-1};
std::atomic<bool> g_loggedCombineOverride;

bool g_inTaskBandLaunch;
bool g_inUpdateItemIcon;
bool g_inTaskBtnGroupGetIcon;
bool g_inGetJumpViewParams;
bool g_inShowJumpView;
bool g_inFindTaskBtnGroup;
PVOID g_findTaskBtnGroup_TaskGroupSentinel =
    &g_findTaskBtnGroup_TaskGroupSentinel;
std::function<bool(PVOID)> g_findTaskBtnGroup_Callback;
std::atomic<DWORD> g_cTaskListWnd__CreateTBGroup_ThreadId;
bool g_disableGetLauncherName;
std::atomic<DWORD> g_compareStringOrdinalHookThreadId;
bool g_compareStringOrdinalIgnoreSuffix;
bool g_compareStringOrdinalAnySuffixEqual;
std::atomic<DWORD> g_doingPinnedItemSwapThreadId;
void* g_doingPinnedItemSwapFromTaskGroup;
void* g_doingPinnedItemSwapToTaskGroup;
int g_doingPinnedItemSwapIndex = -1;

constexpr size_t ITaskListUIOffset = 0x28;

winrt::com_ptr<IUnknown> GetTaskGroupWithoutSuffix(
    PVOID taskGroup,
    IUnknown** taskItem = nullptr);

// constexpr size_t kAppIdSuffixLen = 13;

bool AddAppIdSuffix(WCHAR appId[MAX_PATH], WCHAR type, DWORD id) {
    size_t len = wcslen(appId);
    size_t newLen = len + 13;
    if (newLen >= MAX_PATH) {
        return false;
    }

    swprintf(appId + len, L"~Wh~%c%08X", type, id);
    return true;
}

PCWSTR FindAppIdSuffix(PCWSTR appId) {
    auto isUpperHex = [](PCWSTR start, PCWSTR end) {
        for (PCWSTR p = start; p != end; p++) {
            if ((*p < '0' || *p > '9') && (*p < 'A' || *p > 'F')) {
                return false;
            }
        }
        return true;
    };

    size_t len = wcslen(appId);
    if (len <= 13 || appId[len - 13] != L'~' || appId[len - 12] != L'W' ||
        appId[len - 11] != L'h' || appId[len - 10] != L'~' ||
        !isUpperHex(&appId[len - 8], &appId[len])) {
        return nullptr;
    }

    return appId + len - 13;
}

bool RemoveAppIdSuffix(WCHAR appIdStripped[MAX_PATH], PCWSTR appIdWithSuffix) {
    PCWSTR suffix = FindAppIdSuffix(appIdWithSuffix);
    if (!suffix) {
        return false;
    }

    wcsncpy_s(appIdStripped, MAX_PATH, appIdWithSuffix,
              suffix - appIdWithSuffix);
    return true;
}

bool IsNativeVerticalTaskbar() {
    APPBARDATA appBarData{.cbSize = sizeof(APPBARDATA)};
    if (!SHAppBarMessage(ABM_GETTASKBARPOS, &appBarData)) {
        return false;
    }

    int edge = static_cast<int>(appBarData.uEdge);
    if (g_lastTaskbarEdge.exchange(edge) != edge) {
        Wh_Log(L"Native taskbar edge: %d", edge);
        g_loggedCombineOverride = false;
    }

    return appBarData.uEdge == ABE_LEFT || appBarData.uEdge == ABE_RIGHT;
}

using CTaskGroup_GetNumItems_t = int(WINAPI*)(PVOID pThis);
CTaskGroup_GetNumItems_t CTaskGroup_GetNumItems_Original;

using CTaskGroup_SetAppID_t = HRESULT(WINAPI*)(PVOID pThis, PCWSTR appId);
CTaskGroup_SetAppID_t CTaskGroup_SetAppID_Original;

using CTaskGroup_GetFlags_t = DWORD(WINAPI*)(PVOID pThis);
CTaskGroup_GetFlags_t CTaskGroup_GetFlags_Original;

using CTaskGroup_UpdateFlags_t = HRESULT(WINAPI*)(PVOID pThis,
                                                  DWORD updateMask,
                                                  DWORD newFlags);
CTaskGroup_UpdateFlags_t CTaskGroup_UpdateFlags_Original;

using CTaskGroup_GetTitleText_t = HRESULT(WINAPI*)(PVOID pThis,
                                                   PVOID taskItem,
                                                   WCHAR* buffer,
                                                   int bufferSize);
CTaskGroup_GetTitleText_t CTaskGroup_GetTitleText_Original;

using CTaskGroup_SetTip_t = HRESULT(WINAPI*)(PVOID pThis, PCWSTR tip);
CTaskGroup_SetTip_t CTaskGroup_SetTip_Original;

using CTaskGroup_GetIconId_t = HRESULT(WINAPI*)(PVOID pThis,
                                                PVOID taskItem,
                                                int* id);
CTaskGroup_GetIconId_t CTaskGroup_GetIconId_Original;

using CTaskGroup_SetIconId_t = HRESULT(WINAPI*)(PVOID pThis,
                                                PVOID taskItem,
                                                int id);
CTaskGroup_SetIconId_t CTaskGroup_SetIconId_Original;

using CTaskGroup_DoesWindowMatch_t =
    HRESULT(WINAPI*)(PVOID pThis,
                     HWND hWnd,
                     const ITEMIDLIST* idList,
                     PCWSTR appId,
                     int* windowMatchConfidence,
                     PVOID* taskItem);
CTaskGroup_DoesWindowMatch_t CTaskGroup_DoesWindowMatch_Original;

using CTaskBtnGroup_GetGroupType_t = int(WINAPI*)(PVOID pThis);
CTaskBtnGroup_GetGroupType_t CTaskBtnGroup_GetGroupType_Original;

using CTaskBand__MatchWindow_t = HRESULT(WINAPI*)(PVOID pThis,
                                                  HWND hWnd,
                                                  const ITEMIDLIST* idList,
                                                  PCWSTR appId,
                                                  int windowMatchConfidence,
                                                  PVOID* taskGroup,
                                                  PVOID* taskItem);
CTaskBand__MatchWindow_t CTaskBand__MatchWindow_Original;

void ProcessResolvedWindow(PVOID pThis, RESOLVEDWINDOW* resolvedWindow) {
    Wh_Log(L"==========");
    Wh_Log(L"hButtonWnd=%08X", resolvedWindow->hButtonWnd);
    Wh_Log(L"szPathStr=%s", resolvedWindow->szPathStr);
    Wh_Log(L"szAppIdStr=%s", resolvedWindow->szAppIdStr);
    Wh_Log(L"pAppItemIdList=%p", resolvedWindow->pAppItemIdList);
    Wh_Log(L"hThumbInsertBeforeWnd=%08X",
           resolvedWindow->hThumbInsertBeforeWnd);
    Wh_Log(L"hThumbParentWnd=%08X", resolvedWindow->hThumbParentWnd);
    Wh_Log(L"bSetPinnableAndLaunchable=%d",
           resolvedWindow->bSetPinnableAndLaunchable);
    Wh_Log(L"bSetThumbFlag=%d", resolvedWindow->bSetThumbFlag);

    if (!IsNativeVerticalTaskbar()) {
        return;
    }

    winrt::com_ptr<IUnknown> taskGroupMatched;
    winrt::com_ptr<IUnknown> taskItemMatched;
    HRESULT hr = CTaskBand__MatchWindow_Original(
        pThis, resolvedWindow->hButtonWnd, resolvedWindow->pAppItemIdList,
        resolvedWindow->szAppIdStr, 1, taskGroupMatched.put_void(),
        taskItemMatched.put_void());
    if (FAILED(hr) ||
        CTaskGroup_GetNumItems_Original(taskGroupMatched.get()) == 0) {
        // The first window should keep the original AppID and replace its pin.
        return;
    }

    if (resolvedWindow->pAppItemIdList) {
        ILFree(resolvedWindow->pAppItemIdList);
        resolvedWindow->pAppItemIdList = nullptr;
    }

    bool appIdSuffixAdded;
    if (resolvedWindow->hButtonWnd) {
        appIdSuffixAdded = AddAppIdSuffix(
            resolvedWindow->szAppIdStr, L'w',
            static_cast<DWORD>(reinterpret_cast<DWORD_PTR>(
                resolvedWindow->hButtonWnd)));
    } else {
        static DWORD counter = GetTickCount();
        appIdSuffixAdded =
            AddAppIdSuffix(resolvedWindow->szAppIdStr, L'c', ++counter);
    }

    if (appIdSuffixAdded) {
        Wh_Log(L"New AppId: %s", resolvedWindow->szAppIdStr);
    } else {
        Wh_Log(L"AppId is too long: %s", resolvedWindow->szAppIdStr);
    }
}

using CTaskBand_v_WndProc_t = LRESULT(
    WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CTaskBand_v_WndProc_t CTaskBand_v_WndProc_Original;
LRESULT WINAPI CTaskBand_v_WndProc_Hook(void* pThis,
                                        HWND hWnd,
                                        UINT Msg,
                                        WPARAM wParam,
                                        LPARAM lParam) {
    LRESULT ret;

    // Calls CTaskBand::_HandleWindowResolved.
    if (Msg == 0x44A) {
        Wh_Log(L">");

        g_compareStringOrdinalHookThreadId = GetCurrentThreadId();
        g_compareStringOrdinalIgnoreSuffix = true;

        ret = CTaskBand_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);

        g_compareStringOrdinalHookThreadId = 0;
        g_compareStringOrdinalIgnoreSuffix = false;
    } else {
        ret = CTaskBand_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);
    }

    return ret;
}

using CTaskBand__HandleItemResolved_t =
    void(WINAPI*)(PVOID pThis,
                  RESOLVEDWINDOW* resolvedWindow,
                  PVOID taskListUI,
                  PVOID taskGroup,
                  PVOID taskItem);
CTaskBand__HandleItemResolved_t CTaskBand__HandleItemResolved_Original;
void WINAPI CTaskBand__HandleItemResolved_Hook(PVOID pThis,
                                               RESOLVEDWINDOW* resolvedWindow,
                                               PVOID taskListUI,
                                               PVOID taskGroup,
                                               PVOID taskItem) {
    Wh_Log(L">");

    // Reset flags set by CTaskBand_v_WndProc_Hook.
    g_compareStringOrdinalHookThreadId = 0;
    g_compareStringOrdinalIgnoreSuffix = false;

    ProcessResolvedWindow(pThis, resolvedWindow);

    CTaskBand__HandleItemResolved_Original(pThis, resolvedWindow, taskListUI,
                                           taskGroup, taskItem);
}

using CTaskBand__Launch_t = HRESULT(WINAPI*)(PVOID pThis);
CTaskBand__Launch_t CTaskBand__Launch_Original;
HRESULT WINAPI CTaskBand__Launch_Hook(PVOID pThis) {
    Wh_Log(L">");

    g_inTaskBandLaunch = true;
    HRESULT ret = CTaskBand__Launch_Original(pThis);
    g_inTaskBandLaunch = false;

    return ret;
}

using CTaskGroup_GetAppID_t = PCWSTR(WINAPI*)(PVOID pThis);
CTaskGroup_GetAppID_t CTaskGroup_GetAppID_Original;
PCWSTR WINAPI CTaskGroup_GetAppID_Hook(PVOID pThis) {
    Wh_Log(L">");

    if (g_inUpdateItemIcon || g_inShowJumpView) {
        winrt::com_ptr<IUnknown> taskGroupWithoutSuffix =
            GetTaskGroupWithoutSuffix(pThis);
        if (taskGroupWithoutSuffix) {
            return CTaskGroup_GetAppID_Original(taskGroupWithoutSuffix.get());
        }
    }

    return CTaskGroup_GetAppID_Original(pThis);
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

PVOID GetTaskBand() {
    static PVOID taskBand = nullptr;
    if (taskBand) {
        return taskBand;
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
        if (hTaskSwWnd) {
            taskBand = (PVOID)GetWindowLongPtr(hTaskSwWnd, 0);
        }
    }

    return taskBand;
}

winrt::com_ptr<IUnknown> GetTaskGroupWithoutSuffix(
    PVOID taskGroup,
    IUnknown** taskItem /*= nullptr*/) {
    PVOID taskBand = GetTaskBand();
    if (!taskBand) {
        return nullptr;
    }

    PCWSTR appId = CTaskGroup_GetAppID_Original(taskGroup);
    if (!appId) {
        return nullptr;
    }

    WCHAR appIdOriginal[MAX_PATH];
    if (!RemoveAppIdSuffix(appIdOriginal, appId)) {
        return nullptr;
    }

    winrt::com_ptr<IUnknown> taskGroupMatched;
    winrt::com_ptr<IUnknown> taskItemMatched;
    HRESULT hr = CTaskBand__MatchWindow_Original(
        taskBand, nullptr, nullptr, appIdOriginal, 1,
        taskGroupMatched.put_void(), taskItemMatched.put_void());
    if (FAILED(hr)) {
        return nullptr;
    }

    if (taskItem) {
        *taskItem = taskItemMatched.detach();
    }

    return taskGroupMatched;
}

using CTaskGroup_IsImmersiveGroup_t = bool(WINAPI*)(PVOID pThis);
CTaskGroup_IsImmersiveGroup_t CTaskGroup_IsImmersiveGroup_Original;
bool WINAPI CTaskGroup_IsImmersiveGroup_Hook(PVOID pThis) {
    Wh_Log(L">");

    if (g_inUpdateItemIcon) {
        winrt::com_ptr<IUnknown> taskGroupWithoutSuffix =
            GetTaskGroupWithoutSuffix(pThis);
        if (taskGroupWithoutSuffix) {
            return CTaskGroup_IsImmersiveGroup_Original(
                taskGroupWithoutSuffix.get());
        }
    }

    return CTaskGroup_IsImmersiveGroup_Original(pThis);
}

using CTaskGroup_GetApplicationIDList_t = ITEMIDLIST*(WINAPI*)(PVOID pThis);
CTaskGroup_GetApplicationIDList_t CTaskGroup_GetApplicationIDList_Original;

using CTaskGroup_GetShortcutIDList_t = const ITEMIDLIST*(WINAPI*)(PVOID pThis);
CTaskGroup_GetShortcutIDList_t CTaskGroup_GetShortcutIDList_Original;
const ITEMIDLIST* WINAPI CTaskGroup_GetShortcutIDList_Hook(PVOID pThis) {
    // Wh_Log(L">");

    // Fixes launching a new instance on middle click or Shift+click for some
    // apps. Actually I think that might be a Windows bug.
    if (g_inTaskBandLaunch) {
        return CTaskGroup_GetApplicationIDList_Original(pThis);
    }

    if (g_inTaskBtnGroupGetIcon && CTaskGroup_GetNumItems_Original(pThis) > 0) {
        winrt::com_ptr<IUnknown> taskGroupWithoutSuffix =
            GetTaskGroupWithoutSuffix(pThis);

        if (taskGroupWithoutSuffix && CTaskGroup_IsImmersiveGroup_Original(
                                          taskGroupWithoutSuffix.get())) {
            return nullptr;
        }

        if (taskGroupWithoutSuffix) {
            return CTaskGroup_GetShortcutIDList_Original(
                taskGroupWithoutSuffix.get());
        }
    }

    return CTaskGroup_GetShortcutIDList_Original(pThis);
}

using CTaskGroup_SetShortcutIDList_t =
    HRESULT(WINAPI*)(PVOID pThis, const ITEMIDLIST* itList);
CTaskGroup_SetShortcutIDList_t CTaskGroup_SetShortcutIDList_Original;

using CTaskGroup_GetIconResource_t = PCWSTR(WINAPI*)(PVOID pThis);
CTaskGroup_GetIconResource_t CTaskGroup_GetIconResource_Original;
PCWSTR WINAPI CTaskGroup_GetIconResource_Hook(PVOID pThis) {
    // Wh_Log(L">");

    winrt::com_ptr<IUnknown> taskGroupWithoutSuffix =
        GetTaskGroupWithoutSuffix(pThis);
    if (taskGroupWithoutSuffix) {
        return CTaskGroup_GetIconResource_Original(
            taskGroupWithoutSuffix.get());
    }

    return CTaskGroup_GetIconResource_Original(pThis);
}

using CTaskBand__UpdateItemIcon_t = void(WINAPI*)(PVOID pThis,
                                                  PVOID taskGroup,
                                                  PVOID taskItem);
CTaskBand__UpdateItemIcon_t CTaskBand__UpdateItemIcon_Original;
void WINAPI CTaskBand__UpdateItemIcon_Hook(PVOID pThis,
                                           PVOID taskGroup,
                                           PVOID taskItem) {
    Wh_Log(L">");

    g_inUpdateItemIcon = true;
    CTaskBand__UpdateItemIcon_Original(pThis, taskGroup, taskItem);
    g_inUpdateItemIcon = false;
}

using CTaskBand_Launch_t = HRESULT(WINAPI*)(PVOID pThis,
                                            PVOID taskGroup,
                                            PVOID point,
                                            int launchFromTaskbarOptions);
CTaskBand_Launch_t CTaskBand_Launch_Original;
HRESULT WINAPI CTaskBand_Launch_Hook(PVOID pThis,
                                     PVOID taskGroup,
                                     PVOID point,
                                     int launchFromTaskbarOptions) {
    Wh_Log(L">");

    winrt::com_ptr<IUnknown> taskGroupWithoutSuffix =
        GetTaskGroupWithoutSuffix(taskGroup);
    if (taskGroupWithoutSuffix) {
        return CTaskBand_Launch_Original(pThis, taskGroupWithoutSuffix.get(),
                                         point, launchFromTaskbarOptions);
    }

    return CTaskBand_Launch_Original(pThis, taskGroup, point,
                                     launchFromTaskbarOptions);
}

using CTaskGroup_GetLauncherName_t = HRESULT(WINAPI*)(PVOID pThis,
                                                      LPWSTR* ppwsz);
CTaskGroup_GetLauncherName_t CTaskGroup_GetLauncherName_Original;
HRESULT WINAPI CTaskGroup_GetLauncherName_Hook(PVOID pThis, LPWSTR* ppwsz) {
    Wh_Log(L">");

    if (g_disableGetLauncherName) {
        return E_FAIL;
    }

    return CTaskGroup_GetLauncherName_Original(pThis, ppwsz);
}

using CTaskListWnd__GetJumpViewParams_t = HRESULT(WINAPI*)(PVOID pThis,
                                                           PVOID taskBtnGroup,
                                                           PVOID taskItem,
                                                           int param3,
                                                           bool param4,
                                                           PVOID param5);
CTaskListWnd__GetJumpViewParams_t CTaskListWnd__GetJumpViewParams_Original;
HRESULT WINAPI CTaskListWnd__GetJumpViewParams_Hook(PVOID pThis,
                                                    PVOID taskBtnGroup,
                                                    PVOID taskItem,
                                                    int param3,
                                                    bool param4,
                                                    PVOID param5) {
    Wh_Log(L">");

    g_inGetJumpViewParams = true;
    HRESULT ret = CTaskListWnd__GetJumpViewParams_Original(
        pThis, taskBtnGroup, taskItem, param3, param4, param5);
    g_inGetJumpViewParams = false;

    return ret;
}

using CTaskListWnd_ShowJumpView_t = HRESULT(WINAPI*)(PVOID pThis,
                                                     PVOID taskGroup,
                                                     PVOID taskItem,
                                                     bool param3);
CTaskListWnd_ShowJumpView_t CTaskListWnd_ShowJumpView_Original;
HRESULT WINAPI CTaskListWnd_ShowJumpView_Hook(PVOID pThis,
                                              PVOID taskGroup,
                                              PVOID taskItem,
                                              bool param3) {
    Wh_Log(L">");

    g_inShowJumpView = true;
    HRESULT ret =
        CTaskListWnd_ShowJumpView_Original(pThis, taskGroup, taskItem, param3);
    g_inShowJumpView = false;

    return ret;
}

using CTaskBtnGroup_GetIcon_t = HRESULT(WINAPI*)(PVOID pThis,
                                                 PVOID taskItem,
                                                 HICON** icon);
CTaskBtnGroup_GetIcon_t CTaskBtnGroup_GetIcon_Original;
HRESULT WINAPI CTaskBtnGroup_GetIcon_Hook(PVOID pThis,
                                          PVOID taskItem,
                                          HICON** icon) {
    Wh_Log(L">");

    g_inTaskBtnGroupGetIcon = true;
    HRESULT ret = CTaskBtnGroup_GetIcon_Original(pThis, taskItem, icon);
    g_inTaskBtnGroupGetIcon = false;

    return ret;
}

using CTaskBtnGroup_GetGroup_t = PVOID(WINAPI*)(PVOID pThis);
CTaskBtnGroup_GetGroup_t CTaskBtnGroup_GetGroup_Original;
PVOID WINAPI CTaskBtnGroup_GetGroup_Hook(PVOID pThis) {
    // Wh_Log(L">");

    if (g_inFindTaskBtnGroup) {
        if (g_findTaskBtnGroup_Callback(pThis)) {
            return g_findTaskBtnGroup_TaskGroupSentinel;
        }

        return nullptr;
    }

    PVOID taskGroup = CTaskBtnGroup_GetGroup_Original(pThis);

    if (g_inGetJumpViewParams) {
        winrt::com_ptr<IUnknown> taskGroupWithoutSuffix =
            GetTaskGroupWithoutSuffix(taskGroup);
        if (taskGroupWithoutSuffix) {
            return taskGroupWithoutSuffix.get();
        }
    }

    return taskGroup;
}

using CTaskListWnd__GetTBGroupFromGroup_t = PVOID(WINAPI*)(PVOID pThis,
                                                           PVOID taskGroup,
                                                           int* foundIndex);
CTaskListWnd__GetTBGroupFromGroup_t CTaskListWnd__GetTBGroupFromGroup_Original;

PVOID FindTaskBtnGroup(PVOID taskList,
                       std::function<bool(PVOID)> callback,
                       int* foundIndex = nullptr) {
    g_inFindTaskBtnGroup = true;
    g_findTaskBtnGroup_Callback = std::move(callback);

    PVOID taskBtnGroup = CTaskListWnd__GetTBGroupFromGroup_Original(
        taskList, g_findTaskBtnGroup_TaskGroupSentinel, foundIndex);

    g_findTaskBtnGroup_Callback = nullptr;
    g_inFindTaskBtnGroup = false;

    return taskBtnGroup;
}

using CTaskListWnd_IsOnPrimaryTaskband_t = BOOL(WINAPI*)(PVOID pThis);
CTaskListWnd_IsOnPrimaryTaskband_t CTaskListWnd_IsOnPrimaryTaskband_Original;

using CTaskListWnd__CreateTBGroup_t = PVOID(WINAPI*)(PVOID pThis,
                                                     PVOID taskGroup,
                                                     int index);
CTaskListWnd__CreateTBGroup_t CTaskListWnd__CreateTBGroup_Original;
PVOID WINAPI CTaskListWnd__CreateTBGroup_Hook(PVOID pThis,
                                              PVOID taskGroup,
                                              int index) {
    Wh_Log(L">");

    g_cTaskListWnd__CreateTBGroup_ThreadId = GetCurrentThreadId();

    PVOID ret = CTaskListWnd__CreateTBGroup_Original(pThis, taskGroup, index);

    g_cTaskListWnd__CreateTBGroup_ThreadId = 0;

    return ret;
}

using DPA_InsertPtr_t = decltype(&DPA_InsertPtr);
DPA_InsertPtr_t DPA_InsertPtr_Original;
int WINAPI DPA_InsertPtr_Hook(HDPA hdpa, int i, void* p) {
    if (g_doingPinnedItemSwapThreadId == GetCurrentThreadId()) {
        Wh_Log(L">");

        if (g_doingPinnedItemSwapIndex != -1) {
            PVOID taskGroup = CTaskBtnGroup_GetGroup_Original(p);
            if (taskGroup && taskGroup == g_doingPinnedItemSwapToTaskGroup) {
                i = g_doingPinnedItemSwapIndex;
            }
        }

        return DPA_InsertPtr_Original(hdpa, i, p);
    }

    auto original = [=]() { return DPA_InsertPtr_Original(hdpa, i, p); };

    if (g_cTaskListWnd__CreateTBGroup_ThreadId != GetCurrentThreadId()) {
        return original();
    }

    Wh_Log(L">");

    if (i != DA_LAST || !p) {
        return original();
    }

    PVOID taskGroup = CTaskBtnGroup_GetGroup_Original(p);
    if (!taskGroup) {
        return original();
    }

    const ITEMIDLIST* idList = CTaskGroup_GetShortcutIDList_Original(taskGroup);
    PCWSTR appId = CTaskGroup_GetAppID_Original(taskGroup);

    int lastMatchIndex = DA_LAST;

    int count = DPA_GetPtrCount(hdpa);
    for (int i = 0; i < count; i++) {
        PVOID taskBtnGroupIter = DPA_GetPtr(hdpa, i);
        if (!taskBtnGroupIter) {
            continue;
        }

        PVOID taskGroupIter = CTaskBtnGroup_GetGroup_Original(taskBtnGroupIter);
        if (!taskGroupIter) {
            continue;
        }

        g_compareStringOrdinalHookThreadId = GetCurrentThreadId();
        g_compareStringOrdinalIgnoreSuffix = true;

        int windowMatchConfidence;
        winrt::com_ptr<IUnknown> taskItemMatched;
        HRESULT hr = CTaskGroup_DoesWindowMatch_Original(
            taskGroupIter, nullptr, idList, appId, &windowMatchConfidence,
            taskItemMatched.put_void());
        if (SUCCEEDED(hr)) {
            lastMatchIndex = i;
        }

        g_compareStringOrdinalHookThreadId = 0;
        g_compareStringOrdinalIgnoreSuffix = false;
    }

    if (lastMatchIndex != DA_LAST) {
        i = lastMatchIndex + 1;
    }

    return DPA_InsertPtr_Original(hdpa, i, p);
}

using DPA_DeletePtr_t = decltype(&DPA_DeletePtr);
DPA_DeletePtr_t DPA_DeletePtr_Original;
PVOID WINAPI DPA_DeletePtr_Hook(HDPA hdpa, int i) {
    if (g_doingPinnedItemSwapThreadId == GetCurrentThreadId()) {
        Wh_Log(L">");

        void* p = DPA_GetPtr(hdpa, i);
        if (p) {
            PVOID taskGroup = CTaskBtnGroup_GetGroup_Original(p);
            if (taskGroup && taskGroup == g_doingPinnedItemSwapFromTaskGroup) {
                g_doingPinnedItemSwapIndex = i;
            }
        }
    }

    return DPA_DeletePtr_Original(hdpa, i);
}

using CTaskBand_HandleTaskGroupSwitchItemAdded_t =
    void(WINAPI*)(PVOID pThis, PVOID switchItem);
CTaskBand_HandleTaskGroupSwitchItemAdded_t
    CTaskBand_HandleTaskGroupSwitchItemAdded_Original;
void WINAPI CTaskBand_HandleTaskGroupSwitchItemAdded_Hook(PVOID pThis,
                                                          PVOID switchItem) {
    Wh_Log(L">");

    // Disable creating groups on the taskbar when snapping windows, as it
    // doesn't work well with ungrouping. The function creates new task items
    // with the same AppId as the target windows, but the taskbar has other
    // AppIds for them, so there's no match and explorer crashes.

    // CTaskBand_HandleTaskGroupSwitchItemAdded_Original(pThis, switchItem);
}

using CTaskListWnd_HandleTaskGroupPinned_t = void(WINAPI*)(PVOID pThis,
                                                           PVOID taskGroup);
CTaskListWnd_HandleTaskGroupPinned_t
    CTaskListWnd_HandleTaskGroupPinned_Original;

// The flags argument is absent in newer Windows versions.
using CTaskListWnd_HandleTaskGroupUnpinned_t = void(WINAPI*)(PVOID pThis,
                                                             PVOID taskGroup,
                                                             int flags);
CTaskListWnd_HandleTaskGroupUnpinned_t
    CTaskListWnd_HandleTaskGroupUnpinned_Original;

void SwapTaskGroupIds(PVOID taskGroup1, PVOID taskGroup2) {
    WCHAR appId1Copy[MAX_PATH] = L"";
    if (PCWSTR appId1 = CTaskGroup_GetAppID_Original(taskGroup1)) {
        wcscpy_s(appId1Copy, appId1);
    }

    ITEMIDLIST* idList1Copy = nullptr;
    if (const ITEMIDLIST* idList1 =
            CTaskGroup_GetShortcutIDList_Original(taskGroup1)) {
        idList1Copy = ILClone(idList1);
    }

    DWORD flags1Copy = CTaskGroup_GetFlags_Original(taskGroup1);

    g_disableGetLauncherName = true;
    WCHAR tip1Copy[MAX_PATH] = L"";
    WCHAR tip2Copy[MAX_PATH] = L"";
    CTaskGroup_GetTitleText_Original(taskGroup1, nullptr, tip1Copy, MAX_PATH);
    CTaskGroup_GetTitleText_Original(taskGroup2, nullptr, tip2Copy, MAX_PATH);
    g_disableGetLauncherName = false;

    int iconId1 = 0;
    int iconId2 = 0;
    CTaskGroup_GetIconId_Original(taskGroup1, nullptr, &iconId1);
    CTaskGroup_GetIconId_Original(taskGroup2, nullptr, &iconId2);

    CTaskGroup_SetAppID_Original(taskGroup1,
                                 CTaskGroup_GetAppID_Original(taskGroup2));
    CTaskGroup_SetShortcutIDList_Original(
        taskGroup1, CTaskGroup_GetShortcutIDList_Original(taskGroup2));
    CTaskGroup_UpdateFlags_Original(taskGroup1, ~0,
                                    CTaskGroup_GetFlags_Original(taskGroup2));
    CTaskGroup_SetTip_Original(taskGroup1, tip2Copy);
    CTaskGroup_SetIconId_Original(taskGroup1, nullptr, iconId2);

    CTaskGroup_SetAppID_Original(taskGroup2, appId1Copy);
    CTaskGroup_SetShortcutIDList_Original(taskGroup2, idList1Copy);
    CTaskGroup_UpdateFlags_Original(taskGroup2, ~0, flags1Copy);
    CTaskGroup_SetTip_Original(taskGroup2, tip1Copy);
    CTaskGroup_SetIconId_Original(taskGroup2, nullptr, iconId1);

    if (idList1Copy) {
        ILFree(idList1Copy);
    }
}

void HandleUnsuffixedInstanceOnTaskDestroyed(PVOID taskList_TaskListUI,
                                             PVOID taskGroup) {
    PVOID taskBand = GetTaskBand();
    if (!taskBand) {
        return;
    }

    PCWSTR appId = CTaskGroup_GetAppID_Original(taskGroup);
    if (!appId || FindAppIdSuffix(appId)) {
        return;
    }

    WCHAR appIdWithSuffix[MAX_PATH];
    wcscpy_s(appIdWithSuffix, appId);
    if (!AddAppIdSuffix(appIdWithSuffix, L'_', 0)) {
        return;
    }

    g_compareStringOrdinalHookThreadId = GetCurrentThreadId();
    g_compareStringOrdinalAnySuffixEqual = true;

    winrt::com_ptr<IUnknown> taskGroupMatched;
    winrt::com_ptr<IUnknown> taskItemMatched;
    HRESULT hr = CTaskBand__MatchWindow_Original(
        taskBand, nullptr, nullptr, appIdWithSuffix, 1,
        taskGroupMatched.put_void(), taskItemMatched.put_void());

    g_compareStringOrdinalHookThreadId = 0;
    g_compareStringOrdinalAnySuffixEqual = false;

    if (FAILED(hr) || !taskGroupMatched) {
        return;
    }

    bool taskGroupIsPinned = CTaskGroup_GetFlags_Original(taskGroup) & 1;

    Wh_Log(L"Swapping with matched suffixed item");

    SwapTaskGroupIds(taskGroup, taskGroupMatched.get());

    if (taskGroupIsPinned) {
        g_doingPinnedItemSwapThreadId = GetCurrentThreadId();
        g_doingPinnedItemSwapFromTaskGroup = taskGroup;
        g_doingPinnedItemSwapToTaskGroup = taskGroupMatched.get();
        g_doingPinnedItemSwapIndex = -1;
        // The flags argument is absent in newer Windows versions. According to
        // the calling convention, it just gets ignored.
        CTaskListWnd_HandleTaskGroupUnpinned_Original(taskList_TaskListUI,
                                                      taskGroup, 0);
        CTaskListWnd_HandleTaskGroupPinned_Original(taskList_TaskListUI,
                                                    taskGroupMatched.get());
        g_doingPinnedItemSwapThreadId = 0;
        g_doingPinnedItemSwapFromTaskGroup = nullptr;
        g_doingPinnedItemSwapToTaskGroup = nullptr;
        g_doingPinnedItemSwapIndex = -1;
    }
}

LONG_PTR OnTaskDestroyed(std::function<LONG_PTR()> original,
                         PVOID taskList_TaskListUI,
                         PVOID taskGroup,
                         PVOID taskItem) {
    // taskItem is null when unpinning, for example. Not returning in this case
    // causes a bug in which the item stays pinned if there are running
    // instances on other monitors or virtual desktops.
    if (!taskItem) {
        return original();
    }

    bool isPrimaryTaskbar =
        CTaskListWnd_IsOnPrimaryTaskband_Original(taskList_TaskListUI);
    int numItems = CTaskGroup_GetNumItems_Original(taskGroup);
    if (isPrimaryTaskbar && numItems == 1) {
        HandleUnsuffixedInstanceOnTaskDestroyed(taskList_TaskListUI, taskGroup);
    }

    LONG_PTR ret = original();

    if (isPrimaryTaskbar && numItems == 0) {
        HandleUnsuffixedInstanceOnTaskDestroyed(taskList_TaskListUI, taskGroup);
    }

    return ret;
}

using CTaskListWnd_TaskDestroyed_t = LONG_PTR(WINAPI*)(PVOID pThis,
                                                       PVOID taskGroup,
                                                       PVOID taskItem,
                                                       int taskDestroyedFlags);
CTaskListWnd_TaskDestroyed_t CTaskListWnd_TaskDestroyed_Original;
LONG_PTR WINAPI CTaskListWnd_TaskDestroyed_Hook(PVOID pThis,
                                                PVOID taskGroup,
                                                PVOID taskItem,
                                                int taskDestroyedFlags) {
    Wh_Log(L">");

    auto original = [=]() {
        return CTaskListWnd_TaskDestroyed_Original(pThis, taskGroup, taskItem,
                                                   taskDestroyedFlags);
    };

    return OnTaskDestroyed(original, pThis, taskGroup, taskItem);
}

using CTaskListWnd_TaskDestroyed_2_t = LONG_PTR(WINAPI*)(PVOID pThis,
                                                         PVOID taskGroup,
                                                         PVOID taskItem);
CTaskListWnd_TaskDestroyed_2_t CTaskListWnd_TaskDestroyed_2_Original;
LONG_PTR WINAPI CTaskListWnd_TaskDestroyed_2_Hook(PVOID pThis,
                                                  PVOID taskGroup,
                                                  PVOID taskItem) {
    Wh_Log(L">");

    auto original = [=]() {
        return CTaskListWnd_TaskDestroyed_2_Original(pThis, taskGroup,
                                                     taskItem);
    };

    return OnTaskDestroyed(original, pThis, taskGroup, taskItem);
}

void HandleSuffixedInstanceOnTaskCreated(PVOID taskList_TaskListUI,
                                         PVOID taskGroup) {
    PVOID taskList = (BYTE*)taskList_TaskListUI - ITaskListUIOffset;

    PCWSTR appId = CTaskGroup_GetAppID_Original(taskGroup);
    if (!appId) {
        return;
    }

    WCHAR appIdOriginal[MAX_PATH];
    if (!RemoveAppIdSuffix(appIdOriginal, appId)) {
        return;
    }

    PVOID taskBtnGroupMatched =
        FindTaskBtnGroup(taskList, [appIdOriginal](PVOID taskBtnGroup) {
            PVOID taskGroup = CTaskBtnGroup_GetGroup_Original(taskBtnGroup);
            if (!taskGroup) {
                return false;
            }

            int windowMatchConfidence;
            winrt::com_ptr<IUnknown> taskItemMatched;
            HRESULT hr = CTaskGroup_DoesWindowMatch_Original(
                taskGroup, nullptr, nullptr, appIdOriginal,
                &windowMatchConfidence, taskItemMatched.put_void());
            bool matched = SUCCEEDED(hr);

            return matched;
        });
    if (!taskBtnGroupMatched) {
        return;
    }

    bool taskGroupMatchedIsPinnedType =
        CTaskBtnGroup_GetGroupType_Original(taskBtnGroupMatched) == 2;
    if (!taskGroupMatchedIsPinnedType) {
        return;
    }

    PVOID taskGroupMatched =
        CTaskBtnGroup_GetGroup_Original(taskBtnGroupMatched);
    if (!taskGroupMatched) {
        return;
    }

    Wh_Log(L"Swapping with matched pinned item");

    SwapTaskGroupIds(taskGroup, taskGroupMatched);

    // The flags argument is absent in newer Windows versions. According to the
    // calling convention, it just gets ignored.
    CTaskListWnd_HandleTaskGroupUnpinned_Original(taskList_TaskListUI,
                                                  taskGroupMatched, 0);
    CTaskListWnd_HandleTaskGroupPinned_Original(taskList_TaskListUI, taskGroup);
}

using CTaskListWnd__TaskCreated_t = LONG_PTR(WINAPI*)(PVOID pThis,
                                                      PVOID taskGroup,
                                                      PVOID taskItem,
                                                      int param3);
CTaskListWnd__TaskCreated_t CTaskListWnd__TaskCreated_Original;
LONG_PTR WINAPI CTaskListWnd__TaskCreated_Hook(PVOID pThis,
                                               PVOID taskGroup,
                                               PVOID taskItem,
                                               int param3) {
    Wh_Log(L">");

    auto original = [=]() {
        return CTaskListWnd__TaskCreated_Original(pThis, taskGroup, taskItem,
                                                  param3);
    };

    PVOID pThis_TaskListUI = (BYTE*)pThis + ITaskListUIOffset;

    if (!CTaskListWnd_IsOnPrimaryTaskband_Original(pThis_TaskListUI)) {
        return original();
    }

    LONG_PTR ret = original();

    // Check if it exists on the task list.
    PVOID taskBtnGroup =
        CTaskListWnd__GetTBGroupFromGroup_Original(pThis, taskGroup, nullptr);
    if (!taskBtnGroup) {
        return ret;
    }

    HandleSuffixedInstanceOnTaskCreated(pThis_TaskListUI, taskGroup);

    return ret;
}

using CompareStringOrdinal_t = decltype(&CompareStringOrdinal);
CompareStringOrdinal_t CompareStringOrdinal_Original;
int WINAPI CompareStringOrdinal_Hook(LPCWCH lpString1,
                                     int cchCount1,
                                     LPCWCH lpString2,
                                     int cchCount2,
                                     BOOL bIgnoreCase) {
    if (g_compareStringOrdinalHookThreadId == GetCurrentThreadId() &&
        cchCount1 == -1 && cchCount2 == -1) {
        PCWSTR suffix1 = FindAppIdSuffix(lpString1);
        PCWSTR suffix2 = FindAppIdSuffix(lpString2);

        if (g_compareStringOrdinalAnySuffixEqual) {
            if (suffix1 && suffix2) {
                cchCount1 = suffix1 - lpString1;
                cchCount2 = suffix2 - lpString2;
            }
        } else if (g_compareStringOrdinalIgnoreSuffix) {
            if (suffix1) {
                cchCount1 = suffix1 - lpString1;
            }

            if (suffix2) {
                cchCount2 = suffix2 - lpString2;
            }
        }
    }

    return CompareStringOrdinal_Original(lpString1, cchCount1, lpString2,
                                         cchCount2, bIgnoreCase);
}

using RegGetValueW_t = decltype(&RegGetValueW);
RegGetValueW_t RegGetValueW_Original;
LONG WINAPI RegGetValueW_Hook(HKEY key,
                              LPCWSTR subKey,
                              LPCWSTR valueName,
                              DWORD flags,
                              LPDWORD type,
                              PVOID data,
                              LPDWORD dataSize) {
    LONG result = RegGetValueW_Original(key, subKey, valueName, flags, type,
                                       data, dataSize);

    if (g_unloading || key != HKEY_CURRENT_USER || !subKey || !valueName ||
        _wcsicmp(
            subKey,
            LR"(SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Advanced)") !=
            0 ||
        (_wcsicmp(valueName, L"TaskbarGlomLevel") != 0 &&
         _wcsicmp(valueName, L"MMTaskbarGlomLevel") != 0) ||
        flags != RRF_RT_REG_DWORD || !data || !dataSize ||
        *dataSize != sizeof(DWORD) || !IsNativeVerticalTaskbar()) {
        return result;
    }

    DWORD original = result == ERROR_SUCCESS ? *static_cast<DWORD*>(data) : 0;
    *static_cast<DWORD*>(data) = 0;  // Always combine: compact icon-only frame.
    if (type) {
        *type = REG_DWORD;
    }

    if (!g_loggedCombineOverride.exchange(true)) {
        Wh_Log(L"Forcing %s to compact combine mode: %u->0", valueName,
               original);
    }

    return ERROR_SUCCESS;
}

void RequestTaskbarBehaviorRefresh() {
    HWND taskbar = FindCurrentProcessTaskbarWnd();
    if (taskbar) {
        SendMessage(taskbar, WM_SETTINGCHANGE, 0, 0);
    }
}

bool HookTaskbarSymbols() {
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] =  //
        {
            {
                {LR"(public: virtual int __cdecl CTaskGroup::GetNumItems(void))"},
                &CTaskGroup_GetNumItems_Original,
            },
            {
                {LR"(public: virtual long __cdecl CTaskGroup::SetAppID(unsigned short const *))"},
                &CTaskGroup_SetAppID_Original,
            },
            {
                {LR"(public: virtual unsigned long __cdecl CTaskGroup::GetFlags(void)const )"},
                &CTaskGroup_GetFlags_Original,
            },
            {
                {LR"(public: virtual long __cdecl CTaskGroup::UpdateFlags(unsigned long,unsigned long))"},
                &CTaskGroup_UpdateFlags_Original,
            },
            {
                {LR"(public: virtual long __cdecl CTaskGroup::GetTitleText(struct ITaskItem *,unsigned short *,int))"},
                &CTaskGroup_GetTitleText_Original,
            },
            {
                {LR"(public: virtual long __cdecl CTaskGroup::SetTip(unsigned short const *))"},
                &CTaskGroup_SetTip_Original,
            },
            {
                {LR"(public: virtual long __cdecl CTaskGroup::GetIconId(struct ITaskItem *,int *))"},
                &CTaskGroup_GetIconId_Original,
            },
            {
                {LR"(public: virtual long __cdecl CTaskGroup::SetIconId(struct ITaskItem *,int))"},
                &CTaskGroup_SetIconId_Original,
            },
            {
                {LR"(public: virtual long __cdecl CTaskGroup::DoesWindowMatch(struct HWND__ *,struct _ITEMIDLIST_ABSOLUTE const *,unsigned short const *,enum WINDOWMATCHCONFIDENCE *,struct ITaskItem * *))"},
                &CTaskGroup_DoesWindowMatch_Original,
            },
            {
                {LR"(protected: long __cdecl CTaskBand::_MatchWindow(struct HWND__ *,struct _ITEMIDLIST_ABSOLUTE const *,unsigned short const *,enum WINDOWMATCHCONFIDENCE,struct ITaskGroup * *,struct ITaskItem * *))"},
                &CTaskBand__MatchWindow_Original,
            },
            {
                {LR"(public: virtual enum eTBGROUPTYPE __cdecl CTaskBtnGroup::GetGroupType(void))"},
                &CTaskBtnGroup_GetGroupType_Original,
            },
            {
                {LR"(protected: virtual __int64 __cdecl CTaskBand::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
                &CTaskBand_v_WndProc_Original,
                CTaskBand_v_WndProc_Hook,
            },
            {
                {LR"(protected: void __cdecl CTaskBand::_HandleItemResolved(struct RESOLVEDWINDOW *,struct ITaskListUI *,struct ITaskGroup *,struct ITaskItem *))"},
                &CTaskBand__HandleItemResolved_Original,
                CTaskBand__HandleItemResolved_Hook,
            },
            {
                {LR"(private: long __cdecl CTaskBand::CLauncherTask::_Launch(void))"},
                &CTaskBand__Launch_Original,
                CTaskBand__Launch_Hook,
            },
            {
                {LR"(public: virtual unsigned short const * __cdecl CTaskGroup::GetAppID(void))"},
                &CTaskGroup_GetAppID_Original,
                CTaskGroup_GetAppID_Hook,
            },
            {
                {LR"(public: virtual bool __cdecl CTaskGroup::IsImmersiveGroup(void))"},
                &CTaskGroup_IsImmersiveGroup_Original,
                CTaskGroup_IsImmersiveGroup_Hook,
            },
            {
                {LR"(public: virtual struct _ITEMIDLIST_ABSOLUTE * __cdecl CTaskGroup::GetApplicationIDList(void))"},
                &CTaskGroup_GetApplicationIDList_Original,
            },
            {
                {LR"(public: virtual struct _ITEMIDLIST_ABSOLUTE const * __cdecl CTaskGroup::GetShortcutIDList(void))"},
                &CTaskGroup_GetShortcutIDList_Original,
                CTaskGroup_GetShortcutIDList_Hook,
            },
            {
                {LR"(public: virtual long __cdecl CTaskGroup::SetShortcutIDList(struct _ITEMIDLIST_ABSOLUTE const *))"},
                &CTaskGroup_SetShortcutIDList_Original,
            },
            {
                {LR"(public: virtual unsigned short const * __cdecl CTaskGroup::GetIconResource(void))"},
                &CTaskGroup_GetIconResource_Original,
                CTaskGroup_GetIconResource_Hook,
            },
            {
                {LR"(protected: void __cdecl CTaskBand::_UpdateItemIcon(struct ITaskGroup *,struct ITaskItem *))"},
                &CTaskBand__UpdateItemIcon_Original,
                CTaskBand__UpdateItemIcon_Hook,
            },
            {
                {LR"(public: virtual long __cdecl CTaskBand::Launch(struct ITaskGroup *,struct tagPOINT const &,enum LaunchFromTaskbarOptions))"},
                &CTaskBand_Launch_Original,
                CTaskBand_Launch_Hook,
            },
            {
                {LR"(public: virtual long __cdecl CTaskGroup::GetLauncherName(unsigned short * *))"},
                &CTaskGroup_GetLauncherName_Original,
                CTaskGroup_GetLauncherName_Hook,
            },
            {
                {LR"(protected: long __cdecl CTaskListWnd::_GetJumpViewParams(struct ITaskBtnGroup *,struct ITaskItem *,int,bool,struct Windows::Internal::Shell::JumpView::IJumpViewParams * *)const )"},
                &CTaskListWnd__GetJumpViewParams_Original,
                CTaskListWnd__GetJumpViewParams_Hook,
            },
            {
                {LR"(public: virtual long __cdecl CTaskListWnd::ShowJumpView(struct ITaskGroup *,struct ITaskItem *,bool))"},
                &CTaskListWnd_ShowJumpView_Original,
                CTaskListWnd_ShowJumpView_Hook,
            },
            {
                // Available from Windows 11.
                {LR"(public: virtual long __cdecl CTaskBtnGroup::GetIcon(struct ITaskItem *,struct HICON__ * *))"},
                &CTaskBtnGroup_GetIcon_Original,
                CTaskBtnGroup_GetIcon_Hook,
                true,
            },
            {
                {LR"(public: virtual struct ITaskGroup * __cdecl CTaskBtnGroup::GetGroup(void))"},
                &CTaskBtnGroup_GetGroup_Original,
                CTaskBtnGroup_GetGroup_Hook,
            },
            {
                {LR"(protected: struct ITaskBtnGroup * __cdecl CTaskListWnd::_GetTBGroupFromGroup(struct ITaskGroup *,int *))"},
                &CTaskListWnd__GetTBGroupFromGroup_Original,
            },
            {
                {LR"(public: virtual int __cdecl CTaskListWnd::IsOnPrimaryTaskband(void))"},
                &CTaskListWnd_IsOnPrimaryTaskband_Original,
            },
            {
                {LR"(protected: struct ITaskBtnGroup * __cdecl CTaskListWnd::_CreateTBGroup(struct ITaskGroup *,int))"},
                &CTaskListWnd__CreateTBGroup_Original,
                CTaskListWnd__CreateTBGroup_Hook,
            },
            {
                // Available from Windows 11.
                {LR"(protected: void __cdecl CTaskBand::HandleTaskGroupSwitchItemAdded(struct winrt::Windows::Internal::ComposableShell::Multitasking::ISwitchItem const &))"},
                &CTaskBand_HandleTaskGroupSwitchItemAdded_Original,
                CTaskBand_HandleTaskGroupSwitchItemAdded_Hook,
                true,
            },
            {
                {LR"(public: virtual void __cdecl CTaskListWnd::HandleTaskGroupPinned(struct ITaskGroup *))"},
                &CTaskListWnd_HandleTaskGroupPinned_Original,
            },
            {
                {
                    LR"(public: virtual void __cdecl CTaskListWnd::HandleTaskGroupUnpinned(struct ITaskGroup *))",

                    // Before Windows 11 24H2.
                    LR"(public: virtual void __cdecl CTaskListWnd::HandleTaskGroupUnpinned(struct ITaskGroup *,enum HandleTaskGroupUnpinnedFlags))",
                },
                &CTaskListWnd_HandleTaskGroupUnpinned_Original,
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
            {
                {LR"(protected: long __cdecl CTaskListWnd::_TaskCreated(struct ITaskGroup *,struct ITaskItem *,int))"},
                &CTaskListWnd__TaskCreated_Original,
                CTaskListWnd__TaskCreated_Hook,
            },
        };

    HMODULE module = LoadLibraryEx(L"taskbar.dll", nullptr,
                                   LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load taskbar.dll");
        return false;
    }

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (!HookTaskbarSymbols()) {
        return FALSE;
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto kernelBaseCompareStringOrdinal =
        (decltype(&CompareStringOrdinal))GetProcAddress(kernelBaseModule,
                                                        "CompareStringOrdinal");
    auto kernelBaseRegGetValueW = (decltype(&RegGetValueW))GetProcAddress(
        kernelBaseModule, "RegGetValueW");
    if (!kernelBaseCompareStringOrdinal || !kernelBaseRegGetValueW ||
        !WindhawkUtils::SetFunctionHook(kernelBaseCompareStringOrdinal,
                                        CompareStringOrdinal_Hook,
                                        &CompareStringOrdinal_Original) ||
        !WindhawkUtils::SetFunctionHook(kernelBaseRegGetValueW,
                                        RegGetValueW_Hook,
                                        &RegGetValueW_Original) ||
        !WindhawkUtils::SetFunctionHook(DPA_InsertPtr, DPA_InsertPtr_Hook,
                                        &DPA_InsertPtr_Original) ||
        !WindhawkUtils::SetFunctionHook(DPA_DeletePtr, DPA_DeletePtr_Hook,
                                        &DPA_DeletePtr_Original)) {
        Wh_Log(L"Failed to install API hooks");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    RequestTaskbarBehaviorRefresh();
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    RequestTaskbarBehaviorRefresh();
}
