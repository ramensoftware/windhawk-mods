// ==WindhawkMod==
// @id              snap-border-fix
// @name            Snap Border Fix
// @description     Changes window borders to a custom color, mainly because of the white borders forced by the Windows 11 snapping feature. Credits to Bo0ii for the Source Code.
// @version         1.0.0
// @author          M4D_MAXX_
// @github          https://github.com/M4DMAXX
// @homepage        https://github.com/M4DMAXX/windhawk-mods
// @include         *
// @exclude         devenv.exe
// @compilerOptions -ldwmapi -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Snap Border Fix

Change the window borders to a custom color, to get rid of the white borders when using the Windows snapping feature. No other mod I found addressed this issue in the way I needed.

![Windows 11 Default vs. Snap Border Fix](https://raw.githubusercontent.com/M4DMAXX/my-windhawk-mods/main/snap-border-fix/Snap%20Border%20Fix%20Comparsion.png)
![Invisible Borders / Window Border Customizer](https://raw.githubusercontent.com/M4DMAXX/my-windhawk-mods/main/snap-border-fix/Snap%20Border%20Fix%20Comparsion%20to%20Other%20Mod.png)

## Features

- **Custom Border Color and No More White Snap Borders**: Replaces the white borders shown by Windows when using the snapping feature, by a custom color. Default Color: Dark Anthracite

## Support

Contact me via Discord/Github - m4d_maxx_

## Credits

This mod is derived from the source code of "Invisible Window Borders" by Bo0ii, released under the MIT License.

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
#include <windhawk_utils.h>
#include <atomic>

std::atomic<COLORREF> BorderColor = RGB(32, 32, 32);
const COLORREF ColorDefault = DWMWA_COLOR_DEFAULT;

std::atomic<bool> SpecialWindows = false;

bool HexToColorref(PCWSTR hex, COLORREF* color)
{
    if (!hex || !color)
        return false;

    if (hex[0] == L'#')
        hex++;

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
    auto hexColor = WindhawkUtils::StringSetting::make(L"BorderColor");

COLORREF newColor;

if (HexToColorref(hexColor.get(), &newColor))
{
    BorderColor.store(newColor);

    COLORREF borderColor = BorderColor.load();

    Wh_Log(
        L"Loaded border color: #%02X%02X%02X",
        GetRValue(borderColor),
        GetGValue(borderColor),
        GetBValue(borderColor)
    );
}
else
{
    BorderColor.store(RGB(32, 32, 32));

    Wh_Log(
        L"Invalid border color setting, using default #202020"
    );
}

SpecialWindows.store(Wh_GetIntSetting(L"SpecialWindows"));
}



BOOL IsValidWindow(HWND hWnd) {
    DWORD dwStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
    // Better exclude context menus
    return (dwStyle & WS_THICKFRAME) == WS_THICKFRAME ||
           (dwStyle & WS_CAPTION) == WS_CAPTION ||
           (SpecialWindows.load() &&
            (dwStyle & WS_OVERLAPPED) == WS_OVERLAPPED &&
            (dwStyle & WS_POPUP) != WS_POPUP);
}

using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_orig;
HRESULT WINAPI DwmSetWindowAttribute_hook(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute) {
    if (dwAttribute == DWMWA_BORDER_COLOR &&
        cbAttribute == sizeof(COLORREF) &&
        IsValidWindow(hwnd)) {
        Wh_Log(L"Intercepted DWMWA_BORDER_COLOR - setting new border color");
        // Override with border Color
        COLORREF borderColor = BorderColor.load();

        return DwmSetWindowAttribute_orig(
            hwnd,
            dwAttribute,
            &borderColor,
            sizeof(borderColor)
        );
    }

    return DwmSetWindowAttribute_orig(hwnd, dwAttribute, pvAttribute, cbAttribute);
}

void SetBorderColor(HWND hWnd)
{
    if (!IsValidWindow(hWnd))
        return;

    Wh_Log(L"Setting border color");
    COLORREF borderColor = BorderColor.load();

    DwmSetWindowAttribute_orig(
        hWnd,
        DWMWA_BORDER_COLOR,
        &borderColor,
        sizeof(borderColor)
    );
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

    if (!WindhawkUtils::SetFunctionHook(
            DwmSetWindowAttribute,
            DwmSetWindowAttribute_hook,
            &DwmSetWindowAttribute_orig)) {
        Wh_Log(L"Failed to hook DwmSetWindowAttribute");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(
            DefWindowProcW,
            DefWindowProcW_hook,
            &DefWindowProcW_orig)) {
        Wh_Log(L"Failed to hook DefWindowProcW");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(
            DefWindowProcA,
            DefWindowProcA_hook,
            &DefWindowProcA_orig)) {
        Wh_Log(L"Failed to hook DefWindowProcA");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(
            DefDlgProcW,
            DefDlgProcW_hook,
            &DefDlgProcW_orig)) {
        Wh_Log(L"Failed to hook DefDlgProcW");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(
            DefDlgProcA,
            DefDlgProcA_hook,
            &DefDlgProcA_orig)) {
        Wh_Log(L"Failed to hook DefDlgProcA");
        return FALSE;
    }

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
