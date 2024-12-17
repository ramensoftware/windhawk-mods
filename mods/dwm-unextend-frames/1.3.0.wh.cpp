// ==WindhawkMod==
// @id              dwm-unextend-frames
// @name            DWM Unextend Frames
// @description     Makes applications think DWM is disabled
// @version         1.3.0
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
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- secureonly: false
  $name: Apply to Secure Desktop only
  $description: Only the Secure Desktop (UAC, LogonUI) will behave like DWM is off
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <processthreadsapi.h>
#include <windhawk_utils.h>

#ifdef _WIN64
#   define THISCALL  __cdecl
#   define STHISCALL L"__cdecl"
#else
#   define THISCALL  __thiscall
#   define STHISCALL L"__thiscall"
#endif

/* Not defined for some reason */
#ifndef DWM_E_COMPOSITIONDISABLED
#   define DWM_E_COMPOSITIONDISABLED 0x80263001
#endif

bool g_bSecureOnly = false;

bool IsThreadDesktopSecure(void)
{
    HDESK hDesk = GetThreadDesktop(GetCurrentThreadId());
    if (hDesk)
    {
        WCHAR szDeskName[256] = { 0 };
        if (GetUserObjectInformationW(
            hDesk,
            UOI_NAME,
            &szDeskName,
            sizeof(szDeskName),
            nullptr
        ))
        {
            if (0 == wcsicmp(szDeskName, L"winlogon"))
            {
                return true;
            }
        }
    }
    return false;
}

/* uxtheme.dll hooks */

typedef BOOL (WINAPI *IsCompositionActive_t)(void);
IsCompositionActive_t IsCompositionActive_orig;
BOOL WINAPI IsCompositionActive_hook(void)
{
    return g_bSecureOnly ? !IsThreadDesktopSecure() : FALSE;
}

/* dwmapi.dll hooks */

