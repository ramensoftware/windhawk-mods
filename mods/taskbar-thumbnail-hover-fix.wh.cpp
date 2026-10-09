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

static HANDLE g_workerThread = nullptr;
static DWORD g_taskbarThreadId = 0;

// Find the thread ID that created Shell_TrayWnd across all processes
DWORD FindTaskbarThreadId() {
    DWORD taskbarThreadId = 0;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            wchar_t className[32];
            if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                wcscmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<DWORD*>(lParam) = GetWindowThreadProcessId(hWnd, nullptr);
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&taskbarThreadId));

    return taskbarThreadId;
}

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

    // First time we encounter a window destruction, verify we're in the taskbar owner process
    if (g_taskbarThreadId == 0) {
        DWORD taskbarThreadId = FindTaskbarThreadId();
        if (taskbarThreadId == 0) {
            // Shell_TrayWnd doesn't exist yet, skip for now and try again next time
            return false;
        }
        
        // Check if the taskbar thread belongs to our process
        DWORD currentProcessId = GetCurrentProcessId();
        HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, taskbarThreadId);
        if (!hThread) {
            Wh_Log(L"Shell_TrayWnd exists but not in our process - terminating mod");
            if (g_workerThread) {
                PostThreadMessage(GetThreadId(g_workerThread), WM_APP, 0, 0);
            }
            return false;
        }
        
        DWORD threadProcessId = GetProcessIdOfThread(hThread);
        CloseHandle(hThread);
        
        if (threadProcessId != currentProcessId) {
            Wh_Log(L"Shell_TrayWnd exists but not in our process - terminating mod");
            if (g_workerThread) {
                PostThreadMessage(GetThreadId(g_workerThread), WM_APP, 0, 0);
            }
            return false;
        }
        
        // Cache the taskbar thread ID
        g_taskbarThreadId = taskbarThreadId;
        Wh_Log(L"Taskbar ownership verified - thread ID: %u", g_taskbarThreadId);
    }
    
    // Check if window belongs to taskbar thread
    DWORD windowThreadId = GetWindowThreadProcessId(root, nullptr);
    if (windowThreadId != g_taskbarThreadId) {
        return false;
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

DWORD WINAPI WorkerThreadProc(LPVOID lpParam) {
    // Install WinEvent hook
    HWINEVENTHOOK winEventHook = SetWinEventHook(
        EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY,
        nullptr,
        WinEventProc,
        0, 0,
        WINEVENT_OUTOFCONTEXT
    );

    if (!winEventHook) {
        Wh_Log(L"Failed to install WinEventHook: %u", GetLastError());
        return 1;
    }

    Wh_Log(L"Hook installed successfully");

    // Message pump for WinEventHook
    BOOL bRet;
    MSG msg;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (bRet == -1) {
            msg.wParam = 0;
            break;
        }

        if (msg.hwnd == NULL && msg.message == WM_APP) {
            PostQuitMessage(0);
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (winEventHook) {
        UnhookWinEvent(winEventHook);
    }

    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing");

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);

    if (!g_workerThread) {
        Wh_Log(L"Failed to create worker thread: %u", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_workerThread) {
        PostThreadMessage(GetThreadId(g_workerThread), WM_APP, 0, 0);
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
}
