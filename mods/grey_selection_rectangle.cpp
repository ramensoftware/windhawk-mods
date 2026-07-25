// ==WindhawkMod==
// @id              grey-selection-rectangle
// @name            Grey Selection Rectangle
// @description     Makes the Windows selection rectangle dark grey with a white border
// @version         0.1
// @author          Aëven Augustin
// @include         *
// @github             https://github.com/AevenAugustin/WindHawk-Grey-Selection
// @compilerOptions -lgdi32 -luser32 -lversion

// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
XP-style selection rectangle:
- Dark gray fill
- White 1px border
- No transparency, no DWM effects
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <wingdi.h>
#include <commctrl.h>
#include <windhawk_utils.h>

#ifdef _WIN64
#    define THISCALL  __cdecl
#    define STHISCALL L"__cdecl"
#else
#    define THISCALL  __thiscall
#    define STHISCALL L"__thiscall"
#endif

// =====================
// XP colors
// =====================
COLORREF g_clrMarqueeFill   = RGB(90, 90, 90);     // Dark gray
COLORREF g_clrMarqueeBorder = RGB(255, 255, 255);  // White border

thread_local bool g_fDrawMarquee = false;

// =====================
// Version utils
// =====================
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
        *puPtrLen = uPtrLen;

    return (VS_FIXEDFILEINFO *)pFixedFileInfo;
}

// =====================
// Load comctl32 v6
// =====================
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

// =====================
// Hooks
// =====================
void (THISCALL *CLVDrawManager__PaintMarquee_orig)(void *, HDC, void *, RECT *);
void THISCALL CLVDrawManager__PaintMarquee_hook(void *pThis, HDC hdc, void *pv, RECT *prc)
{
    g_fDrawMarquee = true;
    CLVDrawManager__PaintMarquee_orig(pThis, hdc, pv, prc);
    g_fDrawMarquee = false;
}

COLORREF (__fastcall *FillRectClr_orig)(HDC hdc, RECT *lprect, COLORREF color);
COLORREF __fastcall FillRectClr_hook(HDC hdc, RECT *lprect, COLORREF color)
{
    if (g_fDrawMarquee)
    {
        // Fill
        HBRUSH hBrush = CreateSolidBrush(g_clrMarqueeFill);
        FillRect(hdc, lprect, hBrush);
        DeleteObject(hBrush);

        // Border
        HPEN hPen = CreatePen(PS_SOLID, 1, g_clrMarqueeBorder);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

        Rectangle(
            hdc,
            lprect->left,
            lprect->top,
            lprect->right,
            lprect->bottom
        );

        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hPen);

        return g_clrMarqueeFill;
    }

    return FillRectClr_orig(hdc, lprect, color);
}

// =====================
// Init
// =====================
BOOL Wh_ModInit(void)
{
    HMODULE hComCtl = LoadComCtlModule();
    if (!hComCtl)
    {
        Wh_Log(L"Failed to load comctl32.dll");
        return FALSE;
    }

    const WindhawkUtils::SYMBOL_HOOK hooks[] =
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

    if (!WindhawkUtils::HookSymbols(hComCtl, hooks, ARRAYSIZE(hooks)))
    {
        Wh_Log(L"Hook failed");
        return FALSE;
    }

    return TRUE;
}
