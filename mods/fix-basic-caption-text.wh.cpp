// ==WindhawkMod==
// @id              fix-basic-caption-text
// @name            Fix Basic Caption Text
// @description     Fixes non-DWM and UIRibbon frames having incorrect caption colors
// @version         1.1.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -luser32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fix Basic Caption Text
Since Windows 10, the caption colors on basic themed has been changed to adapt to dark mode
among other things. This mod reverts it to the old behavior of just showing the user-defined
system colors. It also affects the caption color of custom-drawn title text like UIRibbon's.

You can also optionally make basic windows use small icons instead of downscaling large icons.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/fix-basic-caption-text-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/main/fix-basic-caption-text-after.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- smallicons: true
  $name: Small window icons
  $description: Use small icons on basic theme windows instead of downscaling large icons
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <uxtheme.h>

#ifdef _WIN64
#   define SSTDCALL L"__cdecl"
#   define STHISCALL L"__cdecl"
#else
#   define SSTDCALL L"__stdcall"
#   define STHISCALL L"__thiscall"
#endif

bool g_bSmallIcons = true;

#define  _NCWNDMET_Active(pThis)  *((DWORD *)pThis + 21)

DWORD (__thiscall *_NCWNDMET__GetCaptionColor_orig)(LPVOID, bool);
DWORD __thiscall _NCWNDMET__GetCaptionColor_hook(LPVOID pThis, bool bDarkText)
{
    return GetSysColor(
        (_NCWNDMET_Active(pThis) != 1) ? COLOR_INACTIVECAPTIONTEXT : COLOR_CAPTIONTEXT
    );
}

/* Always use small icon */
typedef HICON (__fastcall *_GetWindowIcon_t)(HWND, BOOL);
_GetWindowIcon_t _GetWindowIcon_orig;
HICON __fastcall _GetWindowIcon_hook(HWND hWnd, BOOL bLarge)
{
    if (g_bSmallIcons)
        bLarge = FALSE;
    return _GetWindowIcon_orig(hWnd, bLarge);
}

const WindhawkUtils::SYMBOL_HOOK uxthemeDllHooks[] = { 
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

BOOL Wh_ModInit(void)
{
    HMODULE hUxTheme = LoadLibraryW(L"uxtheme.dll");
    if (!hUxTheme)
    {
        Wh_Log(L"Failed to load uxtheme.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hUxTheme,
        uxthemeDllHooks,
        ARRAYSIZE(uxthemeDllHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in uxtheme.dll");
        return FALSE;
    }
    
    return TRUE;
}