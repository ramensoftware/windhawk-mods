// ==WindhawkMod==
// @id              add-virtual-folders-to-nav-top
// @name            Add This PC and Desktop to Nav Top
// @description     Adds This PC and Desktop to the top of Explorer's navigation pane
// @version         1.0
// @author          Rod
// @include         *
// @architecture    x86-64
// @compilerOptions -lole32 -lshell32 -luuid -luxtheme -lgdi32 -lgdiplus -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Add This PC and Desktop to Nav Top

This mod adds two virtual folders to the top of File Explorer's
navigation pane and fixes the chevron rendering.

- **Show This PC at top:** Adds an expandable This PC entry
with drives. (Expandable This PC can't be pinned to the top
without this mod.)

- **Show Desktop at top:** Adds a Desktop entry for the root
namespace object. This includes Recycle Bin, Control Panel, etc.
instead of just the items on the desktop. (This target can't be
pinned without this mod.)

- **Fix chevron drawing:** Replaces the pixelated and clipped
chevron with a smooth anti-aliased versions. The size can be
tweaked to any percentage or pixel dimensions.

Both virtual folders have a toggle for whether they are expandable
or not. Their position can be swapped. Duplicate entries of Desktop
or This PC can be removed from other parts of the nav.

This mod injects only in processes that have ExplorerFrame.dll,
so the include is set to `*` but it will not touch most processes.
You can set it to `explorer.exe` only, if you don't want the nav in
Open/Save dialogs changed (only modern dialogs are affected.)

Before:

