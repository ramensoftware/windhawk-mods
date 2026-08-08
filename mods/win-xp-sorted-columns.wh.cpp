// ==WindhawkMod==
// @id              win-xp-sorted-columns
// @name            Windows XP Sorted Columns
// @description     Tints sorted listview columns like in XP
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @include         notepad.exe
// @compilerOptions -lversion
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows XP Sorted Columns
In Windows XP, list views in the Details view could show the sorted column in
a different color. This mod restores that behavior.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/win-xp-sorted-columns/before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/win-xp-sorted-columns/after.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <commctrl.h>

struct CCompositedDraw
{
    void  *__vftable;
    HWND   _hwnd;
    HDC    _hdcPaint;
    BOOL   _fCompositedDrawEnabled;
    BOOL   _fOpaque;
    BOOL   _fReleaseDC;
    HANDLE _hBufferedPaint;
    BOOL   _fBufferedPaintInit;
};

typedef DWORD BITBOOL;

struct DPISCALEINFO
{
    UINT m_uDpiX;
    UINT m_uDpiY;
    bool m_fDPIAware : 1;
    bool m_fIsThemingEnabled : 1;
    bool m_fIsIgnoringDpiChanges : 1;
};

typedef struct tagControlInfo
{
    HWND hwnd;
    HWND hwndParent;
    UINT style;
    DWORD dwCustom;
#ifdef _WIN64
    BOOL bUnicode;
    BOOL bInFakeCustomDraw;
#else
    BOOL bUnicode : 1;
    BOOL bInFakeCustomDraw : 1;
#endif
    UINT uiCodePage;
    DWORD dwExStyle;
    int iVersion;
    WORD wUIState;
    DPISCALEINFO dpi;
} CCONTROLINFO, *LPCCONTROLINFO;

struct CListView
{
    void            *__vftables0[3];
    CCompositedDraw  CCompositedDraw;
    void            *__vftable3;
    BITBOOL          _fWMCreate  : 1;
    BITBOOL          _fWMDestroy : 1;
    CCONTROLINFO     _ci;
    // continues but we don't need more fields
};

#define    SATURATEBYTE(percent, x)  { if (x + (percent * 10 * (x)) / 1000 > 0xFF) { if (fAllowDesaturation) x -= (x) / 30;  else x = 0xFF; } else x += (percent * 10 * (x)) / 1000; }
COLORREF GetSortColor(int iPercent, COLORREF clr)
{
    BOOL fAllowDesaturation;
    BYTE r, g, b;
    if (clr == 0) // Black huh?
    {
        return RGB(128,128,128);
    }

    // Doing this is less expensive than Luminance adjustment
    fAllowDesaturation = FALSE;
    r = GetRValue(clr);
    g = GetGValue(clr);
    b = GetBValue(clr);
    // If all colors are above positive saturation, allow a desaturation
    if (r > 0xF0 && g > 0xF0 && b > 0xF0)
    {
        fAllowDesaturation = TRUE;
    }

    SATURATEBYTE(iPercent, r);
    SATURATEBYTE(iPercent, g);
    SATURATEBYTE(iPercent, b);

    return RGB(r,g,b);
}

// CListView::GetSortColumnColor is inlined on x86, and
// CListView::OnSetSortColumnColor is inlined on x64. You love to see it...
//
// Hooking these functions in particular is the least nasty solution I could
// come up with.

#ifdef _WIN64

COLORREF (*CListView_GetSortColumnColor_orig)(CListView *);
COLORREF CListView_GetSortColumnColor_hook(CListView *pThis)
{
    return GetSortColor(10, ListView_GetBkColor(pThis->_ci.hwnd));
}

#else

void (__thiscall *CListView_OnSetSortColumnColor_orig)(CListView *, COLORREF);
void __thiscall CListView_OnSetSortColumnColor_hook(CListView *pThis, COLORREF clrSort)
{
    CListView_OnSetSortColumnColor_orig(pThis, GetSortColor(10, ListView_GetBkColor(pThis->_ci.hwnd)));
}

#endif

VS_FIXEDFILEINFO *GetModuleVersionInfo(HMODULE hModule, UINT *puPtrLen)
{
    void *pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResourceW(hModule, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION);
    if (hResource)
    {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal)
        {
            void *pData = LockResource(hGlobal);
            if (pData)
            {
                if (!VerQueryValueW(pData, L"\\", &pFixedFileInfo, &uPtrLen)
                || uPtrLen == 0)
                {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen)
    {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO *)pFixedFileInfo;
}

/**
  * Loads comctl32.dll, version 6.0.
  * This uses an activation context that uses shell32.dll's manifest
  * to load 6.0, even in apps which don't have the proper manifest for
  * it.
  */
HMODULE LoadComCtlModule(void)
{
    HMODULE hShell32 = LoadLibraryExW(L"shell32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    ACTCTXW actCtx = { sizeof(actCtx) };
    actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_HMODULE_VALID;
    actCtx.lpResourceName = MAKEINTRESOURCEW(124);
    actCtx.hModule = hShell32;
    HANDLE hActCtx = CreateActCtxW(&actCtx);
    ULONG_PTR ulCookie;
    ActivateActCtx(hActCtx, &ulCookie);
    HMODULE hComCtl = LoadLibraryExW(L"comctl32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    /**
      * Certain processes will ignore the activation context and load
      * comctl32.dll 5.82 anyway. If that occurs, just reject it.
      */
    VS_FIXEDFILEINFO *pVerInfo = GetModuleVersionInfo(hComCtl, nullptr);
    if (!pVerInfo || HIWORD(pVerInfo->dwFileVersionMS) < 6)
    {
        FreeLibrary(hComCtl);
        hComCtl = NULL;
    }
    DeactivateActCtx(0, ulCookie);
    ReleaseActCtx(hActCtx);
    FreeLibrary(hShell32);
    return hComCtl;
}

const WindhawkUtils::SYMBOL_HOOK comctl32DllHooks[] = {
#ifdef _WIN64
    {
        {
            L"public: unsigned long __cdecl CListView::GetSortColumnColor(void)"
        },
        &CListView_GetSortColumnColor_orig,
        CListView_GetSortColumnColor_hook,
        false
    },
#else
    {
        {
            L"public: void __thiscall CListView::OnSetSortColumnColor(unsigned long)"
        },
        &CListView_OnSetSortColumnColor_orig,
        CListView_OnSetSortColumnColor_hook,
        false
    },
#endif
};

BOOL Wh_ModInit(void)
{
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
        Wh_Log(L"Failed to hook one or more symbol functions in comctl32.dll");
        return FALSE;
    }

    return TRUE;
}