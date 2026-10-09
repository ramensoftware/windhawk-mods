// ==WindhawkMod==
// @id              classic-selection-rectangle-color
// @name            Classic Selection Rectangle Color
// @description     XP styled selection rectangle color
// @version         1.0
// @author          xalejandro
// @github          https://github.com/tetawaves
// @include         *
// @compilerOptions -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Selection Rectangle Color
Makes the selection rectangle background color like it was in Windows XP. Does not work with DirectUI list view.

**Before**

![Before](https://i.imgur.com/5ImD1u6.png)

**After**

![After](https://i.imgur.com/oAGCiJr.png)

*/
// ==/WindhawkModReadme==

#include <commctrl.h>
#include <windhawk_utils.h>

#ifdef _WIN64
#    define THISCALL  __cdecl
#    define STHISCALL L"__cdecl"
#else
#    define THISCALL  __thiscall
#    define STHISCALL L"__thiscall"
#endif

COLORREF g_clrMenuHilight = 0;
thread_local bool g_fDrawMarquee = false;

#define    LIGHTENBYTE(percent, x) { x += (255 - x) * percent / 100;}
COLORREF GetBorderSelectColor(int iPercent, COLORREF clr)
{
    //BOOL fAllowDesaturation;
    BYTE r, g, b;

    // Doing this is less expensive than Luminance adjustment
    //fAllowDesaturation = FALSE;
    r = GetRValue(clr);
    g = GetGValue(clr);
    b = GetBValue(clr);
    // If all colors are above positive saturation, allow a desaturation
    /*if (r > 0xF0 && g > 0xF0 && b > 0xF0)
    {
        fAllowDesaturation = TRUE;
    }*/

    LIGHTENBYTE(iPercent, r);
    LIGHTENBYTE(iPercent, g);
    LIGHTENBYTE(iPercent, b);

    return RGB(r,g,b);
}

void GetMenuHilightColor()
{
    BOOL fFlatMenuMode = FALSE;
    SystemParametersInfo(SPI_GETFLATMENU, 0, (PVOID)&fFlatMenuMode, 0);

    if (fFlatMenuMode)
        g_clrMenuHilight = GetSysColor(COLOR_MENUHILIGHT);
    else
        g_clrMenuHilight = GetBorderSelectColor(60, GetSysColor(COLOR_HIGHLIGHT));
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT *puPtrLen) 
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
    HMODULE hShell32 = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    ACTCTXW actCtx = { sizeof(actCtx) };
    actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_HMODULE_VALID;
    actCtx.lpResourceName = MAKEINTRESOURCEW(124);
    actCtx.hModule = hShell32;
    HANDLE hActCtx = CreateActCtxW(&actCtx);
    ULONG_PTR ulCookie;
    ActivateActCtx(hActCtx, &ulCookie);
    HMODULE hComCtl = LoadLibraryExW(L"comctl32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
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

void (THISCALL *CLVDrawManager__PaintMarquee_orig)(void *, HDC, void *, RECT *);
void THISCALL CLVDrawManager__PaintMarquee_hook(void *pThis, HDC hdc, void *pv, RECT *prc)
{
    g_fDrawMarquee = true;
    CLVDrawManager__PaintMarquee_orig(pThis, hdc, pv, prc);
    g_fDrawMarquee = false;
}

LRESULT (THISCALL *CListView_WndProc_orig)(void *, HWND, UINT, WPARAM, LPARAM);
LRESULT THISCALL CListView_WndProc_hook(void *pThis, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_SYSCOLORCHANGE || msg == WM_SETTINGCHANGE)
        GetMenuHilightColor();
    return CListView_WndProc_orig(pThis, hwnd, msg, wParam, lParam);
}

COLORREF (__fastcall *FillRectClr_orig)(HDC hdc, RECT *lprect, COLORREF color);
COLORREF __fastcall FillRectClr_hook(HDC hdc, RECT *lprect, COLORREF color)
{
    if (g_fDrawMarquee)
        color = g_clrMenuHilight;   
    return FillRectClr_orig(hdc, lprect, color);
}

BOOL Wh_ModInit(void)
{
    HMODULE hComCtl = LoadComCtlModule();
    if (!hComCtl)
    {
        Wh_Log(L"Failed to load comctl32.dll");
        return FALSE;
    }

    GetMenuHilightColor();

    const WindhawkUtils::SYMBOL_HOOK comctl32DllHooks[] = 
    {
        {
            {
                L"private: void " STHISCALL " CLVDrawManager::_PaintMarquee(struct HDC__ *,void *,struct tagRECT const *)"
            },
            &CLVDrawManager__PaintMarquee_orig,
            CLVDrawManager__PaintMarquee_hook,
            false
        },
        {
            {
                #if _WIN64
                L"public: __int64 __cdecl CListView::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
                #else
                L"public: long __thiscall CListView::WndProc(struct HWND__ *,unsigned int,unsigned int,long)"
                #endif
            },
            &CListView_WndProc_orig,
            CListView_WndProc_hook,
            false
        },
        {
            {
                #ifdef _WIN64
                L"FillRectClr"
                #else
                L"_FillRectClr@12"
                #endif
            },
            &FillRectClr_orig,
            FillRectClr_hook,
            false
        }
    };

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
