// ==WindhawkMod==
// @id              restore-win-xp-folder-thumbnails
// @name            Windows XP Folder Thumbnails
// @description     Makes the generated folder thumbnails in Windows Explorer look like they did in Windows XP.
// @version         1.0.0
// @author          neptune
// @include         explorer.exe
// @compilerOptions -lgdi32 -lshlwapi -lole32 -lgdi32 -lmsimg32 -loleaut32 -luuid -lpropsys
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows XP Folder Thumbnails

Brings back the 4 preview folder thumbnails from Windows XP.

## Setup

If you have an existing Windows XP theme on your system, it's very likely that a hack has been employed to prevent
folder thumbnails from showing up. This hack is a nonexistent logo image, which stops the thumbnail generation
algorithm. When you install this mod, you want to disable that.

Usually, a value called `Logo` is written to `HKEY_CURRENT_USER\Software\Classes\Local Settings\Software\Microsoft\Windows\Shell\Bags\AllFolders\Shell`.
If this value exists at that location, then delete it and restart Explorer. That should restore thumbnail generation.

## Clearing Thumbnail Cache

If you see thumbnails in the style of Windows 10 on some folders, then you need to clear the thumbnail cache. You can do
this easily with the "Clear Thumbnail/Icon Cache" tool provided by [NTMU](//get-ntmu.github.io).

## Credits

- [aubymori](//github.com/aubymori) for preliminary testing and development help.
- [AllieTheFox](//github.com/AllieTheFox) for prerelease testing and QA.
- Rain for prerelease testing and QA.
- Joshua (jb8h on Discord)
- Vruh (vruh2 github.com/kfh83) for his michael jackson impression that greatly inspired the creation of this modr
- Geography nerd testing
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <thumbcache.h>
#include <initguid.h>
#include <propsys.h>
#include <propvarutil.h>
#include <wrl.h>
#include <wil/resource.h>
#include <wil/result_macros.h>

DEFINE_GUID(IID_IExtractLogo, 0xd4029ec0, 0x920, 0x11d1, 0x9a, 0xb, 0x0, 0xc0, 0x4f, 0xc2, 0xd6, 0xc1);

using Microsoft::WRL::ComPtr;
using UniquePidl = wil::unique_any<ITEMIDLIST *, decltype(&ILFree), ILFree>;

// shorthand
#ifndef ATOMICRELEASE
#ifdef __cplusplus
#define ATOMICRELEASET(p, type) { if(p) { type* punkT=p; p=NULL; punkT->Release();} }
#else
#define ATOMICRELEASET(p, type) { if(p) { type* punkT=p; p=NULL; punkT->lpVtbl->Release(punkT);} }
#endif
#endif

#define ATOMICRELEASE(p) ATOMICRELEASET(p, IUnknown)

// If these values are modified, the logic in Extract() must be modified too.
#define SMALLEST_THUMBNAIL_WITH_4_PREVIEWS 96
#define MAX_MINIPREVIEWS_COLLECT 8 // collect more than we're going to show, just in case one fails
#define MAX_MINIPREVIEWS 4

typedef enum
{
    MINIPREVIEW_LAYOUT_1 = 0,
    MINIPREVIEW_LAYOUT_4 = 1,
} MINIPREVIEW_LAYOUT;

// The size of the mini-thumbnails for each thumbnail size. For each thumbnail
// size, there is a mini-thumbnail size for single layout and 2x2 layout.
const LONG kFolder120PreviewSize[] = { 104, 48 };
const LONG kFolder96PreviewSize[] = { 82, 40 };
const LONG kFolder80PreviewSize[] = { 69, 32 };

// These are the margins at which the mini-thumbnails appear within the main thumbnail.
// For thumbnails with only one large minipreview, we can just use x1,y1.
const LONG kFolder120PreviewOffsets[] = { 8, 64, 13, 67 }; // x1, x2, y1, y2
const LONG kFolder96PreviewOffsets[]  = { 7, 49, 11, 52 }; // x1, x2, y1, y2
const LONG kFolder80PreviewOffsets[]  = { 5, 42, 9,  45 }; // x1, x2, y1, y2

// The files that can serve as thumbnails for folders:
const LPCWSTR kFolderThumbnailPaths[] = { L"folder.jpg", L"folder.gif" };

// We always have four now.
MINIPREVIEW_LAYOUT _GetMiniPreviewLayout(SIZE size)
{
    return MINIPREVIEW_LAYOUT_4;
}

void FreeMiniPreviewPidls(LPITEMIDLIST apidlPreviews[], UINT cpidlPreviews)
{
    for (UINT u = 0; u < cpidlPreviews; u++)
    {
        ILFree(apidlPreviews[u]);
    }
}

/**
 * In: uLayout - The layout (1 or 4 mini previews)
 *     sizeRequested - The size of the thumbnail we are trying to generate 
 *
 * Out:
 * -psizeFolderBmp is set to
 * the size of the bitmap.
 *
 * -aptOrigins array is filled in with the locations of the n minipreviews
 * (note, aptOrigins is assumed to have MAX_MINIPREVIEWS cells)
 * The size of the minipreviews (square) is returned in pSizeMinipreview;
 */
void GetMiniPreviewLocations(MINIPREVIEW_LAYOUT uLayout, SIZE sizeRequested, SIZE *psizeFolderBmp, 
                             POINT aptOrigins[], SIZE *psizeMiniPreview)
{
    const LONG *pOffsets = nullptr;
    LONG lSize; // One of the standard sizes, that we have a folder bitmap for.
    LONG lSmallestDimension = std::min(sizeRequested.cx, sizeRequested.cy);

    if (lSmallestDimension > 96) // For stuff bigger than 96, we use the 120 size
    {
        lSize = 120;
        pOffsets = kFolder120PreviewOffsets;
        psizeMiniPreview->cx = psizeMiniPreview->cy = kFolder120PreviewSize[uLayout];
    }
    else if (lSmallestDimension > 80) // For stuff bigger than 80, but <= 96, we use the 96 size.
    {
        lSize = 96;
        pOffsets = kFolder96PreviewOffsets;
        psizeMiniPreview->cx = psizeMiniPreview->cy = kFolder96PreviewSize[uLayout];
    }
    else // For stuff <= 80, we use 80.
    {
        lSize = 80;
        pOffsets = kFolder80PreviewOffsets;
        psizeMiniPreview->cx = psizeMiniPreview->cy = kFolder80PreviewSize[uLayout];
    }

    psizeFolderBmp->cx = psizeFolderBmp->cy = lSize;

    aptOrigins[0].x = pOffsets[0];
    aptOrigins[0].y = pOffsets[2];
    aptOrigins[1].x = pOffsets[1];
    aptOrigins[1].y = pOffsets[2];
    aptOrigins[2].x = pOffsets[0];
    aptOrigins[2].y = pOffsets[3];
    aptOrigins[3].x = pOffsets[1];
    aptOrigins[3].y = pOffsets[3];
}

HBITMAP _CreateDIBSection(HDC h, int cx, int cy, RGBQUAD** pprgb)
{
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = cx;
    bi.bmiHeader.biHeight = cy;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    return CreateDIBSection(h, &bi, DIB_RGB_COLORS, (void**)pprgb, NULL, 0);
}

// Pre multiplies alpha channel
void PreProcessDIB(int cx, int cy, RGBQUAD* pargb)
{
    int cTotal = cx * cy;
    for (int i = 0; i < cTotal; i++)
    {
        RGBQUAD* prgb = &pargb[i];
        if (prgb->rgbReserved != 0)
        {
            prgb->rgbRed      = ((prgb->rgbRed   * prgb->rgbReserved) + 128) / 255;
            prgb->rgbGreen    = ((prgb->rgbGreen * prgb->rgbReserved) + 128) / 255;
            prgb->rgbBlue     = ((prgb->rgbBlue  * prgb->rgbReserved) + 128) / 255;
        }
        else
        {
            *((DWORD*)prgb) = 0;
        }
    }
}

/**
 * @brief Check if there's an alpha channel.
 */
BOOL HasAlpha(RECT rc, int cx, RGBQUAD *pArgb)
{
    for (int y = rc.top; y < rc.bottom; y++)
    {
        for (int x = rc.left; x < rc.right; x++)
        {
            int iOffset = y * cx;
            if (pArgb[x + iOffset].rgbReserved != 0)
                return TRUE;
        }
    }

    return FALSE;
}

#define IDI_FOLDER 4

void WINAPI SHFillRectClr(HDC hdc, LPRECT prc, COLORREF clr)
{
    COLORREF oldColor = SetBkColor(hdc, clr);
    ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, prc, nullptr, 0, nullptr);
    SetBkColor(hdc, oldColor);
}

/** In:
 *   fAlpha: Do we want the folder background to have an alpha channel?
 *   sizeFolderBmp: size of the thumbnail
 *
 *  Out:
 *   pIsAlpha: Did we get what we wanted, if we wanted an alpha channel?
 *             (e.g. we won't get it if we're in < 24bit mode.)
 */
HRESULT DrawMiniPreviewBackground(HDC hdc, SIZE sizeFolderBmp, BOOL fAlpha, BOOL *pfIsAlpha, RGBQUAD *prgb)
{
    HRESULT hr = E_FAIL;

    wil::unique_hmodule hShell32(LoadLibraryW(L"shell32.dll"));
    RETURN_LAST_ERROR_IF_NULL(hShell32);

    HICON hicon = (HICON)LoadImage(
        hShell32.get(),
        MAKEINTRESOURCE(IDI_FOLDER),
        IMAGE_ICON,
        sizeFolderBmp.cx,
        sizeFolderBmp.cy,
        0
    );

    if (hicon)
    {
        *pfIsAlpha = FALSE;
        if (fAlpha)
        {
            // Try to blt an alpha channel icon into the dc
            ICONINFO io;
            if (GetIconInfo(hicon, &io))
            {
                BITMAP bm;
                if (GetObject(io.hbmColor, sizeof(bm), &bm))
                {
                    if (32 == bm.bmBitsPixel)
                    {
                        HDC hdcSrc = CreateCompatibleDC(hdc);
                        if (hdcSrc)
                        {
                            HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcSrc, io.hbmColor);

                            BitBlt(hdc, 0, 0, sizeFolderBmp.cx, sizeFolderBmp.cy, hdcSrc, 0, 0, SRCCOPY);

                            // Preprocess the alpha
                            PreProcessDIB(sizeFolderBmp.cx, sizeFolderBmp.cy, prgb);

                            *pfIsAlpha = TRUE;
                            SelectObject(hdcSrc, hbmpOld);
                            DeleteDC(hdcSrc);
                        }   
                    }
                }

                DeleteObject(io.hbmColor);
                DeleteObject(io.hbmMask);
            }
        }

        if (!*pfIsAlpha)
        {
            // Didn't create an alpha bitmap
            // We're filling the background with background window color.
            RECT rc = { 0, 0, (long)sizeFolderBmp.cx + 1, (long)sizeFolderBmp.cy + 1};
            SHFillRectClr(hdc, &rc, GetSysColor(COLOR_WINDOW));

            // Then drawing the icon on top.
            DrawIconEx(hdc, 0, 0, hicon, sizeFolderBmp.cx, sizeFolderBmp.cy, 0, NULL, DI_NORMAL);

            // This may have resulted in an alpha channel - we need to know.  (If it
            // did, then when we add a nonalpha minibitmap to this main one, we need to restore
            // the nuked out alpha channel)
            // Check if we have alpha (prgb is the bits for the DIB of size sizeFolderBmp):
            rc.right = sizeFolderBmp.cx;
            rc.bottom = sizeFolderBmp.cy;
            *pfIsAlpha = HasAlpha(rc, sizeFolderBmp.cx, prgb);
        }

        DestroyIcon(hicon);
        hr = S_OK;
    }

    return hr;
}

#define VS_BAGSTR_EXPLORER L"Shell"

// exported from SHLWAPI as ordinal 494
HRESULT (*SHPropertyBag_ReadStr)(
    IPropertyBag *ppb,
    LPCWSTR pszPropName,
    LPWSTR pszBuffer,
    int cchBuffer) = nullptr;

bool DoesFolderContainLogo(LPCITEMIDLIST pidlFull)
{
    bool bRet = false;
    ComPtr<IPropertyBag> spPropBag;
    if (SUCCEEDED(SHGetViewStatePropertyBag(pidlFull, VS_BAGSTR_EXPLORER, SHGVSPB_PERUSER | SHGVSPB_PERFOLDER, IID_PPV_ARGS(&spPropBag))))
    {
        WCHAR szLogo[MAX_PATH];
        szLogo[0] = 0;

        if (SUCCEEDED(SHPropertyBag_ReadStr(spPropBag.Get(), TEXT("Logo"), szLogo, ARRAYSIZE(szLogo))) && szLogo[0])
        {
            bRet = true;
        }
    }
    return bRet;
}

#define IID_X_PPV_ARG(IType, X, ppType) IID_##IType, X, reinterpret_cast<void**>(static_cast<IType**>(ppType))

bool DoesFolderContainFolderJPG(IShellFolder *psf, LPCITEMIDLIST pidl)
{
    ComPtr<IShellFolder> spsfSubfolder;

    // SHBTO can deal with NULL psf, he turns it into psfDesktop
    RETURN_IF_FAILED(SHBindToObject(psf, pidl, nullptr, IID_PPV_ARGS(&spsfSubfolder)));

    for (auto i : kFolderThumbnailPaths)
    {
        DWORD dwFlags = (SFGAO_FILESYSTEM | SFGAO_FOLDER);
        LPITEMIDLIST pidlItem;
        if (SUCCEEDED(spsfSubfolder->ParseDisplayName(nullptr, nullptr, (LPOLESTR)i, nullptr, &pidlItem, &dwFlags)))
        {
            ILFree(pidlItem);
            if (SFGAO_FILESYSTEM == (dwFlags & (SFGAO_FILESYSTEM | SFGAO_FOLDER)))
            {
                return true;
            }
        }
    }

    return false;
}

//
//  COM Initialization is weird due to multithreaded apartments.
//
//  If this thread has not called CoInitialize yet, but some other thread
//  in the process has called CoInitialize with the COINIT_MULTITHREADED,
//  then that infects our thread with the multithreaded virus, and a
//  COINIT_APARTMENTTHREADED will fail.
//
//  In this case, we must turn around and re-init ourselves as
//  COINIT_MULTITHREADED to increment the COM refcount on our thread.
//  If we didn't do that, and that other thread decided to do a
//  CoUninitialize, that >secretly< uninitializes COM on our own thread
//  and we fall over and die.
//
HRESULT WINAPI SHCoInitialize()
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr))
    {
        hr = CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);
    }
    return hr;
}

