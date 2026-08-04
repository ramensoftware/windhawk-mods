// ==WindhawkMod==
// @id              auto-hide-taskbar-desktop-only
// @name            auto-hide-taskbar-desktop-only
// @version         1.0.0
// @author          qwertyuiop00-art
// @github          https://github.com/qwertyuiop00-art
// @description     Hides taskbar on desktop, shows when an app is active.
// @architecture    x86-64
// @include         explorer.exe
// @compilerOptions -lshell32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Auto-hide Taskbar on Desktop Only

Automatically hides the taskbar when switching to the desktop, and restores it when any application is active/focused.
*/
// ==/WindhawkModReadme==

#include <windows.h>

static HANDLE g_thread = NULL;
static DWORD g_threadId = 0;
static HANDLE g_readyEvent = NULL;
static UINT g_originalState = 0;

void SetTaskbarAutoHide(bool enable) {
    APPBARDATA abd = { sizeof(APPBARDATA) };
    abd.hWnd = FindWindow(L"Shell_TrayWnd", NULL);
    abd.lParam = enable ? ABS_AUTOHIDE : ABS_ALWAYSONTOP;
    SHAppBarMessage(ABM_SETSTATE, &abd);
}

void UpdateStateForWindow(HWND hwnd) {
    if (!hwnd) hwnd = GetForegroundWindow();
    if (!hwnd) return;

    WCHAR className[256] = {0};
    GetClassNameW(hwnd, className, 256);

    bool isDesktop = (wcscmp(className, L"Progman") == 0 || wcscmp(className, L"WorkerW") == 0);
    SetTaskbarAutoHide(isDesktop);
}

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, 
                            LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (event == EVENT_SYSTEM_FOREGROUND) {
        UpdateStateForWindow(hwnd);
    }
}

DWORD WINAPI WinEventHookThread(LPVOID) {
    // Force message queue creation to avoid PostThreadMessage race condition
    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
    
    HWINEVENTHOOK hook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, 
                                         NULL, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    // Signal readiness
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

BOOL Wh_ModInit() {
    // Save user's original taskbar auto-hide setting
    APPBARDATA abd = { sizeof(APPBARDATA) };
    g_originalState = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);

    g_readyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!g_readyEvent) return FALSE;

    g_thread = CreateThread(NULL, 0, WinEventHookThread, NULL, 0, &g_threadId);
    if (!g_thread) {
        CloseHandle(g_readyEvent);
        return FALSE;
    }

    // Wait safely for thread message queue to initialize
    WaitForSingleObject(g_readyEvent, 3000);
    CloseHandle(g_readyEvent);
    g_readyEvent = NULL;

    return TRUE;
}

void Wh_ModUninit() {
    if (g_thread) {
        PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 2000);
        CloseHandle(g_thread);
        g_thread = NULL;
    }

    // Restore original state
    APPBARDATA abd = { sizeof(APPBARDATA) };
    abd.hWnd = FindWindow(L"Shell_TrayWnd", NULL);
    abd.lParam = g_originalState;
    SHAppBarMessage(ABM_SETSTATE, &abd);
}
