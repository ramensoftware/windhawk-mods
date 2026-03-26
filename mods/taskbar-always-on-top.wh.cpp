// ==WindhawkMod==
// @id              taskbar-always-on-top
// @name            Taskbar Always On Top
// @description     Keeps Shell_TrayWnd topmost by rewriting WM_WINDOWPOSCHANGING
// @version         0.1
// @author          meteoni
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

#include <windows.h>

static HWND g_taskbarWnd = nullptr;
static WNDPROC g_originalTaskbarProc = nullptr;

static LRESULT CALLBACK TaskbarSubclassProc(
    HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_WINDOWPOSCHANGING && lParam) {
        auto* wp = reinterpret_cast<WINDOWPOS*>(lParam);

        // Ignore cases where no Z-order change is requested.
        if (!(wp->flags & SWP_NOZORDER)) {
            // force every attempted Z-order change to keep the taskbar topmost.
            wp->hwndInsertAfter = HWND_TOPMOST;
            wp->flags &= ~SWP_NOZORDER;

            Wh_Log(L"Blocked taskbar Z-order demotion, flags=0x%08X", wp->flags);
        }
    }

    return CallWindowProcW(g_originalTaskbarProc, hwnd, msg, wParam, lParam);
}

static void InstallTaskbarSubclass()
{
    if (g_originalTaskbarProc) {
        return;
    }

    g_taskbarWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!g_taskbarWnd) {
        Wh_Log(L"Shell_TrayWnd not found");
        return;
    }

    auto prev = reinterpret_cast<WNDPROC>(
        SetWindowLongPtrW(
            g_taskbarWnd,
            GWLP_WNDPROC,
            reinterpret_cast<LONG_PTR>(TaskbarSubclassProc)));

    if (!prev) {
        Wh_Log(L"SetWindowLongPtrW failed: %lu", GetLastError());
        g_taskbarWnd = nullptr;
        return;
    }

    g_originalTaskbarProc = prev;

    // Immediately push it to topmost once on load.
    SetWindowPos(
        g_taskbarWnd,
        HWND_TOPMOST,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    Wh_Log(L"Taskbar subclass installed");
}

static void RemoveTaskbarSubclass()
{
    if (g_taskbarWnd && g_originalTaskbarProc) {
        SetWindowLongPtrW(
            g_taskbarWnd,
            GWLP_WNDPROC,
            reinterpret_cast<LONG_PTR>(g_originalTaskbarProc));

        Wh_Log(L"Taskbar subclass removed");
    }

    g_originalTaskbarProc = nullptr;
    g_taskbarWnd = nullptr;
}

BOOL Wh_ModInit()
{
    return TRUE;
}

void Wh_ModAfterInit()
{
    InstallTaskbarSubclass();
}

void Wh_ModBeforeUninit()
{
    RemoveTaskbarSubclass();
}