inline void SHCoUninitialize(HRESULT hr)
{
    if (SUCCEEDED(hr))
    {
        CoUninitialize();
    }
}

//
// Helpers to banish STRRET's into the realm of darkness
//

HRESULT WINAPI DisplayNameOf(IShellFolder *psf, LPCITEMIDLIST pidl, DWORD flags, LPTSTR psz, UINT cch)
{
    *psz = 0;
    STRRET sr;
    HRESULT hr = psf->GetDisplayNameOf(pidl, flags, &sr);
    if (SUCCEEDED(hr))
        hr = StrRetToBuf(&sr, pidl, psz, cch);
    return hr;
}

// deals with goofyness of IShellFolder::GetAttributesOf() including 
//      in/out param issue
//      failures
//      goofy cast for 1 item case
//      masks off results to only return what you asked for

DWORD WINAPI SHGetAttributes(IShellFolder *psf, LPCITEMIDLIST pidl, DWORD dwAttribs)
{
    // like SHBindToObject, if psf is NULL, use absolute pidl
    LPCITEMIDLIST pidlChild;
    if (!psf)
    {
        SHBindToParent(pidl, IID_PPV_ARGS(&psf), &pidlChild);
    }
    else
    {
        psf->AddRef();
        pidlChild = pidl;
    }

    DWORD dw = 0;
    if (psf)
    {
        dw = dwAttribs;
        dw = (
            SUCCEEDED(psf->GetAttributesOf(
                1,
                (LPCITEMIDLIST *)&pidlChild, &dw)
            )
            ? (dwAttribs & dw)
            : 0
        );
    }

    if (psf)
    {
        psf->Release();
    }

    return dw;
}

// get the name and flags of an absolute IDlist
// in:
//      dwFlags     SHGDN_ flags as hints to the name space GetDisplayNameOf() function
//
// in/out:
//      *pdwAttribs (optional) return flags
HRESULT WINAPI SHGetNameAndFlags(LPCITEMIDLIST pidl, DWORD dwFlags, LPWSTR pszName, UINT cchName, DWORD *pdwAttribs)
{
    if (pszName)
    {
        *pszName = L'\0';
    }

    HRESULT hrInit = SHCoInitialize();

    ComPtr<IShellFolder> psf;
    LPCITEMIDLIST pidlLast;
    HRESULT hr = SHBindToParent(pidl, IID_PPV_ARGS(&psf), &pidlLast);
    if (SUCCEEDED(hr))
    {
        if (pszName)
        {
            hr = DisplayNameOf(psf.Get(), pidlLast, dwFlags, pszName, cchName);
        }

        if (SUCCEEDED(hr) && pdwAttribs)
        {
            *pdwAttribs = SHGetAttributes(psf.Get(), pidlLast, *pdwAttribs);
        }
    }

    SHCoUninitialize(hrInit);
    return hr;
}

bool _IsShortcutTargetACandidate(IShellFolder *psf, LPCITEMIDLIST pidlPreview, BOOL *pbTryCached)
{
    *pbTryCached = TRUE;

    ComPtr<IShellLink> psl;
    if (FAILED(psf->GetUIObjectOf(NULL, 1, &pidlPreview, IID_IShellLink, nullptr, (void **)&psl)))
    {
        return false;
    }

    UniquePidl pidlTarget;
    if (FAILED(psl->GetIDList(&pidlTarget)))
    {
        return false;
    }

    DWORD dwTargetFlags = SFGAO_FOLDER;
    if (FAILED(SHGetNameAndFlags(pidlTarget.get(), 0, nullptr, 0, &dwTargetFlags)))
    {
        return false;
    }

    // return true if its not a folder, or if the folder contains a logo
    // note that this is kinda like recursing into the below function again
    if (0 != (dwTargetFlags & SFGAO_FOLDER))
    {
        if (DoesFolderContainLogo(pidlTarget.get()) || DoesFolderContainFolderJPG(NULL, pidlTarget.get()))
        {
            // It's a logo folder, don't try the cached image.
            *pbTryCached = FALSE;
            return true;
        }
    }

    return false;
}

