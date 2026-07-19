// ==WindhawkMod==
// @id              disable-inactive-menubar-greying
// @name            Disable Inactive Menubar Greying
// @description     Prevents menubar text from being greyed out in inactive folder windows in Classic theme
// @version         1.2
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Inactive Menubar Greying

This mod prevents the classic menubar text (File, Edit, View, etc.) in folder windows
under the Classic theme from appearing greyed out when the window loses focus, 
the way it was in Windows 95, before Windows 98.

![screenshot](https://i.imgur.com/FCEXTyt.png)

![screenshot](https://i.imgur.com/XBVNN0v.png)

*/
// ==/WindhawkModReadme==

typedef COLORREF (WINAPI *SetTextColor_t)(HDC hdc, COLORREF color);
SetTextColor_t SetTextColor_Original;

bool IsGreyColor(COLORREF color, COLORREF menuTextColor)
{
    // Отсекаем PALETTERGB/PALETTEINDEX - ненулевой старший байт.
    // Настоящий менюбар всегда получает цвет через GetSysColor,
    // где старший байт всегда нулевой.
    if ((color & 0xFF000000) != 0)
        return false;

    BYTE r = GetRValue(color);
    BYTE g = GetGValue(color);
    BYTE b = GetBValue(color);

    return (r == g && g == b && r > 0 && r < 255 && color != menuTextColor);
}

// Возвращает true, если поток, вызвавший нас, владеет окном,
// которое прямо сейчас находится в фокусе (foreground) у системы.
bool IsCurrentThreadWindowFocused()
{
    HWND fg = GetForegroundWindow();
    if (!fg)
        return false; // ни одно окно не в фокусе -> точно не мы

    DWORD fgThreadId = GetWindowThreadProcessId(fg, nullptr);
    return fgThreadId == GetCurrentThreadId();
}

COLORREF WINAPI SetTextColor_Hook(HDC hdc, COLORREF color)
{
    // Если рисует поток окна, которое реально в фокусе - не вмешиваемся вообще.
    if (IsCurrentThreadWindowFocused())
    {
        return SetTextColor_Original(hdc, color);
    }

    COLORREF menuTextColor = GetSysColor(COLOR_MENUTEXT);

    if (IsGreyColor(color, menuTextColor) && !WindowFromDC(hdc))
    {
        return SetTextColor_Original(hdc, menuTextColor);
    }

    return SetTextColor_Original(hdc, color);
}

BOOL Wh_ModInit()
{
    Wh_SetFunctionHook(
        (void*)GetProcAddress(GetModuleHandle(L"gdi32.dll"), "SetTextColor"),
        (void*)SetTextColor_Hook,
        (void**)&SetTextColor_Original);

    return TRUE;
}
