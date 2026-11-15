// ==WindhawkMod==
// @id              explorer-unlocked-toolbars-fix
// @name            Explorer Unlocked Toolbars Fix
// @description     Removes the splitter that shows up with unlocked toolbars in Explorer
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Unlocked Toolbars Fix
In Windows Vista, a splitter was added to the bottom of the Explorer toolbar band
site that renders when toolbars are unlocked. This mod removes that splitter.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/explorer-unlocked-toolbars-fix-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/explorer-unlocked-toolbars-fix-after.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <commctrl.h>

SUBCLASSPROC CInternetToolbar_CITBandSite_s_RebarSubclassWndProc_orig;
LRESULT CALLBACK CInternetToolbar_CITBandSite_s_RebarSubclassWndProc_hook(
    HWND      hWnd,
    UINT      uMsg,
    WPARAM    wParam,
    LPARAM    lParam,
    UINT_PTR  uIdSubclass,
    DWORD_PTR dwRefData
)
{
    if (uMsg == RB_SETEXTENDEDSTYLE)
        return 0;
    
    return CInternetToolbar_CITBandSite_s_RebarSubclassWndProc_orig(
        hWnd, uMsg, wParam, lParam, uIdSubclass, dwRefData
    );
}

const WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[] = {
    {
        {
            L"private: static __int64 __cdecl CInternetToolbar::CITBandSite::s_RebarSubclassWndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,unsigned __int64,unsigned __int64)"
        },
        &CInternetToolbar_CITBandSite_s_RebarSubclassWndProc_orig,
        CInternetToolbar_CITBandSite_s_RebarSubclassWndProc_hook,
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
        Wh_Log(L"Failed to hook CInternetToolbar::CITBandSite::s_RebarSubclassWndProc");
        return FALSE;
    }

    return TRUE;
}