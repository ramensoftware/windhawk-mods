// ==WindhawkMod==
// @id              taskbar-left-click-cycle
// @name            Cycle through taskbar windows on click
// @description     Makes clicking on combined taskbar items cycle through windows instead of opening thumbnail previews, and double-clicking minimizes all windows of the app
// @version         1.2.0
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

Double-clicking a combined taskbar group minimizes all of its windows.

In addition, makes Win+# hotkeys (Win+1, Win+2, etc.) cycle through taskbar
windows.

Only Windows 10 64-bit and Windows 11 are supported. For older Windows versions
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
- doubleClickMinimize: true
  $name: Double-click to minimize all windows
  $description: >-
    Double-clicking a combined taskbar group minimizes all of its windows.
- doubleClickTime: 0
  $name: Double-click time (ms)
  $description: >-
    Maximum time in milliseconds between two clicks to count as a double-click.
    Set to 0 to use the Windows system double-click time (usually 500 ms).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <psapi.h>

#include <atomic>

struct {
    bool oldTaskbarOnWin11;
    bool doubleClickMinimize;
    int doubleClickTime;
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

bool g_shouldCycleWindowsHandled;
bool g_inHandleWinNumHotKey;

// Double-click tracking for the minimize-all feature.
PVOID g_lastClickedGroup;
ULONGLONG g_lastClickTime;

using ShouldCycleWindows_t = bool(WINAPI*)();
ShouldCycleWindows_t ShouldCycleWindows_Original;
bool WINAPI ShouldCycleWindows_Hook() {
    Wh_Log(L">");

    g_shouldCycleWindowsHandled = true;
    bool ctrlKeyDown = GetKeyState(VK_CONTROL) < 0;
    return !ctrlKeyDown;
}

using CTaskBtnGroup_GetGroupType_t = int(WINAPI*)(PVOID pThis);
CTaskBtnGroup_GetGroupType_t CTaskBtnGroup_GetGroupType_Original;

// Used for the double-click minimize feature.
using CTaskBtnGroup_GetNumItems_t = int(WINAPI*)(PVOID pThis);
CTaskBtnGroup_GetNumItems_t CTaskBtnGroup_GetNumItems_Original;

using CTaskBtnGroup_GetTaskItem_t = void*(WINAPI*)(PVOID pThis, int index);
CTaskBtnGroup_GetTaskItem_t CTaskBtnGroup_GetTaskItem_Original;

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(PVOID pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow_Original;

using CImmersiveTaskItem_GetWindow_t = HWND(WINAPI*)(PVOID pThis);
CImmersiveTaskItem_GetWindow_t CImmersiveTaskItem_GetWindow_Original;

void* CImmersiveTaskItem_vftable;

// Returns the HWND for a task item, handling both regular and immersive (UWP)
// window types.
HWND GetTaskItemHwnd(void* taskItem) {
    if (!taskItem) {
        return nullptr;
    }
    if (CImmersiveTaskItem_vftable && CImmersiveTaskItem_GetWindow_Original &&
        *(void**)taskItem == CImmersiveTaskItem_vftable) {
        return CImmersiveTaskItem_GetWindow_Original(taskItem);
    }
    return CWindowTaskItem_GetWindow_Original
               ? CWindowTaskItem_GetWindow_Original(taskItem)
               : nullptr;
}

// Minimizes every non-minimized window belonging to a combined taskbar group.
void MinimizeAllGroupWindows(PVOID taskBtnGroup) {
    if (!CTaskBtnGroup_GetNumItems_Original || !CTaskBtnGroup_GetTaskItem_Original) {
        return;
    }
    int count = CTaskBtnGroup_GetNumItems_Original(taskBtnGroup);
    for (int i = 0; i < count; i++) {
        void* taskItem = CTaskBtnGroup_GetTaskItem_Original(taskBtnGroup, i);
        HWND hwnd = GetTaskItemHwnd(taskItem);
        if (hwnd && IsWindowVisible(hwnd) && !IsIconic(hwnd)) {
            ShowWindow(hwnd, SW_MINIMIZE);
        }
    }
}

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
        original();
        return;
    }

