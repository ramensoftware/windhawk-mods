// ==WindhawkMod==
// @id              add-virtual-folders-to-nav-top
// @name            Add This PC and Desktop to Nav Top
// @description     Adds This PC and Desktop to the top of Explorer's navigation pane
// @version         1.1.1
// @author          Rod Boev
// @github          https://github.com/rodboev
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

Both virtual folders have a toggle for whether they are expandable
or not. Their position can be swapped. Duplicate entries of Desktop
or This PC can be removed from other parts of the nav. Also:

- **Hide Home/Gallery:** Optional, default-enabled toggles to
hide the native Explorer entries. They don't affect Quick Access.

- **Remove separators:** Optionally remove each separator individually.

- **Fix chevron drawing:** Replaces the pixelated and clipped
chevron with a smooth anti-aliased versions. The size (which
matches other UI elements by default) is configurable.

This mod injects only in processes that have ExplorerFrame.dll,
so the include is set to `*` but it will not touch most processes.
You can set it to `explorer.exe` only, if you don't want the nav in
Open/Save dialogs changed (only modern dialogs are affected.)

Before:

![Before](https://i.imgur.com/nC9T5A7.jpeg)

After:

![After](https://i.imgur.com/JQQ5ZTN.jpeg)

The screenshots show the normal Desktop pinned to Quick Access in
the before, and the mod with defaults set in the after.
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
    $description: Hide This PC elsewhere in the nav.
  $name: This PC
- Desktop:
  - showDesktopAtTop: false
    $name: Add to top
    $description: Adds the namespace root to the top of the nav.
  - desktopExpandable: false
    $name: Make expandable
    $description: Shows namespace children when expanded.
  - desktopAboveThisPC: true
    $name: Place above This PC
  - hideDesktopFromQuickAccess: true
    $name: Hide duplicates
    $description: Hide Desktop entries elsewhere in the nav.
  $name: Desktop
- HomeGallery:
  - hideHome: true
    $name: Hide Home
  - hideGallery: true
    $name: Hide Gallery
  $name: Hide Home/Gallery (does not affect Quick Access)
- Separators:
  - removeSepBelowNav: false
    $name: Remove separator below top nav
  - removeSepBelowQA: false
    $name: Remove separator below Quick Access
  $name: Separators
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
      125 = 25% larger. Only applies when Fix chevron drawing is enabled.
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
#include <unordered_map>
#include <unordered_set>
#include <vector>

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
    bool hideHome;
    bool hideGallery;
    bool fixChevronDrawing;
    int chevronScale;
    bool hidePinButtons;
    bool removeSepBelowNav;
    bool removeSepBelowQA;
} g_settings;

static PIDLIST_ABSOLUTE g_pidlThisPC = nullptr;
static PIDLIST_ABSOLUTE g_pidlDesktop = nullptr;
static PIDLIST_ABSOLUTE g_pidlHome = nullptr;
static PIDLIST_ABSOLUTE g_pidlGallery = nullptr;
static ULONG_PTR g_gdipToken = 0;
static std::set<HWND> g_subclassedParents;
static COLORREF g_sepColor = CLR_INVALID;
static bool g_sepColorPendingVerify = false;
static bool g_logSepDraw = true;

// Per-tree state: one entry per SysTreeView32 that we've injected items into.
struct TreeState {
    void *pNscTree = nullptr;
    unsigned long enumFlags = 0;
    IShellItemFilter *pFilter = nullptr;
    HTREEITEM hThisPC = nullptr;
    HTREEITEM hDesktop = nullptr;
    HTREEITEM hHome = nullptr;
    HTREEITEM hGallery = nullptr;
    HTREEITEM hiddenDuplicate = nullptr;
    bool dupAtBoundary = false;
    HTREEITEM boundaryItem = nullptr;
    HTREEITEM belowQAItem = nullptr;
    bool qaCleanupDone = false;
    bool qaEverCleaned = false;
    bool dupCollapsesDone = false;
    bool homeGalleryCleanupDone = false;
    bool needHotInsert = false;
    bool needFullRebuild = false;
    bool ownsNscRef = false;
    HIMAGELIST savedStateImageList = nullptr;
};
static std::unordered_map<HWND, TreeState> g_trees;

static TreeState* GetTree(HWND hWnd) {
    auto it = g_trees.find(hWnd);
    return (it != g_trees.end()) ? &it->second : nullptr;
}

static bool IsNavPaneHost(HWND hTree)
{
    for (HWND h = GetAncestor(hTree, GA_ROOT); h; h = nullptr)
    {
        WCHAR cls[64];
        if (GetClassNameW(h, cls, ARRAYSIZE(cls)))
        {
            if (wcscmp(cls, L"CabinetWClass") == 0)
                return true;
            if (wcscmp(cls, L"#32770") == 0)
                return true;
        }
    }
    return false;
}

static bool IsOurSection(const TreeState& ts, HTREEITEM h)
{
    return h == ts.hThisPC || h == ts.hDesktop ||
           (ts.hHome && h == ts.hHome) ||
           (ts.hGallery && h == ts.hGallery);
}

static void ResetTreeCleanup(TreeState& ts)
{
    ts.hiddenDuplicate = nullptr;
    ts.dupAtBoundary = false;
    ts.boundaryItem = nullptr;
    ts.belowQAItem = nullptr;
    ts.qaCleanupDone = false;
    ts.qaEverCleaned = false;
    ts.dupCollapsesDone = false;
    ts.homeGalleryCleanupDone = false;
}

// 0=not ours, 1=This PC, 2=Desktop — set during AppendOneItem
// so the TVM_INSERTITEM handler knows which item is being inserted
// without comparing localized display text.
static int g_insertingItem = 0;

// Temporary globals bridging AppendRoot_hook to TVM_INSERTITEM handler.
// Copied into TreeState once the tree HWND is known.
static void *g_pNscTree = nullptr;
static unsigned long g_lastEnumFlags = 0;
static IShellItemFilter *g_pLastFilter = nullptr;

// Scopes g_insertingItem to a specific tree: TVM_INSERTITEM from other
// trees during message pumping is ignored. Null = accept any tree
// (used by AppendRoot_hook for fresh windows where HWND is unknown).
static HWND g_insertingForTree = nullptr;

// Re-entrancy guard: FullRebuildTree and HotEnableInsert set global
// state (g_insertingItem, g_inCustomAppend, g_pNscTree) that would be
// corrupted if a second tree's deferred op runs while the first is
// in progress (AppendRoot_orig pumps messages internally).
static bool g_deferredOpInProgress = false;
static std::unordered_set<HWND> g_pendingRebuildTrees;

#define WM_DEFERRED_REBUILD  (WM_APP + 0x101)

static void DrainPendingRebuilds()
{
    g_deferredOpInProgress = false;
    while (!g_pendingRebuildTrees.empty())
    {
        HWND hNext = *g_pendingRebuildTrees.begin();
        g_pendingRebuildTrees.erase(g_pendingRebuildTrees.begin());
        if (IsWindow(hNext) && GetTree(hNext))
            PostMessage(hNext, WM_DEFERRED_REBUILD, 0, 0);
    }
}
static void RefreshNavPane(HWND hTree);
static void HotEnableInsert(HWND hWnd);
static void FullRebuildTree(HWND hTree);

static bool ShouldRemoveInternalSep()
{
    return g_settings.showThisPCAtTop && g_settings.showDesktopAtTop;
}

// --- AppendRoot hook ---

using AppendRoot_t = HRESULT (THISCALL *)(
    void *pThis, IShellItem *psiRoot, unsigned long grfEnumFlags,
    unsigned long grfRootStyle, IShellItemFilter *pFilter);

AppendRoot_t AppendRoot_orig;

static bool g_inCustomAppend = false;

static void AppendOneItem(
    void *pThis, PIDLIST_ABSOLUTE pidl, bool expandable,
    unsigned long grfEnumFlags, IShellItemFilter *pOrigFilter,
    unsigned long rootStyle = 0)
{
    if (!pidl)
        return;

    IShellItem *pItem = nullptr;
    if (FAILED(SHCreateItemFromIDList(pidl, IID_IShellItem, (void **)&pItem)))
        return;

    AppendRoot_orig(pThis, pItem,
        expandable ? grfEnumFlags : 0,
        rootStyle,
        pOrigFilter);

    pItem->Release();
}

struct NavItem {
    PIDLIST_ABSOLUTE pidl;
    bool expandable;
    bool enabled;
    int id;
    unsigned long style;
};

static void BuildItemOrder(NavItem items[2])
{
    unsigned long thisPCStyle = g_settings.thisPCStartExpanded ? 0x2 : 0;

    if (g_settings.desktopAboveThisPC)
    {
        items[0] = { g_pidlDesktop, g_settings.desktopExpandable,
                     g_settings.showDesktopAtTop, 2, 0 };
        items[1] = { g_pidlThisPC, g_settings.thisPCExpandable,
                     g_settings.showThisPCAtTop, 1, thisPCStyle };
    }
    else
    {
        items[0] = { g_pidlThisPC, g_settings.thisPCExpandable,
                     g_settings.showThisPCAtTop, 1, thisPCStyle };
        items[1] = { g_pidlDesktop, g_settings.desktopExpandable,
                     g_settings.showDesktopAtTop, 2, 0 };
    }
}

static void InsertItems(void *pNsc, const NavItem items[2],
                        unsigned long enumFlags, IShellItemFilter *filter)
{
    for (int i = 1; i >= 0; i--)
    {
        if (items[i].enabled)
        {
            g_insertingItem = items[i].id;
            AppendOneItem(pNsc, items[i].pidl, items[i].expandable,
                          enumFlags, filter, items[i].style);
            g_insertingItem = 0;
        }
    }
}

HRESULT THISCALL AppendRoot_hook(
    void *pThis, IShellItem *psiRoot, unsigned long grfEnumFlags,
    unsigned long grfRootStyle, IShellItemFilter *pFilter)
{
    // Intercept Home/Gallery root items by PIDL comparison.
    // Must happen BEFORE g_inCustomAppend check: during FullRebuildTree,
    // AppendRoot_orig for the hidden root can trigger Explorer to add
    // Home/Gallery internally while g_inCustomAppend is true. Without
    // this, ts.hHome/ts.hGallery would never be cached.
    if (g_pidlHome || g_pidlGallery)
    {
        PIDLIST_ABSOLUTE pidlRoot = nullptr;
        if (SUCCEEDED(SHGetIDListFromObject(psiRoot, &pidlRoot)))
        {
            bool isHome = g_pidlHome && ILIsEqual(pidlRoot, g_pidlHome);
            bool isGallery = g_pidlGallery && ILIsEqual(pidlRoot, g_pidlGallery);
            CoTaskMemFree(pidlRoot);

            if (isHome || isGallery)
            {
                bool hide = isHome ? g_settings.hideHome : g_settings.hideGallery;
                if (hide)
                {
                    LPWSTR supName = nullptr;
                    psiRoot->GetDisplayName(SIGDN_NORMALDISPLAY, &supName);
                    Wh_Log(L"[HOOK] Suppressing '%s' root",
                           supName ? supName : L"?");
                    if (supName) CoTaskMemFree(supName);
                    return S_OK;
                }
                g_insertingItem = isHome ? 3 : 4;
                HRESULT hr = AppendRoot_orig(pThis, psiRoot, grfEnumFlags,
                                             grfRootStyle, pFilter);
                g_insertingItem = 0;
                return hr;
            }
        }
    }

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
        g_pNscTree = pThis;
        g_lastEnumFlags = grfEnumFlags;
        if (g_pLastFilter != pFilter)
        {
            if (g_pLastFilter) g_pLastFilter->Release();
            g_pLastFilter = pFilter;
            if (g_pLastFilter) g_pLastFilter->AddRef();
        }

        g_inCustomAppend = true;

        NavItem items[2];
        BuildItemOrder(items);
        InsertItems(pThis, items, grfEnumFlags, pFilter);

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
    if (s <= 0) s = 100;
    int nw = MulDiv(w, s, 100);
    int nh = MulDiv(h, s, 100);
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

static bool IsDepth1Item(HWND hTree, HTREEITEM h)
{
    HTREEITEM parent = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                                TVGN_PARENT, (LPARAM)h);
    if (!parent)
        return false;
    HTREEITEM gp = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                            TVGN_PARENT, (LPARAM)parent);
    return (gp == nullptr);
}

