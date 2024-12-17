// ==WindhawkMod==
// @id              classic-menus
// @name            Classic Menus
// @description     Makes menus classic themed while keeping other controls themed
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Menus
In Windows XP, menus remained unthemed even with themes enabled. This mod replicates
that functionality.

**This mod will only work on Windhawk v1.4 and greater.**

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/classic-menus-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/main/classic-menus-after.png)
*/
// ==/WindhawkModReadme==

#include <uxtheme.h>
#include <windhawk_utils.h>

typedef HTHEME (__fastcall *OpenThemeDataExInternal_t)(HWND, LPCWSTR, int, INT_PTR, int);
OpenThemeDataExInternal_t OpenThemeDataExInternal_orig;
HTHEME __fastcall OpenThemeDataExInternal_hook(
    HWND    hWnd,
    LPCWSTR pszClassList,
    int     i1,
    INT_PTR i2,
    int     i3
)
{
    if (!wcsicmp(pszClassList, L"MENU"))
    {
        SetLastError(E_POINTER);
        return 0;
    }

    return OpenThemeDataExInternal_orig(
        hWnd, pszClassList, i1, i2, i3
    );
}

BOOL Wh_ModInit(void)
{
    HMODULE hUxTheme = LoadLibraryW(L"uxtheme.dll");
    if (!hUxTheme)
    {
        Wh_Log(L"Failed to load uxtheme.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK hook = {
        {
            #ifdef _WIN64
            L"OpenThemeDataExInternal"
            #else
            L"_OpenThemeDataExInternal@20"
            #endif
        },
        &OpenThemeDataExInternal_orig,
        OpenThemeDataExInternal_hook,
        false
    };

    if (!WindhawkUtils::HookSymbols(
        hUxTheme,
        &hook,
        1
    ))
    {
        Wh_Log(L"Failed to hook OpenThemeDataExInternal");
        return FALSE;
    }

    return TRUE;
}