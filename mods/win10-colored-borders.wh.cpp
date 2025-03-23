// ==WindhawkMod==
// @id              win10-colored-borders
// @name            Windows 10 Colored Borders
// @description     Makes active borders always use the accent color in Windows 10
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         dwm.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 10 Colored Borders
Starting from Windows 10 version 1809, the borders of active windows aren't colored
unless the user enables the option to color titlebars. This mod reverts that behavior
and makes active borders always use the user's accent color regardless.

## ⚠ Important usage note ⚠ 

In order to use this mod, you must allow Windhawk to inject into the **dwm.exe** 
system process. To do so, add it to the process inclusion list in the advanced 
settings. If you do not do this, it will silently fail to inject.

![Advanced settings screenshot](https://i.imgur.com/LRhREtJ.png)

**Preview**:

![Preview](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/win10-colored-borders-preview.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

void (*CGlassColorizationParameters_AdjustWindowColorization_orig)(void *, void *, float, DWORD);
void CGlassColorizationParameters_AdjustWindowColorization_hook(
    void *pThis,
    void *pgpcc,
    float flUnknown,
    DWORD dwFlags
)
{
    if (!(dwFlags & 8))
        dwFlags &= ~4;
    CGlassColorizationParameters_AdjustWindowColorization_orig(
        pThis, pgpcc, flUnknown, dwFlags
    );
}

const WindhawkUtils::SYMBOL_HOOK uDWMDllHooks[] = {
    {
        {
            L"public: void __cdecl CGlassColorizationParameters::AdjustWindowColorization(union GpCC const *,float,struct TMILFlagsEnum<enum ColorizationFlags::FlagsEnum>)"
        },
        &CGlassColorizationParameters_AdjustWindowColorization_orig,
        CGlassColorizationParameters_AdjustWindowColorization_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    HMODULE uDWM = LoadLibraryW(L"uDWM.dll");
    if (!uDWM)
    {
        Wh_Log(L"Failed to load uDWM.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        uDWM,
        uDWMDllHooks,
        ARRAYSIZE(uDWMDllHooks)
    ))
    {
        Wh_Log(L"Failed to hook CGlassColorizationParameters::AdjustWindowColorization");
        return FALSE;
    }

    return TRUE;
}