// ==WindhawkMod==
// @id              classic-taskdlg-fix
// @name            Classic Task Dialog Fix
// @description     Fixes task dialog buttons in classic theme
// @version         1.1.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Task Dialog Fix
In Windows 10, the buttons in task dialogs (and some other DirectUI dialogs)
are too small vertically. This mod fixes that, and makes them look how they did
in Windows 8.1 and before.

### This mod will only work on Windhawk v1.4 and greater.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/classic-taskdlg-fix-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/main/classic-taskdlg-fix-after.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

/* Calling conventions */
#ifdef _WIN64
#   define THISCALL  __cdecl
#   define STHISCALL L"__cdecl"
#else
#   define THISCALL  __thiscall
#   define STHISCALL L"__thiscall"
#endif

/* Only available in Windows 10 1607 and up. */ 
typedef UINT (WINAPI *GetSystemDpiForProcess_t)(HANDLE);
GetSystemDpiForProcess_t GetSystemDpiForProcess;

typedef LPSIZE (* THISCALL DirectUI_CCPushButton_GetContentSize_t)(void *, LPSIZE, int, void *, HDC);
DirectUI_CCPushButton_GetContentSize_t DirectUI_CCPushButton_GetContentSize_orig;
LPSIZE THISCALL DirectUI_CCPushButton_GetContentSize_hook(
    void   *pThis,
    LPSIZE  psz,
    int     i1,
    void   *pSurface,
    HDC     hDC       /* Not actually a real device context :/ */
)
{
    /* Get original metrics */
    DirectUI_CCPushButton_GetContentSize_orig(
        pThis, psz, i1, pSurface, hDC
    );

    int dpi;
    if (GetSystemDpiForProcess)
    {
        dpi = GetSystemDpiForProcess(GetCurrentProcess());
    }
    else
    {
        dpi = 96;
    }

    psz->cy += MulDiv(7, dpi, 96);

    return psz;
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

BOOL Wh_ModInit(void)
{
    HMODULE hComCtl32 = LoadComCtlModule();
    if (!hComCtl32)
    {
        Wh_Log(L"Failed to load comctl32.dll");
        return FALSE;
    }

    HMODULE hUser32 = LoadLibraryW(L"user32.dll");
    if (hUser32)
    {
        GetSystemDpiForProcess = (GetSystemDpiForProcess_t)GetProcAddress(hUser32, "GetSystemDpiForProcess");
    }

    WindhawkUtils::SYMBOL_HOOK hook = {
        {
            L"public: virtual struct tagSIZE "
            STHISCALL
            L" DirectUI::CCPushButton::GetContentSize(int,int,class DirectUI::Surface *)"
        },
        &DirectUI_CCPushButton_GetContentSize_orig,
        DirectUI_CCPushButton_GetContentSize_hook,
        false
    };

    if (!HookSymbols(hComCtl32, &hook, 1))
    {
        Wh_Log(L"Failed to hook DirectUI::CCPushButton::GetContentSize");
        return FALSE;
    }

    return TRUE;
}