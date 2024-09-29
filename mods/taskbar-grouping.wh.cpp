// ==WindhawkMod==
// @id              taskbar-grouping
// @name            Disable grouping on the taskbar
// @description     Causes a separate button to be created on the taskbar for each new window
// @version         1.3.5
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -D__USE_MINGW_ANSI_STDIO=0 -lcomctl32 -loleaut32 -lole32 -lshlwapi -lversion -lwininet
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
# Disable grouping on the taskbar

Causes a separate button to be created on the taskbar for each new window. For
example, notice the two separate buttons for Notepad on the screenshot:

![Demonstration](https://i.imgur.com/uLITliK.png)

Also, custom groups can be configured in the settings. For example, you can
choose to group Paint and Notepad:

![Custom groups](https://i.imgur.com/moj4nOV.png)

**Note:** After enabling the mod, the relevant windows must be reopened to apply
the grouping settings.

Only Windows 10 64-bit and Windows 11 are supported. For other Windows versions
check out [7+ Taskbar Tweaker](https://tweaker.ramensoftware.com/).

**Note:** To customize the old taskbar on Windows 11 (if using ExplorerPatcher
or a similar tool), enable the relevant option in the mod's settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- pinnedItemsMode: replace
  $name: Pinned items mode
  $description: >-
    By default, running instances replace pinned items. This option allows to
    choose that pinned item will always remain in place, and running instances
    will be opened separately. The third option allows to keep pinned items in
    place, while still having running instances grouped.
  $options:
  - replace: Running items replace pinned items
  - keepInPlace: Pinned items remain in place
  - keepInPlaceAndNoUngrouping: >-
      Pinned items remain in place, group running instances
- placeUngroupedItemsTogether: false
  $name: Place ungrouped items together
  $description: >-
    Place each newly opened item next to existing items it would group with.
- useWindowIcons: false
  $name: Use window icons
  $description: >-
    By default, application icons are used. Enable this option to use window
    icons instead. Usually it doesn't matter, an example where it does is an
    open folder window - with application icons, the icon on the taskbar is
    always the icon of Explorer, while with window icons, the icon changes
    depending on the open folder.
- customGroups:
  - - name: Group 1
      $name: Group name
      $description: >-
        Must not be empty. Will be shown on the taskbar if labels are shown.
    - items: [group1-program1.exe, group1-program2.exe]
      $name: Process names, paths or application IDs
      $description: >-
        For example:

        mspaint.exe

        C:\Windows\System32\notepad.exe

        Microsoft.WindowsCalculator_8wekyb3d8bbwe!App
  $name: Custom groups
  $description: >-
    Each custom group is a list of names/paths of programs that will be grouped
    together.
- excludedPrograms: [excluded1.exe]
  $name: Excluded programs
  $description: >-
    Each entry is a name, path, or application ID that the mod will ignore.
    Excluded programs will keep their own grouping behavior. Usually that means
    that each program will be grouped separately, but sometimes there are custom
    grouping rules, e.g. Chrome creates a group for each browser profile.
- groupingMode: regular
  $name: Grouping mode
  $options:
  - regular: Disable grouping unless excluded
  - inverse: "Inverse: Only disable grouping if excluded"
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    ExplorerPatcher or a similar tool). Note: For Windhawk versions older than
    1.3, you have to disable and re-enable the mod to apply this option.
*/
// ==/WindhawkModSettings==

#include <shlobj.h>
#include <shlwapi.h>
#include <wininet.h>
#include <winrt/base.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

enum class PinnedItemsMode {
    replace,
    keepInPlace,
    keepInPlaceAndNoUngrouping,
};

enum class GroupingMode {
    regular,
    inverse,
};

struct {
    PinnedItemsMode pinnedItemsMode;
    bool placeUngroupedItemsTogether;
    bool useWindowIcons;
    std::unordered_set<std::wstring> excludedProgramItems;
    std::vector<std::wstring> customGroupNames;
    std::unordered_map<std::wstring, int> customGroupProgramItems;
    GroupingMode groupingMode;
    bool oldTaskbarOnWin11;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
};

constexpr WCHAR kCustomGroupPrefix[] = L"Windhawk_Group_";
constexpr size_t kCustomGroupPrefixLen = ARRAYSIZE(kCustomGroupPrefix) - 1;

WinVersion g_winVersion;

bool g_inTaskBandLaunch;
bool g_inUpdateItemIcon;
bool g_inTaskBtnGroupGetIcon;
bool g_inGetJumpViewParams;
bool g_inFindTaskBtnGroup;
PVOID g_findTaskBtnGroup_TaskGroupSentinel =
    &g_findTaskBtnGroup_TaskGroupSentinel;
std::function<bool(PVOID)> g_findTaskBtnGroup_Callback;
std::atomic<DWORD> g_cTaskListWnd__CreateTBGroup_ThreadId;
bool g_disableGetLauncherName;
std::atomic<DWORD> g_compareStringOrdinalHookThreadId;
bool g_compareStringOrdinalIgnoreSuffix;
bool g_compareStringOrdinalAnySuffixEqual;

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

PWSTR FindAppIdSuffix(PWSTR appId) {
    return const_cast<PWSTR>(FindAppIdSuffix(static_cast<PCWSTR>(appId)));
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

    DWORD resolvedAppIdStrLen = wcslen(resolvedWindow->szAppIdStr);
    WCHAR resolvedAppIdStrUpper[MAX_PATH];
    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE,
                  resolvedWindow->szAppIdStr, resolvedAppIdStrLen + 1,
                  resolvedAppIdStrUpper, resolvedAppIdStrLen + 1, nullptr,
                  nullptr, 0);

    DWORD resolvedWindowProcessPathLen = 0;
    WCHAR resolvedWindowProcessPath[MAX_PATH];
    WCHAR resolvedWindowProcessPathUpper[MAX_PATH];
    PCWSTR programFileNameUpper = nullptr;
    if (resolvedWindow->hButtonWnd) {
        DWORD dwProcessId = 0;
        if (GetWindowThreadProcessId(resolvedWindow->hButtonWnd,
                                     &dwProcessId)) {
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

        LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE,
                      resolvedWindowProcessPath,
                      resolvedWindowProcessPathLen + 1,
                      resolvedWindowProcessPathUpper,
                      resolvedWindowProcessPathLen + 1, nullptr, nullptr, 0);

        programFileNameUpper = wcsrchr(resolvedWindowProcessPathUpper, L'\\');
        if (programFileNameUpper) {
            programFileNameUpper++;
            if (!*programFileNameUpper) {
                programFileNameUpper = nullptr;
            }
        }
    }

    bool excluded = false;

    if (g_settings.excludedProgramItems.contains(resolvedAppIdStrUpper)) {
        Wh_Log(L"Excluding %s", resolvedWindow->szAppIdStr);
        excluded = true;
    }

    if (!excluded && resolvedWindowProcessPathLen > 0 &&
        g_settings.excludedProgramItems.contains(
            resolvedWindowProcessPathUpper)) {
        Wh_Log(L"Excluding %s", resolvedWindowProcessPath);
        excluded = true;
    }

    if (!excluded && programFileNameUpper &&
        g_settings.excludedProgramItems.contains(programFileNameUpper)) {
        Wh_Log(L"Excluding %s", resolvedWindowProcessPath);
        excluded = true;
    }

    if (g_settings.groupingMode == GroupingMode::inverse) {
        excluded = !excluded;
    }

    if (excluded) {
        return;
    }

    int customGroup = 0;

    if (auto it =
            g_settings.customGroupProgramItems.find(resolvedAppIdStrUpper);
        it != g_settings.customGroupProgramItems.end()) {
        customGroup = it->second;
    }

    if (!customGroup && resolvedWindowProcessPathLen > 0) {
        if (auto it = g_settings.customGroupProgramItems.find(
                resolvedWindowProcessPathUpper);
            it != g_settings.customGroupProgramItems.end()) {
            customGroup = it->second;
        }
    }

    if (!customGroup && programFileNameUpper) {
        if (auto it =
                g_settings.customGroupProgramItems.find(programFileNameUpper);
            it != g_settings.customGroupProgramItems.end()) {
            customGroup = it->second;
        }
    }

    if (!customGroup) {
        winrt::com_ptr<IUnknown> taskGroupMatched;
        winrt::com_ptr<IUnknown> taskItemMatched;
        HRESULT hr = CTaskBand__MatchWindow_Original(
            pThis, resolvedWindow->hButtonWnd, resolvedWindow->pAppItemIdList,
            resolvedWindow->szAppIdStr, 1, taskGroupMatched.put_void(),
            taskItemMatched.put_void());
        if (FAILED(hr)) {
            // Nothing to group with, resolve normally.
            return;
        }

        bool isMatchPinned =
            CTaskGroup_GetNumItems_Original(taskGroupMatched.get()) == 0;

        if (g_settings.pinnedItemsMode == PinnedItemsMode::replace &&
            isMatchPinned) {
            // Will group with a pinned item, resolve normally.
            return;
        }
    }

    if (resolvedWindow->pAppItemIdList) {
        ILFree(resolvedWindow->pAppItemIdList);
        resolvedWindow->pAppItemIdList = nullptr;
    }

    if (customGroup) {
        swprintf(resolvedWindow->szAppIdStr, L"%s%d", kCustomGroupPrefix,
                 customGroup);
        Wh_Log(L"Custom group AppId: %s", resolvedWindow->szAppIdStr);
    } else {
        bool appIdSuffixAdded;
        if (g_settings.pinnedItemsMode ==
            PinnedItemsMode::keepInPlaceAndNoUngrouping) {
            appIdSuffixAdded =
                AddAppIdSuffix(resolvedWindow->szAppIdStr, L'p', 0);
        } else if (resolvedWindow->hButtonWnd) {
            appIdSuffixAdded =
                AddAppIdSuffix(resolvedWindow->szAppIdStr, L'w',
                               (DWORD)(DWORD_PTR)resolvedWindow->hButtonWnd);
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
}

using CTaskBand__HandleWindowResolved_t =
    void(WINAPI*)(PVOID pThis, RESOLVEDWINDOW* resolvedWindow);
CTaskBand__HandleWindowResolved_t CTaskBand__HandleWindowResolved_Original;
void WINAPI
CTaskBand__HandleWindowResolved_Hook(PVOID pThis,
                                     RESOLVEDWINDOW* resolvedWindow) {
    Wh_Log(L">");

    g_compareStringOrdinalHookThreadId = GetCurrentThreadId();
    g_compareStringOrdinalIgnoreSuffix = true;

    CTaskBand__HandleWindowResolved_Original(pThis, resolvedWindow);

    g_compareStringOrdinalHookThreadId = 0;
    g_compareStringOrdinalIgnoreSuffix = false;
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

    // Reset flags set by CTaskBand__HandleWindowResolved_Hook.
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

    if (g_inUpdateItemIcon) {
        winrt::com_ptr<IUnknown> taskGroupWithoutSuffix =
            GetTaskGroupWithoutSuffix(pThis);
        if (taskGroupWithoutSuffix) {
            return CTaskGroup_GetAppID_Original(taskGroupWithoutSuffix.get());
        }
    }

    return CTaskGroup_GetAppID_Original(pThis);
}

PVOID GetTaskBand() {
    static PVOID taskBand = nullptr;
    if (taskBand) {
        return taskBand;
    }

    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    DWORD processId = 0;
    if (hTaskbarWnd && GetWindowThreadProcessId(hTaskbarWnd, &processId) &&
        processId == GetCurrentProcessId()) {
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

        if (g_settings.useWindowIcons) {
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

    PCWSTR appId = CTaskGroup_GetAppID_Original(pThis);
    if (appId &&
        wcsncmp(appId, kCustomGroupPrefix, kCustomGroupPrefixLen) == 0) {
        int customGroup = _wtoi(appId + kCustomGroupPrefixLen);
        int groupIndex = customGroup - 1;
        if (groupIndex >= 0 &&
            groupIndex < static_cast<int>(g_settings.customGroupNames.size())) {
            return SHStrDup(g_settings.customGroupNames[groupIndex].c_str(),
                            ppwsz);
        }
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

using CTaskBtnGroup__DrawRegularButton_t = void(WINAPI*)(PVOID pThis,
                                                         PVOID param1,
                                                         PVOID param2);
CTaskBtnGroup__DrawRegularButton_t CTaskBtnGroup__DrawRegularButton_Original;
void WINAPI CTaskBtnGroup__DrawRegularButton_Hook(PVOID pThis,
                                                  PVOID param1,
                                                  PVOID param2) {
    Wh_Log(L">");

    // This call resembles CTaskBtnGroup::GetIcon of Windows 11 regarding icon
    // handling.
    g_inTaskBtnGroupGetIcon = true;
    CTaskBtnGroup__DrawRegularButton_Original(pThis, param1, param2);
    g_inTaskBtnGroupGetIcon = false;
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
    auto original = [&]() { return DPA_InsertPtr_Original(hdpa, i, p); };

    if (g_cTaskListWnd__CreateTBGroup_ThreadId != GetCurrentThreadId()) {
        return original();
    }

    Wh_Log(L">");

    if (!g_settings.placeUngroupedItemsTogether || i != DA_LAST || !p) {
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

    return original();
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

using CTaskListWnd_GroupChanged_t = LONG_PTR(WINAPI*)(void* pThis,
                                                      void* taskGroup,
                                                      int taskGroupProperty);
CTaskListWnd_GroupChanged_t CTaskListWnd_GroupChanged_Original;

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
        wcscpy_s(appId1Copy, CTaskGroup_GetAppID_Original(taskGroup1));
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

    CTaskGroup_SetAppID_Original(taskGroup1,
                                 CTaskGroup_GetAppID_Original(taskGroup2));
    CTaskGroup_SetShortcutIDList_Original(
        taskGroup1, CTaskGroup_GetShortcutIDList_Original(taskGroup2));
    CTaskGroup_UpdateFlags_Original(taskGroup1, ~0,
                                    CTaskGroup_GetFlags_Original(taskGroup2));
    CTaskGroup_SetTip_Original(taskGroup1, tip2Copy);

    CTaskGroup_SetAppID_Original(taskGroup2, appId1Copy);
    CTaskGroup_SetShortcutIDList_Original(taskGroup2, idList1Copy);
    CTaskGroup_UpdateFlags_Original(taskGroup2, ~0, flags1Copy);
    CTaskGroup_SetTip_Original(taskGroup2, tip1Copy);

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
        CTaskListWnd_HandleTaskGroupPinned_Original(taskList_TaskListUI,
                                                    taskGroupMatched.get());
    }
}

LONG_PTR OnTaskDestroyed(std::function<LONG_PTR()> original,
                         PVOID taskList_TaskListUI,
                         PVOID taskGroup,
                         PVOID taskItem) {
    // taskItem is null when unpinning, for example. Not returning in this case
    // causes a bug in which the item stays pinned if there are running
    // instances on other monitors or virtual desktops.
    if (!CTaskListWnd_IsOnPrimaryTaskband_Original(taskList_TaskListUI) ||
        !taskItem) {
        return original();
    }

    int numItems = CTaskGroup_GetNumItems_Original(taskGroup);
    bool taskGroupIsPinned = CTaskGroup_GetFlags_Original(taskGroup) & 1;

    if (numItems == 1) {
        HandleUnsuffixedInstanceOnTaskDestroyed(taskList_TaskListUI, taskGroup);
    }

    LONG_PTR ret = original();

    if (numItems == 0) {
        HandleUnsuffixedInstanceOnTaskDestroyed(taskList_TaskListUI, taskGroup);
    }

    if (taskGroupIsPinned && numItems == 1 && g_settings.useWindowIcons &&
        CTaskListWnd_GroupChanged_Original) {
        // Trigger CTaskListWnd::GroupChanged to trigger an icon change.
        // https://github.com/ramensoftware/windhawk-mods/issues/644
        int taskGroupProperty = 4;  // saw this in the debugger
        CTaskListWnd_GroupChanged_Original(taskList_TaskListUI, taskGroup,
                                           taskGroupProperty);
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

    auto original = [&]() {
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

    auto original = [&]() {
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

    auto original = [&]() {
        return CTaskListWnd__TaskCreated_Original(pThis, taskGroup, taskItem,
                                                  param3);
    };

    if (g_settings.pinnedItemsMode != PinnedItemsMode::replace) {
        return original();
    }

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
                                     WINBOOL bIgnoreCase) {
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

WinVersion GetWindowsVersion() {
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
            } else {
                return WinVersion::Win11;
            }
            break;
    }

    return WinVersion::Unsupported;
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
        Wh_Log(L"GetModuleFileName returned an unsupported path");
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
    constexpr WCHAR kModIdForCache[] = L"taskbar-grouping";

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

bool HookTaskbarSymbols() {
    // Taskbar.dll, explorer.exe
    SYMBOL_HOOK symbolHooks[] =
        {
            {
                {
                    LR"(public: virtual int __cdecl CTaskGroup::GetNumItems(void))",
                    LR"(public: virtual int __cdecl CTaskGroup::GetNumItems(void) __ptr64)",
                },
                (void**)&CTaskGroup_GetNumItems_Original,
            },
            {
                {
                    LR"(public: virtual long __cdecl CTaskGroup::SetAppID(unsigned short const *))",
                    LR"(public: virtual long __cdecl CTaskGroup::SetAppID(unsigned short const * __ptr64) __ptr64)",
                },
                (void**)&CTaskGroup_SetAppID_Original,
            },
            {
                {
                    LR"(public: virtual unsigned long __cdecl CTaskGroup::GetFlags(void)const )",
                    LR"(public: virtual unsigned long __cdecl CTaskGroup::GetFlags(void)const __ptr64)",
                },
                (void**)&CTaskGroup_GetFlags_Original,
            },
            {
                {
                    LR"(public: virtual long __cdecl CTaskGroup::UpdateFlags(unsigned long,unsigned long))",
                    LR"(public: virtual long __cdecl CTaskGroup::UpdateFlags(unsigned long,unsigned long) __ptr64)",
                },
                (void**)&CTaskGroup_UpdateFlags_Original,
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
                    LR"(public: virtual long __cdecl CTaskGroup::SetTip(unsigned short const *))",
                    LR"(public: virtual long __cdecl CTaskGroup::SetTip(unsigned short const * __ptr64) __ptr64)",
                },
                (void**)&CTaskGroup_SetTip_Original,
            },
            {
                {
                    LR"(public: virtual long __cdecl CTaskGroup::DoesWindowMatch(struct HWND__ *,struct _ITEMIDLIST_ABSOLUTE const *,unsigned short const *,enum WINDOWMATCHCONFIDENCE *,struct ITaskItem * *))",
                    LR"(public: virtual long __cdecl CTaskGroup::DoesWindowMatch(struct HWND__ * __ptr64,struct _ITEMIDLIST_ABSOLUTE const * __ptr64,unsigned short const * __ptr64,enum WINDOWMATCHCONFIDENCE * __ptr64,struct ITaskItem * __ptr64 * __ptr64) __ptr64)",
                },
                (void**)&CTaskGroup_DoesWindowMatch_Original,
            },
            {
                {
                    LR"(protected: long __cdecl CTaskBand::_MatchWindow(struct HWND__ *,struct _ITEMIDLIST_ABSOLUTE const *,unsigned short const *,enum WINDOWMATCHCONFIDENCE,struct ITaskGroup * *,struct ITaskItem * *))",
                    LR"(protected: long __cdecl CTaskBand::_MatchWindow(struct HWND__ * __ptr64,struct _ITEMIDLIST_ABSOLUTE const * __ptr64,unsigned short const * __ptr64,enum WINDOWMATCHCONFIDENCE,struct ITaskGroup * __ptr64 * __ptr64,struct ITaskItem * __ptr64 * __ptr64) __ptr64)",
                },
                (void**)&CTaskBand__MatchWindow_Original,
            },
            {
                {
                    LR"(public: virtual enum eTBGROUPTYPE __cdecl CTaskBtnGroup::GetGroupType(void))",
                    LR"(public: virtual enum eTBGROUPTYPE __cdecl CTaskBtnGroup::GetGroupType(void) __ptr64)",
                },
                (void**)&CTaskBtnGroup_GetGroupType_Original,
            },
            {
                {
                    LR"(protected: void __cdecl CTaskBand::_HandleWindowResolved(struct RESOLVEDWINDOW *))",
                    LR"(protected: void __cdecl CTaskBand::_HandleWindowResolved(struct RESOLVEDWINDOW * __ptr64) __ptr64)",
                },
                (void**)&CTaskBand__HandleWindowResolved_Original,
                (void*)CTaskBand__HandleWindowResolved_Hook,
            },
            {
                {
                    LR"(protected: void __cdecl CTaskBand::_HandleItemResolved(struct RESOLVEDWINDOW *,struct ITaskListUI *,struct ITaskGroup *,struct ITaskItem *))",
                    LR"(protected: void __cdecl CTaskBand::_HandleItemResolved(struct RESOLVEDWINDOW * __ptr64,struct ITaskListUI * __ptr64,struct ITaskGroup * __ptr64,struct ITaskItem * __ptr64) __ptr64)",
                },
                (void**)&CTaskBand__HandleItemResolved_Original,
                (void*)CTaskBand__HandleItemResolved_Hook,
            },
            {
                {
                    LR"(private: long __cdecl CTaskBand::CLauncherTask::_Launch(void))",
                    LR"(private: long __cdecl CTaskBand::CLauncherTask::_Launch(void) __ptr64)",
                },
                (void**)&CTaskBand__Launch_Original,
                (void*)CTaskBand__Launch_Hook,
            },
            {
                {
                    LR"(public: virtual unsigned short const * __cdecl CTaskGroup::GetAppID(void))",
                    LR"(public: virtual unsigned short const * __ptr64 __cdecl CTaskGroup::GetAppID(void) __ptr64)",
                },
                (void**)&CTaskGroup_GetAppID_Original,
                (void*)CTaskGroup_GetAppID_Hook,
            },
            {
                {
                    LR"(public: virtual bool __cdecl CTaskGroup::IsImmersiveGroup(void))",
                    LR"(public: virtual bool __cdecl CTaskGroup::IsImmersiveGroup(void) __ptr64)",
                },
                (void**)&CTaskGroup_IsImmersiveGroup_Original,
                (void*)CTaskGroup_IsImmersiveGroup_Hook,
            },
            {
                {
                    LR"(public: virtual struct _ITEMIDLIST_ABSOLUTE * __cdecl CTaskGroup::GetApplicationIDList(void))",
                    LR"(public: virtual struct _ITEMIDLIST_ABSOLUTE * __ptr64 __cdecl CTaskGroup::GetApplicationIDList(void) __ptr64)",
                },
                (void**)&CTaskGroup_GetApplicationIDList_Original,
            },
            {
                {
                    LR"(public: virtual struct _ITEMIDLIST_ABSOLUTE const * __cdecl CTaskGroup::GetShortcutIDList(void))",
                    LR"(public: virtual struct _ITEMIDLIST_ABSOLUTE const * __ptr64 __cdecl CTaskGroup::GetShortcutIDList(void) __ptr64)",
                },
                (void**)&CTaskGroup_GetShortcutIDList_Original,
                (void*)CTaskGroup_GetShortcutIDList_Hook,
            },
            {
                {
                    LR"(public: virtual long __cdecl CTaskGroup::SetShortcutIDList(struct _ITEMIDLIST_ABSOLUTE const *))",
                    LR"(public: virtual long __cdecl CTaskGroup::SetShortcutIDList(struct _ITEMIDLIST_ABSOLUTE const * __ptr64) __ptr64)",
                },
                (void**)&CTaskGroup_SetShortcutIDList_Original,
            },
            {
                {
                    LR"(public: virtual unsigned short const * __cdecl CTaskGroup::GetIconResource(void))",
                    LR"(public: virtual unsigned short const * __ptr64 __cdecl CTaskGroup::GetIconResource(void) __ptr64)",
                },
                (void**)&CTaskGroup_GetIconResource_Original,
                (void*)CTaskGroup_GetIconResource_Hook,
            },
            {
                {
                    LR"(protected: void __cdecl CTaskBand::_UpdateItemIcon(struct ITaskGroup *,struct ITaskItem *))",
                    LR"(protected: void __cdecl CTaskBand::_UpdateItemIcon(struct ITaskGroup * __ptr64,struct ITaskItem * __ptr64) __ptr64)",
                },
                (void**)&CTaskBand__UpdateItemIcon_Original,
                (void*)CTaskBand__UpdateItemIcon_Hook,
            },
            {
                {
                    LR"(public: virtual long __cdecl CTaskBand::Launch(struct ITaskGroup *,struct tagPOINT const &,enum LaunchFromTaskbarOptions))",
                    LR"(public: virtual long __cdecl CTaskBand::Launch(struct ITaskGroup * __ptr64,struct tagPOINT const & __ptr64,enum LaunchFromTaskbarOptions) __ptr64)",
                },
                (void**)&CTaskBand_Launch_Original,
                (void*)CTaskBand_Launch_Hook,
            },
            {
                {
                    LR"(public: virtual long __cdecl CTaskGroup::GetLauncherName(unsigned short * *))",
                    LR"(public: virtual long __cdecl CTaskGroup::GetLauncherName(unsigned short * __ptr64 * __ptr64) __ptr64)",
                },
                (void**)&CTaskGroup_GetLauncherName_Original,
                (void*)CTaskGroup_GetLauncherName_Hook,
            },
            {
                {
                    LR"(protected: long __cdecl CTaskListWnd::_GetJumpViewParams(struct ITaskBtnGroup *,struct ITaskItem *,int,bool,struct Windows::Internal::Shell::JumpView::IJumpViewParams * *)const )",
                    LR"(protected: long __cdecl CTaskListWnd::_GetJumpViewParams(struct ITaskBtnGroup * __ptr64,struct ITaskItem * __ptr64,int,bool,struct Windows::Internal::Shell::JumpView::IJumpViewParams * __ptr64 * __ptr64)const __ptr64)",
                },
                (void**)&CTaskListWnd__GetJumpViewParams_Original,
                (void*)CTaskListWnd__GetJumpViewParams_Hook,
            },
            {
                // Available from Windows 11.
                {
                    LR"(public: virtual long __cdecl CTaskBtnGroup::GetIcon(struct ITaskItem *,struct HICON__ * *))",
                    LR"(public: virtual long __cdecl CTaskBtnGroup::GetIcon(struct ITaskItem * __ptr64,struct HICON__ * __ptr64 * __ptr64) __ptr64)",
                },
                (void**)&CTaskBtnGroup_GetIcon_Original,
                (void*)CTaskBtnGroup_GetIcon_Hook,
                true,
            },
            {
                // Available until Windows 10.
                {
                    LR"(private: void __cdecl CTaskBtnGroup::_DrawRegularButton(struct HDC__ *,struct BUTTONRENDERINFO const &))",
                    LR"(private: void __cdecl CTaskBtnGroup::_DrawRegularButton(struct HDC__ * __ptr64,struct BUTTONRENDERINFO const & __ptr64) __ptr64)",
                },
                (void**)&CTaskBtnGroup__DrawRegularButton_Original,
                (void*)CTaskBtnGroup__DrawRegularButton_Hook,
                true,
            },
            {
                {
                    LR"(public: virtual struct ITaskGroup * __cdecl CTaskBtnGroup::GetGroup(void))",
                    LR"(public: virtual struct ITaskGroup * __ptr64 __cdecl CTaskBtnGroup::GetGroup(void) __ptr64)",
                },
                (void**)&CTaskBtnGroup_GetGroup_Original,
                (void*)CTaskBtnGroup_GetGroup_Hook,
            },
            {
                {
                    LR"(protected: struct ITaskBtnGroup * __cdecl CTaskListWnd::_GetTBGroupFromGroup(struct ITaskGroup *,int *))",
                    LR"(protected: struct ITaskBtnGroup * __ptr64 __cdecl CTaskListWnd::_GetTBGroupFromGroup(struct ITaskGroup * __ptr64,int * __ptr64) __ptr64)",
                },
                (void**)&CTaskListWnd__GetTBGroupFromGroup_Original,
            },
            {
                {
                    LR"(public: virtual int __cdecl CTaskListWnd::IsOnPrimaryTaskband(void))",
                    LR"(public: virtual int __cdecl CTaskListWnd::IsOnPrimaryTaskband(void) __ptr64)",
                },
                (void**)&CTaskListWnd_IsOnPrimaryTaskband_Original,
            },
            {
                {
                    LR"(protected: struct ITaskBtnGroup * __cdecl CTaskListWnd::_CreateTBGroup(struct ITaskGroup *,int))",
                    LR"(protected: struct ITaskBtnGroup * __ptr64 __cdecl CTaskListWnd::_CreateTBGroup(struct ITaskGroup * __ptr64,int) __ptr64)",
                },
                (void**)&CTaskListWnd__CreateTBGroup_Original,
                (void*)CTaskListWnd__CreateTBGroup_Hook,
            },
            {
                // Available from Windows 11.
                {
                    LR"(protected: void __cdecl CTaskBand::HandleTaskGroupSwitchItemAdded(struct winrt::Windows::Internal::ComposableShell::Multitasking::ISwitchItem const &))",
                    LR"(protected: void __cdecl CTaskBand::HandleTaskGroupSwitchItemAdded(struct winrt::Windows::Internal::ComposableShell::Multitasking::ISwitchItem const & __ptr64) __ptr64)",
                },
                (void**)&CTaskBand_HandleTaskGroupSwitchItemAdded_Original,
                (void*)CTaskBand_HandleTaskGroupSwitchItemAdded_Hook,
                true,
            },
            {
                // Available from Windows 11.
                {
                    LR"(public: virtual void __cdecl CTaskListWnd::GroupChanged(struct ITaskGroup *,enum winrt::WindowsUdk::UI::Shell::TaskGroupProperty))",
                    LR"(public: virtual void __cdecl CTaskListWnd::GroupChanged(struct ITaskGroup * __ptr64,enum winrt::WindowsUdk::UI::Shell::TaskGroupProperty) __ptr64)",
                },
                (void**)&CTaskListWnd_GroupChanged_Original,
                nullptr,
                true,
            },
            {
                {
                    LR"(public: virtual void __cdecl CTaskListWnd::HandleTaskGroupPinned(struct ITaskGroup *))",
                    LR"(public: virtual void __cdecl CTaskListWnd::HandleTaskGroupPinned(struct ITaskGroup * __ptr64) __ptr64)",
                },
                (void**)&CTaskListWnd_HandleTaskGroupPinned_Original,
            },
            {
                {
                    LR"(public: virtual void __cdecl CTaskListWnd::HandleTaskGroupUnpinned(struct ITaskGroup *))",
                    LR"(public: virtual void __cdecl CTaskListWnd::HandleTaskGroupUnpinned(struct ITaskGroup * __ptr64) __ptr64)",

                    // Before Windows 11 24H2.
                    LR"(public: virtual void __cdecl CTaskListWnd::HandleTaskGroupUnpinned(struct ITaskGroup *,enum HandleTaskGroupUnpinnedFlags))",
                    LR"(public: virtual void __cdecl CTaskListWnd::HandleTaskGroupUnpinned(struct ITaskGroup * __ptr64,enum HandleTaskGroupUnpinnedFlags) __ptr64)",
                },
                (void**)&CTaskListWnd_HandleTaskGroupUnpinned_Original,
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
            {
                {
                    LR"(protected: long __cdecl CTaskListWnd::_TaskCreated(struct ITaskGroup *,struct ITaskItem *,int))",
                    LR"(protected: long __cdecl CTaskListWnd::_TaskCreated(struct ITaskGroup * __ptr64,struct ITaskItem * __ptr64,int) __ptr64)",
                },
                (void**)&CTaskListWnd__TaskCreated_Original,
                (void*)CTaskListWnd__TaskCreated_Hook,
            },
        };

    HMODULE module;
    if (g_winVersion <= WinVersion::Win10) {
        module = GetModuleHandle(nullptr);
    } else {
        module = LoadLibrary(L"taskbar.dll");
        if (!module) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return FALSE;
        }
    }

    return HookSymbolsWithOnlineCacheFallback(module, symbolHooks,
                                              ARRAYSIZE(symbolHooks));
}

void LoadSettings() {
    PCWSTR pinnedItemsMode = Wh_GetStringSetting(L"pinnedItemsMode");
    g_settings.pinnedItemsMode = PinnedItemsMode::replace;
    if (wcscmp(pinnedItemsMode, L"keepInPlace") == 0) {
        g_settings.pinnedItemsMode = PinnedItemsMode::keepInPlace;
    } else if (wcscmp(pinnedItemsMode, L"keepInPlaceAndNoUngrouping") == 0) {
        g_settings.pinnedItemsMode =
            PinnedItemsMode::keepInPlaceAndNoUngrouping;
    }
    Wh_FreeStringSetting(pinnedItemsMode);

    g_settings.placeUngroupedItemsTogether =
        Wh_GetIntSetting(L"placeUngroupedItemsTogether");
    g_settings.useWindowIcons = Wh_GetIntSetting(L"useWindowIcons");

    g_settings.excludedProgramItems.clear();

    for (int i = 0;; i++) {
        PCWSTR program = Wh_GetStringSetting(L"excludedPrograms[%d]", i);

        bool hasProgram = *program;
        if (hasProgram) {
            std::wstring programUpper = program;
            LCMapStringEx(
                LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, &programUpper[0],
                static_cast<int>(programUpper.length()), &programUpper[0],
                static_cast<int>(programUpper.length()), nullptr, nullptr, 0);

            g_settings.excludedProgramItems.insert(std::move(programUpper));
        }

        Wh_FreeStringSetting(program);

        if (!hasProgram) {
            break;
        }
    }

    g_settings.customGroupNames.clear();
    g_settings.customGroupProgramItems.clear();

    for (int groupIndex = 0;; groupIndex++) {
        PCWSTR name = Wh_GetStringSetting(L"customGroups[%d].name", groupIndex);

        bool hasName = *name;
        if (hasName) {
            g_settings.customGroupNames.push_back(name);
        }

        Wh_FreeStringSetting(name);

        if (!hasName) {
            break;
        }

        for (int i = 0;; i++) {
            PCWSTR program = Wh_GetStringSetting(L"customGroups[%d].items[%d]",
                                                 groupIndex, i);

            bool hasProgram = *program;
            if (hasProgram) {
                std::wstring programUpper = program;
                LCMapStringEx(
                    LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, &programUpper[0],
                    static_cast<int>(programUpper.length()), &programUpper[0],
                    static_cast<int>(programUpper.length()), nullptr, nullptr,
                    0);

                g_settings.customGroupProgramItems.insert(
                    {std::move(programUpper), groupIndex + 1});
            }

            Wh_FreeStringSetting(program);

            if (!hasProgram) {
                break;
            }
        }
    }

    PCWSTR groupingMode = Wh_GetStringSetting(L"groupingMode");
    g_settings.groupingMode = GroupingMode::regular;
    if (wcscmp(groupingMode, L"inverse") == 0) {
        g_settings.groupingMode = GroupingMode::inverse;
    }
    Wh_FreeStringSetting(groupingMode);

    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_winVersion = GetWindowsVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (g_winVersion >= WinVersion::Win11 && g_settings.oldTaskbarOnWin11) {
        g_winVersion = WinVersion::Win10;
    }

    if (!HookTaskbarSymbols()) {
        return FALSE;
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    FARPROC kernelBaseCompareStringOrdinal =
        GetProcAddress(kernelBaseModule, "CompareStringOrdinal");
    Wh_SetFunctionHook((void*)kernelBaseCompareStringOrdinal,
                       (void*)CompareStringOrdinal_Hook,
                       (void**)&CompareStringOrdinal_Original);

    Wh_SetFunctionHook((void*)DPA_InsertPtr, (void*)DPA_InsertPtr_Hook,
                       (void**)&DPA_InsertPtr_Original);

    return TRUE;
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

// For pre-1.3 Windhawk compatibility.
void Wh_ModSettingsChanged() {
    Wh_Log(L"> pre-1.3");

    BOOL bReload = FALSE;
    Wh_ModSettingsChanged(&bReload);
}
