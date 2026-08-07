// ==WindhawkMod==
// @id              autohide-taskbar-on-desktop
// @name            autohide-taskbar-on-desktop
// @version         1.0.1
// @author          qwertyuiop00-art
// @github          https://github.com/qwertyuiop00-art
// @description     Hides taskbar on desktop, shows when an app is active.
// @architecture    x86-64
// @include         windhawk.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Autohide Taskbar on Desktop 

Automatically hides the taskbar when switching to the desktop, and restores it when any application is active/focused.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shellapi.h>
#include <windhawk_api.h>

static HANDLE g_thread = NULL;
static DWORD g_threadId = 0;
static HANDLE g_readyEvent = NULL;
static int g_lastApplied = -1;

void SetTaskbarAutoHide(bool enable) {
    APPBARDATA abd = { sizeof(APPBARDATA) };
    UINT state = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
    
    // Only flip the ABS_AUTOHIDE bit, leave others like ABS_ALWAYSONTOP alone
    UINT newState = enable ? (state | ABS_AUTOHIDE) : (state & ~ABS_AUTOHIDE);
    
    if (state != newState) {
        abd.lParam = newState;
        SHAppBarMessage(ABM_SETSTATE, &abd);
        Wh_Log(L"> Taskbar state changed to: %d", enable);
    }
}

void UpdateStateForWindow(HWND hwnd) {
    if (!hwnd) hwnd = GetForegroundWindow();
    if (!hwnd) return;

    WCHAR className[256] = {0};
    GetClassNameW(hwnd, className, ARRAYSIZE(className));

    // Ignore shell flyouts and start menu to prevent visual flickering
    if (wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0 ||
        wcscmp(className, L"XamlExplorerHostIslandWindow") == 0 ||
        wcscmp(className, L"Shell_TrayWnd") == 0) {
        return;
    }

    bool isDesktop = (wcscmp(className, L"Progman") == 0 || wcscmp(className, L"WorkerW") == 0);
    
    // Only apply if the state has actually changed
    if (g_lastApplied != (int)isDesktop) {
        g_lastApplied = isDesktop;
        SetTaskbarAutoHide(isDesktop);
        Wh_Log(L"> Window switched. isDesktop=%d, class=%s", isDesktop, className);
    }
}

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, 
                            LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    // Filter to the specific window object as recommended
    if (event != EVENT_SYSTEM_FOREGROUND || idObject != OBJID_WINDOW ||
        idChild != CHILDID_SELF || !hwnd) {
        return;
    }
    UpdateStateForWindow(hwnd);
}

DWORD WINAPI WinEventHookThread(LPVOID) {
    MSG msg;
    // Force message queue creation
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
    
    HWINEVENTHOOK hook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, 
                                         NULL, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    if (g_readyEvent) {
        SetEvent(g_readyEvent);
    }

    UpdateStateForWindow(GetForegroundWindow());

    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (hook) UnhookWinEvent(hook);
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"> WhTool_ModInit started");

    // Lazily capture original state only once and persist it so it survives restarts
    if (Wh_GetIntValue(L"OriginalStateSaved") != 1) {
        APPBARDATA abd = { sizeof(APPBARDATA) };
        UINT state = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
        Wh_SetIntValue(L"OriginalState", state);
        Wh_SetIntValue(L"OriginalStateSaved", 1);
    }

    g_readyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!g_readyEvent) return FALSE;

    g_thread = CreateThread(NULL, 0, WinEventHookThread, NULL, 0, &g_threadId);
    if (!g_thread) {
        CloseHandle(g_readyEvent);
        return FALSE;
    }

    WaitForSingleObject(g_readyEvent, 3000);
    CloseHandle(g_readyEvent);
    g_readyEvent = NULL;

    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"> WhTool_ModUninit started");
    
    if (g_thread) {
        // Safely close thread handling race conditions
        while (!PostThreadMessage(g_threadId, WM_QUIT, 0, 0)) {
            Sleep(10);
        }
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = NULL;
    }

    // Restore user's original taskbar state cleanly
    if (Wh_GetIntValue(L"OriginalStateSaved") == 1) {
        UINT originalState = (UINT)Wh_GetIntValue(L"OriginalState");
        APPBARDATA abd = { sizeof(APPBARDATA) };
        abd.lParam = originalState;
        SHAppBarMessage(ABM_SETSTATE, &abd);
        // Clear saved state so next time we capture a fresh one
        Wh_SetIntValue(L"OriginalStateSaved", 0);
    }
}
