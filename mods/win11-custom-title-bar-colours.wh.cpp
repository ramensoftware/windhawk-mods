// ==WindhawkMod==
// @id              win11-custom-title-bar-colours
// @name            Windows 11 Custom Title Bar Colours
// @description     Allows changing the colour of active/inactive title bars, as well as enabling/disabling Immersive Dark Mode.
// @version         1.0.0
// @author          Th3Fanbus
// @github          https://github.com/Th3Fanbus
// @include         *
// @exclude         devenv.exe
// @compilerOptions -ldwmapi -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
My first attempt at a Windhawk mod, the goal is to make window title bars have consistent colours, especially when using a dark theme.

Based on `win11-accent-border` by Guerra24.

**Consider disabling *Show accent color on title bars and window borders* as it likely conflicts with this mod!**

# Colours

The active and inactive colours can be specified in the mod's settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- UseImmersiveDarkMode: true
  $name: Use Immersive Dark Mode
  $description: Enable this when using dark themes.
- ActiveColour:
  - R: 0
  - G: 0
  - B: 0
- InactiveColour:
  - R: 32
  - G: 32
  - B: 32
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <windhawk_api.h>

COLORREF GetColourSetting(PCWSTR valueName)
{
    COLORREF Color = 0;
    Color |= (UINT8)Wh_GetIntSetting(valueName, 'R') <<  0;
    Color |= (UINT8)Wh_GetIntSetting(valueName, 'G') <<  8;
    Color |= (UINT8)Wh_GetIntSetting(valueName, 'B') << 16;
    return Color;
}

BOOL IsValidWindow(HWND hWnd)
{
    // Better exclude context menus
    DWORD dwStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
    return (dwStyle & WS_THICKFRAME) == WS_THICKFRAME || (dwStyle & WS_CAPTION) == WS_CAPTION;
}

void SetTitleBarColour(HWND hWnd, BOOL activate)
{
    if (!IsValidWindow(hWnd)) {
        return;
    }
    Wh_Log(L"Activate: %d", activate);
    
    BOOL UseDarkMode = Wh_GetIntSetting(L"UseImmersiveDarkMode");
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &UseDarkMode, sizeof(UseDarkMode));

    if (activate) {
        COLORREF CaptionActive = GetColourSetting(L"ActiveColour.%c");
        Wh_Log(L"CaptionActive: %06x", CaptionActive);
        DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR, &CaptionActive, sizeof(CaptionActive));
    } else {
        COLORREF CaptionInactive = GetColourSetting(L"InactiveColour.%c");
        Wh_Log(L"CaptionActive: %06x", CaptionInactive);
        DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR, &CaptionInactive, sizeof(CaptionInactive));
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
            SetTitleBarColour(hWnd, wParam);
        break;
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            SetTitleBarColour(hWnd, GetForegroundWindow() == hWnd);
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
            SetTitleBarColour(hWnd, wParam);
        break;
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            SetTitleBarColour(hWnd, GetForegroundWindow() == hWnd);
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
            SetTitleBarColour(hWnd, wParam);
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
            SetTitleBarColour(hWnd, wParam);
        break;
    }
    return result;
}

BOOL CALLBACK EnableEnumWindowsCallback(HWND hWnd, LPARAM lParam)
{
    DWORD pid = lParam;
    DWORD wPid = 0;
    GetWindowThreadProcessId(hWnd, &wPid);
    if (pid == wPid) {
        SetTitleBarColour(hWnd, GetForegroundWindow() == hWnd);
    }

    return TRUE;
}

BOOL CALLBACK DisableEnumWindowsCallback(HWND hWnd, LPARAM lParam)
{
    const COLORREF ColorDefault = DWMWA_COLOR_DEFAULT;

    DWORD pid = lParam;
    DWORD wPid = 0;
    GetWindowThreadProcessId(hWnd, &wPid);
    if (pid == wPid) {
        if (IsValidWindow(hWnd)) {
            DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR, &ColorDefault, sizeof(ColorDefault));
        }
    }

    return TRUE;
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

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
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");

    EnumWindows(DisableEnumWindowsCallback, GetCurrentProcessId());
}