![Before](https://i.imgur.com/aahBZPG.png)

After:

![After](https://i.imgur.com/e2fEDwb.png)

The screenshots show the mod with the normal Desktop pinned to
Quick Access in before, compared to adding it using the mod.
Home and Gallery are already hidden using other tweaks.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ThisPC:
  - showThisPCAtTop: true
    $name: Add to top
    $description: Add an expandable entry for This PC to the top of the navigation pane.
  - thisPCExpandable: true
    $name: Make expandable
    $description: Shows drives underneath This PC when expanded.
  - thisPCStartExpanded: true
    $name: Start expanded
    $description: Auto-expand This PC when window opens.
  - hideThisPCFromQuickAccess: true
    $name: Hide duplicates
    $description: Hide This PC elsewhere in nav. Disable if there are issues with separators.
  $name: This PC
- Desktop:
  - showDesktopAtTop: false
    $name: Add to top
    $description: Adds the namespace root to the top of the navigation pane.
  - desktopExpandable: false
    $name: Make expandable
    $description: Shows namespace children when expanded.
  - desktopAboveThisPC: true
    $name: Place above This PC
    $description: Disable if there are issues with separator lines.
  - hideDesktopFromQuickAccess: true
    $name: Hide duplicates
    $description: Hide Desktop elsewhere in nav. Disable if there are issues with separators.
  $name: Desktop
- Resources:
  - fixChevronDrawing: true
    $name: Fix chevron drawing
    $description: >-
      Replaces theme-drawn expand/collapse chevrons with custom
      anti-aliased ones (fixes clipping at non-standard DPI like 225%)
  - chevronScale: 0
    $name: Chevron scale (%)
    $description: >-
      Size of expand/collapse chevrons. 0 or 100 = default size,
      125 = 25% larger. Negative = absolute pixel size (-20 = 20×20 px).
      Only applies when Fix chevron drawing is enabled.
  - hidePinButtons: true
    $name: Hide "pin" icons
    $description: Hide gray pin icons to the right of Quick Access items.
  $name: Resources
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <shlobj.h>
#include <commctrl.h>
#include <windowsx.h>
#include <uxtheme.h>
#include <gdiplus.h>
#include <set>

#ifdef _WIN64
#define THISCALL __cdecl
#else
#define THISCALL __thiscall
#endif

struct {
    bool showThisPCAtTop;
    bool thisPCExpandable;
    bool thisPCStartExpanded;
    bool hideThisPCFromQuickAccess;
    bool showDesktopAtTop;
    bool desktopAboveThisPC;
    bool desktopExpandable;
    bool hideDesktopFromQuickAccess;
    bool fixChevronDrawing;
    int chevronScale;
    bool hidePinButtons;
} g_settings;

static PIDLIST_ABSOLUTE g_pidlThisPC = nullptr;
static PIDLIST_ABSOLUTE g_pidlDesktop = nullptr;
static ULONG_PTR g_gdipToken = 0;
static std::set<HWND> g_subclassedParents;
static COLORREF g_sepColor = CLR_INVALID;
static HTREEITEM g_hCachedThisPC = nullptr;
static HTREEITEM g_hCachedDesktop = nullptr;
static HWND g_hCachedTree = nullptr;

// 0=not ours, 1=This PC, 2=Desktop — set during AppendOneItem
// so the TVM_INSERTITEM handler knows which item is being inserted
// without comparing localized display text.
static thread_local int g_insertingItem = 0;

static bool ShouldRemoveSeparators()
{
    return g_settings.showThisPCAtTop && g_settings.showDesktopAtTop &&
           g_settings.desktopAboveThisPC;
}

// --- AppendRoot hook ---

using AppendRoot_t = HRESULT (THISCALL *)(
    void *pThis, IShellItem *psiRoot, unsigned long grfEnumFlags,
    unsigned long grfRootStyle, IShellItemFilter *pFilter);

AppendRoot_t AppendRoot_orig;

static thread_local bool g_inCustomAppend = false;

static void AppendOneItem(
    void *pThis, PIDLIST_ABSOLUTE pidl, bool expandable,
    unsigned long grfEnumFlags, IShellItemFilter *pOrigFilter,
    const WCHAR *label, unsigned long rootStyle = 0)
{
    if (!pidl)
        return;

    IShellItem *pItem = nullptr;
    if (FAILED(SHCreateItemFromIDList(pidl, IID_IShellItem, (void **)&pItem)))
        return;

    HRESULT hr = AppendRoot_orig(pThis, pItem,
        expandable ? grfEnumFlags : 0,
        rootStyle,
        pOrigFilter);

    Wh_Log(L"AppendRoot: %s (%s) rootStyle=0x%lx result=0x%lx",
            label, expandable ? L"expandable" : L"leaf", rootStyle, hr);

    pItem->Release();
}

HRESULT THISCALL AppendRoot_hook(
    void *pThis, IShellItem *psiRoot, unsigned long grfEnumFlags,
    unsigned long grfRootStyle, IShellItemFilter *pFilter)
{
    if (g_inCustomAppend)
        return AppendRoot_orig(pThis, psiRoot, grfEnumFlags,
                               grfRootStyle, pFilter);

    bool isHiddenRoot = (grfRootStyle & 0x1) != 0;
    bool wantItems = isHiddenRoot &&
        (g_settings.showThisPCAtTop || g_settings.showDesktopAtTop);

    HRESULT hr = AppendRoot_orig(pThis, psiRoot, grfEnumFlags,
                                 grfRootStyle, pFilter);

    if (wantItems)
    {
        g_inCustomAppend = true;

        unsigned long thisPCStyle = g_settings.thisPCStartExpanded ? 0x2 : 0;

        struct { PIDLIST_ABSOLUTE pidl; bool expandable; bool enabled;
                 int id; const WCHAR *label; unsigned long style; } items[2];

        if (g_settings.desktopAboveThisPC)
        {
            items[0] = { g_pidlDesktop, g_settings.desktopExpandable,
                         g_settings.showDesktopAtTop, 2, L"Desktop", 0 };
            items[1] = { g_pidlThisPC, g_settings.thisPCExpandable,
                         g_settings.showThisPCAtTop, 1, L"This PC", thisPCStyle };
        }
        else
        {
            items[0] = { g_pidlThisPC, g_settings.thisPCExpandable,
                         g_settings.showThisPCAtTop, 1, L"This PC", thisPCStyle };
            items[1] = { g_pidlDesktop, g_settings.desktopExpandable,
                         g_settings.showDesktopAtTop, 2, L"Desktop", 0 };
        }

        for (int i = 0; i < 2; i++)
        {
            if (items[i].enabled)
            {
                g_insertingItem = items[i].id;
                AppendOneItem(pThis, items[i].pidl, items[i].expandable,
                              grfEnumFlags, pFilter, items[i].label,
                              items[i].style);
            }
        }
        g_insertingItem = 0;

        g_inCustomAppend = false;
    }

    return hr;
}

// --- Chevron fix: anti-aliased custom drawing via GDI+ ---

#define TVP_GLYPH        2
#define TVP_HOTGLYPH     4

static void CalcGlyphRect(LPCRECT src, RECT *dst)
{
    int w = src->right - src->left;
    int h = src->bottom - src->top;
    int s = g_settings.chevronScale;
    if (s == 0) s = 100;
    int nw, nh;
    if (s < 0)
        nw = nh = -s;
    else
    {
        nw = MulDiv(w, s, 100);
        nh = MulDiv(h, s, 100);
    }
    dst->right  = src->right;
    dst->left   = src->right - nw;
    dst->top    = src->top + (h - nh) / 2;
    dst->bottom = dst->top + nh;
}

static void DrawChevron(HDC hdc, const RECT *r, int partId, int stateId)
{
    int w  = r->right  - r->left;
    int h  = r->bottom - r->top;
    int sz = (w < h) ? w : h;

    float armLong  = sz * 0.375f;
    float armShort = armLong * 0.55f;

    float pw = sz / 14.0f;
    if (pw < 1.2f) pw = 1.2f;

    float cy = r->top + h * 0.5f;

    bool hot = (partId == TVP_HOTGLYPH);
    Gdiplus::Color col(hot ? 255 : 160, hot ? 255 : 160, hot ? 255 : 160);

    Gdiplus::Graphics gfx(hdc);
    gfx.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    Gdiplus::Pen pen(col, pw);
    pen.SetStartCap(Gdiplus::LineCapRound);
    pen.SetEndCap(Gdiplus::LineCapRound);
    pen.SetLineJoin(Gdiplus::LineJoinRound);

    Gdiplus::PointF pts[3];
    if (stateId == 2)   // expanded  ∨
    {
        float cx = (float)r->right - armLong - pw;
        pts[0] = { cx - armLong,  cy - armShort };
        pts[1] = { cx,            cy + armShort };
        pts[2] = { cx + armLong,  cy - armShort };
    }
    else                // collapsed >
    {
        float cx = (float)r->right - armShort - pw;
        pts[0] = { cx - armShort, cy - armLong };
        pts[1] = { cx + armShort, cy           };
        pts[2] = { cx - armShort, cy + armLong };
    }

    gfx.DrawLines(&pen, pts, 3);
}

static thread_local bool g_inTreePaint = false;

// --- Separator removal ---
// Strategy: swallowing NM_CUSTOMDRAW CDDS_PREPAINT (by returning
// CDRF_NOTIFYPOSTPAINT without calling DefSubclassProc) hides ALL
// separator lines. At CDDS_POSTPAINT we redraw the ones we want to
// keep — all section boundaries EXCEPT the one between our custom items.

// Sample the separator line color from the DC. Called on the first
// paint cycle when CDDS_PREPAINT was NOT swallowed (separators visible).
static COLORREF SampleSeparatorColor(HWND hTree, HDC hdc)
{
    if (!hdc || !IsWindow(hTree))
        return CLR_INVALID;

    RECT client;
    GetClientRect(hTree, &client);
    if (client.right <= 0)
        return CLR_INVALID;

    int baseHeight = 0;
    HTREEITEM h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                          TVGN_FIRSTVISIBLE, 0);
    while (h)
    {
        RECT rc = {};
        *(HTREEITEM*)&rc = h;
        if (SendMessageW(hTree, TVM_GETITEMRECT, FALSE, (LPARAM)&rc))
        {
            int ih = rc.bottom - rc.top;
            if (ih > 0 && (baseHeight == 0 || ih < baseHeight))
                baseHeight = ih;
        }
        h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                    TVGN_NEXTVISIBLE, (LPARAM)h);
    }
    if (baseHeight <= 0)
        return CLR_INVALID;

    int tallThreshold = baseHeight + baseHeight / 2;

    // Find the first tall depth-1 item — its separator is at rc.top + baseHeight/2
    h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                TVGN_FIRSTVISIBLE, 0);
    while (h)
    {
        HTREEITEM parent = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                                    TVGN_PARENT, (LPARAM)h);
        HTREEITEM gp = parent ?
            (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                    TVGN_PARENT, (LPARAM)parent) : nullptr;
        if (parent && !gp)
        {
            RECT rc = {};
            *(HTREEITEM*)&rc = h;
            if (SendMessageW(hTree, TVM_GETITEMRECT, FALSE, (LPARAM)&rc))
            {
                int ih = rc.bottom - rc.top;
                if (ih >= tallThreshold)
                {
                    int sepY = rc.top + baseHeight / 2;
                    int sampleX = client.right / 2;
                    COLORREF c = GetPixel(hdc, sampleX, sepY);
                    Wh_Log(L"[SEP-COLOR] sampled at (%d,%d) = 0x%06X",
                           sampleX, sepY, c);
                    if (c != CLR_INVALID && c != 0x000000)
                        return c;
                    // Try a few pixels around it
                    for (int dy = -2; dy <= 2; dy++)
                    {
                        c = GetPixel(hdc, sampleX, sepY + dy);
                        if (c != CLR_INVALID && c != 0x000000)
                        {
                            Wh_Log(L"[SEP-COLOR] sampled at (%d,%d) = 0x%06X",
                                   sampleX, sepY + dy, c);
                            return c;
                        }
                    }
                }
            }
        }
        h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                    TVGN_NEXTVISIBLE, (LPARAM)h);
    }

    return CLR_INVALID;
}

