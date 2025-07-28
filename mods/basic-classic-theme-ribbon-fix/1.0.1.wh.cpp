// ==WindhawkMod==
// @id              basic-classic-theme-ribbon-fix
// @name            Basic/Classic Theme Ribbon Fix
// @description     Remove the gap between the titlebar and the ribbon when using basic theme
// @version         1.0.1
// @author          aubymori
// @github          https://github.com/aubymori
// @include         mspaint.exe
// @include         wordpad.exe
// @include         explorer.exe
// @include         Windows Style Builder.exe
// @include         WindowsLiveWriter.exe
// @include         wlmail.exe
// @include         MovieMaker.exe
// @include         WLXPhotoGallery.exe
// @compilerOptions -luxtheme -DWINVER=0x0A00
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Basic/Classic Theme Ribbon Fix
Starting with Windows 8, UIRibbon does not consider DWM being off (basic/classic theme)
and will always add a vertical spacer for the DWM titlebar. This mod removes that and
makes UIRibbon look nice on basic/classic again.

## Notice
This mod injects into Windows Explorer, Paint, WordPad, Windows Style Builder, and the
Windows Live 2012 Essentials by default, which should cover most cases. If you use 
another application that uses the built-in UIRibbon framework, go to this mod's Advanced
tab and add it to the Custom process inclusion list.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/basic-classic-theme-ribbon-fix/before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/basic-classic-theme-ribbon-fix/after.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <uxtheme.h>

#ifdef _WIN64
#   define THISCALL_STR L"__cdecl"
#else
#   define THISCALL_STR L"__thiscall"
#endif

thread_local bool g_fReflowing = false;

using GetSystemMetricsForDpi_t = decltype(&GetSystemMetricsForDpi);
GetSystemMetricsForDpi_t GetSystemMetricsForDpi_orig;
int WINAPI GetSystemMetricsForDpi_hook(int nIndex, UINT dpi)
{
    if (g_fReflowing && (!IsAppThemed() || !IsCompositionActive()))
    {
        if (nIndex == SM_CYCAPTION
        || nIndex == SM_CYFRAME
        || nIndex == SM_CXPADDEDBORDER)
        {
            return 0;
        }
    }
    return GetSystemMetricsForDpi_orig(nIndex, dpi);
}

void (__thiscall *TBS_DockReflowAsNeeded_orig)(class TBS *);
void __thiscall TBS_DockReflowAsNeeded_hook(class TBS *pThis)
{
    g_fReflowing = true;
    TBS_DockReflowAsNeeded_orig(pThis);
    g_fReflowing = false;
}

const WindhawkUtils::SYMBOL_HOOK uiRibbonDllHooks[] = {
    {
        {
            L"private: void "
            THISCALL_STR
            L" TBS::DockReflowAsNeeded(void)"
        },
        &TBS_DockReflowAsNeeded_orig,
        TBS_DockReflowAsNeeded_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    HMODULE hUIRibbon = LoadLibraryExW(L"UIRibbon.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hUIRibbon)
    {
        Wh_Log(L"Failed to load UIRibbon.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hUIRibbon,
        uiRibbonDllHooks,
        ARRAYSIZE(uiRibbonDllHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in UIRibbon.dll");
        return FALSE;
    }

    Wh_SetFunctionHook(
        (void *)GetSystemMetricsForDpi,
        (void *)GetSystemMetricsForDpi_hook,
        (void **)&GetSystemMetricsForDpi_orig
    );

    return TRUE;   
}