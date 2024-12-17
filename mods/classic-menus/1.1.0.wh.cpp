// ==WindhawkMod==
// @id              classic-menus
// @name            Classic Menus
// @description     Makes menus classic themed while keeping other controls themed
// @version         1.1.0
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

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/classic-menus-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/main/classic-menus-after.png)
*/
// ==/WindhawkModReadme==

#include <uxtheme.h>
#include <windhawk_utils.h>

int (__fastcall *CThemeMenu_Attach_orig)(HWND, HMENU, int, bool, void **);
int __fastcall CThemeMenu_Attach_hook(HWND, HMENU, int, bool, void **)
{
    return 0;
}

WindhawkUtils::SYMBOL_HOOK uxThemeDllHooks[] = {
    {
        {
            L"protected: static int "
#ifdef _WIN64
            L"__cdecl"
#else
            L"__stdcall"
#endif
            L" CThemeMenu::Attach(struct HWND__ *,struct HMENU__ *,int,bool,class CThemeMenu * *)"
        },
        &CThemeMenu_Attach_orig,
        CThemeMenu_Attach_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    HMODULE hUxTheme = LoadLibraryW(L"uxtheme.dll");
    if (!hUxTheme)
    {
        Wh_Log(L"Failed to load uxtheme.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hUxTheme,
        uxThemeDllHooks,
        ARRAYSIZE(uxThemeDllHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in uxtheme.dll");
        return FALSE;
    }

    return TRUE;
}