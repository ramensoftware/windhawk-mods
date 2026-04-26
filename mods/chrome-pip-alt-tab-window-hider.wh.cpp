// ==WindhawkMod==
// @id              chrome-pip-alt-tab-window-hider
// @name            Chrome PiP Alt+Tab Window Hider
// @description     Hides Chrome Picture-in-Picture windows from Alt+Tab and the taskbar.
// @version         1.0.0
// @author          BCRTVKCS
// @github          https://github.com/bcrtvkcs
// @twitter         https://x.com/bcrtvkcs
// @homepage        https://grdigital.pro
// @include         chrome.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Chrome PiP Alt+Tab Window Hider

Removes Chrome Picture-in-Picture windows from the Alt+Tab switcher and Win+Tab,
while keeping them fully functional (always-on-top, playback controls, etc.).

## How it works
Chrome PiP windows are identified by three concurrent traits:
- Window class: `Chrome_WidgetWin_1`
- Extended style: `WS_EX_TOPMOST` set
- No owner window
- Size smaller than 50% of the primary monitor in both dimensions

When a matching window is found, `WS_EX_TOOLWINDOW` is added and
`WS_EX_APPWINDOW` is removed — the standard Win32 technique for hiding
a window from Alt+Tab and the taskbar.

## Compatibility
- Windows 10 and Windows 11
- Targets `chrome.exe` only
*/
// ==/WindhawkModReadme==

#include <windows.h>

static BOOL g_bInitialized = FALSE;

static BOOL IsChromePiPWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd))
        return FALSE;

    wchar_t cls[64] = {};
    if (GetClassNameW(hwnd, cls, 64) == 0)
        return FALSE;
    if (wcscmp(cls, L"Chrome_WidgetWin_1") != 0)
        return FALSE;

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (!(exStyle & WS_EX_TOPMOST))
        return FALSE;

    if (GetWindow(hwnd, GW_OWNER) != NULL)
        return FALSE;

    RECT rc = {};
    if (!GetWindowRect(hwnd, &rc))
        return FALSE;
    int w = rc.right  - rc.left;
    int h = rc.bottom - rc.top;
    if (w < 80 || h < 45)
        return FALSE;
    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);
    if (w > sw / 2 || h > sh / 2)
        return FALSE;

    return TRUE;
}

static void HideFromAltTab(HWND hwnd) {
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    LONG_PTR patched = (ex & ~WS_EX_APPWINDOW) | WS_EX_TOOLWINDOW;
    if (patched != ex)
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, patched);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    if (g_bInitialized &&
        nWidth > 0 && nHeight > 0 &&
        (dwExStyle & WS_EX_TOPMOST) &&
        lpClassName && ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0 &&
        wcscmp(lpClassName, L"Chrome_WidgetWin_1") == 0)
    {
        dwExStyle &= ~WS_EX_APPWINDOW;
        dwExStyle |= WS_EX_TOOLWINDOW;
    }

    HWND hwnd = CreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName,
        dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam);

    if (hwnd && g_bInitialized && IsChromePiPWindow(hwnd))
        HideFromAltTab(hwnd);

    return hwnd;
}

using SetWindowLongPtrW_t = decltype(&SetWindowLongPtrW);
static SetWindowLongPtrW_t SetWindowLongPtrW_Original;

LONG_PTR WINAPI SetWindowLongPtrW_Hook(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
    if (g_bInitialized && nIndex == GWL_EXSTYLE) {
        wchar_t cls[64] = {};
        if (GetClassNameW(hWnd, cls, 64) > 0 &&
            wcscmp(cls, L"Chrome_WidgetWin_1") == 0 &&
            (dwNewLong & WS_EX_TOPMOST) &&
            GetWindow(hWnd, GW_OWNER) == NULL)
        {
            dwNewLong &= ~WS_EX_APPWINDOW;
            dwNewLong |= WS_EX_TOOLWINDOW;
        }
    }
    return SetWindowLongPtrW_Original(hWnd, nIndex, dwNewLong);
}

BOOL Wh_ModInit() {
    if (!Wh_SetFunctionHook((void*)CreateWindowExW,
                             (void*)CreateWindowExW_Hook,
                             (void**)&CreateWindowExW_Original))
        return FALSE;

    if (!Wh_SetFunctionHook((void*)SetWindowLongPtrW,
                             (void*)SetWindowLongPtrW_Hook,
                             (void**)&SetWindowLongPtrW_Original))
        return FALSE;

    g_bInitialized = TRUE;
    return TRUE;
}

void Wh_ModUninit() {}
