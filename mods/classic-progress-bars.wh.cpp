// ==WindhawkMod==
// @id              classic-progress-bars
// @name            Classic Progress Bars
// @description     Reverts progress bars to how they looked in Windows XP and before
// @version         1.0.1
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -lversion -luxtheme -lgdi32 -lpsapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Progress Bars
Before Windows Vista, progress bars looked different, commonly being broken up into
blocks rather than a smooth single bar. This mod restores that appearance of progress bars
from Windows XP and before.

# ⚠️ READ ME! Theme compatibility ⚠️
While the default Windows 10 theme works well with this mod, third-party
Windows XP-styled themes that use hacks to restore the old progress bar look
may (and probably will) have incompatibilities with this mod.

You should use [this utility](https://github.com/aubymori/ProgressBarTest/releases/latest)
to test progress bars with the mod before committing to using it system wide.

## For theme authors
Since this mod restores the rendering from XP, you should replicate the properties
from XP's `Progress` class closely. It should be noted that the `SourceShrink` property
from the `Bar` part should be replicated to the `Chunk` part, or else the horizontal
chunks will not render correctly.

It should also be noted that the old `Chunk` and `ChunkVert` parts will often use 
same images as the modern `Fill` and `FillVert` parts, so keep that in mind and try
to make sure the default Vista+ progress bar rendering doesn't break.

This mod has support for custom states on the `Chunk` and `ChunkVert` part of the `Progress`
class:
- 1 - Normal
- 2 - Error
- 3 - Paused

You should add these states to your theme to make error and paused progress bars visually
distinct from normal progress bars.

**Before**:

| ![Before (classic)](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-progress-bars/before-classic.png) | ![Before (classic, marquee)](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-progress-bars/before-marquee-classic.gif) |
|-|-|
| ![Before (themed)](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-progress-bars/before-themed.png) | ![Before (themed, marquee)](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-progress-bars/before-marquee-themed.gif) |

**After**:

| ![After (classic)](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-progress-bars/after-classic.png) | ![After (classic, marquee)](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-progress-bars/after-marquee-classic.gif) |
|-|-|
| ![After (themed)](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-progress-bars/after-themed.png) | ![After (themed, marquee)](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-progress-bars/after-marquee-themed.gif) |
*/
// ==/WindhawkModReadme==

#include <uxtheme.h>
#include <vssym32.h>
#include <commctrl.h>
#include <psapi.h>
#include <windhawk_utils.h>

#ifdef _WIN64
#   define STDCALL_STR  L"__cdecl"
#else
#   define STDCALL_STR  L"__stdcall"
#endif

#define RECTWIDTH(rc)  ((rc).right - (rc).left)
#define RECTHEIGHT(rc) ((rc).bottom - (rc).top)

#define MARQUEE_TIMER 1

ULONGLONG g_ulComCtlBase = 0;
ULONGLONG g_ulComCtlSize = 0;

/**
  * Some new states were added to progress bars in Windows Vista
  * that got put onto the Fill and FillVert parts but not the
  * Chunk and ChunkVert parts. Unfortunately, rendering is quite
  * buggy with just using the Fill and FillVert parts, so we can't
  * do that to achieve that appearance. Since this mod will be used
  * with custom themes anyway, I'll simply add custom states that theme
  * authors can support in their themes.
  */
enum CUSTOMCHUNKSTATES
{
    CCS_NORMAL = 1,
    CCS_ERROR = 2,
    CCS_PAUSED = 3,
    // NOTE: The fill parts have a "Partial" state
    // that appears to be light blue, but it is
    // never used by the ComCtl progress bar control.
};

struct DPISCALEINFO
{
    UINT m_uDpiX;
    UINT m_uDpiY;
    BOOL m_fDPIAware : 1;
    BOOL m_fIsThemingEnabled : 1;
    BOOL m_fIsIgnoringDpiChanges : 1;
};

struct PRO_DATA
{
    HWND hwnd;
    DWORD dwStyle;
    int iLow;
    int iHigh;
    int iPos;
    int iCurrPos;
    int iState;
    int iCurrState;
    int iStep;
    HFONT hfont;
    COLORREF clrBk;
    COLORREF clrBar;
    HTHEME hTheme;
    DWORD dwFlags;
    int iHighlightPos;
    int iFramesPerSecond;
    int iMarqueeMilliseconds;
    int iStartPos;
    int iDuration;
    DWORD dwStartTime;
    DWORD dwLastPaintTick;
    DPISCALEINFO dpi;
};

/* Adds back the removed iMarqueePos member.
   See HeapAlloc_hook for how the extra memory gets allocated. */
struct PRO_DATA_EX : PRO_DATA
{
    int iMarqueePos;
};

void ProGetPaintMetrics(PRO_DATA *ppd, RECT *prcClient, RECT *prc, int *pdxSpace, int *pdxBlock)
{
    int dxSpace, dxBlock;
    RECT rc;

    GetClientRect(ppd->hwnd, prcClient);

    if (ppd->hTheme)
    {
        int iPartBar = (ppd->dwStyle & PBS_VERTICAL)? PP_BARVERT : PP_BAR;
        GetThemeBackgroundContentRect(ppd->hTheme, NULL, iPartBar, 0, prcClient, &rc);
    }
    else
    {
        //  give 1 pixel around the bar
        rc = *prcClient;
        InflateRect(&rc, -1, -1);
    }

    if (ppd->dwStyle & PBS_VERTICAL)
        dxBlock = (rc.right - rc.left) * 2 / 3;
    else
        dxBlock = (rc.bottom - rc.top) * 2 / 3;

    dxSpace = 2;
    if (dxBlock == 0)
        dxBlock = 1;    // avoid div by zero

    if (ppd->dwStyle & PBS_SMOOTH) 
    {
        dxBlock = 1;
        dxSpace = 0;
    }

    if (ppd->hTheme)
    {
        int dx;
        if (SUCCEEDED(GetThemeInt(ppd->hTheme, 0, 0, TMT_PROGRESSCHUNKSIZE, &dx)))
        {
            dxBlock = dx;
        }

        if (SUCCEEDED(GetThemeInt(ppd->hTheme, 0, 0, TMT_PROGRESSSPACESIZE, &dx)))
        {
            dxSpace = dx;
        }
    }

    *prc = rc;
    *pdxSpace = dxSpace;
    *pdxBlock = dxBlock;
}

int GetProgressScreenPos(PRO_DATA *ppd, int iNewPos, RECT *pRect)
{
    int iStart, iEnd;
    if (ppd->dwStyle & PBS_VERTICAL)
    {
        iStart = pRect->top;
        iEnd = pRect->bottom;
    }
    else
    {
        iStart = pRect->left;
        iEnd = pRect->right;
    }
    return MulDiv(iEnd - iStart, iNewPos - ppd->iLow, ppd->iHigh - ppd->iLow);
}

void FillRectClr(HDC hdc, PRECT prc, COLORREF clr)
{
    COLORREF clrSave = SetBkColor(hdc, clr);
    ExtTextOut(hdc, 0, 0, ETO_OPAQUE, prc, NULL, 0, NULL);
    SetBkColor(hdc, clrSave);
}

void ProEraseBkgnd(PRO_DATA *ppd, HDC hdc, RECT* prcClient)
{
    COLORREF clrBk = ppd->clrBk;

    if (clrBk == CLR_DEFAULT)
        clrBk = GetSysColor(COLOR_BTNFACE);

    FillRectClr(hdc, prcClient, clrBk);
}

#define BLOCKSINMARQUEE 5
BOOL MarqueeShowBlock(int iBlock, int iMarqueeBlock, int nBlocks)
{
    int i;
    for (i = 0; i < BLOCKSINMARQUEE; i++)
    {
        if ((iMarqueeBlock + i - (BLOCKSINMARQUEE / 2)) % nBlocks == iBlock)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/* Re-allocate the PRO_DATA struct to include our extra field if
   necessary. */
PRO_DATA_EX *ReAllocProDataIfNecessary(PRO_DATA_EX *ppd)
{
    size_t cbProData = HeapSize(GetProcessHeap(), NULL, ppd);
    if (cbProData == sizeof(PRO_DATA_EX))
        return ppd;

    PRO_DATA_EX *ppdNew = (PRO_DATA_EX *)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ppd, sizeof(PRO_DATA_EX));
    if (!ppdNew)
        return ppd;

    SetWindowLongPtrW(ppdNew->hwnd, 0, (LONG_PTR)ppdNew);
    return ppdNew;
}

void (__fastcall *Progress_Paint_orig)(PRO_DATA_EX *, HDC);
void __fastcall Progress_Paint_hook(PRO_DATA_EX *ppd, HDC hdcIn)
{
    int x, dxSpace, dxBlock, nBlocks, i;
    HDC    hdc, hdcPaint, hdcMem = NULL;
    HBITMAP hbmpOld = NULL;
    RECT rc, rcClient;
    PAINTSTRUCT ps;
    HRESULT hr = E_FAIL;
    int iPart, iState;
    BOOL fShowBlock;

    ppd = ReAllocProDataIfNecessary(ppd);

    if (hdcIn == NULL)
    {
        hdc = hdcPaint = BeginPaint(ppd->hwnd, &ps);

        // Only make large enough for clipping region
        hdcMem = CreateCompatibleDC(hdc);
        if (hdcMem)
        {
            HBITMAP hMemBm = CreateCompatibleBitmap(hdc, RECTWIDTH(ps.rcPaint), RECTHEIGHT(ps.rcPaint));
            if (hMemBm)
            {
                hbmpOld = (HBITMAP)SelectObject(hdcMem, hMemBm);

                // Override painting DC with memory DC
                hdc = hdcMem;
            }
            else
                DeleteDC(hdcMem);
        }
    }
    else
        hdc = hdcIn;

    
    ProGetPaintMetrics(ppd, &rcClient, &rc, &dxSpace, &dxBlock);

    if (hdcMem)
    {
        // OffsetWindowOrgEx() doesn't work with the themes, need to change painting rects
        OffsetRect(&rcClient, -ps.rcPaint.left, -ps.rcPaint.top);
        OffsetRect(&rc, -ps.rcPaint.left, -ps.rcPaint.top);
    }

    x = GetProgressScreenPos(ppd, ppd->iPos, &rcClient);

    // Paint background and set up part/state
    if (ppd->hTheme)
    {
        int iPartBar = (ppd->dwStyle & PBS_VERTICAL)? PP_BARVERT : PP_BAR;
        iPart = (ppd->dwStyle & PBS_VERTICAL)? PP_CHUNKVERT: PP_CHUNK;

        iState = CCS_NORMAL;
        if (ppd->iState == PBST_ERROR)
            iState = CCS_ERROR;
        else if (ppd->iState == PBST_PAUSED)
            iState = CCS_PAUSED;

        DrawThemeBackground(ppd->hTheme, hdc, iPartBar, 0, &rcClient, 0);

        // Something changed between UXTheme v3 and v4 that causes this margin
        // to automatically apply in v3 but not v4.
        MARGINS mrg = { 0 };
        if (FAILED(GetThemeMargins(ppd->hTheme, hdc, iPartBar, 0, TMT_CONTENTMARGINS, nullptr, &mrg))
        || (!mrg.cxLeftWidth && !mrg.cxRightWidth && !mrg.cyTopHeight && !mrg.cyBottomHeight))
            GetThemeMargins(ppd->hTheme, hdc, iPartBar, 0, TMT_SIZINGMARGINS, nullptr, &mrg);
        rc.left += mrg.cxLeftWidth;
        rc.top += mrg.cyTopHeight;
        rc.right -= mrg.cxRightWidth;
        rc.bottom -= mrg.cyBottomHeight;
        rcClient.left += mrg.cxLeftWidth;
        rcClient.top += mrg.cyTopHeight;
        rcClient.right -= mrg.cxRightWidth;
        rcClient.bottom -= mrg.cyBottomHeight;
    }
    else
    {
        ProEraseBkgnd(ppd, hdc, &rcClient);
    }

    if (dxBlock == 1 && dxSpace == 0 && ppd->hTheme != NULL)
    {
        if (ppd->dwStyle & PBS_VERTICAL) 
            rc.top = x;
        else
            rc.right = x;

        hr = DrawThemeBackground(ppd->hTheme, hdc, iPart, iState, &rc, 0);
    }
    else
    {
        if (ppd->dwStyle & PBS_MARQUEE)
        {
            // Consider the full bar
            if (ppd->dwStyle & PBS_VERTICAL)
            {
                nBlocks = ((rc.bottom - rc.top) + (dxBlock + dxSpace) - 1) / (dxBlock + dxSpace); // round up
            }
            else
            {
                nBlocks = ((rc.right - rc.left) + (dxBlock + dxSpace) - 1) / (dxBlock + dxSpace); // round up
            }   

            ppd->iMarqueePos = (ppd->iMarqueePos + 1) % nBlocks;
        }
        else
        {
            nBlocks = (x + (dxBlock + dxSpace) - 1) / (dxBlock + dxSpace); // round up
        }

        for (i = 0; i < nBlocks; i++) 
        {
            if (ppd->dwStyle & PBS_VERTICAL) 
            {
                rc.top = rc.bottom - dxBlock;

                // are we past the end?
                if (rc.bottom <= rcClient.top)
                    break;

                if (rc.top <= rcClient.top)
                    rc.top = rcClient.top + 1;
            } 
            else 
            {
                rc.right = rc.left + dxBlock;

                // are we past the end?
                if (rc.left >= rcClient.right)
                    break;

                if (rc.right >= rcClient.right)
                    rc.right = rcClient.right - 1;
            }

            if (ppd->dwStyle & PBS_MARQUEE)
            {
                fShowBlock = MarqueeShowBlock(i, ppd->iMarqueePos, nBlocks);
            }
            else
            {
                fShowBlock = TRUE;
            }

            if (fShowBlock)
            {
                if (ppd->hTheme)
                {
                    hr = DrawThemeBackground(ppd->hTheme, hdc, iPart, iState, &rc, 0);
                }

                if (FAILED(hr))
                {
                    COLORREF cr = ppd->clrBar;
                    if (ppd->clrBar == CLR_DEFAULT)
                    {
                        cr = GetSysColor(COLOR_HIGHLIGHT);
                        if (ppd->iState == PBST_ERROR)
                            cr = RGB(255, 0, 0);
                        if (ppd->iState == PBST_PAUSED)
                            cr = RGB(255, 255, 0);
                    }

                    FillRectClr(hdc, &rc, cr);
                }
            }

            if (ppd->dwStyle & PBS_VERTICAL) 
            {
                rc.bottom = rc.top - dxSpace;
            } 
            else 
            {
                rc.left = rc.right + dxSpace;
            }
        }
    }

    if (hdcMem != NULL)
    {
        BitBlt(hdcPaint, ps.rcPaint.left, ps.rcPaint.top, RECTWIDTH(ps.rcPaint), RECTHEIGHT(ps.rcPaint),
            hdc, 0, 0, SRCCOPY);
        DeleteObject(SelectObject(hdcMem, hbmpOld));
        DeleteDC(hdcMem);
    }

    if (hdcIn == NULL)
        EndPaint(ppd->hwnd, &ps);
}

BOOL ProNeedsRepaint(PRO_DATA *ppd, int iOldPos)
{
    BOOL fRet = FALSE;
    RECT rc, rcClient;
    int dxSpace, dxBlock;
    int x, xOld;

    if (iOldPos != ppd->iPos)
    {
        ProGetPaintMetrics(ppd, &rcClient, &rc, &dxSpace, &dxBlock);

        x = GetProgressScreenPos(ppd, ppd->iPos, &rc);
        xOld = GetProgressScreenPos(ppd, iOldPos, &rc);

        if (x != xOld)
        {
            if (dxBlock == 1 && dxSpace == 0) 
            {
                fRet = TRUE;
            }
            else
            {
                int nBlocks, nOldBlocks;
                nBlocks = (x + (dxBlock + dxSpace) - 1) / (dxBlock + dxSpace); // round up
                nOldBlocks = (xOld + (dxBlock + dxSpace) - 1) / (dxBlock + dxSpace); // round up

                if (nBlocks != nOldBlocks)
                    fRet = TRUE;
            }
        }
    }
    return fRet;
}

int UpdatePosition(PRO_DATA *ppd, int iNewPos, BOOL bAllowWrap)
{
    int iOldPos = ppd->iPos;
    UINT uRedraw = RDW_INVALIDATE | RDW_UPDATENOW;
    BOOL fNeedsRepaint = TRUE;

    if (ppd->dwStyle & PBS_MARQUEE)
    {
        // Do an immediate repaint
        uRedraw |= RDW_ERASE;
    }
    else
    {
        if (ppd->iLow == ppd->iHigh)
            iNewPos = ppd->iLow;

        if (iNewPos < ppd->iLow) 
        {
            if (!bAllowWrap)
                iNewPos = ppd->iLow;
            else
                iNewPos = ppd->iHigh - ((ppd->iLow - iNewPos) % (ppd->iHigh - ppd->iLow));
        }
        else if (iNewPos > ppd->iHigh) 
        {
            if (!bAllowWrap)
                iNewPos = ppd->iHigh;
            else
                iNewPos = ppd->iLow + ((iNewPos - ppd->iHigh) % (ppd->iHigh - ppd->iLow));
        }

        // if moving backwards, erase old version
        if (iNewPos < iOldPos)
            uRedraw |= RDW_ERASE;

        ppd->iPos = iNewPos;
        fNeedsRepaint = ProNeedsRepaint(ppd, iOldPos);
    }

    if (fNeedsRepaint)
    {
        RedrawWindow(ppd->hwnd, NULL, NULL, uRedraw);
        NotifyWinEvent(EVENT_OBJECT_VALUECHANGE, ppd->hwnd, OBJID_CLIENT, 0);
    }

    return iOldPos;
}

void (__fastcall *Progress_SetMarqueeTimer_orig)(PRO_DATA_EX *, BOOL);
void __fastcall Progress_SetMarqueeTimer_hook(PRO_DATA_EX *ppd, BOOL fDoMarquee)
{
    ppd = ReAllocProDataIfNecessary(ppd);
    if (fDoMarquee)
        ppd->iMarqueePos = 0;
    Progress_SetMarqueeTimer_orig(ppd, fDoMarquee);
}

/* Disable the animation timer to prevent redundant redraws. */
bool (__fastcall *Progress_SetAnimateTimer_orig)(PRO_DATA *, BOOL);
bool __fastcall Progress_SetAnimateTimer_hook(PRO_DATA *ppd, BOOL fDoAnimation)
{
    return false;
}

/* Disable the highlight timer to prevent redundant redraws. */
void (__fastcall *Progress_SetHighlightTimer_orig)(PRO_DATA *, BOOL);
void __fastcall Progress_SetHighlightTimer_hook(PRO_DATA *ppd, BOOL fDoHighlight)
{
    return;
}

WNDPROC Progress_WndProc_orig;
LRESULT CALLBACK Progress_WndProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PRO_DATA *ppd = (PRO_DATA *)GetWindowLongPtrW(hWnd, 0);
    if (ppd && uMsg == WM_TIMER && wParam == MARQUEE_TIMER)
    {
        UpdatePosition(ppd, ppd->iPos, TRUE);
        return 0;
    }
    return Progress_WndProc_orig(hWnd, uMsg, wParam, lParam);
}

/* Allocate extra bytes for our extension of the PRO_DATA struct. */
LPVOID (WINAPI *HeapAlloc_orig)(HANDLE, DWORD, SIZE_T);
LPVOID WINAPI HeapAlloc_hook(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes)
{
    ULONGLONG ulRetAddr = (ULONGLONG)__builtin_return_address(0);
    if (dwBytes == sizeof(PRO_DATA)
    && (ulRetAddr >= g_ulComCtlBase) && (ulRetAddr < (g_ulComCtlBase + g_ulComCtlSize)))
    {
        dwBytes = sizeof(PRO_DATA_EX);
    }
    return HeapAlloc_orig(hHeap, dwFlags, dwBytes);
}

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
    {
        {
            L"void "
            STDCALL_STR
            L" Progress_Paint(struct PRO_DATA *,struct HDC__ *)"
        },
        &Progress_Paint_orig,
        Progress_Paint_hook,
        false
    },
    {
        {
            L"void "
            STDCALL_STR
            L" Progress_SetMarqueeTimer(struct PRO_DATA *,int)"
        },
        &Progress_SetMarqueeTimer_orig,
        Progress_SetMarqueeTimer_hook,
    },
    {
        {
            L"bool "
            STDCALL_STR
            L" Progress_SetAnimateTimer(struct PRO_DATA *,int)"
        },
        &Progress_SetAnimateTimer_orig,
        Progress_SetAnimateTimer_hook,
        false
    },
    {
        {
            L"void "
            STDCALL_STR
            L" Progress_SetHighlightTimer(struct PRO_DATA *,bool)"
        },
        &Progress_SetHighlightTimer_orig,
        Progress_SetHighlightTimer_hook,
        false
    },
    {
        {
#ifdef _WIN64
            L"__int64 __cdecl Progress_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
#else
            L"long __stdcall Progress_WndProc(struct HWND__ *,unsigned int,unsigned int,long)"
#endif
        },
        &Progress_WndProc_orig,
        Progress_WndProc_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    HMODULE hComCtl = LoadComCtlModule();
    if (!hComCtl)
    {
        Wh_Log(L"Failed to load comctl32.dll");
        return FALSE;
    }

    MODULEINFO mi;
    GetModuleInformation(GetCurrentProcess(), hComCtl, &mi, sizeof(mi));
    g_ulComCtlBase = (ULONGLONG)mi.lpBaseOfDll;
    g_ulComCtlSize = mi.SizeOfImage;

    if (!Wh_SetFunctionHook(
        (void *)HeapAlloc,
        (void *)HeapAlloc_hook,
        (void **)&HeapAlloc_orig
    ))
    {
        Wh_Log(L"Failed to hook HeapAlloc");
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