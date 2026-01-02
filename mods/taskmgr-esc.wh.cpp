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
 
#include <windows.h>
#include <psapi.h>
#include <windhawk_utils.h>

static HHOOK g_keyboardHook = nullptr;

void LogForegroundWindowInfo()
{
    HWND hwnd = GetForegroundWindow();
    if (!hwnd)
    {
        Wh_Log(L"[ESC] No foreground window");
        return;
    }

    wchar_t className[256] = L"";
    GetClassNameW(hwnd, className, 256);

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);

    wchar_t processName[MAX_PATH] = L"";
    if (pid)
    {
        HANDLE hProc = OpenProcess(
            PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ,
            FALSE,
            pid
        );
        if (hProc)
        {
            GetModuleBaseNameW(hProc, nullptr, processName, MAX_PATH);
            CloseHandle(hProc);
        }
    }

    Wh_Log(
        L"[ESC] Foreground hwnd=0x%p class=\"%s\" pid=%lu process=\"%s\"",
        hwnd,
        className,
        pid,
        processName
    );
}

bool IsTaskManagerForeground()
{
    HWND hwnd = GetForegroundWindow();
    if (!hwnd)
        return false;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid)
        return false;

    HANDLE hProc = OpenProcess(
        PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ,
        FALSE,
        pid
    );
    if (!hProc)
        return false;

    wchar_t processName[MAX_PATH] = L"";
    GetModuleBaseNameW(hProc, nullptr, processName, MAX_PATH);
    CloseHandle(hProc);

    if (_wcsicmp(processName, L"taskmgr.exe") != 0)
        return false;

    return true;
}

LRESULT CALLBACK LowLevelKeyboardProc(
    int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        auto* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) &&
            kb->vkCode == VK_ESCAPE)
        {
            Wh_Log(L"[ESC] Key pressed");

            LogForegroundWindowInfo();

            if (IsTaskManagerForeground())
            {
                HWND hwnd = GetForegroundWindow();
                if (hwnd)
                {
                    Wh_Log(L"[ESC] Closing Task Manager");
                    PostMessageW(hwnd, WM_CLOSE, 0, 0);
                    return 1;  
                }
            }
            else
            {
                Wh_Log(L"[ESC] Foreground is NOT Task Manager");
            }
        }
    }

    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

BOOL Wh_ModInit()
{
    Wh_Log(L"TaskManager ESC Mod init");

    g_keyboardHook = SetWindowsHookExW(
        WH_KEYBOARD_LL,
        LowLevelKeyboardProc,
        GetModuleHandleW(nullptr),
        0
    );

    if (!g_keyboardHook)
    {
        Wh_Log(L"Failed to install keyboard hook");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"TaskManager ESC Mod uninit");

    if (g_keyboardHook)
    {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
}