static HTREEITEM GetFirstDepth1Child(HWND hTree)
{
    HTREEITEM hRoot = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                              TVGN_ROOT, 0);
    return hRoot ? (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                            TVGN_CHILD, (LPARAM)hRoot)
                 : nullptr;
}

static bool CollapseItemIntegral(HWND hTree, HTREEITEM h)
{
    TVITEMEXW tvi = {};
    tvi.mask = TVIF_HANDLE | TVIF_INTEGRAL;
    tvi.hItem = h;
    SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
    if (tvi.iIntegral >= 2)
    {
        tvi.iIntegral = 1;
        SendMessageW(hTree, TVM_SETITEMW, 0, (LPARAM)&tvi);
        return true;
    }
    return false;
}

static int GetBaseItemHeight(HWND hTree)
{
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
    return baseHeight;
}

static void DrawSeparatorLine(HDC hdc, HWND hTree, int sepY)
{
    RECT client;
    GetClientRect(hTree, &client);
    int pad = 18;
    int sepLeft = (client.right >= pad * 2 + 8) ? pad : 0;
    int sepRight = (client.right >= pad * 2 + 8) ? client.right - pad : client.right;
    RECT sepRect = { sepLeft, sepY, sepRight, sepY + 2 };

    HBRUSH brush = CreateSolidBrush(g_sepColor);
    if (brush)
    {
        FillRect(hdc, &sepRect, brush);
        DeleteObject(brush);
    }
}

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

    int baseHeight = GetBaseItemHeight(hTree);
    if (baseHeight <= 0)
        return CLR_INVALID;

    int tallThreshold = baseHeight + baseHeight / 2;

    // Find the first tall depth-1 item — its separator is at rc.top + baseHeight/2
    HTREEITEM h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                          TVGN_FIRSTVISIBLE, 0);
    while (h)
    {
        if (IsDepth1Item(hTree, h))
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

                    // Sample background color nearby for validation
                    COLORREF bg = GetPixel(hdc, sampleX,
                                           rc.top + baseHeight + baseHeight / 4);

                    auto isPlausible = [&](COLORREF c) -> bool {
                        if (c == CLR_INVALID || c == 0x000000)
                            return false;
                        if (bg == CLR_INVALID)
                            return true;
                        // Reject if separator color is too far from background
                        int dr = abs((int)GetRValue(c) - (int)GetRValue(bg));
                        int dg = abs((int)GetGValue(c) - (int)GetGValue(bg));
                        int db = abs((int)GetBValue(c) - (int)GetBValue(bg));
                        return (dr + dg + db) < 200;
                    };

                    COLORREF c = GetPixel(hdc, sampleX, sepY);
                    if (isPlausible(c))
                        return c;
                    for (int dy = -2; dy <= 2; dy++)
                    {
                        c = GetPixel(hdc, sampleX, sepY + dy);
                        if (isPlausible(c))
                            return c;
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

static void RedrawOtherSeparators(HWND hTree, HDC hdc, TreeState& ts)
{
    if (!hdc || !IsWindow(hTree))
        return;

    RECT client;
    GetClientRect(hTree, &client);
    if (client.right <= 0 || client.bottom <= 0)
        return;

    int baseHeight = GetBaseItemHeight(hTree);
    if (baseHeight <= 0)
        baseHeight = 48;

    int tallThreshold = baseHeight + baseHeight / 2;

    bool passedOurSection = false;
    bool foundBoundary = false;
    bool foundBelowQA = false;
    int sepCount = 0;

    // Diagnostic: build a walk summary for logging
    // Each depth-1 item gets a tag: O=ours, H=home, G=gallery,
    // D=dup(at boundary), d=dup(not at boundary), B=boundary(tall),
    // b=boundary(short), Q=belowQA, S=sep-drawn, .=other
    WCHAR walkLog[64] = {};
    int walkIdx = 0;

    HTREEITEM h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                          TVGN_FIRSTVISIBLE, 0);
    while (h)
    {
        if (IsDepth1Item(hTree, h))
        {
            RECT rc = {};
            *(HTREEITEM*)&rc = h;
            int ih = 0;
            if (SendMessageW(hTree, TVM_GETITEMRECT, FALSE, (LPARAM)&rc))
                ih = rc.bottom - rc.top;

            if (IsOurSection(ts, h))
            {
                passedOurSection = true;
                if (g_logSepDraw && walkIdx < 60)
                {
                    if (h == ts.hThisPC || h == ts.hDesktop) walkLog[walkIdx++] = L'O';
                    else if (ts.hHome && h == ts.hHome) walkLog[walkIdx++] = L'H';
                    else walkLog[walkIdx++] = L'G';
                }
            }
            else if (h == ts.hiddenDuplicate)
            {
                if (g_logSepDraw && walkIdx < 60)
                    walkLog[walkIdx++] = ts.dupAtBoundary ? L'D' : L'd';
            }
            else
            {
                bool isTall = (ih >= tallThreshold);
                bool drawSep = false;
                int sepY = 0;
                WCHAR tag = L'.';

                if (passedOurSection && !foundBoundary)
                {
                    foundBoundary = true;
                    tag = isTall ? L'B' : L'b';
                    if (!g_settings.removeSepBelowNav &&
                        !ts.dupAtBoundary)
                    {
                        drawSep = true;
                        sepY = isTall ? rc.top + baseHeight / 2
                                      : rc.top;
                    }
                }
                else if (foundBoundary && isTall)
                {
                    if (g_settings.removeSepBelowQA && !foundBelowQA)
                    {
                        foundBelowQA = true;
                        tag = L'Q';
                    }
                    else
                    {
                        drawSep = true;
                        sepY = rc.top + baseHeight / 2;
                        tag = L'S';
                    }
                }

                if (drawSep)
                {
                    sepCount++;
                    DrawSeparatorLine(hdc, hTree, sepY);
                }

                if (g_logSepDraw && walkIdx < 60)
                    walkLog[walkIdx++] = tag;
            }
        }

        h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                    TVGN_NEXTVISIBLE, (LPARAM)h);
    }
    walkLog[walkIdx] = 0;
    if (g_logSepDraw)
    {
        Wh_Log(L"[SEP-DRAW] tree=%p walk=[%s] drew=%d hHome=%p hGallery=%p hDup=%p",
               hTree, walkLog, sepCount, ts.hHome, ts.hGallery, ts.hiddenDuplicate);
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
            if (hdr->code == (UINT)NM_CUSTOMDRAW && !g_deferredOpInProgress)
            {
                LPNMTVCUSTOMDRAW cd = (LPNMTVCUSTOMDRAW)lParam;
                DWORD stage = cd->nmcd.dwDrawStage;
                bool hasItemsAtTop = g_settings.showThisPCAtTop ||
                                     g_settings.showDesktopAtTop;
                TreeState* ts = GetTree(hTree);
                bool hasDupHide = (ts && ts->hiddenDuplicate != nullptr);

                if (stage == CDDS_PREPAINT &&
                    (hasItemsAtTop || hasDupHide))
                {
                    // Skipping DefSubclassProc suppresses ALL native separator
                    // lines but also prevents per-item custom draw
                    // (CDDS_ITEMPREPAINT) for other subclasses on this tree.
                    if (hasItemsAtTop && g_sepColor != CLR_INVALID &&
                        !g_sepColorPendingVerify)
                        return CDRF_NOTIFYPOSTPAINT;
                    LRESULT r = DefSubclassProc(hWnd, uMsg, wParam, lParam);
                    return r | CDRF_NOTIFYPOSTPAINT;
                }

                if (stage == CDDS_POSTPAINT)
                {
                    // Re-verify separator color after theme/syscolor
                    // change. CDDS_PREPAINT let native separators
                    // through so we can re-sample. If the color
                    // changed, update and repaint; if same, just
                    // repaint to re-suppress native separators.
                    if (g_sepColorPendingVerify &&
                        g_sepColor != CLR_INVALID &&
                        (hasItemsAtTop || hasDupHide))
                    {
                        g_sepColorPendingVerify = false;
                        COLORREF c = SampleSeparatorColor(hTree, cd->nmcd.hdc);
                        if (c != CLR_INVALID && c != g_sepColor)
                        {
                            Wh_Log(L"[SEP] color changed 0x%06X -> 0x%06X tree=%p",
                                   g_sepColor, c, hTree);
                            g_sepColor = c;
                            g_logSepDraw = true;
                        }
                        InvalidateRect(hTree, NULL, TRUE);
                    }

                    if (g_sepColor == CLR_INVALID &&
                        (hasItemsAtTop || hasDupHide))
                    {
                        COLORREF c = SampleSeparatorColor(hTree, cd->nmcd.hdc);
                        if (c != CLR_INVALID)
                        {
                            g_sepColor = c;
                            g_logSepDraw = true;
                            Wh_Log(L"[SEP] color=0x%06X tree=%p",
                                   c, hTree);
                            InvalidateRect(hTree, NULL, TRUE);
                        }
                    }

                    if (hasItemsAtTop &&
                        g_sepColor != CLR_INVALID && ts)
                        RedrawOtherSeparators(hTree, cd->nmcd.hdc, *ts);

                    // Re-fetch ts: SendMessageW calls in RedrawOtherSeparators
                    // could have modified g_trees
                    ts = GetTree(hTree);
                    hasDupHide = (ts && ts->hiddenDuplicate != nullptr);

                    if (hasDupHide)
                    {
                        RECT rcHide = {};
                        *(HTREEITEM*)&rcHide = ts->hiddenDuplicate;
                        if (SendMessageW(hTree, TVM_GETITEMRECT, FALSE, (LPARAM)&rcHide))
                        {
                            HDC hdc = cd->nmcd.hdc;
                            RECT client;
                            GetClientRect(hTree, &client);
                            COLORREF bg = GetPixel(hdc, client.right / 2, rcHide.top + 2);
                            if (bg == CLR_INVALID || bg == 0x000000)
                                bg = GetPixel(hdc, client.right - 2, rcHide.top + 2);
                            if (bg == CLR_INVALID || bg == 0x000000)
                                bg = GetPixel(hdc, 1, rcHide.top + 2);
                            if (bg == CLR_INVALID)
                                bg = (COLORREF)SendMessageW(hTree, TVM_GETBKCOLOR, 0, 0);
                            if (bg == CLR_INVALID || bg == (COLORREF)-1)
                                bg = GetSysColor(COLOR_WINDOW);
                            if (bg != CLR_INVALID)
                            {
                                HBRUSH bgBrush = CreateSolidBrush(bg);
                                if (bgBrush)
                                {
                                    FillRect(hdc, &rcHide, bgBrush);
                                    DeleteObject(bgBrush);
                                }
                            }

                            // Only draw separator line through the dup
                            // if it's at the boundary (right after our
                            // section). Otherwise just paint over.
                            if (g_sepColor != CLR_INVALID)
                            {
                                HTREEITEM hPrev = (HTREEITEM)SendMessageW(
                                    hTree, TVM_GETNEXTITEM, TVGN_PREVIOUS,
                                    (LPARAM)ts->hiddenDuplicate);
                                bool atBoundary = hPrev && IsOurSection(*ts, hPrev);

                                if (atBoundary)
                                {
                                    int h = rcHide.bottom - rcHide.top;
                                    int sepY = rcHide.top + h / 2;
                                    DrawSeparatorLine(hdc, hTree, sepY);
                                }
                            }
                            if (g_logSepDraw)
                            {
                                Wh_Log(L"[SEP-HIDE] dup=%p bg=0x%06X sepColor=0x%06X y=%d-%d",
                                       ts->hiddenDuplicate, bg,
                                       g_sepColor != CLR_INVALID ? g_sepColor : 0,
                                       rcHide.top, rcHide.bottom);
                            }
                        }
                    }

                    g_logSepDraw = false;
                }
            }

            // After expand/collapse, force full repaint so stale
            // separator lines at old positions get overwritten.
            if (hdr->code == TVN_ITEMEXPANDEDW ||
                hdr->code == TVN_ITEMEXPANDEDA)
            {
                LRESULT r = DefSubclassProc(hWnd, uMsg, wParam, lParam);
                g_logSepDraw = true;
                TreeState* tsExp = GetTree(hTree);
                if (tsExp)
                    tsExp->dupCollapsesDone = false;
                InvalidateRect(hTree, NULL, TRUE);
                return r;
            }

            if (hdr->code == (UINT)TVN_SELCHANGEDW ||
                hdr->code == (UINT)TVN_SELCHANGEDA)
            {
                LPNMTREEVIEWW nm = (LPNMTREEVIEWW)lParam;
                TreeState* tsSel = GetTree(hTree);
                if (tsSel && tsSel->hiddenDuplicate &&
                    nm->itemNew.hItem == tsSel->hiddenDuplicate)
                {
                    HTREEITEM hAlt = (HTREEITEM)SendMessageW(
                        hTree, TVM_GETNEXTITEM, TVGN_NEXTVISIBLE,
                        (LPARAM)tsSel->hiddenDuplicate);
                    if (!hAlt)
                        hAlt = (HTREEITEM)SendMessageW(
                            hTree, TVM_GETNEXTITEM, TVGN_PREVIOUSVISIBLE,
                            (LPARAM)tsSel->hiddenDuplicate);
                    if (hAlt)
                        SendMessageW(hTree, TVM_SELECTITEM,
                                     TVGN_CARET, (LPARAM)hAlt);
                }
            }
        }
    }

    if (uMsg == WM_THEMECHANGED || uMsg == WM_SYSCOLORCHANGE)
    {
        g_sepColorPendingVerify = true;
    }

    if (uMsg == WM_NCDESTROY)
        g_subclassedParents.erase(hWnd);

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
    // Always let CNscTree accept the image list so it remains alive and
    // can be cached by WM_PAINT (which hides it before the tree draws).
    return SetStateImageList_orig(pThis, himl);
}

