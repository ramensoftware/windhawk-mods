// ==WindhawkMod==
// @id              disable-inactive-menubar-greying
// @name            Disable Inactive Menubar Greying
// @description     Prevents menubar text from being greyed out in inactive folder windows in Classic theme
// @version         1.1
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

#include <unordered_map>
#include <unordered_set>

typedef COLORREF (WINAPI *SetTextColor_t)(HDC hdc, COLORREF color);
SetTextColor_t SetTextColor_Original;

struct HDCInfo {
    std::unordered_set<COLORREF> greyColors;
};

std::unordered_map<HDC, HDCInfo> g_hdcMap;
CRITICAL_SECTION g_cs;

bool IsGreyColor(COLORREF color, COLORREF menuTextColor)
{
    BYTE r = GetRValue(color);
    BYTE g = GetGValue(color);
    BYTE b = GetBValue(color);

    return (r == g && g == b && r > 0 && r < 255 && color != menuTextColor);
}

COLORREF WINAPI SetTextColor_Hook(HDC hdc, COLORREF color)
{
    COLORREF menuTextColor = GetSysColor(COLOR_MENUTEXT);

    if (IsGreyColor(color, menuTextColor) && !WindowFromDC(hdc))
    {
        // Для HDC без окна - отслеживаем количество серых цветов
        EnterCriticalSection(&g_cs);
        g_hdcMap[hdc].greyColors.insert(color);
        size_t greyCount = g_hdcMap[hdc].greyColors.size();
        LeaveCriticalSection(&g_cs);

        // Только один оттенок серого - менюбар
        if (greyCount == 1)
        {
            return SetTextColor_Original(hdc, menuTextColor);
        }
    }

    return SetTextColor_Original(hdc, color);
}

BOOL Wh_ModInit()
{
    InitializeCriticalSection(&g_cs);

    Wh_SetFunctionHook(
        (void*)GetProcAddress(GetModuleHandle(L"gdi32.dll"), "SetTextColor"),
        (void*)SetTextColor_Hook,
        (void**)&SetTextColor_Original);

    return TRUE;
}

void Wh_ModUninit()
{
    DeleteCriticalSection(&g_cs);
}
