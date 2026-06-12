// ==WindhawkMod==
// @id              fix-file-explorer-toolbar-position
// @name            File Explorer Toolbar Position Fix
// @description     Fixes issue with explorer toolbars not setting the position correctly
// @version         1.0
// @author          xalejandro
// @github          https://github.com/tetawaves
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# File Explorer Toolbar Position Fix
This mod fixes an issue with file explorer toolbars not setting the position correctly when opening explorer.

**Before**  
After opening explorer, all of the toolbars will be forced to be on a new line, ignoring the previous position and style.
![Before](https://i.imgur.com/v8bj787.png)

**After**  
Now the toolbars will respect the previous position and style when opening explorer.
![After](https://i.imgur.com/zGqzAfz.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

typedef struct tagBANDSAVE
{
    UINT wID;
    UINT fStyle;
    UINT cx;
} BANDSAVE, *PBANDSAVE;

thread_local UINT g_fShowBandCommonStyle = -1;
thread_local bool g_fShowBandCommon = false;

void (*CInternetToolbar__ShowBandCommon_orig)(void *, PBANDSAVE, void *, int);
void CInternetToolbar__ShowBandCommon_hook(void *pThis, PBANDSAVE pbs, void *cbid, int id)
{
    g_fShowBandCommon = true;
    g_fShowBandCommonStyle = pbs ? pbs->fStyle : -1;

    CInternetToolbar__ShowBandCommon_orig(pThis, pbs, cbid, id); 
    
    g_fShowBandCommon = false;
    g_fShowBandCommonStyle = -1;
}

using SendMessageW_t = decltype(&SendMessageW);
SendMessageW_t SendMessageW_orig;
LRESULT SendMessageW_hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (g_fShowBandCommon && Msg == RB_SETBANDINFO)
    {
        if (REBARBANDINFOW *rbbi = (REBARBANDINFO*)lParam; rbbi && rbbi->fMask & RBBIM_STYLE)
        {
            // the function CInternetToolbar::_ShowBandCommon ignores the previous saved style and forces RBBS_BREAK
            if (g_fShowBandCommonStyle != (UINT) -1)
                rbbi->fStyle = (rbbi->fStyle & ~RBBS_BREAK) | (g_fShowBandCommonStyle & RBBS_BREAK);
            else
                return 0;
        }
    }
    return SendMessageW_orig(hWnd, Msg, wParam, lParam);
}

BOOL Wh_ModInit() 
{
    Wh_Log(L"Init");

    HMODULE hExplorerFrame = LoadLibraryExW(L"explorerframe.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hExplorerFrame) 
    {
        Wh_Log(L"Failed to load explorerframe.dll");
        return FALSE;
    }
    
    WindhawkUtils::SYMBOL_HOOK explorerFrameDllHook
    {
        {
            L"protected: void __cdecl CInternetToolbar::_ShowBandCommon(struct tagBANDSAVE *,class CBandItemData *,int)"
        },
        &CInternetToolbar__ShowBandCommon_orig,
        CInternetToolbar__ShowBandCommon_hook,
        false
    };

    if (!WindhawkUtils::HookSymbols(hExplorerFrame, &explorerFrameDllHook, 1)) 
    {
        Wh_Log(L"Failed to hook explorerframe.dll");
        return FALSE;
    }

    return Wh_SetFunctionHook((void*)SendMessageW, (void*)SendMessageW_hook,
                       (void**)&SendMessageW_orig);
}