//
// ILCombine using Task allocator
//
HRESULT WINAPI SHILCombine(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2, LPITEMIDLIST *ppidlOut)
{
    *ppidlOut = ILCombine(pidl1, pidl2);
    return *ppidlOut ? S_OK : E_OUTOFMEMORY;
}

bool IsMiniPreviewCandidate(IShellFolder *psf, LPCITEMIDLIST pidl, BOOL *pbTryCached)
{
    DWORD dwAttr = SHGetAttributes(psf, pidl, SFGAO_FOLDER | SFGAO_LINK | SFGAO_FILESYSANCESTOR);
    *pbTryCached = TRUE; 

    // if its a folder, check and see if its got a logo
    // note that folder shortcuts will have both folder and link, and since we check folder first, we won't recurse into folder shortcuts
    // dont do anything unless pidl is a folder on a real filesystem (i.e. dont walk into zip/cab)
    if ((dwAttr & (SFGAO_FOLDER | SFGAO_FILESYSANCESTOR)) == (SFGAO_FOLDER | SFGAO_FILESYSANCESTOR))
    {
        UniquePidl pidlParent;
        if (SUCCEEDED(SHGetIDListFromObject(psf, &pidlParent)))
        {
            UniquePidl pidlFull;
            if (SUCCEEDED(SHILCombine(pidlParent.get(), pidl, &pidlFull)))
            {
                if (DoesFolderContainLogo(pidlFull.get()))
                {
                    *pbTryCached = FALSE;
                    return true;
                }
            }
        }

        // no logo image, check for a "folder.jpg"
        // if its not there, then don't display pidl as a mini-preview, as it would recurse and produce dumb-looking 1/16 scale previews 
        if (DoesFolderContainFolderJPG(psf, pidl))
        {
            *pbTryCached = FALSE;
            return true;
        }
    }
    else 
    {
        // Only if its not a link, or if its a link to a valid candidate, then we can get its extractor
        if (0 == (dwAttr & SFGAO_LINK) || 
            _IsShortcutTargetACandidate(psf, pidl, pbTryCached))
        {
            ComPtr<IExtractImage> spei;
            if (SUCCEEDED(psf->GetUIObjectOf(NULL, 1, &pidl, IID_X_PPV_ARG(IExtractImage, NULL, &spei))))
            {
                return true;
            }
        }
    }

    return false;
}

/**
 * @brief Creates the main rendering DC.
 *
 * @details
 *      We return the bits to the DIB section in the DC if asked for. We need
 *      this for preprocessing the alpha channel if one exists.
 */
HRESULT CreateMainRenderingDC(HDC *phdc, HBITMAP *phbmp, HBITMAP *phbmpOld, int cx, int cy, RGBQUAD **pprgb)
{
    HRESULT hr = E_OUTOFMEMORY;
    wil::unique_hdc_window shdc(GetDC(NULL));

    if (shdc)
    {
        *phdc = CreateCompatibleDC(shdc.get());
        if (*phdc)
        {
            RGBQUAD *prgbDummy;
            *phbmp = _CreateDIBSection(*phdc, cx, cy, &prgbDummy); 
            if (*phbmp)
            {
                *phbmpOld = (HBITMAP)SelectObject(*phdc, *phbmp);
                if (pprgb)
                    *pprgb = prgbDummy;
                hr = S_OK;
            }
            else
            {
                DeleteDC(*phdc);
            }
        }
    }

    return hr;
}

/**
 * @brief Unselects the bitmap and deletes the DC.
 */
void DestroyMainRenderingDC(HDC hdc, HBITMAP hbmpOld)
{
    if (hbmpOld)
        SelectObject(hdc, hbmpOld);
    DeleteDC(hdc);
}

/**
 * @brief Repair the alpha channel after blitting a non-alpha image.
 */
void RepairAlphaChannel(RECT rc, SIZE sizeBmp, RGBQUAD *pargb)
{
    for (int y = (sizeBmp.cy - rc.bottom); y < (sizeBmp.cy - rc.top); y++)
    {
        int iOffset = y * sizeBmp.cx;
        for (int x = rc.left; x < rc.right; x++)
        {
            pargb[x + iOffset].rgbReserved = 0xff;
        }
    }
}

/**
 * In
 *  hbmpSub - little bitmap that we're adding to the thumbnail bitmap.
 *  ptMargin - where we're adding it on the destination thumbnail bitmap.
 *  sizeDest - how big it needs to be on the destination thumbnail bitmap.
 *  sizeSource - how bit it is.
 *  fAlphaSource - does the bitmap we're adding have an alpha channel?
 *  fAlphaDest - does what we're adding it to, have an alpha channel?
 *  prgbDest - the bits of the destination bitmap - needed if we add a non-alpha bitmap
 *             to an alpha background, so we can reset the alpha.
 *  sizeFolderBmp - the size of the destination bitmap - need this along with prgbDest.
 */
HRESULT AddBitmap(HDC hdc, HBITMAP hbmpSub, POINT ptMargin, SIZE sizeDest, SIZE sizeSource, BOOL fAlphaSource, BOOL fAlphaDest, RGBQUAD *prgbDest, SIZE sizeFolderBmp)
{
    HRESULT hr = E_OUTOFMEMORY;

    HDC hdcFrom = CreateCompatibleDC(hdc);
    if (hdcFrom)
    {
        // Select the bitmap into the source hdc.
        HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcFrom, hbmpSub);
        if (hbmpOld)
        {
            // Adjust destination size to preserve aspect ratio
            SIZE sizeDestActual;
            if ((1000 * sizeDest.cx / sizeSource.cx) <      // 1000 -> float simulation
                (1000 * sizeDest.cy / sizeSource.cy))
            {
                // Keep destination width
                sizeDestActual.cy = sizeSource.cy * sizeDest.cx / sizeSource.cx;
                sizeDestActual.cx = sizeDest.cx;
                ptMargin.y += (sizeDest.cy - sizeDestActual.cy) / 2; // Center
            }
            else
            {
                // Keep destination height
                sizeDestActual.cx = sizeSource.cx * sizeDest.cy / sizeSource.cy;
                sizeDestActual.cy = sizeDest.cy;
                ptMargin.x += (sizeDest.cx - sizeDestActual.cx) / 2; // Center
            }

            // Now blit the image onto our folder background.
            // Three alpha possibilities:
            // Dest: no alpha, Src: no alpha -> the normal case
            // Dest: no alpha, Src: alpha -> one of the minipreviews is a logo-ized folder.
            // Dest: alpha, Src: no alpha -> we're a logoized folder being rendered as a minipreview in
            //                               the parent folder's thumbnail.

            // If we got back an alpha image, we need to alphablend it.
            if (fAlphaSource)
            {
                // XP would only ever get an alpha image back if it were to (wrongfully) draw a child folder thumbnail
                // on a parent folder. However, things changed since XP and now all thumbnails are alpha. As such, we
                // need to manually fill the background with white in order to ensure the correct appearance.
                RECT rc = { ptMargin.x, ptMargin.y, ptMargin.x + sizeDestActual.cx, ptMargin.y + sizeDestActual.cy };
                FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
                RepairAlphaChannel(rc, sizeFolderBmp, prgbDest);

                BLENDFUNCTION bf;
                bf.BlendOp = AC_SRC_OVER;
                bf.SourceConstantAlpha = 255;
                bf.AlphaFormat = AC_SRC_ALPHA;
                bf.BlendFlags = 0;
                if (AlphaBlend(hdc, ptMargin.x, ptMargin.y, sizeDestActual.cx, sizeDestActual.cy, hdcFrom, 0 ,0, sizeSource.cx, sizeSource.cy, bf))
                    hr = S_OK;
            }
            else
            {
                // Otherwise, just blt it.
                int iModeSave = SetStretchBltMode(hdc, HALFTONE);
                if (StretchBlt(hdc, ptMargin.x, ptMargin.y, sizeDestActual.cx, sizeDestActual.cy, hdcFrom, 0 ,0, sizeSource.cx, sizeSource.cy, SRCCOPY))
                    hr = S_OK;
                SetStretchBltMode(hdc, iModeSave);

                // Are we alpha'd?  We didn't have an alpha source, so where we blt'd it, we've
                // lost the alpha channel.  Restore it.
                if (fAlphaDest)
                {
                    // Set the alpha channel over where we just blt'd.
                    RECT rc = {ptMargin.x, ptMargin.y, ptMargin.x + sizeDestActual.cx, ptMargin.y + sizeDestActual.cy};
                    RepairAlphaChannel(rc, sizeFolderBmp, prgbDest);
                }
            }
            SelectObject(hdcFrom, hbmpOld);
        }
        DeleteDC(hdcFrom);
    }

    return hr;
}

DEFINE_GUID(IID_IAlphaThumbnailExtractor, 0x0F97F9D3, 0xA7E2, 0x4DB7, 0xA9,0xB4, 0xC5,0x40,0xBD,0x4B,0x80,0xA9);
MIDL_INTERFACE("0F97F9D3-A7E2-4db7-A9B4-C540BD4B80A9")
IAlphaThumbnailExtractor : public IUnknown
{
    STDMETHOD(RequestAlphaThumbnail)(THIS) PURE;
};
__CRT_UUID_DECL(IAlphaThumbnailExtractor, 0x0F97F9D3, 0xA7E2, 0x4DB7, 0xA9,0xB4, 0xC5,0x40,0xBD,0x4B,0x80,0xA9);

