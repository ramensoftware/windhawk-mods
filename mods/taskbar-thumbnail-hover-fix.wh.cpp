// ==WindhawkMod==
// @id              taskbar-thumbnail-hover-fix
// @name            Taskbar Thumbnail Hover Fix
// @description     Fixes hover state not updating when closing a thumbnail and another slides into position
// @version         1.0
// @author          Alchemy
// @github          https://github.com/alchemyyy
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Thumbnail Hover Fix

When you close a window via the taskbar thumbnail close button and another
window slides into its position, Windows doesn't register that the mouse is
now hovering over the new thumbnail until you move the cursor.

This mod fixes that issue by sending a minimal mouse movement after a window
closes while the cursor is over the thumbnail preview area.

Only the explorer.exe instance that owns the taskbar will activate this mod.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#define MUTEX_NAME L"Local\\TaskbarThumbnailHoverFix"

static HANDLE g_workerThread = nullptr;
static HANDLE g_mutex = nullptr;
static HWINEVENTHOOK g_winEventHook = nullptr;
static volatile bool g_running = false;

void TriggerHoverRefresh() {
    INPUT inputs[2] = {};

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_MOVE;
    inputs[0].mi.dy = -1;

    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_MOVE;
    inputs[1].mi.dy = 1;

    SendInput(2, inputs, sizeof(INPUT));
}

bool IsCursorOverThumbnail() {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return false;
    }

    HWND hwnd = WindowFromPoint(pt);
    if (!hwnd) {
        return false;
    }

    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root) {
        root = hwnd;
    }

    WCHAR className[256];
    if (GetClassName(root, className, ARRAYSIZE(className))) {
        if (wcsstr(className, L"Xaml") != nullptr) {
            return true;
        }
    }

    return false;
}

void CALLBACK WinEventProc(
    HWINEVENTHOOK hWinEventHook,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG idChild,
    DWORD idEventThread,
    DWORD dwmsEventTime
) {
    if (idObject != OBJID_WINDOW || idChild != 0) {
        return;
    }

    if (IsCursorOverThumbnail()) {
        TriggerHoverRefresh();
    }
}

bool IsMutexOwned() {
    HANDLE testMutex = OpenMutexW(SYNCHRONIZE, FALSE, MUTEX_NAME);
    if (testMutex) {
        CloseHandle(testMutex);
        return true;
    }
    return false;
}

DWORD WINAPI WorkerThreadProc(LPVOID lpParam) {
    DWORD ourPid = GetCurrentProcessId();

    // Wait for taskbar to appear and verify ownership
    while (g_running) {
        if (IsMutexOwned()) {
            Wh_Log(L"Another instance already active");
            return 0;
        }

        HWND taskbarWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (taskbarWnd) {
            DWORD taskbarPid = 0;
            GetWindowThreadProcessId(taskbarWnd, &taskbarPid);

            if (taskbarPid != ourPid) {
                Wh_Log(L"Taskbar owned by different process");
                return 0;
            }

            break;
        }

        Sleep(500);
    }

    if (!g_running) {
        return 0;
    }

    // Acquire mutex
    g_mutex = CreateMutexW(nullptr, TRUE, MUTEX_NAME);
    if (!g_mutex) {
        Wh_Log(L"Failed to create mutex: %u", GetLastError());
        return 0;
    }
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(g_mutex);
        g_mutex = nullptr;
        Wh_Log(L"Another instance claimed mutex first");
        return 0;
    }

    // Install WinEvent hook
    g_winEventHook = SetWinEventHook(
        EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY,
        nullptr,
        WinEventProc,
        0, 0,
        WINEVENT_OUTOFCONTEXT
    );

    if (!g_winEventHook) {
        Wh_Log(L"Failed to install WinEventHook: %u", GetLastError());
        ReleaseMutex(g_mutex);
        CloseHandle(g_mutex);
        g_mutex = nullptr;
        return 1;
    }

    Wh_Log(L"Hook installed successfully");

    // Message pump for WinEventHook
    MSG msg;
    while (g_running) {
        DWORD result = MsgWaitForMultipleObjects(0, nullptr, FALSE, 100, QS_ALLINPUT);
        if (result == WAIT_OBJECT_0) {
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    g_running = false;
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    if (g_winEventHook) {
        UnhookWinEvent(g_winEventHook);
        g_winEventHook = nullptr;
    }

    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing");

    g_running = true;
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);

    if (!g_workerThread) {
        Wh_Log(L"Failed to create worker thread: %u", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    g_running = false;

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 2000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    if (g_mutex) {
        ReleaseMutex(g_mutex);
        CloseHandle(g_mutex);
        g_mutex = nullptr;
    }
}
