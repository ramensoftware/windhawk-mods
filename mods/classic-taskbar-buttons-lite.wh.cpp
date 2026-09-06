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

![Before](https://i.imgur.com/jupSjfl.png)

After:

![After](https://i.imgur.com/Jz4EkRQ.png)

Be warned that the progress indicator will not be displayed on the task buttons in the 3D mode, so if you need the progress bar, don't apply this mod.

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <mutex>
#include <unordered_set>

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

static bool IsWindowOfCurrentProcess(HWND hWnd)
{
    DWORD dwPid = 0;
    GetWindowThreadProcessId(hWnd, &dwPid);
    return dwPid == GetCurrentProcessId();
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

// --- per-thread painting tracker ---

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

// tracked set of subclassed lists - exact teardown, no enumeration
static std::mutex g_subclassedMutex;
static std::unordered_set<HWND> g_subclassedTaskLists;

// WH_SUBCLASSPROC in this SDK is 5 params: HWND, UINT, WPARAM, LPARAM, DWORD_PTR
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
        {
            std::lock_guard<std::mutex> lock(g_subclassedMutex);
            g_subclassedTaskLists.erase(hWnd);
        }
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, TaskListSubclassProc);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static void SubclassTaskListIfNew(HWND hList)
{
    if (!hList ||!IsWindowOfCurrentProcess(hList))
        return;

    bool isNew = false;
    {
        std::lock_guard<std::mutex> lock(g_subclassedMutex);
        isNew = g_subclassedTaskLists.insert(hList).second;
    }
    if (isNew)
    {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hList, TaskListSubclassProc, 0);
    }
}

static void SubclassAllTaskListsCurrentProcess()
{
    DWORD curPid = GetCurrentProcessId();

    HWND hMain = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hMain && IsWindowOfCurrentProcess(hMain))
    {
        HWND hList = FindTaskListForTray(hMain);
        SubclassTaskListIfNew(hList);
    }

    HWND hSec = NULL;
    while ((hSec = FindWindowExW(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)))
    {
        DWORD pid = 0;
        GetWindowThreadProcessId(hSec, &pid);
        if (pid!= curPid)
            continue;
        HWND hList = FindTaskListForTray(hSec);
        SubclassTaskListIfNew(hList);
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
        SubclassTaskListIfNew(hWnd);
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
    // else: keep default true, no enumeration from paint path

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

    // Direct hooks, no PDB download, no network dependency
    if (!WindhawkUtils::SetFunctionHook(BeginPaint, BeginPaint_hook, &BeginPaint_orig) ||
       !WindhawkUtils::SetFunctionHook(EndPaint, EndPaint_hook, &EndPaint_orig))
    {
        Wh_Log(L"Failed to hook BeginPaint/EndPaint");
        return FALSE;
    }

    SubclassAllTaskListsCurrentProcess();

    return TRUE;
}

void Wh_ModUninit(void)
{
    std::unordered_set<HWND> taskLists;
    {
        std::lock_guard<std::mutex> lock(g_subclassedMutex);
        taskLists.swap(g_subclassedTaskLists);
    }
    for (HWND hList : taskLists)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hList, TaskListSubclassProc);
    }
}