static void GetPidlDisplayName(PIDLIST_ABSOLUTE pidl, WCHAR *buf, int len)
{
    IShellItem *psi = nullptr;
    if (SUCCEEDED(SHCreateItemFromIDList(pidl, IID_IShellItem,
                                         (void **)&psi)) && psi)
    {
        LPWSTR name = nullptr;
        if (SUCCEEDED(psi->GetDisplayName(SIGDN_NORMALDISPLAY, &name))
            && name)
        {
            wcsncpy_s(buf, len, name, _TRUNCATE);
            CoTaskMemFree(name);
        }
        psi->Release();
    }
}

static void GetItemText(HWND hTree, HTREEITEM h, WCHAR *buf, int len)
{
    TVITEMEXW tvi = {};
    tvi.mask = TVIF_HANDLE | TVIF_TEXT;
    tvi.hItem = h;
    tvi.pszText = buf;
    tvi.cchTextMax = len;
    SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
}

static void ResolveHomeGalleryNames(WCHAR *homeName, int homeLen,
                                     WCHAR *galleryName, int galleryLen)
{
    if (g_pidlHome) GetPidlDisplayName(g_pidlHome, homeName, homeLen);
    if (g_pidlGallery) GetPidlDisplayName(g_pidlGallery, galleryName, galleryLen);
}

// --- Quick Access item hiding ---
// Walk depth-1 children of the hidden root and delete items matching
// our items' display text. Handles two kinds of duplicates:
// - Childless QA pins: kept as hiddenDuplicate (separator mechanism)
//   when removeSepBelowNav is off, otherwise deleted.
// - Native sections with children (e.g. the native "This PC" below
//   Quick Access): always deleted — our top-level items replace them.
//
// Returns true when all expected childless duplicates were found.
// Returns false if some are still missing (async population race).
static bool CleanupQuickAccessDuplicates(HWND hTree, TreeState& ts)
{
    if (!ts.hThisPC && !ts.hDesktop)
        return true;

    WCHAR thisPCText[64] = {};
    WCHAR desktopText[64] = {};
    int expectedCount = 0;

    if (g_settings.showThisPCAtTop &&
        g_settings.hideThisPCFromQuickAccess && ts.hThisPC)
    {
        GetItemText(hTree, ts.hThisPC, thisPCText, ARRAYSIZE(thisPCText));
        if (thisPCText[0]) expectedCount++;
    }

    if (g_settings.showDesktopAtTop &&
        g_settings.hideDesktopFromQuickAccess && ts.hDesktop)
    {
        GetItemText(hTree, ts.hDesktop, desktopText, ARRAYSIZE(desktopText));
        if (desktopText[0]) expectedCount++;
    }

    if (!expectedCount)
        return true;

    HTREEITEM h = GetFirstDepth1Child(hTree);
    if (!h) return false;

    HTREEITEM toDeleteChildless[8] = {};
    HTREEITEM toDeleteSections[4] = {};
    int childlessCount = 0;
    int sectionCount = 0;

    // Find the boundary position: first non-our/non-HG depth-1 child
    // after our section. A dup can only be kept as hiddenDuplicate if
    // it occupies this exact position.
    //
    // ts.hHome/hGallery may be null at this point (COM vtable bypass),
    // so resolve Home/Gallery display names from PIDLs to identify them.
    WCHAR homeName[64] = {};
    WCHAR galleryName[64] = {};
    ResolveHomeGalleryNames(homeName, ARRAYSIZE(homeName),
                            galleryName, ARRAYSIZE(galleryName));

    HTREEITEM hBoundaryPos = nullptr;
    {
        bool passedOurs = false;
        HTREEITEM hWalk = h;
        while (hWalk)
        {
            bool isOursOrHG = IsOurSection(ts, hWalk);

            if (!isOursOrHG && (homeName[0] || galleryName[0]))
            {
                WCHAR text[64] = {};
                GetItemText(hTree, hWalk, text, ARRAYSIZE(text));
                if ((homeName[0] && wcscmp(text, homeName) == 0) ||
                    (galleryName[0] && wcscmp(text, galleryName) == 0))
                    isOursOrHG = true;
            }

            if (isOursOrHG)
                passedOurs = true;
            else if (passedOurs)
            {
                hBoundaryPos = hWalk;
                break;
            }
            hWalk = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                             TVGN_NEXT, (LPARAM)hWalk);
        }
    }

    while (h)
    {
        HTREEITEM hNext = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                                   TVGN_NEXT, (LPARAM)h);
        if (h != ts.hThisPC && h != ts.hDesktop)
        {
            WCHAR text[64] = {};
            GetItemText(hTree, h, text, ARRAYSIZE(text));

            bool match = false;
            if (thisPCText[0] && wcscmp(text, thisPCText) == 0)
                match = true;
            if (desktopText[0] && wcscmp(text, desktopText) == 0)
                match = true;

            if (match)
            {
                HTREEITEM hChild = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                                            TVGN_CHILD, (LPARAM)h);
                if (!hChild)
                {
                    if (childlessCount < 8)
                        toDeleteChildless[childlessCount++] = h;
                }
                else
                {
                    if (sectionCount < 4)
                        toDeleteSections[sectionCount++] = h;
                }
            }
        }
        h = hNext;
    }

    // Freeze redraw during deletions to prevent Explorer from
    // re-inserting items synchronously (which would reset cleanup
    // flags via the TVM_INSERTITEM handler and cause a loop).
    bool needDelete = (sectionCount > 0);
    for (int i = 0; i < childlessCount && !needDelete; i++)
    {
        bool wouldKeep = (!g_settings.removeSepBelowNav &&
                          toDeleteChildless[i] == hBoundaryPos);
        if (!wouldKeep)
            needDelete = true;
    }
    if (needDelete)
        SendMessageW(hTree, WM_SETREDRAW, FALSE, 0);

    // Native sections with children: always delete (our items replace them)
    for (int i = 0; i < sectionCount; i++)
    {
        Wh_Log(L"[QA-HIDE] deleting native section=%p", toDeleteSections[i]);
        SendMessageW(hTree, TVM_DELETEITEM, 0, (LPARAM)toDeleteSections[i]);
    }

    // Only keep a childless match as hiddenDuplicate if it IS the
    // boundary item (first non-our item after our section). A dup in
    // the middle of QA would be painted over but leave a blank gap
    // with no separator, since RedrawOtherSeparators skips the boundary
    // when hiddenDuplicate exists.
    HTREEITEM prevDup = ts.hiddenDuplicate;
    ts.hiddenDuplicate = nullptr;
    ts.dupAtBoundary = false;
    for (int i = 0; i < childlessCount; i++)
    {
        if (!g_settings.removeSepBelowNav && !ts.hiddenDuplicate &&
            toDeleteChildless[i] == hBoundaryPos)
        {
            ts.hiddenDuplicate = toDeleteChildless[i];
            TVITEMEXW fix = {};
            fix.mask = TVIF_HANDLE | TVIF_INTEGRAL;
            fix.hItem = toDeleteChildless[i];
            fix.iIntegral = 1;
            SendMessageW(hTree, TVM_SETITEMW, 0, (LPARAM)&fix);
            if (ts.hiddenDuplicate != prevDup)
                Wh_Log(L"[QA-HIDE] keeping dup=%p at boundary (iIntegral=1)",
                       toDeleteChildless[i]);
            continue;
        }
        Wh_Log(L"[QA-HIDE] deleting duplicate item=%p (boundary=%p)",
               toDeleteChildless[i], hBoundaryPos);
        SendMessageW(hTree, TVM_DELETEITEM, 0, (LPARAM)toDeleteChildless[i]);
    }

    if (needDelete)
    {
        SendMessageW(hTree, WM_SETREDRAW, TRUE, 0);
        InvalidateRect(hTree, nullptr, TRUE);
    }

    if (ts.hiddenDuplicate != prevDup || sectionCount > 0)
        Wh_Log(L"[QA-HIDE] tree=%p childless=%d sections=%d expected=%d dup=%p",
               hTree, childlessCount, sectionCount, expectedCount,
               ts.hiddenDuplicate);

    return (childlessCount >= expectedCount);
}

