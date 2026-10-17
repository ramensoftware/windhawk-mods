// ==WindhawkMod==
// @id              snap-restore-position
// @name            Restore to Snap Position
// @description     When a snapped window is maximized and then restored, it returns to its snapped position instead of its pre-snap position
// @version         1.0
// @author          pajs
// @github          https://github.com/pajs
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Restore to Snap Position

![Demo](https://i.imgur.com/XqdsB0w.gif)

In Windows, when a window snapped with Snap (for example, to the left half of
the screen) is maximized and then restored, it goes back to the position and
size it had **before** it was snapped.

With this mod, when a snapped window is maximized, its snapped position
becomes the window's "normal" position. The **Restore** button, double-clicking
the title bar and **Win + ↓** then return the window exactly to where it was
snapped.

## Notes
- After being restored, the window comes back as a regular window, not as a
  snapped one, so it is no longer part of a Snap Group.
- The size the window had before it was snapped is no longer remembered.
- Works with the maximize button, double-clicking the title bar and the
  window menu (including dialogs and MDI frame windows), and with programs that
  maximize their own windows through `ShowWindow` or `ShowWindowAsync`.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windhawk_utils.h>

using IsWindowArranged_t = BOOL(WINAPI*)(HWND);
IsWindowArranged_t pIsWindowArranged = nullptr;

using DefWindowProcW_t = decltype(&DefWindowProcW);
DefWindowProcW_t DefWindowProcW_Original = nullptr;

using DefWindowProcA_t = decltype(&DefWindowProcA);
DefWindowProcA_t DefWindowProcA_Original = nullptr;

using DefDlgProcW_t = decltype(&DefDlgProcW);
DefDlgProcW_t DefDlgProcW_Original = nullptr;

using DefDlgProcA_t = decltype(&DefDlgProcA);
DefDlgProcA_t DefDlgProcA_Original = nullptr;

using DefFrameProcW_t = decltype(&DefFrameProcW);
DefFrameProcW_t DefFrameProcW_Original = nullptr;

using DefFrameProcA_t = decltype(&DefFrameProcA);
DefFrameProcA_t DefFrameProcA_Original = nullptr;

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original = nullptr;

using ShowWindowAsync_t = decltype(&ShowWindowAsync);
ShowWindowAsync_t ShowWindowAsync_Original = nullptr;

thread_local bool g_inApply = false;

static bool ScreenToWorkspaceRect(HWND hWnd, RECT* rc) {
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) {
        return true;
    }

    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(monitor, &mi)) {
        return false;
    }

    OffsetRect(rc, -(mi.rcWork.left - mi.rcMonitor.left),
               -(mi.rcWork.top - mi.rcMonitor.top));
    return true;
}

static bool GetSnappedRect(HWND hWnd, RECT* snappedRect) {
    if (!hWnd || !IsWindow(hWnd) || !IsWindowVisible(hWnd)) {
        return false;
    }

    if (IsZoomed(hWnd) || IsIconic(hWnd)) {
        return false;
    }

    if (GetAncestor(hWnd, GA_ROOT) != hWnd) {
        return false;
    }

    WINDOWPLACEMENT wp{};
    wp.length = sizeof(wp);
    if (!GetWindowPlacement(hWnd, &wp)) {
        return false;
    }

    RECT rc;
    if (!GetWindowRect(hWnd, &rc)) {
        return false;
    }

    if (!ScreenToWorkspaceRect(hWnd, &rc)) {
        return false;
    }

    if (EqualRect(&rc, &wp.rcNormalPosition)) {
        return false;
    }

    if (pIsWindowArranged && !pIsWindowArranged(hWnd)) {
        return false;
    }

    *snappedRect = rc;
    return true;
}

