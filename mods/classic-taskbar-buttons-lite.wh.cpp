// ==WindhawkMod==
// @id classic-taskbar-buttons-lite
// @name Classic Taskbar 3D buttons Lite
// @description Lightweight mod restoring the 3D buttons in classic theme
// @version 1.4
// @author Anixx
// @github https://github.com/Anixx
// @include explorer.exe
// @compilerOptions -lgdi32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Taskbar 3D buttons
Lightweight mod which restores 3D buttons on taskbar when using Windows Classic theme.
The idea is based on the mod by Aubymori (https://github.com/aubymori).

Before:

[Before](https://i.imgur.com/jupSjfl.png)

After:

[After](https://i.imgur.com/Jz4EkRQ.png)

Be warned that the progress indicator will not be displayed on the task buttons in the 3D mode, so if you need the progress bar, don't apply this mod.

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#ifdef _WIN64
#define CALCON __cdecl
#define SCALCON L"__cdecl"
#else
#define CALCON __thiscall
#define SCALCON L"__thiscall"
#endif

typedef struct tagBUTTONRENDERINFOSTATES
{
    char data[12];
} BUTTONRENDERINFOSTATES, *PBUTTONRENDERINFOSTATES;

typedef void (*CTaskBtnGroup__DrawBar_t)(void *, HDC, void *, void *);
CTaskBtnGroup__DrawBar_t CTaskBtnGroup__DrawBar_orig;

typedef HDC(WINAPI *BeginPaint_t)(HWND, LPPAINTSTRUCT);
typedef BOOL(WINAPI *EndPaint_t)(HWND, const PAINTSTRUCT *);
BeginPaint_t BeginPaint_orig = nullptr;
EndPaint_t EndPaint_orig = nullptr;

static HWND FindTaskListForTray(HWND hTray)
{
    HWND hReBar = FindWindowExW(hTray, NULL, L"ReBarWindow32", NULL);
    if (!hReBar)
        return NULL;

    HWND hTaskSw = FindWindowExW(hReBar, NULL, L"MSTaskSwWClass", NULL);
    if (!hTaskSw)
        return NULL;

    return FindWindowExW(hTaskSw, NULL, L"MSTaskListWClass", NULL);
}

static HWND GetTrayForTaskList(HWND hList)
{
    HWND hCur = hList;
    for (int i = 0; i < 10 && hCur; i++)
    {
        HWND hParent = GetParent(hCur);
        if (!hParent)
            break;
        wchar_t szClass[64] = {};
        GetClassNameW(hParent, szClass, 64);
        if (wcscmp(szClass, L"Shell_TrayWnd") == 0 ||
            wcscmp(szClass, L"Shell_SecondaryTrayWnd") == 0)
            return hParent;
        hCur = hParent;
    }
    return NULL;
}

static bool IsTrayHorizontal(HWND hTray)
{
    if (!hTray ||!IsWindow(hTray))
        return true;
    RECT rc{};
    if (!GetWindowRect(hTray, &rc))
        return true;
    return (rc.right - rc.left) > (rc.bottom - rc.top);
}

static thread_local HWND tl_currentTaskList = NULL;
static thread_local HWND tl_currentTaskListStack[8] = {};
static thread_local int tl_stackDepth = 0;

static void PushTaskList(HWND hList)
{
    if (tl_stackDepth < 8)
        tl_currentTaskListStack[tl_stackDepth++] = tl_currentTaskList;
    tl_currentTaskList = hList;
}

static void PopTaskList()
{
    if (tl_stackDepth > 0)
        tl_currentTaskList = tl_currentTaskListStack[--tl_stackDepth];
    else
        tl_currentTaskList = NULL;
}

LRESULT CALLBACK TaskListSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData)
{
    if (uMsg == WM_PAINT || uMsg == WM_PRINTCLIENT)
    {
        PushTaskList(hWnd);
        LRESULT ret = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        PopTaskList();
        return ret;
    }
    else if (uMsg == WM_NCDESTROY)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, TaskListSubclassProc);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static void SubclassTaskList(HWND hList)
{
    if (!hList)
        return;
    WindhawkUtils::SetWindowSubclassFromAnyThread(hList, TaskListSubclassProc, 0);
}

static void SubclassAllTaskLists()
{
    HWND hTrays[16];
    int n = 0;
    HWND hMain = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hMain)
        hTrays[n++] = hMain;
    HWND hSec = NULL;
    while ((hSec = FindWindowExW(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)) && n < 16)
        hTrays[n++] = hSec;

    for (int i = 0; i < n; i++)
    {
        HWND hList = FindTaskListForTray(hTrays[i]);
        SubclassTaskList(hList);
    }
}

HDC WINAPI BeginPaint_hook(HWND hWnd, LPPAINTSTRUCT lpPaint)
{
    HDC hdc = BeginPaint_orig(hWnd, lpPaint);
    wchar_t szClass[64] = {};
    GetClassNameW(hWnd, szClass, 64);
    if (wcscmp(szClass, L"MSTaskListWClass") == 0)
    {
        PushTaskList(hWnd);
    }
    return hdc;
}

BOOL WINAPI EndPaint_hook(HWND hWnd, const PAINTSTRUCT *lpPaint)
{
    wchar_t szClass[64] = {};
    GetClassNameW(hWnd, szClass, 64);
    if (wcscmp(szClass, L"MSTaskListWClass") == 0)
    {
        if (tl_currentTaskList == hWnd)
            PopTaskList();
    }
    return EndPaint_orig(hWnd, lpPaint);
}

void CALCON CTaskBtnGroup__DrawBar_hook(void *pThis, HDC hDC, void *pRenderInfo, PBUTTONRENDERINFOSTATES pRenderStates)
{
    LPRECT lprcDest = (LPRECT)((char *)pRenderInfo + 4);

    bool isHorizontal = true;

    if (tl_currentTaskList && IsWindow(tl_currentTaskList))
    {
        HWND hTray = GetTrayForTaskList(tl_currentTaskList);
        if (hTray)
            isHorizontal = IsTrayHorizontal(hTray);
    }
    else
    {
        SubclassAllTaskLists();
    }

    if (isHorizontal)
    {
        if ((lprcDest->right - lprcDest->left) > 2)
            lprcDest->right -= 2;
    }

    UINT uState = DFCS_BUTTONPUSH;

    if (pRenderStates->data[2])
        uState |= DFCS_CHECKED;
    else if (pRenderStates->data[4])
        uState |= DFCS_PUSHED;

    DrawFrameControl(hDC, lprcDest, DFC_BUTTON, uState);

    if (pRenderStates->data[2] || pRenderStates->data[4])
    {
        lprcDest->top++;
        lprcDest->bottom++;
        lprcDest->left++;
        lprcDest->right++;
    }
}

BOOL Wh_ModInit(void)
{
    HMODULE hExplorer = GetModuleHandleW(NULL);
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");

    WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
        {
            {L"private: void " SCALCON L" CTaskBtnGroup::_DrawBar(struct HDC__ *,struct BUTTONRENDERINFO const &,struct BUTTONRENDERINFOSTATES const &)"},
            (void **)&CTaskBtnGroup__DrawBar_orig,
            (void *)CTaskBtnGroup__DrawBar_hook,
            FALSE,
        },
    };

    if (!WindhawkUtils::HookSymbols(hExplorer, explorerExeHooks, ARRAYSIZE(explorerExeHooks)))
        return FALSE;

    WindhawkUtils::SYMBOL_HOOK user32DllHooks[] = {
        { {L"BeginPaint"}, (void **)&BeginPaint_orig, (void *)BeginPaint_hook, FALSE },
        { {L"EndPaint"}, (void **)&EndPaint_orig, (void *)EndPaint_hook, FALSE },
    };

    WindhawkUtils::HookSymbols(hUser32, user32DllHooks, ARRAYSIZE(user32DllHooks));

    SubclassAllTaskLists();

    return TRUE;
}

void Wh_ModUninit(void)
{
    HWND hTrays[16];
    int n = 0;
    HWND hMain = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hMain)
        hTrays[n++] = hMain;
    HWND hSec = NULL;
    while ((hSec = FindWindowExW(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)) && n < 16)
        hTrays[n++] = hSec;

    for (int i = 0; i < n; i++)
    {
        HWND hList = FindTaskListForTray(hTrays[i]);
        if (hList)
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hList, TaskListSubclassProc);
    }
}