// Walk depth-1 children and cache Home/Gallery handles by matching
// PIDL-derived display names. Called when handles are unknown (e.g.,
// after FullRebuildTree where items existed before our hooks).
static void CacheHomeGalleryHandles(HWND hTree, TreeState& ts)
{
    if (ts.hHome && ts.hGallery)
        return;

    bool needHome = !ts.hHome && g_pidlHome && !g_settings.hideHome;
    bool needGallery = !ts.hGallery && g_pidlGallery && !g_settings.hideGallery;
    if (!needHome && !needGallery)
        return;

    WCHAR homeName[64] = {};
    WCHAR galleryName[64] = {};
    ResolveHomeGalleryNames(homeName, ARRAYSIZE(homeName),
                            galleryName, ARRAYSIZE(galleryName));

    HTREEITEM h = GetFirstDepth1Child(hTree);
    if (!h) return;

    while (h)
    {
        if (h != ts.hThisPC && h != ts.hDesktop)
        {
            WCHAR text[64] = {};
            GetItemText(hTree, h, text, ARRAYSIZE(text));

            if (needHome && homeName[0] && wcscmp(text, homeName) == 0)
            {
                ts.hHome = h;
                needHome = false;
                Wh_Log(L"[CACHE] Home item=%p tree=%p (walk)", h, hTree);
            }
            else if (needGallery && galleryName[0] &&
                     wcscmp(text, galleryName) == 0)
            {
                ts.hGallery = h;
                needGallery = false;
                Wh_Log(L"[CACHE] Gallery item=%p tree=%p (walk)", h, hTree);
            }

            if (!needHome && !needGallery)
                break;
        }
        h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                    TVGN_NEXT, (LPARAM)h);
    }
}

// Walk depth-1 items and delete Home/Gallery by matching their
// display names derived from PIDLs (localization-agnostic).
// Returns true when all expected items have been found and removed.
static bool RemoveHomeGalleryItems(HWND hTree, TreeState& ts)
{
    if (!IsWindow(hTree))
        return true;

    bool wantHome = g_settings.hideHome && g_pidlHome;
    bool wantGallery = g_settings.hideGallery && g_pidlGallery;
    if (!wantHome && !wantGallery)
        return true;

    WCHAR homeName[64] = {};
    WCHAR galleryName[64] = {};
    ResolveHomeGalleryNames(homeName, ARRAYSIZE(homeName),
                            galleryName, ARRAYSIZE(galleryName));

    int expectedCount = 0;
    if (wantHome && homeName[0]) expectedCount++;
    if (wantGallery && galleryName[0]) expectedCount++;
    if (!expectedCount) return true;

    HTREEITEM h = GetFirstDepth1Child(hTree);
    if (!h) return false;
    struct { HTREEITEM h; bool isHome; } toDelete[2] = {};
    int delCount = 0;

    while (h && delCount < 2)
    {
        HTREEITEM hNext = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM,
                                                   TVGN_NEXT, (LPARAM)h);
        if (h != ts.hThisPC && h != ts.hDesktop &&
            h != ts.hiddenDuplicate)
        {
            WCHAR text[64] = {};
            GetItemText(hTree, h, text, ARRAYSIZE(text));

            if (text[0])
            {
                bool isHome = wantHome && homeName[0] &&
                              wcscmp(text, homeName) == 0;
                bool isGallery = wantGallery && galleryName[0] &&
                                 wcscmp(text, galleryName) == 0;
                if (isHome || isGallery)
                    toDelete[delCount++] = { h, isHome };
            }
        }
        h = hNext;
    }

    if (delCount > 0)
        SendMessageW(hTree, WM_SETREDRAW, FALSE, 0);

    for (int i = 0; i < delCount; i++)
    {
        Wh_Log(L"[HIDE] Deleting '%s' item=%p",
               toDelete[i].isHome ? homeName : galleryName, toDelete[i].h);
        SendMessageW(hTree, TVM_DELETEITEM, 0, (LPARAM)toDelete[i].h);
        if (toDelete[i].isHome)
            ts.hHome = nullptr;
        else
            ts.hGallery = nullptr;
    }

    if (delCount > 0)
    {
        SendMessageW(hTree, WM_SETREDRAW, TRUE, 0);
        InvalidateRect(hTree, nullptr, TRUE);
    }

    return (delCount >= expectedCount);
}