static void UseSnappedRectAsNormal(HWND hWnd) {
    if (g_inApply) {
        return;
    }

    RECT snappedRect;
    if (!GetSnappedRect(hWnd, &snappedRect)) {
        return;
    }

    WINDOWPLACEMENT wp{};
    wp.length = sizeof(wp);
    if (!GetWindowPlacement(hWnd, &wp)) {
        return;
    }

    wp.flags = 0;
    wp.showCmd = SW_SHOWNOACTIVATE;
    wp.rcNormalPosition = snappedRect;

    g_inApply = true;
    SetWindowPlacement(hWnd, &wp);
    g_inApply = false;

    Wh_Log(L"Saved snap position for window %p", hWnd);
}

static void OnMessage(HWND hWnd, UINT msg, WPARAM wParam) {
    if (msg == WM_SYSCOMMAND && (wParam & 0xFFF0) == SC_MAXIMIZE) {
        UseSnappedRectAsNormal(hWnd);
    }
}

LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT msg, WPARAM wParam,
                                   LPARAM lParam) {
    OnMessage(hWnd, msg, wParam);
    return DefWindowProcW_Original(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI DefWindowProcA_Hook(HWND hWnd, UINT msg, WPARAM wParam,
                                   LPARAM lParam) {
    OnMessage(hWnd, msg, wParam);
    return DefWindowProcA_Original(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI DefDlgProcW_Hook(HWND hWnd, UINT msg, WPARAM wParam,
                                LPARAM lParam) {
    OnMessage(hWnd, msg, wParam);
    return DefDlgProcW_Original(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI DefDlgProcA_Hook(HWND hWnd, UINT msg, WPARAM wParam,
                                LPARAM lParam) {
    OnMessage(hWnd, msg, wParam);
    return DefDlgProcA_Original(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI DefFrameProcW_Hook(HWND hWnd, HWND hWndMDIClient, UINT msg,
                                  WPARAM wParam, LPARAM lParam) {
    OnMessage(hWnd, msg, wParam);
    return DefFrameProcW_Original(hWnd, hWndMDIClient, msg, wParam, lParam);
}

LRESULT WINAPI DefFrameProcA_Hook(HWND hWnd, HWND hWndMDIClient, UINT msg,
                                  WPARAM wParam, LPARAM lParam) {
    OnMessage(hWnd, msg, wParam);
    return DefFrameProcA_Original(hWnd, hWndMDIClient, msg, wParam, lParam);
}

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (nCmdShow == SW_MAXIMIZE) {
        UseSnappedRectAsNormal(hWnd);
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

BOOL WINAPI ShowWindowAsync_Hook(HWND hWnd, int nCmdShow) {
    if (nCmdShow == SW_MAXIMIZE &&
        GetWindowThreadProcessId(hWnd, nullptr) == GetCurrentThreadId()) {
        UseSnappedRectAsNormal(hWnd);
    }
    return ShowWindowAsync_Original(hWnd, nCmdShow);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    pIsWindowArranged = (IsWindowArranged_t)(void*)GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "IsWindowArranged");

    WindhawkUtils::SetFunctionHook(DefWindowProcW, DefWindowProcW_Hook,
                                   &DefWindowProcW_Original);
    WindhawkUtils::SetFunctionHook(DefWindowProcA, DefWindowProcA_Hook,
                                   &DefWindowProcA_Original);
    WindhawkUtils::SetFunctionHook(DefDlgProcW, DefDlgProcW_Hook,
                                   &DefDlgProcW_Original);
    WindhawkUtils::SetFunctionHook(DefDlgProcA, DefDlgProcA_Hook,
                                   &DefDlgProcA_Original);
    WindhawkUtils::SetFunctionHook(DefFrameProcW, DefFrameProcW_Hook,
                                   &DefFrameProcW_Original);
    WindhawkUtils::SetFunctionHook(DefFrameProcA, DefFrameProcA_Hook,
                                   &DefFrameProcA_Original);
    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook,
                                   &ShowWindow_Original);
    WindhawkUtils::SetFunctionHook(ShowWindowAsync, ShowWindowAsync_Hook,
                                   &ShowWindowAsync_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
