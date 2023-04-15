// ==WindhawkMod==
// @id              taskbar-grouping
// @name            Disable grouping on the taskbar
// @description     Causes a separate button to be created on the taskbar for each new window
// @version         1.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshlwapi -lversion
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

Only Windows 10 64-bit and Windows 11 are supported. For other Windows version
check out [7+ Taskbar Tweaker](https://tweaker.ramensoftware.com/).

**Note:** To customize the old taskbar on Windows 11 (if using Explorer Patcher
or a similar tool), enable the relevant option in the mod's settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- keepPinnedItemsSeparated: true
  $name: Keep pinned items separated
  $description: >-
    If enabled, pinned items will be kept separated from running instances. If
    not enabled, the first running instance will replace the pinned item. Note
    that the current implementation has some limitations, for example, closing
    the first window will turn it back into a pinned item even if there are
    other running instances on the taskbar.
- customGroups:
  - - name: Group 1
      $name: Group name
      $description: >-
        Must not be empty. Will be shown on the taskbar if labels are shown.
    - items: [group1-program1.exe, group1-program2.exe]
      $name: Process names/paths
  $name: Custom groups
  $description: >-
    Each custom group is a list of names/paths of programs that will be grouped
    together.
- excludedPrograms: [excluded1.exe]
  $name: Excluded programs
  $description: >-
    Each entry is a name or path of a program that the mod will ignore. Excluded
    programs will keep their own grouping behavior. Usually that means that each
    program will be grouped separately, but sometimes there are custom grouping
    rules, e.g. Chrome creates a group for each browser profile.
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    Explorer Patcher or a similar tool). Note: For Windhawk versions older
    than 1.3, you have to disable and re-enable the mod to apply this option.
*/
// ==/WindhawkModSettings==

#include <shlobj.h>
#include <shlwapi.h>

#include <algorithm>
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

struct {
    bool keepPinnedItemsSeparated;
    std::unordered_set<std::wstring> excludedProgramPaths;
    std::unordered_set<std::wstring> excludedProgramNames;
    std::vector<std::wstring> customGroupNames;
    std::unordered_map<std::wstring, int> customGroupProgramPaths;
    std::unordered_map<std::wstring, int> customGroupProgramNames;
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

bool g_inTaskBandLaunch = false;

using CTaskGroup_GetApplicationIDList_t = ITEMIDLIST*(WINAPI*)(PVOID pThis);
CTaskGroup_GetApplicationIDList_t CTaskGroup_GetApplicationIDList_Original;

using CTaskGroup_GetShortcutIDList_t = const ITEMIDLIST*(WINAPI*)(PVOID pThis);
CTaskGroup_GetShortcutIDList_t CTaskGroup_GetShortcutIDList_Original;
const ITEMIDLIST* WINAPI CTaskGroup_GetShortcutIDList_Hook(PVOID pThis) {
    Wh_Log(L">");

    // Fixes launching a new instance on middle click or Shift+click for some
    // apps. Actually I think that might be a Windows bug.
    if (g_inTaskBandLaunch) {
        return CTaskGroup_GetApplicationIDList_Original(pThis);
    }

    return CTaskGroup_GetShortcutIDList_Original(pThis);
}

using CTaskGroup_GetNumItems_t = int(WINAPI*)(PVOID pThis);
CTaskGroup_GetNumItems_t CTaskGroup_GetNumItems_Original;

using CTaskBand__MatchWindow_t = HRESULT(WINAPI*)(PVOID pThis,
                                                  HWND hWnd,
                                                  const ITEMIDLIST* idList,
                                                  PCWSTR appId,
                                                  int windowMatchConfidence,
                                                  PVOID* taskGroup,
                                                  PVOID* taskItem);
CTaskBand__MatchWindow_t CTaskBand__MatchWindow_Original;

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
    Wh_Log(L"==========");
    Wh_Log(L"Resolved new item:");
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

    auto original = [&]() {
        CTaskBand__HandleItemResolved_Original(pThis, resolvedWindow,
                                               taskListUI, taskGroup, taskItem);
    };

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

    int customGroup = 0;

