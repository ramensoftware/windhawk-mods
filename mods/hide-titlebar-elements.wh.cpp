// ==WindhawkMod==
// @id              hide-titlebar-elements
// @name            Hide Titlebar Icon and Text
// @description     Hides certain elements in window titlebars
// @version         1.0
// @author          darkthemer
// @github          https://github.com/darkthemer
// @include         *
// @compilerOptions -luxtheme -ldwmapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Titlebar Icon/Text

Hides certain elements in window titlebars

## Features

- Hide titlebar icon
![hide icon preview](https://i.imgur.com/phtVQdl.png)

- Hide titlebar text
![hide text preview](https://i.imgur.com/G1biOig.png)

## Notes

- Implemented through overriding the `dwAttributes` argument of the `SetWindowThemeNonClientAttributes` function and
applied to windows and dialogs that receive the `WM_NCCREATE` system message.

## Credits

- Inspired by an AutoHotKey script: [TBarIconBlanker](https://www.autohotkey.com/board/topic/78706-tbariconblanker/)
- Some other mods were used as reference:
    - [auto-custom-titlebar-colors](https://windhawk.net/mods/auto-custom-titlebar-colors)
    - [aerexplorer](https://windhawk.net/mods/aerexplorer)
    - [translucent-windows](https://windhawk.net/mods/translucent-windows)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideIcon: true
  $name: Hide Titlebar Icon
  $description: Prevents drawing the titlebar icon
- hideText: true
  $name: Hide Titlebar Text
  $description: Prevents drawing the titlebar text
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <string>
#include <uxtheme.h>

struct
{
    bool hideIcon;
    bool hideText;
} settings;

DWORD g_attributes = 0;

// -----------------------------------------------------------------------------
// Apply mod functionality according to eligibility and settings
// -----------------------------------------------------------------------------

// window eligibility checks mostly from the translucent-windows mod
std::wstring GetWindowClass(HWND hWnd)
{
    WCHAR buffer[MAX_PATH];
    GetClassNameW(hWnd, buffer, MAX_PATH);
    return buffer;
}

BOOL IsWindowClass(HWND hWnd, LPCWSTR className)
{
    return GetWindowClass(hWnd) == className;
}

BOOL IsWindowCloaked(HWND hwnd)
{
    BOOL isCloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &isCloaked, sizeof(isCloaked))) && isCloaked;
}

BOOL IsWindowEligible(HWND hWnd)
{
    BOOL isFlyoutWindow = IsWindowClass(hWnd, TOOLTIPS_CLASS) || IsWindowClass(hWnd, L"DropDown") ||
                          IsWindowClass(hWnd, L"ViewControlClass");
    if (isFlyoutWindow)
        return FALSE;

    LONG_PTR styleEx = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);

    HWND hParentWnd = GetAncestor(hWnd, GA_PARENT);
    if (hParentWnd && hParentWnd != GetDesktopWindow())
        return FALSE;

    BOOL hasTitleBar = (style & WS_CAPTION) == WS_CAPTION;
    BOOL hasCaptionButtons = (style & (WS_MINIMIZEBOX | WS_MAXIMIZEBOX)) != 0;
    BOOL hasSystemMenu = (style & WS_SYSMENU) != 0;
    BOOL hasThickFrame = (style & WS_THICKFRAME) == WS_THICKFRAME;
    BOOL isWindowCEF = (IsWindowClass(hWnd, L"Chrome_WidgetWin_1") || IsWindowClass(hWnd, L"Chrome_WidgetWin_0"));

    // https://devblogs.microsoft.com/oldnewthing/20200302-00/?p=103507
    //  Allow containers of Windows Store apps (WinStore.exe, Settings.exe, etc.)
    //  Allow also Chromium Embedded Framework (Brave.exe) created as cloaked.
    if (IsWindowCloaked(hWnd) && !IsWindowClass(hWnd, L"ApplicationFrameWindow") && !isWindowCEF)
        return FALSE;

    // Windows become disabled even when they are displayed (e.g. Recycle Bin) when a pop-up window opens in front.
    if (!IsWindowEnabled(hWnd) && !IsWindowVisible(hWnd))
        return FALSE;

    // Pass ineligible CEF windows like Discord/Vencord
    if (isWindowCEF && (hasCaptionButtons || hasTitleBar))
        return TRUE;
    // Fixes Snipping Tool recording
    if ((styleEx & WS_EX_NOACTIVATE) || (styleEx & WS_EX_TRANSPARENT))
        return FALSE;
    // Most top-level windows
    if ((style & WS_POPUPWINDOW) == WS_POPUPWINDOW || (style & WS_OVERLAPPEDWINDOW) == WS_OVERLAPPEDWINDOW ||
        (styleEx & WS_EX_DLGMODALFRAME) == WS_EX_DLGMODALFRAME)
        return TRUE;
    // Overlapped windows like the Win32 progress window
    if (hasTitleBar && hasSystemMenu && (hasCaptionButtons || hasThickFrame))
        return TRUE;

    return FALSE;
}

VOID HideTitlebarElements(HWND hWnd)
{
    if (!IsWindowEligible(hWnd))
        return;

    Wh_Log(L"Hidden");
    SetWindowThemeNonClientAttributes(hWnd, WTNCA_VALIDBITS, g_attributes);
}

// -----------------------------------------------------------------------------
// Enumerate top-level windows for modding
// -----------------------------------------------------------------------------

BOOL CALLBACK ApplyEnumWindowsProc(HWND hWnd, LPARAM)
{
    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hWnd, &pid) || pid != GetCurrentProcessId())
        return TRUE;

    HideTitlebarElements(hWnd);

    return TRUE;
}

BOOL CALLBACK UninitEnumWindowsProc(HWND hWnd, LPARAM)
{
    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hWnd, &pid) || pid != GetCurrentProcessId())
        return TRUE;

    if (!IsWindowEligible(hWnd))
        return TRUE;

    SetWindowThemeNonClientAttributes(hWnd, WTNCA_VALIDBITS, (DWORD)0);

    return TRUE;
}