// At CDDS_POSTPAINT, all separators have been hidden by CDDS_PREPAINT
// swallowing. Redraw separators for section boundaries we want to keep.
// Tracks last-drawn positions to only log when something changes.
static int g_lastSepPositions[8] = {};
static int g_lastSepCount = 0;
static bool g_qaCleanupDone = false;
static HTREEITEM g_hQABoundaryItem = nullptr;
static HTREEITEM g_hiddenDuplicate = nullptr;

static void RedrawOtherSeparators(HWND hTree, HDC hdc)
{
    if (!hdc || !IsWindow(hTree))
        return;

    RECT client;
    GetClientRect(hTree, &client);
    if (client.right <= 0 || client.bottom <= 0)
        return;

    HTREEITEM hThisPC = (g_hCachedTree == hTree) ? g_hCachedThisPC : nullptr;
    HTREEITEM hDesktop = (g_hCachedTree == hTree) ? g_hCachedDesktop : nullptr;

    int baseHeight = 0;
    HTREEITEM h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                          TVGN_FIRSTVISIBLE, 0);
    while (h)
    {
        RECT rc = {};
        *(HTREEITEM*)&rc = h;
        if (SendMessageW(hTree, TVM_GETITEMRECT, FALSE, (LPARAM)&rc))
        {
            int ih = rc.bottom - rc.top;
            if (ih > 0 && (baseHeight == 0 || ih < baseHeight))
                baseHeight = ih;
        }
        h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                    TVGN_NEXTVISIBLE, (LPARAM)h);
    }
    if (baseHeight <= 0)
        baseHeight = 48;

    int tallThreshold = baseHeight + baseHeight / 2;
    int restored = 0;
    int positions[8] = {};

    h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                TVGN_FIRSTVISIBLE, 0);
    while (h)
    {
        HTREEITEM parent = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                                    TVGN_PARENT, (LPARAM)h);
        HTREEITEM grandparent = parent ?
            (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                    TVGN_PARENT, (LPARAM)parent) : nullptr;
        bool isDepth1 = (parent != nullptr && grandparent == nullptr);

        if (isDepth1)
        {
            RECT rc = {};
            *(HTREEITEM*)&rc = h;
            if (SendMessageW(hTree, TVM_GETITEMRECT, FALSE, (LPARAM)&rc))
            {
                int ih = rc.bottom - rc.top;
                bool isTall = (ih >= tallThreshold);
                bool isOurs = (h == hThisPC || h == hDesktop);

                if (isTall && !isOurs)
                {
                    int sepY = rc.top + baseHeight / 2;
                    int sepH = 2;

                    int padLeft = 18;
                    int padRight = 18;
                    int sepLeft = (client.right >= padLeft * 2 + 8) ? padLeft : 0;
                    int sepRight = (client.right >= padRight * 2 + 8) ? client.right - padRight : client.right;

                    RECT sepRect = { sepLeft, sepY, sepRight, sepY + sepH };

                    COLORREF sepColor = (g_sepColor != CLR_INVALID)
                        ? g_sepColor : RGB(255, 0, 0);

                    HBRUSH brush = CreateSolidBrush(sepColor);
                    if (brush)
                    {
                        FillRect(hdc, &sepRect, brush);
                        DeleteObject(brush);
                    }

                    if (restored < 8)
                        positions[restored] = sepY;
                    restored++;
                }
            }
        }

        h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                    TVGN_NEXTVISIBLE, (LPARAM)h);
    }


    // Only log when positions change
    bool changed = (restored != g_lastSepCount);
    if (!changed)
    {
        for (int i = 0; i < restored && i < 8; i++)
            if (positions[i] != g_lastSepPositions[i])
                { changed = true; break; }
    }
    if (changed)
    {
        g_lastSepCount = restored;
        for (int i = 0; i < 8; i++)
            g_lastSepPositions[i] = positions[i];

        if (restored == 0)
            Wh_Log(L"[SEP-RESTORE] no separators to restore (baseH=%d)", baseHeight);
        else if (restored == 1)
            Wh_Log(L"[SEP-RESTORE] 1 separator at y=%d (baseH=%d pad=18)",
                   positions[0], baseHeight);
        else
            Wh_Log(L"[SEP-RESTORE] %d separators at y=%d,%d (baseH=%d pad=18)",
                   restored, positions[0], positions[1], baseHeight);
    }
}

