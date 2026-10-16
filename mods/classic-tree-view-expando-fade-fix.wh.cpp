// ==WindhawkMod==
// @id              classic-tree-view-expando-fade-fix
// @name            Classic Tree View Expando Fade Fix
// @description     Fixes tree view expand and collapse buttons on the classic theme
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -lversion
// @license         BSD-3-Clause
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Tree View Expando Fade Fix
In newer versions of Windows 10, tree views with the extended style
`TVS_EX_FADEINOUTEXPANDOS` (most notably the Explorer navigation pane) fail to
render expandos in classic theme. This mod fixes that bug, restoring the previous behavior.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-tree-view-expando-fade-fix/before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-tree-view-expando-fade-fix/after.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <commctrl.h>
#include <shlwapi.h>
#include <uxtheme.h>

#ifdef _WIN64
    #define STDCALL_STR L"__cdecl"
#else
    #define STDCALL_STR L"__stdcall"
#endif

typedef DWORD BITBOOL;

typedef struct tagDPISCALEINFO
{
    UINT m_uDpiX;
    UINT m_uDpiY;
    bool m_fDPIAware : 1;
    bool m_fIsThemingEnabled : 1;
    bool m_fIsIgnoringDpiChanges : 1;
} DPISCALEINFO, *PDPISCALEINFO;

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

typedef struct tagISEARCHINFO
{
    int iIncrSearchFailed;
    WCHAR *pszCharBuf;
    int cbCharBuf;
    int ichCharBuf;
    DWORD timeLast;
    BOOL fReplaceCompChar;
} ISEARCHINFO, *PISEARCHINFO;

typedef struct tagTREE
{
    CCONTROLINFO ci;
    BITBOOL fHorz : 1;
    BITBOOL fVert : 1;
    BITBOOL fFocus : 1;
    BITBOOL fNameEditPending : 1;
    BITBOOL fRedraw : 1;
    BITBOOL fScrollWait : 1;
    BITBOOL fCreatedFont : 1;
    BITBOOL fNoDismissEdit : 1;
    BITBOOL fIndentSet : 1;
    BITBOOL fTrackSet : 1;
    BITBOOL fPlaceTooltip : 1;
    BITBOOL fCyItemSet : 1;
    BITBOOL fInsertAfter : 1;
    BITBOOL fRestoreOldDrop : 1;
    BITBOOL fThemedBk : 1;
    BITBOOL fAutoScrolling : 1;
    BITBOOL fVisibleExpandos : 1;
    BITBOOL fHasHotExpandoState : 1;
    BITBOOL fHasUserSetBkColor : 1;
    BITBOOL fStateImageListIsPublic : 1;
    HTREEITEM hRoot;
    HTREEITEM hCaret;
    HTREEITEM hSelRoot;
    HTREEITEM hDropTarget;
    HTREEITEM hOldDrop;
    HTREEITEM htiEdit;
    HTREEITEM hHot;
    HTREEITEM hHotButton;
    HTREEITEM hToolTip;
    HTREEITEM htiInsert;
    HTREEITEM htiSearch;
    HTREEITEM htiDrag;
    HDPA hdpaWatch;
    HIMAGELIST hImageList;
    HIMAGELIST himlState;
    // Added after build 14361. Its use in build 19041 matches a scaled
    // version of the normal image list; the private field name is unavailable.
    HIMAGELIST himlScaled;
    HCURSOR hCurHot;
    int iPuntChar;
    int cxState;
    int cyState;
    HBRUSH hbrBk;
    HFONT hFont;
    HFONT hFontHot;
    HFONT hFontBold;
    HFONT hFontBoldHot;
    HBITMAP hStartBmp;
    HBITMAP hBmp;
    HDC hdcBits;
    HTREEITEM hItemPainting;
    HANDLE hheap;
    HBRUSH hbrLine;
    HBRUSH hbrText;
    POINT ptCapture;
    COLORREF clrText;
    COLORREF clrBk;
    COLORREF clrim;
    COLORREF clrLine;
    COLORREF clrLineNonTheme;
    SHORT cxImage;
    SHORT cyImage;
    SHORT cxNativeImage;
    SHORT cyNativeImage;
    SHORT cyText;
    SHORT cyItem;
    SHORT cxBorder;
    SHORT cyBorder;
    SHORT cxIndent;
    SHORT cxWnd;
    SHORT cyWnd;
    SHORT cyTopMargin;
    USHORT cxMax;
    USHORT cFullVisible;
    SHORT xPos;
    UINT cShowing;
    UINT cItems;
    HTREEITEM hTop;
    UINT uMaxScrollTime;
    UINT uPixPerSec;
    UINT uUpdateTime;
    HWND hwndEdit;
    WNDPROC pfnEditWndProc;
    HWND hwndToolTips;
    WCHAR *pszTip;
    CHAR *pszTipA;
    ISEARCHINFO is;
    HTHEME hTheme;
    DWORD dwLastAccId;
    DWORD dwExStyle;
    // We don't need anything past dwExStyle.
} TREE, *PTREE;

