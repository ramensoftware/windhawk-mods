// ==WindhawkMod==
// @id              dwm-unextend-frames
// @name            DWM Unextend Frames
// @description     Makes applications think DWM is disabled
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# DWM Unextend Frames
This mod makes applications think DWM is disabled when in reality it
is not. This is useful for anyone doing a setup with non-DWM frames
(Classic, XP, Vista/7 Basic).

**This mod requires Windhawk v1.4 or greater.**

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/dwm-unextend-frames-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/main/dwm-unextend-frames-after.png)

*Mod originally authored by Taniko Yamamoto.*
*/
// ==/WindhawkModReadme==

#include <dwmapi.h>
#include <windhawk_utils.h>

/* Not defined for some reason */
#ifndef DWM_E_COMPOSITIONDISABLED
#   define DWM_E_COMPOSITIONDISABLED 0x80263001
#endif

/* uxtheme.dll hooks */

typedef BOOL (WINAPI *IsCompositionActive_t)(void);
IsCompositionActive_t IsCompositionActive_orig;
BOOL WINAPI IsCompositionActive_hook(void)
{
    return FALSE;
}

/* dwmapi.dll hooks */

typedef HRESULT (WINAPI *DwmIsCompositionEnabled_t)(BOOL *);
DwmIsCompositionEnabled_t DwmIsCompositionEnabled_orig;
HRESULT WINAPI DwmIsCompositionEnabled_hook(
    BOOL *pfEnabled
)
{
    HRESULT hr = DwmIsCompositionEnabled_orig(pfEnabled);
    if (SUCCEEDED(hr))
    {
        *pfEnabled = FALSE;
    }
    return hr;
}

typedef HRESULT (WINAPI *DwmExtendFrameIntoClientArea_t)(HWND, const MARGINS *);
DwmExtendFrameIntoClientArea_t DwmExtendFrameIntoClientArea_orig;
HRESULT WINAPI DwmExtendFrameIntoClientArea_hook(
          HWND     hWnd,
    const MARGINS *pMarInset
)
{
    return DWM_E_COMPOSITIONDISABLED;
}

typedef HRESULT (WINAPI *DwmEnableBlurBehindWindow_t)(HWND, const DWM_BLURBEHIND *);
DwmEnableBlurBehindWindow_t DwmEnableBlurBehindWindow_orig;
HRESULT WINAPI DwmEnableBlurBehindWindow_hook(
          HWND            hWnd,
    const DWM_BLURBEHIND *pBlurBehind
)
{
    return DWM_E_COMPOSITIONDISABLED;
}

typedef HRESULT (WINAPI *DwmSetWindowAttribute_t)(HWND, DWORD, LPCVOID, DWORD);
DwmSetWindowAttribute_t DwmSetWindowAttribute_orig;
HRESULT WINAPI DwmSetWindowAttribute_hook(
    HWND    hWnd,
    DWORD   dwAttribute,
    LPCVOID pvAttribute,
    DWORD   cbAttribute
    )
{
    if (dwAttribute != DWMWA_EXTENDED_FRAME_BOUNDS)
    {
        return DwmSetWindowAttribute_orig(
            hWnd, dwAttribute, pvAttribute, cbAttribute
        );
    }
    return S_OK;
}

/* user32.dll hooks */

typedef struct tagWINCOMPATTRDATA
{
    int   nAttr;
    PVOID pvData;
    ULONG ulSize;
} WINCOMPATTRDATA, *LPWINCOMPATTRDATA;

typedef BOOL (WINAPI *SetWindowCompositionAttribute_t)(HWND, LPWINCOMPATTRDATA);
SetWindowCompositionAttribute_t SetWindowCompositionAttribute_orig;
BOOL WINAPI SetWindowCompositionAttribute_hook(
    HWND              hWnd,
    LPWINCOMPATTRDATA pData
)
{
    if (pData->nAttr != 19)
    {
        return SetWindowCompositionAttribute_orig(
            hWnd, pData
        );
    }
    return TRUE;
}

#define MODULE_VARNAME(NAME) hMod_ ## NAME
#define WSTR(VALUE) L ## #VALUE

#define LOAD_MODULE(NAME)                                        \
HMODULE MODULE_VARNAME(NAME) = LoadLibraryW(WSTR(NAME) L".dll"); \
if (!MODULE_VARNAME(NAME))                                       \
{                                                                \
    Wh_Log(L"Failed to load " WSTR(NAME) L".dll");               \
    return FALSE;                                                \
}

#define HOOK_FUNCTION(MODULE, FUNCTION)                                    \
if (!WindhawkUtils::Wh_SetFunctionHookT(                                   \
    (FUNCTION ## _t)GetProcAddress(MODULE_VARNAME(MODULE), #FUNCTION),     \
    FUNCTION ## _hook,                                                     \
    &FUNCTION ## _orig                                                     \
))                                                                         \
{                                                                          \
    Wh_Log(L"Failed to hook" WSTR(FUNCTION) L" in " WSTR(MODULE) L".dll"); \
    return FALSE;                                                          \
}

BOOL Wh_ModInit(void)
{
    LOAD_MODULE(uxtheme)
    HOOK_FUNCTION(uxtheme, IsCompositionActive)

    LOAD_MODULE(dwmapi)
    HOOK_FUNCTION(dwmapi, DwmIsCompositionEnabled)
    HOOK_FUNCTION(dwmapi, DwmExtendFrameIntoClientArea)
    HOOK_FUNCTION(dwmapi, DwmEnableBlurBehindWindow)
    HOOK_FUNCTION(dwmapi, DwmSetWindowAttribute)

    LOAD_MODULE(user32)
    HOOK_FUNCTION(user32, SetWindowCompositionAttribute)

    return TRUE;
}