// Tree subclass installed via SetWindowSubclass — runs BEFORE
// Explorer's other subclasses (LIFO order), blocking right-click
// on the hidden duplicate.
// TODO: instead of blocking, redirect to empty space and reposition
// the resulting background context menu to the click point.
static LRESULT CALLBACK TreeInteractionProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    TreeState* ts = GetTree(hWnd);
    HTREEITEM hiddenDup = ts ? ts->hiddenDuplicate : nullptr;

    if (hiddenDup &&
        (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP ||
         uMsg == WM_MOUSEMOVE))
    {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        TVHITTESTINFO ht = {};
        ht.pt = pt;
        HTREEITEM hHit = (HTREEITEM)SendMessageW(
            hWnd, TVM_HITTEST, 0, (LPARAM)&ht);
        if (hHit == hiddenDup)
            return 0;
    }

    if (uMsg == WM_NCDESTROY)
        RemoveWindowSubclass(hWnd, TreeInteractionProc, uIdSubclass);

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
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
        bool isOurInsert = (g_insertingItem >= 1 && g_insertingItem <= 4) &&
                           (!g_insertingForTree || hWnd == g_insertingForTree);

        if (isOurInsert && (g_insertingItem == 1 || g_insertingItem == 2) && lParam)
        {
            TVINSERTSTRUCTW *pInsert = (TVINSERTSTRUCTW *)lParam;
            pInsert->hInsertAfter = TVI_FIRST;
        }

        LRESULT result = SubClassTreeWndProc_orig(hWnd, uMsg, wParam,
                            lParam, uIdSubclass, dwRefData);
        HTREEITEM hNew = (HTREEITEM)result;
        if (hNew)
        {
            if (isOurInsert && (g_insertingItem == 1 || g_insertingItem == 2))
            {
                bool isThisPC = (g_insertingItem == 1);
                g_insertingItem = 0;
                if (!GetTree(hWnd) && !IsNavPaneHost(hWnd))
                    return result;
                auto [it, _] = g_trees.try_emplace(hWnd);
                TreeState& ts = it->second;
                (isThisPC ? ts.hThisPC : ts.hDesktop) = hNew;
                ts.qaCleanupDone = false;
                ts.hiddenDuplicate = nullptr;
                ts.dupAtBoundary = false;
                if (!ts.pNscTree)
                {
                    ts.pNscTree = g_pNscTree;
                    ts.enumFlags = g_lastEnumFlags;
                    if (g_pLastFilter && !ts.pFilter)
                    {
                        g_pLastFilter->AddRef();
                        ts.pFilter = g_pLastFilter;
                    }
                }
                SetPropW(hWnd, L"WH_NscTree", (HANDLE)ts.pNscTree);
                SetPropW(hWnd, L"WH_EnumFlags", (HANDLE)(ULONG_PTR)ts.enumFlags);
                Wh_Log(L"[CACHE] %s item=%p tree=%p",
                       isThisPC ? L"This PC" : L"Desktop", hNew, hWnd);
            }
            else if (isOurInsert && (g_insertingItem == 3 || g_insertingItem == 4))
            {
                bool isHome = (g_insertingItem == 3);
                g_insertingItem = 0;
                if (!GetTree(hWnd) && !IsNavPaneHost(hWnd))
                    return result;
                auto [it, _] = g_trees.try_emplace(hWnd);
                TreeState& ts = it->second;
                (isHome ? ts.hHome : ts.hGallery) = hNew;
                WCHAR itemText[64] = {};
                GetItemText(hWnd, hNew, itemText, ARRAYSIZE(itemText));
                Wh_Log(L"[CACHE] '%s' item=%p tree=%p", itemText, hNew, hWnd);
                bool hidden = isHome ? g_settings.hideHome : g_settings.hideGallery;
                if (!hidden &&
                    (g_settings.showThisPCAtTop || g_settings.showDesktopAtTop))
                {
                    TVITEMEXW fix = {};
                    fix.mask = TVIF_HANDLE | TVIF_INTEGRAL;
                    fix.hItem = hNew;
                    fix.iIntegral = 1;
                    SendMessageW(hWnd, TVM_SETITEMW, 0, (LPARAM)&fix);
                }
            }
            else
            {
                TreeState* ts = GetTree(hWnd);
                if (ts && IsDepth1Item(hWnd, hNew))
                {
                    if (g_settings.hideHome || g_settings.hideGallery)
                    {
                        WCHAR homeName[64] = {}, galleryName[64] = {};
                        ResolveHomeGalleryNames(homeName, ARRAYSIZE(homeName),
                                                galleryName, ARRAYSIZE(galleryName));
                        WCHAR itemText[64] = {};
                        GetItemText(hWnd, hNew, itemText, ARRAYSIZE(itemText));
                        bool isHome = g_settings.hideHome && homeName[0] &&
                                      wcscmp(itemText, homeName) == 0;
                        bool isGallery = g_settings.hideGallery && galleryName[0] &&
                                         wcscmp(itemText, galleryName) == 0;
                        if (isHome || isGallery)
                        {
                            bool nearOurSection = false;
                            HTREEITEM hPrev = hNew;
                            for (int walk = 0; walk < 6 && hPrev; walk++)
                            {
                                hPrev = (HTREEITEM)SendMessageW(hWnd, TVM_GETNEXTITEM,
                                    TVGN_PREVIOUS, (LPARAM)hPrev);
                                if (hPrev && IsOurSection(*ts, hPrev))
                                {
                                    nearOurSection = true;
                                    break;
                                }
                            }
                            if (nearOurSection)
                            {
                                ts->homeGalleryCleanupDone = false;
                                Wh_Log(L"[INSERT] '%s' item=%p tree=%p",
                                       itemText, hNew, hWnd);
                            }
                        }
                    }
                    if (!ts->qaEverCleaned && ts->qaCleanupDone)
                    {
                        ts->qaCleanupDone = false;
                        ts->hiddenDuplicate = nullptr;
                        ts->dupAtBoundary = false;
                    }
                }
            }
        }
        return result;
    }

    // Block all interaction with hidden duplicate
    {
        TreeState* ts = GetTree(hWnd);
        HTREEITEM hiddenDup = ts ? ts->hiddenDuplicate : nullptr;

        if (hiddenDup &&
            (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN ||
             uMsg == WM_RBUTTONUP || uMsg == WM_CONTEXTMENU ||
             uMsg == WM_SETCURSOR))
        {
            POINT pt = {};
            bool checkHit = true;
            if (uMsg == WM_CONTEXTMENU)
            {
                pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                if (pt.x == -1 && pt.y == -1)
                    checkHit = false;
                else
                    ScreenToClient(hWnd, &pt);
            }
            else if (uMsg == WM_SETCURSOR)
            {
                GetCursorPos(&pt);
                ScreenToClient(hWnd, &pt);
            }
            else
            {
                pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            }
            if (checkHit)
            {
                TVHITTESTINFO ht = {};
                ht.pt = pt;
                HTREEITEM hHit = (HTREEITEM)SendMessageW(hWnd, TVM_HITTEST, 0, (LPARAM)&ht);
                if (hHit == hiddenDup)
                {
                    if (uMsg == WM_SETCURSOR)
                    {
                        SetCursor(LoadCursorW(nullptr, IDC_ARROW));
                        return TRUE;
                    }
                    return 0;
                }
            }
        }

        // Skip hidden duplicate on keyboard navigation
        if (uMsg == WM_KEYDOWN && hiddenDup &&
            (wParam == VK_UP || wParam == VK_DOWN))
        {
            LRESULT r = SubClassTreeWndProc_orig(hWnd, uMsg, wParam, lParam,
                                                  uIdSubclass, dwRefData);
            HTREEITEM hSel = (HTREEITEM)SendMessageW(hWnd, TVM_GETNEXTITEM,
                                                       TVGN_CARET, 0);
            if (hSel == hiddenDup)
            {
                UINT dir = (wParam == VK_DOWN) ? TVGN_NEXTVISIBLE : TVGN_PREVIOUSVISIBLE;
                HTREEITEM hNext = (HTREEITEM)SendMessageW(hWnd, TVM_GETNEXTITEM,
                                                            dir, (LPARAM)hSel);
                if (hNext)
                    SendMessageW(hWnd, TVM_SELECTITEM, TVGN_CARET, (LPARAM)hNext);
            }
            return r;
        }
    }

    if (uMsg == TVM_SETITEMW || uMsg == TVM_SETITEMA)
    {
        TVITEMEXW* tvi = (TVITEMEXW*)lParam;
        if (tvi && (tvi->mask & TVIF_INTEGRAL))
        {
            TreeState* ts = GetTree(hWnd);
            HTREEITEM hiddenDup = ts ? ts->hiddenDuplicate : nullptr;
            HTREEITEM boundaryIt = ts ? ts->boundaryItem : nullptr;
            HTREEITEM belowQAIt = ts ? ts->belowQAItem : nullptr;

            // Keep hidden duplicate at iIntegral=1 so it occupies
            // exactly one row of space for the painted-over separator.
            if (hiddenDup && tvi->hItem == hiddenDup &&
                tvi->iIntegral != 1)
            {
                tvi->iIntegral = 1;
            }

            if (boundaryIt && tvi->hItem == boundaryIt &&
                tvi->iIntegral >= 2 &&
                (g_settings.removeSepBelowNav ||
                 ts->dupAtBoundary))
            {
                tvi->iIntegral = 1;
            }

            if (belowQAIt && tvi->hItem == belowQAIt &&
                tvi->iIntegral >= 2)
            {
                tvi->iIntegral = 1;
            }

            if (ts && tvi->iIntegral >= 2 &&
                (g_settings.showThisPCAtTop || g_settings.showDesktopAtTop))
            {
                bool isVisibleHome = ts->hHome && tvi->hItem == ts->hHome &&
                                     !g_settings.hideHome;
                bool isVisibleGallery = ts->hGallery && tvi->hItem == ts->hGallery &&
                                        !g_settings.hideGallery;
                if (isVisibleHome || isVisibleGallery)
                    tvi->iIntegral = 1;
            }

            // Collapse the boundary between Desktop and This PC.
            if (ShouldRemoveInternalSep() && tvi->iIntegral >= 2 && ts)
            {
                HTREEITEM hA = ts->hThisPC;
                HTREEITEM hB = ts->hDesktop;
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
                            tvi->iIntegral = 1;
                    }
                }
            }
        }
    }

    if (uMsg == WM_DEFERRED_REBUILD)
    {
        if (g_deferredOpInProgress)
        {
            g_pendingRebuildTrees.insert(hWnd);
            return 0;
        }

        TreeState* ts = GetTree(hWnd);
        if (ts && ts->needFullRebuild)
        {
            g_deferredOpInProgress = true;
            ts->needFullRebuild = false;
            FullRebuildTree(hWnd);
            ts = GetTree(hWnd);
            DrainPendingRebuilds();
        }
        if (ts && ts->needHotInsert)
        {
            g_deferredOpInProgress = true;
            ts->needHotInsert = false;
            HotEnableInsert(hWnd);
            DrainPendingRebuilds();
        }
        return 0;
    }

    if (uMsg == WM_PAINT)
    {
        TreeState* ts = GetTree(hWnd);

        // Full rebuild deferred from settings change or missed message.
        // Guarded to prevent re-entrancy (AppendRoot_orig pumps messages).
        if (ts && ts->needFullRebuild && !g_deferredOpInProgress)
        {
            g_deferredOpInProgress = true;
            ts->needFullRebuild = false;
            FullRebuildTree(hWnd);
            ts = GetTree(hWnd);
            DrainPendingRebuilds();
        }

        // Hot-enable deferred insertion: items are inserted on the UI
        // thread so TVM_INSERTITEM fires through the subclass (TVI_FIRST
        // + caching work). Items land as children of the hidden root.
        // Guarded: same re-entrancy risk as FullRebuildTree.
        if (ts && ts->needHotInsert && !g_deferredOpInProgress)
        {
            g_deferredOpInProgress = true;
            ts->needHotInsert = false;
            HotEnableInsert(hWnd);
            ts = GetTree(hWnd);
            DrainPendingRebuilds();
        }

        // Cache Home/Gallery handles if unknown (items exist in tree
        // but were added before our hooks, e.g., after FullRebuildTree).
        if (ts && ((g_pidlHome && !ts->hHome && !g_settings.hideHome) ||
                   (g_pidlGallery && !ts->hGallery && !g_settings.hideGallery)))
            CacheHomeGalleryHandles(hWnd, *ts);

        if (ts && !ts->qaCleanupDone &&
            ((g_settings.showThisPCAtTop && g_settings.hideThisPCFromQuickAccess) ||
             (g_settings.showDesktopAtTop && g_settings.hideDesktopFromQuickAccess)))
        {
            ts->qaCleanupDone = CleanupQuickAccessDuplicates(hWnd, *ts);
            if (ts->qaCleanupDone)
                ts->qaEverCleaned = true;
        }

        // Collapse visible duplicates of our items so they don't
        // mirror the expanded state of our top copies. Runs whenever
        // a dup is visible: parent off (dup never hidden) or parent
        // on but not hiding from nav. Uses PIDL fallback for names
        // when our item handle doesn't exist.
        if (ts)
        {
            WCHAR collapseText[2][64] = {};
            int collapseCount = 0;

            auto getTextFromItem = [&](HTREEITEM h) {
                if (!h || collapseCount >= 2) return;
                GetItemText(hWnd, h, collapseText[collapseCount], 64);
                if (collapseText[collapseCount][0]) collapseCount++;
            };

            if (!g_settings.showThisPCAtTop ||
                !g_settings.hideThisPCFromQuickAccess)
            {
                if (ts->hThisPC)
                    getTextFromItem(ts->hThisPC);
                else if (g_pidlThisPC && collapseCount < 2)
                {
                    GetPidlDisplayName(g_pidlThisPC,
                                       collapseText[collapseCount], 64);
                    if (collapseText[collapseCount][0]) collapseCount++;
                }
            }
            if (!g_settings.showDesktopAtTop ||
                !g_settings.hideDesktopFromQuickAccess)
            {
                if (ts->hDesktop)
                    getTextFromItem(ts->hDesktop);
                else if (g_pidlDesktop && collapseCount < 2)
                {
                    GetPidlDisplayName(g_pidlDesktop,
                                       collapseText[collapseCount], 64);
                    if (collapseText[collapseCount][0]) collapseCount++;
                }
            }

            if (collapseCount > 0 && !ts->dupCollapsesDone)
            {
                ts->dupCollapsesDone = true;
                HTREEITEM h = GetFirstDepth1Child(hWnd);
                while (h)
                {
                    if (h != ts->hThisPC && h != ts->hDesktop)
                    {
                        WCHAR text[64] = {};
                        TVITEMEXW tvi = {};
                        tvi.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_STATE;
                        tvi.stateMask = TVIS_EXPANDED;
                        tvi.hItem = h;
                        tvi.pszText = text;
                        tvi.cchTextMax = 64;
                        SendMessageW(hWnd, TVM_GETITEMW, 0, (LPARAM)&tvi);

                        if ((tvi.state & TVIS_EXPANDED) && text[0])
                        {
                            for (int j = 0; j < collapseCount; j++)
                            {
                                if (wcscmp(text, collapseText[j]) == 0)
                                {
                                    SendMessageW(hWnd, TVM_EXPAND, TVE_COLLAPSE, (LPARAM)h);
                                    break;
                                }
                            }
                        }
                    }
                    h = (HTREEITEM)SendMessageW(hWnd, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)h);
                }
            }
        }

        // Remove Home/Gallery items by PIDL (children of hidden root)
        if (ts && !ts->homeGalleryCleanupDone &&
            ts->pNscTree && (g_pidlHome || g_pidlGallery) &&
            (g_settings.hideHome || g_settings.hideGallery))
        {
            ts->homeGalleryCleanupDone = RemoveHomeGalleryItems(hWnd, *ts);
        }

        // Collapse iIntegral on visible Home/Gallery so there's no
        // space between our items and them.
        if (ts && (g_settings.showThisPCAtTop || g_settings.showDesktopAtTop))
        {
            if (ts->hHome && !g_settings.hideHome)
                CollapseItemIntegral(hWnd, ts->hHome);
            if (ts->hGallery && !g_settings.hideGallery)
                CollapseItemIntegral(hWnd, ts->hGallery);
        }

        // Find and optionally collapse separator boundary items.
        // Always runs when we have items at top (to track boundaryItem
        // for RedrawOtherSeparators). Only COLLAPSES iIntegral when the
        // user wants to remove the separator:
        // - boundary: collapse when removeSepBelowNav (otherwise its
        //   iIntegral=2 provides space for the separator we draw)
        // - below-QA: collapse when removeSepBelowQA
        // Deferred until g_sepColor is captured so tall items remain
        // for the sampling pass on the first paint cycle.
        if ((g_settings.showThisPCAtTop || g_settings.showDesktopAtTop) &&
            g_sepColor != CLR_INVALID && ts)
        {
            HTREEITEM hChild = GetFirstDepth1Child(hWnd);
            if (hChild)
            {
                bool passedOurs = false;
                bool foundBoundary = false;
                while (hChild)
                {
                    bool isOursOrHG = IsOurSection(*ts, hChild);

                    if (isOursOrHG)
                    {
                        passedOurs = true;
                    }
                    else if (passedOurs && hChild != ts->hiddenDuplicate)
                    {
                        if (!foundBoundary)
                        {
                            ts->boundaryItem = hChild;
                            if (ts->hiddenDuplicate)
                            {
                                HTREEITEM hPrev = (HTREEITEM)SendMessageW(
                                    hWnd, TVM_GETNEXTITEM, TVGN_PREVIOUS,
                                    (LPARAM)hChild);
                                ts->dupAtBoundary = (hPrev == ts->hiddenDuplicate);
                            }
                            if (g_settings.removeSepBelowNav ||
                                ts->dupAtBoundary)
                            {
                                CollapseItemIntegral(hWnd, hChild);
                            }
                            foundBoundary = true;
                            if (!g_settings.removeSepBelowQA)
                                break;
                        }
                        else if (CollapseItemIntegral(hWnd, hChild))
                        {
                            ts->belowQAItem = hChild;
                            break;
                        }
                    }
                    hChild = (HTREEITEM)SendMessageW(
                        hWnd, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hChild);
                }
            }
        }

        if (g_settings.hidePinButtons && ts)
        {
            HIMAGELIST hState = (HIMAGELIST)SendMessageW(
                hWnd, TVM_GETIMAGELIST, TVSIL_STATE, 0);
            if (hState)
            {
                ts->savedStateImageList = hState;
                SendMessageW(hWnd, TVM_SETIMAGELIST, TVSIL_STATE, 0);
            }
        }

        if (g_settings.showThisPCAtTop || g_settings.showDesktopAtTop)
            EnsureParentSubclass(hWnd);

        // Install tree subclass for right-click blocking on hidden dup
        if (ts && ts->hiddenDuplicate)
            SetWindowSubclass(hWnd, TreeInteractionProc, 0xAF01, 0);

        if (g_settings.fixChevronDrawing)
            g_inTreePaint = true;

        LRESULT result = SubClassTreeWndProc_orig(hWnd, uMsg, wParam,
                            lParam, uIdSubclass, dwRefData);

        g_inTreePaint = false;

        return result;
    }

    if (uMsg == WM_NCDESTROY)
    {
        auto it = g_trees.find(hWnd);
        if (it != g_trees.end())
        {
            RemoveWindowSubclass(hWnd, TreeInteractionProc, 0xAF01);
            IShellItemFilter *pf = it->second.pFilter;
            it->second.pFilter = nullptr;
            INameSpaceTreeControl *pNsc = nullptr;
            if (it->second.ownsNscRef && it->second.pNscTree)
                pNsc = (INameSpaceTreeControl *)it->second.pNscTree;
            it->second.pNscTree = nullptr;
            g_trees.erase(it);
            if (pf)
                pf->Release();
            if (pNsc)
                pNsc->Release();
        }
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
    g_settings.hideHome = Wh_GetIntSetting(L"HomeGallery.hideHome");
    g_settings.hideGallery = Wh_GetIntSetting(L"HomeGallery.hideGallery");
    g_settings.fixChevronDrawing = Wh_GetIntSetting(L"Resources.fixChevronDrawing");
    g_settings.chevronScale = Wh_GetIntSetting(L"Resources.chevronScale");
    g_settings.hidePinButtons = Wh_GetIntSetting(L"Resources.hidePinButtons");
    g_settings.removeSepBelowNav = Wh_GetIntSetting(L"Separators.removeSepBelowNav");
    g_settings.removeSepBelowQA = Wh_GetIntSetting(L"Separators.removeSepBelowQA");

    Wh_Log(L"Settings: thisPCAtTop=%d (expand=%d, startExp=%d, hideQA=%d) "
            L"desktopAtTop=%d (above=%d, expand=%d, hideQA=%d) "
            L"hideHome=%d hideGallery=%d "
            L"fixChevron=%d chevronScale=%d hidePins=%d "
            L"rmSepNav=%d rmSepQA=%d",
            g_settings.showThisPCAtTop,
            g_settings.thisPCExpandable, g_settings.thisPCStartExpanded,
            g_settings.hideThisPCFromQuickAccess,
            g_settings.showDesktopAtTop, g_settings.desktopAboveThisPC,
            g_settings.desktopExpandable,
            g_settings.hideDesktopFromQuickAccess,
            g_settings.hideHome, g_settings.hideGallery,
            g_settings.fixChevronDrawing,
            g_settings.chevronScale,
            g_settings.hidePinButtons,
            g_settings.removeSepBelowNav,
            g_settings.removeSepBelowQA);
}

