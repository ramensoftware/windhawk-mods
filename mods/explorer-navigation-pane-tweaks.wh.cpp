// ==WindhawkMod==
// @id              explorer-navigation-pane-tweaks
// @name            Explorer Navigation Pane Tweaks
// @description     Adjusts the navigation pane tree indent and treeview visual styles in Windows Explorer
// @version         1.0
// @author          Languster
// @github          https://github.com/Languster
// @include         %SystemRoot%\explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Navigation Pane Tweaks

Adjusts the indentation and visual tree style of the navigation pane in Windows Explorer.

## Before / After

### Default Explorer

![Before](https://raw.githubusercontent.com/Languster/ExplorerNavHook/main/screenshots/before.jpg)

### With Explorer Navigation Pane Tweaks

![After](https://raw.githubusercontent.com/Languster/ExplorerNavHook/main/screenshots/after.jpg)

## Features

- changes the navigation tree indent
- can remove expand/collapse buttons
- can remove connector lines
- can remove root lines

## Settings

- **Target indent**: controls how far the tree items are shifted
- **Remove TVS_HASBUTTONS**: removes tree buttons
- **Remove TVS_HASLINES**: removes connector lines
- **Remove TVS_LINESATROOT**: removes root lines

## Compatibility

- designed for Windows Explorer (`explorer.exe`)
- tested with the original ExplorerNavHook behavior migrated to Windhawk
- behavior may vary between different Windows builds and Explorer variants

## Notes

- targets `explorer.exe`
- based on the original ExplorerNavHook project
- changes apply to the Explorer navigation tree

## Limitations

- this mod targets the navigation tree in Explorer, not every tree view in the system
- some Explorer variants or future Windows updates may behave differently
- visual results depend on the current Explorer implementation
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- TargetIndent: 25
  $name: Navigation tree indent
  $description: Controls the horizontal indent of items in the Explorer navigation pane

- RemoveHasButtons: true
  $name: Remove expand/collapse buttons
  $description: Removes the tree expand/collapse buttons in the navigation pane

- RemoveHasLines: true
  $name: Remove connector lines
  $description: Removes the tree connector lines in the navigation pane

- RemoveLinesAtRoot: true
  $name: Remove root lines
  $description: Removes root-level tree lines and the first root expand visual
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <vector>

struct ExplorerState
{
    HWND hExplorer;
    HWND hTree;
    bool repatchPending;
};

static HANDLE g_hWorkerThread = nullptr;
static DWORD g_WorkerThreadId = 0;
static HWINEVENTHOOK g_hEventHookShow = nullptr;
static HWINEVENTHOOK g_hEventHookCreate = nullptr;

static int g_TargetIndent = 25;
static BOOL g_RemoveHasButtons = TRUE;
static BOOL g_RemoveHasLines = TRUE;
static BOOL g_RemoveLinesAtRoot = TRUE;

static CRITICAL_SECTION g_StateLock;
static std::vector<ExplorerState> g_States;

#define TABMODE_REPATCH_DELAY1_MS 120
#define TABMODE_REPATCH_DELAY2_MS 320

static void LoadSettings()
{
    g_TargetIndent = Wh_GetIntSetting(L"TargetIndent");
    g_RemoveHasButtons = Wh_GetIntSetting(L"RemoveHasButtons");
    g_RemoveHasLines = Wh_GetIntSetting(L"RemoveHasLines");
    g_RemoveLinesAtRoot = Wh_GetIntSetting(L"RemoveLinesAtRoot");

    Wh_Log(L"Settings loaded, indent=%d", g_TargetIndent);
}

static bool IsExplorerTopWindow(HWND hwnd)
{
    if (!IsWindow(hwnd))
        return false;

    wchar_t cls[128] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));

    return lstrcmpiW(cls, L"CabinetWClass") == 0 ||
           lstrcmpiW(cls, L"ExploreWClass") == 0;
}

static HWND FindExplorerTopWindowFromChild(HWND hwnd)
{
    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (root && IsExplorerTopWindow(root))
        return root;

    return nullptr;
}

static bool IsLikelyNavTree(HWND hwnd)
{
    if (!IsWindow(hwnd))
        return false;

    wchar_t cls[128] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));

    if (lstrcmpiW(cls, WC_TREEVIEWW) != 0 &&
        lstrcmpiW(cls, L"SysTreeView32") != 0)
        return false;

    RECT rc{};
    if (!GetWindowRect(hwnd, &rc))
        return false;

    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;

    if (w < 120 || h < 200)
        return false;

    return FindExplorerTopWindowFromChild(hwnd) != nullptr;
}

