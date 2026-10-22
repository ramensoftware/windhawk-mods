// ==WindhawkMod==
// @id              chrome-middle-click-new-tab
// @name            Middle-click empty tab bar to open a new tab (Chrome/Edge/Brave)
// @description     Middle-click the empty area of the tab bar to open a new tab
// @version         1.0
// @author          erenalpars
// @github          https://github.com/erenalpars
// @include         chrome.exe
// @include         msedge.exe
// @include         brave.exe
// @compilerOptions -lcomctl32 -lgdi32 -lole32 -lshell32
// @license         GPL-3.0
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Based on "Chrome/Edge scroll tabs with mouse wheel" by m417z, published under
// The GNU General Public License v3.0:
// https://github.com/m417z/my-windhawk-mods/blob/main/mods/chrome-wheel-scroll-tabs.wh.cpp
// The browser window detection and enumeration, DPI helper and CreateWindowExW
// hook code are taken from that mod.

// ==WindhawkModReadme==
/*
# Middle-click empty tab bar to open a new tab (Chrome/Edge/Brave)

Middle-click the empty area of the tab bar (between the "+" button and the
minimize/maximize/close buttons) to open a new tab, just like clicking "+".

![demonstration](https://i.imgur.com/KBVoOg9.gif)

Currently supported browsers: Google Chrome, Microsoft Edge, Brave. Other
Chromium-based browsers may work too: add their executable in the mod's
Advanced settings (custom process inclusion list).

The empty tab bar area is detected using the window's hit-testing: Chrome
reports it as a caption (draggable) area, while tabs and buttons are reported
as client area. By default, only caption areas that have a tab or a button
between them and the side away from the window buttons are used, so the thin
drag strip above the tabs and the area before the first tab are ignored. This
also works with right-to-left UI languages, where the tab bar is mirrored.

Installed web apps (PWAs) are ignored, since they have a title bar instead of a
tab bar.

The new tab is opened with the browser's "new" app command (the same command as
Ctrl+T), so no keyboard input is simulated.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- onlyNextToTabs: true
  $name: Only next to the tabs
  $description: >-
    Only react to the empty area next to the tabs (e.g. between the "+" button
    and the window buttons). When disabled, any empty part of the title bar
    works, including the thin strip above the tabs and the title bar of windows
    without tabs, such as popups and DevTools; there, a new tab opens in a
    regular browser window.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <commctrl.h>
#include <propsys.h>
#include <shobjidl.h>
#include <windowsx.h>

struct {
    bool onlyNextToTabs;
} g_settings;

DWORD g_uiThreadId;
HWND g_middleButtonDownWnd;

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

// Installed web apps (PWAs) use the same window class as browser windows, but
// their AppUserModelID looks like "Chrome._crx_<app id>".
bool IsWebAppWindow(HWND hWnd) {
    // PKEY_AppUserModel_ID, defined here to avoid linking with uuid.lib.
    constexpr PROPERTYKEY kAppUserModelIdKey = {
        {0x9F4C2855,
         0x9F79,
         0x4B39,
         {0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3}},
        5};

    IPropertyStore* propertyStore = nullptr;
    if (FAILED(SHGetPropertyStoreForWindow(hWnd, IID_PPV_ARGS(&propertyStore)))) {
        return false;
    }

    bool isWebApp = false;

    PROPVARIANT value;
    PropVariantInit(&value);
    if (SUCCEEDED(propertyStore->GetValue(kAppUserModelIdKey, &value)) &&
        value.vt == VT_LPWSTR && value.pwszVal) {
        isWebApp = wcsstr(value.pwszVal, L"_crx_") != nullptr;
    }

    PropVariantClear(&value);
    propertyStore->Release();

    return isWebApp;
}

// With a right-to-left UI language, the tab strip is mirrored and the window
// buttons are on the left. Chrome doesn't set WS_EX_LAYOUTRTL on the window, so
// check which top corner has the close button.
bool AreWindowButtonsOnLeft(HWND hWnd, const RECT& rect, UINT dpi) {
    int x = rect.left + MulDiv(24, dpi, 96);
    int y = rect.top + MulDiv(16, dpi, 96);
    return SendMessage(hWnd, WM_NCHITTEST, 0, MAKELPARAM(x, y)) == HTCLOSE;
}

// Checks whether there's a client area element (a tab or the "+" button) on the
// same row, on the side away from the window buttons. This is true for the
// empty area after the tabs, but not for the drag strip above the tabs or the
// area before the first tab.
bool IsNextToTabs(HWND hWnd, int xPos, int yPos) {
    RECT rect{};
    GetWindowRect(hWnd, &rect);

    UINT dpi = GetDpiForWindowWithFallback(hWnd);

    int step = MulDiv(2, dpi, 96);
    if (step < 1) {
        step = 1;
    }

    if (AreWindowButtonsOnLeft(hWnd, rect, dpi)) {
        Wh_Log(L"Window buttons are on the left, scanning to the right");
    } else {
        step = -step;
    }

    for (int x = xPos + step; x >= rect.left && x < rect.right; x += step) {
        LRESULT hitTest =
            SendMessage(hWnd, WM_NCHITTEST, 0, MAKELPARAM(x, yPos));
        if (hitTest == HTCLIENT) {
            return true;
        }

        if (hitTest != HTCAPTION) {
            return false;
        }
    }

    return false;
}

bool IsEmptyTabBarArea(HWND hWnd, int xPos, int yPos) {
    if (IsWebAppWindow(hWnd)) {
        Wh_Log(L"Web app window, ignoring");
        return false;
    }

    if (g_settings.onlyNextToTabs && !IsNextToTabs(hWnd, xPos, yPos)) {
        Wh_Log(L"Not next to the tabs, ignoring");
        return false;
    }

    return true;
}

void OpenNewTab(HWND hWnd) {
    Wh_Log(L"Opening a new tab in window %08X", (DWORD)(ULONG_PTR)hWnd);

    // Chrome maps APPCOMMAND_NEW to IDC_NEW_TAB (same as Ctrl+T).
    PostMessage(hWnd, WM_APPCOMMAND, (WPARAM)hWnd,
                MAKELPARAM(0, FAPPCOMMAND_MOUSE | APPCOMMAND_NEW));
}

LRESULT CALLBACK BrowserWindowSubclassProc(HWND hWnd,
                                           UINT uMsg,
                                           WPARAM wParam,
                                           LPARAM lParam,
                                           DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_NCMBUTTONDOWN:
        // A quick second click arrives as a double click, treat it as another
        // click.
        case WM_NCMBUTTONDBLCLK:
            g_middleButtonDownWnd = nullptr;
            if (wParam == HTCAPTION &&
                IsEmptyTabBarArea(hWnd, GET_X_LPARAM(lParam),
                                  GET_Y_LPARAM(lParam))) {
                g_middleButtonDownWnd = hWnd;
                return 0;
            }
            break;

        // A press in the client area means that a release on the tab bar
        // doesn't complete a click that started there.
        case WM_MBUTTONDOWN:
            g_middleButtonDownWnd = nullptr;
            break;

        case WM_NCMBUTTONUP:
            if (g_middleButtonDownWnd == hWnd) {
                g_middleButtonDownWnd = nullptr;
                // Only open a tab if the button was released in the same
                // area, like a regular button click.
                if (wParam == HTCAPTION &&
                    IsEmptyTabBarArea(hWnd, GET_X_LPARAM(lParam),
                                      GET_Y_LPARAM(lParam))) {
                    OpenNewTab(hWnd);
                }
                return 0;
            }
            break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Best effort determination as of whether the window is a browser window.
bool IsBrowserWindow(HWND hWnd) {
    if (!(GetWindowLong(hWnd, GWL_STYLE) & WS_SYSMENU)) {
        return false;
    }

    WCHAR windowClassName[256];
    if (!GetClassName(hWnd, windowClassName, ARRAYSIZE(windowClassName))) {
        return false;
    }

    bool classNameMatch = false;
    PCWSTR classNames[] = {
        L"Chrome_WidgetWin_1",
        L"YandexBrowser_WidgetWin_1",
    };
    for (PCWSTR className : classNames) {
        if (_wcsicmp(windowClassName, className) == 0) {
            classNameMatch = true;
            break;
        }
    }

    return classNameMatch;
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

        WindhawkUtils::SetWindowSubclassFromAnyThread(
            hWnd, BrowserWindowSubclassProc, 0);
    }

    return TRUE;
}

BOOL CALLBACK EnumBrowserWindowsUnsubclassFunc(HWND hWnd, LPARAM lParam) {
    if (IsBrowserWindow(hWnd)) {
        Wh_Log(L"Browser window to unsubclass: %08X", (DWORD)(ULONG_PTR)hWnd);

        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hWnd, BrowserWindowSubclassProc);
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

        WindhawkUtils::SetWindowSubclassFromAnyThread(
            hWnd, BrowserWindowSubclassProc, 0);
    }

    return hWnd;
}

// Renderer, GPU and other child processes have a "--type=" switch and own no
// browser windows.
bool IsBrowserProcess() {
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return false;
    }

    bool isBrowserProcess = true;
    for (int i = 1; i < argc; i++) {
        if (wcsncmp(argv[i], L"--type=", sizeof("--type=") - 1) == 0) {
            isBrowserProcess = false;
            break;
        }
    }

    LocalFree(argv);

    return isBrowserProcess;
}

void LoadSettings() {
    g_settings.onlyNextToTabs = Wh_GetIntSetting(L"onlyNextToTabs");
}

BOOL Wh_ModInit() {
    if (!IsBrowserProcess()) {
        return FALSE;
    }

    Wh_Log(L">");

    LoadSettings();

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExWHook,
                                   &pOriginalCreateWindowExW);

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
