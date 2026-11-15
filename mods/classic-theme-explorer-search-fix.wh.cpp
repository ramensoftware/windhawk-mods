// ==WindhawkMod==
// @id              classic-theme-explorer-search-fix
// @name            Classic Theme Explorer Search Fix
// @description     Fixes the appearance of the search box in Explorer with classic theme
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Theme Explorer Search Fix
When using classic theme in Windows 7 and up, the colors of the placeholder
text in Explorer's search box are hardcoded. This mod makes it use system
colors instead.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-theme-explorer-search-fix/before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-theme-explorer-search-fix/after.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

void (__fastcall *CSearchEditBox__Draw_orig)(void *pThis, HDC, LPRECT);
void __fastcall CSearchEditBox__Draw_hook(void *pThis, HDC hdc, LPRECT prcDraw)
{
    COLORREF crSave = SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
    COLORREF crBkSave = SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
    CSearchEditBox__Draw_orig(pThis, hdc, prcDraw);
    SetBkColor(hdc, crBkSave);
    SetTextColor(hdc, crSave);
}

const WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[] = {
    {
        {
            L"private: void "
#ifdef _WIN64
            L"__cdecl"
#else
            L"__stdcall"
#endif
            L" CSearchEditBox::_Draw(struct HDC__ *,struct tagRECT *)"
        },
        &CSearchEditBox__Draw_orig,
        CSearchEditBox__Draw_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    HMODULE hExplorerFrame = LoadLibraryExW(L"ExplorerFrame.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hExplorerFrame)
    {
        Wh_Log(L"Failed to load ExplorerFrame.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hExplorerFrame,
        explorerFrameDllHooks,
        ARRAYSIZE(explorerFrameDllHooks)
    ))
    {
        Wh_Log(L"Failed to hook CSearchEditBox::_Draw");
        return FALSE;
    }

    return TRUE;
}