static void PatchTree(HWND hTree)
{
    if (!IsWindow(hTree))
        return;

    LONG_PTR style = GetWindowLongPtrW(hTree, GWL_STYLE);

    if (g_RemoveHasButtons)
        style &= ~TVS_HASBUTTONS;

    if (g_RemoveHasLines)
        style &= ~TVS_HASLINES;

    if (g_RemoveLinesAtRoot)
        style &= ~TVS_LINESATROOT;

    SetWindowLongPtrW(hTree, GWL_STYLE, style);

    SendMessageW(hTree, TVM_SETINDENT, (WPARAM)g_TargetIndent, 0);

    SetWindowPos(
        hTree,
        nullptr,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    InvalidateRect(hTree, nullptr, TRUE);
    UpdateWindow(hTree);

    Wh_Log(L"Patched tree hwnd=%p", hTree);
}

static BOOL CALLBACK FindTreeEnumProc(HWND hwnd, LPARAM lParam)
{
    HWND* pFound = (HWND*)lParam;
    if (*pFound)
        return FALSE;

    if (IsLikelyNavTree(hwnd))
    {
        *pFound = hwnd;
        return FALSE;
    }

    return TRUE;
}

static HWND FindNavTreeInExplorer(HWND hExplorer)
{
    if (!IsExplorerTopWindow(hExplorer))
        return nullptr;

    HWND found = nullptr;
    EnumChildWindows(hExplorer, FindTreeEnumProc, (LPARAM)&found);
    return found;
}

static void PatchExplorerWindow(HWND hExplorer)
{
    HWND hTree = FindNavTreeInExplorer(hExplorer);
    if (hTree)
        PatchTree(hTree);
}

static ExplorerState* GetOrCreateState_NoLock(HWND hExplorer)
{
    for (auto& s : g_States)
    {
        if (s.hExplorer == hExplorer)
            return &s;
    }

    ExplorerState s{};
    s.hExplorer = hExplorer;
    s.hTree = nullptr;
    s.repatchPending = false;
    g_States.push_back(s);
    return &g_States.back();
}

static void CleanupDeadStates()
{
    EnterCriticalSection(&g_StateLock);

    for (size_t i = 0; i < g_States.size();)
    {
        if (!IsWindow(g_States[i].hExplorer))
            g_States.erase(g_States.begin() + i);
        else
            ++i;
    }

    LeaveCriticalSection(&g_StateLock);
}

struct RepatchContext
{
    HWND hExplorer;
};

static DWORD WINAPI RepatchThreadProc(LPVOID param)
{
    RepatchContext* ctx = (RepatchContext*)param;
    if (!ctx)
        return 0;

    HWND hExplorer = ctx->hExplorer;
    delete ctx;

    Sleep(TABMODE_REPATCH_DELAY1_MS);
    if (IsWindow(hExplorer))
    {
        PatchExplorerWindow(hExplorer);
        Wh_Log(L"Tabmode repatch #1 done");
    }

    Sleep(TABMODE_REPATCH_DELAY2_MS - TABMODE_REPATCH_DELAY1_MS);
    if (IsWindow(hExplorer))
    {
        PatchExplorerWindow(hExplorer);
        Wh_Log(L"Tabmode repatch #2 done");
    }

    EnterCriticalSection(&g_StateLock);
    for (auto& s : g_States)
    {
        if (s.hExplorer == hExplorer)
        {
            s.repatchPending = false;
            break;
        }
    }
    LeaveCriticalSection(&g_StateLock);

    return 0;
}

static void ScheduleTabModeRepatch(HWND hExplorer)
{
    if (!IsWindow(hExplorer))
        return;

    bool shouldStart = false;

    EnterCriticalSection(&g_StateLock);
    ExplorerState* s = GetOrCreateState_NoLock(hExplorer);
    if (s && !s->repatchPending)
    {
        s->repatchPending = true;
        shouldStart = true;
    }
    LeaveCriticalSection(&g_StateLock);

    if (!shouldStart)
        return;

    RepatchContext* ctx = new RepatchContext{};
    ctx->hExplorer = hExplorer;

    HANDLE hThread = CreateThread(nullptr, 0, RepatchThreadProc, ctx, 0, nullptr);
    if (hThread)
    {
        CloseHandle(hThread);
        Wh_Log(L"Tabmode repatch scheduled");
    }
    else
    {
        EnterCriticalSection(&g_StateLock);
        for (auto& s : g_States)
        {
            if (s.hExplorer == hExplorer)
            {
                s.repatchPending = false;
                break;
            }
        }
        LeaveCriticalSection(&g_StateLock);

        delete ctx;
    }
}

static void UpdateExplorerStateAndMaybeRepatch(HWND hExplorer)
{
    if (!IsExplorerTopWindow(hExplorer))
        return;

    HWND currentTree = FindNavTreeInExplorer(hExplorer);
    if (!currentTree)
        return;

    bool needRepatch = false;
    HWND oldTree = nullptr;

    EnterCriticalSection(&g_StateLock);

    ExplorerState* s = GetOrCreateState_NoLock(hExplorer);
    if (s)
    {
        oldTree = s->hTree;

        if (s->hTree == nullptr)
        {
            s->hTree = currentTree;
        }
        else if (s->hTree != currentTree)
        {
            s->hTree = currentTree;
            needRepatch = true;
        }
    }

    LeaveCriticalSection(&g_StateLock);

    PatchTree(currentTree);

    if (needRepatch)
    {
        Wh_Log(L"Tree hwnd changed old=%p new=%p", oldTree, currentTree);
        ScheduleTabModeRepatch(hExplorer);
    }
}

static BOOL CALLBACK EnumTopWindowsProc(HWND hwnd, LPARAM)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);

    if (pid == GetCurrentProcessId() && IsExplorerTopWindow(hwnd))
        UpdateExplorerStateAndMaybeRepatch(hwnd);

    return TRUE;
}

