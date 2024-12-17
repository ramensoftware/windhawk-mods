// ==WindhawkMod==
// @id              virtual-desktop-taskbar-order
// @name            Virtual Desktop Preserve Taskbar Order
// @description     The order on the taskbar isn't preserved between virtual desktop switches, this mod fixes it
// @version         1.0.4
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lversion -lwininet
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
# Virtual Desktop Preserve Taskbar Order

The order on the taskbar isn't preserved between virtual desktop switches, this
mod fixes it.

Only Windows 11 is currently supported. For older Windows versions check out [7+
Taskbar Tweaker](https://tweaker.ramensoftware.com/).

![Demonstration](https://i.imgur.com/ie8Q9cl.gif)
*/
// ==/WindhawkModReadme==

#include <algorithm>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

#include <commctrl.h>
#include <wininet.h>

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
};

WinVersion g_winVersion;
DWORD g_taskbarThreadId;
HWND g_hTaskbarWnd;

#pragma region offsets

void* CTaskListWnd_GetFocusedBtn;
void* CTaskBand__EnumExistingImmersiveApps;
void* CApplicationViewManager__GetViewInFocus;

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

HDPA* EV_MM_TASKLIST_BUTTON_GROUPS_HDPA(LONG_PTR lp) {
    static size_t offset = OffsetFromAssembly(CTaskListWnd_GetFocusedBtn, 0xE0);

    return (HDPA*)(lp + offset);
}

LONG_PTR* EV_TASK_SW_APP_VIEW_MGR(LONG_PTR lp) {
    static size_t offset =
        OffsetFromAssembly(CTaskBand__EnumExistingImmersiveApps, 0x220);

    return (LONG_PTR*)(lp + offset);
}

size_t EV_APP_VIEW_MGR_APP_ARRAY_LOCK_OFFSET() {
    static size_t offset = OffsetFromAssembly(
        CApplicationViewManager__GetViewInFocus, 0x210, "lea");

    return offset;
}

SRWLOCK* EV_APP_VIEW_MGR_APP_ARRAY_LOCK(LONG_PTR lp) {
    return (SRWLOCK*)(lp + EV_APP_VIEW_MGR_APP_ARRAY_LOCK_OFFSET());
}

LONG_PTR** EV_APP_VIEW_MGR_APP_ARRAY(LONG_PTR lp) {
    return (LONG_PTR**)(lp + EV_APP_VIEW_MGR_APP_ARRAY_LOCK_OFFSET() + 0x08);
}

size_t* EV_APP_VIEW_MGR_APP_ARRAY_SIZE(LONG_PTR lp) {
    return (size_t*)(lp + EV_APP_VIEW_MGR_APP_ARRAY_LOCK_OFFSET() + 0x10);
}

#pragma endregion  // offsets

using CTaskBtnGroup_GetGroup_t = void*(WINAPI*)(void* pThis);
CTaskBtnGroup_GetGroup_t CTaskBtnGroup_GetGroup;

using CTaskBtnGroup_GetNumItems_t = int(WINAPI*)(void* pThis);
CTaskBtnGroup_GetNumItems_t CTaskBtnGroup_GetNumItems;

using CTaskBtnGroup_GetTaskItem_t = void*(WINAPI*)(void* pThis, int index);
CTaskBtnGroup_GetTaskItem_t CTaskBtnGroup_GetTaskItem;

using CTaskGroup_DoesWindowMatch_t =
    HRESULT(WINAPI*)(LPVOID pThis,
                     HWND hCompareWnd,
                     LPVOID pCompareItemIdList,  // ITEMIDLIST*
                     WCHAR* pCompareAppId,
                     int* pnMatch,  // WINDOWMATCHCONFIDENCE*
                     LONG_PTR** p_task_item);
CTaskGroup_DoesWindowMatch_t CTaskGroup_DoesWindowMatch_Original;

using CTaskBand_ViewVirtualDesktopChanged_t = HRESULT(WINAPI*)(
    LPVOID pThis,
    LPVOID applicationView  // IApplicationView*
);
CTaskBand_ViewVirtualDesktopChanged_t
    CTaskBand_ViewVirtualDesktopChanged_Original;

using CTaskListWnd_TryMoveGroup_t =
    bool(WINAPI*)(LPVOID pThis,
                  LPVOID taskGroup,  // ITaskGroup *
                  DWORD index);
CTaskListWnd_TryMoveGroup_t CTaskListWnd_TryMoveGroup_Original;

int g_doesWindowMatchCalls;

HRESULT WINAPI
CTaskGroup_DoesWindowMatch_Hook(LPVOID pThis,
                                HWND hCompareWnd,
                                LPVOID pCompareItemIdList,  // ITEMIDLIST*
                                WCHAR* pCompareAppId,
                                int* pnMatch,  // WINDOWMATCHCONFIDENCE*
                                LONG_PTR** p_task_item) {
    g_doesWindowMatchCalls++;

    HRESULT ret = CTaskGroup_DoesWindowMatch_Original(
        pThis, hCompareWnd, pCompareItemIdList, pCompareAppId, pnMatch,
        p_task_item);

    g_doesWindowMatchCalls--;

    return ret;
}

LONG_PTR g_tryMoveGroup_taskListLongPtr;
LONG_PTR* g_tryMoveGroup_taskGroup;

bool WINAPI CTaskListWnd_TryMoveGroup_Hook(LPVOID pThis,
                                           LPVOID taskGroup,  // ITaskGroup *
                                           DWORD index) {
    g_tryMoveGroup_taskListLongPtr = (LONG_PTR)pThis - 0x28;
    g_tryMoveGroup_taskGroup = (LONG_PTR*)taskGroup;

    bool ret = CTaskListWnd_TryMoveGroup_Original(pThis, taskGroup, index);

    g_tryMoveGroup_taskListLongPtr = 0;
    g_tryMoveGroup_taskGroup = nullptr;

    return ret;
}

#pragma region pointer_redirection

#ifdef _WIN64
#define POINTER_REDIRECTION_ASM_COMMAND "\xFF\x25\xF2\xFF\xFF\xFF"
#else
#define POINTER_REDIRECTION_ASM_COMMAND "\xE8\x00\x00\x00\x00\x58\xFF\x60\xF7"
#endif

#define POINTER_REDIRECTION_SIGNATURE "ptr_redr"

struct POINTER_REDIRECTION {
    void* pOriginalAddress = nullptr;
    void* pRedirectionAddress = nullptr;
    BYTE bAsmCommand[sizeof(POINTER_REDIRECTION_ASM_COMMAND)] =
        POINTER_REDIRECTION_ASM_COMMAND;
    BYTE bSignature[sizeof(POINTER_REDIRECTION_SIGNATURE)] =
        POINTER_REDIRECTION_SIGNATURE;
};

#define POINTER_REDIRECTION_VAR \
    __attribute__((section(".text"))) constinit POINTER_REDIRECTION

void PatchPtr(void** ppAddress, void* pPtr) {
    DWORD dwOldProtect, dwOtherProtect;

    VirtualProtect(ppAddress, sizeof(void*), PAGE_EXECUTE_READWRITE,
                   &dwOldProtect);
    *ppAddress = pPtr;
    VirtualProtect(ppAddress, sizeof(void*), dwOldProtect, &dwOtherProtect);
}

void PointerRedirectionAdd(void** pp, void* pNew, POINTER_REDIRECTION* ppr) {
    PatchPtr(&ppr->pOriginalAddress, *pp);
    PatchPtr(&ppr->pRedirectionAddress, pNew);

    PatchPtr(pp, &ppr->bAsmCommand);
}

void PointerRedirectionRemove(void** pp, POINTER_REDIRECTION* ppr) {
    POINTER_REDIRECTION* pprTemp;

    if (*pp != ppr->bAsmCommand) {
        pprTemp =
            (POINTER_REDIRECTION*)((BYTE*)*pp -
                                   offsetof(POINTER_REDIRECTION, bAsmCommand));
        while (pprTemp->pOriginalAddress != ppr->bAsmCommand) {
            pprTemp = (POINTER_REDIRECTION*)((BYTE*)pprTemp->pOriginalAddress -
                                             offsetof(POINTER_REDIRECTION,
                                                      bAsmCommand));
        }

        PatchPtr(&pprTemp->pOriginalAddress, ppr->pOriginalAddress);
    } else {
        PatchPtr(pp, ppr->pOriginalAddress);
    }
}

#pragma endregion  // pointer_redirection

POINTER_REDIRECTION_VAR prTaskGroupRelease;
POINTER_REDIRECTION_VAR prTaskItemRelease;

LONG_PTR* g_taskGroupVirtualDesktopReleased;
LONG_PTR* g_taskItemVirtualDesktopReleased;

ULONG WINAPI TaskGroupReleaseHook(LONG_PTR this_ptr) {
    ULONG ulRet;

    ulRet = ((ULONG(WINAPI*)(LONG_PTR))prTaskGroupRelease.pOriginalAddress)(
        this_ptr);
    if (ulRet > 0 && g_doesWindowMatchCalls == 0) {
        g_taskGroupVirtualDesktopReleased = (LONG_PTR*)this_ptr;
    }

    return ulRet;
}

ULONG WINAPI TaskItemReleaseHook(LONG_PTR this_ptr) {
    ULONG ulRet;

    ulRet = ((ULONG(WINAPI*)(LONG_PTR))prTaskItemRelease.pOriginalAddress)(
        this_ptr);
    if (ulRet > 0 && g_doesWindowMatchCalls == 0) {
        g_taskItemVirtualDesktopReleased = (LONG_PTR*)this_ptr;
    }

    return ulRet;
}

void OnButtonGroupInserted(LONG_PTR lpTaskSwLongPtr,
                           HDPA hButtonGroupsDpa,
                           int nButtonGroupIndex) {
    LONG_PTR* plp = (LONG_PTR*)hButtonGroupsDpa;
    int button_groups_count = (int)plp[0];
    LONG_PTR** button_groups = (LONG_PTR**)plp[1];
    LONG_PTR* button_group = button_groups[nButtonGroupIndex];

    int buttons_count = CTaskBtnGroup_GetNumItems(button_group);
    if (buttons_count == 0) {
        return;
    }

    LONG_PTR* task_group = (LONG_PTR*)CTaskBtnGroup_GetGroup(button_group);
    if (!task_group) {
        return;
    }

    LONG_PTR* first_task_item =
        (LONG_PTR*)CTaskBtnGroup_GetTaskItem(button_group, 0);
    if (!first_task_item) {
        return;
    }

    plp = *(LONG_PTR**)task_group;
    void** ppTaskGroupRelease = (void**)&plp[2];
    PointerRedirectionAdd(ppTaskGroupRelease, (void*)TaskGroupReleaseHook,
                          &prTaskGroupRelease);

    plp = *(LONG_PTR**)first_task_item;
    void** ppTaskItemRelease = (void**)&plp[2];
    PointerRedirectionAdd(ppTaskItemRelease, (void*)TaskItemReleaseHook,
                          &prTaskItemRelease);

    LONG_PTR lpAppViewMgr = *EV_TASK_SW_APP_VIEW_MGR(lpTaskSwLongPtr);
    SRWLOCK* pArrayLock = EV_APP_VIEW_MGR_APP_ARRAY_LOCK(lpAppViewMgr);

    AcquireSRWLockExclusive(pArrayLock);

    LONG_PTR* lpArray = *EV_APP_VIEW_MGR_APP_ARRAY(lpAppViewMgr);
    size_t nArraySize = *EV_APP_VIEW_MGR_APP_ARRAY_SIZE(lpAppViewMgr);

    int nMatchCount = 0;
    size_t nRightNeighbourItemIndex = nArraySize;

    // Stage one: move all items in lpArray matching the items
    // in the newly inserted group to the beginning of the array.
    // Their amount is maintained in nMatchCount.
    // Also, find nRightNeighbourItemIndex, which is the index of
    // the item in lpArray which will be the first one before the
    // found matching items.

    for (size_t i = 0; i < nArraySize; i++) {
        g_taskGroupVirtualDesktopReleased = NULL;
        g_taskItemVirtualDesktopReleased = NULL;

        LONG_PTR this_ptr = (LONG_PTR)(lpTaskSwLongPtr + 0x70);
        plp = *(LONG_PTR**)this_ptr;

        ReleaseSRWLockExclusive(pArrayLock);

        CTaskBand_ViewVirtualDesktopChanged_Original((LPVOID)this_ptr,
                                                     (LPVOID)lpArray[i]);

        AcquireSRWLockExclusive(pArrayLock);

        if (lpArray != *EV_APP_VIEW_MGR_APP_ARRAY(lpAppViewMgr) ||
            nArraySize != *EV_APP_VIEW_MGR_APP_ARRAY_SIZE(lpAppViewMgr)) {
            // Something went wrong, abort.
            nMatchCount = 0;
            break;
        }

        if (!g_taskGroupVirtualDesktopReleased) {
            continue;
        }

        if (g_taskGroupVirtualDesktopReleased != task_group) {
            if (nRightNeighbourItemIndex == nArraySize) {
                for (int j = nButtonGroupIndex + 1; j < button_groups_count;
                     j++) {
                    LONG_PTR* check_button_group = button_groups[j];
                    LONG_PTR* check_task_group =
                        (LONG_PTR*)CTaskBtnGroup_GetGroup(check_button_group);
                    if (g_taskGroupVirtualDesktopReleased == check_task_group) {
                        // The current item in lpArray is from the same group
                        // of at least one of the items in button_groups to the
                        // right of the newly added item.
                        nRightNeighbourItemIndex = i - nMatchCount;
                        break;
                    }
                }
            }

            continue;
        }

        if (!g_taskItemVirtualDesktopReleased) {
            continue;
        }

        for (int j = 0; j < buttons_count; j++) {
            LONG_PTR* task_item =
                (LONG_PTR*)CTaskBtnGroup_GetTaskItem(button_group, j);

            if (g_taskItemVirtualDesktopReleased == task_item) {
                // The current item in lpArray matches one of the
                // buttons in the newly added item.
                if (i > (size_t)nMatchCount) {
                    LONG_PTR lpTemp = lpArray[i];
                    memmove(&lpArray[nMatchCount + 1], &lpArray[nMatchCount],
                            (i - nMatchCount) * sizeof(LONG_PTR));
                    lpArray[nMatchCount] = lpTemp;
                }

                nMatchCount++;
                break;
            }
        }
    }

    PointerRedirectionRemove(ppTaskGroupRelease, &prTaskGroupRelease);
    PointerRedirectionRemove(ppTaskItemRelease, &prTaskItemRelease);

    // Stage two: move the found items before the item in
    // nRightNeighbourItemIndex.

    if (nRightNeighbourItemIndex == nArraySize) {
        // By default, move to the right end.
        nRightNeighbourItemIndex = nArraySize - nMatchCount;
    }

    if (nMatchCount > 0 && nRightNeighbourItemIndex > 0) {
        LONG_PTR* lpBuffer = (LONG_PTR*)HeapAlloc(
            GetProcessHeap(), 0, nMatchCount * sizeof(LONG_PTR));
        if (lpBuffer) {
            memcpy(lpBuffer, lpArray, nMatchCount * sizeof(LONG_PTR));
            memmove(&lpArray[0], &lpArray[nMatchCount],
                    nRightNeighbourItemIndex * sizeof(LONG_PTR));
            memcpy(&lpArray[nRightNeighbourItemIndex], lpBuffer,
                   nMatchCount * sizeof(LONG_PTR));

            HeapFree(GetProcessHeap(), 0, lpBuffer);
        }
    }

    ReleaseSRWLockExclusive(pArrayLock);
}

void ComFuncVirtualDesktopFixAfterDPA_InsertPtr(HDPA pdpa, int index, void* p) {
    if (index == INT_MAX) {
        return;
    }

    if (!g_tryMoveGroup_taskListLongPtr) {
        return;
    }

    HDPA hButtonGroupsDpa =
        *EV_MM_TASKLIST_BUTTON_GROUPS_HDPA(g_tryMoveGroup_taskListLongPtr);
    if (!hButtonGroupsDpa || pdpa != hButtonGroupsDpa) {
        return;
    }

    HWND hTaskSwWnd = (HWND)GetProp(g_hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return;
    }

    LONG_PTR lpTaskSwLongPtr = GetWindowLongPtr(hTaskSwWnd, 0);
    if (!lpTaskSwLongPtr) {
        return;
    }

    OnButtonGroupInserted(lpTaskSwLongPtr, pdpa, index);
}

bool InitializeTaskbarVariables(HWND hTaskbarWnd) {
    DWORD processId;
    DWORD taskbarThreadId = GetWindowThreadProcessId(hTaskbarWnd, &processId);

    if (!taskbarThreadId) {
        Wh_Log(L"GetWindowThreadProcessId() failed for taskbar %08X",
               (DWORD)(ULONG_PTR)hTaskbarWnd);
        return false;
    }

    if (processId != GetCurrentProcessId()) {
        Wh_Log(L"Taskbar %08X is from another process",
               (DWORD)(ULONG_PTR)hTaskbarWnd);
        return false;
    }

    Wh_Log(L"Initialized for taskbar %08X", (DWORD)(ULONG_PTR)hTaskbarWnd);

    g_taskbarThreadId = taskbarThreadId;
    g_hTaskbarWnd = hTaskbarWnd;
    return true;
}

using DPA_InsertPtr_t = decltype(&DPA_InsertPtr);
DPA_InsertPtr_t DPA_InsertPtr_Original;
auto WINAPI DPA_InsertPtr_Hook(HDPA hdpa, int i, void* p) {
    Wh_Log(L">");

    auto ret = DPA_InsertPtr_Original(hdpa, i, p);

    if (GetCurrentThreadId() == g_taskbarThreadId) {
        ComFuncVirtualDesktopFixAfterDPA_InsertPtr(hdpa, i, p);
    }

    return ret;
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
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd) {
        return hWnd;
    }

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName && wcsicmp(lpClassName, L"Shell_TrayWnd") == 0) {
        Wh_Log(L"Taskbar window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        InitializeTaskbarVariables(hWnd);
    }

    return hWnd;
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

    if (puPtrLen)
        *puPtrLen = uPtrLen;

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetWindowsVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo)
        return WinVersion::Unsupported;

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000)
                return WinVersion::Win10;
            else
                return WinVersion::Win11;
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
    const WCHAR cacheSep = L'@';
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
    constexpr WCHAR kModIdForCache[] = L"virtual-desktop-taskbar-order";

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

