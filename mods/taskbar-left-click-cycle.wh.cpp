// ==WindhawkMod==
// @id              taskbar-left-click-cycle
// @name            Cycle through taskbar windows on click
// @description     Makes clicking on combined taskbar items cycle through windows instead of opening thumbnail previews
// @version         1.1.1
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
# Cycle through taskbar windows on click

Makes clicking on combined taskbar items cycle through windows instead of
opening thumbnail previews. It's still possible to open thumbnail previews by
holding the Ctrl key while clicking.

In addition, makes Win+# hotkeys (Win+1, Win+2, etc.) cycle through taskbar
windows.

Only Windows 10 64-bit and Windows 11 are supported. For other Windows versions
check out [7+ Taskbar Tweaker](https://tweaker.ramensoftware.com/).

**Note:** To customize the old taskbar on Windows 11 (if using ExplorerPatcher
or a similar tool), enable the relevant option in the mod's settings.

![Demonstration](https://i.imgur.com/ecYYtGU.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    ExplorerPatcher or a similar tool).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <psapi.h>

#include <atomic>

struct {
    bool oldTaskbarOnWin11;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;

std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

bool g_inHandleWinNumHotKey;

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

    auto original = [=]() {
        CTaskListWnd__HandleClick_Original(pThis, taskBtnGroup, taskItemIndex,
                                           clickAction, param4, param5);
    };

    // Group types:
    // 1 - Single item or multiple uncombined items
    // 2 - Pinned item
    // 3 - Multiple combined items
    int groupType = CTaskBtnGroup_GetGroupType_Original(taskBtnGroup);
    if (groupType != 3) {
        return original();
    }

    constexpr int kClick = 0;
    constexpr int kForward = 1;
    constexpr int kBack = 2;
    constexpr int kCtrlClick = 4;

    int newClickAction = clickAction;
    if (g_inHandleWinNumHotKey) {
        if (clickAction == kForward || clickAction == kBack) {
            newClickAction = kCtrlClick;
        } else if (clickAction == kCtrlClick) {
            newClickAction = kForward;
        }

        Wh_Log(L"-> %d", newClickAction);
    } else {
        if (clickAction == kClick) {
            newClickAction = kCtrlClick;
        } else if (clickAction == kCtrlClick) {
            newClickAction = kClick;
        }

        Wh_Log(L"-> %d", newClickAction);
    }

    CTaskListWnd__HandleClick_Original(pThis, taskBtnGroup, taskItemIndex,
                                       newClickAction, param4, param5);
}

using CTaskListWnd_HandleWinNumHotKey_t = HRESULT(WINAPI*)(void* pThis,
                                                           short param1,
                                                           WORD param2);
CTaskListWnd_HandleWinNumHotKey_t CTaskListWnd_HandleWinNumHotKey_Original;
HRESULT WINAPI CTaskListWnd_HandleWinNumHotKey_Hook(void* pThis,
                                                    short param1,
                                                    WORD param2) {
    Wh_Log(L">");

    g_inHandleWinNumHotKey = true;

    HRESULT ret =
        CTaskListWnd_HandleWinNumHotKey_Original(pThis, param1, param2);

    g_inHandleWinNumHotKey = false;

    return ret;
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

bool HookExplorerPatcherSymbols(HMODULE explorerPatcherModule) {
    if (g_explorerPatcherInitialized.exchange(true)) {
        return true;
    }

    if (g_winVersion >= WinVersion::Win11) {
        g_winVersion = WinVersion::Win10;
    }

    struct EXPLORER_PATCHER_HOOK {
        PCSTR symbol;
        void** pOriginalFunction;
        void* hookFunction = nullptr;
        bool optional = false;
    };

    EXPLORER_PATCHER_HOOK hooks[] = {
        {R"(?GetGroupType@CTaskBtnGroup@@UEAA?AW4eTBGROUPTYPE@@XZ)",
         (void**)&CTaskBtnGroup_GetGroupType_Original},
        {R"(?_HandleClick@CTaskListWnd@@IEAAXPEAUITaskBtnGroup@@HW4eCLICKACTION@1@HH@Z)",
         (void**)&CTaskListWnd__HandleClick_Original,
         (void*)CTaskListWnd__HandleClick_Hook},
        {R"(?HandleWinNumHotKey@CTaskListWnd@@UEAAJFG@Z)",
         (void**)&CTaskListWnd_HandleWinNumHotKey_Original,
         (void*)CTaskListWnd_HandleWinNumHotKey_Hook},
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

    if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    return succeeded;
}

bool HandleModuleIfExplorerPatcher(HMODULE module) {
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

    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) !=
        0) {
        return true;
    }

    Wh_Log(L"ExplorerPatcher taskbar loaded: %s", moduleFileName);
    return HookExplorerPatcherSymbols(module);
}

void HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            HandleModuleIfExplorerPatcher(hMods[i]);
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized) {
        HandleModuleIfExplorerPatcher(module);
    }

    return module;
}

bool HookTaskbarSymbols() {
    HMODULE module;
    if (g_winVersion <= WinVersion::Win10) {
        module = GetModuleHandle(nullptr);
    } else {
        module = LoadLibrary(L"taskbar.dll");
        if (!module) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return false;
        }
    }

    // Taskbar.dll, explorer.exe
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual enum eTBGROUPTYPE __cdecl CTaskBtnGroup::GetGroupType(void))"},
            &CTaskBtnGroup_GetGroupType_Original,
        },
        {
            {LR"(protected: void __cdecl CTaskListWnd::_HandleClick(struct ITaskBtnGroup *,int,enum CTaskListWnd::eCLICKACTION,int,int))"},
            &CTaskListWnd__HandleClick_Original,
            CTaskListWnd__HandleClick_Hook,
        },
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleWinNumHotKey(short,unsigned short))"},
            &CTaskListWnd_HandleWinNumHotKey_Original,
            CTaskListWnd_HandleWinNumHotKey_Hook,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

void LoadSettings() {
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

    HandleLoadedExplorerPatcher();

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    FARPROC pKernelBaseLoadLibraryExW =
        GetProcAddress(kernelBaseModule, "LoadLibraryExW");
    Wh_SetFunctionHook((void*)pKernelBaseLoadLibraryExW,
                       (void*)LoadLibraryExW_Hook,
                       (void**)&LoadLibraryExW_Original);

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

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;

    return TRUE;
}
