// ==WindhawkMod==
// @id              classic-theme-transparency-fix
// @name            Classic theme transparency fix
// @description     Fixes transparency glitches in Classic theme
// @version         1.0
// @author          anixx
// @github          https://github.com/Anixx
// @include         *
// @include         dllhost.exe

// ==/WindhawkMod==

// ==WindhawkModReadme==

#include <windhawk_utils.h>

/*
This mod includes a subset of functionality of the mod "DWM Unextend frames", but only
includes fixes needed for Classic theme. It hooks less functions, is more lightweight, does not
use pointer arithmetic and the code is more compact.
It also injects dllhost.exe by default, which is needed for Windows Photo Viewer
and some dialogs.
*/
// ==/WindhawkModReadme==

typedef HRESULT (*DwmIsCompositionEnabled_t)(OUT BOOL *pfEnabled);
DwmIsCompositionEnabled_t DwmIsCompositionEnabled_orig;
HRESULT DwmIsCompositionEnabled_hook(BOOL *pfEnabled)
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
    HMODULE dwmapiModule = LoadLibrary(L"dwmapi.dll");
    FARPROC pFunction = GetProcAddress(dwmapiModule, "DwmIsCompositionEnabled");
    WindhawkUtils::Wh_SetFunctionHookT((void*)pFunction, (void*)DwmIsCompositionEnabled_hook, (void**)&DwmIsCompositionEnabled_orig);
    HMODULE uxthemeModule = LoadLibrary(L"uxtheme.dll");
    pFunction = GetProcAddress(uxthemeModule, "IsCompositionActive");
    WindhawkUtils::Wh_SetFunctionHookT((void*)pFunction, (void*)IsCompositionActive_hook, (void**)&IsCompositionActive_orig);
    return TRUE;
}
