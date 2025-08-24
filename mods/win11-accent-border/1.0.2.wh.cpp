// ==WindhawkMod==
// @id              win11-accent-border
// @name            Windows 11 Accent Window Border
// @description     Show the accent color on the border but not on the titlebar
// @version         1.0.2
// @author          Guerra24
// @github          https://github.com/Guerra24
// @include         *
// @exclude         devenv.exe
// @compilerOptions -ldwmapi -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Mimics the behavior the *Show accent color on title bars and window borders* setting had in Windows 11 Build 22000.

When it was enabled in build 22000 the accent color appeared behind the Mica effect but in 22621 and newer it appears on top and it doesn't look that great. This mod restores the former behavior.

**Make sure *Show accent color on title bars and window borders* is disabled as it conflicts with this mod!**

Before:
![image](https://i.imgur.com/LnPyxkb.png)

After:
![image](https://i.imgur.com/TpGSX6X.png)

# Colors

The focused color is always the accent color.

You can use `AccentColorInactive` in `HKEY_CURRENT_USER\Software\Microsoft\Windows\DWM` to change the inactive color. This is a HEX value in BGR format.
*/
// ==/WindhawkModReadme==

#include <dwmapi.h>
#include <windhawk_api.h>

COLORREF BorderActive;
COLORREF BorderInactive = 0x000000;
const COLORREF ColorDefault = DWMWA_COLOR_DEFAULT;

void LoadColors() {
    DWORD color;
    DWORD colorSize = sizeof(color);
    RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent", L"AccentColorMenu", RRF_RT_REG_DWORD, NULL, &color, &colorSize);

    BorderActive = (color & 0xFF0000) | (color & 0xFF00) | (color & 0xFF);

    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM", L"AccentColorInactive", RRF_RT_REG_DWORD, NULL, &color, &colorSize) == ERROR_SUCCESS) {
        BorderInactive = (color & 0xFF0000) | (color & 0xFF00) | (color & 0xFF);
    }
}

void SetBorderColor(HWND hWnd, BOOL activate)
{
    DWORD dwStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
    //Better exclude context menus
    if ((dwStyle & WS_THICKFRAME) != WS_THICKFRAME && (dwStyle & WS_CAPTION) != WS_CAPTION)
    {
        return;
    }
    Wh_Log(L"Activate: %d", activate);
    if (activate)
    {
        DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR, &BorderActive, sizeof(BorderActive));
    }
    else
    {
        DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR, &BorderInactive, sizeof(BorderInactive));
    }
}

typedef LRESULT (WINAPI *DefWindowProcA_t)(HWND, UINT, WPARAM, LPARAM);
DefWindowProcA_t DefWindowProcA_orig;
LRESULT WINAPI DefWindowProcA_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefWindowProcA_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_ACTIVATE:
        case WM_NCACTIVATE:
            SetBorderColor(hWnd, wParam);
        break;
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            LoadColors();
            SetBorderColor(hWnd, GetForegroundWindow() == hWnd);
        break;
    }

    return result;
}

typedef LRESULT (WINAPI *DefWindowProcW_t)(HWND, UINT, WPARAM, LPARAM);
DefWindowProcW_t DefWindowProcW_orig;
LRESULT WINAPI DefWindowProcW_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefWindowProcW_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_ACTIVATE:
        case WM_NCACTIVATE:
            SetBorderColor(hWnd, wParam);
        break;
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            LoadColors();
            SetBorderColor(hWnd, GetForegroundWindow() == hWnd);
        break;
    }

    return result;
}

typedef LRESULT (WINAPI *DefDlgProcA_t)(HWND, UINT, WPARAM, LPARAM);
DefDlgProcA_t DefDlgProcA_orig;
LRESULT WINAPI DefDlgProcA_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefDlgProcA_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_NCACTIVATE:
            SetBorderColor(hWnd, wParam);
        break;
    }

    return result;
}

typedef LRESULT (WINAPI *DefDlgProcW_t)(HWND, UINT, WPARAM, LPARAM);
DefDlgProcW_t DefDlgProcW_orig;
LRESULT WINAPI DefDlgProcW_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefDlgProcW_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_NCACTIVATE:
            SetBorderColor(hWnd, wParam);
        break;
    }

    return result;
}

BOOL CALLBACK EnableEnumWindowsCallback(HWND hWnd, LPARAM lParam) {
    DWORD pid = lParam;

    DWORD wPid = 0;
    GetWindowThreadProcessId(hWnd, &wPid);

    if (pid == wPid) {
        SetBorderColor(hWnd, GetForegroundWindow() == hWnd);
    }

    return TRUE;
}

BOOL CALLBACK DisableEnumWindowsCallback(HWND hWnd, LPARAM lParam) {
    DWORD pid = lParam;

    DWORD wPid = 0;
    GetWindowThreadProcessId(hWnd, &wPid);

    if (pid == wPid) {
        DWORD dwStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
        //Better exclude context menus
        if ((dwStyle & WS_THICKFRAME) == WS_THICKFRAME || (dwStyle & WS_CAPTION) == WS_CAPTION)
        {
            DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR, &ColorDefault, sizeof(ColorDefault));
        }
    }

    return TRUE;
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadColors();

    EnumWindows(EnableEnumWindowsCallback, GetCurrentProcessId());
    
    HMODULE user32 = LoadLibraryW(L"user32.dll");

    FARPROC pDefWindowProcW = GetProcAddress(user32, "DefWindowProcW");

    Wh_SetFunctionHook(
        (void *)pDefWindowProcW,
        (void *)DefWindowProcW_hook,
        (void **)&DefWindowProcW_orig
    );

    FARPROC pDefWindowProcA = GetProcAddress(user32, "DefWindowProcA");

    Wh_SetFunctionHook(
        (void *)pDefWindowProcA,
        (void *)DefWindowProcA_hook,
        (void **)&DefWindowProcA_orig
    );

    FARPROC pDefDlgProcW = GetProcAddress(user32, "DefDlgProcW");

    Wh_SetFunctionHook(
        (void *)pDefDlgProcW,
        (void *)DefDlgProcW_hook,
        (void **)&DefDlgProcW_orig
    );

    FARPROC pDefDlgProcA = GetProcAddress(user32, "DefDlgProcA");

    Wh_SetFunctionHook(
        (void *)pDefDlgProcA,
        (void *)DefDlgProcA_hook,
        (void **)&DefDlgProcA_orig
    );

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    EnumWindows(DisableEnumWindowsCallback, GetCurrentProcessId());
}
