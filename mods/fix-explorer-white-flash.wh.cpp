// ==WindhawkMod==
// @id              fix-explorer-white-flash
// @name            Fix white flashes in explorer
// @description     Fixes white flashes when creating new tabs in "This PC".
// @version         1.2
// @author          Mgg Sk
// @github          https://github.com/MGGSK
// @include         explorer.exe
// @compilerOptions -lGdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fix white flashes in explorer
Fixes white flashes when creating new tabs in "This PC".

### Before
![Before](https://raw.githubusercontent.com/MGGSK/MGGSK/refs/heads/main/WindhawkModReadmeImages/fix-explorer-white-flash-before.png)

### After
![After](https://raw.githubusercontent.com/MGGSK/MGGSK/refs/heads/main/WindhawkModReadmeImages/fix-explorer-white-flash-after.png)

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <Windows.h>

const HBRUSH g_windowBrush = CreateSolidBrush(0x191919);
HMODULE g_hUxTheme = nullptr;

using ShouldAppsUseDarkMode_T = bool(WINAPI*)();
ShouldAppsUseDarkMode_T ShouldAppsUseDarkMode = nullptr;

decltype(&GetSysColorBrush) GetSysColorBrush_Original;
HBRUSH WINAPI GetSysColorBrush_Hook(int nIndex)
{
    if(nIndex == COLOR_WINDOW && ShouldAppsUseDarkMode())
        return g_windowBrush;

    return GetSysColorBrush_Original(nIndex);
}

BOOL Wh_ModInit()
{
    g_hUxTheme = LoadLibraryExW(L"UxTheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!g_hUxTheme)
    {
        Wh_Log(L"Failed to load UxTheme.dll!");
        return FALSE;
    }

    ShouldAppsUseDarkMode = (ShouldAppsUseDarkMode_T)GetProcAddress(g_hUxTheme, MAKEINTRESOURCEA(132));
    if(!ShouldAppsUseDarkMode)
    {
        Wh_Log(L"Failed to load ShouldAppsUseDarkMode!");
        return FALSE;
    }

    return WindhawkUtils::SetFunctionHook(
            GetSysColorBrush,
            GetSysColorBrush_Hook, 
            &GetSysColorBrush_Original);
}

void Wh_ModUninit()
{
    if(g_windowBrush)
        DeleteObject(g_windowBrush);

    if(g_hUxTheme)
        FreeLibrary(g_hUxTheme);
}
