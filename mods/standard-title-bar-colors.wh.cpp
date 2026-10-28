// ==WindhawkMod==
// @id              standard-title-bar-colors
// @name            Explorer Standard Title Bar Colors
// @description     Forces Explorer to use the same titlebar colors as most apps.
// @version         1.0.0
// @author          j--m
// @github          https://github.com/j--m
// @include         explorer.exe         
// @compilerOptions -ldwmapi -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Explorer overrides its titlebar colors. This stops that.
If you're not applying your accent color to the titlebars, you may not notice the difference.

If you disable this mod, toggling the focus of an open window will prompt Explorer to override the
color again.

This mod can probably be applied, at your own risk, to other apps that use DwmSetWindowAttribute to
change their titlebar colors.

Derived from `win11-custom-title-bar-colours` by Th3Fanbus.

Before: Explorer does its own thing.

![image of two apps with different titlebar colors](https://raw.githubusercontent.com/j--m/windhawk-mods/refs/heads/pages/before.png)

After: They match!

![image of two apps with same titlebar colors](https://raw.githubusercontent.com/j--m/windhawk-mods/refs/heads/pages/after.png)

*/
// ==/WindhawkModReadme==

#include <dwmapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

BOOL IsValidWindow(HWND hWnd)
{
    // Better exclude context menus
    DWORD dwStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
    return (dwStyle & WS_THICKFRAME) == WS_THICKFRAME || (dwStyle & WS_CAPTION) == WS_CAPTION;
}

decltype(&DwmSetWindowAttribute) DwmSetWindowAttribute_orig;

HRESULT WINAPI
DwmSetWindowAttribute_hook(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
{
    if(dwAttribute != DWMWA_CAPTION_COLOR)
       return DwmSetWindowAttribute_orig(hwnd, dwAttribute, pvAttribute, cbAttribute);
    return S_OK;
}

BOOL CALLBACK EnumWindowsCallback(HWND hWnd, LPARAM lParam)
{
    const COLORREF ColorDefault = DWMWA_COLOR_DEFAULT;

    DWORD pid = lParam;
    DWORD wPid = 0;
    GetWindowThreadProcessId(hWnd, &wPid);
    if (pid == wPid) {
        if (IsValidWindow(hWnd)) {
            DwmSetWindowAttribute_orig(hWnd, DWMWA_CAPTION_COLOR, &ColorDefault, sizeof(ColorDefault));
        }
    }

    return TRUE;
}

// prevent explorer from changing titlebar colors
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");
    
    WindhawkUtils
        ::SetFunctionHook(DwmSetWindowAttribute, DwmSetWindowAttribute_hook, &DwmSetWindowAttribute_orig);
    return TRUE;
}

// reset existing titlebars to default
void Wh_ModAfterInit()
{
    EnumWindows(EnumWindowsCallback, GetCurrentProcessId());
}

// explorer will resume its normal behavior of changing the titlebar colors (added to readme)
// i should try to make it do so now, probably by enumerating its windows and telling them their
// active state has changed, but it's not critical and for so small a gain i don't want to release
// an implementation that isn't rock-solid
void Wh_ModUninit()
{ 
    Wh_Log(L"Uninit");
}
