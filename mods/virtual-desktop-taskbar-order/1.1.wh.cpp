// ==WindhawkMod==
// @id              virtual-desktop-taskbar-order
// @name            Virtual Desktop Preserve Taskbar Order
// @description     The order on the taskbar isn't preserved between virtual desktop switches, this mod fixes it
// @version         1.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lversion
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

Only Windows 10 64-bit and Windows 11 are supported. For older Windows versions
check out [7+ Taskbar Tweaker](https://tweaker.ramensoftware.com/).

![Demonstration](https://i.imgur.com/ie8Q9cl.gif)
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

#include <algorithm>
#include <atomic>
#include <climits>
#include <map>
#include <utility>
#include <vector>

#include <commctrl.h>
#include <psapi.h>

struct {
    bool oldTaskbarOnWin11;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
};

WinVersion g_winVersion;

std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

DWORD g_taskbarThreadId;

void* CTaskBtnGroup_ITaskBtnGroup_vftable;

using CTaskBtnGroup_GetGroup_t = void*(WINAPI*)(void* pThis);
CTaskBtnGroup_GetGroup_t CTaskBtnGroup_GetGroup;

bool IsButtonGroup(void* p) {
    return p && *(void**)p == CTaskBtnGroup_ITaskBtnGroup_vftable;
}

bool IsButtonGroupOf(void* p, void* taskGroup) {
    return taskGroup && IsButtonGroup(p) &&
           CTaskBtnGroup_GetGroup(p) == taskGroup;
}

// The task groups of each taskbar's buttons, in order, per virtual desktop. A
// taskbar is identified by the DPA of its button groups, and a desktop by its
// IVirtualDesktop pointer. The keys and the task groups are only compared.
std::map<std::pair<HDPA, void*>, std::vector<void*>> g_desktopArrangements;

// The desktop which the taskbar shows, null until the first switch since the
// mod was loaded.
void* g_currentDesktop;

struct DesktopSwitch {
    void* oldDesktop;
    void* newDesktop;
    std::vector<HDPA> savedDpas;
};

// Set while the taskbar updates its buttons for a virtual desktop switch.
thread_local DesktopSwitch* g_desktopSwitch;

std::vector<void*> GetButtonGroupsOrder(HDPA hdpa) {
    std::vector<void*> taskGroups;
    for (int i = 0; i < DPA_GetPtrCount(hdpa); i++) {
        taskGroups.push_back(CTaskBtnGroup_GetGroup(DPA_GetPtr(hdpa, i)));
    }

    return taskGroups;
}

// Saves the arrangement of the old desktop the first time the switch changes
// the taskbar's buttons.
void SaveArrangementBeforeSwitchChange(HDPA hdpa) {
    std::vector<HDPA>& savedDpas = g_desktopSwitch->savedDpas;
    if (std::ranges::find(savedDpas, hdpa) != savedDpas.end()) {
        return;
    }

    savedDpas.push_back(hdpa);

    g_desktopArrangements[{hdpa, g_desktopSwitch->oldDesktop}] =
        GetButtonGroupsOrder(hdpa);
}

// Returns the index at which the task group's new button group gets the
// position it had in the arrangement of the new desktop, or INT_MAX to append
// it.
int GetDesktopArrangementIndex(HDPA hdpa, void* taskGroup) {
    auto it = g_desktopArrangements.find({hdpa, g_desktopSwitch->newDesktop});
    if (it == g_desktopArrangements.end()) {
        return INT_MAX;
    }

    const std::vector<void*>& arrangement = it->second;
    auto position = std::ranges::find(arrangement, taskGroup);
    if (position == arrangement.end()) {
        return INT_MAX;
    }

    // Insert before the first button group which follows it in the
    // arrangement.
    for (int i = 0; i < DPA_GetPtrCount(hdpa); i++) {
        void* buttonTaskGroup = CTaskBtnGroup_GetGroup(DPA_GetPtr(hdpa, i));
        if (std::find(position + 1, arrangement.end(), buttonTaskGroup) !=
            arrangement.end()) {
            return i;
        }
    }

    return INT_MAX;
}

template <typename Function>
HRESULT HandleDesktopSwitch(void* oldDesktop,
                            void* newDesktop,
                            Function original) {
    if (GetCurrentThreadId() != g_taskbarThreadId || g_desktopSwitch) {
        return original();
    }

    Wh_Log(L"> oldDesktop=%p, newDesktop=%p", oldDesktop, newDesktop);

    DesktopSwitch desktopSwitch{oldDesktop, newDesktop, {}};
    g_desktopSwitch = &desktopSwitch;
    HRESULT hr = original();
    g_desktopSwitch = nullptr;

    if (!g_currentDesktop) {
        // Drags before the first switch saved arrangements without the
        // desktop. Keep them unless the switch saved newer ones.
        std::vector<HDPA> pendingDpas;
        for (const auto& [key, arrangement] : g_desktopArrangements) {
            if (!key.second) {
                pendingDpas.push_back(key.first);
            }
        }

        for (HDPA hdpa : pendingDpas) {
            auto node = g_desktopArrangements.extract({hdpa, nullptr});
            node.key().second = oldDesktop;
            g_desktopArrangements.insert(std::move(node));
        }
    }

    g_currentDesktop = newDesktop;

    return hr;
}

using CTaskBand_CurrentVirtualDesktopChanged_t =
    HRESULT(WINAPI*)(void* pThis,
                     void* oldDesktop,   // IVirtualDesktop*
                     void* newDesktop);  // IVirtualDesktop*
CTaskBand_CurrentVirtualDesktopChanged_t
    CTaskBand_CurrentVirtualDesktopChanged_Original;
HRESULT WINAPI CTaskBand_CurrentVirtualDesktopChanged_Hook(void* pThis,
                                                           void* oldDesktop,
                                                           void* newDesktop) {
    return HandleDesktopSwitch(oldDesktop, newDesktop, [&] {
        return CTaskBand_CurrentVirtualDesktopChanged_Original(
            pThis, oldDesktop, newDesktop);
    });
}

using CTaskBand_CurrentVirtualDesktopChangedPre24H2_t =
    HRESULT(WINAPI*)(void* pThis,
                     void* objectArray,  // IObjectArray*
                     void* oldDesktop,   // IVirtualDesktop*
                     void* newDesktop);  // IVirtualDesktop*
CTaskBand_CurrentVirtualDesktopChangedPre24H2_t
    CTaskBand_CurrentVirtualDesktopChangedPre24H2_Original;
HRESULT WINAPI
CTaskBand_CurrentVirtualDesktopChangedPre24H2_Hook(void* pThis,
                                                   void* objectArray,
                                                   void* oldDesktop,
                                                   void* newDesktop) {
    return HandleDesktopSwitch(oldDesktop, newDesktop, [&] {
        return CTaskBand_CurrentVirtualDesktopChangedPre24H2_Original(
            pThis, objectArray, oldDesktop, newDesktop);
    });
}

CTaskBand_CurrentVirtualDesktopChanged_t
    CTaskBand_CurrentVirtualDesktopChangedAnimated_Original;
HRESULT WINAPI
CTaskBand_CurrentVirtualDesktopChangedAnimated_Hook(void* pThis,
                                                    void* oldDesktop,
                                                    void* newDesktop) {
    return HandleDesktopSwitch(oldDesktop, newDesktop, [&] {
        return CTaskBand_CurrentVirtualDesktopChangedAnimated_Original(
            pThis, oldDesktop, newDesktop);
    });
}

thread_local void* g_createTBGroup_taskGroup;

using CTaskListWnd__CreateTBGroup_t =
    void*(WINAPI*)(void* pThis,
                   void* taskGroup,  // ITaskGroup*
                   int index);
CTaskListWnd__CreateTBGroup_t CTaskListWnd__CreateTBGroup_Original;
void* WINAPI CTaskListWnd__CreateTBGroup_Hook(void* pThis,
                                              void* taskGroup,
                                              int index) {
    g_createTBGroup_taskGroup = taskGroup;

    void* ret = CTaskListWnd__CreateTBGroup_Original(pThis, taskGroup, index);

    g_createTBGroup_taskGroup = nullptr;

    return ret;
}

thread_local void* g_taskDestroyedFinal_buttonGroup;

using CTaskListWnd__TaskDestroyedFinal_t =
    void(WINAPI*)(void* pThis,
                  void* buttonGroup,  // ITaskBtnGroup*
                  int param);
CTaskListWnd__TaskDestroyedFinal_t CTaskListWnd__TaskDestroyedFinal_Original;
void WINAPI CTaskListWnd__TaskDestroyedFinal_Hook(void* pThis,
                                                  void* buttonGroup,
                                                  int param) {
    g_taskDestroyedFinal_buttonGroup = buttonGroup;

    CTaskListWnd__TaskDestroyedFinal_Original(pThis, buttonGroup, param);

    g_taskDestroyedFinal_buttonGroup = nullptr;
}

// Set while the taskbar moves a button group to another position.
thread_local bool g_movingButtonGroup;

using CTaskListWnd_TryMoveGroup_t =
    bool(WINAPI*)(void* pThis,
                  void* taskGroup,  // ITaskGroup*
                  DWORD index);
CTaskListWnd_TryMoveGroup_t CTaskListWnd_TryMoveGroup_Original;
bool WINAPI CTaskListWnd_TryMoveGroup_Hook(void* pThis,
                                           void* taskGroup,
                                           DWORD index) {
    g_movingButtonGroup = true;

    bool ret = CTaskListWnd_TryMoveGroup_Original(pThis, taskGroup, index);

    g_movingButtonGroup = false;

    return ret;
}

using CTaskListWnd__MoveTaskBtnGroup_t = HRESULT(WINAPI*)(void* pThis,
                                                         int indexFrom,
                                                         int indexTo);
CTaskListWnd__MoveTaskBtnGroup_t CTaskListWnd__MoveTaskBtnGroup_Original;
HRESULT WINAPI CTaskListWnd__MoveTaskBtnGroup_Hook(void* pThis,
                                                   int indexFrom,
                                                   int indexTo) {
    g_movingButtonGroup = true;

    HRESULT ret =
        CTaskListWnd__MoveTaskBtnGroup_Original(pThis, indexFrom, indexTo);

    g_movingButtonGroup = false;

    return ret;
}

using DPA_InsertPtr_t = decltype(&DPA_InsertPtr);
DPA_InsertPtr_t DPA_InsertPtr_Original;
int WINAPI DPA_InsertPtr_Hook(HDPA hdpa, int i, void* p) {
    if (g_desktopSwitch && IsButtonGroupOf(p, g_createTBGroup_taskGroup)) {
        SaveArrangementBeforeSwitchChange(hdpa);
        if (i == INT_MAX) {
            i = GetDesktopArrangementIndex(hdpa, g_createTBGroup_taskGroup);
            Wh_Log(L"Inserting button group %p at %d", p, i);
        }
    }

    int ret = DPA_InsertPtr_Original(hdpa, i, p);

    if (g_movingButtonGroup && !g_desktopSwitch && IsButtonGroup(p) &&
        GetCurrentThreadId() == g_taskbarThreadId) {
        Wh_Log(L"Button group %p moved to %d", p, ret);
        g_desktopArrangements[{hdpa, g_currentDesktop}] =
            GetButtonGroupsOrder(hdpa);
    }

    return ret;
}

using DPA_DeletePtr_t = decltype(&DPA_DeletePtr);
DPA_DeletePtr_t DPA_DeletePtr_Original;
void* WINAPI DPA_DeletePtr_Hook(HDPA hdpa, int i) {
    if (g_desktopSwitch && g_taskDestroyedFinal_buttonGroup &&
        DPA_GetPtr(hdpa, i) == g_taskDestroyedFinal_buttonGroup) {
        SaveArrangementBeforeSwitchChange(hdpa);
    }

    return DPA_DeletePtr_Original(hdpa, i);
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

void InitializeTaskbarVariables(HWND hTaskbarWnd) {
    Wh_Log(L"Initialized for taskbar %08X", (DWORD)(ULONG_PTR)hTaskbarWnd);
    g_taskbarThreadId = GetWindowThreadProcessId(hTaskbarWnd, nullptr);
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

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetExplorerVersion(bool* hasWin10Taskbar) {
    *hasWin10Taskbar = false;

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
            // explorer.exe includes the Win10 taskbar before this build.
            *hasWin10Taskbar = build < 26100 || (build == 26100 && qfe < 1301);
            if (build < 22000) {
                return WinVersion::Win10;
            } else {
                return WinVersion::Win11;
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
        {R"(??_7CTaskBtnGroup@@6BITaskBtnGroup@@@)",
         &CTaskBtnGroup_ITaskBtnGroup_vftable},
        {R"(?GetGroup@CTaskBtnGroup@@UEAAPEAUITaskGroup@@XZ)",
         &CTaskBtnGroup_GetGroup},
        {R"(?_MoveTaskBtnGroup@CTaskListWnd@@IEAAJHH@Z)",
         &CTaskListWnd__MoveTaskBtnGroup_Original,
         CTaskListWnd__MoveTaskBtnGroup_Hook},
        {R"(?_CreateTBGroup@CTaskListWnd@@IEAAPEAUITaskBtnGroup@@PEAUITaskGroup@@H@Z)",
         &CTaskListWnd__CreateTBGroup_Original,
         CTaskListWnd__CreateTBGroup_Hook},
        {R"(?_TaskDestroyedFinal@CTaskListWnd@@IEAAXPEAUITaskBtnGroup@@H@Z)",
         &CTaskListWnd__TaskDestroyedFinal_Original,
         CTaskListWnd__TaskDestroyedFinal_Hook},
        {R"(?CurrentVirtualDesktopChanged@CTaskBand@@UEAAJPEAUIVirtualDesktop@@0@Z)",
         &CTaskBand_CurrentVirtualDesktopChanged_Original,
         CTaskBand_CurrentVirtualDesktopChanged_Hook},
        {// Windows 11 before version 24H2.
         R"(?CurrentVirtualDesktopChanged@CTaskBand@@UEAAJPEAUIObjectArray@@PEAUIVirtualDesktop@@1@Z)",
         &CTaskBand_CurrentVirtualDesktopChangedPre24H2_Original,
         CTaskBand_CurrentVirtualDesktopChangedPre24H2_Hook, true},
        {R"(?CurrentVirtualDesktopChangedAnimated@CTaskBand@@UEAAJPEAUIVirtualDesktop@@0@Z)",
         &CTaskBand_CurrentVirtualDesktopChangedAnimated_Original,
         CTaskBand_CurrentVirtualDesktopChangedAnimated_Hook},
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
            {LR"(const CTaskBtnGroup::`vftable'{for `ITaskBtnGroup'})"},
            &CTaskBtnGroup_ITaskBtnGroup_vftable,
        },
        {
            {LR"(public: virtual struct ITaskGroup * __cdecl CTaskBtnGroup::GetGroup(void))"},
            &CTaskBtnGroup_GetGroup,
        },
        {
            // Windows 11.
            {LR"(public: virtual bool __cdecl CTaskListWnd::TryMoveGroup(struct ITaskGroup *,unsigned int))"},
            &CTaskListWnd_TryMoveGroup_Original,
            CTaskListWnd_TryMoveGroup_Hook,
            true,
        },
        {
            {LR"(protected: long __cdecl CTaskListWnd::_MoveTaskBtnGroup(int,int))"},
            &CTaskListWnd__MoveTaskBtnGroup_Original,
            CTaskListWnd__MoveTaskBtnGroup_Hook,
        },
        {
            {LR"(protected: struct ITaskBtnGroup * __cdecl CTaskListWnd::_CreateTBGroup(struct ITaskGroup *,int))"},
            &CTaskListWnd__CreateTBGroup_Original,
            CTaskListWnd__CreateTBGroup_Hook,
        },
        {
            {LR"(protected: void __cdecl CTaskListWnd::_TaskDestroyedFinal(struct ITaskBtnGroup *,int))"},
            &CTaskListWnd__TaskDestroyedFinal_Original,
            CTaskListWnd__TaskDestroyedFinal_Hook,
        },
        {
            // Windows 10, and Windows 11 version 24H2 and newer.
            {LR"(public: virtual long __cdecl CTaskBand::CurrentVirtualDesktopChanged(struct IVirtualDesktop *,struct IVirtualDesktop *))"},
            &CTaskBand_CurrentVirtualDesktopChanged_Original,
            CTaskBand_CurrentVirtualDesktopChanged_Hook,
            true,
        },
        {
            // Windows 11 before version 24H2.
            {LR"(public: virtual long __cdecl CTaskBand::CurrentVirtualDesktopChanged(struct IObjectArray *,struct IVirtualDesktop *,struct IVirtualDesktop *))"},
            &CTaskBand_CurrentVirtualDesktopChangedPre24H2_Original,
            CTaskBand_CurrentVirtualDesktopChangedPre24H2_Hook,
            true,
        },
        {
            {LR"(public: virtual long __cdecl CTaskBand::CurrentVirtualDesktopChangedAnimated(struct IVirtualDesktop *,struct IVirtualDesktop *))"},
            &CTaskBand_CurrentVirtualDesktopChangedAnimated_Original,
            CTaskBand_CurrentVirtualDesktopChangedAnimated_Hook,
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

    bool hasWin10Taskbar;
    g_winVersion = GetExplorerVersion(&hasWin10Taskbar);
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (g_settings.oldTaskbarOnWin11) {
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
    WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                   LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);

    WindhawkUtils::SetFunctionHook(DPA_InsertPtr, DPA_InsertPtr_Hook,
                                   &DPA_InsertPtr_Original);

    WindhawkUtils::SetFunctionHook(DPA_DeletePtr, DPA_DeletePtr_Hook,
                                   &DPA_DeletePtr_Original);

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);

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

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        InitializeTaskbarVariables(hTaskbarWnd);
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;

    return TRUE;
}
