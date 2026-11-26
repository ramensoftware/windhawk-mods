// ==WindhawkMod==
// @id              taskbar-group-click-toggle
// @name            Taskbar group click: restore/minimize all
// @description     Left-click a combined taskbar group (e.g. Chrome, Excel) to restore all its windows; click again to minimize them all. A toggle, instead of the thumbnail flyout.
// @version         3.0.0
// @author          Anthony Gia
// @github          https://github.com/anthony-github1
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lversion
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Built on the symbol-hooking scaffolding from m417z's official taskbar mods
// ("Cycle through taskbar windows on click" and "Middle click to close on the
// taskbar"), which is what makes it work on current Windows 11 builds.

// ==WindhawkModReadme==
/*
# Taskbar group click: restore / minimize all

When a taskbar icon represents a **group** of windows (multiple Chrome windows,
several Excel workbooks, etc.), Windows normally pops up the thumbnail preview
flyout when you left-click it. This mod replaces that with a toggle:

- **Click once** -> restore (un-minimize) and bring forward **every** window in
  that group.
- **Click again** -> minimize **every** window in that group.

If the group is in a mixed state (some minimized, some not), the first click
collapses them all, so the next click expands them all. Configurable below.

Single-window taskbar buttons keep the normal Windows behavior. Shift+click
(new instance), Ctrl+click, and middle-click are left untouched.

Only Windows 11 (64-bit) is targeted. Tested against build 26100 / 26200.

## If it isn't working

1. Open this mod's page -> **Advanced** tab -> set **Debug logging** to
   **Detailed debug logs**.
2. **Disable** the mod, then **enable** it again (logging only attaches on
   load).
3. Click **Show log output**. You should immediately see init lines like
   `Win version build 26200`, `Click hook: OK`, and `Window enumeration: OK`.
   - If you DON'T see those, the mod failed to load — copy whatever is there.
4. Click a grouped icon (e.g. two Chrome windows). You should see
   `> clickAction=0 groupType=3` and then `Restoring N` / `Minimizing N`.
   - Copy these lines if behavior is still wrong.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mixedStateAction: minimize
  $name: When the group is in a mixed state
  $description: >-
    What the first click does when some windows are minimized and some are not.
  $options:
  - minimize: Minimize all (so the next click restores all)
  - restore: Restore all (so the next click minimizes all)
- bringToFront: true
  $name: Bring windows to the front when restoring
  $description: >-
    Activate the group's windows when restoring, so they come on top.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <psapi.h>

#include <atomic>
#include <vector>

struct {
    bool mixedStateRestore;  // false => minimize all when mixed (default)
    bool bringToFront;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;
std::atomic<bool> g_initialized;
std::atomic<bool> g_taskbarViewDllLoaded;
bool g_enumAvailable;        // true if window-enumeration symbols resolved
bool g_shouldCycleHooked;    // true if ShouldCycleWindows was hooked

// --- Taskbar.View.dll: divert combined-group clicks into _HandleClick -------
//
// On the modern Win11 taskbar, clicking a combined group shows the thumbnail
// flyout from the XAML layer and never calls CTaskListWnd::_HandleClick. The
// XAML layer asks ShouldCycleWindows() whether to route the click into the
// activate/cycle path (which DOES go through _HandleClick) instead. Returning
// true (unless Ctrl is held) gets the click to our hook below.

using ShouldCycleWindows_t = bool(WINAPI*)();
ShouldCycleWindows_t ShouldCycleWindows_Original;
bool WINAPI ShouldCycleWindows_Hook() {
    bool ctrlKeyDown = GetKeyState(VK_CONTROL) < 0;
    Wh_Log(L"> ShouldCycleWindows (ctrl=%d)", ctrlKeyDown ? 1 : 0);
    return !ctrlKeyDown;
}

// --- Taskbar internals ------------------------------------------------------

using CTaskListWnd__HandleClick_t = void(WINAPI*)(PVOID pThis,
                                                  PVOID taskBtnGroup,
                                                  int taskItemIndex,
                                                  int clickAction,
                                                  int param4,
                                                  int param5);
CTaskListWnd__HandleClick_t CTaskListWnd__HandleClick_Original;

using CTaskBtnGroup_GetGroupType_t = int(WINAPI*)(PVOID pThis);
CTaskBtnGroup_GetGroupType_t CTaskBtnGroup_GetGroupType_Original;

using CTaskBtnGroup_GetTaskItem_t = void*(WINAPI*)(PVOID pThis, int);
CTaskBtnGroup_GetTaskItem_t CTaskBtnGroup_GetTaskItem_Original;

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(PVOID pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow_Original;

using CImmersiveTaskItem_GetWindow_t = HWND(WINAPI*)(PVOID pThis);
CImmersiveTaskItem_GetWindow_t CImmersiveTaskItem_GetWindow_Original;

void* CImmersiveTaskItem_vftable;

// click actions (from the eCLICKACTION enum):
//   0 = plain click, 1 = cycle forward, 2 = cycle back, 3 = new instance,
//   4 = ctrl-click. When we force ShouldCycleWindows=true, a group click
//   arrives here as a cycle/activate action rather than 0, so we accept any
//   action except "new instance" (3).
constexpr int kClickActionNewInstance = 3;

// --- Window helpers ---------------------------------------------------------

static std::vector<HWND> CollectGroupWindows(PVOID taskBtnGroup) {
    std::vector<HWND> windows;

    for (int i = 0; i < 256; i++) {
        void* taskItem = CTaskBtnGroup_GetTaskItem_Original(taskBtnGroup, i);
        if (!taskItem) {
            break;
        }

        HWND hWnd = nullptr;
        if (CImmersiveTaskItem_vftable &&
            *(void**)taskItem == CImmersiveTaskItem_vftable) {
            hWnd = CImmersiveTaskItem_GetWindow_Original(taskItem);
        } else {
            hWnd = CWindowTaskItem_GetWindow_Original(taskItem);
        }

        if (hWnd && IsWindow(hWnd)) {
            windows.push_back(hWnd);
        }
    }

    return windows;
}

static void RestoreWindow(HWND hWnd, bool activate) {
    WINDOWPLACEMENT wp = {sizeof(WINDOWPLACEMENT)};
    if (GetWindowPlacement(hWnd, &wp) && (wp.flags & WPF_RESTORETOMAXIMIZED)) {
        ShowWindowAsync(hWnd, activate ? SW_SHOWMAXIMIZED : SW_MAXIMIZE);
    } else {
        ShowWindowAsync(hWnd, activate ? SW_RESTORE : SW_SHOWNOACTIVATE);
    }
}

// Returns true if it handled the click (caller should suppress default).
static bool ToggleGroup(PVOID taskBtnGroup) {
    if (!g_enumAvailable) {
        Wh_Log(L"Enumeration unavailable; passing click through");
        return false;
    }

    std::vector<HWND> windows = CollectGroupWindows(taskBtnGroup);
    Wh_Log(L"Group has %zu window(s)", windows.size());
    if (windows.empty()) {
        return false;
    }

    int minimizedCount = 0;
    for (HWND hWnd : windows) {
        if (IsIconic(hWnd)) {
            minimizedCount++;
        }
    }

    bool allMinimized = (minimizedCount == (int)windows.size());
    bool allRestored = (minimizedCount == 0);

    bool doRestore;
    if (allMinimized) {
        doRestore = true;
    } else if (allRestored) {
        doRestore = false;
    } else {
        doRestore = g_settings.mixedStateRestore;
    }

    if (doRestore) {
        Wh_Log(L"Restoring %zu window(s)", windows.size());
        for (HWND hWnd : windows) {
            RestoreWindow(hWnd, g_settings.bringToFront);
        }
        if (g_settings.bringToFront) {
            HWND top = windows.back();
            DWORD pid = 0;
            GetWindowThreadProcessId(top, &pid);
            AllowSetForegroundWindow(pid);
            SetForegroundWindow(top);
        }
    } else {
        Wh_Log(L"Minimizing %zu window(s)", windows.size());
        for (HWND hWnd : windows) {
            ShowWindowAsync(hWnd, SW_MINIMIZE);
        }
    }

    return true;
}

// --- Click hook -------------------------------------------------------------

void WINAPI CTaskListWnd__HandleClick_Hook(PVOID pThis,
                                           PVOID taskBtnGroup,
                                           int taskItemIndex,
                                           int clickAction,
                                           int param4,
                                           int param5) {
    int groupType = CTaskBtnGroup_GetGroupType_Original(taskBtnGroup);
    Wh_Log(L"> clickAction=%d groupType=%d taskItemIndex=%d", clickAction,
           groupType, taskItemIndex);

    auto original = [&]() {
        CTaskListWnd__HandleClick_Original(pThis, taskBtnGroup, taskItemIndex,
                                           clickAction, param4, param5);
    };

    // Override an unmodified left-click on a combined group (type 3). Skip the
    // new-instance action and any modifier-held click.
    bool modifierHeld = GetKeyState(VK_SHIFT) < 0 ||
                        GetKeyState(VK_CONTROL) < 0 ||
                        GetKeyState(VK_MENU) < 0 || GetKeyState(VK_LWIN) < 0 ||
                        GetKeyState(VK_RWIN) < 0;

    if (groupType != 3 || clickAction == kClickActionNewInstance ||
        modifierHeld) {
        return original();
    }

    if (!ToggleGroup(taskBtnGroup)) {
        return original();
    }
    // Handled: suppress the default thumbnail flyout.
}

// --- Version detection & symbol hooking ------------------------------------

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
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Win version build %u", build);

    if (major != 10 || build < 22000) {
        return WinVersion::Unsupported;
    }
    return build < 26100 ? WinVersion::Win11 : WinVersion::Win11_24H2;
}

bool HookTaskbarSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load taskbar.dll");
        return false;
    }

    // Required: the click hook + group-type query. (Proven on 24H2/26100+.)
    WindhawkUtils::SYMBOL_HOOK requiredHooks[] = {
        {
            {LR"(protected: void __cdecl CTaskListWnd::_HandleClick(struct ITaskBtnGroup *,int,enum CTaskListWnd::eCLICKACTION,int,int))"},
            &CTaskListWnd__HandleClick_Original,
            CTaskListWnd__HandleClick_Hook,
        },
        {
            {LR"(public: virtual enum eTBGROUPTYPE __cdecl CTaskBtnGroup::GetGroupType(void))"},
            &CTaskBtnGroup_GetGroupType_Original,
        },
    };

    if (!HookSymbols(module, requiredHooks, ARRAYSIZE(requiredHooks))) {
        Wh_Log(L"Required HookSymbols failed");
        return false;
    }
    Wh_Log(L"Click hook: OK");

    // Optional: window enumeration. If any of these don't resolve on this
    // build, the mod still loads and logs, so we can see what's missing.
    WindhawkUtils::SYMBOL_HOOK enumHooks[] = {
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

    HookSymbols(module, enumHooks, ARRAYSIZE(enumHooks));

    g_enumAvailable = CTaskBtnGroup_GetTaskItem_Original &&
                      CWindowTaskItem_GetWindow_Original;
    Wh_Log(L"Window enumeration: %s", g_enumAvailable ? L"OK" : L"MISSING");

    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(bool __cdecl ShouldCycleWindows(void))"},
            &ShouldCycleWindows_Original,
            ShouldCycleWindows_Hook,
            true,  // optional
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"Taskbar.View.dll HookSymbols failed");
        return false;
    }

    g_shouldCycleHooked = ShouldCycleWindows_Original != nullptr;
    Wh_Log(L"ShouldCycleWindows hook: %s",
           g_shouldCycleHooked ? L"OK" : L"MISSING");
    return true;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);
        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
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
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }
    return module;
}

void LoadSettings() {
    PCWSTR mixedStateAction = Wh_GetStringSetting(L"mixedStateAction");
    g_settings.mixedStateRestore = wcscmp(mixedStateAction, L"restore") == 0;
    Wh_FreeStringSetting(mixedStateAction);

    g_settings.bringToFront = Wh_GetIntSetting(L"bringToFront");
}

BOOL Wh_ModInit() {
    Wh_Log(L"> Init");

    LoadSettings();

    g_winVersion = GetExplorerVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (!HookTaskbarSymbols()) {
        return FALSE;
    }

    // Hook Taskbar.View.dll (ShouldCycleWindows) so combined-group clicks reach
    // our _HandleClick hook instead of opening the flyout. It may not be loaded
    // yet; if so, the LoadLibraryExW hook / Wh_ModAfterInit will catch it.
    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        HookTaskbarViewDllSymbols(taskbarViewModule);
    } else {
        Wh_Log(L"Taskbar.View.dll not loaded yet; will hook on load");
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

    g_initialized = true;
    Wh_Log(L"Init done");
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // In case Taskbar.View.dll loaded between init and now.
    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");
    LoadSettings();
    *bReload = FALSE;
    return TRUE;
}