/**
 * @brief Manages the extract image (aka thumbnail) of a folder.
 */
class CFolderExtractImage
    : public IExtractImage2
    , public IPersistPropertyBag
    , public IAlphaThumbnailExtractor
    , public IRunnableTask
{
    LPCWSTR _GetImagePath(UINT cx);
    HRESULT _CreateWithMiniPreviews(IShellFolder *psf, const LPCITEMIDLIST *apidlPreviews, BOOL *abTryCached, UINT cpidlPreviews, MINIPREVIEW_LAYOUT uLayout, HBITMAP *phBmpThumbnail);
    HRESULT _FindMiniPreviews(LPITEMIDLIST apidlPreviews[], BOOL abTryCached[], UINT *cpidlPreviews);
    HRESULT _CreateThumbnailFromIconResource(HBITMAP* phBmpThumbnail, int res);

    ComPtr<IRunnableTask> _spRun;
    long _cRef = 1;

    WCHAR _szFolder[MAX_PATH];
    WCHAR _szLogo[MAX_PATH];
    WCHAR _szWideLogo[MAX_PATH];

    ComPtr<IShellFolder2> _spsf;

    SIZE _size;
    UniquePidl _spidl;
    ComPtr<IPropertyBag> _sppb;

    LONG _lState = IRTIR_TASK_NOT_RUNNING;
    bool _fAlpha = false;

    DWORD _dwPriority;
    DWORD _dwRecClrDepth;
    DWORD _dwExtractFlags;

public:
    // IUnknown
    STDMETHOD (QueryInterface)(REFIID riid, void **ppv);
    STDMETHOD_(ULONG, AddRef) ();
    STDMETHOD_(ULONG, Release) ();

    // IExtractImage2
    STDMETHOD (GetLocation)(LPWSTR pszPath, DWORD cch, DWORD *pdwPriority, const SIZE *prgSize, DWORD dwRecClrDepth, DWORD *pdwFlags);
    STDMETHOD (Extract)(HBITMAP *phbm);

    // IExtractImage2
    STDMETHOD (GetDateStamp)(FILETIME *pftDateStamp);

    // IPersist
    STDMETHOD(GetClassID)(CLSID *pClassID);

    // IPersistPropertyBag
    STDMETHOD(InitNew)();
    STDMETHOD(Load)(IPropertyBag *ppb, IErrorLog *pErr);
    STDMETHOD(Save)(IPropertyBag *ppb, BOOL fClearDirty, BOOL fSaveAll)
    {
        return E_NOTIMPL;
    }

    // IRunnableTask
    STDMETHOD (Run)(void);
    STDMETHOD (Kill)(BOOL fWait);
    STDMETHOD (Suspend)(void);
    STDMETHOD (Resume)(void);
    STDMETHOD_(ULONG, IsRunning)(void);

    // IAlphaThumbnailExtractor
    STDMETHOD (RequestAlphaThumbnail)(void);

    STDMETHOD(Init)(IShellFolder *psf, LPCITEMIDLIST pidl);
};

HRESULT WINAPI CFolderExtractImage_Create(IShellFolder *psf, LPCITEMIDLIST pidl, REFIID riid, void **ppv)
{
    HRESULT hr = E_OUTOFMEMORY;
    CFolderExtractImage *pfei = new CFolderExtractImage();
    if (pfei)
    {
        hr = pfei->Init(psf, pidl);
        if (SUCCEEDED(hr))
            hr = pfei->QueryInterface(riid, ppv);
        pfei->Release();
    }
    return hr;
}

STDMETHODIMP CFolderExtractImage::QueryInterface(REFIID riid, void **ppv) noexcept
{
    static const QITAB qit[] = {
        QITABENT      (CFolderExtractImage, IExtractImage2),
        QITABENTMULTI (CFolderExtractImage, IExtractImage,         IExtractImage2),
        QITABENTMULTI2(CFolderExtractImage, IID_IExtractLogo,      IExtractImage2),
        QITABENT      (CFolderExtractImage, IPersistPropertyBag),
        QITABENT      (CFolderExtractImage, IRunnableTask),
        QITABENT      (CFolderExtractImage, IAlphaThumbnailExtractor),
        QITABENTMULTI (CFolderExtractImage, IPersist,              IPersistPropertyBag),
        { 0 },
    };
    return QISearch(this, qit, riid, ppv);
}

STDMETHODIMP_(ULONG) CFolderExtractImage::AddRef() noexcept
{
    return InterlockedIncrement(&_cRef);
}

STDMETHODIMP_(ULONG) CFolderExtractImage::Release() noexcept
{
    if (InterlockedDecrement(&_cRef))
        return _cRef;

    // NOLINTNEXTLINE(*delete-non-abstract-non-virtual-dtor): XP behavior.
    delete this;
    return 0;
}