BOOL Wh_ModInit() {
    Wh_Log(L">");

    g_winVersion = GetWindowsVersion();
    if (g_winVersion < WinVersion::Win11) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: virtual struct ITaskGroup * __cdecl CTaskBtnGroup::GetGroup(void))"},
            (void**)&CTaskBtnGroup_GetGroup,
        },
        {
            {LR"(public: virtual int __cdecl CTaskBtnGroup::GetNumItems(void))"},
            (void**)&CTaskBtnGroup_GetNumItems,
        },
        {
            {LR"(public: virtual struct ITaskItem * __cdecl CTaskBtnGroup::GetTaskItem(int))"},
            (void**)&CTaskBtnGroup_GetTaskItem,
        },
        {
            {LR"(public: virtual long __cdecl CTaskGroup::DoesWindowMatch(struct HWND__ *,struct _ITEMIDLIST_ABSOLUTE const *,unsigned short const *,enum WINDOWMATCHCONFIDENCE *,struct ITaskItem * *))"},
            (void**)&CTaskGroup_DoesWindowMatch_Original,
            (void*)CTaskGroup_DoesWindowMatch_Hook,
        },
        {
            {LR"(public: virtual bool __cdecl CTaskListWnd::TryMoveGroup(struct ITaskGroup *,unsigned int))"},
            (void**)&CTaskListWnd_TryMoveGroup_Original,
            (void*)CTaskListWnd_TryMoveGroup_Hook,
        },
        {
            {LR"(public: virtual long __cdecl CTaskBand::ViewVirtualDesktopChanged(struct IApplicationView *))"},
            (void**)&CTaskBand_ViewVirtualDesktopChanged_Original,
        },
        // For offsets:
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::GetFocusedBtn(struct ITaskGroup * *,int *))"},
            (void**)&CTaskListWnd_GetFocusedBtn,
        },
        {
            {LR"(protected: void __cdecl CTaskBand::_EnumExistingImmersiveApps(void))"},
            (void**)&CTaskBand__EnumExistingImmersiveApps,
        },
    };

    HMODULE taskbarModule = LoadLibrary(L"taskbar.dll");
    if (!taskbarModule) {
        Wh_Log(L"Couldn't load taskbar.dll");
        return FALSE;
    }

    if (!HookSymbolsWithOnlineCacheFallback(taskbarModule, taskbarDllHooks,
                                            ARRAYSIZE(taskbarDllHooks))) {
        return FALSE;
    }

    // twinui.pcshell.dll
    SYMBOL_HOOK twinuiPcshellSymbolHooks[] = {
        // For offsets:
        {
            {LR"(public: virtual long __cdecl CApplicationViewManager::GetViewInFocus(struct IApplicationView * *))"},
            (void**)&CApplicationViewManager__GetViewInFocus,
        },
    };

    HMODULE twinuiPcshellModule = LoadLibrary(L"twinui.pcshell.dll");
    if (!twinuiPcshellModule) {
        Wh_Log(L"Couldn't load twinui.pcshell.dll");
        return FALSE;
    }

    if (!HookSymbolsWithOnlineCacheFallback(
            twinuiPcshellModule, twinuiPcshellSymbolHooks,
            ARRAYSIZE(twinuiPcshellSymbolHooks))) {
        return FALSE;
    }

    Wh_SetFunctionHook((void*)DPA_InsertPtr, (void*)DPA_InsertPtr_Hook,
                       (void**)&DPA_InsertPtr_Original);

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    if (hTaskbarWnd) {
        InitializeTaskbarVariables(hTaskbarWnd);
    }

    return TRUE;
}