// Parent subclass — intercepts NM_CUSTOMDRAW.
// Returning CDRF_NOTIFYPOSTPAINT at CDDS_PREPAINT without calling
// DefSubclassProc hides ALL separator lines
// At CDDS_POSTPAINT we redraw the separators we want to keep.
static LRESULT CALLBACK SepParentSubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    DWORD_PTR dwRefData)
{
    HWND hTree = (HWND)dwRefData;

    if (uMsg == WM_NOTIFY && hTree && IsWindow(hTree))
    {
        LPNMHDR hdr = (LPNMHDR)lParam;
        if (hdr && hdr->hwndFrom == hTree)
        {
            if (hdr->code == (UINT)NM_CUSTOMDRAW)
            {
                LPNMTVCUSTOMDRAW cd = (LPNMTVCUSTOMDRAW)lParam;
                DWORD stage = cd->nmcd.dwDrawStage;
                bool removeSep = ShouldRemoveSeparators();
                bool hasDupHide = (g_hiddenDuplicate != nullptr);

                if (stage == CDDS_PREPAINT && (removeSep || hasDupHide))
                {
                    if (removeSep)
                    {
                        if (g_sepColor == CLR_INVALID)
                        {
                            LRESULT r = DefSubclassProc(hWnd, uMsg, wParam, lParam);
                            return r | CDRF_NOTIFYPOSTPAINT;
                        }
                        return CDRF_NOTIFYPOSTPAINT;
                    }
                    else
                    {
                        LRESULT r = DefSubclassProc(hWnd, uMsg, wParam, lParam);
                        return r | CDRF_NOTIFYPOSTPAINT;
                    }
                }

                if (stage == CDDS_POSTPAINT)
                {
                    if (g_sepColor == CLR_INVALID && (removeSep || hasDupHide))
                    {
                        COLORREF c = SampleSeparatorColor(hTree, cd->nmcd.hdc);
                        if (c != CLR_INVALID)
                        {
                            g_sepColor = c;
                            Wh_Log(L"[SEP-COLOR] captured separator color: 0x%06X", c);
                            if (removeSep)
                                InvalidateRect(hTree, NULL, TRUE);
                        }
                    }

                    if (removeSep && g_sepColor != CLR_INVALID)
                        RedrawOtherSeparators(hTree, cd->nmcd.hdc);

                    if (hasDupHide)
                    {
                        RECT rcHide = {};
                        *(HTREEITEM*)&rcHide = g_hiddenDuplicate;
                        if (SendMessageW(hTree, TVM_GETITEMRECT, FALSE, (LPARAM)&rcHide))
                        {
                            HDC hdc = cd->nmcd.hdc;
                            COLORREF bg = GetPixel(hdc, 1, rcHide.top + 2);
                            if (bg == CLR_INVALID || bg == 0)
                            {
                                RECT client;
                                GetClientRect(hTree, &client);
                                bg = GetPixel(hdc, client.right - 2, rcHide.top + 2);
                            }
                            if (bg != CLR_INVALID)
                            {
                                HBRUSH bgBrush = CreateSolidBrush(bg);
                                if (bgBrush)
                                {
                                    FillRect(hdc, &rcHide, bgBrush);
                                    DeleteObject(bgBrush);
                                }
                            }

                            if (g_sepColor != CLR_INVALID)
                            {
                                int h = rcHide.bottom - rcHide.top;
                                int sepY = rcHide.top + h / 2;
                                RECT client;
                                GetClientRect(hTree, &client);
                                int padL = 18, padR = 18;
                                int sepLeft = (client.right >= padL * 2 + 8) ? padL : 0;
                                int sepRight = (client.right >= padR * 2 + 8)
                                    ? client.right - padR : client.right;
                                RECT sepRect = { sepLeft, sepY, sepRight, sepY + 2 };

                                HBRUSH sepBrush = CreateSolidBrush(g_sepColor);
                                if (sepBrush)
                                {
                                    FillRect(hdc, &sepRect, sepBrush);
                                    DeleteObject(sepBrush);
                                }
                            }
                        }
                    }
                }
            }

            // After expand/collapse, force full repaint so stale
            // separator lines at old positions get overwritten.
            if (hdr->code == TVN_ITEMEXPANDEDW ||
                hdr->code == TVN_ITEMEXPANDEDA)
            {
                LRESULT r = DefSubclassProc(hWnd, uMsg, wParam, lParam);
                InvalidateRect(hTree, NULL, TRUE);
                Wh_Log(L"[SEP-EXPAND] tree=%p expanded/collapsed, full repaint scheduled", hTree);
                return r;
            }
        }
    }

    if (uMsg == WM_THEMECHANGED || uMsg == WM_SYSCOLORCHANGE)
    {
        g_sepColor = CLR_INVALID;
        Wh_Log(L"[SEP-COLOR] theme/colors changed, will resample");
    }

    if (uMsg == WM_NCDESTROY)
    {
        g_subclassedParents.erase(hWnd);
        Wh_Log(L"[SEP-PARENT] parent=%p destroyed, removed from tracking", hWnd);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static void EnsureParentSubclass(HWND hTree)
{
    HWND parent = GetParent(hTree);
    if (!parent)
        return;
    if (g_subclassedParents.count(parent))
        return;
    bool ok = WindhawkUtils::SetWindowSubclassFromAnyThread(
        parent, SepParentSubclassProc, (DWORD_PTR)hTree);
    Wh_Log(L"[SEP-PARENT] subclass on parent=%p for tree=%p ok=%d", parent, hTree, ok);
    if (ok)
        g_subclassedParents.insert(parent);
}

// --- Pin button hiding ---

using SetStateImageList_t = HRESULT (THISCALL *)(void *, HIMAGELIST);
SetStateImageList_t SetStateImageList_orig;

HRESULT THISCALL SetStateImageList_hook(void *pThis, HIMAGELIST himl)
{
    if (g_settings.hidePinButtons)
        return S_OK;
    return SetStateImageList_orig(pThis, himl);
}

// --- Quick Access item hiding ---
// Quick Access pinned items bypass all CNscTree insertion predicates
// (_ShouldInsertChild, _InsertChild) so we can't filter them at
// insertion time. Instead, on the first WM_PAINT after our items
// are cached, walk depth-1 items and delete any matching childless
// duplicates (QA pins have no children; nav pane sections do).

static void CleanupQuickAccessDuplicates(HWND hTree)
{
    if (!g_hCachedThisPC && !g_hCachedDesktop)
        return;

    WCHAR thisPCText[64] = {};
    WCHAR desktopText[64] = {};

    if (g_settings.showThisPCAtTop &&
        g_settings.hideThisPCFromQuickAccess && g_hCachedThisPC)
    {
        TVITEMEXW tvi = {};
        tvi.mask = TVIF_HANDLE | TVIF_TEXT;
        tvi.hItem = g_hCachedThisPC;
        tvi.pszText = thisPCText;
        tvi.cchTextMax = ARRAYSIZE(thisPCText);
        SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
    }

    if (g_settings.showDesktopAtTop &&
        g_settings.hideDesktopFromQuickAccess && g_hCachedDesktop)
    {
        TVITEMEXW tvi = {};
        tvi.mask = TVIF_HANDLE | TVIF_TEXT;
        tvi.hItem = g_hCachedDesktop;
        tvi.pszText = desktopText;
        tvi.cchTextMax = ARRAYSIZE(desktopText);
        SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
    }

    HTREEITEM h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                          TVGN_ROOT, 0);
    if (!h) return;

    // Walk depth-1 items (children of root)
    h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                TVGN_CHILD, (LPARAM)h);

    HTREEITEM toDelete[8] = {};
    int delCount = 0;

    while (h)
    {
        HTREEITEM hNext = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                                   TVGN_NEXT, (LPARAM)h);
        if (h != g_hCachedThisPC && h != g_hCachedDesktop)
        {
            HTREEITEM hChild = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                                        TVGN_CHILD, (LPARAM)h);
            if (!hChild)
            {
                WCHAR text[64] = {};
                TVITEMEXW tvi = {};
                tvi.mask = TVIF_HANDLE | TVIF_TEXT;
                tvi.hItem = h;
                tvi.pszText = text;
                tvi.cchTextMax = ARRAYSIZE(text);
                SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);

                bool match = false;
                if (thisPCText[0] && wcscmp(text, thisPCText) == 0)
                    match = true;
                if (desktopText[0] && wcscmp(text, desktopText) == 0)
                    match = true;

                if (match && delCount < 8)
                    toDelete[delCount++] = h;
            }
        }
        h = hNext;
    }

    // Only keep a duplicate hidden (paint-over) if it's a section
    // boundary item (iIntegral>=2) — removing it would destroy the
    // gap where the separator line goes. All others are safe to delete.
    g_hiddenDuplicate = nullptr;
    for (int i = 0; i < delCount; i++)
    {
        if (!g_hiddenDuplicate)
        {
            TVITEMEXW check = {};
            check.mask = TVIF_HANDLE | TVIF_INTEGRAL;
            check.hItem = toDelete[i];
            SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&check);

            if (check.iIntegral >= 2)
            {
                g_hiddenDuplicate = toDelete[i];
                TVITEMEXW forceSmall = {};
                forceSmall.mask = TVIF_HANDLE | TVIF_INTEGRAL;
                forceSmall.hItem = toDelete[i];
                forceSmall.iIntegral = 1;
                SendMessageW(hTree, TVM_SETITEMW, 0, (LPARAM)&forceSmall);
                Wh_Log(L"[QA-HIDE] keeping boundary item=%p invisible (was iIntegral=%d)",
                       toDelete[i], check.iIntegral);
                continue;
            }
        }

        Wh_Log(L"[QA-HIDE] deleting duplicate item=%p", toDelete[i]);
        SendMessageW(hTree, TVM_DELETEITEM, 0, (LPARAM)toDelete[i]);
    }
}

