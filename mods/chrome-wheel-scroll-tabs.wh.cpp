// ==WindhawkMod==
// @id              chrome-wheel-scroll-tabs
// @name            Chrome/Edge scroll tabs with mouse wheel
// @description     Use the mouse wheel while hovering over the tab bar to switch between tabs
// @version         1.0.3
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         chrome.exe
// @include         msedge.exe
// @include         opera.exe
// @include         brave.exe
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Chrome/Edge scroll tabs with mouse wheel

Use the mouse wheel while hovering over the tab bar to switch between tabs.

Currently supported browsers: Google Chrome, Microsoft Edge, Opera, Brave.

![demonstration](https://i.imgur.com/GWCsO70.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- reverseScrollingDirection: false
  $name: Reverse Scrolling Direction
*/
// ==/WindhawkModSettings==

#include <commctrl.h>

struct {
    bool reverseScrollingDirection;
} g_settings;

DWORD g_uiThreadId;
DWORD g_lastScrollTime;
HWND g_lastScrollWnd;
short g_lastScrollDeltaRemainder;

// wParam - TRUE to subclass, FALSE to unsubclass
// lParam - subclass data
UINT g_subclassRegisteredMsg = RegisterWindowMessage(
    L"Windhawk_SetWindowSubclassFromAnyThread_chrome-wheel-scroll-tabs");

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

bool OnMouseWheel(HWND hWnd, short delta) {
    HWND hForegroundWnd = GetForegroundWindow();
    if (!hForegroundWnd || GetAncestor(hForegroundWnd, GA_ROOTOWNER) != hWnd) {
        g_lastScrollWnd = nullptr;
        return false;
    }

    if (GetKeyState(VK_CONTROL) < 0 || GetKeyState(VK_MENU) < 0 ||
        GetKeyState(VK_SHIFT) < 0 || GetKeyState(VK_PRIOR) < 0 ||
        GetKeyState(VK_NEXT) < 0) {
        return false;
    }

    if (hWnd == g_lastScrollWnd &&
        GetTickCount() - g_lastScrollTime < 1000 * 5) {
        delta += g_lastScrollDeltaRemainder;
    }

    int clicks = delta / WHEEL_DELTA;
    Wh_Log(L"%d clicks (delta=%d)", clicks, delta);

    if (g_settings.reverseScrollingDirection) {
        clicks = -clicks;
    }

    WORD key = VK_PRIOR;
    if (clicks < 0) {
        clicks = -clicks;
        key = VK_NEXT;
    }

    INPUT* input = new INPUT[clicks * 2 + 2];
    for (size_t i = 0; i < clicks * 2 + 2; i++) {
        input[i].type = INPUT_KEYBOARD;
        input[i].ki.wScan = 0;
        input[i].ki.time = 0;
        input[i].ki.dwExtraInfo = 0;
    }

    input[0].ki.wVk = VK_CONTROL;
    input[0].ki.dwFlags = 0;

    for (size_t i = 0; i < clicks; i++) {
        input[1 + i * 2].ki.wVk = key;
        input[1 + i * 2].ki.dwFlags = 0;
        input[1 + i * 2 + 1].ki.wVk = key;
        input[1 + i * 2 + 1].ki.dwFlags = KEYEVENTF_KEYUP;
    }

    input[1 + clicks * 2].ki.wVk = VK_CONTROL;
    input[1 + clicks * 2].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(clicks * 2 + 2, input, sizeof(input[0]));

    delete[] input;

    g_lastScrollTime = GetTickCount();
    g_lastScrollWnd = hWnd;
    g_lastScrollDeltaRemainder = delta % WHEEL_DELTA;

    return true;
}

LRESULT CALLBACK BrowserWindowSubclassProc(_In_ HWND hWnd,
                                           _In_ UINT uMsg,
                                           _In_ WPARAM wParam,
                                           _In_ LPARAM lParam,
                                           _In_ UINT_PTR uIdSubclass,
                                           _In_ DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_MOUSEWHEEL:
            switch (SendMessage(hWnd, WM_NCHITTEST, 0, lParam)) {
                case HTCLIENT:
                case HTCAPTION:
                case HTSYSMENU:
                case HTMINBUTTON:
                case HTMAXBUTTON:
                case HTCLOSE:
                    if (OnMouseWheel(hWnd, GET_WHEEL_DELTA_WPARAM(wParam))) {
                        return 0;
                    }
                    break;
            }
            break;

        default:
            if (uMsg == g_subclassRegisteredMsg && !wParam)
                RemoveWindowSubclass(hWnd, BrowserWindowSubclassProc, 0);
            break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Best effort determination as of whether the window is a browser window.
bool IsBrowserWindow(HWND hWnd) {
    if (!(GetWindowLong(hWnd, GWL_STYLE) & WS_SYSMENU)) {
        return false;
    }

    WCHAR szClassName[32];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0 ||
        wcsicmp(szClassName, L"Chrome_WidgetWin_1") != 0) {
        return false;
    }

    return true;
}

BOOL CALLBACK InitialEnumBrowserWindowsFunc(HWND hWnd, LPARAM lParam) {
    DWORD dwProcessId = 0;
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
    if (!dwThreadId || dwProcessId != GetCurrentProcessId()) {
        return TRUE;
    }

    if (g_uiThreadId && g_uiThreadId != dwThreadId) {
        return TRUE;
    }

    if (IsBrowserWindow(hWnd)) {
        Wh_Log(L"Browser window found: %08X", (DWORD)(ULONG_PTR)hWnd);

        if (!g_uiThreadId) {
            g_uiThreadId = dwThreadId;
        }

        SetWindowSubclassFromAnyThread(hWnd, BrowserWindowSubclassProc, 0, 0);
    }

    return TRUE;
}

BOOL CALLBACK EnumBrowserWindowsUnsubclassFunc(HWND hWnd, LPARAM lParam) {
    if (IsBrowserWindow(hWnd)) {
        Wh_Log(L"Browser window to unsubclass: %08X", (DWORD)(ULONG_PTR)hWnd);

        SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0);
    }

    return TRUE;
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

    if (g_uiThreadId && g_uiThreadId != GetCurrentThreadId()) {
        return hWnd;
    }

    if (IsBrowserWindow(hWnd)) {
        Wh_Log(L"Browser window created: %08X", (DWORD)(ULONG_PTR)hWnd);

        if (!g_uiThreadId) {
            g_uiThreadId = GetCurrentThreadId();
        }

        SetWindowSubclass(hWnd, BrowserWindowSubclassProc, 0, 0);
    }

    return hWnd;
}

void LoadSettings() {
    g_settings.reverseScrollingDirection =
        Wh_GetIntSetting(L"reverseScrollingDirection");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExWHook,
                       (void**)&pOriginalCreateWindowExW);

    EnumWindows(InitialEnumBrowserWindowsFunc, 0);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_uiThreadId != 0) {
        EnumThreadWindows(g_uiThreadId, EnumBrowserWindowsUnsubclassFunc, 0);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
