// ==WindhawkMod==
// @id              pinned-items-double-click
// @name            Open pinned items with double click
// @description     Only open pinned items when double clicking on them to avoid accidental clicks
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lversion
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
# Open pinned items with double click

Only open pinned items when double clicking on them to avoid accidental clicks.

![Demonstration](https://i.imgur.com/Si3siPm.gif)

Only Windows 10 64-bit and Windows 11 are supported. For other Windows versions
check out [7+ Taskbar Tweaker](https://tweaker.ramensoftware.com/).

**Note:** To customize the old taskbar on Windows 11 (if using Explorer Patcher
or a similar tool), enable the relevant option in the mod's settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    Explorer Patcher or a similar tool). Note: For Windhawk versions older
    than 1.3, you have to disable and re-enable the mod to apply this option.
*/
// ==/WindhawkModSettings==

#include <windowsx.h>

#include <algorithm>
#include <string>
#include <string_view>
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

bool IsDoubleClickDistance(DWORD pos1, DWORD pos2) {
    return abs(GET_X_LPARAM(pos1) - GET_X_LPARAM(pos2)) <=
               GetSystemMetrics(SM_CXDOUBLECLK) &&
           abs(GET_Y_LPARAM(pos1) - GET_Y_LPARAM(pos2)) <=
               GetSystemMetrics(SM_CYDOUBLECLK);
}

using CTaskBtnGroup_GetGroupType_t = int(WINAPI*)(PVOID pThis);
CTaskBtnGroup_GetGroupType_t CTaskBtnGroup_GetGroupType_Original;

using CTaskListWnd__HandleClick_t = void(WINAPI*)(PVOID pThis,
                                                  PVOID taskBtnGroup,
                                                  int taskItemIndex,
                                                  int clickAction,
                                                  int param4,
                                                  int param5);
CTaskListWnd__HandleClick_t CTaskListWnd__HandleClick_Original;
void WINAPI CTaskListWnd__HandleClick_Hook(PVOID pThis,
                                           PVOID taskBtnGroup,
                                           int taskItemIndex,
                                           int clickAction,
                                           int param4,
                                           int param5) {
    Wh_Log(L"> %d", clickAction);

    auto original = [&]() {
        CTaskListWnd__HandleClick_Original(pThis, taskBtnGroup, taskItemIndex,
                                           clickAction, param4, param5);
    };

    if (clickAction != 0) {
        return original();
    }

    // Group types:
    // 1 - Single item (or multiple uncombined items - Win10)
    // 2 - Pinned item
    // 3 - Multiple combined items
    int groupType = CTaskBtnGroup_GetGroupType_Original(taskBtnGroup);
    if (groupType != 2) {
        return original();
    }

    static ULONGLONG firstClickTickCount = 0;
    static DWORD firstClickMessagePos;
    static PVOID firstClickTaskBtnGroup;

    ULONGLONG tickCount = GetTickCount64();
    ULONGLONG messagePos = GetMessagePos();

    if (firstClickTickCount &&
        IsDoubleClickDistance(firstClickMessagePos, messagePos) &&
        firstClickTaskBtnGroup == taskBtnGroup &&
        tickCount - firstClickTickCount <= GetDoubleClickTime()) {
        // Double click detected, proceed.
        firstClickTickCount = 0;
        return original();
    }

    firstClickTickCount = tickCount;
    firstClickMessagePos = messagePos;
    firstClickTaskBtnGroup = taskBtnGroup;
}

using CTaskListWnd__HandleMouseButtonDown_t = void(WINAPI*)(PVOID pThis,
                                                            ULONGLONG param1,
                                                            const POINT* point,
                                                            bool isDoubleClick);
CTaskListWnd__HandleMouseButtonDown_t
    CTaskListWnd__HandleMouseButtonDown_Original;
void WINAPI CTaskListWnd__HandleMouseButtonDown_Hook(PVOID pThis,
                                                     ULONGLONG param1,
                                                     const POINT* point,
                                                     bool isDoubleClick) {
    Wh_Log(L">");

    // In Windows 10, double clicks on pinned items are ignored. Make all clicks
    // seem like single clicks.
    isDoubleClick = false;

    CTaskListWnd__HandleMouseButtonDown_Original(pThis, param1, point,
                                                 isDoubleClick);
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

void LoadSettings() {
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

    SYMBOL_HOOK symbolHooks[] = {
        {
            {
                LR"(public: virtual enum eTBGROUPTYPE __cdecl CTaskBtnGroup::GetGroupType(void))",
                LR"(public: virtual enum eTBGROUPTYPE __cdecl CTaskBtnGroup::GetGroupType(void) __ptr64)",
            },
            (void**)&CTaskBtnGroup_GetGroupType_Original,
        },
        {
            {
                LR"(protected: void __cdecl CTaskListWnd::_HandleClick(struct ITaskBtnGroup *,int,enum CTaskListWnd::eCLICKACTION,int,int))",
                LR"(protected: void __cdecl CTaskListWnd::_HandleClick(struct ITaskBtnGroup * __ptr64,int,enum CTaskListWnd::eCLICKACTION,int,int) __ptr64)",
            },
            (void**)&CTaskListWnd__HandleClick_Original,
            (void*)CTaskListWnd__HandleClick_Hook,
        },
        {
            // Windows 10 only.
            {
                LR"(protected: void __cdecl CTaskListWnd::_HandleMouseButtonDown(unsigned __int64,struct tagPOINT const &,bool))",
                LR"(protected: void __cdecl CTaskListWnd::_HandleMouseButtonDown(unsigned __int64,struct tagPOINT const & __ptr64,bool) __ptr64)",
            },
            (void**)&CTaskListWnd__HandleMouseButtonDown_Original,
            (void*)CTaskListWnd__HandleMouseButtonDown_Hook,
            true,
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

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        return FALSE;
    }

    return TRUE;
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
