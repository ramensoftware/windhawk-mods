// ==WindhawkMod==
// @id              classic-list-group-fix
// @name            Classic List Group Fix
// @description     Makes the appearance of list group headers in classic theme like Windows XP.
// @version         1.1.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic List Group Fix
This mod makes list group headers in classic theme look as they did in Windows XP.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-list-group-fix-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-list-group-fix-after.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hidecollapse: true
  $name: Hide collapse button
  $description: Hides the button that expands and collapses the list group. This is purely visual; you can still click where the button once was.
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <windhawk_utils.h>

#ifdef _WIN64
#    define STHISCALL L"__cdecl"
#else
#    define STHISCALL L"__thiscall"
#endif

bool g_fHideCollapse = false;

/* CListGroup::HeaderHeight causes crashes when allocating stack memory,
   so store icon font globally so it can access it. */
LOGFONTW g_lfIconFont = { 0 };

/* I can't for the life of me find an API to get this. */
bool GetIconFont(LOGFONTW *lplf)
{
    HKEY hKey;
    if (ERROR_SUCCESS != RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Control Panel\\Desktop\\WindowMetrics",
        NULL,
        KEY_READ,
        &hKey
    ))
        return false;

    DWORD cbData = sizeof(LOGFONTW);
    bool bSuccess = ERROR_SUCCESS == RegQueryValueExW(
        hKey, L"IconFont", NULL, NULL,
        (BYTE *)lplf, &cbData
    );
    RegCloseKey(hKey);
    return bSuccess;
}

INT_PTR (__thiscall *CListGroup_GetGroupRect_orig)(void *, LPRECT);
INT_PTR __thiscall CListGroup_GetGroupRect_hook(
    void  *pThis,
    LPRECT lprc
)
{
    INT_PTR res = CListGroup_GetGroupRect_orig(
        pThis, lprc
    );
    lprc->left = 0;

    if (lprc->top == 5)
    {
        OffsetRect(lprc, 0, -5);
    }
    return res;
}

void (__thiscall *CListGroup__PaintHeader_orig)(void *, ULONG, LPNMLVCUSTOMDRAW);
void __thiscall CListGroup__PaintHeader_hook(
    void            *pThis,
    ULONG            uFlags,
    LPNMLVCUSTOMDRAW lpcd
)
{
    lpcd->rcText.left = 6;
    lpcd->rcText.top++;

    GetIconFont(&g_lfIconFont);
    LOGFONTW lf;
    lf = g_lfIconFont;
    lf.lfWeight = FW_BOLD;
    lf.lfItalic = FALSE;
    HFONT hfIcon = CreateFontIndirectW(&lf);
    SelectObject(lpcd->nmcd.hdc, hfIcon);

    CListGroup__PaintHeader_orig(
        pThis, uFlags, lpcd
    );

    DeleteObject(hfIcon);
}

INT_PTR (__thiscall *CListGroup_HeaderHeight_orig)(void *);
INT_PTR __thiscall CListGroup_HeaderHeight_hook(
    void *pThis
)
{
    return -g_lfIconFont.lfHeight + 2;
}

void (__thiscall *CListGroup__PaintCollapse_orig)(void *, LPNMLVCUSTOMDRAW);
void __thiscall CListGroup__PaintCollapse_hook(
    void            *pThis,
    LPNMLVCUSTOMDRAW lpcd
)
{
    if (!g_fHideCollapse)
        CListGroup__PaintCollapse_orig(pThis, lpcd);
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
    || wcsstr(szPath, L"\\System32")
    || wcsstr(szPath, L"\\SysWOW64"))
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

const WindhawkUtils::SYMBOL_HOOK comctl32DllHooks[] = {
    {
        {
            L"public: int "
            STHISCALL
            L" CListGroup::GetGroupRect(struct tagRECT *)const "
        },
        &CListGroup_GetGroupRect_orig,
        CListGroup_GetGroupRect_hook,
        false
    },
    {
        {
            L"private: void "
            STHISCALL
            L" CListGroup::_PaintHeader(unsigned long,struct tagNMLVCUSTOMDRAW *)"
        },
        &CListGroup__PaintHeader_orig,
        CListGroup__PaintHeader_hook,
        false
    },
    {
        {
            L"public: int "
            STHISCALL
            L" CListGroup::HeaderHeight(void)const "
        },
        &CListGroup_HeaderHeight_orig,
        CListGroup_HeaderHeight_hook,
        false
    },
    {
        {
            L"private: void "
            STHISCALL
            L" CListGroup::_PaintCollapse(struct tagNMLVCUSTOMDRAW *)"
        },
        &CListGroup__PaintCollapse_orig,
        CListGroup__PaintCollapse_hook,
        false
    }
};

void Wh_ModSettingsChanged(void)
{
    g_fHideCollapse = Wh_GetIntSetting(L"hidecollapse");
}

BOOL Wh_ModInit(void)
{
    GetIconFont(&g_lfIconFont);
    g_fHideCollapse = Wh_GetIntSetting(L"hidecollapse");

    HMODULE hComCtl = LoadComCtlModule();
    if (!hComCtl)
    {
        Wh_Log(L"Failed to load comctl32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hComCtl,
        comctl32DllHooks,
        ARRAYSIZE(comctl32DllHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions");
        return FALSE;
    }

    return TRUE;
}