STDMETHODIMP CFolderExtractImage::GetDateStamp(FILETIME *pftDateStamp) noexcept
{
    HANDLE h = CreateFile(_szFolder, GENERIC_READ,
                          FILE_SHARE_READ | FILE_SHARE_DELETE, NULL,
                          OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    HRESULT hr = (h != INVALID_HANDLE_VALUE) ? S_OK : E_FAIL;
    if (SUCCEEDED(hr))
    {
        hr = GetFileTime(h, NULL, NULL, pftDateStamp) ? S_OK : E_FAIL;
        CloseHandle(h);
    }
    return hr;
}

HRESULT CFolderExtractImage::InitNew() noexcept
{
    RETURN_IF_FAILED(SHGetViewStatePropertyBag(_spidl.get(), VS_BAGSTR_EXPLORER, SHGVSPB_PERUSER | SHGVSPB_PERFOLDER, IID_PPV_ARGS(&_sppb)));
    return S_OK;
}

HRESULT CFolderExtractImage::Load(IPropertyBag *ppb, IErrorLog *pErr) noexcept
{
    _sppb = ppb;
    return S_OK;
}

LPCTSTR CFolderExtractImage::_GetImagePath(UINT cx)
{
    if (!_szLogo[0])
    {
        if (_sppb && SUCCEEDED(SHPropertyBag_ReadStr(_sppb.Get(), TEXT("Logo"), _szLogo, ARRAYSIZE(_szLogo))) && _szLogo[0])
        {
            // The PathCombine calls here are for relative path support.

            if (SUCCEEDED(SHPropertyBag_ReadStr(_sppb.Get(), TEXT("WideLogo"), _szWideLogo, ARRAYSIZE(_szWideLogo))) && _szWideLogo[0])
            {
                PathCombine(_szWideLogo, _szFolder, _szWideLogo);
            }

            PathCombine(_szLogo, _szFolder, _szLogo);
        }
        else
        {
            WCHAR szFind[MAX_PATH];

            for (auto i : kFolderThumbnailPaths)
            {
                PathCombine(szFind, _szFolder, i);
                if (PathFileExists(szFind))
                {
                    lstrcpyn(_szLogo, szFind, ARRAYSIZE(_szLogo));
                    break;
                }
            }
        }
    }

    LPCWSTR psz = ((cx > 120) && _szWideLogo[0])
        ? _szWideLogo
        : _szLogo;
    return *psz ? psz : nullptr;
}

STDMETHODIMP CFolderExtractImage::RequestAlphaThumbnail() noexcept
{
    _fAlpha = true;
    return S_OK;
}

STDMETHODIMP CFolderExtractImage::GetLocation(LPWSTR pszPath, DWORD cch,
                                              DWORD *pdwPriority, const SIZE *prgSize,
                                              DWORD dwRecClrDepth, DWORD *pdwFlags) noexcept
{
    lstrcpyn(pszPath, _szFolder, cch);

    _size = *prgSize;

    // TODO: Size handling is a bit of a mess. This is a hack for PaneXP which
    // only looks right with fixed 96x96 thumbnails. We will not attempt to
    // generate any lower size.
    _size.cx = std::max(_size.cx, 95L);
    _size.cy = std::max(_size.cy, 95L);

    // HACKHACK: PaneXP really hates specifically 96x96 scale thumbnails. So we
    // will avoid them and force 95x95 ones instead:
    if (96 == _size.cx)
    {
        _size.cx = 95;
        _size.cy = 95;
    }

    Wh_Log(L"Size: { %d, %d }", _size.cx, _size.cy);

    _dwRecClrDepth = dwRecClrDepth;
    _dwExtractFlags = *pdwFlags;

    if (pdwFlags)
    {
        // XP avoided caching here and instead managed caching itself via an
        // interface that no longer exists. I modified the behavior to act just
        // like Vista+. However, it is optimized for quality now.
        *pdwFlags |= IEIFLAG_QUALITY;
        *pdwFlags |= IEIFLAG_CACHE;
    }

    if (pdwPriority)
    {
        _dwPriority = *pdwPriority;
        *pdwPriority = 1; // Very low.
    }

    return S_OK;
}

#define IDI_MYDOCS              235
#define IDI_MYPICS              236
#define IDI_MYMUSIC             237
#define IDI_MYVIDEOS            238

#define REGSTR_EXPLORER_ADVANCED TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced")

STDMETHODIMP CFolderExtractImage::Extract(HBITMAP *phbm) noexcept
{
    // Set it to running (only if we're in the not running state).
    LONG lResOld = InterlockedCompareExchange(&_lState, IRTIR_TASK_RUNNING, IRTIR_TASK_NOT_RUNNING);

    if (lResOld != IRTIR_TASK_NOT_RUNNING)
    {
        // If we weren't in the not running state, bail.
        return E_FAIL;
    }

    // If we have an extractor, use that.
    HRESULT hr = E_FAIL;
    LPITEMIDLIST apidlPreviews[MAX_MINIPREVIEWS_COLLECT];
    BOOL abTryCached[MAX_MINIPREVIEWS_COLLECT];
    UINT cpidlPreviews = 0;

    LPCTSTR pszLogo = _GetImagePath(_size.cx);
    if (pszLogo)
    {
        LPITEMIDLIST pidl;
        hr = SHILCreateFromPath(pszLogo, &pidl, NULL);
        if (SUCCEEDED(hr))
        {
            LPCITEMIDLIST pidlChild;
            IShellFolder* psfLogo;
            hr = SHBindToParent(pidl, IID_PPV_ARGS(&psfLogo), &pidlChild);
            if (SUCCEEDED(hr))
            {
                hr = _CreateWithMiniPreviews(psfLogo, &pidlChild, NULL, 1, MINIPREVIEW_LAYOUT_1, phbm);
                psfLogo->Release();
            }
            ILFree(pidl);
        }
    }
    else
    {
        const struct 
        {
            int csidl;
            int res;
        } 
        thumblist[] = 
        {
            {CSIDL_PERSONAL,           IDI_MYDOCS},
            {CSIDL_MYMUSIC,            IDI_MYMUSIC},
            {CSIDL_MYPICTURES,         IDI_MYPICS},
            {CSIDL_MYVIDEO,            IDI_MYVIDEOS},
            {CSIDL_COMMON_DOCUMENTS,   IDI_MYDOCS},
            {CSIDL_COMMON_MUSIC,       IDI_MYMUSIC},
            {CSIDL_COMMON_PICTURES,    IDI_MYPICS},
            {CSIDL_COMMON_VIDEO,       IDI_MYVIDEOS}
        };
        BOOL bFound = FALSE;

        for (size_t i = 0; i < ARRAYSIZE(thumblist) && !bFound; i++)
        {
            TCHAR szPath[MAX_PATH];
            SHGetFolderPath(NULL, thumblist[i].csidl, NULL, 0, szPath);
            if (!lstrcmp(_szFolder, szPath))
            {
                // We return failure in this case so that the requestor can do
                // the default action.
                hr = E_FAIL;
                bFound = TRUE;
            }
        }

        if (!bFound)
        {
            cpidlPreviews = ARRAYSIZE(apidlPreviews);
            hr = _FindMiniPreviews(apidlPreviews, abTryCached, &cpidlPreviews);
            if (SUCCEEDED(hr))
            {
                if (cpidlPreviews)
                {
                    hr = _CreateWithMiniPreviews(_spsf.Get(), apidlPreviews, abTryCached, cpidlPreviews, _GetMiniPreviewLayout(_size), phbm);
                    FreeMiniPreviewPidls(apidlPreviews, cpidlPreviews);
                }
                else
                {
                    // We return failure in this case so that the requestor can do
                    // the default action
                    hr = E_FAIL; 
                }
            }
        }
    }
    
    return hr;
}

STDMETHODIMP CFolderExtractImage::GetClassID(CLSID *pClassID) noexcept
{
    return E_NOTIMPL;
}

STDMETHODIMP CFolderExtractImage::Init(IShellFolder *psf, LPCITEMIDLIST pidl) noexcept
{
    HRESULT hr = DisplayNameOf(psf, pidl, SHGDN_FORPARSING, _szFolder, ARRAYSIZE(_szFolder));
    if (SUCCEEDED(hr))
    {
        UniquePidl spidlFolder;
        hr = SHGetIDListFromObject(psf, &spidlFolder); // Replaced from SHGetIDListFromUnk
        if (SUCCEEDED(hr))
        {
            hr = SHILCombine(spidlFolder.get(), pidl, &_spidl);
            if (SUCCEEDED(hr))
            {
                // hold the _psf for this guy so we can enum
                hr = psf->BindToObject(pidl, NULL, IID_PPV_ARGS(&_spsf));
                if (SUCCEEDED(hr))
                {
                    hr = InitNew();
                }
            }
        }
    }
    return hr;
}

// Not necessary --- IExtractImage::Extract() starts us up.
STDMETHODIMP CFolderExtractImage::Run(void) noexcept
{
    return E_NOTIMPL;
}

STDMETHODIMP CFolderExtractImage::Kill(BOOL fWait) noexcept
{
    // Try to kill the current subextraction task that's running, if any.
    if (_spRun != NULL)
    {
        _spRun->Kill(fWait);
        // If it didn't work, no big deal, we'll complete this subextraction task,
        // and bail before starting the next one.
    }

    // If we're running, set to pending.
    LONG lResOld = InterlockedCompareExchange(&_lState, IRTIR_TASK_PENDING, IRTIR_TASK_RUNNING);
    if (lResOld == IRTIR_TASK_RUNNING)
    {
        // We've now set it to pending - ready to die.
        return S_OK;
    }
    else if (lResOld == IRTIR_TASK_PENDING || lResOld == IRTIR_TASK_FINISHED)
    {
        // We've already been killed.
        return S_FALSE;
    }

    return E_FAIL;
}

STDMETHODIMP CFolderExtractImage::Suspend(void) noexcept
{
    return E_NOTIMPL;
}

STDMETHODIMP CFolderExtractImage::Resume(void) noexcept
{
    return E_NOTIMPL;
}

STDMETHODIMP_(ULONG) CFolderExtractImage::IsRunning(void) noexcept
{
    return _lState;
}

HRESULT CFolderExtractImage::_CreateWithMiniPreviews(IShellFolder *psf, const LPCITEMIDLIST *apidlPreviews, BOOL *abTryCached, UINT cpidlPreviews, MINIPREVIEW_LAYOUT uLayout, HBITMAP *phBmpThumbnail)
{
    *phBmpThumbnail = NULL;

    HBITMAP hbmpOld;
    HDC hdc;

    SIZE sizeOriginal;      // Size of the source bitmaps that go into the minipreview.
    SIZE sizeFolderBmp;     // Size of the folder bmp we use for the background.
    SIZE sizeMiniPreview;   // The size calculated for the minipreviews
    POINT aptOrigins[MAX_MINIPREVIEWS];
    RGBQUAD* prgb;          // the bits of the destination bitmap. 

    GetMiniPreviewLocations(uLayout, _size, &sizeFolderBmp,
                             aptOrigins, &sizeMiniPreview);

    // sizeFolderBmp is the size of the folder background bitmap that we're working with,
    // not the size of the final thumbnail.
    HRESULT hr = CreateMainRenderingDC(&hdc, phBmpThumbnail, &hbmpOld, sizeFolderBmp.cx, sizeFolderBmp.cy, &prgb);

    if (SUCCEEDED(hr))
    {
        BOOL fIsAlphaBackground;
        hr = DrawMiniPreviewBackground(hdc, sizeFolderBmp, _fAlpha, &fIsAlphaBackground, prgb);

        if (SUCCEEDED(hr))
        {
            ULONG uPreviewLocation = 0;

            // Extract the images for the minipreviews
            for (ULONG i = 0 ; i < cpidlPreviews && uPreviewLocation < ARRAYSIZE(aptOrigins) ; i++)
            {
                bool foundAlphaImage = false;

                // If we've been killed, stop the processing the minipreviews:
                // PENDING?, we're now FINISHED.
                InterlockedCompareExchange(&_lState, IRTIR_TASK_FINISHED, IRTIR_TASK_PENDING);

                if (_lState == IRTIR_TASK_FINISHED)
                {
                    // Get out.
                    hr = E_FAIL;
                    break;
                }

                HBITMAP hbmpSubs;
                bool foundImage = false;

                // Resort to calling extractor if the image was not in the cache.
                if (!foundImage)
                {
                    ComPtr<IExtractImage> speiSub;
                    HRESULT hr2 = psf->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST *)&apidlPreviews[i], IID_X_PPV_ARG(IExtractImage, NULL, &speiSub));
                    if (SUCCEEDED(hr2))
                    {
                        // Now extract the image.
                        DWORD dwPriority = 0;
                        DWORD dwFlags = IEIFLAG_ORIGSIZE | IEIFLAG_QUALITY;// ORIGSIZE -> preserve aspect ratio

                        WCHAR szPathBuffer[MAX_PATH];
                        hr2 = speiSub->GetLocation(szPathBuffer, ARRAYSIZE(szPathBuffer), &dwPriority, &sizeMiniPreview, 24, &dwFlags);

                        // This will always be assumed to be the case because
                        // otherwise alpha would never be used.
                        // IAlphaThumbnailExtractor is not implemented by anything
                        // in modern Windows, which is what the XP logic relied on.
                        foundAlphaImage = true;

                        if (SUCCEEDED(hr2))
                        {
                            // After we check for IRTIR_TASK_PENDING, but before
                            // we call peiSub->Extract,  it is possible someone calls
                            // Kill on us.
                            // Since _pRun will be NULL, we will not kill
                            // the subtask, but will instead continue and call extract
                            // on it, and not bail until we try the next subthumbnail.
                            // Oh well.
                            // We could add another check here to reduce the window of
                            // opportunity in which this could happen.

                            // Try to get an IRunnableTask so that we can stop execution
                            // of this subtask if necessary.
                            speiSub->QueryInterface(IID_PPV_ARGS(&_spRun));

                            if (SUCCEEDED(speiSub->Extract(&hbmpSubs)))
                            {
                                foundImage = true;
                            }

                            _spRun.Reset();
                        }
                    }
                }

                // Add the extracted bitmap to the main one...
                if (foundImage)
                {
                    // The bitmap will of course need to be resized:
                    BITMAP rgBitmap;
                    if  (::GetObject((HGDIOBJ)hbmpSubs, sizeof(rgBitmap), &rgBitmap))
                    {
                        sizeOriginal.cx = rgBitmap.bmWidth;
                        sizeOriginal.cy = rgBitmap.bmHeight;

                        // We need to check if this is really an alpha bitmap.  It's possible that the
                        // extractor said it could generate one, but ended up not being able to.
                        if (foundAlphaImage)
                        {
                            RECT rc = {0, 0, rgBitmap.bmWidth, rgBitmap.bmHeight};
                            foundAlphaImage = (rgBitmap.bmBitsPixel == 32) &&
                                                HasAlpha(rc, rgBitmap.bmWidth, (RGBQUAD*)rgBitmap.bmBits);
                        }
                    }
                    else
                    {
                        // Couldn't get the info, oh well, no resize.
                        // alpha may also be screwed up here, but oh well.
                        sizeOriginal = sizeMiniPreview;
                    }

                    if (SUCCEEDED(AddBitmap(hdc, hbmpSubs, aptOrigins[uPreviewLocation], sizeMiniPreview, sizeOriginal, foundAlphaImage, fIsAlphaBackground, prgb, sizeFolderBmp)))
                    {
                        uPreviewLocation++;
                    }

                    DeleteObject(hbmpSubs);
                }
            }

            if (!uPreviewLocation)
            {
                // For whatever reason, we have no mini thumbnails to show, so fail this entire extraction.
                hr = E_FAIL;
            }
        }

        if (SUCCEEDED(hr))
        {
            // Is the requested size one of the sizes of the folder background bitmaps?
            // Test against smallest requested dimension, because we're square, and we'll fit into that rectangle
            int iSmallestDimension = std::min(_size.cx, _size.cy);
            if ((sizeFolderBmp.cx != iSmallestDimension) || (sizeFolderBmp.cy != iSmallestDimension))
            {
                // Nope - we need to do some scaling.
                // Create another dc and bitmap the size of the requested bitmap
                HBITMAP hBmpThumbnailFinal = NULL;
                HBITMAP hbmpOld2;
                HDC hdcFinal;
                RGBQUAD *prgbFinal;
                hr = CreateMainRenderingDC(&hdcFinal, &hBmpThumbnailFinal, &hbmpOld2, iSmallestDimension, iSmallestDimension, &prgbFinal);
                if (SUCCEEDED(hr))
                {
                    // Now scale it.
                    if (fIsAlphaBackground)
                    {
                        BLENDFUNCTION bf;
                        bf.BlendOp = AC_SRC_OVER;
                        bf.SourceConstantAlpha = 255;
                        bf.AlphaFormat = AC_SRC_ALPHA;
                        bf.BlendFlags = 0;
                        if (AlphaBlend(hdcFinal, 0, 0, iSmallestDimension, iSmallestDimension, hdc, 0 ,0, sizeFolderBmp.cx, sizeFolderBmp.cy, bf))
                            hr = S_OK;
                    }
                    else
                    {
                        int iModeSave = SetStretchBltMode(hdcFinal, HALFTONE);

                        if (StretchBlt(hdcFinal, 0, 0, iSmallestDimension, iSmallestDimension, hdc, 0 ,0, sizeFolderBmp.cx, sizeFolderBmp.cy, SRCCOPY))
                            hr = S_OK;

                        SetStretchBltMode(hdcFinal, iModeSave);
                    }

                    // Destroy the dc.
                    DestroyMainRenderingDC(hdcFinal, hbmpOld2);

                    // Now do a switcheroo
                    // Don't need to check for success here.  Down below, we'll delete *phBmpThumbnail
                    // if StretchBlt FAILED - and in that case, *pbBmpThumbnail will be hBmpThumbnailFinal.
                    DeleteObject(*phBmpThumbnail); // delete this, we don't need it.
                    *phBmpThumbnail = hBmpThumbnailFinal; // This is the one we want.
                }
            }
        }
        DestroyMainRenderingDC(hdc, hbmpOld);
    }


    if (FAILED(hr) && *phBmpThumbnail) // Something didn't work? Make sure we delete our bmp
    {
        DeleteObject(*phBmpThumbnail);
    }

    return hr;
}


