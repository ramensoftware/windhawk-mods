// ==WindhawkMod==
// @id              chrome-wheel-scroll-tabs
// @name            Chrome/Edge scroll tabs with mouse wheel
// @description     Use the mouse wheel while hovering over the tab bar to switch between tabs
// @version         1.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         chrome.exe
// @include         msedge.exe
// @include         opera.exe
// @include         brave.exe
// @compilerOptions -lcomctl32 -lgdi32
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
- horizontalScrolling: false
  $name: Horizontal scrolling
  $description: >-
    Switch between tabs when the mouse's horizontal scroll wheel is tilted or
    rotated
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <windowsx.h>

struct {
    bool reverseScrollingDirection;
    bool horizontalScrolling;
} g_settings;

bool g_isEdge;
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

UINT GetDpiForWindowWithFallback(HWND hWnd) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND hwnd);
    static GetDpiForWindow_t pGetDpiForWindow = []() {
        HMODULE hUser32 = GetModuleHandle(L"user32.dll");
        if (hUser32) {
            return (GetDpiForWindow_t)GetProcAddress(hUser32,
                                                     "GetDpiForWindow");
        }

        return (GetDpiForWindow_t) nullptr;
    }();

    UINT dpi = 96;
    if (pGetDpiForWindow) {
        dpi = pGetDpiForWindow(hWnd);
    } else {
        HDC hdc = GetDC(nullptr);
        if (hdc) {
            dpi = GetDeviceCaps(hdc, LOGPIXELSX);
            ReleaseDC(nullptr, hdc);
        }
    }

    return dpi;
}

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
    SendMessage(hWnd, g_subclassRegisteredMsg, TRUE, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return param.result;
}

bool OnMouseWheel(HWND hWnd, WORD keys, short delta, int xPos, int yPos) {
    if (keys) {
        return false;
    }

    switch (SendMessage(hWnd, WM_NCHITTEST, 0, MAKELPARAM(xPos, yPos))) {
        case HTCLIENT:
        case HTCAPTION:
        case HTSYSMENU:
        case HTMINBUTTON:
        case HTMAXBUTTON:
        case HTCLOSE:
            break;

        default:
            return false;
    }

    HWND hForegroundWnd = GetForegroundWindow();
    if (!hForegroundWnd || GetAncestor(hForegroundWnd, GA_ROOTOWNER) != hWnd) {
        g_lastScrollWnd = nullptr;
        return false;
    }

    if (GetKeyState(VK_MENU) < 0 || GetKeyState(VK_LWIN) < 0 ||
        GetKeyState(VK_RWIN) < 0 || GetKeyState(VK_PRIOR) < 0 ||
        GetKeyState(VK_NEXT) < 0) {
        return false;
    }

    // Edge has a thin border around the web content which triggers the tab
    // scrolling. Ignore events which are very close to the window borders.
    if (g_isEdge) {
        UINT dpi = GetDpiForWindowWithFallback(hWnd);
        RECT rect{};
        GetWindowRect(hWnd, &rect);
        if (MulDiv(xPos - rect.left, 96, dpi) <= 15 ||
            MulDiv(rect.right - xPos, 96, dpi) <= 15 ||
            MulDiv(rect.bottom - yPos, 96, dpi) <= 15) {
            return false;
        }
    }

    if (hWnd == g_lastScrollWnd &&
        GetTickCount() - g_lastScrollTime < 1000 * 5) {
        delta += g_lastScrollDeltaRemainder;
    }

    int clicks = delta / WHEEL_DELTA;
    Wh_Log(L"%d clicks (delta=%d)", clicks, delta);

    WORD key = VK_NEXT;
    if (clicks < 0) {
        clicks = -clicks;
        key = VK_PRIOR;
    }

    INPUT* input = new INPUT[clicks * 2 + 2];
    for (int i = 0; i < clicks * 2 + 2; i++) {
        input[i].type = INPUT_KEYBOARD;
        input[i].ki.wScan = 0;
        input[i].ki.time = 0;
        input[i].ki.dwExtraInfo = 0;
    }

    input[0].ki.wVk = VK_CONTROL;
    input[0].ki.dwFlags = 0;

    for (int i = 0; i < clicks; i++) {
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
    if (uMsg == WM_NCDESTROY || (uMsg == g_subclassRegisteredMsg && !wParam)) {
        RemoveWindowSubclass(hWnd, BrowserWindowSubclassProc, 0);
    }

    switch (uMsg) {
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL: {
            WORD fwKeys = GET_KEYSTATE_WPARAM(wParam);
            short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            if (uMsg == WM_MOUSEHWHEEL) {
                if (!g_settings.horizontalScrolling) {
                    break;
                }

                // For horizontal scrolling, a large delta value might be posted
                // for a single click (e.g. 480). Limit the value to 120.
                if (zDelta < -120) {
                    zDelta = -120;
                } else if (zDelta > 120) {
                    zDelta = 120;
                }
            } else if (!g_settings.reverseScrollingDirection) {
                zDelta = -zDelta;
            }

            if (OnMouseWheel(hWnd, fwKeys, zDelta, xPos, yPos)) {
                return 0;
            }
            break;
        }
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
    g_settings.horizontalScrolling = Wh_GetIntSetting(L"horizontalScrolling");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_isEdge = !!GetModuleHandle(L"msedge.exe");

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