// --- SubClassTreeWndProc ---

using SubClassTreeWndProc_t = LRESULT (CALLBACK *)(
    HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

SubClassTreeWndProc_t SubClassTreeWndProc_orig;

LRESULT CALLBACK SubClassTreeWndProc_hook(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    if (uMsg == TVM_INSERTITEMW || uMsg == TVM_INSERTITEMA)
    {
        LRESULT result = SubClassTreeWndProc_orig(hWnd, uMsg, wParam,
                            lParam, uIdSubclass, dwRefData);
        HTREEITEM hNew = (HTREEITEM)result;
        if (hNew)
        {
            if (g_insertingItem == 1)
            {
                g_hCachedThisPC = hNew;
                g_hCachedTree = hWnd;
                g_qaCleanupDone = false;
                Wh_Log(L"[CACHE] This PC item=%p tree=%p", hNew, hWnd);
            }
            else if (g_insertingItem == 2)
            {
                g_hCachedDesktop = hNew;
                g_hCachedTree = hWnd;
                g_qaCleanupDone = false;
                Wh_Log(L"[CACHE] Desktop item=%p tree=%p", hNew, hWnd);
            }
            else if (g_qaCleanupDone && g_hCachedTree == hWnd)
            {
                HTREEITEM hPar = (HTREEITEM)SendMessageW(hWnd, TVM_GETNEXTITEM,
                                                          TVGN_PARENT, (LPARAM)hNew);
                if (hPar)
                {
                    HTREEITEM hGP = (HTREEITEM)SendMessageW(hWnd, TVM_GETNEXTITEM,
                                                              TVGN_PARENT, (LPARAM)hPar);
                    if (!hGP)
                    {
                        g_qaCleanupDone = false;
                        g_hiddenDuplicate = nullptr;
                        Wh_Log(L"[QA-HIDE] depth-1 item inserted, scheduling re-cleanup");
                    }
                }
            }
        }
        return result;
    }

    // Block clicks and cursor changes on hidden duplicate
    if (g_hiddenDuplicate &&
        (uMsg == WM_LBUTTONDOWN || uMsg == WM_SETCURSOR))
    {
        POINT pt;
        if (uMsg == WM_LBUTTONDOWN)
        {
            pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        }
        else
        {
            GetCursorPos(&pt);
            ScreenToClient(hWnd, &pt);
        }
        TVHITTESTINFO ht = {};
        ht.pt = pt;
        HTREEITEM hHit = (HTREEITEM)SendMessageW(hWnd, TVM_HITTEST, 0, (LPARAM)&ht);
        if (hHit == g_hiddenDuplicate)
        {
            if (uMsg == WM_LBUTTONDOWN)
                return 0;
            SetCursor(LoadCursorW(nullptr, IDC_ARROW));
            return TRUE;
        }
    }

    // Skip hidden duplicate on keyboard navigation
    if (uMsg == WM_KEYDOWN && g_hiddenDuplicate &&
        (wParam == VK_UP || wParam == VK_DOWN))
    {
        LRESULT r = SubClassTreeWndProc_orig(hWnd, uMsg, wParam, lParam,
                                              uIdSubclass, dwRefData);
        HTREEITEM hSel = (HTREEITEM)SendMessageW(hWnd, TVM_GETNEXTITEM,
                                                   TVGN_CARET, 0);
        if (hSel == g_hiddenDuplicate)
        {
            UINT dir = (wParam == VK_DOWN) ? TVGN_NEXTVISIBLE : TVGN_PREVIOUSVISIBLE;
            HTREEITEM hNext = (HTREEITEM)SendMessageW(hWnd, TVM_GETNEXTITEM,
                                                        dir, (LPARAM)hSel);
            if (hNext)
                SendMessageW(hWnd, TVM_SELECTITEM, TVGN_CARET, (LPARAM)hNext);
        }
        return r;
    }

    if (uMsg == TVM_SETITEMW || uMsg == TVM_SETITEMA)
    {
        TVITEMEXW* tvi = (TVITEMEXW*)lParam;
        if (tvi && (tvi->mask & TVIF_INTEGRAL))
        {
            // Keep hidden duplicate at iIntegral=1 so it occupies
            // exactly one row of space for the painted-over separator.
            if (g_hiddenDuplicate && tvi->hItem == g_hiddenDuplicate &&
                tvi->iIntegral != 1)
            {
                tvi->iIntegral = 1;
            }

            // Collapse the boundary between Desktop and This PC.
            if (ShouldRemoveSeparators() && tvi->iIntegral >= 2)
            {
                HTREEITEM hA = (g_hCachedTree == hWnd) ? g_hCachedThisPC : nullptr;
                HTREEITEM hB = (g_hCachedTree == hWnd) ? g_hCachedDesktop : nullptr;
                if (hA && hB)
                {
                    RECT rcA = {}, rcB = {};
                    *(HTREEITEM*)&rcA = hA;
                    *(HTREEITEM*)&rcB = hB;
                    bool gotA = SendMessageW(hWnd, TVM_GETITEMRECT, FALSE, (LPARAM)&rcA);
                    bool gotB = SendMessageW(hWnd, TVM_GETITEMRECT, FALSE, (LPARAM)&rcB);

                    if (gotA && gotB)
                    {
                        HTREEITEM hLower = (rcA.top > rcB.top) ? hA : hB;
                        if (tvi->hItem == hLower)
                        {
                            Wh_Log(L"[SEP-COLLAPSE] TVM_SETITEMW: iIntegral %d->1 on item=%p",
                                   tvi->iIntegral, tvi->hItem);
                            tvi->iIntegral = 1;
                        }
                    }
                }
            }
        }
    }

    if (uMsg == WM_PAINT)
    {
        if (!g_qaCleanupDone && g_hCachedTree == hWnd &&
            ((g_settings.showThisPCAtTop && g_settings.hideThisPCFromQuickAccess) ||
             (g_settings.showDesktopAtTop && g_settings.hideDesktopFromQuickAccess)))
        {
            g_qaCleanupDone = true;
            CleanupQuickAccessDuplicates(hWnd);
        }

        if (g_settings.hidePinButtons)
        {
            HIMAGELIST hState = (HIMAGELIST)SendMessageW(
                hWnd, TVM_GETIMAGELIST, TVSIL_STATE, 0);
            if (hState)
                SendMessageW(hWnd, TVM_SETIMAGELIST, TVSIL_STATE, 0);
        }

        if (ShouldRemoveSeparators() || g_hiddenDuplicate)
            EnsureParentSubclass(hWnd);

        if (g_settings.fixChevronDrawing)
            g_inTreePaint = true;

        LRESULT result = SubClassTreeWndProc_orig(hWnd, uMsg, wParam,
                            lParam, uIdSubclass, dwRefData);

        g_inTreePaint = false;

        return result;
    }

    return SubClassTreeWndProc_orig(hWnd, uMsg, wParam, lParam,
                                    uIdSubclass, dwRefData);
}

using DrawThemeBackground_t = HRESULT (WINAPI *)(
    HTHEME, HDC, int, int, LPCRECT, LPCRECT);

DrawThemeBackground_t DrawThemeBackground_orig;

HRESULT WINAPI DrawThemeBackground_hook(
    HTHEME hTheme, HDC hdc, int iPartId, int iStateId,
    LPCRECT pRect, LPCRECT pClipRect)
{
    if (g_inTreePaint && (iPartId == TVP_GLYPH || iPartId == TVP_HOTGLYPH))
    {
        RECT drawRect;
        CalcGlyphRect(pRect, &drawRect);
        DrawChevron(hdc, &drawRect, iPartId, iStateId);
        return S_OK;
    }
    return DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId,
                                    pRect, pClipRect);
}

