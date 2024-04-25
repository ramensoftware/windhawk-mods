// ==WindhawkMod==
// @id              desktop-watermark-tweaks
// @name            Desktop Watermark Tweaks
// @description     Tweaks for the desktop watermark
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Watermark Tweaks
This mod lets you tweak certain things about the desktop watermark, like whether to display it
and what to display in the build section.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- display: true
  $name: Display watermark
  $description: Whether or not to display the desktop watermark.
- build: ""
  $name: Build string
  $description: String to display in place of build section. Leave blank for default.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

struct
{
    bool                         display;
    WindhawkUtils::StringSetting build;
} settings;

//public: static bool __cdecl CDesktopWatermark::s_WantWatermark(void)
bool (*CDesktopWatermark_s_WantWatermark_orig)(void);
bool CDesktopWatermark_s_WantWatermark_hook(void)
{
    return settings.display;
}

//private: static void __cdecl CDesktopWatermark::s_GetProductBuildString(unsigned short *,unsigned int)
void (*CDesktopWatermark_s_GetProductBuildString_orig)(LPWSTR);
void CDesktopWatermark_s_GetProductBuildString_hook(
    LPWSTR lpszOut
)
{
    LPCWSTR build = settings.build.get();
    if (build && *build != L'\0')
    {
        wcsncpy(lpszOut, build, 400);
        return;
    }
    CDesktopWatermark_s_GetProductBuildString_orig(lpszOut);
}

#define LoadIntSetting(NAME)    settings.NAME = Wh_GetIntSetting(L ## #NAME)
#define LoadStringSetting(NAME) settings.NAME = WindhawkUtils::StringSetting::make(L ## #NAME)

void LoadSettings(void)
{
    LoadIntSetting(display);
    LoadStringSetting(build);
}

const WindhawkUtils::SYMBOL_HOOK hooks[] = {
    {
        {
            L"public: static bool __cdecl CDesktopWatermark::s_WantWatermark(void)"
        },
        &CDesktopWatermark_s_WantWatermark_orig,
        CDesktopWatermark_s_WantWatermark_hook,
        false
    },
    {
        {
            L"private: static void __cdecl CDesktopWatermark::s_GetProductBuildString(unsigned short *,unsigned int)"
        },
        &CDesktopWatermark_s_GetProductBuildString_orig,
        CDesktopWatermark_s_GetProductBuildString_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    LoadSettings();

    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hShell32,
        hooks,
        ARRAYSIZE(hooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in shell32.dll");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}