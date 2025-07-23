// ==WindhawkMod==
// @id              uiribbon-insetting-fix
// @name            Fix Ribbon UI Insetting for Windows 10+
// @description     Fixes issues with inset elements in some applications, such as those using Ribbon UI.
// @version         1.0
// @author          Isabella Lulamoon (kawapure)
// @github          https://github.com/kawapure
// @twitter         https://twitter.com/kawaipure
// @homepage        https://kawapure.github.io/
// @include         mspaint.exe
// @include         explorer.exe
// @include         wordpad.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# UIRibbon Insetting Fix

This mod fixes problems with inset elements in the caption of UIRibbon (and maybe some other) applications in Windows 10.
This is particularly useful if you have a Windows 7 or 8 theme.

Previously, the caption icon would be pushed in due to a change in Windows 10's UxTheme module, and the caption buttons
would be measured incorrectly due to limitations in scope of popular DWM mods.

| Before | ![](https://raw.githubusercontent.com/kawapure/images/refs/heads/main/uiribbon-insetfix-before.png) |
|-|-|
| After | ![](https://raw.githubusercontent.com/kawapure/images/refs/heads/main/uiribbon-insetfix-after.png) |

----

Thanks to ImSwordQueen for helping me test the mod and for providing the images in the README.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- caption_button_preset: windows_7
  $name: Caption button width
  $options:
  - windows_10: Windows 10 (do not modify)
  - windows_8: Windows 8 (4.6 to caption height ratio)
  - windows_7: Windows 7 (5.0 to caption height ratio)
  - custom: Custom (specify below)
- custom_caption_button_width_ratio: 5.0
  $name: Custom caption button width ratio
  $description: Describes a custom caption button width ratio.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <dwmapi.h>

struct
{
    float flCaptionButtonWidth = 5.0f;
} g_settings;

#define SETTING_CAPTION_WIDTH_IGNORE (-1) // Windows 10 option

void LoadSettings()
{
    WindhawkUtils::StringSetting spCaptionPreset = 
        WindhawkUtils::StringSetting::make(L"caption_button_preset");
    
    if (wcscmp(spCaptionPreset.get(), L"windows_10") == 0)
    {
        g_settings.flCaptionButtonWidth = SETTING_CAPTION_WIDTH_IGNORE;
    }
    else if (wcscmp(spCaptionPreset.get(), L"windows_8") == 0)
    {
        g_settings.flCaptionButtonWidth = 4.6f;
    }
    else if (wcscmp(spCaptionPreset.get(), L"windows_7") == 0)
    {
        g_settings.flCaptionButtonWidth = 5.0f;
    }
    else if (wcscmp(spCaptionPreset.get(), L"custom") == 0)
    {
        g_settings.flCaptionButtonWidth = Wh_GetIntSetting(L"custom_caption_button_width_ratio");
    }
}

// Most of these are undocumented.
enum WINDOWCOMPOSITIONATTRIB : __int32
{
    WCA_UNDEFINED                   = 0x0,
    WCA_NCRENDERING_ENABLED         = 0x1,
    WCA_NCRENDERING_POLICY          = 0x2,
    WCA_TRANSITIONS_FORCEDISABLED   = 0x3,
    WCA_ALLOW_NCPAINT               = 0x4,
    WCA_CAPTION_BUTTON_BOUNDS       = 0x5,
    WCA_NONCLIENT_RTL_LAYOUT        = 0x6,
    WCA_FORCE_ICONIC_REPRESENTATION = 0x7,
    WCA_EXTENDED_FRAME_BOUNDS       = 0x8,
    WCA_HAS_ICONIC_BITMAP           = 0x9,
    WCA_THEME_ATTRIBUTES            = 0xA,
    WCA_NCRENDERING_EXILED          = 0xB,
    WCA_NCADORNMENTINFO             = 0xC,
    WCA_EXCLUDED_FROM_LIVEPREVIEW   = 0xD,
    WCA_VIDEO_OVERLAY_ACTIVE        = 0xE,
    WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 0xF,
    WCA_DISALLOW_PEEK               = 0x10,
    WCA_CLOAK                       = 0x11,
    WCA_CLOAKED                     = 0x12,
    WCA_ACCENT_POLICY               = 0x13,
    WCA_FREEZE_REPRESENTATION       = 0x14,
    WCA_EVER_UNCLOAKED              = 0x15,
    WCA_VISUAL_OWNER                = 0x16,
    WCA_LAST                        = 0x17,
};

typedef struct tagWINDOWCOMPOSITIONATTRIBDATA
{
    WINDOWCOMPOSITIONATTRIB Attrib;
    void* pvData;
    UINT cbData;
} WINDOWCOMPOSITIONATTRIBDATA;

// dwmapi!GetWindowCompositionAttribute
//
// We want to report that the caption button bounds are as they were in previous
// versions of Windows.
HRESULT (WINAPI *GetWindowCompositionAttribute_orig)(HWND hwnd, WINDOWCOMPOSITIONATTRIBDATA *pwcad);
HRESULT WINAPI GetWindowCompositionAttribute_hook(HWND hwnd, WINDOWCOMPOSITIONATTRIBDATA *pwcad)
{
    HRESULT hr = GetWindowCompositionAttribute_orig(hwnd, pwcad);

    if (pwcad->Attrib == WCA_CAPTION_BUTTON_BOUNDS && 
        g_settings.flCaptionButtonWidth != SETTING_CAPTION_WIDTH_IGNORE)
    {
        if (SUCCEEDED(hr))
        {
            RECT *prc = (RECT *)pwcad->pvData;

            prc->left = prc->right - (g_settings.flCaptionButtonWidth * GetSystemMetrics(SM_CYCAPTION));
        }
    }

    return hr;
}

// uxtheme!ClassicGetSystemMetrics
//
// This is called by _GetNcBtnMetrics with SM_CXFRAME in order to determine the width
// of the frame. This is hooked to nullify an expression.
bool g_fDisableClassicGetSystemMetrics = false;
int (WINAPI *ClassicGetSystemMetrics_orig)(int nIndex);
int WINAPI ClassicGetSystemMetrics_hook(int nIndex)
{
    if (g_fDisableClassicGetSystemMetrics && nIndex == SM_CXFRAME)
    {
        return 0;
    }

    return ClassicGetSystemMetrics_orig(nIndex);
}

// uxtheme!_GetNcBtnMetrics
//
// Behaviour added to this method in Windows 10 adds 2 * SM_CXFRAME if pncwm->fCustomFrame is
// true and the window is not maximised. This is responsible for the left-side indentation
// of the caption.
//
// The code responsible is:
//      fIsDefaultFrame = pncwm->fCustomFrame == 0;   // maybe dwm??
//      v104 = v106 - SystemMetrics - (_DWORD)v110;
//      if ( !fIsDefaultFrame )
//      {
//          if ( (pncwm->dwStyle & WS_MAXIMIZE) != 0 )
//              SystemMetrics = 0;
//          else
//              SystemMetrics = 2 * ClassicGetSystemMetrics(SM_CXFRAME);
//      }
//
// As you can see, we can get the desired effect by simply making ClassicGetSystemMetrics
// return 0 here: 2 * 0 = 0; no inset added.
//
// This function is __fastcall on 32-bit, and __cdecl (the default) on 64-bit. The FASTCALL
// macro that Microsoft provides is only set to __fastcall on i386, and blank otherwise,
// so this behaviour is ensured from the signature (although it's a little unclear).
BOOL (FASTCALL *_GetNcBtnMetrics_orig)(void *pncwm, void *pnctm, HICON hAppIcon, BOOL fCanClose);
BOOL FASTCALL _GetNcBtnMetrics_hook(void *pncwm, void *pnctm, HICON hAppIcon, BOOL fCanClose)
{
    g_fDisableClassicGetSystemMetrics = true;

    BOOL fOriginalResult = _GetNcBtnMetrics_orig(pncwm, pnctm, hAppIcon, fCanClose);

    g_fDisableClassicGetSystemMetrics = false;

    return fOriginalResult;
}

// uxtheme.dll
const WindhawkUtils::SYMBOL_HOOK c_uxthemeHooks[] = {
    {
        {
#ifdef _WIN64
            L"int __cdecl _GetNcBtnMetrics(struct _NCWNDMET *,struct _NCTHEMEMET const *,struct HICON__ *,int)",
#else
            L"int __stdcall _GetNcBtnMetrics(struct _NCWNDMET *,struct _NCTHEMEMET const *,struct HICON__ *,int)",
#endif
        },
        &_GetNcBtnMetrics_orig,
        _GetNcBtnMetrics_hook,
    },
    {
        {
#ifdef _WIN64
            L"ClassicGetSystemMetrics",
#else
            L"_ClassicGetSystemMetrics@4",
#endif
        },
        &ClassicGetSystemMetrics_orig,
        ClassicGetSystemMetrics_hook,
    },
};

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    LoadSettings();

    HMODULE hUxTheme = LoadLibraryExW(L"uxtheme.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (!WindhawkUtils::HookSymbols(hUxTheme, c_uxthemeHooks, ARRAYSIZE(c_uxthemeHooks)))
    {
        Wh_Log(L"Failed to hook one or more symbols in UxTheme.dll");
        return FALSE;
    }

    HMODULE hUser32 = LoadLibraryExW(L"user32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);

    FARPROC pfnGetWindowCompositionAttribute = GetProcAddress(hUser32, "GetWindowCompositionAttribute");

    if (!Wh_SetFunctionHook(
        (void *)pfnGetWindowCompositionAttribute,
        (void *)GetWindowCompositionAttribute_hook,
        (void **)&GetWindowCompositionAttribute_orig
    ))
    {
        Wh_Log(L"Failed to hook dwmapi!GetWindowCompositionAttribute.");
        return FALSE;
    }

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged()
{
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}
