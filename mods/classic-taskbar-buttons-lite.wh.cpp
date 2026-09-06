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
#include <vector>

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

using CreateWindowExW_t = HWND(WINAPI *)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
CreateWindowExW_t CreateWindowExW_orig = nullptr;

static HWND FindTaskListForTray(HWND hTray)
{
    wchar_t szClass[32]{};
    if (!GetClassNameW(hTray, szClass, ARRAYSIZE(szClass)))
        return NULL;

    HWND hParent = NULL;
    if (_wcsicmp(szClass, L"Shell_SecondaryTrayWnd") == 0)
    {
        hParent = FindWindowExW(hTray, NULL, L"WorkerW", NULL);
    }
    else
    {
        HWND hReBar = FindWindowExW(hTray, NULL, L"ReBarWindow32", NULL);
        hParent = hReBar? FindWindowExW(hReBar, NULL, L"MSTaskSwWClass", NULL) : NULL;
    }

    return hParent? FindWindowExW(hParent, NULL, L"MSTaskListWClass", NULL) : NULL;
}

static bool IsWindowOfCurrentProcess(HWND hWnd)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    return pid == GetCurrentProcessId();
}

static HWND GetTrayForTaskList(HWND hList)
{
    HWND cur = hList;
    for (int i = 0; i < 10 && cur; i++)
    {
        HWND parent = GetParent(cur);
        if (!parent)
            break;
        wchar_t cls[64]{};
        GetClassNameW(parent, cls, 64);
        if (wcscmp(cls, L"Shell_TrayWnd") == 0 ||
            wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0)
            return parent;
        cur = parent;
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

static std::vector<HWND> EnumCurrentProcessTrayTaskLists()
{
    std::vector<HWND> out;
    DWORD curPid = GetCurrentProcessId();

    HWND hMain = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hMain)
    {
        DWORD pid = 0;
        GetWindowThreadProcessId(hMain, &pid);
        if (pid == curPid)
        {
            HWND hList = FindTaskListForTray(hMain);
            if (hList)
                out.push_back(hList);
        }
    }

    HWND hSec = NULL;
    while ((hSec = FindWindowExW(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)))
    {
        DWORD pid = 0;
        GetWindowThreadProcessId(hSec, &pid);
        if (pid!= curPid)
            continue;
        HWND hList = FindTaskListForTray(hSec);
        if (hList)
            out.push_back(hList);
    }
    return out;
}

// --- painting tracker, no USER32 calls in _DrawBar ---

static thread_local HWND tl_currentTaskList = NULL;
static thread_local bool tl_isHorizontal = true;
static thread_local HWND tl_stackList[8]{};
static thread_local bool tl_stackHoriz[8]{};
static thread_local int tl_depth = 0;

static void PushTaskList(HWND hList)
{
    if (tl_depth < 8)
    {
        tl_stackList[tl_depth] = tl_currentTaskList;
        tl_stackHoriz[tl_depth] = tl_isHorizontal;
        tl_depth++;
    }
    tl_currentTaskList = hList;
    HWND hTray = GetTrayForTaskList(hList);
    tl_isHorizontal = hTray? IsTrayHorizontal(hTray) : true;
}

static void PopTaskList()
{
    if (tl_depth > 0)
    {
        tl_depth--;
        tl_currentTaskList = tl_stackList[tl_depth];
        tl_isHorizontal = tl_stackHoriz[tl_depth];
    }
    else
    {
        tl_currentTaskList = NULL;
        tl_isHorizontal = true;
    }
}

static std::mutex g_subclassedMutex;
static std::unordered_set<HWND> g_subclassedTaskLists;

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
        std::lock_guard<std::mutex> lock(g_subclassedMutex);
        g_subclassedTaskLists.erase(hWnd);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static void SubclassTaskListIfNew(HWND hList)
{
    if (!hList ||!IsWindow(hList) ||!IsWindowOfCurrentProcess(hList))
        return;
    bool isNew = false;
    {
        std::lock_guard<std::mutex> lock(g_subclassedMutex);
        isNew = g_subclassedTaskLists.insert(hList).second;
    }
    if (isNew)
        WindhawkUtils::SetWindowSubclassFromAnyThread(hList, TaskListSubclassProc, 0);
}

HWND WINAPI CreateWindowExW_hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hWnd = CreateWindowExW_orig(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hWnd && lpClassName && ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xFFFF))
    {
        if (_wcsicmp(lpClassName, L"MSTaskListWClass") == 0)
        {
            SubclassTaskListIfNew(hWnd);
        }
    }
    return hWnd;
}

void CALCON CTaskBtnGroup__DrawBar_hook(void *pThis, HDC hDC, void *pRenderInfo, PBUTTONRENDERINFOSTATES pRenderStates)
{
    LPRECT lprcDest = (LPRECT)((char *)pRenderInfo + 4);

    bool isHorizontal = tl_currentTaskList? tl_isHorizontal : true;

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

    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_hook, &CreateWindowExW_orig))
    {
        Wh_Log(L"Failed to hook CreateWindowExW");
        return FALSE;
    }

    for (HWND hList : EnumCurrentProcessTrayTaskLists())
        SubclassTaskListIfNew(hList);

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
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hList, TaskListSubclassProc);
}
