// ==WindhawkMod==
// @id              snap-border-fix
// @name            Snap Border Fix - Fork/Inspiration by Invisible Borders
// @description     Changes window borders to a custom color, mainly because of the white borders forced by the Windows 11 snapping feature. Inspired by "Invisible Borders".
// @version         1.0.0
// @author          M4D_MAXX_, Bo0ii (Original Source Code)
// @github          https://github.com/M4DMAXX
// @include         *
// @exclude         devenv.exe
// @compilerOptions -ldwmapi -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Snap Border Fix

Change the window borders to a custom color, to get rid of the white borders when using the Windows snapping feature.

## Features

- **Custom Border Color and No More White Snap Borders**: Replaces the white borders shown by Windows when using the snapping feature, by a custom color. Default Color: Dark Anthracite
- **Universal**: Designed to work with all applications
- **Lightweight**: Minimal performance impact

## Compatibility

- Windows 10 (version 1809 and later)
- Windows 11 (all versions)
- Requires DWM (Desktop Window Manager) to be enabled

## Support

Contact me via Discord/Github - m4d_maxx_

## License

MIT License - Feel free to modify and distribute
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- BorderColor: "202020"
  $name: Border Color
  $description: HEX color in RRGGBB format (e.g. 202020)
- SpecialWindows: false
  $name: Apply to Special Windows
  $description: Also apply to special windows like dialogs
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <windhawk_api.h>

COLORREF BorderColor = RGB(32, 32, 32);  // Set desired Color for the border - no more ugly white ^^
const COLORREF ColorDefault = DWMWA_COLOR_DEFAULT;

bool SpecialWindows = false;

bool HexToColorref(PCWSTR hex, COLORREF* color)
{
    if (!hex || !color)
        return false;

    if (wcslen(hex) != 6)
        return false;

    unsigned int value = 0;

    for (int i = 0; i < 6; i++)
    {
        wchar_t c = hex[i];
        unsigned int digit;

        if (c >= L'0' && c <= L'9')
            digit = c - L'0';
        else if (c >= L'A' && c <= L'F')
            digit = c - L'A' + 10;
        else if (c >= L'a' && c <= L'f')
            digit = c - L'a' + 10;
        else
            return false;

        value = (value << 4) | digit;
    }

    BYTE r = (BYTE)((value >> 16) & 0xFF);
    BYTE g = (BYTE)((value >> 8) & 0xFF);
    BYTE b = (BYTE)(value & 0xFF);

    *color = RGB(r, g, b);
    return true;
}

void LoadSettings()
{
    PCWSTR hexColor = Wh_GetStringSetting(L"BorderColor");

    COLORREF newColor;

    if (HexToColorref(hexColor, &newColor))
    {
        BorderColor = newColor;

        Wh_Log(
            L"Loaded border color: #%02X%02X%02X",
            GetRValue(BorderColor),
            GetGValue(BorderColor),
            GetBValue(BorderColor)
        );
    }
    else
    {
        BorderColor = RGB(32, 32, 32);

        Wh_Log(
            L"Invalid border color setting, using default #202020"
        );
    }

    Wh_FreeStringSetting(hexColor);

    SpecialWindows = Wh_GetIntSetting(L"SpecialWindows");
}



BOOL IsValidWindow(HWND hWnd) {
    DWORD dwStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
    // Better exclude context menus
    return (dwStyle & WS_THICKFRAME) == WS_THICKFRAME ||
           (dwStyle & WS_CAPTION) == WS_CAPTION ||
           (SpecialWindows && (dwStyle & WS_OVERLAPPED) == WS_OVERLAPPED && (dwStyle & WS_POPUP) != WS_POPUP);
}

using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_orig;
HRESULT WINAPI DwmSetWindowAttribute_hook(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute) {
    if (dwAttribute == DWMWA_BORDER_COLOR && IsValidWindow(hwnd)) {
        Wh_Log(L"Intercepted DWMWA_BORDER_COLOR - setting new border color");
        // Override with border Color
        return DwmSetWindowAttribute_orig(hwnd, dwAttribute, &BorderColor, sizeof(BorderColor));
    }

    return DwmSetWindowAttribute_orig(hwnd, dwAttribute, pvAttribute, cbAttribute);
}