void (__fastcall *TV_GetItem)(
    PTREE pTree,
    HTREEITEM hItem,
    UINT mask,
    TVITEMEXW *pItem);

BOOL (__fastcall *TV_RealDrawPlusMinus)(
    PTREE pTree,
    HTREEITEM hItem,
    HDC hdc,
    int x,
    int y,
    int xMid,
    int yMid,
    int c,
    BOOL fPlus,
    BOOL fHot);

void (__fastcall *TV_DrawPlusMinus_orig)(PTREE, HTREEITEM, HDC, int, int, int, int, int, BOOL, BOOL);
void __fastcall TV_DrawPlusMinus_hook(
    PTREE pTree,
    HTREEITEM hItem,
    HDC hdc,
    int x,
    int y,
    int xMid,
    int yMid,
    int c,
    BOOL fPlus,
    BOOL fHot)
{
    if ((pTree->dwExStyle & TVS_EX_FADEINOUTEXPANDOS) != 0
        && !pTree->hTheme)
    {
        if (pTree->fVisibleExpandos)
        {
            TVITEMEXW item = {};
            TV_GetItem(pTree, hItem, TVIF_CHILDREN, &item);
            if (item.cChildren)
            {
                TV_RealDrawPlusMinus(
                    pTree,
                    hItem,
                    hdc,
                    x,
                    y,
                    xMid,
                    yMid,
                    c,
                    fPlus,
                    fHot);
            }
        }

        return;
    }

    TV_DrawPlusMinus_orig(
        pTree,
        hItem,
        hdc,
        x,
        y,
        xMid,
        yMid,
        c,
        fPlus,
        fHot);
}

void (__fastcall *TV_ExpandoFadeStart_orig)(PTREE, BOOL);
void __fastcall TV_ExpandoFadeStart_hook(PTREE pTree, BOOL fFadeIn)
{
    BOOL fClassic = (pTree->dwExStyle & TVS_EX_FADEINOUTEXPANDOS) != 0
        && !pTree->hTheme;

    if ((pTree->dwExStyle & TVS_EX_FADEINOUTEXPANDOS) != 0)
    {
        pTree->fVisibleExpandos = fFadeIn != FALSE;
    }

    TV_ExpandoFadeStart_orig(pTree, fFadeIn);

    if (fClassic)
    {
        RedrawWindow(
            pTree->ci.hwnd,
            nullptr,
            nullptr,
            RDW_INVALIDATE | RDW_ERASE);
    }
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
            L" TV_GetItem(struct _TREE *,struct _TREEITEM *,unsigned int,struct tagTVITEMEXW *)"
        },
        &TV_GetItem
    },
    {
        {
            L"int "
            STDCALL_STR
            L" TV_RealDrawPlusMinus(struct _TREE *,struct _TREEITEM *,struct HDC__ *,int,int,int,int,int,int,int)"
        },
        &TV_RealDrawPlusMinus
    },
    {
        {
            L"void "
            STDCALL_STR
            L" TV_DrawPlusMinus(struct _TREE *,struct _TREEITEM *,struct HDC__ *,int,int,int,int,int,int,int)"
        },
        &TV_DrawPlusMinus_orig,
        TV_DrawPlusMinus_hook,
        false
    },
    {
        {
            L"void "
            STDCALL_STR
            L" TV_ExpandoFadeStart(struct _TREE *,int)"
        },
        &TV_ExpandoFadeStart_orig,
        TV_ExpandoFadeStart_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    HMODULE hComCtl = LoadComCtlModule();
    if (!hComCtl)
    {
        Wh_Log(L"Failed to load comctl32.dll version 6");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hComCtl,
        comctl32DllHooks,
        ARRAYSIZE(comctl32DllHooks)))
    {
        Wh_Log(L"Failed to hook one or more tree view functions in comctl32.dll");
        return FALSE;
    }

    return TRUE;
}
