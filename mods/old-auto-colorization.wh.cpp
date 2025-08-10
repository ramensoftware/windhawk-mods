// ==WindhawkMod==
// @id              old-auto-colorization
// @name            Old Auto Colorization
// @description     Restores old auto colorization algorithms from before Windows 10 1809
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @compilerOptions -lshlwapi
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Old Auto Colorization
The algorithm used to calculate the color for wallpaper auto colorization changed
in Windows 10 1507, and again in Windows 10 1809. This mod restores the algorithms
from both Windows 8.x and Windows 10 1507-1803.

*Previews shown use Windows 8.1's default wallpaper to calculate the color.*

**Windows 8.x:**

![Windows 8.x](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/old-auto-colorization/win8.png)

**Windows 10 1507-1803:**

![Windows 10 1507-1803](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/old-auto-colorization/win10_1507.png)

**Windows 10 1809+:**

![Windows 10 1809+](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/old-auto-colorization/win10_1809.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: win10_1507
  $name: Auto colorization mode
  $description: Which OS should automatic colors be calculated like?
  $options:
  - win8: Windows 8.x
  - win10_1507: Windows 10 1507-1803
  - win10_1809: Windows 10 1809+
- no_change_immersive_color: false
  $name: Don't change immersive color
  $description: "Only change the DWM colorization color when applying an automatic color, like Windows 8.x.
    NOTE: Windows 10's DWM will use the immersive color for titlebars by default. You will need a third-party
    tool to make it use the DWM colorization color instead."
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <shlwapi.h>

enum class EAutoColorMode
{
    Win8 = 0,
    Win10_1507,
    Win10_1809
} g_eAutoColorMode;

bool g_fNoChangeImmersiveColor = false;

struct IMMERSIVE_COLOR_PREFERENCE
{
    COLORREF crStartColor;
    COLORREF crAccentColor;
};

HRESULT (*SetDwmColorizationColor)(COLORREF cr);

COLORREF (*StartMenuColor_ApplyLuminanceCap_orig)(COLORREF);
COLORREF StartMenuColor_ApplyLuminanceCap_hook(COLORREF cr)
{
    switch (g_eAutoColorMode)
    {
        case EAutoColorMode::Win8:
            return cr;
        case EAutoColorMode::Win10_1507:
        {
            // Slight modification from the original 1507 function, this used to read
            // from a registry key to determine the luminance cap (55). The registry key
            // does not exist by default, and nobody's documented it online, so I
            // think it's safe to say nobody cares to change the cap.
            WORD wHue = 0, wLuminance = 0, wSaturation = 0;
            ColorRGBToHLS(cr, &wHue, &wLuminance, &wSaturation);
            if (MulDiv(wLuminance, 100, 240) > 55)
                wLuminance = MulDiv(55, 240, 100);
            return ColorHLSToRGB(wHue, wLuminance, wSaturation);
        }
        case EAutoColorMode::Win10_1809:
            return StartMenuColor_ApplyLuminanceCap_orig(cr);
    }
}

void (*CDesktopBrowser__SetDWMAndImmersiveColor_orig)(void *, COLORREF);
void CDesktopBrowser__SetDWMAndImmersiveColor_hook(void *pThis, COLORREF cr)
{
    if (!g_fNoChangeImmersiveColor)
        return CDesktopBrowser__SetDWMAndImmersiveColor_orig(pThis, cr);

    Wh_Log(L"NOT setting start color");
    SetDwmColorizationColor(cr);
}

const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
    {
        {
            L"long __cdecl SetDwmColorizationColor(unsigned long,enum DWMPGLASSATTRIBUTE,int)"
        },
        &SetDwmColorizationColor,
        nullptr,
        false
    },
    {
        {
            L"unsigned long __cdecl StartMenuColor::ApplyLuminanceCap(unsigned long)"
        },
        &StartMenuColor_ApplyLuminanceCap_orig,
        StartMenuColor_ApplyLuminanceCap_hook,
        false
    },
    {
        {
            L"protected: void __cdecl CDesktopBrowser::_SetDWMAndImmersiveColor(unsigned long)"
        },
        &CDesktopBrowser__SetDWMAndImmersiveColor_orig,
        CDesktopBrowser__SetDWMAndImmersiveColor_hook,
        false
    }
};

void Wh_ModSettingsChanged()
{
    g_fNoChangeImmersiveColor = Wh_GetIntSetting(L"no_change_immersive_color");
    LPCWSTR pszSetting = Wh_GetStringSetting(L"mode");
    if (0 == wcscmp(pszSetting, L"win8"))
    {
        g_eAutoColorMode = EAutoColorMode::Win8;
    }
    else if (0 == wcscmp(pszSetting, L"win10_1507"))
    {
        g_eAutoColorMode = EAutoColorMode::Win10_1507;
    }
    else
    {
        g_eAutoColorMode = EAutoColorMode::Win10_1809;
    }
    Wh_FreeStringSetting(pszSetting);
}

BOOL Wh_ModInit(void)
{
    Wh_ModSettingsChanged();

    HMODULE hShell32 = LoadLibraryExW(L"shell32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hShell32,
        shell32DllHooks,
        ARRAYSIZE(shell32DllHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol function in shell32.dll");
        return FALSE;
    }

    return TRUE;
}