HRESULT WINAPI GetDateProperty(IShellFolder2 *psf, LPCITEMIDLIST pidl, const SHCOLUMNID *pscid, FILETIME *pft)
{
    VARIANT var = { { {0} } };
    HRESULT hr = psf->GetDetailsEx(pidl, pscid, &var);
    if (SUCCEEDED(hr))
    {
        hr = E_FAIL;
        if (VT_DATE == var.vt)
        {
            SYSTEMTIME st;
            if (VariantTimeToSystemTime(var.date, &st) && SystemTimeToFileTime(&st, pft))
            {
                hr = S_OK;
            }
        }

        VariantClear(&var); // Done with it.
    }
    return hr;
}

#define PSGUID_STORAGE  { 0xb725f130,           \
                          0x47ef, 0x101a,       \
                          { 0xa5, 0xf1, 0x02, 0x60, 0x8c, 0x9e, 0xeb, 0xac } }
#define PID_STG_WRITETIME               ((PROPID) 0x0000000e)

const SHCOLUMNID SCID_WRITETIME = { PSGUID_STORAGE, PID_STG_WRITETIME };

/**
 * In/Out: cpidlPreviews - the number of preview items we should look for. Returns the number found.
 * number of pidls returned is cpidlPreviews.
 * Out: apidlPreviews - array of pidls found.  The caller must free them.
 */
HRESULT CFolderExtractImage::_FindMiniPreviews(LPITEMIDLIST apidlPreviews[], BOOL abTryCached[], UINT *pcpidlPreviews)
{   
    UINT cMaxPreviews = *pcpidlPreviews;
    int uNumPreviewsSoFar = 0;
    BOOL bKilled = FALSE;

    // Make sure our aFileTimes array is the right size...
    // ASSERT(MAX_MINIPREVIEWS_COLLECT == cMaxPreviews);

    *pcpidlPreviews = 0; // start with none in case of failure

    IEnumIDList *penum;
    if (S_OK == _spsf->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &penum))
    {
        FILETIME aFileTimes[MAX_MINIPREVIEWS_COLLECT] = {0};

        LPITEMIDLIST pidl;
        BOOL bTryCached;
        while (S_OK == penum->Next(1, &pidl, NULL))
        {
            // _IsMiniPreviewCandidate is a potentially expensive operation, so before
            // doing it, we'll check to see if anyone has killed us.

            // Are we PENDING? Then we're FINISHED.
            InterlockedCompareExchange(&_lState, IRTIR_TASK_FINISHED, IRTIR_TASK_PENDING);

            // Get out?
            bKilled = (_lState == IRTIR_TASK_FINISHED);

            if (!bKilled && IsMiniPreviewCandidate(_spsf.Get(), pidl, &bTryCached))
            {
                // Get file time of this guy.
                FILETIME ft;
                if (SUCCEEDED(GetDateProperty(_spsf.Get(), pidl, &SCID_WRITETIME, &ft)))
                {
                    int i;
                    for (i = 0; i < uNumPreviewsSoFar; i++)
                    {
                        if (CompareFileTime(&aFileTimes[i], &ft) < 0)
                        {
                            int j;
                            // Put it in this slot. First, move guys down by one.
                            // No need to copy last guy:
                            if (uNumPreviewsSoFar == (int)cMaxPreviews)
                            {   
                                j = (cMaxPreviews - 2);
                                // And we must free the pidl we're nuking.
                                ILFree(apidlPreviews[cMaxPreviews - 1]);
                                apidlPreviews[cMaxPreviews - 1] = NULL;
                            }
                            else
                            {
                                j = uNumPreviewsSoFar - 1;
                                uNumPreviewsSoFar++;
                            }

                            for (; j >= i; j--)
                            {
                                apidlPreviews[j+1] = apidlPreviews[j];
                                abTryCached[j+1] = abTryCached[j];
                                aFileTimes[j+1] = aFileTimes[j];
                            }

                            aFileTimes[i] = ft;
                            apidlPreviews[i] = pidl;
                            abTryCached[i] = bTryCached;
                            pidl = NULL;    // don't free
                            break;  // for loop
                        }
                    }

                    // Did we complete the loop?
                    if (i == uNumPreviewsSoFar)
                    {
                        if (i < (int)cMaxPreviews)
                        {
                            // We still have room for more previews, so tack this on at the end.
                            uNumPreviewsSoFar++;
                            aFileTimes[i] = ft;
                            apidlPreviews[i] = pidl;
                            abTryCached[i] = bTryCached;
                            pidl = NULL;    // don't free below
                        }
                    }

                    *pcpidlPreviews = uNumPreviewsSoFar;
                }
            }
            ILFree(pidl);   // NULL pidl OK

            if (bKilled)
            {
                break;
            }
        }
        penum->Release();
    }

    if (bKilled)
    {
        FreeMiniPreviewPidls(apidlPreviews, *pcpidlPreviews);
        *pcpidlPreviews = 0;
        return E_FAIL;
    }
    else
    {
        return (uNumPreviewsSoFar > 0) ? S_OK : S_FALSE;
    }
}