// --- Settings ---

void LoadSettings()
{
    g_settings.showThisPCAtTop = Wh_GetIntSetting(L"ThisPC.showThisPCAtTop");
    g_settings.thisPCExpandable = Wh_GetIntSetting(L"ThisPC.thisPCExpandable");
    g_settings.thisPCStartExpanded = Wh_GetIntSetting(L"ThisPC.thisPCStartExpanded");
    g_settings.hideThisPCFromQuickAccess = Wh_GetIntSetting(L"ThisPC.hideThisPCFromQuickAccess");
    g_settings.showDesktopAtTop = Wh_GetIntSetting(L"Desktop.showDesktopAtTop");
    g_settings.desktopAboveThisPC = Wh_GetIntSetting(L"Desktop.desktopAboveThisPC");
    g_settings.desktopExpandable = Wh_GetIntSetting(L"Desktop.desktopExpandable");
    g_settings.hideDesktopFromQuickAccess = Wh_GetIntSetting(L"Desktop.hideDesktopFromQuickAccess");
    g_settings.fixChevronDrawing = Wh_GetIntSetting(L"Resources.fixChevronDrawing");
    g_settings.chevronScale = Wh_GetIntSetting(L"Resources.chevronScale");
    g_settings.hidePinButtons = Wh_GetIntSetting(L"Resources.hidePinButtons");

    g_sepColor = CLR_INVALID;

    Wh_Log(L"Settings: thisPCAtTop=%d (expand=%d, startExp=%d, hideQA=%d) "
            L"desktopAtTop=%d (above=%d, expand=%d, hideQA=%d) "
            L"sepRemoval=%d fixChevron=%d chevronScale=%d hidePins=%d",
            g_settings.showThisPCAtTop,
            g_settings.thisPCExpandable, g_settings.thisPCStartExpanded,
            g_settings.hideThisPCFromQuickAccess,
            g_settings.showDesktopAtTop, g_settings.desktopAboveThisPC,
            g_settings.desktopExpandable,
            g_settings.hideDesktopFromQuickAccess,
            ShouldRemoveSeparators(),
            g_settings.fixChevronDrawing,
            g_settings.chevronScale,
            g_settings.hidePinButtons);
}