    if (resolvedWindowProcessPathLen > 0) {
        if (g_settings.excludedProgramPaths.contains(
                resolvedWindowProcessPathUpper)) {
            Wh_Log(L"Excluding %s", resolvedWindowProcessPath);
            return original();
        }

        if (programFileNameUpper &&
            g_settings.excludedProgramNames.contains(programFileNameUpper)) {
            Wh_Log(L"Excluding %s", resolvedWindowProcessPath);
            return original();
        }

        if (auto it = g_settings.customGroupProgramPaths.find(
                resolvedWindowProcessPathUpper);
            it != g_settings.customGroupProgramPaths.end()) {
            customGroup = it->second;
        } else if (programFileNameUpper) {
            if (auto it = g_settings.customGroupProgramNames.find(
                    programFileNameUpper);
                it != g_settings.customGroupProgramNames.end()) {
                customGroup = it->second;
            }
        }
    }

    if (!customGroup) {
        IUnknown* taskGroupMatched;
        IUnknown* taskItemMatched;
        HRESULT hr = CTaskBand__MatchWindow_Original(
            pThis, resolvedWindow->hButtonWnd, resolvedWindow->pAppItemIdList,
            resolvedWindow->szAppIdStr, 1, (void**)&taskGroupMatched,
            (void**)&taskItemMatched);
        if (FAILED(hr)) {
            // Nothing to group with, resolve normally.
            return original();
        }

        bool isMatchPinned =
            CTaskGroup_GetNumItems_Original(taskGroupMatched) == 0;

        if (taskGroupMatched) {
            taskGroupMatched->Release();
        }

        if (taskItemMatched) {
            taskItemMatched->Release();
        }

        if (!g_settings.keepPinnedItemsSeparated && isMatchPinned) {
            // Will group with a pinned item, resolve normally.
            return original();
        }
    }

    if (resolvedWindowProcessPathLen > 0) {
        wcscpy(resolvedWindow->szPathStr, resolvedWindowProcessPath);
        Wh_Log(L"New path: %s", resolvedWindow->szPathStr);
    }

    if (resolvedWindow->pAppItemIdList) {
        ILFree(resolvedWindow->pAppItemIdList);
        resolvedWindow->pAppItemIdList = nullptr;
    }

    if (customGroup) {
        wsprintf(resolvedWindow->szAppIdStr, L"%s%d", kCustomGroupPrefix,
                 customGroup);
        Wh_Log(L"Custom group AppId: %s", resolvedWindow->szAppIdStr);
    } else {
        size_t len = wcslen(resolvedWindow->szAppIdStr);
        size_t newLen = len + 9;
        if (newLen < MAX_PATH) {
            if (resolvedWindow->hButtonWnd) {
                wsprintf(resolvedWindow->szAppIdStr + len, L"_%08X",
                         (DWORD)(DWORD_PTR)resolvedWindow->hButtonWnd);
            } else {
                static DWORD counter = GetTickCount();
                wsprintf(resolvedWindow->szAppIdStr + len, L"~%08X", ++counter);
            }

            Wh_Log(L"New AppId: %s", resolvedWindow->szAppIdStr);
        } else {
            Wh_Log(L"AppId is too long: %s", resolvedWindow->szAppIdStr);
        }
    }

