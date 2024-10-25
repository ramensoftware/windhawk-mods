// ==WindhawkMod==
// @id              taskbar-auto-hide-when-maximized
// @name            Taskbar auto-hide when maximized
// @description     Automatically hides the taskbar when a window is maximized, and brings it back when there are no maximized windows
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Taskbar auto-hide when maximized

Automatically hides the taskbar when a window is maximized, and brings it back
when there are no maximized windows.
*/
// ==/WindhawkModReadme==

#include <optional>

#include <dwmapi.h>

HANDLE g_winObjectLocationChangeThread;
UINT_PTR g_pendingLocationChangeTimer;
std::optional<bool> g_wasAutoHideSet;

// https://devblogs.microsoft.com/oldnewthing/20200302-00/?p=103507
bool IsWindowCloaked(HWND hwnd) {
    BOOL isCloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &isCloaked,
                                           sizeof(isCloaked))) &&
           isCloaked;
}

bool SetTaskbarAutoHide(bool set) {
    APPBARDATA abd;

    // Both ABM_GETSTATE and ABM_SETSTATE require cbSize to be set.
    abd.cbSize = sizeof(APPBARDATA);
    // Get state.
    UINT state = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
    // Set auto hide state.
    if (set) {
        abd.lParam = state | ABS_AUTOHIDE;
    } else {
        abd.lParam = state & ~ABS_AUTOHIDE;
    }
    // Set state.
    SHAppBarMessage(ABM_SETSTATE, &abd);
    return state & ABS_AUTOHIDE;
}

bool DoesAnyMonitorHaveMaximizedWindow() {
    bool hasMaximized = false;

    auto enumWindowsProc = [&hasMaximized](HWND hWnd) -> BOOL {
        if (!IsWindowVisible(hWnd) || IsWindowCloaked(hWnd) || IsIconic(hWnd)) {
            return TRUE;
        }

        if (GetWindowLong(hWnd, GWL_EXSTYLE) &
            (WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW)) {
            return TRUE;
        }

        WINDOWPLACEMENT wp{
            .length = sizeof(WINDOWPLACEMENT),
        };
        if (!GetWindowPlacement(hWnd, &wp) || wp.showCmd != SW_SHOWMAXIMIZED) {
            return TRUE;
        }

        hasMaximized = true;
        return FALSE;
    };

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            auto& proc = *reinterpret_cast<decltype(enumWindowsProc)*>(lParam);
            return proc(hWnd);
        },
        reinterpret_cast<LPARAM>(&enumWindowsProc));

    return hasMaximized;
}

void AdjustTaskbarStyle() {
    bool wasAutoHideSet =
        SetTaskbarAutoHide(DoesAnyMonitorHaveMaximizedWindow());
    if (!g_wasAutoHideSet) {
        g_wasAutoHideSet = wasAutoHideSet;
    }
}

void ResetTaskbarStyle() {
    if (g_wasAutoHideSet) {
        SetTaskbarAutoHide(*g_wasAutoHideSet);
    }
}

void CALLBACK LocationChangeWinEventProc(HWINEVENTHOOK hWinEventHook,
                                         DWORD event,
                                         HWND hWnd,
                                         LONG idObject,
                                         LONG idChild,
                                         DWORD dwEventThread,
                                         DWORD dwmsEventTime) {
    if (idObject != OBJID_WINDOW) {
        return;
    }

    Wh_Log(L">");

    if (g_pendingLocationChangeTimer) {
        return;
    }

    g_pendingLocationChangeTimer =
        SetTimer(nullptr, 0, 200,
                 [](HWND hwnd,         // handle of window for timer messages
                    UINT uMsg,         // WM_TIMER message
                    UINT_PTR idEvent,  // timer identifier
                    DWORD dwTime       // current system time
                    ) WINAPI {
                     Wh_Log(L">");

                     KillTimer(nullptr, g_pendingLocationChangeTimer);
                     g_pendingLocationChangeTimer = 0;

                     AdjustTaskbarStyle();
                 });
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    return TRUE;
}

void Wh_ModAfterInit() {
    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(NULL), L"Shell_TrayWnd", &wndclass)) {
        AdjustTaskbarStyle();
    }

    g_winObjectLocationChangeThread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParameter) WINAPI -> DWORD {
            HWINEVENTHOOK winObjectLocationChangeEventHook = SetWinEventHook(
                EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
                nullptr, LocationChangeWinEventProc, 0, 0,
                WINEVENT_OUTOFCONTEXT);
            if (!winObjectLocationChangeEventHook) {
                Wh_Log(L"Error: SetWinEventHook");
                return 0;
            }

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

            UnhookWinEvent(winObjectLocationChangeEventHook);
            return 0;
        },
        nullptr, 0, nullptr);
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_winObjectLocationChangeThread) {
        PostThreadMessage(GetThreadId(g_winObjectLocationChangeThread), WM_APP,
                          0, 0);
        WaitForSingleObject(g_winObjectLocationChangeThread, INFINITE);
        CloseHandle(g_winObjectLocationChangeThread);
    }

    ResetTaskbarStyle();
}
