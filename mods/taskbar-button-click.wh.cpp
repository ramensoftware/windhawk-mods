// ==WindhawkMod==
// @id              taskbar-button-click
// @name            Middle click to close on the taskbar
// @description     Close programs with a middle click on the taskbar instead of creating a new instance
// @version         1.0.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Middle click to close on the taskbar

Close programs with the middle click on the taskbar instead of creating a new
instance.

Only Windows 10 64-bit and Windows 11 are supported.
For other Windows version check out [7+ Taskbar
Tweaker](https://tweaker.ramensoftware.com/).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- multipleItemsBehavior: closeAll
  $name: Multiple items behavior
  $description: >-
    You can choose the desired behavior for middle clicking on a group of
    windows
  $options:
  - closeAll: Close all windows
  - closeForeground: Close foreground window
  - none: Do nothing
*/
// ==/WindhawkModSettings==

#include <regex>

enum {
    MULTIPLE_ITEMS_BEHAVIOR_NONE,
    MULTIPLE_ITEMS_BEHAVIOR_CLOSE_ALL,
    MULTIPLE_ITEMS_BEHAVIOR_CLOSE_FOREGROUND,
};

struct {
    int multipleItemsBehavior;
} g_settings;

using CTaskListWnd_HandleClick_t = long(WINAPI*)(
    LPVOID pThis,
    LPVOID,  // ITaskGroup *
    LPVOID,  // ITaskItem *
    LPVOID   // winrt::Windows::System::LauncherOptions const &
);
using CTaskListWnd__HandleClick_t =
    void(WINAPI*)(LPVOID pThis,
                  LPVOID,  // ITaskBtnGroup *
                  int,
                  int,  // enum CTaskListWnd::eCLICKACTION
                  int,
                  int);
using CTaskBand_Launch_t = long(WINAPI*)(LPVOID pThis,
                                         LPVOID,  // ITaskGroup *
                                         LPVOID,  // tagPOINT const &
                                         int  // enum LaunchFromTaskbarOptions
);
using CTaskListWnd_ProcessJumpViewCloseWindow_t =
    void(WINAPI*)(LPVOID pThis,
                  HWND,
                  LPVOID,  // struct ITaskGroup *
                  HMONITOR);
using CTaskBtnGroup_GetGroupType_t = int(WINAPI*)(LPVOID pThis);

CTaskListWnd_HandleClick_t pOriginal_CTaskListWnd_HandleClick;
CTaskListWnd__HandleClick_t pOriginal_CTaskListWnd__HandleClick;
CTaskBand_Launch_t pOriginal_CTaskBand_Launch;
CTaskListWnd_ProcessJumpViewCloseWindow_t
    pOriginal_CTaskListWnd_ProcessJumpViewCloseWindow;
CTaskBtnGroup_GetGroupType_t pOriginal_CTaskBtnGroup_GetGroupType;

LPVOID pCTaskListWndHandlingClick;
LPVOID pCTaskListWndTaskBtnGroup;
int CTaskListWndClickAction = -1;

long WINAPI CTaskListWnd_HandleClick_Hook(LPVOID pThis,
                                          LPVOID param1,
                                          LPVOID param2,
                                          LPVOID param3) {
    Wh_Log(L">");

    pCTaskListWndHandlingClick = pThis;

    long ret =
        pOriginal_CTaskListWnd_HandleClick(pThis, param1, param2, param3);

    pCTaskListWndHandlingClick = nullptr;

    return ret;
}

void WINAPI CTaskListWnd__HandleClick_Hook(LPVOID pThis,
                                           LPVOID taskBtnGroup,
                                           int param2,
                                           int clickAction,
                                           int param4,
                                           int param5) {
    Wh_Log(L"> %d", clickAction);

    if (!pOriginal_CTaskListWnd_HandleClick) {
        // A magic number for pre-Win11.
        pCTaskListWndHandlingClick = (BYTE*)pThis + 0x28;
    }

    pCTaskListWndTaskBtnGroup = taskBtnGroup;
    CTaskListWndClickAction = clickAction;

    pOriginal_CTaskListWnd__HandleClick(pThis, taskBtnGroup, param2,
                                        clickAction, param4, param5);

    if (!pOriginal_CTaskListWnd_HandleClick) {
        pCTaskListWndHandlingClick = nullptr;
    }

    pCTaskListWndTaskBtnGroup = nullptr;
    CTaskListWndClickAction = -1;
}

long WINAPI CTaskBand_Launch_Hook(LPVOID pThis,
                                  LPVOID taskGroup,
                                  LPVOID param2,
                                  int param3) {
    Wh_Log(L">");

    BOOL isShiftKeyDown = GetKeyState(VK_SHIFT) < 0;

    // The click action of launching a new instance can happen in two ways:
    // * Middle click.
    // * Shift + Left click.
    // Exclude the second click action by checking whether the shift key is
    // down.
    if (pCTaskListWndHandlingClick && CTaskListWndClickAction == 3 &&
        !isShiftKeyDown) {
        int groupType = -1;
        if (pCTaskListWndTaskBtnGroup) {
            // Group types:
            // 1 - Single item
            // 2 - Pinned item
            // 3 - Multiple items
            groupType =
                pOriginal_CTaskBtnGroup_GetGroupType(pCTaskListWndTaskBtnGroup);
        }

        if (groupType == 3 &&
            g_settings.multipleItemsBehavior == MULTIPLE_ITEMS_BEHAVIOR_NONE) {
            return 0;
        }

        if (groupType == 3 && g_settings.multipleItemsBehavior ==
                                  MULTIPLE_ITEMS_BEHAVIOR_CLOSE_FOREGROUND) {
            HWND hForegroundWindow = GetForegroundWindow();
            if (hForegroundWindow) {
                WCHAR szClassName[32];
                if (GetClassName(hForegroundWindow, szClassName,
                                 ARRAYSIZE(szClassName)) &&
                    (wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
                     wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0)) {
                    hForegroundWindow = nullptr;
                }
            }

            if (hForegroundWindow) {
                POINT pt;
                GetCursorPos(&pt);
                HMONITOR monitor =
                    MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
                pOriginal_CTaskListWnd_ProcessJumpViewCloseWindow(
                    pCTaskListWndHandlingClick, hForegroundWindow, taskGroup,
                    monitor);
            }

            return 0;
        }

        if (groupType != 2) {
            POINT pt;
            GetCursorPos(&pt);
            HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
            pOriginal_CTaskListWnd_ProcessJumpViewCloseWindow(
                pCTaskListWndHandlingClick, nullptr, taskGroup, monitor);
            return 0;
        }
    }

    return pOriginal_CTaskBand_Launch(pThis, taskGroup, param2, param3);
}

struct SYMBOL_HOOK {
    std::wregex symbolRegex;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;
};

bool HookSymbols(HMODULE module,
                 SYMBOL_HOOK* symbolHooks,
                 size_t symbolHooksCount) {
    WH_FIND_SYMBOL symbol;
    HANDLE findSymbol = Wh_FindFirstSymbol(module, nullptr, &symbol);
    if (!findSymbol) {
        return false;
    }

    do {
        for (size_t i = 0; i < symbolHooksCount; i++) {
            if (!*symbolHooks[i].pOriginalFunction &&
                std::regex_match(symbol.symbol, symbolHooks[i].symbolRegex)) {
                if (symbolHooks[i].hookFunction) {
                    Wh_SetFunctionHook(symbol.address,
                                       symbolHooks[i].hookFunction,
                                       symbolHooks[i].pOriginalFunction);
                    Wh_Log(L"Hooked %p (%s)", symbol.address, symbol.symbol);
                } else {
                    *symbolHooks[i].pOriginalFunction = symbol.address;
                    Wh_Log(L"Found %p (%s)", symbol.address, symbol.symbol);
                }
                break;
            }
        }
    } while (Wh_FindNextSymbol(findSymbol, &symbol));

    Wh_FindCloseSymbol(findSymbol);

    for (size_t i = 0; i < symbolHooksCount; i++) {
        if (!symbolHooks[i].optional && !*symbolHooks[i].pOriginalFunction) {
            Wh_Log(L"Missing symbol: %d", i);
            return false;
        }
    }

    return true;
}

void LoadSettings() {
    PCWSTR multipleItemsBehavior =
        Wh_GetStringSetting(L"multipleItemsBehavior");
    g_settings.multipleItemsBehavior = MULTIPLE_ITEMS_BEHAVIOR_CLOSE_ALL;
    if (wcscmp(multipleItemsBehavior, L"closeForeground") == 0) {
        g_settings.multipleItemsBehavior =
            MULTIPLE_ITEMS_BEHAVIOR_CLOSE_FOREGROUND;
    } else if (wcscmp(multipleItemsBehavior, L"none") == 0) {
        g_settings.multipleItemsBehavior = MULTIPLE_ITEMS_BEHAVIOR_NONE;
    }
    Wh_FreeStringSetting(multipleItemsBehavior);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    bool isBeforeWin11 = false;
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        isBeforeWin11 = true;
        module = GetModuleHandle(nullptr);
    }

    SYMBOL_HOOK symbolHooks[] = {
        {std::wregex(
             LR"(public: virtual long __cdecl CTaskListWnd::HandleClick\(struct ITaskGroup \* __ptr64,struct ITaskItem \* __ptr64,struct winrt::Windows::System::LauncherOptions const & __ptr64\) __ptr64)"),
         (void**)&pOriginal_CTaskListWnd_HandleClick,
         (void*)CTaskListWnd_HandleClick_Hook},
        {std::wregex(
             LR"(protected: void __cdecl CTaskListWnd::_HandleClick\(struct ITaskBtnGroup \* __ptr64,int,enum CTaskListWnd::eCLICKACTION,int,int\) __ptr64)"),
         (void**)&pOriginal_CTaskListWnd__HandleClick,
         (void*)CTaskListWnd__HandleClick_Hook},
        {std::wregex(
             LR"(public: virtual long __cdecl CTaskBand::Launch\(struct ITaskGroup \* __ptr64,struct tagPOINT const & __ptr64,enum LaunchFromTaskbarOptions\) __ptr64)"),
         (void**)&pOriginal_CTaskBand_Launch, (void*)CTaskBand_Launch_Hook},
        {std::wregex(
             LR"(public: virtual void __cdecl CTaskListWnd::ProcessJumpViewCloseWindow\(struct HWND__ \* __ptr64,struct ITaskGroup \* __ptr64,struct HMONITOR__ \* __ptr64\) __ptr64)"),
         (void**)&pOriginal_CTaskListWnd_ProcessJumpViewCloseWindow},
        {std::wregex(
             LR"(public: virtual enum eTBGROUPTYPE __cdecl CTaskBtnGroup::GetGroupType\(void\) __ptr64)"),
         (void**)&pOriginal_CTaskBtnGroup_GetGroupType}};

    if (isBeforeWin11) {
        symbolHooks[0].optional = true;
    }

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
