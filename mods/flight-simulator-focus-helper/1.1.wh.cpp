// ==WindhawkMod==
// @id              flight-simulator-focus-helper
// @name            Flight Simulator window focus helper
// @description     Makes the game window active on mouse hover and inactive on mouse leave
// @version         1.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         FlightSimulator.exe
// @compilerOptions -lcomctl32
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
# Flight Simulator window focus helper

Makes the game window active on mouse hover and inactive on mouse leave.

The Microsoft Flight Simulator game has non-standard mouse scrolling handling -
the scrolling affects the game depending on whether the window is active or not,
unlike other apps for which mouse scrolling works when the mouse is hovered over
the window. This mod helps with this quirk by making the game window inactive
when the mouse leaves it, so mouse scrolling no longer affects it, and by making
the game window active when the mouse hovers over it, making mouse scrolling
work again.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- bringToFrontOnMouseClick: true
  $name: Bring to front on mouse click
*/
// ==/WindhawkModSettings==

#include <commctrl.h>

struct {
    bool bringToFrontOnMouseClick;
} g_settings;

HWND g_gameWnd;
bool g_settingForegroundWindow = false;

// wParam - TRUE to subclass, FALSE to unsubclass
// lParam - subclass data
UINT g_subclassRegisteredMsg = RegisterWindowMessage(
    L"Windhawk_SetWindowSubclassFromAnyThread_flight-simulator-wheel-fix");

struct SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM {
    SUBCLASSPROC pfnSubclass;
    UINT_PTR uIdSubclass;
    DWORD_PTR dwRefData;
    BOOL result;
};

LRESULT CALLBACK CallWndProcForWindowSubclass(int nCode,
                                              WPARAM wParam,
                                              LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == g_subclassRegisteredMsg && cwp->wParam) {
            SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM* param =
                (SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM*)cwp->lParam;
            param->result =
                SetWindowSubclass(cwp->hwnd, param->pfnSubclass,
                                  param->uIdSubclass, param->dwRefData);
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

BOOL SetWindowSubclassFromAnyThread(HWND hWnd,
                                    SUBCLASSPROC pfnSubclass,
                                    UINT_PTR uIdSubclass,
                                    DWORD_PTR dwRefData) {
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return FALSE;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        return SetWindowSubclass(hWnd, pfnSubclass, uIdSubclass, dwRefData);
    }

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProcForWindowSubclass,
                                  nullptr, dwThreadId);
    if (!hook) {
        return FALSE;
    }

    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM param;
    param.pfnSubclass = pfnSubclass;
    param.uIdSubclass = uIdSubclass;
    param.dwRefData = dwRefData;
    param.result = FALSE;
    SendMessage(hWnd, g_subclassRegisteredMsg, TRUE, (WPARAM)&param);

    UnhookWindowsHookEx(hook);

    return param.result;
}

HWND GetTaskbarWindow() {
    return FindWindow(L"Shell_TrayWnd", nullptr);
}

LRESULT CALLBACK GameWindowSubclassProc(_In_ HWND hWnd,
                                        _In_ UINT uMsg,
                                        _In_ WPARAM wParam,
                                        _In_ LPARAM lParam,
                                        _In_ UINT_PTR uIdSubclass,
                                        _In_ DWORD_PTR dwRefData) {
    if (uMsg == WM_NCDESTROY || (uMsg == g_subclassRegisteredMsg && !wParam)) {
        RemoveWindowSubclass(hWnd, GameWindowSubclassProc, 0);
    }

    switch (uMsg) {
        case WM_MOUSEMOVE:
            if (GetForegroundWindow() != hWnd) {
                // Allows to steal focus.
                INPUT input{};
                SendInput(1, &input, sizeof(INPUT));

                g_settingForegroundWindow = true;
                SetForegroundWindow(hWnd);
                g_settingForegroundWindow = false;
            }
            break;

        case WM_MOUSELEAVE:
            if (GetForegroundWindow() == hWnd) {
                if (HWND hTaskbarWnd = GetTaskbarWindow()) {
                    SetForegroundWindow(hTaskbarWnd);
                }
            }
            break;

        case WM_WINDOWPOSCHANGING:
            if (g_settingForegroundWindow) {
                auto pwpos = (WINDOWPOS*)lParam;
                pwpos->flags |= SWP_NOZORDER;
            }
            break;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            // Bring to top on mouse click, as it might be the foreground window
            // but not on top due to the trick in WM_WINDOWPOSCHANGING.
            if (g_settings.bringToFrontOnMouseClick) {
                SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0,
                             SWP_NOSIZE | SWP_NOMOVE);
            }
            break;

        case WM_NCDESTROY:
            g_gameWnd = nullptr;
            break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

bool IsGameWindow(HWND hWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) ||
        _wcsicmp(szClassName, L"AceApp") != 0) {
        return false;
    }

    return true;
}

BOOL CALLBACK InitialEnumGameWindowsFunc(HWND hWnd, LPARAM lParam) {
    DWORD dwProcessId = 0;
    if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
        dwProcessId != GetCurrentProcessId()) {
        return TRUE;
    }

    if (!IsGameWindow(hWnd)) {
        return TRUE;
    }

    Wh_Log(L"Game window found: %08X", (DWORD)(ULONG_PTR)hWnd);

    g_gameWnd = hWnd;
    SetWindowSubclassFromAnyThread(hWnd, GameWindowSubclassProc, 0, 0);

    return FALSE;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t pOriginalCreateWindowExW;
HWND WINAPI CreateWindowExWHook(DWORD dwExStyle,
                                LPCWSTR lpClassName,
                                LPCWSTR lpWindowName,
                                DWORD dwStyle,
                                int X,
                                int Y,
                                int nWidth,
                                int nHeight,
                                HWND hWndParent,
                                HMENU hMenu,
                                HINSTANCE hInstance,
                                LPVOID lpParam) {
    HWND hWnd = pOriginalCreateWindowExW(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd) {
        return hWnd;
    }

    if (!g_gameWnd && IsGameWindow(hWnd)) {
        Wh_Log(L"Game window created: %08X", (DWORD)(ULONG_PTR)hWnd);

        g_gameWnd = hWnd;
        SetWindowSubclass(hWnd, GameWindowSubclassProc, 0, 0);
    }

    return hWnd;
}

void LoadSettings() {
    g_settings.bringToFrontOnMouseClick =
        Wh_GetIntSetting(L"bringToFrontOnMouseClick");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExWHook,
                       (void**)&pOriginalCreateWindowExW);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    EnumWindows(InitialEnumGameWindowsFunc, 0);
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_gameWnd) {
        SendMessage(g_gameWnd, g_subclassRegisteredMsg, FALSE, 0);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
