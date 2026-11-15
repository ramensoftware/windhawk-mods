// ==WindhawkMod==
// @id              classic-explorer-menubar
// @name            Classic Explorer Menubar
// @description     Turns off theming in Explorer menu bar
// @version         1.0.1
// @author          xalejandro
// @github          https://github.com/tetawaves
// @include         explorer.exe
// @compilerOptions -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Explorer Menubar
Turns off theming for Explorer menu bar, making it use system colors, like in Windows XP.

![Before / After](https://i.imgur.com/yAyegLY.png)
*/
// ==/WindhawkModReadme==

#include <uxtheme.h>
#include <windhawk_utils.h>

#ifdef _WIN64
#   define THISCALL  __cdecl
#   define STHISCALL L"__cdecl"
#else
#   define THISCALL  __thiscall
#   define STHISCALL L"__thiscall"
#endif

LRESULT (THISCALL *CMenuStaticToolbar__DefWindowProc_orig)(void *pThis, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT THISCALL CMenuStaticToolbar__DefWindowProc_hook(void *pThis, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HTHEME hTheme = OpenThemeData(hWnd, L"TOOLBAR");
    WCHAR szClassName[256];
    GetClassNameW(GetAncestor(hWnd, GA_ROOT), szClassName, 256);

    if (hTheme && !wcscmp(szClassName, L"CabinetWClass"))
    {
        SetWindowTheme(hWnd, L"", L"");
        SendMessage(hWnd, WM_THEMECHANGED, NULL, NULL);
        CloseThemeData(hTheme);
    }
    
    return CMenuStaticToolbar__DefWindowProc_orig(pThis, hWnd, uMsg, wParam, lParam);
}

BOOL Wh_ModInit() 
{
    Wh_Log(L"Init");

    HMODULE hShell32 = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hShell32) 
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }
    
    WindhawkUtils::SYMBOL_HOOK shell32DllHook
    {
        {
            L"protected: virtual __int64 " STHISCALL " CMenuStaticToolbar::_DefWindowProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
        },
        &CMenuStaticToolbar__DefWindowProc_orig,
        CMenuStaticToolbar__DefWindowProc_hook,
        false
    };

    if (!WindhawkUtils::HookSymbols(hShell32, &shell32DllHook, 1)) 
    {
        Wh_Log(L"Failed to hook shell32.dll");
        return FALSE;
    }

    return TRUE;
}
