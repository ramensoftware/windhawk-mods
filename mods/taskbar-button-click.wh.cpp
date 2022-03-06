// ==WindhawkMod==
// @id              taskbar-button-click
// @name            Middle click to close on the taskbar
// @description     Close programs with a middle click on the taskbar instead of creating a new instance
// @version         1.0
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

Close programs with the middle click on the taskbar instead of creating a new instance.

Only Windows 10 64-bit and Windows 11 are supported.
For other Windows version check out [7+ Taskbar Tweaker](https://rammichael.com/7-taskbar-tweaker).
*/
// ==/WindhawkModReadme==

typedef void (WINAPI *CTaskListWndProcessJumpViewCloseWindow_t)(
    LPVOID pThis,
    HWND,
    LPVOID, // struct ITaskGroup *
    HMONITOR
);
typedef long (WINAPI *CTaskListWndHandleClick_t)(
    LPVOID pThis,
    LPVOID, // ITaskGroup *
    LPVOID, // ITaskItem *
    LPVOID // winrt::Windows::System::LauncherOptions const &
);
typedef void (WINAPI *CTaskListWnd_HandleClick_t)(
    LPVOID pThis,
    LPVOID, // ITaskBtnGroup *
    int,
    int, // enum CTaskListWnd::eCLICKACTION
    int,
    int
);
typedef long (WINAPI *CTaskBandLaunch_t)(
    LPVOID pThis,
    LPVOID, // ITaskGroup *
    LPVOID, // tagPOINT const &
    int // enum LaunchFromTaskbarOptions
);

CTaskListWndProcessJumpViewCloseWindow_t pCTaskListWndProcessJumpViewCloseWindow;
CTaskListWndHandleClick_t pOriginalCTaskListWndHandleClick;
CTaskListWnd_HandleClick_t pOriginalCTaskListWnd_HandleClick;
CTaskBandLaunch_t pOriginalCTaskBandLaunch;

LPVOID pCTaskListWndHandlingClick;
int CTaskListWndClickAction = -1;

long WINAPI CTaskListWndHandleClickHook(
    LPVOID pThis,
    LPVOID param1,
    LPVOID param2,
    LPVOID param3
)
{
    Wh_Log(L">");

    pCTaskListWndHandlingClick = pThis;

    long ret = pOriginalCTaskListWndHandleClick(
        pThis,
        param1,
        param2,
        param3
    );

    pCTaskListWndHandlingClick = NULL;

    return ret;
}

void WINAPI CTaskListWnd_HandleClickHook(
    LPVOID pThis,
    LPVOID param1,
    int param2,
    int param3,
    int param4,
    int param5
)
{
    Wh_Log(L"> %d", param3);

    CTaskListWndClickAction = param3;

    if (!pOriginalCTaskListWndHandleClick) {
        // A magic number for pre-Win11.
        pCTaskListWndHandlingClick = (BYTE*)pThis + 0x28;
    }

    pOriginalCTaskListWnd_HandleClick(
        pThis,
        param1,
        param2,
        param3,
        param4,
        param5
    );

    CTaskListWndClickAction = -1;

    if (!pOriginalCTaskListWndHandleClick) {
        pCTaskListWndHandlingClick = NULL;
    }
}

long WINAPI CTaskBandLaunchHook(
    LPVOID pThis,
    LPVOID param1,
    LPVOID param2,
    int param3
)
{
    Wh_Log(L">");

    BOOL isShiftKeyDown = GetKeyState(VK_SHIFT) < 0;

    // The click action of launching a new instance can happen in two ways:
    // * Middle click.
    // * Shift + Left click.
    // Exclude the second click action by checking whether the shift key is down.
    if (pCTaskListWndHandlingClick && CTaskListWndClickAction == 3 && !isShiftKeyDown) {
        POINT pt;
        GetCursorPos(&pt);
        HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        pCTaskListWndProcessJumpViewCloseWindow(pCTaskListWndHandlingClick, NULL, param1, monitor);
        return 0;
    }
    else {
        return pOriginalCTaskBandLaunch(pThis, param1, param2, param3);
    }
}

struct SYMBOLHOOKS {
    PCWSTR symbolName;
    void* hookFunction;
    void** pOriginalFunction;
};

BOOL Wh_ModInit(void)
{
    Wh_Log(L">");

    WH_FIND_SYMBOL symbol;
    HANDLE find_symbol;

    bool isBeforeWin11 = false;
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        isBeforeWin11 = true;
        module = GetModuleHandle(NULL);
    }

    SYMBOLHOOKS taskbarHooks[] = {
        {
            L"public: virtual void __cdecl CTaskListWnd::ProcessJumpViewCloseWindow(struct HWND__ * __ptr64,struct ITaskGroup * __ptr64,struct HMONITOR__ * __ptr64) __ptr64",
            NULL,
            (void**)&pCTaskListWndProcessJumpViewCloseWindow
        },
        {
            L"public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup * __ptr64,struct ITaskItem * __ptr64,struct winrt::Windows::System::LauncherOptions const & __ptr64) __ptr64",
            (void*)CTaskListWndHandleClickHook,
            (void**)&pOriginalCTaskListWndHandleClick
        },
        {
            L"protected: void __cdecl CTaskListWnd::_HandleClick(struct ITaskBtnGroup * __ptr64,int,enum CTaskListWnd::eCLICKACTION,int,int) __ptr64",
            (void*)CTaskListWnd_HandleClickHook,
            (void**)&pOriginalCTaskListWnd_HandleClick
        },
        {
            L"public: virtual long __cdecl CTaskBand::Launch(struct ITaskGroup * __ptr64,struct tagPOINT const & __ptr64,enum LaunchFromTaskbarOptions) __ptr64",
            (void*)CTaskBandLaunchHook,
            (void**)&pOriginalCTaskBandLaunch
        }
    };

    find_symbol = Wh_FindFirstSymbol(module, NULL, &symbol);
    if (find_symbol) {
        do {
            for (size_t i = 0; i < ARRAYSIZE(taskbarHooks); i++) {
                if (!*taskbarHooks[i].pOriginalFunction && wcscmp(symbol.symbol, taskbarHooks[i].symbolName) == 0) {
                    if (taskbarHooks[i].hookFunction) {
                        Wh_SetFunctionHook(symbol.address, taskbarHooks[i].hookFunction, taskbarHooks[i].pOriginalFunction);
                        Wh_Log(L"Hooked %p (%s)", symbol.address, taskbarHooks[i].symbolName);
                    }
                    else {
                        *taskbarHooks[i].pOriginalFunction = symbol.address;
                        Wh_Log(L"Found %p (%s)", symbol.address, taskbarHooks[i].symbolName);
                    }
                    break;
                }
            }
        } while (Wh_FindNextSymbol(find_symbol, &symbol));

        Wh_FindCloseSymbol(find_symbol);
    }

    for (size_t i = 0; i < ARRAYSIZE(taskbarHooks); i++) {
        if (isBeforeWin11 && i == 1) {
            continue;
        }

        if (!*taskbarHooks[i].pOriginalFunction) {
            return FALSE;
        }
    }

    return TRUE;
}