BOOL Wh_ModInit()
{
    HMODULE hExplorerFrame = GetModuleHandleW(L"ExplorerFrame.dll");
    if (!hExplorerFrame)
        return FALSE;

    LoadSettings();

    Gdiplus::GdiplusStartupInput gdipIn;
    Gdiplus::GdiplusStartup(&g_gdipToken, &gdipIn, NULL);

    SHGetSpecialFolderLocation(nullptr, CSIDL_DRIVES, &g_pidlThisPC);
    if (!g_pidlThisPC)
    {
        Wh_Log(L"Failed to get This PC PIDL");
        return FALSE;
    }

    SHGetSpecialFolderLocation(nullptr, CSIDL_DESKTOP, &g_pidlDesktop);
    if (!g_pidlDesktop)
        Wh_Log(L"Warning: failed to get Desktop PIDL");

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                L"public: virtual long __cdecl"
                L" CNscTree::AppendRoot("
                L"struct IShellItem *,"
                L"unsigned long,"
                L"unsigned long,"
                L"struct IShellItemFilter *)"
            },
            &AppendRoot_orig,
            AppendRoot_hook,
            false
        },
        {
            {
                L"private: static __int64 __cdecl"
                L" CNscTree::s_SubClassTreeWndProc("
                L"struct HWND__ *,"
                L"unsigned int,"
                L"unsigned __int64,"
                L"__int64,"
                L"unsigned __int64,"
                L"unsigned __int64)"
            },
            &SubClassTreeWndProc_orig,
            SubClassTreeWndProc_hook,
            false
        },
        {
            {
                L"public: virtual long __cdecl"
                L" CNscTree::SetStateImageList("
                L"struct _IMAGELIST *)"
            },
            &SetStateImageList_orig,
            SetStateImageList_hook,
            false
        },
    };

    if (!WindhawkUtils::HookSymbols(hExplorerFrame, hooks, ARRAYSIZE(hooks)))
    {
        Wh_Log(L"Failed to hook symbols");
        return FALSE;
    }

    HMODULE hUxTheme = GetModuleHandleW(L"uxtheme.dll");
    if (hUxTheme)
    {
        FARPROC pDTB = GetProcAddress(hUxTheme, "DrawThemeBackground");
        if (pDTB)
            Wh_SetFunctionHook((void *)pDTB,
                               (void *)DrawThemeBackground_hook,
                               (void **)&DrawThemeBackground_orig);
    }

    Wh_Log(L"Mod initialized successfully");
    return TRUE;
}

void Wh_ModSettingsChanged()
{
    LoadSettings();
}

void Wh_ModUninit()
{
    if (g_gdipToken)
    {
        Gdiplus::GdiplusShutdown(g_gdipToken);
        g_gdipToken = 0;
    }
    if (g_pidlThisPC)
    {
        CoTaskMemFree(g_pidlThisPC);
        g_pidlThisPC = nullptr;
    }
    if (g_pidlDesktop)
    {
        CoTaskMemFree(g_pidlDesktop);
        g_pidlDesktop = nullptr;
    }
    for (HWND parent : g_subclassedParents)
    {
        if (IsWindow(parent))
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(parent, SepParentSubclassProc);
    }
    g_subclassedParents.clear();
    g_hCachedThisPC = nullptr;
    g_hCachedDesktop = nullptr;
    g_hCachedTree = nullptr;
    g_hiddenDuplicate = nullptr;
    Wh_Log(L"Mod uninitialized");
}