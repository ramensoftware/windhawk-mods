// ==WindhawkMod==
// @id              disable-inactive-menubar-greying
// @name            Disable Inactive Menubar Greying
// @description     Prevents menubar text from being greyed out in inactive folder windows in Classic theme
// @version         1.3
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

thread_local bool checked=false;
thread_local bool found=false;

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

BOOL CALLBACK EnumThreadWndProc_CheckTaskbar(HWND hwnd, LPARAM lParam)
{
    wchar_t className[256];
    if (GetClassNameW(hwnd, className, 256) > 0)
    {
        if (wcscmp(className, L"Shell_TrayWnd") == 0 ||
            wcscmp(className, L"Shell_SecondaryTrayWnd") == 0)
        {
            *(bool*)lParam = true;
            return FALSE; // нашли, прекращаем перебор
        }
    }
    return TRUE; // продолжаем перебор
}

// Возвращает true, если текущий поток владеет хотя бы одним окном панели задач
bool IsCurrentThreadTaskbar()
{
    if (!checked) {
        EnumThreadWindows(GetCurrentThreadId(), EnumThreadWndProc_CheckTaskbar, (LPARAM)&found);
        checked=true;
    }
    return found;
}

COLORREF WINAPI SetTextColor_Hook(HDC hdc, COLORREF color)
{
    COLORREF menuTextColor = GetSysColor(COLOR_MENUTEXT);

    // Cheapest checks first: no syscalls unless the color actually matches.
    if (!IsGreyColor(color, menuTextColor) || WindowFromDC(hdc) ||
        IsCurrentThreadWindowFocused() || IsCurrentThreadTaskbar()) {
        return SetTextColor_Original(hdc, color);
    }

    return SetTextColor_Original(hdc, menuTextColor);
}

BOOL Wh_ModInit()
{
    Wh_SetFunctionHook(
        (void*)GetProcAddress(GetModuleHandle(L"gdi32.dll"), "SetTextColor"),
        (void*)SetTextColor_Hook,
        (void**)&SetTextColor_Original);

    return TRUE;
}
