// ==WindhawkMod==
// @id              taskbar-z-order-override
// @name            Taskbar Z-Order Override
// @description     Forces the main taskbar to stay at the top or bottom of the Z-order
// @version         0.2
// @author          meteoni
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==
// ==WindhawkModSettings==
/*
- TaskbarZOrder: top
  $name: Taskbar Z-Order
  $description: Choose whether the taskbar is forced to the topmost band or the bottom
  $options:
    - top: Always on top
    - bottom: Always at bottom
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>

enum class TaskbarZOrder {
    Top,
    Bottom,
};

TaskbarZOrder g_taskbarZOrder {};

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
            wp->hwndInsertAfter = 
            (g_taskbarZOrder == TaskbarZOrder::Top)
            ? HWND_TOPMOST
            : HWND_BOTTOM;
            
            wp->flags &= ~SWP_NOZORDER;

            Wh_Log(L"Rewrote taskbar Z-order change, flags=0x%08X", wp->flags);
        }
    }

    return CallWindowProcW(g_originalTaskbarProc, hwnd, msg, wParam, lParam);
}

static void ApplyTaskbarZOrder() {
    if (!g_taskbarWnd) {
        return;
    }

    auto order = (g_taskbarZOrder == TaskbarZOrder::Top)
            ? HWND_TOPMOST
            : HWND_BOTTOM;

    if (!SetWindowPos(
                g_taskbarWnd,
                order,
                0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE)) {
            Wh_Log(L"SetWindowPos failed: %lu", GetLastError());
            return;
        };

    Wh_Log(L"Taskbar Z-order updated");
}

static void RestoreTaskbarZOrder(){
    auto prev = g_taskbarZOrder;
    g_taskbarZOrder = TaskbarZOrder::Top;
    ApplyTaskbarZOrder();
    g_taskbarZOrder = prev;
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

    SetLastError(0);
    auto prev = reinterpret_cast<WNDPROC>(
        SetWindowLongPtrW(
            g_taskbarWnd,
            GWLP_WNDPROC,
            reinterpret_cast<LONG_PTR>(TaskbarSubclassProc)));

    if (!prev && GetLastError() != 0) {
        Wh_Log(L"SetWindowLongPtrW failed: %lu", GetLastError());
        g_taskbarWnd = nullptr;
        return;
    }

    g_originalTaskbarProc = prev;

    // Immediately push it to top/bottom once loaded.
    ApplyTaskbarZOrder();
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

void readSettings(){
    PCWSTR raw {Wh_GetStringSetting(L"TaskbarZOrder")};
    std::wstring placement = raw ? raw : L"";
    Wh_FreeStringSetting(raw);

    g_taskbarZOrder = (placement == L"top")
        ? TaskbarZOrder::Top
        : TaskbarZOrder::Bottom;
}

BOOL Wh_ModInit()
{
    readSettings();
    return TRUE;
}

void Wh_ModAfterInit()
{
    InstallTaskbarSubclass();
}

void Wh_ModSettingsChanged(){
    readSettings();
    ApplyTaskbarZOrder();
}

void Wh_ModBeforeUninit()
{
    RemoveTaskbarSubclass();
}