    constexpr int kClick = 0;
    constexpr int kForward = 1;
    constexpr int kBack = 2;
    constexpr int kCtrlClick = 4;

    // Double-click on a combined group: minimize all its windows.
    if (g_settings.doubleClickMinimize && !g_inHandleWinNumHotKey &&
        clickAction == kClick) {
        ULONGLONG now = GetTickCount64();
        // Use the user-configured threshold, or fall back to the system value.
        UINT threshold = g_settings.doubleClickTime > 0
                             ? (UINT)g_settings.doubleClickTime
                             : GetDoubleClickTime();
        bool isDoubleClick = (taskBtnGroup == g_lastClickedGroup) &&
                             (now - g_lastClickTime <= threshold);
        // Reset after double-click so a triple-click doesn't trigger again.
        g_lastClickedGroup = isDoubleClick ? nullptr : taskBtnGroup;
        g_lastClickTime = isDoubleClick ? 0 : now;
        if (isDoubleClick) {
            Wh_Log(L"Double-click: minimizing all group windows");
            MinimizeAllGroupWindows(taskBtnGroup);
            return;
        }
    }

    int newClickAction = clickAction;
    if (g_inHandleWinNumHotKey) {
        if (clickAction == kForward || clickAction == kBack) {
            newClickAction = kCtrlClick;
        } else if (clickAction == kCtrlClick) {
            newClickAction = kForward;
        }

        Wh_Log(L"-> %d", newClickAction);
    } else if (!g_shouldCycleWindowsHandled) {
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

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(bool __cdecl ShouldCycleWindows(void))"},
            &ShouldCycleWindows_Original,
            ShouldCycleWindows_Hook,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
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
        // Optional symbols used for the double-click minimize feature.
        {
            {LR"(public: virtual int __cdecl CTaskBtnGroup::GetNumItems(void))"},
            &CTaskBtnGroup_GetNumItems_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual struct ITaskItem * __cdecl CTaskBtnGroup::GetTaskItem(int))"},
            &CTaskBtnGroup_GetTaskItem_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            &CWindowTaskItem_GetWindow_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetWindow(void))"},
            &CImmersiveTaskItem_GetWindow_Original,
            nullptr,
            true,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            &CImmersiveTaskItem_vftable,
            nullptr,
            true,
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
        {R"(?GetGroupType@CTaskBtnGroup@@UEAA?AW4eTBGROUPTYPE@@XZ)",
         &CTaskBtnGroup_GetGroupType_Original},
        {R"(?_HandleClick@CTaskListWnd@@IEAAXPEAUITaskBtnGroup@@HW4eCLICKACTION@1@HH@Z)",
         &CTaskListWnd__HandleClick_Original, CTaskListWnd__HandleClick_Hook},
        {R"(?HandleWinNumHotKey@CTaskListWnd@@UEAAJFG@Z)",
         &CTaskListWnd_HandleWinNumHotKey_Original,
         CTaskListWnd_HandleWinNumHotKey_Hook},
        // Optional symbols used for the double-click minimize feature.
        {R"(?GetNumItems@CTaskBtnGroup@@UEAAHXZ)",
         &CTaskBtnGroup_GetNumItems_Original, nullptr, true},
        {R"(?GetTaskItem@CTaskBtnGroup@@UEAAPEAUITaskItem@@H@Z)",
         &CTaskBtnGroup_GetTaskItem_Original, nullptr, true},
        {R"(?GetWindow@CWindowTaskItem@@UEAAPEAUHWND__@@XZ)",
         &CWindowTaskItem_GetWindow_Original, nullptr, true},
        {R"(?GetWindow@CImmersiveTaskItem@@UEAAPEAUHWND__@@XZ)",
         &CImmersiveTaskItem_GetWindow_Original, nullptr, true},
        {R"(??_7CImmersiveTaskItem@@6BITaskItem@@@)",
         &CImmersiveTaskItem_vftable, nullptr, true},
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

void LoadSettings() {
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
    g_settings.doubleClickMinimize = Wh_GetIntSetting(L"doubleClickMinimize");
    g_settings.doubleClickTime = Wh_GetIntSetting(L"doubleClickTime");
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
