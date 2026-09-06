// ==WindhawkMod==
// @id              taskbar-disabler
// @name            Taskbar Disabler (for YASB / custom bars)
// @description     Fully hides the Windows taskbar
// @version         1.0.0
// @author          craciu25yt
// @github          https://github.com/craciu25yt
// @twitter         https://twitter.com/craciu25_yt
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Disabler

Completely disables the Windows taskbar in all monitors. Useful when using a custom takbar such as YASB
*/
// ==/WindhawkModReadme==


#include <windhawk_api.h>
#include <windows.h>



static bool IsTaskbarWindow(HWND hWnd) {
    WCHAR cls[64] = {};
    GetClassNameW(hWnd, cls, ARRAYSIZE(cls));
    return (wcscmp(cls, L"Shell_TrayWnd") == 0 ||
            wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0);
}

static void HideAllTaskbars() {
    // Primary taskbar
    HWND hPrimary = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hPrimary) {
        ShowWindow(hPrimary, SW_HIDE);
        Wh_Log(L"Hidden primary taskbar HWND=%p", hPrimary);
    }

    // Secondary taskbars (multi-monitor)
    HWND hSec = nullptr;
    while ((hSec = FindWindowExW(nullptr, hSec,
                                  L"Shell_SecondaryTrayWnd", nullptr)) != nullptr) {
        ShowWindow(hSec, SW_HIDE);
        Wh_Log(L"Hidden secondary taskbar HWND=%p", hSec);
    }
}

static void ShowAllTaskbars() {
    HWND hPrimary = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hPrimary) {
        ShowWindow(hPrimary, SW_SHOW);
        Wh_Log(L"Restored primary taskbar HWND=%p", hPrimary);
    }

    HWND hSec = nullptr;
    while ((hSec = FindWindowExW(nullptr, hSec,
                                  L"Shell_SecondaryTrayWnd", nullptr)) != nullptr) {
        ShowWindow(hSec, SW_SHOW);
        Wh_Log(L"Restored secondary taskbar HWND=%p", hSec);
    }
}

using ShowWindow_t = BOOL(WINAPI*)(HWND hWnd, int nCmdShow);
ShowWindow_t pOrigShowWindow = nullptr;

BOOL WINAPI ShowWindowHook(HWND hWnd, int nCmdShow) {
    if (IsTaskbarWindow(hWnd) && nCmdShow != SW_HIDE) {
        // Intercept any attempt to show the taskbar, basically explorer.exe restart
        Wh_Log(L"Blocked ShowWindow(%p, %d) on taskbar", hWnd, nCmdShow);
        return TRUE;
    }
    return pOrigShowWindow(hWnd, nCmdShow);
}


using SetWindowPos_t = BOOL(WINAPI*)(HWND, HWND, int, int, int, int, UINT);
SetWindowPos_t pOrigSetWindowPos = nullptr;

BOOL WINAPI SetWindowPosHook(HWND hWnd, HWND hWndInsertAfter,
                              int X, int Y, int cx, int cy, UINT uFlags) {
    if (IsTaskbarWindow(hWnd)) {
        uFlags |=  SWP_HIDEWINDOW;
        uFlags &= ~SWP_SHOWWINDOW;
    }
    return pOrigSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}


BOOL Wh_ModInit() {
    Wh_Log(L"Loading taskbar disabler...");

    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(ShowWindow),
            reinterpret_cast<void*>(ShowWindowHook),
            reinterpret_cast<void**>(&pOrigShowWindow))) {
        Wh_Log(L"Failed to hook ShowWindow");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(SetWindowPos),
            reinterpret_cast<void*>(SetWindowPosHook),
            reinterpret_cast<void**>(&pOrigSetWindowPos))) {
        Wh_Log(L"Failed to hook SetWindowPos");
        return FALSE;
    }

    HideAllTaskbars();


    Wh_Log(L"Taskbar disabled");
    return TRUE;
}

void Wh_ModUninit() {
    ShowAllTaskbars();
    Wh_Log(L"Taskbar re-enabled");
}
