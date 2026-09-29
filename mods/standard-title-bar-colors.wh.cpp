// ==WindhawkMod==
// @id              standard-title-bar-colors
// @name            Explorer Standard Title Bar Colours
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
Can probably be applied, at your own risk, to other apps that use DwmSetWindowAttribute to change
their titlebar colors.

Derived from `win11-custom-title-bar-colours` by Th3Fanbus.
*/
// ==/WindhawkModReadme==

#include <dwmapi.h>
#include <windhawk_api.h>

BOOL IsValidWindow(HWND hWnd)
{
    // Better exclude context menus
    DWORD dwStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
    return (dwStyle & WS_THICKFRAME) == WS_THICKFRAME || (dwStyle & WS_CAPTION) == WS_CAPTION;
}

HRESULT (WINAPI * DwmSetWindowAttribute_orig) (HWND, DWORD, LPCVOID, DWORD);

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

// prevent explorer from changing titlebar colors, and reset existing titlebars to default
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");
    
    HMODULE dwmapi = LoadLibraryW(L"dwmapi.dll");
    FARPROC pDwmSetWindowAttribute = GetProcAddress(dwmapi, "DwmSetWindowAttribute");
    Wh_SetFunctionHook(
        (void *) pDwmSetWindowAttribute,
        (void *) DwmSetWindowAttribute_hook,
        (void * *) &DwmSetWindowAttribute_orig
    );

    EnumWindows(EnumWindowsCallback, GetCurrentProcessId());

    return TRUE;
}

// explorer will resume its normal behavior of changing the titlebar colors
// i should try to make it do so now, but not sure i want that to be a blocker for releasing the mod
void Wh_ModUninit()
{ 
    Wh_Log(L"Uninit");
}
