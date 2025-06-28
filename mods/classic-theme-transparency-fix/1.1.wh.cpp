// ==WindhawkMod==
// @id              classic-theme-transparency-fix
// @name            Classic theme transparency fix
// @description     Fixes transparency glitches in Classic theme
// @version         1.1
// @author          anixx
// @github          https://github.com/Anixx
// @include         *
// @include         dllhost.exe

// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod includes a subset of functionality of the mod "DWM Unextend frames", but only
includes fixes needed for Classic theme. It hooks less functions, is more lightweight, does not
use pointer arithmetic and the code is more compact.
It also injects dllhost.exe by default, which is needed for Windows Photo Viewer
and some dialogs.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

typedef HRESULT (WINAPI *DwmIsCompositionEnabled_t)(BOOL *);
DwmIsCompositionEnabled_t DwmIsCompositionEnabled_orig;
HRESULT WINAPI DwmIsCompositionEnabled_hook(BOOL *pfEnabled)
{
    *pfEnabled = FALSE;
    return S_OK;
}

typedef BOOL (WINAPI *IsCompositionActive_t)(void);
IsCompositionActive_t IsCompositionActive_orig;
BOOL WINAPI IsCompositionActive_hook()
{
    return FALSE;
}

BOOL Wh_ModInit()
{
    HMODULE dwmapiModule = LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    FARPROC pFunction = GetProcAddress(dwmapiModule, "DwmIsCompositionEnabled");
    WindhawkUtils::Wh_SetFunctionHookT((void*)pFunction, (void*)DwmIsCompositionEnabled_hook, (void**)&DwmIsCompositionEnabled_orig);
    HMODULE uxthemeModule = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    pFunction = GetProcAddress(uxthemeModule, "IsCompositionActive");
    WindhawkUtils::Wh_SetFunctionHookT((void*)pFunction, (void*)IsCompositionActive_hook, (void**)&IsCompositionActive_orig);
    return TRUE;
}