static void InitialPatchAllExplorerWindows()
{
    EnumWindows(EnumTopWindowsProc, 0);
}

static void CALLBACK WinEventProc(
    HWINEVENTHOOK,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG,
    DWORD,
    DWORD)
{
    if (!hwnd)
        return;

    if (idObject != OBJID_WINDOW)
        return;

    CleanupDeadStates();

    if (IsExplorerTopWindow(hwnd))
    {
        Wh_Log(L"Top explorer event=%u hwnd=%p", event, hwnd);
        UpdateExplorerStateAndMaybeRepatch(hwnd);
        return;
    }

    if (IsLikelyNavTree(hwnd))
    {
        HWND hExplorer = FindExplorerTopWindowFromChild(hwnd);
        if (hExplorer)
        {
            Wh_Log(L"Nav tree event=%u hwnd=%p", event, hwnd);
            UpdateExplorerStateAndMaybeRepatch(hExplorer);
        }
        else
        {
            PatchTree(hwnd);
        }
        return;
    }

    HWND hExplorer = FindExplorerTopWindowFromChild(hwnd);
    if (hExplorer)
        UpdateExplorerStateAndMaybeRepatch(hExplorer);
}

static DWORD WINAPI WorkerThreadProc(LPVOID)
{
    InitializeCriticalSection(&g_StateLock);

    Wh_Log(L"Worker started");

    InitialPatchAllExplorerWindows();

    DWORD pid = GetCurrentProcessId();

    g_hEventHookShow = SetWinEventHook(
        EVENT_OBJECT_SHOW,
        EVENT_OBJECT_SHOW,
        nullptr,
        WinEventProc,
        pid,
        0,
        WINEVENT_OUTOFCONTEXT);

    g_hEventHookCreate = SetWinEventHook(
        EVENT_OBJECT_CREATE,
        EVENT_OBJECT_CREATE,
        nullptr,
        WinEventProc,
        pid,
        0,
        WINEVENT_OUTOFCONTEXT);

    if (!g_hEventHookShow || !g_hEventHookCreate)
        Wh_Log(L"Failed to install WinEvent hook");
    else
        Wh_Log(L"WinEvent hooks installed");

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hEventHookShow)
    {
        UnhookWinEvent(g_hEventHookShow);
        g_hEventHookShow = nullptr;
    }

    if (g_hEventHookCreate)
    {
        UnhookWinEvent(g_hEventHookCreate);
        g_hEventHookCreate = nullptr;
    }

    DeleteCriticalSection(&g_StateLock);

    Wh_Log(L"Worker stopped");
    return 0;
}

BOOL Wh_ModInit()
{
    LoadSettings();

    g_hWorkerThread = CreateThread(
        nullptr,
        0,
        WorkerThreadProc,
        nullptr,
        0,
        &g_WorkerThreadId);

    if (!g_hWorkerThread)
    {
        Wh_Log(L"Failed to create worker thread");
        return FALSE;
    }

    Wh_Log(L"Mod initialized");
    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"Uninitializing");

    if (g_WorkerThreadId)
        PostThreadMessageW(g_WorkerThreadId, WM_QUIT, 0, 0);

    if (g_hWorkerThread)
    {
        WaitForSingleObject(g_hWorkerThread, 3000);
        CloseHandle(g_hWorkerThread);
        g_hWorkerThread = nullptr;
    }

    g_WorkerThreadId = 0;
}

BOOL Wh_ModSettingsChanged(BOOL* bReload)
{
    if (bReload)
        *bReload = TRUE;

    return TRUE;
}
