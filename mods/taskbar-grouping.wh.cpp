// ==WindhawkMod==
// @id              taskbar-grouping
// @name            Disable grouping on the taskbar
// @description     Causes a separate button to be created on the taskbar for each new window
// @version         1.3.11
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lshlwapi -lversion
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

Only Windows 10 64-bit and Windows 11 are supported. For older Windows versions
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
- placeUngroupedItemsTogether: "0"
  $name: Place ungrouped items together
  $description: >-
    Place each newly opened item next to existing items it would group with.
  $options:
  - 0: Off
  - 1: On
  - nonPinnedOnly: On, ignore pinned items
- useWindowIcons: false
  $name: Use window icons
  $description: >-
    By default, application icons are used. Enable this option to use window
    icons instead. Usually it doesn't matter, an example where it does is an
    open folder window - with application icons, the icon on the taskbar is
    always the icon of Explorer, while with window icons, the icon changes
    depending on the open folder.
- windowIconsPrograms: [example1.exe]
  $name: Window icon programs
  $description: >-
    Each entry is a name, path, or application ID that affects window icon behavior.
    When "Use window icons" is enabled, these programs will be excluded. When "Use window icons" is disabled, only these 
    programs will use window icons.
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
    ExplorerPatcher or a similar tool).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <psapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <winrt/base.h>

#include <atomic>
#include <functional>
#include <string>
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

enum class PlaceUngroupedItemsTogetherMode {
    off,
    on,
    nonPinnedOnly,
};

enum class GroupingMode {
    regular,
    inverse,
};

struct {
    PinnedItemsMode pinnedItemsMode;
    PlaceUngroupedItemsTogetherMode placeUngroupedItemsTogether;
    bool useWindowIcons;
    std::unordered_set<std::wstring> windowIconProgramItems;
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
    Win11_24H2,
};

constexpr WCHAR kCustomGroupPrefix[] = L"Windhawk_Group_";
constexpr size_t kCustomGroupPrefixLen = ARRAYSIZE(kCustomGroupPrefix) - 1;

WinVersion g_winVersion;

