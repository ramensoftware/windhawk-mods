// ==WindhawkMod==
// @id              taskbar-thumbnail-reorder
// @name            Taskbar Thumbnail Reorder
// @description     Reorder taskbar thumbnails with the left mouse button
// @version         1.0.3
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Thumbnail Reorder
Reorder taskbar thumbnails with the left mouse button.

Only Windows 10 64-bit and Windows 11 are supported. For other Windows version
check out [7+ Taskbar Tweaker](https://tweaker.ramensoftware.com/).

Note: To customize the old taskbar on Windows 11 (if using Explorer Patcher or a
similar tool), enable the relevant option in the mod's settings.

![demonstration](https://i.imgur.com/wGGe2RS.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    Explorer Patcher or a similar tool). Note: Disable and re-enable the mod to
    apply this option.
*/
// ==/WindhawkModSettings==

#include <commctrl.h>

#include <algorithm>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

struct {
    bool oldTaskbarOnWin11;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
};

WinVersion g_winVersion;

std::unordered_set<HWND> g_thumbnailWindows;

#if defined(__GNUC__) && __GNUC__ > 8
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t WINAPI
#elif defined(__GNUC__)
#define WINAPI_LAMBDA_RETURN(return_t) WINAPI->return_t
#else
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t
#endif

// https://www.geoffchappell.com/studies/windows/shell/comctl32/api/da/dpa/dpa.htm
typedef struct _DPA {
    int cpItems;
    PVOID* pArray;
    HANDLE hHeap;
    int cpCapacity;
    int cpGrow;
} DPA, *HDPA;

////////////////////////////////////////////////////////////////////////////////

using CTaskListThumbnailWnd__RefreshThumbnail_t = void(WINAPI*)(void* pThis,
                                                                int index);
CTaskListThumbnailWnd__RefreshThumbnail_t
    CTaskListThumbnailWnd__RefreshThumbnail;

using CTaskListThumbnailWnd_GetTaskGroup_t = void*(WINAPI*)(void* pThis);
CTaskListThumbnailWnd_GetTaskGroup_t CTaskListThumbnailWnd_GetTaskGroup;

using CTaskListThumbnailWnd_GetHoverIndex_t = int(WINAPI*)(void* pThis);
CTaskListThumbnailWnd_GetHoverIndex_t CTaskListThumbnailWnd_GetHoverIndex;

using CTaskListWnd__GetTBGroupFromGroup_t =
    void*(WINAPI*)(void* pThis, void* pTaskGroup, int* pTaskBtnGroupIndex);
CTaskListWnd__GetTBGroupFromGroup_t CTaskListWnd__GetTBGroupFromGroup;

////////////////////////////////////////////////////////////////////////////////

struct {
    // Reference: CTaskListThumbnailWnd::GetTaskGroup.
    size_t taskGroup;  // 22000: 0x148

    // Reference: CTaskListThumbnailWnd::_RegisterThumbBars.
    size_t thumbnailArray;  // 22000: 0x158

    // Reference: CTaskListThumbnailWnd::SetActiveItem.
    // A base address for thumbnail index values.
    // 0: Active index
    // 1: Something about keyboard focus
    // 2: Tracked index
    // 3: Pressed index
    // 4: Live preview index
    size_t indexValues;  // 22000: 0x288
} g_thumbnailOffsets;

LONG_PTR* ThumbnailTaskGroup(LONG_PTR lpMMThumbnailLongPtr) {
    return *(LONG_PTR**)(lpMMThumbnailLongPtr + g_thumbnailOffsets.taskGroup);
}

HDPA ThumbnailArray(LONG_PTR lpMMThumbnailLongPtr) {
    return *(HDPA*)(lpMMThumbnailLongPtr + g_thumbnailOffsets.thumbnailArray);
}

int* ThumbnailActiveIndex(LONG_PTR lpMMThumbnailLongPtr) {
    return (int*)(lpMMThumbnailLongPtr + g_thumbnailOffsets.indexValues);
}

int* ThumbnailTrackedIndex(LONG_PTR lpMMThumbnailLongPtr) {
    return (int*)(lpMMThumbnailLongPtr + g_thumbnailOffsets.indexValues) + 2;
}

int* ThumbnailPressedIndex(LONG_PTR lpMMThumbnailLongPtr) {
    return (int*)(lpMMThumbnailLongPtr + g_thumbnailOffsets.indexValues) + 3;
}

////////////////////////////////////////////////////////////////////////////////

bool MoveThumbnail(LONG_PTR lpMMThumbnailLongPtr, int indexFrom, int indexTo) {
    HDPA thumbnailArray = ThumbnailArray(lpMMThumbnailLongPtr);
    if (!thumbnailArray) {
        return false;
    }

    LONG_PTR** thumbs = (LONG_PTR**)thumbnailArray->pArray;

    LONG_PTR* thumbTemp = thumbs[indexFrom];
    if (indexFrom < indexTo) {
        memmove(&thumbs[indexFrom], &thumbs[indexFrom + 1],
                (indexTo - indexFrom) * sizeof(LONG_PTR*));
    } else {
        memmove(&thumbs[indexTo + 1], &thumbs[indexTo],
                (indexFrom - indexTo) * sizeof(LONG_PTR*));
    }
    thumbs[indexTo] = thumbTemp;

    int* activeIndex = ThumbnailActiveIndex(lpMMThumbnailLongPtr);
    if (*activeIndex == indexFrom) {
        *activeIndex = indexTo;
    } else if (*activeIndex == indexTo) {
        *activeIndex = indexFrom;
    }

    int* pressedIndex = ThumbnailPressedIndex(lpMMThumbnailLongPtr);
    if (*pressedIndex == indexFrom) {
        *pressedIndex = indexTo;
    } else if (*pressedIndex == indexTo) {
        *pressedIndex = indexFrom;
    }

    CTaskListThumbnailWnd__RefreshThumbnail((void*)lpMMThumbnailLongPtr,
                                            indexFrom);
    CTaskListThumbnailWnd__RefreshThumbnail((void*)lpMMThumbnailLongPtr,
                                            indexTo);

    return true;
}

bool MoveTaskInTaskList(LONG_PTR lpMMTaskListLongPtr,
                        LONG_PTR* taskGroup,
                        LONG_PTR* taskItemFrom,
                        LONG_PTR* taskItemTo) {
    LONG_PTR* taskBtnGroup = (LONG_PTR*)CTaskListWnd__GetTBGroupFromGroup(
        (void*)lpMMTaskListLongPtr, taskGroup, nullptr);
    if (!taskBtnGroup) {
        return false;
    }

    int taskBtnGroupType = (int)taskBtnGroup[8];
    if (taskBtnGroupType != 1 && taskBtnGroupType != 3) {
        return false;
    }

    HDPA buttonsArray = (HDPA)taskBtnGroup[7];

    int buttonsCount = buttonsArray->cpItems;
    LONG_PTR** buttons = (LONG_PTR**)buttonsArray->pArray;

    int indexFrom = -1;
    for (int i = 0; i < buttonsCount; i++) {
        if ((LONG_PTR*)buttons[i][4] == taskItemFrom) {
            indexFrom = i;
            break;
        }
    }

    if (indexFrom == -1) {
        return false;
    }

    int indexTo = -1;
    for (int i = 0; i < buttonsCount; i++) {
        if ((LONG_PTR*)buttons[i][4] == taskItemTo) {
            indexTo = i;
            break;
        }
    }

    if (indexTo == -1) {
        return false;
    }

    LONG_PTR* button = (LONG_PTR*)DPA_DeletePtr(buttonsArray, indexFrom);
    if (!button) {
        return false;
    }

    DPA_InsertPtr(buttonsArray, indexTo, button);

    HWND hMMTaskListWnd = *(HWND*)(lpMMTaskListLongPtr + 0x08);
    InvalidateRect(hMMTaskListWnd, nullptr, FALSE);

    return true;
}

bool MoveTaskInGroup(LONG_PTR* taskGroup,
                     LONG_PTR* taskItemFrom,
                     LONG_PTR* taskItemTo) {
    HDPA taskItemsArray = (HDPA)taskGroup[4];
    if (!taskItemsArray) {
        return false;
    }

    int taskItemsCount = taskItemsArray->cpItems;
    LONG_PTR** taskItems = (LONG_PTR**)taskItemsArray->pArray;

    int indexFrom = -1;
    for (int i = 0; i < taskItemsCount; i++) {
        if (taskItems[i] == taskItemFrom) {
            indexFrom = i;
            break;
        }
    }

    if (indexFrom == -1) {
        return false;
    }

    int indexTo = -1;
    for (int i = 0; i < taskItemsCount; i++) {
        if (taskItems[i] == taskItemTo) {
            indexTo = i;
            break;
        }
    }

    if (indexTo == -1) {
        return false;
    }

    LONG_PTR* taskItemTemp = taskItems[indexFrom];
    if (indexFrom < indexTo) {
        memmove(&taskItems[indexFrom], &taskItems[indexFrom + 1],
                (indexTo - indexFrom) * sizeof(LONG_PTR*));
    } else {
        memmove(&taskItems[indexTo + 1], &taskItems[indexTo],
                (indexFrom - indexTo) * sizeof(LONG_PTR*));
    }
    taskItems[indexTo] = taskItemTemp;

    for (HWND hWnd : g_thumbnailWindows) {
        LONG_PTR lpMMThumbnailLongPtr = GetWindowLongPtr(hWnd, 0);
        if (!lpMMThumbnailLongPtr) {
            continue;
        }

        LONG_PTR lpMMTaskListLongPtr =
            *(LONG_PTR*)(lpMMThumbnailLongPtr + 0x50) - 0x30;

        MoveTaskInTaskList(lpMMTaskListLongPtr, taskGroup, taskItemFrom,
                           taskItemTo);
    }

    return true;
}

bool MoveItemsFromThumbnail(LONG_PTR lpMMThumbnailLongPtr,
                            int indexFrom,
                            int indexTo) {
    HDPA thumbnailArray = ThumbnailArray(lpMMThumbnailLongPtr);
    if (!thumbnailArray || indexFrom < 0 ||
        indexFrom >= thumbnailArray->cpItems || indexTo < 0 ||
        indexTo >= thumbnailArray->cpItems || indexFrom == indexTo) {
        return false;
    }

    LONG_PTR* from = (LONG_PTR*)thumbnailArray->pArray[indexFrom];
    LONG_PTR* to = (LONG_PTR*)thumbnailArray->pArray[indexTo];

    LONG_PTR* taskItemFrom = (LONG_PTR*)from[3];
    LONG_PTR* taskItemTo = (LONG_PTR*)to[3];

    if (!MoveTaskInGroup(ThumbnailTaskGroup(lpMMThumbnailLongPtr), taskItemFrom,
                         taskItemTo)) {
        return false;
    }

    return MoveThumbnail(lpMMThumbnailLongPtr, indexFrom, indexTo);
}

////////////////////////////////////////////////////////////////////////////////

// wParam - TRUE to subclass, FALSE to unsubclass
// lParam - subclass data
UINT g_subclassRegisteredMsg = RegisterWindowMessage(
    L"Windhawk_SetWindowSubclassFromAnyThread_taskbar-thumbnail-reorder");

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
        [](int nCode, WPARAM wParam,
           LPARAM lParam) WINAPI_LAMBDA_RETURN(LRESULT) {
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
    SendMessage(hWnd, g_subclassRegisteredMsg, TRUE, (WPARAM)&param);

    UnhookWindowsHookEx(hook);

    return param.result;
}

LRESULT CALLBACK ThumbnailWindowSubclassProc(HWND hWnd,
                                             UINT uMsg,
                                             WPARAM wParam,
                                             LPARAM lParam,
                                             UINT_PTR uIdSubclass,
                                             DWORD_PTR dwRefData) {
    static int draggedIndex = -1;
    static bool dragDone;
    LRESULT result = 0;
    bool processed = false;

    switch (uMsg) {
        case WM_LBUTTONDOWN:
            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

            {
                LONG_PTR lpMMThumbnailLongPtr = GetWindowLongPtr(hWnd, 0);
                int trackedIndex = *ThumbnailTrackedIndex(lpMMThumbnailLongPtr);
                if (trackedIndex >= 0) {
                    dragDone = false;
                    draggedIndex = trackedIndex;
                    SetCapture(hWnd);
                }
            }
            break;

        case WM_MOUSEMOVE:
            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

            if (GetCapture() == hWnd) {
                LONG_PTR lpMMThumbnailLongPtr = GetWindowLongPtr(hWnd, 0);
                int trackedIndex = *ThumbnailTrackedIndex(lpMMThumbnailLongPtr);
                if (MoveItemsFromThumbnail(lpMMThumbnailLongPtr, draggedIndex,
                                           trackedIndex)) {
                    draggedIndex = trackedIndex;
                    dragDone = true;
                }
            }
            break;

        case WM_LBUTTONUP:
            if (GetCapture() == hWnd) {
                ReleaseCapture();
                if (dragDone) {
                    result = DefWindowProc(hWnd, uMsg, wParam, lParam);
                    processed = true;
                }
            }

            if (!processed)
                result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            break;

        case WM_TIMER:
            // Aero peek window of hovered thumbnail.
            if (wParam == 2006) {
                if (GetCapture() == hWnd) {
                    KillTimer(hWnd, wParam);
                    result = DefWindowProc(hWnd, uMsg, wParam, lParam);
                    processed = true;
                }
            }

            if (!processed)
                result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            break;

        case WM_DESTROY:
            g_thumbnailWindows.erase(hWnd);

            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            break;

        default:
            if (uMsg == g_subclassRegisteredMsg && !wParam)
                RemoveWindowSubclass(hWnd, ThumbnailWindowSubclassProc, 0);

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

void FindCurrentProcessThumbnailWindows() {
    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar found");
        return;
    }

    DWORD dwProcessId = 0;
    DWORD dwThreadId = GetWindowThreadProcessId(hTaskbarWnd, &dwProcessId);
    if (dwProcessId != GetCurrentProcessId()) {
        Wh_Log(L"Taskbar belongs to a different process");
        return;
    }

    EnumThreadWindows(
        dwThreadId,
        [](HWND hWnd, LPARAM lParam) WINAPI_LAMBDA_RETURN(BOOL) {
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0)
                return TRUE;

            if (wcsicmp(szClassName, L"TaskListThumbnailWnd") == 0)
                g_thumbnailWindows.insert(hWnd);

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
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd)
        return hWnd;

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName &&
        wcsicmp(lpClassName, L"TaskListThumbnailWnd") == 0) {
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
                                           LPVOID lpParam,
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
                                    LPVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd)
        return hWnd;

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName &&
        wcsicmp(lpClassName, L"TaskListThumbnailWnd") == 0) {
        Wh_Log(L"Thumbnail window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedThumbnailWindow(hWnd);
    }

    return hWnd;
}

bool InitOffsets() {
    const BYTE* p;

    /*
        Expected implementation:
        180044250 48 8b 81        MOV        RAX,qword ptr [this + 0x148]
                  48 01 00 00
        180044257 c3              RET
    */
    p = (const BYTE*)CTaskListThumbnailWnd_GetTaskGroup;
    if (p[0] == 0x48 && p[1] == 0x8b && p[2] == 0x81) {
        g_thumbnailOffsets.taskGroup = *(DWORD*)(p + 3);
    } else {
        Wh_Log(L"Unexpected GetTaskGroup implementation");
        return false;
    }

    // This worked since the first Windows 10 version, let's hope it will keep
    // working.
    g_thumbnailOffsets.thumbnailArray = g_thumbnailOffsets.taskGroup + 0x10;

    /*
        Expected implementation:
        180046e40 8b 81 78        MOV        EAX,dword ptr [this + 0x278]
                  02 00 00
        180046e46 c3              RET
    */
    p = (const BYTE*)CTaskListThumbnailWnd_GetHoverIndex;
    if (p[0] == 0x8b && p[1] == 0x81) {
        DWORD offset = *(DWORD*)(p + 2);
        offset += 0x10;  // extra vtables
        g_thumbnailOffsets.indexValues = offset - 0x08;
    } else {
        Wh_Log(L"Unexpected GetHoverIndex implementation");
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

bool HookSymbols(PCWSTR cacheId,
                 HMODULE module,
                 const SYMBOL_HOOK* symbolHooks,
                 size_t symbolHooksCount) {
    const WCHAR cacheVer = L'1';
    const WCHAR cacheSep = L'@';
    constexpr size_t cacheMaxSize = 10240;

    WCHAR cacheBuffer[cacheMaxSize + 1];
    std::wstring cacheStrKey = std::wstring(L"symbol-cache-") + cacheId;
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
                             cacheSep, &newSystemCacheStr,
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

            int noAddressMatchCount = 0;
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

void LoadSettings() {
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    g_winVersion = GetWindowsVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    LoadSettings();

    SYMBOL_HOOK symbolHooks[] = {
        {{
             LR"(private: void __cdecl CTaskListThumbnailWnd::_RefreshThumbnail(int) __ptr64)",
             LR"(private: void __cdecl CTaskListThumbnailWnd::_RefreshThumbnail(int))",
         },
         (void**)&CTaskListThumbnailWnd__RefreshThumbnail},
        {{
             LR"(public: virtual struct ITaskGroup * __ptr64 __cdecl CTaskListThumbnailWnd::GetTaskGroup(void)const __ptr64)",
             LR"(public: virtual struct ITaskGroup * __cdecl CTaskListThumbnailWnd::GetTaskGroup(void)const )",
         },
         (void**)&CTaskListThumbnailWnd_GetTaskGroup},
        {{
             LR"(public: virtual int __cdecl CTaskListThumbnailWnd::GetHoverIndex(void)const __ptr64)",
             LR"(public: virtual int __cdecl CTaskListThumbnailWnd::GetHoverIndex(void)const )",
         },
         (void**)&CTaskListThumbnailWnd_GetHoverIndex},
        {{
             LR"(protected: struct ITaskBtnGroup * __ptr64 __cdecl CTaskListWnd::_GetTBGroupFromGroup(struct ITaskGroup * __ptr64,int * __ptr64) __ptr64)",
             LR"(protected: struct ITaskBtnGroup * __cdecl CTaskListWnd::_GetTBGroupFromGroup(struct ITaskGroup *,int *))",
         },
         (void**)&CTaskListWnd__GetTBGroupFromGroup}};

    HMODULE module;

    if (g_winVersion <= WinVersion::Win10 || g_settings.oldTaskbarOnWin11) {
        module = GetModuleHandle(nullptr);
        if (!HookSymbols(L"explorer.exe", module, symbolHooks,
                         ARRAYSIZE(symbolHooks))) {
            return FALSE;
        }
    } else {
        module = LoadLibrary(L"taskbar.dll");
        if (!module) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return FALSE;
        }

        if (!HookSymbols(L"taskbar.dll", module, symbolHooks,
                         ARRAYSIZE(symbolHooks))) {
            return FALSE;
        }
    }

    if (!InitOffsets()) {
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

    WNDCLASS wndclass;
    if (GetClassInfo(module, L"TaskListThumbnailWnd", &wndclass)) {
        FindCurrentProcessThumbnailWindows();
        for (HWND hWnd : g_thumbnailWindows) {
            Wh_Log(L"Thumbnail window found: %08X", (DWORD)(ULONG_PTR)hWnd);
            SubclassThumbnailWindow(hWnd);
        }
    }

    return TRUE;
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
