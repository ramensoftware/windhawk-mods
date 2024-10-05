// ==WindhawkMod==
// @id           lm-mediakey-explorer-fix
// @name         Play-Media-Key fix in Explorer
// @description  Fix the Media 'play' key being suppressed in Explorer when a file is selected.
// @version      1.1
// @author       Mark Jansen
// @github       https://github.com/learn-more
// @twitter      https://twitter.com/learn_more
// @include      explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Play media key fix
When a file is selected in Explorer, and the 'play' media key is pressed (a physical key on some keyboards),
the play event never reaches a media player like Spotify.
This mod makes sure that the play event is not sent to the file, but to the media player instead.
*/
// ==/WindhawkModReadme==
#include <windhawk_utils.h>



WNDPROC pSHELLDLL_DefViewProc = NULL;
LRESULT CALLBACK SHELLDLL_DefViewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_APPCOMMAND && GET_APPCOMMAND_LPARAM(lParam) == APPCOMMAND_MEDIA_PLAY_PAUSE)
    {
        Wh_Log(L"Forwarding APPCOMMAND_MEDIA_PLAY_PAUSE");
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }
    return pSHELLDLL_DefViewProc(hWnd, uMsg, wParam, lParam);
}

#define IS_ATOM(x) (((ULONG_PTR)(x) > 0x0) && ((ULONG_PTR)(x) < 0x10000))
static ATOM (WINAPI *pSHELL32_RegisterClassW) (CONST WNDCLASSW *lpWndClass) = 0;
ATOM WINAPI SHELL32_RegisterClassW (CONST WNDCLASSW *lpWndClass)
{
    if (pSHELLDLL_DefViewProc == NULL &&
        lpWndClass &&
        !IS_ATOM(lpWndClass->lpszClassName) &&
        !wcsicmp(lpWndClass->lpszClassName, L"SHELLDLL_DefView"))
    {
        Wh_Log(L"Got SHELLDLL_DefView, hooking it!");
        Wh_SetFunctionHook((VOID*)lpWndClass->lpfnWndProc, (void*)SHELLDLL_DefViewProc, (void**)&pSHELLDLL_DefViewProc);
        Wh_ApplyHookOperations();
    }
    return pSHELL32_RegisterClassW(lpWndClass);
}


BOOL Wh_ModInit()
{
    // Check if the class is already registered (it's lazy-initialized on first use)
    WNDCLASSW SHELLDLL_DefView = {};
    BOOL bRet;
    if (!(bRet = GetClassInfoW(GetModuleHandleW(L"shell32.dll"), L"SHELLDLL_DefView", &SHELLDLL_DefView)))
    {
        Wh_Log(L"SHELLDLL_DefView not available yet, hooking SHELL32!RegisterClassW instead");
        WindhawkUtils::Wh_SetFunctionHookT(RegisterClassW, SHELL32_RegisterClassW, &pSHELL32_RegisterClassW);
    }
    else
    {
        Wh_Log(L"Hook WndProc");
        WindhawkUtils::Wh_SetFunctionHookT(SHELLDLL_DefView.lpfnWndProc, SHELLDLL_DefViewProc, &pSHELLDLL_DefViewProc);
    }

    return TRUE;
}