typedef HRESULT (WINAPI *DwmIsCompositionEnabled_t)(BOOL *);
DwmIsCompositionEnabled_t DwmIsCompositionEnabled_orig;
HRESULT WINAPI DwmIsCompositionEnabled_hook(
    BOOL *pfEnabled
)
{
    HRESULT hr = DwmIsCompositionEnabled_orig(pfEnabled);
    if (SUCCEEDED(hr) && *pfEnabled)
    {
        *pfEnabled = g_bSecureOnly ? !IsThreadDesktopSecure() : FALSE;
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
    if (g_bSecureOnly && !IsThreadDesktopSecure())
    {
        return DwmExtendFrameIntoClientArea_orig(hWnd, pMarInset);
    }
    return DWM_E_COMPOSITIONDISABLED;
}

typedef HRESULT (WINAPI *DwmEnableBlurBehindWindow_t)(HWND, const DWM_BLURBEHIND *);
DwmEnableBlurBehindWindow_t DwmEnableBlurBehindWindow_orig;
HRESULT WINAPI DwmEnableBlurBehindWindow_hook(
          HWND            hWnd,
    const DWM_BLURBEHIND *pBlurBehind
)
{
    if (g_bSecureOnly && !IsThreadDesktopSecure())
    {
        return DwmEnableBlurBehindWindow_orig(hWnd, pBlurBehind);
    }
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
    if ((g_bSecureOnly && !IsThreadDesktopSecure())
    || dwAttribute != DWMWA_EXTENDED_FRAME_BOUNDS)
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
    if ((g_bSecureOnly && !IsThreadDesktopSecure())
    || pData->nAttr != 19)
    {
        return SetWindowCompositionAttribute_orig(
            hWnd, pData
        );
    }
    return TRUE;
}

/* comctl32.dll hooks */

#ifdef _WIN64
#   define DUIXmlParser_Composited(pThis) *((DWORD *)pThis + 27)
#else
#   define DUIXmlParser_Composited(pThis) *((DWORD *)pThis + 16)
#endif

HRESULT (THISCALL *DirectUI_DUIXmlParser__InitializeTables_orig)(void *);
HRESULT THISCALL DirectUI_DUIXmlParser__InitializeTables_hook(
    void *pThis
)
{
    DUIXmlParser_Composited(pThis) = g_bSecureOnly ? !IsThreadDesktopSecure() : FALSE;
    return DirectUI_DUIXmlParser__InitializeTables_orig(pThis);
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

#define HOOK_FUNCTION(MODULE, FUNCTION)                                     \
if (!WindhawkUtils::Wh_SetFunctionHookT(                                    \
    (FUNCTION ## _t)GetProcAddress(MODULE_VARNAME(MODULE), #FUNCTION),      \
    FUNCTION ## _hook,                                                      \
    &FUNCTION ## _orig                                                      \
))                                                                          \
{                                                                           \
    Wh_Log(L"Failed to hook " WSTR(FUNCTION) L" in " WSTR(MODULE) L".dll"); \
    return FALSE;                                                           \
}

#ifdef _WIN64
#   define PATHCACHE_VALNAME L"last-comctl32-v6-path"
#else
#   define PATHCACHE_VALNAME L"last-comctl32-v6-path-wow64"
#endif

#define COMCTL_582_SEARCH    L"microsoft.windows.common-controls_6595b64144ccf1df_5.82"

/* Load the ComCtl32 module */
HMODULE LoadComCtlModule(void)
{
    HMODULE hComCtl = LoadLibraryW(L"comctl32.dll");
    if (!hComCtl)
    {
        return NULL;
    }

    WCHAR szPath[MAX_PATH];
    GetModuleFileNameW(hComCtl, szPath, MAX_PATH);

    WCHAR szv6Path[MAX_PATH];
    BOOL bNoCache = FALSE;
    if (!Wh_GetStringValue(PATHCACHE_VALNAME, szv6Path, MAX_PATH))
    {
        bNoCache = TRUE;
    }

    /**
      * the !bNoCache check here is nested because we only want to fall through
      * to the cacher if the current comctl32 path is NOT 5.82.
      */
    if (wcsstr(szPath, COMCTL_582_SEARCH)
    || wcsstr(szPath, L"\\Windows\\System32")
    || wcsstr(szPath, L"\\Windows\\SysWOW64"))
    {
        if (!bNoCache)
        {
            hComCtl = LoadLibraryW(szv6Path);
        }
    }
    else if (bNoCache || wcsicmp(szPath, szv6Path))
    {
        Wh_SetStringValue(PATHCACHE_VALNAME, szPath);
    }

    return hComCtl;
}

const WindhawkUtils::SYMBOL_HOOK comctl32_hook = {
    {
        L"public: long "
        STHISCALL
        L" DirectUI::DUIXmlParser::_InitializeTables(void)"
    },
    &DirectUI_DUIXmlParser__InitializeTables_orig,
    DirectUI_DUIXmlParser__InitializeTables_hook,
    false
};

void LoadSettings(void)
{
    g_bSecureOnly = Wh_GetIntSetting(L"secureonly");
}

BOOL Wh_ModInit(void)
{
    LoadSettings();

    LOAD_MODULE(uxtheme)
    HOOK_FUNCTION(uxtheme, IsCompositionActive)

    LOAD_MODULE(dwmapi)
    HOOK_FUNCTION(dwmapi, DwmIsCompositionEnabled)
    HOOK_FUNCTION(dwmapi, DwmExtendFrameIntoClientArea)
    HOOK_FUNCTION(dwmapi, DwmEnableBlurBehindWindow)
    HOOK_FUNCTION(dwmapi, DwmSetWindowAttribute)

    LOAD_MODULE(user32)
    HOOK_FUNCTION(user32, SetWindowCompositionAttribute)

    HMODULE hComCtl = LoadComCtlModule();
    if (!hComCtl)
    {
        Wh_Log(L"Failed to load comctl32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hComCtl,
        &comctl32_hook,
        1
    ))
    {
        Wh_Log(L"Failed to hook DirectUI::DUIXmlParser::_InitializeTables in comctl32.dll");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}