void SetBorderColor(HWND hWnd)
{
    if (!IsValidWindow(hWnd))
        return;

    Wh_Log(L"Setting border color");
    DwmSetWindowAttribute_orig(hWnd, DWMWA_BORDER_COLOR, &BorderColor, sizeof(BorderColor));
}

using DefWindowProcA_t = decltype(&DefWindowProcA);
DefWindowProcA_t DefWindowProcA_orig;
LRESULT WINAPI DefWindowProcA_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefWindowProcA_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_ACTIVATE:
        case WM_NCACTIVATE:
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            SetBorderColor(hWnd);
            break;
    }

    return result;
}

using DefWindowProcW_t = decltype(&DefWindowProcW);
DefWindowProcW_t DefWindowProcW_orig;
LRESULT WINAPI DefWindowProcW_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefWindowProcW_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_ACTIVATE:
        case WM_NCACTIVATE:
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            SetBorderColor(hWnd);
            break;
    }

    return result;
}

using DefDlgProcA_t = decltype(&DefDlgProcA);
DefDlgProcA_t DefDlgProcA_orig;
LRESULT WINAPI DefDlgProcA_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefDlgProcA_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_NCACTIVATE:
            SetBorderColor(hWnd);
            break;
    }

    return result;
}

using DefDlgProcW_t = decltype(&DefDlgProcW);
DefDlgProcW_t DefDlgProcW_orig;
LRESULT WINAPI DefDlgProcW_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefDlgProcW_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_NCACTIVATE:
            SetBorderColor(hWnd);
            break;
    }

    return result;
}

BOOL CALLBACK EnableEnumWindowsCallback(HWND hWnd, LPARAM lParam) {
    DWORD pid = lParam;

    DWORD wPid = 0;
    GetWindowThreadProcessId(hWnd, &wPid);

    if (pid == wPid) {
        SetBorderColor(hWnd);
    }

    return TRUE;
}

BOOL CALLBACK DisableEnumWindowsCallback(HWND hWnd, LPARAM lParam) {
    DWORD pid = lParam;
    DWORD wPid = 0;
    GetWindowThreadProcessId(hWnd, &wPid);

    if (pid == wPid && IsValidWindow(hWnd))
        DwmSetWindowAttribute_orig(hWnd, DWMWA_BORDER_COLOR, &ColorDefault, sizeof(ColorDefault));

    return TRUE;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init - Snap Border Fix");

    LoadSettings();

    Wh_SetFunctionHook(
        (void *)DwmSetWindowAttribute,
        (void *)DwmSetWindowAttribute_hook,
        (void **)&DwmSetWindowAttribute_orig);

    Wh_SetFunctionHook(
        (void *)DefWindowProcW,
        (void *)DefWindowProcW_hook,
        (void **)&DefWindowProcW_orig
    );

    Wh_SetFunctionHook(
        (void *)DefWindowProcA,
        (void *)DefWindowProcA_hook,
        (void **)&DefWindowProcA_orig
    );

    Wh_SetFunctionHook(
        (void *)DefDlgProcW,
        (void *)DefDlgProcW_hook,
        (void **)&DefDlgProcW_orig
    );

    Wh_SetFunctionHook(
        (void *)DefDlgProcA,
        (void *)DefDlgProcA_hook,
        (void **)&DefDlgProcA_orig
    );

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"AfterInit - Applying desired border color");
    EnumWindows(EnableEnumWindowsCallback, GetCurrentProcessId());
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"BeforeUninit - Restoring default border color");
    EnumWindows(DisableEnumWindowsCallback, GetCurrentProcessId());
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    EnumWindows(DisableEnumWindowsCallback, GetCurrentProcessId());
    LoadSettings();
    EnumWindows(EnableEnumWindowsCallback, GetCurrentProcessId());
}
