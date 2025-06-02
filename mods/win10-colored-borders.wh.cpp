// ==WindhawkMod==
// @id              win10-colored-borders
// @name            Windows 10 Colorization Tweaks
// @description     Tweak the colorization of windows in Windows 10
// @version         1.1.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         dwm.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 10 Colorization Tweaks
This mod allows you to tweak the colorization of windows in Windows 10, including
always showing colored borders on active windows and always using dark or light mode
windows.

## ⚠ Important usage note ⚠ 

In order to use this mod, you must allow Windhawk to inject into the **dwm.exe** 
system process. To do so, add it to the process inclusion list in the advanced 
settings. If you do not do this, it will silently fail to inject.

![Advanced settings screenshot](https://i.imgur.com/LRhREtJ.png)

## Previews

**Colored borders**:

![Colored borders](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/win10-colored-borders-preview.png)

**Dark windows**:

![Dark windows](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/win10-colorization-tweaks-dark-window.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- colorborders: true
  $name: Always color borders
  $description: Always color active borders, regardless if titlebars are colored or not.
- windowcolor: normal
  $name: Window color
  $description: Which theme color to use for windows.
  $options:
  - normal: Normal
  - light: Light
  - dark: Dark
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

bool g_fColorBorders = true;

enum WINDOWCOLOR
{
    WC_NORMAL = 0,
    WC_LIGHT,
    WC_DARK
} g_eWindowColor = WC_NORMAL;

void (*CGlassColorizationParameters_AdjustWindowColorization_orig)(void *, void *, float, DWORD);
void CGlassColorizationParameters_AdjustWindowColorization_hook(
    void *pThis,
    void *pgpcc,
    float flUnknown,
    DWORD dwFlags
)
{
    if (g_fColorBorders && !(dwFlags & 0x8))
        dwFlags &= ~0x4;
    switch (g_eWindowColor)
    {
        case WC_NORMAL:
            break;
        case WC_LIGHT:
            dwFlags &= ~0x80;
            break;
        case WC_DARK:
            dwFlags |= 0x80;
            break;
    }
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

void Wh_ModSettingsChanged(void)
{
    g_fColorBorders = Wh_GetIntSetting(L"colorborders");

    LPCWSTR pszWindowColor = Wh_GetStringSetting(L"windowcolor");
    if (0 == wcscmp(pszWindowColor, L"light"))
        g_eWindowColor = WC_LIGHT;
    else if (0 == wcscmp(pszWindowColor, L"dark"))
        g_eWindowColor = WC_DARK;
    else
        g_eWindowColor = WC_NORMAL;
    Wh_FreeStringSetting(pszWindowColor);
}

BOOL Wh_ModInit(void)
{
    Wh_ModSettingsChanged();

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