    original();
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

using CTaskGroup_GetIconId_t = HRESULT(WINAPI*)(PVOID pThis,
                                                PVOID taskItem,
                                                int* id);
CTaskGroup_GetIconId_t CTaskGroup_GetIconId_Original;
HRESULT WINAPI CTaskGroup_GetIconId_Hook(PVOID pThis, PVOID taskItem, int* id) {
    Wh_Log(L">");

    // If more than one instance of a UWP app is open, only the first instance
    // gets an icon. The other instances get a blank icon. We fix it below by
    // replacing the task group with the task group with the original AppId on
    // the taskbar, if such item exists. If not, the fix doesn't work.

    HRESULT ret = CTaskGroup_GetIconId_Original(pThis, taskItem, id);
    if (FAILED(ret)) {
        return ret;
    }

    PVOID taskBand = GetTaskBand();
    if (!taskBand) {
        return ret;
    }

    auto isUpperHex = [](PCWSTR start, PCWSTR end) {
        for (PCWSTR p = start; p != end; p++) {
            if ((*p < '0' || *p > '9') && (*p < 'A' || *p > 'F')) {
                return false;
            }
        }
        return true;
    };

    PCWSTR appId = CTaskGroup_GetAppID_Original(pThis);
    size_t appIdLen = wcslen(appId);
    if (appIdLen <= 9 || !isUpperHex(&appId[appIdLen - 8], &appId[appIdLen]) ||
        appId[appIdLen - 9] != L'~') {
        return ret;
    }

    WCHAR appIdOriginal[MAX_PATH];
    wcsncpy_s(appIdOriginal, appId, appIdLen - 9);

    IUnknown* taskGroupMatched;
    IUnknown* taskItemMatched;
    HRESULT hr = CTaskBand__MatchWindow_Original(
        taskBand, nullptr, nullptr, appIdOriginal, 1, (void**)&taskGroupMatched,
        (void**)&taskItemMatched);
    if (FAILED(hr)) {
        return ret;
    }

    if (taskGroupMatched) {
        ret = CTaskGroup_GetIconId_Original(taskGroupMatched, taskItemMatched,
                                            id);
    }

    if (taskGroupMatched) {
        taskGroupMatched->Release();
    }

    if (taskItemMatched) {
        taskItemMatched->Release();
    }

    return ret;
}

using CTaskGroup_GetLauncherName_t = HRESULT(WINAPI*)(PVOID pThis,
                                                      LPWSTR* ppwsz);
CTaskGroup_GetLauncherName_t CTaskGroup_GetLauncherName_Original;
HRESULT WINAPI CTaskGroup_GetLauncherName_Hook(PVOID pThis, LPWSTR* ppwsz) {
    Wh_Log(L">");

    PCWSTR appId = CTaskGroup_GetAppID_Original(pThis);
    if (wcsncmp(appId, kCustomGroupPrefix, kCustomGroupPrefixLen) == 0) {
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
                 size_t symbolHooksCount) {
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

bool HookTaskbarSymbols() {
    SYMBOL_HOOK symbolHooks[] = {
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
                LR"(public: virtual int __cdecl CTaskGroup::GetNumItems(void))",
                LR"(public: virtual int __cdecl CTaskGroup::GetNumItems(void) __ptr64)",
            },
            (void**)&CTaskGroup_GetNumItems_Original,
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
        },
        {
            {
                LR"(public: virtual long __cdecl CTaskGroup::GetIconId(struct ITaskItem *,int *))",
                LR"(public: virtual long __cdecl CTaskGroup::GetIconId(struct ITaskItem * __ptr64,int * __ptr64) __ptr64)",
            },
            (void**)&CTaskGroup_GetIconId_Original,
            (void*)CTaskGroup_GetIconId_Hook,
        },
        {
            {
                LR"(public: virtual long __cdecl CTaskGroup::GetLauncherName(unsigned short * *))",
                LR"(public: virtual long __cdecl CTaskGroup::GetLauncherName(unsigned short * __ptr64 * __ptr64) __ptr64)",
            },
            (void**)&CTaskGroup_GetLauncherName_Original,
            (void*)CTaskGroup_GetLauncherName_Hook,
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

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

void LoadSettings() {
    g_settings.keepPinnedItemsSeparated =
        Wh_GetIntSetting(L"keepPinnedItemsSeparated");

    g_settings.excludedProgramPaths.clear();
    g_settings.excludedProgramNames.clear();

    for (int i = 0;; i++) {
        PCWSTR program = Wh_GetStringSetting(L"excludedPrograms[%d]", i);

        bool hasProgram = *program;
        if (hasProgram) {
            std::wstring programUpper = program;
            LCMapStringEx(
                LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, &programUpper[0],
                static_cast<int>(programUpper.length()), &programUpper[0],
                static_cast<int>(programUpper.length()), nullptr, nullptr, 0);

            if (wcschr(program, L'\\')) {
                g_settings.excludedProgramPaths.insert(std::move(programUpper));
            } else {
                g_settings.excludedProgramNames.insert(std::move(programUpper));
            }
        }

        Wh_FreeStringSetting(program);

        if (!hasProgram) {
            break;
        }
    }

    g_settings.customGroupNames.clear();
    g_settings.customGroupProgramPaths.clear();
    g_settings.customGroupProgramNames.clear();

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

                if (wcschr(program, L'\\')) {
                    g_settings.customGroupProgramPaths.insert(
                        {std::move(programUpper), groupIndex + 1});
                } else {
                    g_settings.customGroupProgramNames.insert(
                        {std::move(programUpper), groupIndex + 1});
                }
            }

            Wh_FreeStringSetting(program);

            if (!hasProgram) {
                break;
            }
        }
    }

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