// -----------------------------------------------------------------------------
// Hooks
// -----------------------------------------------------------------------------

using DefWindowProc_t = decltype(&DefWindowProc);
DefWindowProc_t DefWindowProc_orig;

LRESULT WINAPI DefWindowProc_hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (Msg == WM_NCCREATE)
        HideTitlebarElements(hWnd);

    return DefWindowProc_orig(hWnd, Msg, wParam, lParam);
}

using DefDlgProc_t = decltype(&DefDlgProc);
DefDlgProc_t DefDlgProc_orig;

LRESULT WINAPI DefDlgProc_hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (Msg == WM_NCCREATE)
        HideTitlebarElements(hWnd);

    return DefDlgProc_orig(hWnd, Msg, wParam, lParam);
}

using SetWindowThemeNonClientAttributes_t = decltype(&SetWindowThemeNonClientAttributes);
SetWindowThemeNonClientAttributes_t SetWindowThemeNonClientAttributes_orig;

HRESULT WINAPI SetWindowThemeNonClientAttributes_hook(HWND hWnd, DWORD dwMask, DWORD dwAttributes)
{
    if ((dwMask & g_attributes) && IsWindowEligible(hWnd))
        return SetWindowThemeNonClientAttributes_orig(hWnd, WTNCA_VALIDBITS, g_attributes);

    return SetWindowThemeNonClientAttributes_orig(hWnd, dwMask, dwAttributes);
}

// -----------------------------------------------------------------------------
// Windhawk functions
// -----------------------------------------------------------------------------

void LoadSettings()
{
    settings.hideIcon = Wh_GetIntSetting(L"hideIcon");
    settings.hideText = Wh_GetIntSetting(L"hideText");

    if (settings.hideIcon)
        g_attributes |= WTNCA_NODRAWICON | WTNCA_NOSYSMENU;
    else
        g_attributes &= ~WTNCA_NODRAWICON & ~WTNCA_NOSYSMENU;

    if (settings.hideText)
        g_attributes |= WTNCA_NODRAWCAPTION;
    else
        g_attributes &= ~WTNCA_NODRAWCAPTION;
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    LoadSettings();

    Wh_SetFunctionHook((void *)DefWindowProc, (void *)DefWindowProc_hook, (void **)&DefWindowProc_orig);
    Wh_SetFunctionHook((void *)DefDlgProc, (void *)DefDlgProc_hook, (void **)&DefDlgProc_orig);
    Wh_SetFunctionHook((void *)SetWindowThemeNonClientAttributes, (void *)SetWindowThemeNonClientAttributes_hook,
                       (void **)&SetWindowThemeNonClientAttributes_orig);

    return TRUE;
}

BOOL Wh_ModAfterInit()
{
    Wh_Log(L"AfterInit");

    EnumWindows(ApplyEnumWindowsProc, 0);

    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"Uninit");

    EnumWindows(UninitEnumWindowsProc, 0);
}

void Wh_ModSettingsChanged()
{
    Wh_Log(L"SettingsChanged");

    LoadSettings();

    EnumWindows(ApplyEnumWindowsProc, 0);
}