BOOL Wh_ModInit()
{
    HMODULE hExplorerFrame = GetModuleHandleW(L"ExplorerFrame.dll");
    if (!hExplorerFrame)
        return FALSE;

    LoadSettings();

    auto cleanupOnFail = []() {
        if (g_gdipToken) { Gdiplus::GdiplusShutdown(g_gdipToken); g_gdipToken = 0; }
        if (g_pidlThisPC) { CoTaskMemFree(g_pidlThisPC); g_pidlThisPC = nullptr; }
        if (g_pidlDesktop) { CoTaskMemFree(g_pidlDesktop); g_pidlDesktop = nullptr; }
        if (g_pidlHome) { CoTaskMemFree(g_pidlHome); g_pidlHome = nullptr; }
        if (g_pidlGallery) { CoTaskMemFree(g_pidlGallery); g_pidlGallery = nullptr; }
    };

    Gdiplus::GdiplusStartupInput gdipIn;
    Gdiplus::GdiplusStartup(&g_gdipToken, &gdipIn, NULL);

    SHGetSpecialFolderLocation(nullptr, CSIDL_DRIVES, &g_pidlThisPC);
    if (!g_pidlThisPC)
    {
        Wh_Log(L"Failed to get This PC PIDL");
        cleanupOnFail();
        return FALSE;
    }

    SHGetSpecialFolderLocation(nullptr, CSIDL_DESKTOP, &g_pidlDesktop);
    if (!g_pidlDesktop)
        Wh_Log(L"Warning: failed to get Desktop PIDL");

    SHParseDisplayName(L"::{f874310e-b6b7-47dc-bc84-b9e6b38f5903}",
                       nullptr, &g_pidlHome, 0, nullptr);
    SHParseDisplayName(L"::{e88865ea-0e1c-4e20-9aa6-edcd0212c87c}",
                       nullptr, &g_pidlGallery, 0, nullptr);
    Wh_Log(L"PIDLs: Home=%p Gallery=%p", g_pidlHome, g_pidlGallery);

    WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[] = {
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

    if (!WindhawkUtils::HookSymbols(hExplorerFrame, explorerFrameDllHooks, ARRAYSIZE(explorerFrameDllHooks)))
    {
        Wh_Log(L"Failed to hook symbols");
        cleanupOnFail();
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

struct DiscoveredTree {
    HWND hTop;
    HWND hTree;
    INameSpaceTreeControl *pNsc;
    unsigned long enumFlags;
};

void Wh_ModAfterInit()
{
    std::vector<DiscoveredTree> discovered;

    EnumWindows([](HWND hTop, LPARAM lParam) -> BOOL {
        auto& out = *reinterpret_cast<std::vector<DiscoveredTree>*>(lParam);

        DWORD wndPid = 0;
        GetWindowThreadProcessId(hTop, &wndPid);
        if (wndPid != GetCurrentProcessId())
            return TRUE;
        WCHAR cls[64];
        GetClassNameW(hTop, cls, ARRAYSIZE(cls));
        if (wcscmp(cls, L"CabinetWClass") != 0)
            return TRUE;

        HWND hShellTab = FindWindowExW(hTop, nullptr, L"ShellTabWindowClass", nullptr);
        IShellBrowser *pSB = nullptr;
        if (hShellTab)
            pSB = (IShellBrowser *)SendMessageW(hShellTab, WM_USER + 7, 0, 0);
        if (!pSB)
            pSB = (IShellBrowser *)SendMessageW(hTop, WM_USER + 7, 0, 0);
        if (!pSB)
            return TRUE;

        IServiceProvider *pSP = nullptr;
        if (FAILED(pSB->QueryInterface(IID_IServiceProvider, (void **)&pSP)))
            return TRUE;

        INameSpaceTreeControl *pNsc = nullptr;
        HRESULT hr = pSP->QueryService(IID_INameSpaceTreeControl,
                                         IID_INameSpaceTreeControl,
                                         (void **)&pNsc);
        pSP->Release();
        if (FAILED(hr))
            return TRUE;

        HWND hTree = nullptr;
        IOleWindow *pOleWin = nullptr;
        if (SUCCEEDED(pNsc->QueryInterface(IID_IOleWindow, (void **)&pOleWin)))
        {
            HWND hNscWnd = nullptr;
            if (SUCCEEDED(pOleWin->GetWindow(&hNscWnd)) && hNscWnd)
                hTree = FindWindowExW(hNscWnd, nullptr, L"SysTreeView32", nullptr);
            pOleWin->Release();
        }

        if (!hTree)
        {
            pNsc->Release();
            return TRUE;
        }

        unsigned long enumFlags = 0;
        enumFlags = (unsigned long)(ULONG_PTR)GetPropW(hTree, L"WH_EnumFlags");
        if (!enumFlags)
            enumFlags = SHCONTF_FOLDERS;

        out.push_back({ hTop, hTree, pNsc, enumFlags });
        return TRUE;
    }, (LPARAM)&discovered);

    for (auto& d : discovered)
    {
        TreeState& ts = g_trees[d.hTree];
        ts.pNscTree = (void *)d.pNsc;
        ts.enumFlags = d.enumFlags;
        ts.needFullRebuild = true;
        ts.ownsNscRef = true;

        SetPropW(d.hTree, L"WH_NscTree", (HANDLE)ts.pNscTree);
        SetPropW(d.hTree, L"WH_EnumFlags", (HANDLE)(ULONG_PTR)d.enumFlags);

        PostMessage(d.hTree, WM_DEFERRED_REBUILD, 0, 0);
        Wh_Log(L"[ENABLE] window=%p tree=%p pNsc=%p", d.hTop, d.hTree, d.pNsc);
    }
}

static void RefreshNavPane(HWND hTree)
{
    TreeState* ts = GetTree(hTree);
    if (!ts || !hTree || !IsWindow(hTree) || !ts->pNscTree)
        return;

    if (ts->hThisPC)
    {
        SendMessageW(hTree, TVM_DELETEITEM, 0, (LPARAM)ts->hThisPC);
        ts->hThisPC = nullptr;
    }
    if (ts->hDesktop)
    {
        SendMessageW(hTree, TVM_DELETEITEM, 0, (LPARAM)ts->hDesktop);
        ts->hDesktop = nullptr;
    }

    // Clear managed items. Don't restore hiddenDuplicate's iIntegral —
    // CleanupQuickAccessDuplicates will re-evaluate on the next paint.
    // Restoring to 2 is wrong when the item won't be re-deduped (e.g.,
    // Desktop disabled) and creates visible extra spacing.
    ResetTreeCleanup(*ts);

    INameSpaceTreeControl *pNsc = (INameSpaceTreeControl *)ts->pNscTree;
    pNsc->AddRef();
    unsigned long enumFlags = ts->enumFlags;
    IShellItemFilter *pFilter = ts->pFilter;
    if (pFilter)
        pFilter->AddRef();

    g_pNscTree = ts->pNscTree;
    g_lastEnumFlags = enumFlags;
    g_insertingForTree = hTree;
    g_inCustomAppend = true;

    NavItem items[2];
    BuildItemOrder(items);

    g_deferredOpInProgress = true;
    InsertItems(ts->pNscTree, items, enumFlags, pFilter);
    g_insertingForTree = nullptr;
    g_inCustomAppend = false;
    DrainPendingRebuilds();

    if (pFilter)
        pFilter->Release();
    pNsc->Release();

    RedrawWindow(hTree, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
}

static void HotEnableInsert(HWND hWnd)
{
    TreeState* ts = GetTree(hWnd);
    if (!ts || !ts->pNscTree || !IsWindow(hWnd))
        return;

    HTREEITEM hFirstChild = GetFirstDepth1Child(hWnd);
    if (!hFirstChild)
    {
        ts->needHotInsert = true;
        return;
    }

    ts->hThisPC = nullptr;
    ts->hDesktop = nullptr;
    // Don't clear hHome/hGallery: they may have been cached during
    // FullRebuildTree's AppendRoot_orig and are still valid.
    ResetTreeCleanup(*ts);
    g_sepColor = CLR_INVALID;
    g_logSepDraw = true;

    INameSpaceTreeControl *pNsc = (INameSpaceTreeControl *)ts->pNscTree;
    pNsc->AddRef();
    unsigned long enumFlags = ts->enumFlags;
    IShellItemFilter *pFilter = ts->pFilter;
    if (pFilter)
        pFilter->AddRef();

    g_pNscTree = ts->pNscTree;
    g_lastEnumFlags = enumFlags;
    g_insertingForTree = hWnd;
    g_inCustomAppend = true;

    NavItem items[2];
    BuildItemOrder(items);

    InsertItems(ts->pNscTree, items, enumFlags, pFilter);
    g_insertingForTree = nullptr;
    g_inCustomAppend = false;

    if (pFilter)
        pFilter->Release();
    pNsc->Release();

    ts = GetTree(hWnd);
    Wh_Log(L"[HOTINSERT] Items inserted, ThisPC=%p Desktop=%p Home=%p Gallery=%p",
           ts ? ts->hThisPC : nullptr, ts ? ts->hDesktop : nullptr,
           ts ? ts->hHome : nullptr, ts ? ts->hGallery : nullptr);

    // Invalidate for dedup and separator setup on the next paint
    // (not RDW_UPDATENOW since we're already inside WM_PAINT)
    InvalidateRect(hWnd, nullptr, TRUE);
}

static void FullRebuildTree(HWND hTree)
{
    TreeState* ts = GetTree(hTree);
    if (!ts || !ts->pNscTree || !IsWindow(hTree))
        return;

    INameSpaceTreeControl *pNsc = (INameSpaceTreeControl *)ts->pNscTree;
    pNsc->AddRef();
    void *pNscRaw = ts->pNscTree;
    unsigned long enumFlags = ts->enumFlags;

    IShellItem *pDesktop = nullptr;
    IShellItem *pThisPC = nullptr;
    SHCreateItemFromIDList(g_pidlDesktop, IID_IShellItem, (void **)&pDesktop);
    if (g_pidlThisPC)
        SHCreateItemFromIDList(g_pidlThisPC, IID_IShellItem, (void **)&pThisPC);

    if (pThisPC) { while (SUCCEEDED(pNsc->RemoveRoot(pThisPC))); }
    if (pDesktop) { while (SUCCEEDED(pNsc->RemoveRoot(pDesktop))); }

    IShellItem *pHome = nullptr, *pGallery = nullptr;
    if (g_pidlHome)
        SHCreateItemFromIDList(g_pidlHome, IID_IShellItem, (void **)&pHome);
    if (g_pidlGallery)
        SHCreateItemFromIDList(g_pidlGallery, IID_IShellItem, (void **)&pGallery);

    if (pHome && g_settings.hideHome)
        pNsc->RemoveRoot(pHome);
    if (pGallery && g_settings.hideGallery)
        pNsc->RemoveRoot(pGallery);
    if (pHome) pHome->Release();
    if (pGallery) pGallery->Release();

    if (pDesktop)
    {
        // Clear ALL item handles: RemoveRoot(pDesktop) destroys the
        // hidden root and all its children (Home, Gallery, QA items).
        // All old handles are now invalid.
        ts = GetTree(hTree);
        if (ts)
        {
            ts->hThisPC = nullptr;
            ts->hDesktop = nullptr;
            ts->hHome = nullptr;
            ts->hGallery = nullptr;
            ResetTreeCleanup(*ts);
        }

        g_pNscTree = pNscRaw;
        g_lastEnumFlags = enumFlags;

        g_inCustomAppend = true;
        AppendRoot_orig(pNscRaw, pDesktop, enumFlags, 0x1, nullptr);
        g_inCustomAppend = false;

        // Re-fetch ts: AppendRoot_orig pumps messages, which can
        // trigger TVM_INSERTITEM and rehash g_trees.
        ts = GetTree(hTree);
        if (ts)
        {
            ts->needHotInsert = true;
        }
        g_sepColor = CLR_INVALID;
        g_logSepDraw = true;

        InvalidateRect(hTree, nullptr, TRUE);
        Wh_Log(L"[REBUILD] Deferred insertion for tree=%p hHome=%p hGallery=%p",
               hTree, ts ? ts->hHome : nullptr, ts ? ts->hGallery : nullptr);
    }

    pNsc->Release();

    if (pDesktop) pDesktop->Release();
    if (pThisPC) pThisPC->Release();
}

void Wh_ModSettingsChanged()
{
    // Snapshot ALL settings that affect behavior
    auto prev = g_settings;

    LoadSettings();

    // Classify what changed. Sub-settings only matter when their
    // parent item is enabled — toggling them while disabled is a no-op.
    bool itemsChanged =
        prev.showThisPCAtTop != g_settings.showThisPCAtTop ||
        prev.showDesktopAtTop != g_settings.showDesktopAtTop;

    // This PC sub-settings only matter when This PC is or was at top
    if (g_settings.showThisPCAtTop || prev.showThisPCAtTop)
        itemsChanged = itemsChanged ||
            prev.thisPCExpandable != g_settings.thisPCExpandable ||
            prev.thisPCStartExpanded != g_settings.thisPCStartExpanded;

    // Desktop sub-settings only matter when Desktop is or was at top
    if (g_settings.showDesktopAtTop || prev.showDesktopAtTop)
        itemsChanged = itemsChanged ||
            prev.desktopExpandable != g_settings.desktopExpandable;

    // Order only matters when both items are at top
    if (g_settings.showThisPCAtTop && g_settings.showDesktopAtTop)
        itemsChanged = itemsChanged ||
            prev.desktopAboveThisPC != g_settings.desktopAboveThisPC;

    bool homeGalleryChanged =
        prev.hideHome != g_settings.hideHome ||
        prev.hideGallery != g_settings.hideGallery;

    // Dedup sub-settings only matter when their item is at top
    bool dedupChanged =
        (g_settings.showThisPCAtTop &&
         prev.hideThisPCFromQuickAccess != g_settings.hideThisPCFromQuickAccess) ||
        (g_settings.showDesktopAtTop &&
         prev.hideDesktopFromQuickAccess != g_settings.hideDesktopFromQuickAccess);

    bool sepChanged =
        prev.removeSepBelowNav != g_settings.removeSepBelowNav ||
        prev.removeSepBelowQA != g_settings.removeSepBelowQA;

    bool visualOnly =
        prev.fixChevronDrawing != g_settings.fixChevronDrawing ||
        prev.chevronScale != g_settings.chevronScale ||
        prev.hidePinButtons != g_settings.hidePinButtons;

    // Full rebuild for transitions where RefreshNavPane can't restore
    // deleted/modified items: hidden root children (Home/Gallery),
    // deleted QA duplicates, collapsed iIntegral items.
    bool needRebuild =
        (prev.hideHome && !g_settings.hideHome) ||
        (prev.hideGallery && !g_settings.hideGallery) ||
        (prev.removeSepBelowNav != g_settings.removeSepBelowNav) ||
        (prev.removeSepBelowQA && !g_settings.removeSepBelowQA) ||
        // Dedup toggles in either direction need rebuild when parent is
        // enabled: on→off deletes dups that can't be restored; off→on
        // needs dups discovered and potentially kept as hiddenDuplicate.
        (g_settings.showThisPCAtTop &&
         prev.hideThisPCFromQuickAccess != g_settings.hideThisPCFromQuickAccess) ||
        (g_settings.showDesktopAtTop &&
         prev.hideDesktopFromQuickAccess != g_settings.hideDesktopFromQuickAccess) ||
        // Parent going off while hideFromQA was on: QA dups were deleted
        // and can't be restored without rebuild.
        (prev.showThisPCAtTop && !g_settings.showThisPCAtTop &&
         prev.hideThisPCFromQuickAccess) ||
        (prev.showDesktopAtTop && !g_settings.showDesktopAtTop &&
         prev.hideDesktopFromQuickAccess);

    // Nothing that affects the tree changed — just repaint
    bool needRefresh = itemsChanged || needRebuild;
    bool needRepaint = homeGalleryChanged || dedupChanged || sepChanged || visualOnly;

    if (!needRefresh && !needRepaint)
        return;

    // Snapshot HWNDs: FullRebuildTree pumps messages, which can trigger
    // TVM_INSERTITEM on other trees, calling g_trees[hWnd] (operator[])
    // and invalidating iterators if we're mid-range-for.
    std::vector<HWND> treeList;
    treeList.reserve(g_trees.size());
    for (auto& [hTree, ts] : g_trees)
        treeList.push_back(hTree);
    int treeCount = (int)treeList.size();

    if (sepChanged)
    {
        g_sepColor = CLR_INVALID;
        g_logSepDraw = true;
    }

    for (int i = 0; i < treeCount; i++)
    {
        HWND hTree = treeList[i];
        TreeState* ts = GetTree(hTree);
        if (!ts || !IsWindow(hTree) || !ts->pNscTree)
            continue;

        if (needRebuild)
        {
            g_deferredOpInProgress = true;
            FullRebuildTree(hTree);
            DrainPendingRebuilds();
        }
        else if (itemsChanged)
        {
            ts->boundaryItem = nullptr;
            ts->belowQAItem = nullptr;
            ts->qaCleanupDone = false;
            RefreshNavPane(hTree);
        }
        else
        {
            if (homeGalleryChanged)
            {
                ts->homeGalleryCleanupDone = false;
            }
            if (dedupChanged)
            {
                ts->qaCleanupDone = false;
                ts->qaEverCleaned = false;
            }
            if (sepChanged || dedupChanged)
            {
                ts->boundaryItem = nullptr;
                ts->belowQAItem = nullptr;
            }

            InvalidateRect(hTree, nullptr, TRUE);
        }
    }
}

void Wh_ModUninit()
{
    // Clear state so hooks become no-ops
    g_settings = {};
    g_sepColor = CLR_INVALID;

    // Snapshot HWNDs: AppendRoot_orig pumps messages, which could
    // trigger TVM_INSERTITEM and modify g_trees during iteration.
    std::vector<HWND> uninitList;
    uninitList.reserve(g_trees.size());
    for (auto& [hTree, ts] : g_trees)
        uninitList.push_back(hTree);

    for (int i = 0; i < (int)uninitList.size(); i++)
    {
        HWND hTree = uninitList[i];
        TreeState* ts = GetTree(hTree);
        if (!ts || !IsWindow(hTree))
            continue;

        if (!ts->pNscTree)
            continue;

        INameSpaceTreeControl *pNsc = (INameSpaceTreeControl *)ts->pNscTree;
        pNsc->AddRef();
        void *pNscRaw = ts->pNscTree;
        unsigned long enumFlags = ts->enumFlags;
        IShellItemFilter *pFilter = ts->pFilter;
        if (pFilter)
            pFilter->AddRef();
        HIMAGELIST savedImgList = ts->savedStateImageList;
        bool ownsRef = ts->ownsNscRef;

        IShellItem *pDesktop = nullptr;
        IShellItem *pThisPC = nullptr;
        SHCreateItemFromIDList(g_pidlDesktop, IID_IShellItem, (void **)&pDesktop);
        if (g_pidlThisPC)
            SHCreateItemFromIDList(g_pidlThisPC, IID_IShellItem, (void **)&pThisPC);

        if (pThisPC) { while (SUCCEEDED(pNsc->RemoveRoot(pThisPC))); }
        if (pDesktop) { while (SUCCEEDED(pNsc->RemoveRoot(pDesktop))); }

        if (pDesktop && pNscRaw)
        {
            g_inCustomAppend = true;
            AppendRoot_orig(pNscRaw, pDesktop, enumFlags, 0x1, pFilter);
            g_inCustomAppend = false;

            // Collapse any expanded items matching our items so they
            // don't stay expanded in the restored native nav pane.
            WCHAR collapseNames[2][64] = {};
            int collapseCount = 0;
            if (g_pidlThisPC)
            {
                GetPidlDisplayName(g_pidlThisPC, collapseNames[collapseCount], 64);
                if (collapseNames[collapseCount][0]) collapseCount++;
            }
            if (g_pidlDesktop && collapseCount < 2)
            {
                GetPidlDisplayName(g_pidlDesktop, collapseNames[collapseCount], 64);
                if (collapseNames[collapseCount][0]) collapseCount++;
            }
            if (collapseCount > 0)
            {
                HTREEITEM h = GetFirstDepth1Child(hTree);
                while (h)
                {
                    WCHAR text[64] = {};
                    TVITEMEXW tvi = {};
                    tvi.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_STATE;
                    tvi.stateMask = TVIS_EXPANDED;
                    tvi.hItem = h;
                    tvi.pszText = text;
                    tvi.cchTextMax = 64;
                    SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
                    if ((tvi.state & TVIS_EXPANDED) && text[0])
                    {
                        for (int j = 0; j < collapseCount; j++)
                        {
                            if (wcscmp(text, collapseNames[j]) == 0)
                            {
                                SendMessageW(hTree, TVM_EXPAND, TVE_COLLAPSE, (LPARAM)h);
                                Wh_Log(L"[DISABLE] Collapsed '%s' in tree=%p", text, hTree);
                                break;
                            }
                        }
                    }
                    h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)h);
                }
            }
        }

        if (pDesktop) pDesktop->Release();
        if (pThisPC) pThisPC->Release();

        RemoveWindowSubclass(hTree, TreeInteractionProc, 0xAF01);

        if (savedImgList)
        {
            SendMessageW(hTree, TVM_SETIMAGELIST, TVSIL_STATE, (LPARAM)savedImgList);
            Wh_Log(L"[DISABLE] Restored state image list %p for tree=%p", savedImgList, hTree);
        }

        // Re-fetch ts after message pumping
        ts = GetTree(hTree);
        if (ts)
        {
            if (ownsRef)
                ts->ownsNscRef = false;
            ts->pNscTree = nullptr;
            ts->pFilter = nullptr;
        }

        // Release local refs (safe even if WM_NCDESTROY already released
        // the TreeState's refs — our AddRef keeps the objects alive)
        if (pFilter)
            pFilter->Release();
        if (ownsRef)
            pNsc->Release();
        pNsc->Release();

        RedrawWindow(hTree, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);

        Wh_Log(L"[DISABLE] Cleaned up tree=%p", hTree);
    }

    // Remove all parent subclasses
    for (HWND parent : g_subclassedParents)
    {
        if (IsWindow(parent))
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(parent, SepParentSubclassProc);
    }
    g_subclassedParents.clear();
    // WM_NCDESTROY and the per-tree loop above null pFilter/pNscTree,
    // but trees that were never processed (e.g., !IsWindow) may remain.
    for (auto& [h, ts] : g_trees)
    {
        if (ts.pFilter)
        {
            ts.pFilter->Release();
            ts.pFilter = nullptr;
        }
    }
    g_trees.clear();
    g_pendingRebuildTrees.clear();

    // Clean up global resources
    if (g_gdipToken) { Gdiplus::GdiplusShutdown(g_gdipToken); g_gdipToken = 0; }
    if (g_pidlThisPC) { CoTaskMemFree(g_pidlThisPC); g_pidlThisPC = nullptr; }
    if (g_pidlDesktop) { CoTaskMemFree(g_pidlDesktop); g_pidlDesktop = nullptr; }
    if (g_pidlHome) { CoTaskMemFree(g_pidlHome); g_pidlHome = nullptr; }
    if (g_pidlGallery) { CoTaskMemFree(g_pidlGallery); g_pidlGallery = nullptr; }
    g_pNscTree = nullptr;
    g_lastEnumFlags = 0;
    if (g_pLastFilter) { g_pLastFilter->Release(); g_pLastFilter = nullptr; }
    Wh_Log(L"Mod uninitialized");
}