HRESULT CFolderExtractImage::_CreateThumbnailFromIconResource(HBITMAP* phBmpThumbnail, int res)
{
    *phBmpThumbnail = NULL;

    HBITMAP hbmpOld;
    HDC hdc;
    RGBQUAD* prgb;          // the bits of the destination bitmap. 
    
    HRESULT hr = CreateMainRenderingDC(&hdc, phBmpThumbnail, &hbmpOld, _size.cx, _size.cy, &prgb);

    if (SUCCEEDED(hr))
    {
        wil::unique_hmodule shmShell32(LoadLibraryW(L"shell32.dll"));
        RETURN_LAST_ERROR_IF_NULL(shmShell32);
        HICON hicon = (HICON)LoadImage(shmShell32.get(), MAKEINTRESOURCE(res), IMAGE_ICON, _size.cx, _size.cy, 0);

        if (hicon)
        {
            RECT rc = { 0, 0, _size.cx + 1, _size.cy + 1};
            SHFillRectClr(hdc, &rc, GetSysColor(COLOR_WINDOW));

            DrawIconEx(hdc, 0, 0, hicon, _size.cx, _size.cy, 0, NULL, DI_NORMAL);
            
            DestroyIcon(hicon);
            hr = S_OK;
        }
        
        DestroyMainRenderingDC(hdc, hbmpOld);
    }

    if (FAILED(hr) && *phBmpThumbnail)
    {
        DeleteObject(*phBmpThumbnail);
        *phBmpThumbnail = NULL;
    }

    return hr;
}

HRESULT (__fastcall *CFolderExtractImage_Create_orig_shell32)(IShellFolder *psf, LPCITEMIDLIST pidl, REFIID riid, void **ppv) = nullptr;
HRESULT __fastcall CFolderExtractImage_Create_hook_shell32(IShellFolder *psf, LPCITEMIDLIST pidl, REFIID riid, void **ppv)
{
    return CFolderExtractImage_Create(psf, pidl, riid, ppv);
}

HRESULT (__fastcall *CFolderExtractImage_Create_orig_storage)(IShellFolder *psf, LPCITEMIDLIST pidl, REFIID riid, void **ppv) = nullptr;
HRESULT __fastcall CFolderExtractImage_Create_hook_storage(IShellFolder *psf, LPCITEMIDLIST pidl, REFIID riid, void **ppv)
{
    return CFolderExtractImage_Create(psf, pidl, riid, ppv);
}

inline bool IsDirectoryItemType(LPCWSTR szType)
{
    return 0 == StrCmpIW(szType, L"Directory")
        || 0 == StrCmpIW(szType, L"Folder");
}

unsigned int WINAPI (*GetThumbnailCutoffFromType_orig_shell32)(LPCWSTR szType);
unsigned int WINAPI (*GetThumbnailCutoffFromType_orig_storage)(LPCWSTR szType);
unsigned int WINAPI GetThumbnailCutoffFromType_hook_common(
    decltype(GetThumbnailCutoffFromType_orig_storage) orig, LPCWSTR szType)
{
    if (IsDirectoryItemType(szType))
    {
        // Only show the thumbnail for folders once the icon is at least 80
        // pixels large.
        return 80;
    }
    
    return orig(szType);
}

unsigned int WINAPI GetThumbnailCutoffFromType_hook_storage(LPCWSTR szType)
{
    return GetThumbnailCutoffFromType_hook_common(GetThumbnailCutoffFromType_orig_storage, szType);
}
unsigned int WINAPI GetThumbnailCutoffFromType_hook_shell32(LPCWSTR szType)
{
    return GetThumbnailCutoffFromType_hook_common(GetThumbnailCutoffFromType_orig_shell32, szType);
}

#define THUMBNAILCUTOFF_DEBUGMSG 0
#if THUMBNAILCUTOFF_DEBUGMSG
    #define ThumbnailCutoffLog Wh_Log
#else
    __forceinline void ThumbnailCutoffLog(LPCWSTR sz, ...) {}
#endif

DEFINE_GUID(IID_IDefItem, 0xA2B6220F, 0x928B, 0x4D22, 0x87,0x86, 0xEA,0x96,0xAC,0x60,0xB9,0x65);
MIDL_INTERFACE("a2b6220f-928b-4d22-8786-ea96ac60b965")
IDefItem : public IUnknown
{
};
__CRT_UUID_DECL(IDefItem, 0xA2B6220F, 0x928B, 0x4D22, 0x87,0x86, 0xEA,0x96,0xAC,0x60,0xB9,0x65);

DEFINE_GUID(IID_IItem, 0xAECC1438, 0x059C, 0x44AD, 0xA2,0xB4, 0xE8,0x7F,0x4D,0x3A,0x49,0x87);
MIDL_INTERFACE("aecc1438-059c-44ad-a2b4-e87f4d3a4987")
IItem : public IUnknown
{
    // This isn't a complete interface. It only contains the single method
    // necessary for this mod.
    STDMETHOD(GetValue)(int valueAccessMode, const PROPERTYKEY &propKey, PROPVARIANT *pPropVariant, int *valueState) PURE;
};
__CRT_UUID_DECL(IItem, 0xAECC1438, 0x059C, 0x44AD, 0xA2,0xB4, 0xE8,0x7F,0x4D,0x3A,0x49,0x87);

thread_local IItem *g_pCurrentItem = nullptr;
DEFINE_PROPERTYKEY(PKEY_ItemType, 0x28636aa6, 0x953d, 0x11d2, 0xb5, 0xd6, 0x00, 0xc0, 0x4f, 0xd9, 0x18, 0xd0, 0x0b);

HRESULT (__thiscall *CDefItem__GetValue_orig_storage)(IUnknown *pThis, int valueAccessMode, const PROPERTYKEY &propKey, PROPVARIANT *pPropVariant, int *valueState) = nullptr;
HRESULT (__thiscall *CDefItem__GetValue_orig_shell32)(IUnknown *pThis, int valueAccessMode, const PROPERTYKEY &propKey, PROPVARIANT *pPropVariant, int *valueState) = nullptr;
HRESULT __thiscall CDefItem__GetValue_hook_common(decltype(CDefItem__GetValue_orig_storage) orig, IUnknown *pThis, int valueAccessMode, const PROPERTYKEY &propKey, PROPVARIANT *pPropVariant, int *valueState)
{
    ThumbnailCutoffLog(L"We're in a CDefItem function, so we'll carry the current value over "
        L"to any subsequent CImageManager::_GetThumbnailCutoff call on the "
        L"current thread.");
    g_pCurrentItem = (IItem *)pThis;
    HRESULT hr = orig(pThis, valueAccessMode, propKey, pPropVariant, valueState);
    ThumbnailCutoffLog(L"Exiting CDefItem function, so cleaning up now.");
    g_pCurrentItem = nullptr;
    return hr;
}

HRESULT __thiscall CDefItem__GetValue_hook_storage(IUnknown *pThis, int valueAccessMode, const PROPERTYKEY &propKey, PROPVARIANT *pPropVariant, int *valueState)
{
    return CDefItem__GetValue_hook_common(CDefItem__GetValue_orig_storage, pThis, valueAccessMode, propKey, pPropVariant, valueState);
}
HRESULT __thiscall CDefItem__GetValue_hook_shell32(IUnknown *pThis, int valueAccessMode, const PROPERTYKEY &propKey, PROPVARIANT *pPropVariant, int *valueState)
{
    return CDefItem__GetValue_hook_common(CDefItem__GetValue_orig_shell32, pThis, valueAccessMode, propKey, pPropVariant, valueState);
}

bool __thiscall (*CImageManager___StartThumbnailExtractionForItem_orig_storage)(void *pThis, unsigned long a, IItem *pItem, void *pItemImageStore, IObjectArray *pObjectArray, const GUID *guid, int b, unsigned int c);
bool __thiscall (*CImageManager___StartThumbnailExtractionForItem_orig_shell32)(void *pThis, unsigned long a, IItem *pItem, void *pItemImageStore, IObjectArray *pObjectArray, const GUID *guid, int b, unsigned int c);

bool __thiscall CImageManager___StartThumbnailExtractionForItem_hook_common(
    decltype(CImageManager___StartThumbnailExtractionForItem_orig_storage) orig,
    void *pThis, unsigned long a, IItem *pItem, void *pItemImageStore,
    IObjectArray *pObjectArray, const GUID *guid, int b, unsigned int c)
{
    ThumbnailCutoffLog(
        L"We're in CImageManager::_StartThumbnailExtractionForItem, where we "
        L"received an item pointer. This will be passed along to "
        L"_GetThumbnailCutoff on the current thread."
    );
    g_pCurrentItem = pItem;
    HRESULT hr = orig(pThis, a, pItem, pItemImageStore, pObjectArray, guid, b, c);
    ThumbnailCutoffLog(L"Exiting thumbnail extraction, so cleaning up now...");
    g_pCurrentItem = nullptr;
    return hr;
}

