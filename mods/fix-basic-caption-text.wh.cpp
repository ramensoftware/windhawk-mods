// ==WindhawkMod==
// @id              fix-basic-caption-text
// @name            Fix Basic Caption Text
// @description     Fixes non-DWM frames having hardcoded caption colors
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -luser32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fix Basic Caption Text
Since Windows 10, the caption color has been hardcoded due to different accent
colors needing dark or light text. For whatever reason, this also applies to basic
frames, and as such, they don't pull caption color from the theme anymore. This
results in incorrectly colored and possibly illegible caption text. This mod fixes that.

**This mod requires Windhawk v1.4 or greater.**

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/fix-basic-caption-text-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/main/fix-basic-caption-text-after.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- usethemedata: true
  $name: Use theme data
  $description: Use the theme data for caption color rather than system (classic theme) colors. Disable this if you experience incorrect colors.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#ifdef _WIN64
#   define STDCALL  __cdecl
#   define SSTDCALL L"__cdecl"

#   define THISCALL  __cdecl
#   define STHISCALL L"__cdecl"
#else
#   define STDCALL  __stdcall
#   define SSTDCALL L"__stdcall"

#   define THISCALL __thiscall
#   define STHISCALL L"__thiscall"
#endif

#include <uxtheme.h>
#include <vssym32.h>
#include <versionhelpers.h>
#include <windhawk_utils.h>

/* Use theme data or not */
BOOL g_bUseThemeData;

typedef DWORD (THISCALL *_NCWNDMET__GetCaptionColor_t)(LPVOID, bool);
_NCWNDMET__GetCaptionColor_t _NCWNDMET__GetCaptionColor_orig;
DWORD THISCALL _NCWNDMET__GetCaptionColor_hook(LPVOID pThis, bool bDarkText)
{
#ifdef _WIN64
    constexpr DWORD DEFAULT_INACTIVE_COLOR_DARK = RGB(99, 99, 99);
#else
    constexpr DWORD DEFAULT_INACTIVE_COLOR_DARK = RGB(98, 98, 98);
#endif
    constexpr DWORD DEFAULT_INACTIVE_COLOR_LIGHT = RGB(179, 184, 186);

    DWORD res = _NCWNDMET__GetCaptionColor_orig(pThis, bDarkText);
    DWORD dwCapClr;
	BOOL  bIsInactive = res == (bDarkText
    ? DEFAULT_INACTIVE_COLOR_DARK
    : DEFAULT_INACTIVE_COLOR_LIGHT);

    if (g_bUseThemeData)
    {
        HTHEME hTheme = OpenThemeData(NULL, L"Window");
        if (hTheme && S_OK == GetThemeColor(
            hTheme,
            WP_CAPTION,
            bIsInactive ? MB_INACTIVE : MB_ACTIVE,
            TMT_TEXTCOLOR,
            &dwCapClr
        ))
        {
            CloseThemeData(hTheme);
        }
        else
        {
            dwCapClr = GetSysColor(
                bIsInactive ? COLOR_INACTIVECAPTIONTEXT : COLOR_CAPTIONTEXT
            );
        }
    }
    else
    {
        dwCapClr = GetSysColor(
            bIsInactive ? COLOR_INACTIVECAPTIONTEXT : COLOR_CAPTIONTEXT
        );
    }
    
    return dwCapClr;
}

/* Always use small icon */
typedef HICON (__fastcall *_GetWindowIcon_t)(HWND, BOOL);
_GetWindowIcon_t _GetWindowIcon_orig;
HICON __fastcall _GetWindowIcon_hook(HWND hWnd, BOOL bLarge)
{
    return _GetWindowIcon_orig(hWnd, FALSE);
}

void LoadSettings(void)
{
    g_bUseThemeData = Wh_GetIntSetting(L"usethemedata");
}

BOOL Wh_ModInit(void)
{
    LoadSettings();

    HMODULE hUxTheme = LoadLibrary(L"uxtheme.dll");
    if (!hUxTheme)
    {
        Wh_Log(L"could not load uxtheme; probably unsupported by the application");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK hooks[] = { 
        {
            {
                L"public: unsigned long "
                STHISCALL
                L" _NCWNDMET::GetCaptionColor(bool)"
            },
            &_NCWNDMET__GetCaptionColor_orig,
            _NCWNDMET__GetCaptionColor_hook,
            false
        },
        {
            {
                L"struct HICON__ * "
                SSTDCALL
                L" _GetWindowIcon(struct HWND__ *,int)"
            },
            &_GetWindowIcon_orig,
            _GetWindowIcon_hook,
            false
        }
    };

    if (!WindhawkUtils::HookSymbols(
        hUxTheme,
        hooks,
        ARRAYSIZE(hooks)
    ))
    {
        Wh_Log(L"Failed to install hooks");
        return FALSE;
    }
    
    return TRUE;
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}