// ==WindhawkMod==
// @id              taskmgr-esc
// @name            Press Esc to exit Task Manager
// @description     Press Esc to exit Task Manager
// @name:zh-CN      ESC键退出任务管理器
// @description:zh-CN   ESC键退出任务管理器
// @github          https://github.com/chenxustu1
// @version         1.0
// @author          chenxustu1
// @include         taskmgr.exe
// @license         MIT
// ==/WindhawkMod==

#include <psapi.h>
#include <windhawk_utils.h>
#include <windows.h>

static HHOOK g_keyboardHook = nullptr;
static HANDLE g_thread = nullptr;
static DWORD g_threadId = 0;

HWND GetTaskManagerForegroundWindow() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd)
        return nullptr;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);

    if (pid != GetCurrentProcessId())
        return nullptr;

    return hwnd;
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        auto* kb = (KBDLLHOOKSTRUCT*)lParam;

        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) &&
            kb->vkCode == VK_ESCAPE) {
            Wh_Log(L"[ESC] pressed");

            HWND hwnd = GetTaskManagerForegroundWindow();
            if (hwnd) {
                Wh_Log(L"[ESC] closing task manager");
                PostMessageW(hwnd, WM_CLOSE, 0, 0);
                return 1;
            }
        }
    }

    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

DWORD WINAPI KeyboardThread(LPVOID) {
    Wh_Log(L"Keyboard hook thread started");

    g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc,
                                       GetModuleHandleW(nullptr), 0);

    if (!g_keyboardHook) {
        Wh_Log(L"Failed to install keyboard hook");
        return 0;
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Mod init");

    g_thread =
        CreateThread(nullptr, 0, KeyboardThread, nullptr, 0, &g_threadId);

    return g_thread != nullptr;
}

void Wh_ModUninit() {
    Wh_Log(L"Mod uninit");

    if (g_threadId) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
    }

    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }

    if (g_thread) {
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}