std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

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

        if (resolvedWindowProcessPathLen > 0) {
            LCMapStringEx(
                LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE,
                resolvedWindowProcessPath, resolvedWindowProcessPathLen + 1,
                resolvedWindowProcessPathUpper,
                resolvedWindowProcessPathLen + 1, nullptr, nullptr, 0);

            programFileNameUpper =
                wcsrchr(resolvedWindowProcessPathUpper, L'\\');
            if (programFileNameUpper) {
                programFileNameUpper++;
                if (!*programFileNameUpper) {
                    programFileNameUpper = nullptr;
                }
            }
        } else {
            *resolvedWindowProcessPath = L'\0';
            *resolvedWindowProcessPathUpper = L'\0';
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

        // Determine whether to use window icon for this task group.
        bool shouldUseWindowIcon = g_settings.useWindowIcons;

        // Check if the program is in the window icon programs list.
        if (!g_settings.windowIconProgramItems.empty()) {
            PCWSTR appId = CTaskGroup_GetAppID_Original(pThis);
            if (appId && *appId) {
                WCHAR appIdClean[MAX_PATH];
                if (!RemoveAppIdSuffix(appIdClean, appId)) {
                    wcscpy_s(appIdClean, appId);
                }

                WCHAR appIdUpper[MAX_PATH];
                int len = wcslen(appIdClean);
                LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE,
                              appIdClean, len + 1,
                              appIdUpper, len + 1, nullptr, nullptr, 0);

                bool inList = false;

                if (g_settings.windowIconProgramItems.contains(appIdUpper)) {
                    inList = true;
                }
                // Check process path and file name by window handle.
                else {
                    PCWSTR suffix = FindAppIdSuffix(appId);
                    if (suffix && suffix[4] == L'w') {
                        DWORD hwndValue = 0;
                        if (swscanf(suffix + 5, L"%08X", &hwndValue) == 1 && hwndValue != 0) {
                            HWND hwnd = (HWND)(DWORD_PTR)hwndValue;
                            DWORD dwProcessId = 0;
                            if (GetWindowThreadProcessId(hwnd, &dwProcessId)) {
                                HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
                                                              FALSE, dwProcessId);
                                if (hProcess) {
                                    WCHAR processPath[MAX_PATH];
                                    DWORD dwSize = ARRAYSIZE(processPath);
                                    if (QueryFullProcessImageName(hProcess, 0, processPath, &dwSize)) {
                                        WCHAR processPathUpper[MAX_PATH];
                                        LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE,
                                                      processPath, dwSize + 1,
                                                      processPathUpper, dwSize + 1,
                                                      nullptr, nullptr, 0);

                                        // Check full path.
                                        if (g_settings.windowIconProgramItems.contains(processPathUpper)) {
                                            inList = true;
                                        }
                                        // Check file name.
                                        else {
                                            PCWSTR fileName = wcsrchr(processPathUpper, L'\\');
                                            if (fileName) {
                                                fileName++;
                                                if (*fileName && g_settings.windowIconProgramItems.contains(fileName)) {
                                                    inList = true;
                                                }
                                            }
                                        }
                                    }
                                    CloseHandle(hProcess);
                                }
                            }
                        }
                    }
                }

                // When useWindowIcons is enabled, listed programs are excluded.
                // When useWindowIcons is disabled, only listed programs use window icons.
                shouldUseWindowIcon = (g_settings.useWindowIcons != inList);
            }
        }

        if (shouldUseWindowIcon) {
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

    if (g_settings.placeUngroupedItemsTogether ==
            PlaceUngroupedItemsTogetherMode::off ||
        i != DA_LAST || !p) {
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

        if (g_settings.placeUngroupedItemsTogether ==
            PlaceUngroupedItemsTogetherMode::nonPinnedOnly) {
            bool pinned = CTaskGroup_GetFlags_Original(taskGroupIter) & 1;
            if (pinned) {
                continue;
            }
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
    bool taskGroupIsPinned = CTaskGroup_GetFlags_Original(taskGroup) & 1;

    if (isPrimaryTaskbar && numItems == 1) {
        HandleUnsuffixedInstanceOnTaskDestroyed(taskList_TaskListUI, taskGroup);
    }

    LONG_PTR ret = original();

    if (isPrimaryTaskbar && numItems == 0) {
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
        {R"(?GetNumItems@CTaskGroup@@UEAAHXZ)",
         &CTaskGroup_GetNumItems_Original},
        {R"(?SetAppID@CTaskGroup@@UEAAJPEBG@Z)", &CTaskGroup_SetAppID_Original},
        {R"(?GetFlags@CTaskGroup@@UEBAKXZ)", &CTaskGroup_GetFlags_Original},
        {R"(?UpdateFlags@CTaskGroup@@UEAAJKK@Z)",
         &CTaskGroup_UpdateFlags_Original},
        {R"(?GetTitleText@CTaskGroup@@UEAAJPEAUITaskItem@@PEAGH@Z)",
         &CTaskGroup_GetTitleText_Original},
        {R"(?SetTip@CTaskGroup@@UEAAJPEBG@Z)", &CTaskGroup_SetTip_Original},
        {R"(?GetIconId@CTaskGroup@@UEAAJPEAUITaskItem@@PEAH@Z)",
         &CTaskGroup_GetIconId_Original},
        {R"(?SetIconId@CTaskGroup@@UEAAJPEAUITaskItem@@H@Z)",
         &CTaskGroup_SetIconId_Original},
        {R"(?DoesWindowMatch@CTaskGroup@@UEAAJPEAUHWND__@@PEBU_ITEMIDLIST_ABSOLUTE@@PEBGPEAW4WINDOWMATCHCONFIDENCE@@PEAPEAUITaskItem@@@Z)",
         &CTaskGroup_DoesWindowMatch_Original},
        {R"(?_MatchWindow@CTaskBand@@IEAAJPEAUHWND__@@PEBU_ITEMIDLIST_ABSOLUTE@@PEBGW4WINDOWMATCHCONFIDENCE@@PEAPEAUITaskGroup@@PEAPEAUITaskItem@@@Z)",
         &CTaskBand__MatchWindow_Original},
        {R"(?GetGroupType@CTaskBtnGroup@@UEAA?AW4eTBGROUPTYPE@@XZ)",
         &CTaskBtnGroup_GetGroupType_Original},
        {R"(?v_WndProc@CTaskBand@@MEAA_JPEAUHWND__@@I_K_J@Z)",
         &CTaskBand_v_WndProc_Original, CTaskBand_v_WndProc_Hook},
        {R"(?_HandleItemResolved@CTaskBand@@IEAAXPEAURESOLVEDWINDOW@@PEAUITaskListUI@@PEAUITaskGroup@@PEAUITaskItem@@@Z)",
         &CTaskBand__HandleItemResolved_Original,
         CTaskBand__HandleItemResolved_Hook},
        {R"(?_Launch@CLauncherTask@CTaskBand@@AEAAJXZ)",
         &CTaskBand__Launch_Original, CTaskBand__Launch_Hook},
        {R"(?GetAppID@CTaskGroup@@UEAAPEBGXZ)", &CTaskGroup_GetAppID_Original,
         CTaskGroup_GetAppID_Hook},
        {R"(?IsImmersiveGroup@CTaskGroup@@UEAA_NXZ)",
         &CTaskGroup_IsImmersiveGroup_Original,
         CTaskGroup_IsImmersiveGroup_Hook},
        {R"(?GetApplicationIDList@CTaskGroup@@UEAAPEAU_ITEMIDLIST_ABSOLUTE@@XZ)",
         &CTaskGroup_GetApplicationIDList_Original},
        {R"(?GetShortcutIDList@CTaskGroup@@UEAAPEBU_ITEMIDLIST_ABSOLUTE@@XZ)",
         &CTaskGroup_GetShortcutIDList_Original,
         CTaskGroup_GetShortcutIDList_Hook},
        {R"(?SetShortcutIDList@CTaskGroup@@UEAAJPEBU_ITEMIDLIST_ABSOLUTE@@@Z)",
         &CTaskGroup_SetShortcutIDList_Original},
        {R"(?GetIconResource@CTaskGroup@@UEAAPEBGXZ)",
         &CTaskGroup_GetIconResource_Original, CTaskGroup_GetIconResource_Hook},
        {R"(?_UpdateItemIcon@CTaskBand@@IEAAXPEAUITaskGroup@@PEAUITaskItem@@@Z)",
         &CTaskBand__UpdateItemIcon_Original, CTaskBand__UpdateItemIcon_Hook},
        {R"(?Launch@CTaskBand@@UEAAJPEAUITaskGroup@@AEBUtagPOINT@@W4LaunchFromTaskbarOptions@@@Z)",
         &CTaskBand_Launch_Original, CTaskBand_Launch_Hook},
        {R"(?GetLauncherName@CTaskGroup@@UEAAJPEAPEAG@Z)",
         &CTaskGroup_GetLauncherName_Original, CTaskGroup_GetLauncherName_Hook},
        {R"(?_GetJumpViewParams@CTaskListWnd@@IEBAJPEAUITaskBtnGroup@@PEAUITaskItem@@H_NPEAPEAUIJumpViewParams@JumpView@Shell@Internal@Windows@ABI@@@Z)",
         &CTaskListWnd__GetJumpViewParams_Original,
         CTaskListWnd__GetJumpViewParams_Hook},
        {R"(?ShowJumpView@CTaskListWnd@@UEAAJPEAUITaskGroup@@PEAUITaskItem@@_N@Z)",
         &CTaskListWnd_ShowJumpView_Original, CTaskListWnd_ShowJumpView_Hook},
        // {// Available from Windows 11.
        //  R"()", &CTaskBtnGroup_GetIcon_Original, CTaskBtnGroup_GetIcon_Hook,
        //  true},
        {// Available until Windows 10.
         R"(?_DrawRegularButton@CTaskBtnGroup@@AEAAXPEAUHDC__@@AEBUBUTTONRENDERINFO@@@Z)",
         &CTaskBtnGroup__DrawRegularButton_Original,
         CTaskBtnGroup__DrawRegularButton_Hook, true},
        {R"(?GetGroup@CTaskBtnGroup@@UEAAPEAUITaskGroup@@XZ)",
         &CTaskBtnGroup_GetGroup_Original, CTaskBtnGroup_GetGroup_Hook},
        {R"(?_GetTBGroupFromGroup@CTaskListWnd@@IEAAPEAUITaskBtnGroup@@PEAUITaskGroup@@PEAH@Z)",
         &CTaskListWnd__GetTBGroupFromGroup_Original},
        {R"(?IsOnPrimaryTaskband@CTaskListWnd@@UEAAHXZ)",
         &CTaskListWnd_IsOnPrimaryTaskband_Original},
        {R"(?_CreateTBGroup@CTaskListWnd@@IEAAPEAUITaskBtnGroup@@PEAUITaskGroup@@H@Z)",
         &CTaskListWnd__CreateTBGroup_Original,
         CTaskListWnd__CreateTBGroup_Hook},
        {// Available from Windows 11.
         R"(?HandleTaskGroupSwitchItemAdded@CTaskBand@@IEAAJPEAUISwitchItem@Multitasking@ComposableShell@Internal@Windows@ABI@@@Z)",
         &CTaskBand_HandleTaskGroupSwitchItemAdded_Original,
         CTaskBand_HandleTaskGroupSwitchItemAdded_Hook, true},
        // {// Available from Windows 11.
        //  R"()", &CTaskListWnd_GroupChanged_Original, nullptr, true},
        {R"(?HandleTaskGroupPinned@CTaskListWnd@@UEAAXPEAUITaskGroup@@@Z)",
         &CTaskListWnd_HandleTaskGroupPinned_Original},
        {R"(?HandleTaskGroupUnpinned@CTaskListWnd@@UEAAXPEAUITaskGroup@@W4HandleTaskGroupUnpinnedFlags@@@Z)",
         &CTaskListWnd_HandleTaskGroupUnpinned_Original},
        {// An older variant, see the newer variant below.
         R"(?TaskDestroyed@CTaskListWnd@@UEAAJPEAUITaskGroup@@PEAUITaskItem@@W4TaskDestroyedFlags@@@Z)",
         &CTaskListWnd_TaskDestroyed_Original, CTaskListWnd_TaskDestroyed_Hook,
         true},
        // {// A newer variant seen in insider builds.
        //  R"()", &CTaskListWnd_TaskDestroyed_2_Original,
        //  CTaskListWnd_TaskDestroyed_2_Hook, true},
        {R"(?_TaskCreated@CTaskListWnd@@IEAAJPEAUITaskGroup@@PEAUITaskItem@@H@Z)",
         &CTaskListWnd__TaskCreated_Original, CTaskListWnd__TaskCreated_Hook},
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

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized) {
        if (IsExplorerPatcherModule(module)) {
            HookExplorerPatcherSymbols(module);
        }
    }

    return module;
}

bool HookTaskbarSymbols() {
    // Taskbar.dll, explorer.exe
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] =  //
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
                // Available until Windows 10.
                {LR"(private: void __cdecl CTaskBtnGroup::_DrawRegularButton(struct HDC__ *,struct BUTTONRENDERINFO const &))"},
                &CTaskBtnGroup__DrawRegularButton_Original,
                CTaskBtnGroup__DrawRegularButton_Hook,
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
                // Available from Windows 11.
                {LR"(public: virtual void __cdecl CTaskListWnd::GroupChanged(struct ITaskGroup *,enum winrt::WindowsUdk::UI::Shell::TaskGroupProperty))"},
                &CTaskListWnd_GroupChanged_Original,
                nullptr,
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

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
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

    PCWSTR placeUngroupedItemsTogetherMode =
        Wh_GetStringSetting(L"placeUngroupedItemsTogether");
    g_settings.placeUngroupedItemsTogether =
        PlaceUngroupedItemsTogetherMode::off;
    if (wcscmp(placeUngroupedItemsTogetherMode, L"1") == 0) {
        g_settings.placeUngroupedItemsTogether =
            PlaceUngroupedItemsTogetherMode::on;
    } else if (wcscmp(placeUngroupedItemsTogetherMode, L"nonPinnedOnly") == 0) {
        g_settings.placeUngroupedItemsTogether =
            PlaceUngroupedItemsTogetherMode::nonPinnedOnly;
    }
    Wh_FreeStringSetting(placeUngroupedItemsTogetherMode);

    g_settings.useWindowIcons = Wh_GetIntSetting(L"useWindowIcons");

    g_settings.windowIconProgramItems.clear();

    for (int i = 0;; i++) {
        PCWSTR program = Wh_GetStringSetting(L"windowIconsPrograms[%d]", i);

        bool hasProgram = *program;
        if (hasProgram) {
            std::wstring programUpper = program;
            LCMapStringEx(
                LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, &programUpper[0],
                static_cast<int>(programUpper.length()), &programUpper[0],
                static_cast<int>(programUpper.length()), nullptr, nullptr, 0);

            g_settings.windowIconProgramItems.insert(std::move(programUpper));
        }

        Wh_FreeStringSetting(program);
        
        if (!hasProgram) {
            break;
        }
    }

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
    } else if (!HookTaskbarSymbols()) {
        return FALSE;
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

    auto kernelBaseCompareStringOrdinal =
        (decltype(&CompareStringOrdinal))GetProcAddress(kernelBaseModule,
                                                        "CompareStringOrdinal");
    WindhawkUtils::Wh_SetFunctionHookT(kernelBaseCompareStringOrdinal,
                                       CompareStringOrdinal_Hook,
                                       &CompareStringOrdinal_Original);

    WindhawkUtils::Wh_SetFunctionHookT(DPA_InsertPtr, DPA_InsertPtr_Hook,
                                       &DPA_InsertPtr_Original);

    WindhawkUtils::Wh_SetFunctionHookT(DPA_DeletePtr, DPA_DeletePtr_Hook,
                                       &DPA_DeletePtr_Original);

    g_initialized = true;

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

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