bool __thiscall CImageManager___StartThumbnailExtractionForItem_hook_storage(void *pThis, unsigned long a, IItem *pItem, void *pItemImageStore, IObjectArray *pObjectArray, const GUID *guid, int b, unsigned int c)
{
    return CImageManager___StartThumbnailExtractionForItem_hook_common(
        CImageManager___StartThumbnailExtractionForItem_orig_storage,
        pThis, a, pItem, pItemImageStore, pObjectArray, guid, b, c
    );
}

bool __thiscall CImageManager___StartThumbnailExtractionForItem_hook_shell32(void *pThis, unsigned long a, IItem *pItem, void *pItemImageStore, IObjectArray *pObjectArray, const GUID *guid, int b, unsigned int c)
{
    return CImageManager___StartThumbnailExtractionForItem_hook_common(
        CImageManager___StartThumbnailExtractionForItem_orig_shell32,
        pThis, a, pItem, pItemImageStore, pObjectArray, guid, b, c
    );
}

// Used for some locations, such as the downloads folder.
unsigned int WINAPI (*CImageManager___GetThumbnailCutoff_orig_storage)(void *pThis, void *pImageStore);
unsigned int WINAPI (*CImageManager___GetThumbnailCutoff_orig_shell32)(void *pThis, void *pImageStore);
unsigned int WINAPI CImageManager___GetThumbnailCutoff_hook_common(
    decltype(CDefItem__GetValue_orig_storage) pfnDefItem_GetValue,
    decltype(CImageManager___GetThumbnailCutoff_orig_storage) orig, void *pThis, void *pImageStore)
{
    bool fHandled = false;
    unsigned int uiRet = 0;

    if (g_pCurrentItem)
    {
        ThumbnailCutoffLog(L"A current CDefItem pointer is set on the current thread");
        PROPVARIANT pvar;
        int valueState = 0;

        IDefItem *pDefItem;
        HRESULT hr = g_pCurrentItem->QueryInterface(IID_PPV_ARGS(&pDefItem));
        if (SUCCEEDED(hr))
        {
            ThumbnailCutoffLog(L"Yay, the current item is compatible with DefItem");
            hr = pfnDefItem_GetValue(g_pCurrentItem, 0, PKEY_ItemType, &pvar, &valueState);
            pDefItem->Release();
        }
        else if (E_NOINTERFACE == hr)
        {
            hr = g_pCurrentItem->GetValue(0, PKEY_ItemType, &pvar, &valueState);
        }
        else
        {
            // This shouldn't ever happen.
            hr = E_FAIL;
        }

        if (SUCCEEDED(hr))
        {
            LPCWSTR pszItemType = nullptr;
            if (nullptr != (pszItemType = PropVariantToStringWithDefault(pvar, nullptr)))
            {
                ThumbnailCutoffLog(L"We actually have an item type: %s", pszItemType);
                if (IsDirectoryItemType(pszItemType))
                {
                    fHandled = true;
                    uiRet = 80;
                    ThumbnailCutoffLog(L"Everything is handled :DDD");
                }
            }
            else
            {
                ThumbnailCutoffLog(L"No string exists for the item type property.");
            }
            PropVariantClear(&pvar);
        }
        else
        {
            ThumbnailCutoffLog(L"Failed to get PKEY_ItemType");
        }
    }
    else
    {
        ThumbnailCutoffLog(L"No CDefItem pointer is available, so the default behavior will always be used.");
    }

    return fHandled
        ? uiRet
        : orig(pThis, pImageStore);
}
unsigned int WINAPI CImageManager___GetThumbnailCutoff_hook_storage(void *pThis, void *pImageStore)
{
    return CImageManager___GetThumbnailCutoff_hook_common(
        CDefItem__GetValue_orig_storage,
        CImageManager___GetThumbnailCutoff_orig_storage,
        pThis, pImageStore);
}
unsigned int WINAPI CImageManager___GetThumbnailCutoff_hook_shell32(void *pThis, void *pImageStore)
{
    return CImageManager___GetThumbnailCutoff_hook_common(
        CDefItem__GetValue_orig_shell32,
        CImageManager___GetThumbnailCutoff_orig_shell32,
        pThis, pImageStore);
}

// shell32.dll
WindhawkUtils::SYMBOL_HOOK shell32Hooks[] =
{
    {
        {
            L"CFolderExtractImage_Create",
        },
        &CFolderExtractImage_Create_orig_shell32,
        CFolderExtractImage_Create_hook_shell32,
        true,
    },
    {
        {
            L"unsigned int __cdecl GetThumbnailCutoffFromType(unsigned short const *)",
        },
        &GetThumbnailCutoffFromType_orig_shell32,
        GetThumbnailCutoffFromType_hook_shell32,
        true,
    },
    {
        {
            L"private: unsigned int __cdecl CImageManager::_GetThumbnailCutoff(struct IItemImageStore *)",
        },
        &CImageManager___GetThumbnailCutoff_orig_shell32,
        CImageManager___GetThumbnailCutoff_hook_shell32,
        true,
    },
    {
        {
            L"public: virtual long __cdecl CDefItem::GetValue(enum VALUE_ACCESS_MODE,struct _tagpropertykey const &,struct tagPROPVARIANT *,enum VALUE_STATE *)",
        },
        &CDefItem__GetValue_orig_shell32,
        CDefItem__GetValue_hook_shell32,
        true,
    },
    {
        {
#ifdef _WIN64
            L"private: bool __cdecl CImageManager::_StartThumbnailExtractionForItem(unsigned long,struct IItem *,struct IItemImageStore *,struct IObjectArray *,struct _GUID const &,int,unsigned int)",
#else
            L"private: bool __thiscall CImageManager::_StartThumbnailExtractionForItem(unsigned long,struct IItem *,struct IItemImageStore *,struct IObjectArray *,struct _GUID const &,int,unsigned int)"
#endif
        },
        &CImageManager___StartThumbnailExtractionForItem_orig_shell32,
        CImageManager___StartThumbnailExtractionForItem_hook_shell32,
        true,
    },
};

// windows.storage.dll (which is mostly just another copy of shell32's code)
WindhawkUtils::SYMBOL_HOOK windowsStorageDllHooks[] =
{
    {
        {
            L"CFolderExtractImage_Create",
        },
        &CFolderExtractImage_Create_orig_storage,
        CFolderExtractImage_Create_hook_storage,
    },
    {
        {
            L"unsigned int __cdecl GetThumbnailCutoffFromType(unsigned short const *)",
        },
        &GetThumbnailCutoffFromType_orig_storage,
        GetThumbnailCutoffFromType_hook_storage,
    },
    {
        {
            L"private: unsigned int __cdecl CImageManager::_GetThumbnailCutoff(struct IItemImageStore *)",
        },
        &CImageManager___GetThumbnailCutoff_orig_storage,
        CImageManager___GetThumbnailCutoff_hook_storage,
    },
    {
        {
            L"public: virtual long __cdecl CDefItem::GetValue(enum VALUE_ACCESS_MODE,struct _tagpropertykey const &,struct tagPROPVARIANT *,enum VALUE_STATE *)",
        },
        &CDefItem__GetValue_orig_storage,
        CDefItem__GetValue_hook_storage,
    },
    {
        {
#ifdef _WIN64
            L"private: bool __cdecl CImageManager::_StartThumbnailExtractionForItem(unsigned long,struct IItem *,struct IItemImageStore *,struct IObjectArray *,struct _GUID const &,int,unsigned int)",
#else
            L"private: bool __thiscall CImageManager::_StartThumbnailExtractionForItem(unsigned long,struct IItem *,struct IItemImageStore *,struct IObjectArray *,struct _GUID const &,int,unsigned int)"
#endif
        },
        &CImageManager___StartThumbnailExtractionForItem_orig_storage,
        CImageManager___StartThumbnailExtractionForItem_hook_storage,
    },
};

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    HMODULE shell32 = LoadLibraryW(L"shell32.dll");
    HMODULE windowsStorageDll = LoadLibraryW(L"windows.storage.dll");

    HMODULE shlwapi = LoadLibraryW(L"shlwapi.dll");
    if (!shlwapi)
    {
        Wh_Log(L"Failed to load shlwapi.dll");
        return FALSE;
    }

    *((FARPROC *)&SHPropertyBag_ReadStr) = GetProcAddress(shlwapi, (LPCSTR)494);
    if (!SHPropertyBag_ReadStr)
    {
        Wh_Log(L"Failed to find SHPropertyBag_ReadStr in shlwapi.dll.");
        return FALSE;
    }

    if (!shell32)
    {
        Wh_Log(L"Failed to load shell32.dll.");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(shell32, shell32Hooks, ARRAYSIZE(shell32Hooks)))
    {
        Wh_Log(L"Failed to hook symbols in shell32.dll.");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(windowsStorageDll, windowsStorageDllHooks, ARRAYSIZE(windowsStorageDllHooks)))
    {
        Wh_Log(L"Failed to hook symbols in windows.storage.dll.");
        return FALSE;
    }

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
