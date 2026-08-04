// ==WindhawkMod==
// @id              autohide-taskbar-on-desktop
// @name            Hide Taskbar on Desktop Only
// @version         1.0.0
// @author          qwertyuiop00-art
// @github          https://github.com/qwertyuiop00-art
// @description     Hides taskbar on desktop, shows when an app is active.
// @architecture    x86-64
// @include         windhawk.exe
// @compilerOptions -lshell32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Taskbar on Desktop Only

This tool mod automatically hides the Windows taskbar when you switch to the desktop and restores it when applications are active/focused.

## Features
- Keeps desktop clean by auto-hiding taskbar.
- Restores taskbar instantly when switching to open applications.
- Preserves user's original taskbar auto-hide settings on unload.
*/
// ==/WindhawkModReadme==

#include <windows.h>

HANDLE g_thread = NULL;
DWORD g_threadId = 0;
UINT g_originalState = 0;

void UpdateTaskbarState(HWND hwnd) {
    if (!hwnd) hwnd = GetForegroundWindow();
    
    WCHAR className[256] = {0};
    if (hwnd) {
        GetClassName(hwnd, className, 256);
    }

    bool isDesktop = (wcscmp(className, L"Progman") == 0 || wcscmp(className, L"WorkerW") == 0);

    APPBARDATA abd = { sizeof(APPBARDATA) };
    UINT currentState = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
    
    UINT newState = isDesktop ? (currentState | ABS_AUTOHIDE) : (currentState & ~ABS_AUTOHIDE);

    if (newState != currentState) {
        abd.lParam = newState;
        SHAppBarMessage(ABM_SETSTATE, &abd);
    }
}

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, 
                            LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (event == EVENT_SYSTEM_FOREGROUND && hwnd) {
        UpdateTaskbarState(hwnd);
    }
}

DWORD WINAPI WinEventHookThread(LPVOID) {
    HWINEVENTHOOK hook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                                         NULL, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!hook) return 1;

    // Apply state initially on thread start
    UpdateTaskbarState(GetForegroundWindow());

    MSG msg;
    BOOL bRet;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (bRet == -1) break;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWinEvent(hook);
    return 0;
}

BOOL WhTool_ModInit() {
    APPBARDATA abd = { sizeof(APPBARDATA) };
    g_originalState = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);

    g_thread = CreateThread(NULL, 0, WinEventHookThread, NULL, 0, &g_threadId);
    return g_thread != NULL;
}

void WhTool_ModUninit() {
    if (g_thread) {
        PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = NULL;
    }

    // Restore user's original taskbar auto-hide setting
    APPBARDATA abd = { sizeof(APPBARDATA) };
    abd.lParam = g_originalState;
    SHAppBarMessage(ABM_SETSTATE, &abd);
}
