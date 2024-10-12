// ==WindhawkMod==
// @id              taskbar-classic-menu
// @name            Taskbar classic context menu
// @description     Show the classic context menu when right-clicking on taskbar items
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
# Taskbar classic context menu

Show the classic context menu when right-clicking on taskbar items. You can hold
the Shift key to show the default jump list.

Only Windows 10 64-bit and Windows 11 are supported. For other Windows versions
check out [7+ Taskbar Tweaker](https://tweaker.ramensoftware.com/).

**Note:** To customize the old taskbar on Windows 11 (if using ExplorerPatcher
or a similar tool), enable the relevant option in the mod's settings.

![Demonstration](https://i.imgur.com/lQEHQyR.png)
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

std::atomic<DWORD> g_CTaskListWnd__HandleContextMenuThreadId;
std::atomic<DWORD> g_TaskbarResources_OnTaskListButtonContextRequestedThreadId;

using CTaskListWnd__HandleContextMenu_t = void(WINAPI*)(void* pThis,
                                                        int param1,
                                                        int param2,
                                                        int param3);
CTaskListWnd__HandleContextMenu_t CTaskListWnd__HandleContextMenu_Original;
void WINAPI CTaskListWnd__HandleContextMenu_Hook(void* pThis,
                                                 int param1,
                                                 int param2,
                                                 int param3) {
    Wh_Log(L">");

    g_CTaskListWnd__HandleContextMenuThreadId = GetCurrentThreadId();

    CTaskListWnd__HandleContextMenu_Original(pThis, param1, param2, param3);

    g_CTaskListWnd__HandleContextMenuThreadId = 0;
}

using CTaskListWnd_GetWindow_t = HWND(WINAPI*)(void* pThis);
CTaskListWnd_GetWindow_t CTaskListWnd_GetWindow_Original;

using CTaskListWnd__GetTBGroupFromGroup_t = void*(WINAPI*)(void* pThis,
                                                           void* taskGroup,
                                                           int* index);
CTaskListWnd__GetTBGroupFromGroup_t CTaskListWnd__GetTBGroupFromGroup_Original;

using CTaskBtnGroup_GetGroupType_t = int(WINAPI*)(void* pThis);
CTaskBtnGroup_GetGroupType_t CTaskBtnGroup_GetGroupType_Original;

using CTaskBtnGroup_GetTaskItem_t = void*(WINAPI*)(void* pThis, int index);
CTaskBtnGroup_GetTaskItem_t CTaskBtnGroup_GetTaskItem_Original;

using CTaskListWnd_OnContextMenu_t = HRESULT(WINAPI*)(void* pThis,
                                                      POINT pt,
                                                      HWND hWnd,
                                                      bool param3,
                                                      void* taskGroup,
                                                      void* taskItem);
CTaskListWnd_OnContextMenu_t CTaskListWnd_OnContextMenu_Original;

using CTaskListWnd_HandleClick_t = HRESULT(WINAPI*)(void* pThis,
                                                    void* taskGroup,
                                                    void* taskItem,
                                                    void* launcherOptions);
CTaskListWnd_HandleClick_t CTaskListWnd_HandleClick_Original;
HRESULT WINAPI CTaskListWnd_HandleClick_Hook(void* pThis,
                                             void* taskGroup,
                                             void* taskItem,
                                             void* launcherOptions) {
    Wh_Log(L">");

    if (g_TaskbarResources_OnTaskListButtonContextRequestedThreadId ==
        GetCurrentThreadId()) {
        POINT pt{};
        GetCursorPos(&pt);

        void* pThis2 = (void**)pThis + 1;
        HWND hTaskListWnd = CTaskListWnd_GetWindow_Original(pThis2);

        if (!taskItem && taskGroup) {
            void* taskBtnGroup = CTaskListWnd__GetTBGroupFromGroup_Original(
                (void**)pThis - 5, taskGroup, nullptr);
            if (taskBtnGroup &&
                CTaskBtnGroup_GetGroupType_Original(taskBtnGroup) == 1) {
                taskItem = CTaskBtnGroup_GetTaskItem_Original(taskBtnGroup, 0);
            }
        }

        CTaskListWnd_OnContextMenu_Original(pThis2, pt, hTaskListWnd, false,
                                            taskGroup, taskItem);
        return S_OK;
    }

    HRESULT ret = CTaskListWnd_HandleClick_Original(pThis, taskGroup, taskItem,
                                                    launcherOptions);

    return ret;
}

using CTaskListWnd__DisplayExtendedUI_t = HRESULT(WINAPI*)(void* pThis,
                                                           void* taskBtnGroup,
                                                           int param2,
                                                           DWORD flags,
                                                           int param4);
CTaskListWnd__DisplayExtendedUI_t CTaskListWnd__DisplayExtendedUI_Original;
HRESULT WINAPI CTaskListWnd__DisplayExtendedUI_Hook(void* pThis,
                                                    void* taskBtnGroup,
                                                    int param2,
                                                    DWORD flags,
                                                    int param4) {
    Wh_Log(L"> %x", flags);

    // A workaround to the issue of thumbnail previews popping up above the
    // menu: disallow thumbnails when a menu is open.
    HWND hMenuWnd = FindWindow(L"#32768", nullptr);
    if (hMenuWnd && IsWindowVisible(hMenuWnd)) {
        return S_OK;
    }

    HRESULT ret = CTaskListWnd__DisplayExtendedUI_Original(
        pThis, taskBtnGroup, param2, flags, param4);

    return ret;
}

using TaskbarResources_OnTaskListButtonContextRequested_t =
    void(WINAPI*)(void* pThis, void* param1, void* param2);
TaskbarResources_OnTaskListButtonContextRequested_t
    TaskbarResources_OnTaskListButtonContextRequested_Original;
void WINAPI
TaskbarResources_OnTaskListButtonContextRequested_Hook(void* pThis,
                                                       void* param1,
                                                       void* param2) {
    Wh_Log(L">");

    g_TaskbarResources_OnTaskListButtonContextRequestedThreadId =
        GetCurrentThreadId();

    TaskbarResources_OnTaskListButtonContextRequested_Original(pThis, param1,
                                                               param2);

    g_TaskbarResources_OnTaskListButtonContextRequestedThreadId = 0;
}

using GetKeyState_t = decltype(&GetKeyState);
GetKeyState_t GetKeyState_Original;
SHORT WINAPI GetKeyState_Hook(int nVirtKey) {
    SHORT ret = GetKeyState_Original(nVirtKey);

    if (nVirtKey == VK_SHIFT) {
        DWORD currentThreadId = GetCurrentThreadId();
        if (g_CTaskListWnd__HandleContextMenuThreadId == currentThreadId ||
            g_TaskbarResources_OnTaskListButtonContextRequestedThreadId ==
                currentThreadId) {
            ret ^= 0x8000;
        }
    }

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

    struct EXPLORER_PATCHER_HOOK {
        PCSTR symbol;
        void** pOriginalFunction;
        void* hookFunction = nullptr;
        bool optional = false;
    };

    EXPLORER_PATCHER_HOOK hooks[] = {
        {R"(?_HandleContextMenu@CTaskListWnd@@IEAAXHHH@Z)",
         (void**)&CTaskListWnd__HandleContextMenu_Original,
         (void*)CTaskListWnd__HandleContextMenu_Hook},
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

bool HookWin10TaskbarSymbols() {
    WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
        {
            {
                LR"(protected: void __cdecl CTaskListWnd::_HandleContextMenu(int,int,int))",

                // Older versions:
                LR"(protected: void __thiscall CTaskListWnd::_HandleContextMenu(int,int,int))",
            },
            &CTaskListWnd__HandleContextMenu_Original,
            CTaskListWnd__HandleContextMenu_Hook,
        },
    };

    return HookSymbols(GetModuleHandle(nullptr), explorerExeHooks,
                       ARRAYSIZE(explorerExeHooks));
}

bool HookWin11TaskbarSymbols() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
            &CTaskListWnd_HandleClick_Original,
            CTaskListWnd_HandleClick_Hook,
        },
        {
            {LR"(protected: long __cdecl CTaskListWnd::_DisplayExtendedUI(struct ITaskBtnGroup *,int,unsigned long,int))"},
            &CTaskListWnd__DisplayExtendedUI_Original,
            CTaskListWnd__DisplayExtendedUI_Hook,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CTaskListWnd::GetWindow(void))"},
            &CTaskListWnd_GetWindow_Original,
        },
        {
            {LR"(protected: struct ITaskBtnGroup * __cdecl CTaskListWnd::_GetTBGroupFromGroup(struct ITaskGroup *,int *))"},
            &CTaskListWnd__GetTBGroupFromGroup_Original,
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
            {LR"(public: virtual void __cdecl CTaskListWnd::OnContextMenu(struct tagPOINT,struct HWND__ *,bool,struct ITaskGroup *,struct ITaskItem *))"},
            &CTaskListWnd_OnContextMenu_Original,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
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
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarResources::OnTaskListButtonContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"},
            &TaskbarResources_OnTaskListButtonContextRequested_Original,
            TaskbarResources_OnTaskListButtonContextRequested_Hook,
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

        if (hasWin10Taskbar && !HookWin10TaskbarSymbols()) {
            return FALSE;
        }

        HandleLoadedExplorerPatcher();

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        FARPROC pKernelBaseLoadLibraryExW =
            GetProcAddress(kernelBaseModule, "LoadLibraryExW");
        Wh_SetFunctionHook((void*)pKernelBaseLoadLibraryExW,
                           (void*)LoadLibraryExW_Hook,
                           (void**)&LoadLibraryExW_Original);
    } else if (g_winVersion >= WinVersion::Win11) {
        if (!HookWin11TaskbarSymbols()) {
            return FALSE;
        }

        if (!HookTaskbarViewDllSymbols()) {
            return FALSE;
        }
    } else {
        if (!HookWin10TaskbarSymbols()) {
            return FALSE;
        }
    }

    Wh_SetFunctionHook((void*)GetKeyState, (void*)GetKeyState_Hook,
                       (void**)&GetKeyState_Original);

    g_initialized = true;

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // Try again in case there's a race between the previous attempt and the
    // LoadLibraryExW hook.
    if (g_settings.oldTaskbarOnWin11 && !g_explorerPatcherInitialized) {
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
