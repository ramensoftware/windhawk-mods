// ==WindhawkMod==
// @id              dark-menus
// @name            Dark mode context menus
// @description     Enables dark mode for all win32 menus.
// @version         1.0
// @author          Mgg Sk
// @github          https://github.com/MGGSK
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Dark mode context menus
Forces dark mode for all win32 context menus to create a more consistent UI.

### Before:
![Before](https://i.imgur.com/bGRVJz8.png)

### After:
![After](https://i.imgur.com/BURKEki.png)
*/
// ==/WindhawkModReadme==

#include <minwindef.h>
#include <winerror.h>

enum AppMode
{
	Default,
	AllowDark,
	ForceDark,
	ForceLight,
	Max
};

using FlushMenuThemes_T = void (WINAPI *)();
using SetPreferredAppMode_T = HRESULT (WINAPI *)(AppMode appMode);

FlushMenuThemes_T FlushMenuThemes;
SetPreferredAppMode_T SetPreferredAppMode;

//Applies the theme to all menus.
HRESULT ApplyTheme()
{
    FlushMenuThemes();
    return SetPreferredAppMode(ForceDark);
}

//Import functions
BOOL Wh_ModInit() {
    HMODULE hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    FARPROC pSetPreferredAppMode = GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135));
    FARPROC pFlushMenuThemes = GetProcAddress(hUxtheme, MAKEINTRESOURCEA(136));

    SetPreferredAppMode = reinterpret_cast<SetPreferredAppMode_T>(pSetPreferredAppMode);
    FlushMenuThemes = reinterpret_cast<FlushMenuThemes_T>(pFlushMenuThemes);

    HRESULT hResult = ApplyTheme();
    return SUCCEEDED(hResult);
}

//Restores the default theme.
void Wh_ModUninit()
{
    FlushMenuThemes();
    SetPreferredAppMode(Default);
}
