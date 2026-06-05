// ==WindhawkMod==
// @id              add-virtual-folders-to-nav-top
// @name            Add This PC and Desktop to Nav Top
// @description     Adds This PC and Desktop to the top of Explorer's nav
// @version         1.3.1
// @author          Rod Boev
// @github          https://github.com/rodboev
// @include         *
// @compilerOptions -lole32 -lshell32 -luuid -luxtheme -lgdi32 -lgdiplus -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Add This PC and Desktop to Nav Top

This mod adds two virtual folders to the top of File Explorer's navigation pane and fixes the chevron rendering.

- **Show This PC at top:** Adds an expandable This PC entry with drives. (Expandable This PC can't be pinned to the top without this mod.)

- **Show Desktop at top:** Adds a Desktop entry for the root namespace object. This includes Recycle Bin, Control Panel, etc. instead of just the items on the desktop. (This target can't be pinned without this mod.)

Both virtual folders have a toggle for whether they are expandable or not. Their position can be swapped. Duplicate entries of Desktop or This PC can be removed from other parts of the nav. Also:

- **Hide Home/Gallery:** Optional, default-enabled toggles to hide the native Explorer entries. They don't affect Quick Access.

- **Remove separators:** Optionally remove each separator individually.

- **Fix chevron drawing:** Replaces the pixelated and clipped chevron with a smooth anti-aliased versions. The size (which matches other UI elements by default) is configurable.

This mod injects only in processes that use `ExplorerFrame.dll`, so the include is set to `*` but it will not touch most processes. You can set it to `explorer.exe` only, if you don't want the nav in Open/Save dialogs changed. Only "modern" dialogs are affected.

Before/after:

![](https://i.imgur.com/LZuqzMx.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ThisPC:
  - showThisPCAtTop: true
    $name: Add to top
    $description: Add an expandable entry for This PC to the top of the nav.
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
#include <atomic>
#include <mutex>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifdef _WIN64
#define THISCALL __cdecl
#else
#define THISCALL __stdcall
#endif

template<typename T>
struct ComRef {
    T* p;
    ComRef() : p(nullptr) {}
    explicit ComRef(T* raw, bool addRef = true) : p(raw) {
        if (p && addRef) p->AddRef();
    }
    ~ComRef() { if (p) p->Release(); }
    ComRef(const ComRef&) = delete;
    ComRef& operator=(const ComRef&) = delete;
    ComRef(ComRef&& other) noexcept : p(other.p) { other.p = nullptr; }
    ComRef& operator=(ComRef&& other) noexcept {
        if (this != &other) { if (p) p->Release(); p = other.p; other.p = nullptr; }
        return *this;
    }
    T* operator->() { return p; }
    T* get() const { return p; }
    explicit operator bool() const { return p != nullptr; }
};

enum NavItemId {
    NAV_THISPC  = 0,
    NAV_DESKTOP = 1,
    NAV_HOME    = 2,
    NAV_GALLERY = 3,
    NAV_COUNT   = 4
};

struct NavItemDef {
    PIDLIST_ABSOLUTE pidl;
    WCHAR label[64];
};

static NavItemDef g_navItems[NAV_COUNT] = {};

struct NavItemSettings {
    bool showAtTop;
    bool expandable;
    bool startExpanded;
    bool hideFromQA;
    bool hide;
};

struct Settings {
    NavItemSettings items[NAV_COUNT];
    bool desktopAboveThisPC;
    bool fixChevronDrawing;
    int chevronScale;
    bool hidePinButtons;
    bool removeSepBelowNav;
    bool removeSepBelowQA;
    bool hasItemsAtTop;  // precomputed: any items[i].showAtTop is true
};
static Settings g_settings;

struct RootShellItems {
    ComRef<IShellItem> items[NAV_COUNT];

    bool Create() {
        for (int i = 0; i < NAV_COUNT; i++) {
            if (!g_navItems[i].pidl) continue;
            IShellItem *raw = nullptr;
            if (SUCCEEDED(SHCreateItemFromIDList(
                    g_navItems[i].pidl, IID_IShellItem, (void**)&raw)))
                items[i] = ComRef<IShellItem>(raw, false);
        }
        return (bool)items[NAV_DESKTOP];
    }

    void RemoveInsertableRoots(INameSpaceTreeControl *pNsc) {
        if (items[NAV_THISPC])
            while (SUCCEEDED(pNsc->RemoveRoot(items[NAV_THISPC].get())));
        if (items[NAV_DESKTOP])
            while (SUCCEEDED(pNsc->RemoveRoot(items[NAV_DESKTOP].get())));
    }
};

enum ChangeTier { TIER_NONE, TIER_REPAINT, TIER_REFRESH, TIER_REBUILD };

static ULONG_PTR g_gdipToken = 0;
static std::set<HWND> g_subclassedParents;
static COLORREF g_sepColor = CLR_INVALID;
static bool g_logSepDraw = true;
#define PTR4(p) ((unsigned)(uintptr_t)(p) & 0xFFFF)

static void ResetSepColor() {
    g_sepColor = CLR_INVALID;
    g_logSepDraw = true;
}

static COLORREF GetTreeBgColor(HWND hTree)
{
    COLORREF bg = (COLORREF)SendMessageW(hTree, TVM_GETBKCOLOR, 0, 0);
    if (bg == CLR_INVALID || (int)bg == -1)
        bg = GetSysColor(COLOR_WINDOW);
    return bg;
}

static COLORREF DeriveSepColor(HWND hTree)
{
    COLORREF bg = GetTreeBgColor(hTree);
    int sum = GetRValue(bg) + GetGValue(bg) + GetBValue(bg);
    int d = (sum < 384) ? 56 : -41;
    auto cl = [](int v) { return (BYTE)(v < 0 ? 0 : (v > 255 ? 255 : v)); };
    return RGB(cl(GetRValue(bg) + d), cl(GetGValue(bg) + d), cl(GetBValue(bg) + d));
}

enum PendingWork : uint8_t {
    WORK_FULL_REBUILD = 0x01,
    WORK_HOT_INSERT   = 0x02,
    WORK_QA_CLEANUP   = 0x04,
    WORK_HG_CLEANUP   = 0x08,
    WORK_DUP_COLLAPSE = 0x10,
    WORK_EXPAND       = 0x20,
    WORK_HOME_SPACER  = 0x40,
};

// Per-tree state: one entry per SysTreeView32 that we've injected items into.
struct TreeState {
    void *pNscTree = nullptr;
    unsigned long enumFlags = 0;
    IShellItemFilter *pFilter = nullptr;
    HTREEITEM hItems[NAV_COUNT] = {};
    HTREEITEM hiddenDuplicate = nullptr;
    HTREEITEM homeSpacerItem = nullptr;
    HTREEITEM boundaryItem = nullptr;
    HTREEITEM belowQAItem = nullptr;
    bool belowQADiscovered = false;
    bool freshFromAppend = false;
    uint8_t pendingWork = 0;   // bitmask of PendingWork flags
    int sepRetries = 0;
    bool ownsNscRef = false;
    bool triedHomeSpacer = false;
    HIMAGELIST savedStateImageList = nullptr;
};
static std::unordered_map<HWND, TreeState> g_trees;
static std::recursive_mutex g_treesMutex;

static TreeState* GetTree(HWND hWnd) {
    std::lock_guard<std::recursive_mutex> lock(g_treesMutex);
    auto it = g_trees.find(hWnd);
    return (it != g_trees.end()) ? &it->second : nullptr;
}

static bool IsNavPaneHost(HWND hTree)
{
    HWND h = GetAncestor(hTree, GA_ROOT);
    if (h)
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

static HTREEITEM GetHiddenItem(const TreeState& ts)
{
    return ts.hiddenDuplicate ? ts.hiddenDuplicate : ts.homeSpacerItem;
}

static bool IsOurSection(const TreeState& ts, HTREEITEM h)
{
    if (ts.homeSpacerItem && h == ts.homeSpacerItem)
        return true;
    for (int i = 0; i < NAV_COUNT; i++)
        if (ts.hItems[i] && h == ts.hItems[i])
            return true;
    return false;
}

static bool IsInsertableItem(int id)
{
    return id == NAV_THISPC || id == NAV_DESKTOP;
}

// Settings determine visual order; no rect measurement needed.
static HTREEITEM LowerInsertableItem(const TreeState& ts)
{
    int lower = g_settings.desktopAboveThisPC ? NAV_THISPC : NAV_DESKTOP;
    return ts.hItems[lower];
}

static void GetPidlDisplayName(PIDLIST_ABSOLUTE pidl, WCHAR *buf, int len)
{
    IShellItem *psi = nullptr;
    HRESULT hr = SHCreateItemFromIDList(pidl, IID_IShellItem, (void **)&psi);
    if (SUCCEEDED(hr) && psi)
    {
        LPWSTR name = nullptr;
        if (SUCCEEDED(psi->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) && name)
        {
            wcsncpy_s(buf, len, name, _TRUNCATE);
            CoTaskMemFree(name);
        }
        psi->Release();
    }
    if (!buf[0])
    {
        IShellFolder *pDesktop = nullptr;
        if (SUCCEEDED(SHGetDesktopFolder(&pDesktop)) && pDesktop)
        {
            STRRET str = {};
            if (SUCCEEDED(pDesktop->GetDisplayNameOf((PCUITEMID_CHILD)pidl, SHGDN_NORMAL, &str)))
            {
                if (str.uType == STRRET_WSTR && str.pOleStr)
                {
                    wcsncpy_s(buf, len, str.pOleStr, _TRUNCATE);
                    CoTaskMemFree(str.pOleStr);
                }
                else if (str.uType == STRRET_CSTR)
                {
                    MultiByteToWideChar(CP_ACP, 0, str.cStr, -1, buf, len);
                }
            }
            pDesktop->Release();
        }
    }
}

static void ResetTreeCleanup(TreeState& ts)
{
    ts.hiddenDuplicate = nullptr;
    ts.homeSpacerItem = nullptr;
    ts.boundaryItem = nullptr;
    ts.belowQAItem = nullptr;
    ts.belowQADiscovered = false;
    ts.freshFromAppend = false;
    ts.triedHomeSpacer = false;
    ts.pendingWork = WORK_QA_CLEANUP | WORK_HG_CLEANUP | WORK_DUP_COLLAPSE;
}

// Globals bridging AppendRoot_hook to TVM_INSERTITEM handler.
struct InsertionCtx {
    int item = -1;          // -1=not ours, 0..3=NavItemId
    void *pNsc = nullptr;
    unsigned long enumFlags = 0;
    IShellItemFilter *filter = nullptr;
    HWND forTree = nullptr; // scopes item to a specific tree; null = any
};
static thread_local InsertionCtx g_ins;

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_orig;
static std::atomic<bool> g_explorerFrameLoaded{false};

static thread_local bool g_deferredOpInProgress = false;
static thread_local HWND g_mutatingTree = nullptr;
static thread_local std::unordered_set<HWND> g_pendingRebuildTrees;

#define WM_DEFERRED_REBUILD  (WM_APP + 0x101)

#define WM_SETTINGS_CHANGED  (WM_APP + 0x102)
#define WM_RESTORE_TREE      (WM_APP + 0x103)

struct SettingsChangeInfo {
    ChangeTier tier;
    Settings prev;
    bool sepChanged;
};

static void DrainPendingRebuilds()
{
    g_deferredOpInProgress = false;
    g_mutatingTree = nullptr;
    while (!g_pendingRebuildTrees.empty())
    {
        HWND hNext = *g_pendingRebuildTrees.begin();
        g_pendingRebuildTrees.erase(g_pendingRebuildTrees.begin());
        if (IsWindow(hNext) && GetTree(hNext))
            PostMessage(hNext, WM_DEFERRED_REBUILD, 0, 0);
    }
}

static TreeState* RunDeferredWork(HWND hWnd, TreeState* ts, uint8_t flag, void(*op)(HWND))
{
    if (!ts || !(ts->pendingWork & flag))
        return ts;
    ts->pendingWork &= ~flag;
    op(hWnd);
    ts = GetTree(hWnd);
    return ts;
}

static TreeState* RunDeferredWorkIf(HWND hWnd, TreeState* ts, uint8_t flag,
                                    bool pre, bool(*op)(HWND, TreeState&))
{
    if (!ts || !(ts->pendingWork & flag) || !pre)
        return ts;
    bool done = op(hWnd, *ts);
    ts = GetTree(hWnd);
    if (done && ts)
        ts->pendingWork &= ~flag;
    return ts;
}

static thread_local bool g_inCustomAppend = false;
static thread_local HTREEITEM g_spacerInsertAfter = nullptr;

struct InsertionScope {
    ComRef<INameSpaceTreeControl> pNsc;
    ComRef<IShellItemFilter> pFilter;
    void *pNscRaw;

    InsertionScope(TreeState& ts, HWND hTree)
        : pNsc((INameSpaceTreeControl *)ts.pNscTree)
        , pFilter(ts.pFilter)
        , pNscRaw(ts.pNscTree)
    {
        g_ins.pNsc = ts.pNscTree;
        g_ins.enumFlags = ts.enumFlags;
        g_ins.forTree = hTree;
        g_inCustomAppend = true;
    }

    ~InsertionScope() {
        g_ins.forTree = nullptr;
        g_inCustomAppend = false;
    }
};

static bool ShouldRemoveInternalSep()
{
    return g_settings.items[NAV_THISPC].showAtTop && g_settings.items[NAV_DESKTOP].showAtTop;
}

// --- AppendRoot hook ---

using AppendRoot_t = HRESULT (THISCALL *)(void *pThis, IShellItem *psiRoot, unsigned long grfEnumFlags, unsigned long grfRootStyle, IShellItemFilter *pFilter);

AppendRoot_t AppendRoot_orig;

static void AppendOneItem(void *pThis, PIDLIST_ABSOLUTE pidl, bool expandable, unsigned long grfEnumFlags, IShellItemFilter *pOrigFilter, unsigned long rootStyle = 0)
{
    if (!pidl)
        return;

    IShellItem *pItem = nullptr;
    if (FAILED(SHCreateItemFromIDList(pidl, IID_IShellItem, (void **)&pItem)))
        return;

    AppendRoot_orig(pThis, pItem, expandable ? grfEnumFlags : 0, rootStyle, pOrigFilter);

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
    int first = g_settings.desktopAboveThisPC ? NAV_DESKTOP : NAV_THISPC;
    int second = g_settings.desktopAboveThisPC ? NAV_THISPC : NAV_DESKTOP;

    auto fill = [&](NavItem& out, int id) {
        out.pidl = g_navItems[id].pidl;
        out.expandable = g_settings.items[id].expandable;
        out.enabled = g_settings.items[id].showAtTop;
        out.id = id;
        out.style = 0;
    };

    fill(items[0], first);
    fill(items[1], second);
}

template<typename Pred>
static int BuildMatchList(WCHAR out[][64], int maxOut, Pred&& pred)
{
    int count = 0;
    for (int i = 0; i < NAV_COUNT && count < maxOut; i++)
        if (g_navItems[i].label[0] && pred(i, g_settings.items[i]))
        {
            wcsncpy_s(out[count], 64, g_navItems[i].label, _TRUNCATE);
            count++;
        }
    return count;
}

static void InsertItems(void *pNsc, const NavItem items[2], unsigned long enumFlags, IShellItemFilter *filter)
{
    for (int i = 1; i >= 0; i--)
    {
        if (items[i].enabled)
        {
            g_ins.item = items[i].id;
            AppendOneItem(pNsc, items[i].pidl, items[i].expandable, enumFlags, filter, items[i].style);
            g_ins.item = -1;
        }
    }
}

static void ExpandStartExpandedItems(HWND hTree)
{
    TreeState* ts = GetTree(hTree);
    if (!ts || !ts->pNscTree) return;
    INameSpaceTreeControl *pNsc = (INameSpaceTreeControl *)ts->pNscTree;
    for (int i = 0; i < NAV_COUNT; i++)
    {
        if (!IsInsertableItem(i) || !g_settings.items[i].showAtTop ||
            !g_settings.items[i].startExpanded || !g_navItems[i].pidl)
            continue;
        IShellItem *psi = nullptr;
        if (SUCCEEDED(SHCreateItemFromIDList(g_navItems[i].pidl, IID_IShellItem, (void **)&psi)) && psi)
        {
            pNsc->SetItemState(psi, NSTCIS_EXPANDED, NSTCIS_EXPANDED);
            psi->Release();
        }
    }
}

HRESULT THISCALL AppendRoot_hook(void *pThis, IShellItem *psiRoot, unsigned long grfEnumFlags, unsigned long grfRootStyle, IShellItemFilter *pFilter)
{
    HWND hForTree = nullptr;
    {
        IOleWindow* pOle = nullptr;
        if (SUCCEEDED(((INameSpaceTreeControl*)pThis)->QueryInterface(IID_IOleWindow, (void**)&pOle)))
        {
            HWND hNsc = nullptr;
            if (SUCCEEDED(pOle->GetWindow(&hNsc)) && hNsc)
                hForTree = FindWindowExW(hNsc, nullptr, L"SysTreeView32", nullptr);
            pOle->Release();
        }
    }

    // Must run before g_inCustomAppend check so Home/Gallery get cached.
    {
        PIDLIST_ABSOLUTE pidlRoot = nullptr;
        if (SUCCEEDED(SHGetIDListFromObject(psiRoot, &pidlRoot)))
        {
            int matchId = -1;
            for (int i = NAV_HOME; i < NAV_COUNT; i++)
            {
                if (g_navItems[i].pidl && ILIsEqual(pidlRoot, g_navItems[i].pidl))
                { matchId = i; break; }
            }
            CoTaskMemFree(pidlRoot);

            if (matchId >= 0)
            {
                bool hide = g_settings.items[matchId].hide;
                if (hide)
                {
                    Wh_Log(L"[HOOK] Suppressing '%s' root", g_navItems[matchId].label);
                    return S_OK;
                }
                g_ins.forTree = hForTree;
                g_ins.item = matchId;
                HRESULT hr = AppendRoot_orig(pThis, psiRoot, grfEnumFlags, grfRootStyle, pFilter);
                g_ins.item = -1;
                g_ins.forTree = nullptr;
                return hr;
            }
        }
    }

    if (g_inCustomAppend)
    {
        Wh_Log(L"[APPEND] passthrough (g_inCustomAppend) tree=%04X style=%lX thread=%u",
               PTR4(hForTree), grfRootStyle, GetCurrentThreadId());
        return AppendRoot_orig(pThis, psiRoot, grfEnumFlags, grfRootStyle, pFilter);
    }

    bool isHiddenRoot = (grfRootStyle & 0x1) != 0;
    bool wantItems = isHiddenRoot && g_settings.hasItemsAtTop;
    Wh_Log(L"[APPEND] tree=%04X hidden=%d wantItems=%d style=%lX thread=%u",
           PTR4(hForTree), (int)isHiddenRoot, (int)wantItems, grfRootStyle, GetCurrentThreadId());

    HRESULT hr = AppendRoot_orig(pThis, psiRoot, grfEnumFlags, grfRootStyle, pFilter);

    if (wantItems)
    {
        g_ins.pNsc = pThis;
        g_ins.enumFlags = grfEnumFlags;
        if (g_ins.filter != pFilter)
        {
            if (g_ins.filter) g_ins.filter->Release();
            g_ins.filter = pFilter;
            if (g_ins.filter) g_ins.filter->AddRef();
        }

        g_ins.forTree = hForTree;
        g_inCustomAppend = true;

        NavItem items[2];
        BuildItemOrder(items);
        InsertItems(pThis, items, grfEnumFlags, pFilter);

        g_inCustomAppend = false;
        g_ins.forTree = nullptr;

        TreeState* tsPost = GetTree(hForTree);
        if (tsPost)
        {
            tsPost->freshFromAppend = true;
            tsPost->belowQAItem = nullptr;
            tsPost->belowQADiscovered = false;
            tsPost->boundaryItem = nullptr;
            tsPost->hiddenDuplicate = nullptr;
            tsPost->pendingWork |= WORK_QA_CLEANUP | WORK_HG_CLEANUP | WORK_DUP_COLLAPSE;
            PostMessage(hForTree, WM_DEFERRED_REBUILD, 0, 0);
        }
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
static thread_local int g_inSubclassProc = 0;
static thread_local bool g_inOurSelect = false;
static thread_local int g_lastSepY = -1;
static thread_local int g_firstSepY = -1;

static bool AreWeMutating()
{
    return g_deferredOpInProgress || g_inSubclassProc > 0 || g_inCustomAppend;
}

static bool IsDepth1Item(HWND hTree, HTREEITEM h)
{
    HTREEITEM parent = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)h);
    if (!parent)
        return false;
    HTREEITEM gp = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)parent);
    return (gp == nullptr);
}

static HTREEITEM GetFirstDepth1Child(HWND hTree)
{
    HTREEITEM hRoot = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_ROOT, 0);
    return hRoot ? (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hRoot) : nullptr;
}

template<typename F>
static void ForEachDepth1Item(HWND hTree, F&& visitor)
{
    HTREEITEM h = GetFirstDepth1Child(hTree);
    while (h)
    {
        HTREEITEM hNext = (HTREEITEM)SendMessageW(
            hTree, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)h);
        if (!visitor(h))
            break;
        h = hNext;
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

template<typename Skip, typename OnMatch>
static void ForEachLabeledSibling(HWND hTree, Skip&& skip,
                                  const WCHAR names[][64], int count, OnMatch&& onMatch)
{
    ForEachDepth1Item(hTree, [&](HTREEITEM h) -> bool {
        if (skip(h)) return true;
        WCHAR text[64] = {};
        GetItemText(hTree, h, text, ARRAYSIZE(text));
        for (int j = 0; j < count; j++)
            if (names[j][0] && wcscmp(text, names[j]) == 0)
                return onMatch(h, j);
        return true;
    });
}

static bool CollapseItemIntegral(HWND hTree, HTREEITEM h)
{
    TVITEMEXW tvi = {};
    tvi.mask = TVIF_HANDLE | TVIF_INTEGRAL;
    tvi.hItem = h;
    SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
    if (tvi.iIntegral >= 2)
    {
        Wh_Log(L"[COLLAPSE] tree=%04X item=%04X int=%d->1", PTR4(hTree), PTR4(h), tvi.iIntegral);
        tvi.iIntegral = 1;
        SendMessageW(hTree, TVM_SETITEMW, 0, (LPARAM)&tvi);
        return true;
    }
    return false;
}

static void SetItemIntegral(HWND hTree, HTREEITEM h, int value)
{
    TVITEMEXW tvi = {};
    tvi.mask = TVIF_HANDLE | TVIF_INTEGRAL;
    tvi.hItem = h;
    tvi.iIntegral = value;
    SendMessageW(hTree, TVM_SETITEMW, 0, (LPARAM)&tvi);
}

static int GetItemIntegral(HWND hTree, HTREEITEM h)
{
    TVITEMEXW tvi = {};
    tvi.mask = TVIF_HANDLE | TVIF_INTEGRAL;
    tvi.hItem = h;
    SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
    return tvi.iIntegral;
}

static bool GetItemRect(HWND hTree, HTREEITEM h, RECT *rc)
{
    if (!SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_FIRSTVISIBLE, 0))
        return false;
    *(HTREEITEM *)rc = h;
    return SendMessageW(hTree, TVM_GETITEMRECT, FALSE, (LPARAM)rc) != 0;
}

struct RedrawFreeze {
    HWND h;
    RedrawFreeze(HWND h) : h(h) { SendMessageW(h, WM_SETREDRAW, FALSE, 0); }
    ~RedrawFreeze() { SendMessageW(h, WM_SETREDRAW, TRUE, 0); InvalidateRect(h, nullptr, TRUE); }
};

static bool ShouldBeUnitHeight(const TreeState& ts, HWND hTree, HTREEITEM h)
{
    if (ts.boundaryItem && h == ts.boundaryItem &&
        (g_settings.removeSepBelowNav || GetHiddenItem(ts)))
        return true;

    if (ts.belowQAItem && h == ts.belowQAItem && g_settings.removeSepBelowQA)
        return true;

    if (g_settings.hasItemsAtTop)
    {
        for (int i = NAV_HOME; i < NAV_COUNT; i++)
            if (ts.hItems[i] && h == ts.hItems[i] && !g_settings.items[i].hide)
                return true;
    }

    // Collapse the lower of Desktop/This PC when they're adjacent (no internal sep).
    if (ShouldRemoveInternalSep())
    {
        HTREEITEM hA = ts.hItems[NAV_THISPC];
        HTREEITEM hB = ts.hItems[NAV_DESKTOP];
        if (hA && hB && (h == hA || h == hB))
        {
            HTREEITEM hLower = LowerInsertableItem(ts);
            if (h == hLower)
                return true;
        }
    }

    return false;
}

// --- Separator removal: CDDS_PREPAINT swallows all native separators; CDDS_POSTPAINT redraws wanted ones. ---

static int GetBaseItemHeight(HWND hTree)
{
    int baseHeight = 0;
    HTREEITEM h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_FIRSTVISIBLE, 0);
    while (h)
    {
        RECT rc = {};
        if (GetItemRect(hTree, h, &rc))
        {
            int ih = rc.bottom - rc.top;
            if (ih > 0 && (baseHeight == 0 || ih < baseHeight))
                baseHeight = ih;
        }
        h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_NEXTVISIBLE, (LPARAM)h);
    }
    return baseHeight;
}

static void CollapseMatchingItems(HWND hTree, const TreeState *ts,
                                  const WCHAR names[][64], int count)
{
    ForEachDepth1Item(hTree, [&](HTREEITEM h) -> bool {
        if (!ts || !IsOurSection(*ts, h))
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
                for (int j = 0; j < count; j++)
                {
                    if (wcscmp(text, names[j]) == 0)
                    {
                        SendMessageW(hTree, TVM_EXPAND, TVE_COLLAPSE, (LPARAM)h);
                        break;
                    }
                }
            }
        }
        return true;
    });
}

enum InsertFlags {
    INS_DELETE_FIRST   = 1,  // delete existing items before re-inserting
    INS_DEFER_IF_EMPTY = 2,  // bail via WORK_HOT_INSERT if tree is empty
    INS_RESET_SEPCOLOR = 4,  // reset separator color
    INS_UPDATE_NOW     = 8,  // RedrawWindow with RDW_UPDATENOW
    INS_DEFERRED_GUARD = 16, // set g_deferredOpInProgress + drain
};

static void InsertOurItems(HWND hTree, unsigned flags)
{
    TreeState* ts = GetTree(hTree);
    if (!ts || !ts->pNscTree || !IsWindow(hTree))
        return;

    if ((flags & INS_DEFER_IF_EMPTY) && !GetFirstDepth1Child(hTree))
    {
        ts->pendingWork |= WORK_HOT_INSERT;
        return;
    }

    if (flags & INS_DELETE_FIRST)
    {
        for (int i = NAV_THISPC; i <= NAV_DESKTOP; i++)
        {
            if (ts->hItems[i])
            {
                SendMessageW(hTree, TVM_DELETEITEM, 0, (LPARAM)ts->hItems[i]);
                ts->hItems[i] = nullptr;
            }
        }
    }
    else
    {
        ts->hItems[NAV_THISPC] = nullptr;
        ts->hItems[NAV_DESKTOP] = nullptr;
    }

    ResetTreeCleanup(*ts);
    if (flags & INS_RESET_SEPCOLOR)
        ResetSepColor();

    {
        InsertionScope scope(*ts, hTree);
        NavItem items[2];
        BuildItemOrder(items);
        if (flags & INS_DEFERRED_GUARD)
        {
            g_deferredOpInProgress = true;
            g_mutatingTree = hTree;
        }
        InsertItems(scope.pNscRaw, items, ts->enumFlags, scope.pFilter.get());
    }
    if (flags & INS_DEFERRED_GUARD)
        DrainPendingRebuilds();

    ts = GetTree(hTree);
    if (flags & INS_UPDATE_NOW)
        RedrawWindow(hTree, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    else
    {
        Wh_Log(L"[HOTINSERT] ThisPC=%04X Desktop=%04X Home=%04X Gallery=%04X",
               PTR4(ts ? ts->hItems[NAV_THISPC] : nullptr), PTR4(ts ? ts->hItems[NAV_DESKTOP] : nullptr),
               PTR4(ts ? ts->hItems[NAV_HOME] : nullptr), PTR4(ts ? ts->hItems[NAV_GALLERY] : nullptr));
        InvalidateRect(hTree, nullptr, TRUE); // not RDW_UPDATENOW: inside WM_PAINT
    }
}

static void RefreshNavPane(HWND hTree)
{
    InsertOurItems(hTree, INS_DELETE_FIRST | INS_DEFERRED_GUARD | INS_UPDATE_NOW);
}

static void HotEnableInsert(HWND hWnd)
{
    InsertOurItems(hWnd, INS_DEFER_IF_EMPTY | INS_RESET_SEPCOLOR);
}

static LRESULT CALLBACK TreeInteractionProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    DWORD_PTR dwRefData)
{
    TreeState* ts = GetTree(hWnd);
    HTREEITEM hiddenDup = ts ? GetHiddenItem(*ts) : nullptr;

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

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static void RemoveHomeSpacer(TreeState& ts, INameSpaceTreeControl* pNsc)
{
    if (!ts.homeSpacerItem) return;
    int spacerId = g_navItems[NAV_HOME].pidl ? NAV_HOME
                 : (g_navItems[NAV_GALLERY].pidl ? NAV_GALLERY : -1);
    if (spacerId >= 0)
    {
        IShellItem* raw = nullptr;
        if (SUCCEEDED(SHCreateItemFromIDList(g_navItems[spacerId].pidl,
                                              IID_IShellItem, (void**)&raw)) && raw)
        {
            pNsc->RemoveRoot(raw);
            raw->Release();
        }
    }
    ts.homeSpacerItem = nullptr;
}

static LRESULT CALLBACK SepParentSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);

static void EnsureParentSubclass(HWND hTree)
{
    HWND parent = GetParent(hTree);
    if (!parent)
        return;
    {
        std::lock_guard<std::recursive_mutex> lock(g_treesMutex);
        if (g_subclassedParents.count(parent))
            return;
    }
    bool ok = WindhawkUtils::SetWindowSubclassFromAnyThread(parent, SepParentSubclassProc, (DWORD_PTR)hTree);
    Wh_Log(L"[SEP-PARENT] parent=%04X tree=%04X ok=%d", PTR4(parent), PTR4(hTree), ok);
    if (ok)
    {
        std::lock_guard<std::recursive_mutex> lock(g_treesMutex);
        g_subclassedParents.insert(parent);
    }
}

static void RestoreTree(HWND hTree)
{
    Wh_Log(L"[RESTORE] tree=%04X thread=%u", PTR4(hTree), GetCurrentThreadId());
    TreeState* ts = GetTree(hTree);
    if (!ts || !IsWindow(hTree) || !ts->pNscTree)
    {
        Wh_Log(L"[RESTORE] bail: ts=%p isWindow=%d pNsc=%p", ts, IsWindow(hTree), ts ? ts->pNscTree : nullptr);
        return;
    }

    void *pNscRaw = ts->pNscTree;
    unsigned long enumFlags = ts->enumFlags;
    IShellItemFilter *pFilterRaw = ts->pFilter;
    ts->pFilter = nullptr;
    if (pFilterRaw)
        pFilterRaw->AddRef();
    HIMAGELIST savedImgList = ts->savedStateImageList;
    bool ownsRef = ts->ownsNscRef;

    RemoveHomeSpacer(*ts, (INameSpaceTreeControl *)pNscRaw);

    // Remove our roots
    RootShellItems roots;
    if (!roots.Create()) return;
    roots.RemoveInsertableRoots((INameSpaceTreeControl *)pNscRaw);

    if (roots.items[NAV_DESKTOP] && pNscRaw)
    {
        roots.items[NAV_THISPC] = {};

        g_ins.forTree = hTree;
        g_inCustomAppend = true;
        AppendRoot_orig(pNscRaw, roots.items[NAV_DESKTOP].get(), enumFlags, 0x1, pFilterRaw);
        g_inCustomAppend = false;
        g_ins.forTree = nullptr;

        WCHAR collapseNames[NAV_COUNT][64] = {};
        int collapseCount = BuildMatchList(collapseNames, NAV_COUNT,
            [](int id, const NavItemSettings&) {
                return IsInsertableItem(id) && g_navItems[id].label[0];
            });
        if (collapseCount > 0)
            CollapseMatchingItems(hTree, nullptr, collapseNames, collapseCount);
    }

    if (pFilterRaw)
        pFilterRaw->Release();

    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hTree, TreeInteractionProc);

    // Restore pin icons
    if (savedImgList)
    {
        SendMessageW(hTree, TVM_SETIMAGELIST, TVSIL_STATE, (LPARAM)savedImgList);
    }

    ts = GetTree(hTree);
    if (ts)
    {
        if (ownsRef)
            ts->ownsNscRef = false;
        ts->pNscTree = nullptr;
    }

    RemovePropW(hTree, L"WH_NscTree");
    RemovePropW(hTree, L"WH_EnumFlags");

    if (ownsRef)
        ((INameSpaceTreeControl *)pNscRaw)->Release();

    Wh_Log(L"[DISABLE] tree=%04X imglist=%04X", PTR4(hTree), PTR4(savedImgList));
}

static LRESULT CALLBACK RestoreSubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData)
{
    if (uMsg == WM_RESTORE_TREE)
    {
        Wh_Log(L"[RESTORE-SC] tree=%04X thread=%u", PTR4(hWnd), GetCurrentThreadId());
        g_mutatingTree = hWnd;
        RestoreTree(hWnd);
        g_mutatingTree = nullptr;
        if (g_ins.filter) { g_ins.filter->Release(); g_ins.filter = nullptr; }
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, RestoreSubclassProc);
        if (IsWindow(hWnd))
            RedrawWindow(hWnd, nullptr, nullptr,
                         RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
        return 0x5748;
    }
    if (uMsg == WM_NCDESTROY)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, RestoreSubclassProc);
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static void FullRebuildTree(HWND hTree)
{
    TreeState* ts = GetTree(hTree);
    if (!ts || !ts->pNscTree || !IsWindow(hTree))
        return;

    EnsureParentSubclass(hTree);

    HTREEITEM hSelBefore = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_CARET, 0);
    Wh_Log(L"[REBUILD-START] tree=%04X sel=%04X thread=%u", PTR4(hTree), PTR4(hSelBefore), GetCurrentThreadId());

    ComRef<INameSpaceTreeControl> pNsc((INameSpaceTreeControl *)ts->pNscTree);
    void *pNscRaw = ts->pNscTree;
    unsigned long enumFlags = ts->enumFlags;

    if (ts->pFilter)
    {
        ts->pFilter->Release();
        ts->pFilter = nullptr;
    }

    // Remove HomeSpacer before roots — it's a separate namespace item.
    RemoveHomeSpacer(*ts, pNsc.get());

    RootShellItems roots;
    if (!roots.Create()) return;
    roots.RemoveInsertableRoots(pNsc.get());

    if (roots.items[NAV_HOME])
        pNsc->RemoveRoot(roots.items[NAV_HOME].get());
    if (roots.items[NAV_GALLERY])
        pNsc->RemoveRoot(roots.items[NAV_GALLERY].get());

    if (roots.items[NAV_DESKTOP])
    {
        roots.items[NAV_THISPC] = {};
        roots.items[NAV_HOME] = {};
        roots.items[NAV_GALLERY] = {};
        pNsc = {};

        ts = GetTree(hTree);
        if (ts)
        {
            for (int i = 0; i < NAV_COUNT; i++)
                ts->hItems[i] = nullptr;
            ResetTreeCleanup(*ts);
        }

        g_ins.pNsc = pNscRaw;
        g_ins.enumFlags = enumFlags;

        g_ins.forTree = hTree;
        g_inCustomAppend = true;
        AppendRoot_orig(pNscRaw, roots.items[NAV_DESKTOP].get(), enumFlags, 0x1, nullptr);
        g_inCustomAppend = false;
        g_ins.forTree = nullptr;

        ts = GetTree(hTree);
        if (ts)
        {
            ts->pendingWork |= WORK_HOT_INSERT;
        }
        ResetSepColor();

        HTREEITEM hSelAfter = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_CARET, 0);
        InvalidateRect(hTree, nullptr, TRUE);
        Wh_Log(L"[REBUILD-END] tree=%04X sel=%04X→%04X hHome=%04X hGallery=%04X",
               PTR4(hTree), PTR4(hSelBefore), PTR4(hSelAfter),
               PTR4(ts ? ts->hItems[NAV_HOME] : nullptr), PTR4(ts ? ts->hItems[NAV_GALLERY] : nullptr));
    }
}

// Must use AppendRoot_orig (not raw TVM_INSERTITEM) so the item has namespace backing for Explorer's paint path.
static void InsertHomeSpacer(HWND hTree)
{
    TreeState* ts = GetTree(hTree);
    if (!ts) return;
    ts->triedHomeSpacer = true;
    // Re-check: Desktop must be bottom of our section (alone or below This PC).
    bool desktopAtBottom = ts->hItems[NAV_DESKTOP] &&
        (!ts->hItems[NAV_THISPC] || !g_settings.desktopAboveThisPC);
    if (!ts->pNscTree || !desktopAtBottom) return;
    if (GetHiddenItem(*ts)) return;

    int spacerId = g_navItems[NAV_HOME].pidl ? NAV_HOME
                 : (g_navItems[NAV_GALLERY].pidl ? NAV_GALLERY : -1);
    if (spacerId < 0) return;

    IShellItem* raw = nullptr;
    if (FAILED(SHCreateItemFromIDList(g_navItems[spacerId].pidl,
                                      IID_IShellItem, (void**)&raw)) || !raw)
        return;
    ComRef<IShellItem> spacer(raw, false);

    void *pNsc = ts->pNscTree;
    unsigned long enumFlags = ts->enumFlags;

    g_spacerInsertAfter = ts->hItems[NAV_DESKTOP];
    g_ins.forTree = hTree;
    g_inCustomAppend = true;
    AppendRoot_orig(pNsc, spacer.get(), enumFlags, 0, nullptr);
    g_inCustomAppend = false;
    g_ins.forTree = nullptr;
    g_spacerInsertAfter = nullptr;

    ts = GetTree(hTree);
    if (!ts) return;

    HTREEITEM hSpacer = nullptr;
    WCHAR spacerNames[1][64] = {};
    wcsncpy_s(spacerNames[0], 64, g_navItems[spacerId].label, _TRUNCATE);
    ForEachLabeledSibling(hTree, [&](HTREEITEM h) { return h == ts->hItems[NAV_DESKTOP] || h == ts->hItems[NAV_THISPC]; },
        spacerNames, 1, [&](HTREEITEM h, int) -> bool { hSpacer = h; return false; });

    if (hSpacer)
    {
        ts->homeSpacerItem = hSpacer;
        SetItemIntegral(hTree, hSpacer, 1);
    }

    Wh_Log(L"[SPACER] tree=%04X %s h=%04X after=%04X",
           PTR4(hTree), g_navItems[spacerId].label, PTR4(hSpacer), PTR4(ts->hItems[NAV_DESKTOP]));

    InvalidateRect(hTree, nullptr, TRUE);
}

struct SectionLayout {
    HTREEITEM boundary;       // first non-section item after our items
    HTREEITEM belowQA;        // first tall non-section item after boundary
    bool homePresent;         // a visible Home item exists among depth-1 siblings
    bool galleryPresent;      // a visible Gallery item exists among depth-1 siblings
    bool needsHgCleanup;      // hidden inherited items found that need removal
};

static SectionLayout FindSectionLayout(HWND hTree, TreeState& ts)
{
    SectionLayout layout = {};

    // Text fallback for inherited items whose handles haven't been cached yet.
    WCHAR inheritedNames[NAV_COUNT][64] = {};
    bool inheritedHidden[NAV_COUNT] = {};
    for (int i = NAV_HOME; i < NAV_COUNT; i++)
    {
        inheritedHidden[i] = g_settings.items[i].hide;
        if (!ts.hItems[i])
            wcsncpy_s(inheritedNames[i], 64, g_navItems[i].label, _TRUNCATE);
    }

    int baseHeight = GetBaseItemHeight(hTree);
    int tallThreshold = baseHeight + baseHeight / 2;

    bool passedOurs = false;
    ForEachDepth1Item(hTree, [&](HTREEITEM h) -> bool {
        bool isSection = IsOurSection(ts, h);

        // Text fallback for inherited items with null handles
        if (!isSection)
        {
            WCHAR text[64] = {};
            bool textFetched = false;
            for (int i = NAV_HOME; i < NAV_COUNT; i++)
            {
                if (inheritedNames[i][0])
                {
                    if (!textFetched)
                    {
                        GetItemText(hTree, h, text, ARRAYSIZE(text));
                        textFetched = true;
                    }
                    if (text[0] && wcscmp(text, inheritedNames[i]) == 0)
                    {
                        if (!inheritedHidden[i])
                        {
                            ts.hItems[i] = h;
                            if (i == NAV_HOME) layout.homePresent = true;
                            else if (i == NAV_GALLERY) layout.galleryPresent = true;
                            Wh_Log(L"[CACHE] %s=%04X tree=%04X (walk)", g_navItems[i].label, PTR4(h), PTR4(hTree));
                        }
                        else
                        {
                            layout.needsHgCleanup = true;
                        }
                        isSection = true;
                        inheritedNames[i][0] = L'\0';
                        break;
                    }
                }
            }
        }

        if (isSection)
        {
            if (ts.hItems[NAV_HOME] && h == ts.hItems[NAV_HOME])
                layout.homePresent = true;
            else if (ts.hItems[NAV_GALLERY] && h == ts.hItems[NAV_GALLERY])
                layout.galleryPresent = true;
            passedOurs = true;
        }
        else if (passedOurs && h != GetHiddenItem(ts))
        {
            if (!layout.boundary)
            {
                layout.boundary = h;
            }
            else if (baseHeight > 0)
            {
                RECT rc = {};
                if (GetItemRect(hTree, h, &rc))
                {
                    int ih = rc.bottom - rc.top;
                    if (ih >= tallThreshold)
                    {
                        layout.belowQA = h;
                        return false;
                    }
                }
            }
        }
        return true;
    });
    return layout;
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
        if (g_firstSepY == -1)
            g_firstSepY = sepY;
        g_lastSepY = sepY;
    }
}

// Redraws wanted separators at CDDS_POSTPAINT after CDDS_PREPAINT hid all native ones.

static bool HasOurItemsInTree(const TreeState& ts)
{
    return ts.hItems[NAV_THISPC] || ts.hItems[NAV_DESKTOP];
}

static void RedrawSeps(HWND hTree, HDC hdc, TreeState& ts)
{
    if (!hdc || !IsWindow(hTree) || !HasOurItemsInTree(ts))
        return;

    RECT client;
    GetClientRect(hTree, &client);
    if (client.right <= 0 || client.bottom <= 0)
        return;

    int baseHeight = GetBaseItemHeight(hTree);
    if (baseHeight <= 0)
        baseHeight = 48;

    int tallThreshold = baseHeight + baseHeight / 2;

    bool foundBoundary = false;
    bool foundBelowQA = false;
    int sepCount = 0;

    if (ts.boundaryItem)
    {
        RECT rcBoundary = {};
        if (GetItemRect(hTree, ts.boundaryItem, &rcBoundary) && rcBoundary.bottom <= 0)
            foundBoundary = true;
    }

    // Walk log tags: O=ours, H=home, G=gallery, P=spacer, B/b=boundary, Q=belowQA, S=sep, .=other
    WCHAR walkLog[64] = {};
    int walkIdx = 0;

    HTREEITEM h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_FIRSTVISIBLE, 0);
    while (h)
    {
        if (IsDepth1Item(hTree, h))
        {
            RECT rc = {};
            int ih = 0;
            if (GetItemRect(hTree, h, &rc))
                ih = rc.bottom - rc.top;

            if (IsOurSection(ts, h))
            {
                if (g_logSepDraw && walkIdx < 60)
                {
                    WCHAR tag = L'O';
                    if (ts.homeSpacerItem && h == ts.homeSpacerItem) tag = L'P';
                    else if (ts.hItems[NAV_HOME] && h == ts.hItems[NAV_HOME]) tag = L'H';
                    else if (ts.hItems[NAV_GALLERY] && h == ts.hItems[NAV_GALLERY]) tag = L'G';
                    walkLog[walkIdx++] = tag;
                }
            }
            else if (GetHiddenItem(ts) && h == GetHiddenItem(ts))
            {
                if (ts.boundaryItem && h == ts.boundaryItem)
                    foundBoundary = true;
                if (g_logSepDraw && walkIdx < 60)
                    walkLog[walkIdx++] = L'D';
            }
            else
            {
                bool isTall = (ih >= tallThreshold);
                bool drawSep = false;
                int sepY = 0;
                WCHAR tag = L'.';

                if (ts.boundaryItem && h == ts.boundaryItem)
                {
                    foundBoundary = true;
                    tag = isTall ? L'B' : L'b';
                    if (!g_settings.removeSepBelowNav && !GetHiddenItem(ts))
                    {
                        drawSep = true;
                        sepY = isTall ? rc.top + baseHeight / 2 : rc.top;
                    }
                }
                else if (ts.belowQAItem && h == ts.belowQAItem)
                {
                    foundBelowQA = true;
                    if (isTall && !g_settings.removeSepBelowQA)
                    {
                        drawSep = true;
                        sepY = rc.top + baseHeight / 2;
                        tag = L'S';
                    }
                    else
                        tag = L'Q';
                }
                else if (foundBoundary && isTall && !foundBelowQA)
                {
                    drawSep = true;
                    sepY = rc.top + baseHeight / 2;
                    tag = L'S';
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

        h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_NEXTVISIBLE, (LPARAM)h);
    }
    walkLog[walkIdx] = 0;
    if (g_logSepDraw)
    {
        Wh_Log(L"[SEP-DRAW] tree=%04X walk=[%s] drew=%d hHome=%04X hGallery=%04X", PTR4(hTree), walkLog, sepCount, PTR4(ts.hItems[NAV_HOME]), PTR4(ts.hItems[NAV_GALLERY]));
    }
}

// Parent subclass: CDDS_PREPAINT suppresses native separators, CDDS_POSTPAINT redraws wanted ones.
static LRESULT CALLBACK SepParentSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData)
{
    HWND hTree = (HWND)dwRefData;

    if (uMsg == WM_NOTIFY && hTree && IsWindow(hTree))
    {
        LPNMHDR hdr = (LPNMHDR)lParam;
        if (hdr && hdr->hwndFrom == hTree)
        {
            if (hdr->code == TVN_SELCHANGINGW || hdr->code == TVN_SELCHANGINGA)
            {
                if (hTree == g_mutatingTree || g_inOurSelect || AreWeMutating())
                    return 0;
                LPNMTREEVIEWW nm = (LPNMTREEVIEWW)lParam;
                if (nm->action != TVC_BYMOUSE && nm->action != TVC_BYKEYBOARD &&
                    nm->itemNew.hItem && IsDepth1Item(hTree, nm->itemNew.hItem))
                {
                    Wh_Log(L"[SEL-BLOCK] tree=%04X item=%04X action=%d",
                           PTR4(hTree), PTR4(nm->itemNew.hItem), nm->action);
                    return TRUE;
                }
            }

            if (hdr->code == TVN_SELCHANGEDW || hdr->code == TVN_SELCHANGEDA)
            {
                if (hTree == g_mutatingTree)
                    return 0;
                LPNMTREEVIEWW nm = (LPNMTREEVIEWW)lParam;
                if (nm->action != TVC_BYMOUSE && nm->action != TVC_BYKEYBOARD &&
                    nm->itemNew.hItem && IsDepth1Item(hTree, nm->itemNew.hItem))
                    return 0;
            }

            if (hdr->code == (UINT)NM_CUSTOMDRAW && hTree != g_mutatingTree)
            {
                LPNMTVCUSTOMDRAW cd = (LPNMTVCUSTOMDRAW)lParam;
                DWORD stage = cd->nmcd.dwDrawStage;
                TreeState* ts = GetTree(hTree);
                if (stage == CDDS_PREPAINT && g_settings.hasItemsAtTop)
                {
                    // Only suppress native separators once our items are in this tree.
                    if (ts && HasOurItemsInTree(*ts))
                        return CDRF_NOTIFYPOSTPAINT;
                    LRESULT r = DefSubclassProc(hWnd, uMsg, wParam, lParam);
                    return r | CDRF_NOTIFYPOSTPAINT;
                }

                if (stage == CDDS_POSTPAINT)
                {
                    if (g_sepColor == CLR_INVALID && g_settings.hasItemsAtTop)
                    {
                        g_sepColor = DeriveSepColor(hTree);
                        g_logSepDraw = true;
                        Wh_Log(L"[SEP] 0x%06X tree=%04X", g_sepColor, PTR4(hTree));
                    }

                    HDC hdc = cd->nmcd.hdc;
                    HRGN hOldClip = CreateRectRgn(0, 0, 0, 0);
                    int clipState = GetClipRgn(hdc, hOldClip);
                    SelectClipRgn(hdc, nullptr);

                    if (g_settings.hasItemsAtTop && g_sepColor != CLR_INVALID && ts)
                        RedrawSeps(hTree, hdc, *ts);

                    ts = GetTree(hTree);
                    HTREEITEM hHidden = (ts && HasOurItemsInTree(*ts)) ? GetHiddenItem(*ts) : nullptr;
                    if (hHidden)
                    {
                        RECT rcHide = {};
                        if (GetItemRect(hTree, hHidden, &rcHide))
                        {
                            HDC hdc = cd->nmcd.hdc;
                            RECT client;
                            GetClientRect(hTree, &client);
                            COLORREF bg = GetTreeBgColor(hTree);
                            HBRUSH bgBrush = CreateSolidBrush(bg);
                            if (bgBrush)
                            {
                                FillRect(hdc, &rcHide, bgBrush);
                                DeleteObject(bgBrush);
                            }
                            if (g_sepColor != CLR_INVALID)
                            {
                                bool drawSep = (hHidden == ts->homeSpacerItem);
                                if (!drawSep)
                                {
                                    HTREEITEM hPrev = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_PREVIOUS, (LPARAM)hHidden);
                                    drawSep = hPrev && IsOurSection(*ts, hPrev);
                                }
                                if (drawSep)
                                {
                                    int h = rcHide.bottom - rcHide.top;
                                    DrawSeparatorLine(hdc, hTree, rcHide.top + h / 2);
                                }
                            }
                        }
                    }

                    SelectClipRgn(hdc, clipState == 1 ? hOldClip : nullptr);
                    DeleteObject(hOldClip);

                    g_logSepDraw = false;
                }
            }

            if (hdr->code == TVN_ITEMEXPANDEDW ||
                hdr->code == TVN_ITEMEXPANDEDA)
            {
                LRESULT r = DefSubclassProc(hWnd, uMsg, wParam, lParam);
                g_logSepDraw = true;
                TreeState* tsExp = GetTree(hTree);
                if (tsExp)
                    tsExp->pendingWork |= WORK_DUP_COLLAPSE;
                InvalidateRect(hTree, NULL, TRUE);
                return r;
            }

        }
    }

    if (uMsg == WM_THEMECHANGED || uMsg == WM_SYSCOLORCHANGE)
        ResetSepColor();

    if (uMsg == WM_NCDESTROY)
    {
        std::lock_guard<std::recursive_mutex> lock(g_treesMutex);
        g_subclassedParents.erase(hWnd);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Returns false if expected dups are missing (async population race).
static bool CleanupDups(HWND hTree, TreeState& ts)
{
    if (!ts.hItems[NAV_THISPC] && !ts.hItems[NAV_DESKTOP])
        return true;

    WCHAR matchNames[2][64] = {};
    int expectedCount = BuildMatchList(matchNames, 2,
        [&](int id, const NavItemSettings& s) {
            return IsInsertableItem(id) && s.showAtTop && s.hideFromQA &&
                   ts.hItems[id] != nullptr;
        });

    if (!expectedCount)
        return true;

    if (!GetFirstDepth1Child(hTree)) return false;

    HTREEITEM toDeleteChildless[8] = {};
    HTREEITEM toDeleteSections[4] = {};
    int childlessCount = 0;
    int sectionCount = 0;

    ForEachLabeledSibling(hTree, [&](HTREEITEM h) { return IsOurSection(ts, h); }, matchNames, expectedCount,
        [&](HTREEITEM h, int) -> bool {
            HTREEITEM hChild = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)h);
            if (!hChild) { if (childlessCount < 8) toDeleteChildless[childlessCount++] = h; }
            else { if (sectionCount < 4) toDeleteSections[sectionCount++] = h; }
            return true;
        });

    // Redraw freeze prevents Explorer from synchronously re-inserting during deletion.
    bool needDelete = (sectionCount > 0 || childlessCount > 0);
    HTREEITEM prevDup = ts.hiddenDuplicate;
    if (needDelete)
    {
        RedrawFreeze freeze(hTree);

        for (int i = 0; i < sectionCount; i++)
        {
            Wh_Log(L"[DUP] del section=%04X", PTR4(toDeleteSections[i]));
            SendMessageW(hTree, TVM_DELETEITEM, 0, (LPARAM)toDeleteSections[i]);
        }

        ts.hiddenDuplicate = nullptr;

        SectionLayout layout = FindSectionLayout(hTree, ts);
        HTREEITEM hBoundaryPos = layout.boundary;
        for (int i = 0; i < childlessCount; i++)
        {
            if (!g_settings.removeSepBelowNav && !ts.hiddenDuplicate && toDeleteChildless[i] == hBoundaryPos)
            {
                ts.hiddenDuplicate = toDeleteChildless[i];
                SetItemIntegral(hTree, toDeleteChildless[i], 1);
                if (ts.hiddenDuplicate != prevDup)
                    Wh_Log(L"[DUP] hide dup=%04X", PTR4(toDeleteChildless[i]));
                continue;
            }
            Wh_Log(L"[DUP] del dup=%04X", PTR4(toDeleteChildless[i]));
            SendMessageW(hTree, TVM_DELETEITEM, 0, (LPARAM)toDeleteChildless[i]);
        }
    }

    if (needDelete || ts.hiddenDuplicate != prevDup)
        Wh_Log(L"[DUP] tree=%04X n=%d exp=%d",
               PTR4(hTree), childlessCount + sectionCount, expectedCount);

    return true;
}

static bool RemoveInherited(HWND hTree, TreeState& ts)
{
    if (!IsWindow(hTree)) return true;

    struct Target { int id; WCHAR name[64]; };
    Target targets[NAV_COUNT - NAV_HOME] = {};
    int targetCount = 0;

    for (int i = NAV_HOME; i < NAV_COUNT; i++)
    {
        if (!g_settings.items[i].hide || !g_navItems[i].label[0]) continue;
        targets[targetCount].id = i;
        wcsncpy_s(targets[targetCount].name, 64, g_navItems[i].label, _TRUNCATE);
        if (targets[targetCount].name[0]) targetCount++;
    }
    if (!targetCount) return true;

    if (!GetFirstDepth1Child(hTree)) return false;

    struct { HTREEITEM h; int id; } toDelete[NAV_COUNT] = {};
    int delCount = 0;

    WCHAR targetNames[NAV_COUNT][64] = {};
    for (int i = 0; i < targetCount; i++)
        wcsncpy_s(targetNames[i], 64, targets[i].name, _TRUNCATE);

    ForEachLabeledSibling(hTree, [&](HTREEITEM h) { return IsOurSection(ts, h) || h == ts.hiddenDuplicate || h == ts.homeSpacerItem; },
        targetNames, targetCount, [&](HTREEITEM h, int j) -> bool {
            toDelete[delCount++] = { h, targets[j].id };
            return delCount < targetCount;
        });

    if (delCount > 0)
    {
        RedrawFreeze freeze(hTree);
        for (int i = 0; i < delCount; i++)
        {
            Wh_Log(L"[HIDE] del '%s'=%04X",
                   g_navItems[toDelete[i].id].label, PTR4(toDelete[i].h));
            SendMessageW(hTree, TVM_DELETEITEM, 0, (LPARAM)toDelete[i].h);
            ts.hItems[toDelete[i].id] = nullptr;
        }
    }
    return true;
}

static bool QaCleanupStep(HWND h, TreeState& ts) { return CleanupDups(h, ts); }
static bool HgCleanupStep(HWND h, TreeState& ts) { return RemoveInherited(h, ts); }

// Pre-clear: TVE_COLLAPSE re-sets WORK_DUP_COLLAPSE, so clearing before lets that survive.
static void DupCollapse(HWND h)
{
    TreeState* ts = GetTree(h);
    if (!ts) return;
    WCHAR collapseText[NAV_COUNT][64] = {};
    int collapseCount = BuildMatchList(collapseText, NAV_COUNT,
        [](int id, const NavItemSettings& s) {
            return IsInsertableItem(id) && s.showAtTop && !s.hideFromQA;
        });
    if (collapseCount > 0)
        CollapseMatchingItems(h, ts, collapseText, collapseCount);
}

// --- SubClassTreeWndProc ---

using SubClassTreeWndProc_t = LRESULT (CALLBACK *)(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

SubClassTreeWndProc_t SubClassTreeWndProc_orig;

LRESULT CALLBACK SubClassTreeWndProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    // Re-entered messages pass through to avoid deadlocks from other mods' global CallWndProc hooks.
    if (g_inSubclassProc > 0 &&
        uMsg != TVM_INSERTITEMW && uMsg != TVM_INSERTITEMA &&
        uMsg != WM_NCDESTROY)
    {
        return SubClassTreeWndProc_orig(hWnd, uMsg, wParam, lParam, uIdSubclass, dwRefData);
    }

    if (uMsg == TVM_INSERTITEMW || uMsg == TVM_INSERTITEMA)
    {
        bool isOurInsert = (g_ins.item >= 0 && g_ins.item < NAV_COUNT) &&
                           (!g_ins.forTree || hWnd == g_ins.forTree);

        if (isOurInsert && IsInsertableItem(g_ins.item) && lParam)
        {
            TVINSERTSTRUCTW *pInsert = (TVINSERTSTRUCTW *)lParam;
            pInsert->hInsertAfter = TVI_FIRST;
        }
        else if (g_spacerInsertAfter && lParam && (!g_ins.forTree || hWnd == g_ins.forTree))
        {
            TVINSERTSTRUCTW *pInsert = (TVINSERTSTRUCTW *)lParam;
            pInsert->hInsertAfter = g_spacerInsertAfter;
            g_spacerInsertAfter = nullptr;
        }

        LRESULT result = SubClassTreeWndProc_orig(hWnd, uMsg, wParam, lParam, uIdSubclass, dwRefData);
        HTREEITEM hNew = (HTREEITEM)result;
        if (hNew)
        {
            if (isOurInsert)
            {
                int id = g_ins.item;
                g_ins.item = -1;
                std::lock_guard<std::recursive_mutex> lock(g_treesMutex);
                if (!GetTree(hWnd) && !IsNavPaneHost(hWnd))
                    return result;
                auto [it, _] = g_trees.try_emplace(hWnd);
                TreeState& ts = it->second;
                ts.hItems[id] = hNew;

                if (IsInsertableItem(id))
                {
                    ts.hiddenDuplicate = nullptr;
                    ts.pendingWork |= WORK_QA_CLEANUP | WORK_EXPAND;
                    if (!ts.pNscTree)
                    {
                        ts.pNscTree = g_ins.pNsc;
                        ts.enumFlags = g_ins.enumFlags;
                        if (g_ins.filter && !ts.pFilter)
                        {
                            g_ins.filter->AddRef();
                            ts.pFilter = g_ins.filter;
                        }
                    }
                    SetPropW(hWnd, L"WH_NscTree", (HANDLE)ts.pNscTree);
                    SetPropW(hWnd, L"WH_EnumFlags", (HANDLE)(ULONG_PTR)ts.enumFlags);
                    WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, RestoreSubclassProc, 0);
                }
                else if (g_settings.hasItemsAtTop && !g_settings.items[id].hide)
                {
                    SetItemIntegral(hWnd, hNew, 1);
                }
                Wh_Log(L"[CACHE] %s=%04X tree=%04X", g_navItems[id].label, PTR4(hNew), PTR4(hWnd));
            }
            else
            {
                TreeState* ts = GetTree(hWnd);
                if (ts && IsDepth1Item(hWnd, hNew))
                {
                    WCHAR itemText[64] = {};
                    GetItemText(hWnd, hNew, itemText, ARRAYSIZE(itemText));
                    bool isHome = g_navItems[NAV_HOME].label[0] &&
                                  wcscmp(itemText, g_navItems[NAV_HOME].label) == 0;
                    bool isGallery = g_navItems[NAV_GALLERY].label[0] &&
                                     wcscmp(itemText, g_navItems[NAV_GALLERY].label) == 0;
                    if ((isHome && g_settings.items[NAV_HOME].hide) ||
                        (isGallery && g_settings.items[NAV_GALLERY].hide))
                    {
                        ts->pendingWork |= WORK_HG_CLEANUP;
                        Wh_Log(L"[INSERT] '%s' item=%04X tree=%04X",
                               itemText, PTR4(hNew), PTR4(hWnd));
                    }
                    bool hasOurItems = false;
                    for (int i = NAV_THISPC; i <= NAV_DESKTOP; i++)
                        if (ts->hItems[i]) { hasOurItems = true; break; }

                    if (hasOurItems && !(isHome || isGallery) && !AreWeMutating() && !g_inTreePaint &&
                        !(ts->pendingWork & WORK_FULL_REBUILD) && !ts->freshFromAppend)
                    {
                        ts->pendingWork |= WORK_FULL_REBUILD;
                        PostMessage(hWnd, WM_DEFERRED_REBUILD, 0, 0);
                        Wh_Log(L"[INSERT] depth1=%04X tree=%04X", PTR4(hNew), PTR4(hWnd));
                    }
                    else
                    {
                        ts->pendingWork |= WORK_QA_CLEANUP;
                    }
                }
            }
        }
        return result;
    }

    if (uMsg == TVM_SETITEMW || uMsg == TVM_SETITEMA)
    {
        TVITEMEXW* tvi = (TVITEMEXW*)lParam;
        if (tvi && (tvi->mask & TVIF_INTEGRAL))
        {
            TreeState* ts = GetTree(hWnd);
            HTREEITEM hiddenDup = ts ? GetHiddenItem(*ts) : nullptr;
            // Hidden dup: clamp any non-1 value (not just >= 2).
            if (hiddenDup && tvi->hItem == hiddenDup && tvi->iIntegral != 1)
                tvi->iIntegral = 1;
            else if (ts && tvi->iIntegral >= 2 && ShouldBeUnitHeight(*ts, hWnd, tvi->hItem))
                tvi->iIntegral = 1;
        }
    }

    {
        TreeState* ts = GetTree(hWnd);
        HTREEITEM hiddenDup = ts ? GetHiddenItem(*ts) : nullptr;

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
                {
                    g_inOurSelect = true;
                    SendMessageW(hWnd, TVM_SELECTITEM, TVGN_CARET, (LPARAM)hNext);
                    g_inOurSelect = false;
                }
            }
            return r;
        }
    }

    if (uMsg == WM_DEFERRED_REBUILD)
    {
        if (g_deferredOpInProgress)
        {
            g_pendingRebuildTrees.insert(hWnd);
            return 0;
        }

        g_inSubclassProc++;
        g_deferredOpInProgress = true;
        g_mutatingTree = hWnd;

        TreeState* ts = GetTree(hWnd);
        ts = RunDeferredWork(hWnd, ts, WORK_FULL_REBUILD, FullRebuildTree);
        ts = RunDeferredWork(hWnd, ts, WORK_HOT_INSERT,   HotEnableInsert);
        ts = RunDeferredWorkIf(hWnd, ts, WORK_QA_CLEANUP,
            (g_settings.items[NAV_THISPC].showAtTop && g_settings.items[NAV_THISPC].hideFromQA) ||
            (g_settings.items[NAV_DESKTOP].showAtTop && g_settings.items[NAV_DESKTOP].hideFromQA),
            QaCleanupStep);
        ts = RunDeferredWorkIf(hWnd, ts, WORK_HG_CLEANUP,
            ts && ts->pNscTree &&
            (g_navItems[NAV_HOME].pidl || g_navItems[NAV_GALLERY].pidl) &&
            (g_settings.items[NAV_HOME].hide || g_settings.items[NAV_GALLERY].hide),
            HgCleanupStep);
        ts = RunDeferredWork(hWnd, ts, WORK_DUP_COLLAPSE, DupCollapse);
        ts = RunDeferredWork(hWnd, ts, WORK_EXPAND, ExpandStartExpandedItems);
        ts = RunDeferredWork(hWnd, ts, WORK_HOME_SPACER, InsertHomeSpacer);

        if (ts)
        {
            ts->freshFromAppend = false;
            ts->sepRetries = 0;
        }
        g_inSubclassProc--;
        DrainPendingRebuilds();
        return 0;
    }

    if (uMsg == WM_SETTINGS_CHANGED)
    {
        auto* info = reinterpret_cast<SettingsChangeInfo*>(lParam);
        HTREEITEM hSel = (HTREEITEM)SendMessageW(hWnd, TVM_GETNEXTITEM, TVGN_CARET, 0);
        Wh_Log(L"[SETTINGS-MSG] tree=%04X tier=%d sel=%04X thread=%u",
               PTR4(hWnd), (int)info->tier, PTR4(hSel), GetCurrentThreadId());
        TreeState* ts = GetTree(hWnd);
        if (!ts || !IsWindow(hWnd) || !ts->pNscTree)
            return 0;

        ts->sepRetries = 0;

        if (info->tier == TIER_REBUILD)
        {
            g_deferredOpInProgress = true;
            g_mutatingTree = hWnd;
            FullRebuildTree(hWnd);
            DrainPendingRebuilds();
        }
        else if (info->tier == TIER_REFRESH)
        {
            ts->boundaryItem = nullptr;
            ts->belowQAItem = nullptr;
            ts->belowQADiscovered = false;
            ts->pendingWork |= WORK_QA_CLEANUP;
            RefreshNavPane(hWnd);
        }
        else // TIER_REPAINT
        {
            bool homeGalleryChanged = (info->prev.items[NAV_HOME].hide    != g_settings.items[NAV_HOME].hide ||
                                       info->prev.items[NAV_GALLERY].hide != g_settings.items[NAV_GALLERY].hide);
            bool dedupChanged = false;
            for (int i = NAV_THISPC; i <= NAV_DESKTOP; i++)
                if (g_settings.items[i].showAtTop && info->prev.items[i].hideFromQA != g_settings.items[i].hideFromQA)
                    dedupChanged = true;

            if (homeGalleryChanged)
            {
                for (int j = NAV_HOME; j < NAV_COUNT; j++)
                    if (g_settings.items[j].hide)
                        ts->hItems[j] = nullptr;
                ts->pendingWork |= WORK_HG_CLEANUP;
            }
            if (dedupChanged)
                ts->pendingWork |= WORK_QA_CLEANUP;
            if (info->sepChanged || dedupChanged)
            {
                ts->boundaryItem = nullptr;
                ts->belowQAItem = nullptr;
                ts->belowQADiscovered = false;
            }

            InvalidateRect(hWnd, nullptr, TRUE);
        }
        return 0;
    }

    if (uMsg == WM_PAINT)
    {
        TreeState* ts = GetTree(hWnd);

        constexpr uint8_t DEFER_MASK = WORK_FULL_REBUILD | WORK_HOT_INSERT |
            WORK_QA_CLEANUP | WORK_HG_CLEANUP | WORK_DUP_COLLAPSE | WORK_HOME_SPACER;
        if (ts && (ts->pendingWork & DEFER_MASK))
            PostMessage(hWnd, WM_DEFERRED_REBUILD, 0, 0);

        // Paint-time mutations must be idempotent (no-op when already in target state) to avoid relayout loops.
        if (ts && g_settings.hasItemsAtTop)
        {
            for (int i = NAV_HOME; i < NAV_COUNT; i++)
            {
                if (!ts->hItems[i]) continue;
                if (!g_settings.items[i].hide)
                    CollapseItemIntegral(hWnd, ts->hItems[i]);
            }
            if (ShouldRemoveInternalSep() && ts->hItems[NAV_THISPC] && ts->hItems[NAV_DESKTOP])
                CollapseItemIntegral(hWnd, LowerInsertableItem(*ts));
        }

        SectionLayout layout = {};
        if (g_settings.hasItemsAtTop && ts)
        {
            if (ts->belowQAItem)
            {
                RECT rcCheck = {};
                if (!GetItemRect(hWnd, ts->belowQAItem, &rcCheck))
                    ts->belowQAItem = nullptr;
            }
            layout = FindSectionLayout(hWnd, *ts);
            ts->boundaryItem = layout.boundary;

            bool desktopAtBottom = ts->hItems[NAV_DESKTOP] &&
                (!ts->hItems[NAV_THISPC] || !g_settings.desktopAboveThisPC);
            if (!ts->triedHomeSpacer && layout.boundary && desktopAtBottom
                && !g_settings.removeSepBelowNav && !GetHiddenItem(*ts)
                && !layout.homePresent && !layout.galleryPresent)
            {
                ts->pendingWork |= WORK_HOME_SPACER;
                PostMessage(hWnd, WM_DEFERRED_REBUILD, 0, 0);
            }

            if (layout.needsHgCleanup && !(ts->pendingWork & WORK_HG_CLEANUP))
            {
                ts->pendingWork |= WORK_HG_CLEANUP;
                PostMessage(hWnd, WM_DEFERRED_REBUILD, 0, 0);
            }

            if (g_logSepDraw)
                Wh_Log(L"[SEP-GAP] tree=%04X boundary=%04X int=%d home=%d gallery=%d rmNav=%d hidDup=%04X spacer=%04X tried=%d",
                       PTR4(hWnd), PTR4(layout.boundary),
                       layout.boundary ? GetItemIntegral(hWnd, layout.boundary) : -1,
                       layout.homePresent, layout.galleryPresent,
                       (int)g_settings.removeSepBelowNav, PTR4(ts->hiddenDuplicate), PTR4(ts->homeSpacerItem),
                       (int)ts->triedHomeSpacer);
        }

        if (g_settings.hasItemsAtTop && ts)
        {
            if (layout.boundary && (g_settings.removeSepBelowNav || GetHiddenItem(*ts)))
                CollapseItemIntegral(hWnd, layout.boundary);
            bool treeSettled = !g_deferredOpInProgress && !ts->freshFromAppend && !ts->pendingWork;
            if (layout.belowQA)
            {
                if (g_settings.removeSepBelowQA)
                    CollapseItemIntegral(hWnd, layout.belowQA);
                else if (treeSettled && ts->belowQADiscovered &&
                         layout.belowQA != ts->belowQAItem)
                    CollapseItemIntegral(hWnd, layout.belowQA);
            }
            if (!ts->belowQADiscovered && treeSettled)
            {
                ts->belowQAItem = layout.belowQA;
                ts->belowQADiscovered = true;
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

        if (g_settings.hasItemsAtTop)
            EnsureParentSubclass(hWnd);

        if (ts && GetHiddenItem(*ts))
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TreeInteractionProc, 0);

        g_lastSepY = -1;
        g_firstSepY = -1;
        g_inTreePaint = true;

        LRESULT result = SubClassTreeWndProc_orig(hWnd, uMsg, wParam, lParam, uIdSubclass, dwRefData);

        g_inTreePaint = false;

        ts = GetTree(hWnd);
        if (g_lastSepY >= 0 && g_sepColor != CLR_INVALID && ts && ts->sepRetries < 3)
        {
            HDC hdc = GetDC(hWnd);
            if (hdc)
            {
                RECT client;
                GetClientRect(hWnd, &client);
                int sampleX = client.right / 2;
                bool mismatch = false;
                int failY = g_lastSepY;
                int checkYs[] = { g_firstSepY, g_lastSepY };
                for (int y : checkYs)
                {
                    if (y < 0) continue;
                    COLORREF pixel = GetPixel(hdc, sampleX, y);
                    if (pixel != CLR_INVALID && pixel != g_sepColor)
                        { mismatch = true; failY = y; break; }
                }
                ReleaseDC(hWnd, hdc);
                if (mismatch)
                {
                    ts->sepRetries++;
                    Wh_Log(L"[SEP-VERIFY] tree=%04X at (%d,%d): "
                           L"expected=0x%06X, retry %d",
                           PTR4(hWnd), sampleX, failY, g_sepColor, ts->sepRetries);
                    RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
                }
                else
                    ts->sepRetries = 0;
            }
        }
        else if (g_lastSepY == -1 && g_sepColor != CLR_INVALID
                 && ts && ts->sepRetries < 3 && !ts->pendingWork && HasOurItemsInTree(*ts)
                 && ts->boundaryItem && !g_settings.removeSepBelowNav && !GetHiddenItem(*ts))
        {
            ts->sepRetries++;
            Wh_Log(L"[SEP-VERIFY] tree=%04X boundary=%04X: expected separator but none drawn, retry %d",
                   PTR4(hWnd), PTR4(ts->boundaryItem), ts->sepRetries);
            RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
        }

        return result;
    }

    if (uMsg == WM_NCDESTROY)
    {
        RemovePropW(hWnd, L"WH_NscTree");
        RemovePropW(hWnd, L"WH_EnumFlags");
        IShellItemFilter *pf = nullptr;
        INameSpaceTreeControl *pNsc = nullptr;
        {
            std::lock_guard<std::recursive_mutex> lock(g_treesMutex);
            auto it = g_trees.find(hWnd);
            if (it != g_trees.end())
            {
                pf = it->second.pFilter;
                it->second.pFilter = nullptr;
                if (it->second.ownsNscRef && it->second.pNscTree)
                    pNsc = (INameSpaceTreeControl *)it->second.pNscTree;
                it->second.pNscTree = nullptr;
                g_trees.erase(it);
            }
        }
        if (pf || pNsc)
        {
            if (pf)
                pf->Release();
            if (pNsc)
                pNsc->Release();
        }
    }

    return SubClassTreeWndProc_orig(hWnd, uMsg, wParam, lParam, uIdSubclass, dwRefData);
}

using DrawThemeBackground_t = HRESULT (WINAPI *)(HTHEME, HDC, int, int, LPCRECT, LPCRECT);

DrawThemeBackground_t DrawThemeBackground_orig;

static thread_local bool g_chevronLogged = false;

HRESULT WINAPI DrawThemeBackground_hook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, LPCRECT pClipRect)
{
    if (iPartId == TVP_GLYPH || iPartId == TVP_HOTGLYPH)
    {
        if (!g_chevronLogged && g_inTreePaint)
        {
            g_chevronLogged = true;
            Wh_Log(L"[CHEVRON] DTB hook active, fixEnabled=%d",
                (int)g_settings.fixChevronDrawing);
        }
        if (g_inTreePaint && g_settings.fixChevronDrawing)
        {
            RECT drawRect;
            CalcGlyphRect(pRect, &drawRect);
            DrawChevron(hdc, &drawRect, iPartId, iStateId);
            return S_OK;
        }
    }
    return DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}

// --- Settings ---

void LoadSettings()
{
    g_settings = {};  // zero everything first

    // Per-item settings with explicit YAML paths
    struct ItemSettingDef {
        const wchar_t *showAtTop;
        const wchar_t *expandable;
        const wchar_t *startExpanded;
        const wchar_t *hideFromQA;
        const wchar_t *hide;
    };
    static const ItemSettingDef kItemKeys[NAV_COUNT] = {
        // NAV_THISPC
        { L"ThisPC.showThisPCAtTop", L"ThisPC.thisPCExpandable",
          L"ThisPC.thisPCStartExpanded", L"ThisPC.hideThisPCFromQuickAccess", nullptr },
        // NAV_DESKTOP
        { L"Desktop.showDesktopAtTop", L"Desktop.desktopExpandable",
          nullptr, L"Desktop.hideDesktopFromQuickAccess", nullptr },
        // NAV_HOME
        { nullptr, nullptr, nullptr, nullptr, L"HomeGallery.hideHome" },
        // NAV_GALLERY
        { nullptr, nullptr, nullptr, nullptr, L"HomeGallery.hideGallery" },
    };

    for (int i = 0; i < NAV_COUNT; i++)
    {
        auto& k = kItemKeys[i];
        auto& s = g_settings.items[i];
        if (k.showAtTop)     s.showAtTop     = Wh_GetIntSetting(k.showAtTop);
        if (k.expandable)    s.expandable    = Wh_GetIntSetting(k.expandable);
        if (k.startExpanded) s.startExpanded = Wh_GetIntSetting(k.startExpanded);
        if (k.hideFromQA)    s.hideFromQA    = Wh_GetIntSetting(k.hideFromQA);
        if (k.hide)          s.hide          = Wh_GetIntSetting(k.hide);
    }

    g_settings.desktopAboveThisPC = Wh_GetIntSetting(L"Desktop.desktopAboveThisPC");
    g_settings.fixChevronDrawing  = Wh_GetIntSetting(L"Resources.fixChevronDrawing");
    g_settings.chevronScale       = Wh_GetIntSetting(L"Resources.chevronScale");
    g_settings.hidePinButtons     = Wh_GetIntSetting(L"Resources.hidePinButtons");
    g_settings.removeSepBelowNav  = Wh_GetIntSetting(L"Separators.removeSepBelowNav");
    g_settings.removeSepBelowQA   = Wh_GetIntSetting(L"Separators.removeSepBelowQA");

    // Precompute composite guard
    g_settings.hasItemsAtTop = false;
    for (int i = 0; i < NAV_COUNT; i++)
        if (g_settings.items[i].showAtTop) { g_settings.hasItemsAtTop = true; break; }
}

static void LogSettings()
{
    Wh_Log(L"Settings: thisPCAtTop=%d (expand=%d, startExp=%d, hideQA=%d) "
            L"desktopAtTop=%d (above=%d, expand=%d, hideQA=%d) "
            L"hideHome=%d hideGallery=%d fixChevron=%d chevronScale=%d "
            L"hidePins=%d rmSepNav=%d rmSepQA=%d",
            g_settings.items[NAV_THISPC].showAtTop,
            g_settings.items[NAV_THISPC].expandable,
            g_settings.items[NAV_THISPC].startExpanded,
            g_settings.items[NAV_THISPC].hideFromQA,
            g_settings.items[NAV_DESKTOP].showAtTop,
            g_settings.desktopAboveThisPC,
            g_settings.items[NAV_DESKTOP].expandable,
            g_settings.items[NAV_DESKTOP].hideFromQA,
            g_settings.items[NAV_HOME].hide,
            g_settings.items[NAV_GALLERY].hide,
            g_settings.fixChevronDrawing, g_settings.chevronScale,
            g_settings.hidePinButtons,
            g_settings.removeSepBelowNav, g_settings.removeSepBelowQA);
}

static bool InitializeModCore(HMODULE hExplorerFrame)
{
    LoadSettings();

    auto cleanupOnFail = []() {
        if (g_gdipToken) { Gdiplus::GdiplusShutdown(g_gdipToken); g_gdipToken = 0; }
        for (int i = 0; i < NAV_COUNT; i++)
            if (g_navItems[i].pidl) { CoTaskMemFree(g_navItems[i].pidl); g_navItems[i].pidl = nullptr; }
    };

    for (int i = 0; i < NAV_COUNT; i++)
        g_navItems[i] = {};

    Gdiplus::GdiplusStartupInput gdipIn;
    Gdiplus::GdiplusStartup(&g_gdipToken, &gdipIn, NULL);

    SHGetSpecialFolderLocation(nullptr, CSIDL_DRIVES, &g_navItems[NAV_THISPC].pidl);
    if (!g_navItems[NAV_THISPC].pidl)
    {
        Wh_Log(L"Failed to get This PC PIDL");
        cleanupOnFail();
        return false;
    }

    SHGetSpecialFolderLocation(nullptr, CSIDL_DESKTOP, &g_navItems[NAV_DESKTOP].pidl);

    SHParseDisplayName(L"::{f874310e-b6b7-47dc-bc84-b9e6b38f5903}",
                       nullptr, &g_navItems[NAV_HOME].pidl, 0, nullptr);
    SHParseDisplayName(L"::{e88865ea-0e1c-4e20-9aa6-edcd0212c87c}",
                       nullptr, &g_navItems[NAV_GALLERY].pidl, 0, nullptr);
    for (int i = 0; i < NAV_COUNT; i++)
        if (g_navItems[i].pidl)
            GetPidlDisplayName(g_navItems[i].pidl, g_navItems[i].label, ARRAYSIZE(g_navItems[i].label));

    WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[] = {
        {
            {
#ifdef _WIN64
                L"public: virtual long __cdecl"
                L" CNscTree::AppendRoot("
                L"struct IShellItem *,"
                L"unsigned long,"
                L"unsigned long,"
                L"struct IShellItemFilter *)"
#else
                L"public: virtual long __stdcall"
                L" CNscTree::AppendRoot("
                L"struct IShellItem *,"
                L"unsigned long,"
                L"unsigned long,"
                L"struct IShellItemFilter *)"
#endif
            },
            &AppendRoot_orig,
            AppendRoot_hook,
            false
        },
        {
            {
#ifdef _WIN64
                L"private: static __int64 __cdecl"
                L" CNscTree::s_SubClassTreeWndProc("
                L"struct HWND__ *,"
                L"unsigned int,"
                L"unsigned __int64,"
                L"__int64,"
                L"unsigned __int64,"
                L"unsigned __int64)"
#else
                L"private: static long __stdcall"
                L" CNscTree::s_SubClassTreeWndProc("
                L"struct HWND__ *,"
                L"unsigned int,"
                L"unsigned int,"
                L"long,"
                L"unsigned int,"
                L"unsigned long)"
#endif
            },
            &SubClassTreeWndProc_orig,
            SubClassTreeWndProc_hook,
            false
        },
    };

    if (!WindhawkUtils::HookSymbols(hExplorerFrame, explorerFrameDllHooks, ARRAYSIZE(explorerFrameDllHooks)))
    {
        Wh_Log(L"Failed to hook symbols");
        cleanupOnFail();
        return false;
    }

    HMODULE hUxTheme = GetModuleHandleW(L"uxtheme.dll");
    if (hUxTheme)
    {
        auto pDTB = (DrawThemeBackground_t)GetProcAddress(hUxTheme, "DrawThemeBackground");
        if (pDTB)
        {
            WindhawkUtils::SetFunctionHook(pDTB, DrawThemeBackground_hook, &DrawThemeBackground_orig);
        }
        else
            Wh_Log(L"[CHEVRON] GetProcAddress(DrawThemeBackground) failed");
    }
    else
        Wh_Log(L"[CHEVRON] uxtheme.dll not loaded");

    return true;
}

void Wh_ModAfterInit();

static void CheckLateLoadExplorerFrame(LPCWSTR lpLibFileName, HMODULE hModule)
{
    if (!hModule || !lpLibFileName ||
        g_explorerFrameLoaded.load(std::memory_order_relaxed))
        return;
    LPCWSTR name = wcsrchr(lpLibFileName, L'\\');
    name = name ? name + 1 : lpLibFileName;
    if (_wcsicmp(name, L"ExplorerFrame.dll") != 0 ||
        g_explorerFrameLoaded.exchange(true, std::memory_order_acq_rel))
        return;
    HMODULE pin = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN, L"ExplorerFrame.dll", &pin);
    Wh_Log(L"ExplorerFrame.dll loaded late, installing hooks");
    if (!InitializeModCore(hModule))
    {
        Wh_Log(L"Late-load InitializeModCore failed");
        g_explorerFrameLoaded.store(false, std::memory_order_release);
        return;
    }
    Wh_ApplyHookOperations();
    Wh_Log(L"Late-load hooks installed, running discovery");
    Wh_ModAfterInit();
}

static HMODULE WINAPI LoadLibraryExW_hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
    HMODULE hModule = LoadLibraryExW_orig(lpLibFileName, hFile, dwFlags);
    constexpr DWORD DATA_ONLY = LOAD_LIBRARY_AS_DATAFILE |
        LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE | LOAD_LIBRARY_AS_IMAGE_RESOURCE;
    if (!(dwFlags & DATA_ONLY))
        CheckLateLoadExplorerFrame(lpLibFileName, hModule);
    return hModule;
}

BOOL Wh_ModInit()
{
    HMODULE hExplorerFrame = GetModuleHandleW(L"ExplorerFrame.dll");
    if (!hExplorerFrame)
    {
        auto pLoadLibraryExW = (LoadLibraryExW_t)GetProcAddress(
            GetModuleHandleW(L"kernelbase.dll"), "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pLoadLibraryExW, LoadLibraryExW_hook, &LoadLibraryExW_orig);
        return TRUE;
    }
    HMODULE pin = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN, L"ExplorerFrame.dll", &pin);
    g_explorerFrameLoaded.store(true, std::memory_order_relaxed);
    return InitializeModCore(hExplorerFrame) ? TRUE : FALSE;
}

struct DiscoveredTree {
    HWND hTop;
    HWND hTree;
    INameSpaceTreeControl *pNsc;
    unsigned long enumFlags;
};

static void DiscoverTreeFromBrowser(HWND hTop, const WCHAR *cls,
                                    IShellBrowser *pSB,
                                    std::vector<DiscoveredTree> &out)
{
    IServiceProvider *pSP = nullptr;
    if (FAILED(pSB->QueryInterface(IID_IServiceProvider, (void **)&pSP)))
    {
        Wh_Log(L"[TREE] %s hwnd=%04X no IServiceProvider", cls, PTR4(hTop));
        return;
    }

    INameSpaceTreeControl *pNsc = nullptr;
    HRESULT hr = pSP->QueryService(IID_INameSpaceTreeControl, IID_INameSpaceTreeControl, (void **)&pNsc);
    pSP->Release();
    if (FAILED(hr))
    {
        Wh_Log(L"[TREE] %s hwnd=%04X no INameSpaceTreeControl hr=%08X", cls, PTR4(hTop), hr);
        return;
    }

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
        Wh_Log(L"[TREE] %s hwnd=%04X no SysTreeView32", cls, PTR4(hTop));
        pNsc->Release();
        return;
    }

    // The active tab can resolve through more than one host; don't seed twice.
    for (auto &d : out)
    {
        if (d.hTree == hTree)
        {
            pNsc->Release();
            return;
        }
    }

    unsigned long enumFlags = (unsigned long)(ULONG_PTR)GetPropW(hTree, L"WH_EnumFlags");
    if (!enumFlags)
        enumFlags = SHCONTF_FOLDERS;

    Wh_Log(L"[TREE] %s hwnd=%04X tree=%04X nsc=%04X", cls, PTR4(hTop), PTR4(hTree), PTR4(pNsc));
    out.push_back({ hTop, hTree, pNsc, enumFlags });
}

static BOOL CALLBACK EnumWindowsForTrees(HWND hTop, LPARAM lParam)
{
    auto& out = *reinterpret_cast<std::vector<DiscoveredTree>*>(lParam);

    DWORD wndPid = 0;
    GetWindowThreadProcessId(hTop, &wndPid);
    if (wndPid != GetCurrentProcessId())
        return TRUE;
    WCHAR cls[64];
    GetClassNameW(hTop, cls, ARRAYSIZE(cls));

    bool isCabinet = (wcscmp(cls, L"CabinetWClass") == 0);
    bool isDialog = (wcscmp(cls, L"#32770") == 0);
    if (!isCabinet && !isDialog)
        return TRUE;

    int found = 0;
    HWND hShellTab = nullptr;
    while ((hShellTab = FindWindowExW(hTop, hShellTab, L"ShellTabWindowClass", nullptr)) != nullptr)
    {
        IShellBrowser *pSB = (IShellBrowser *)SendMessageW(hShellTab, WM_USER + 7, 0, 0);
        if (pSB)
        {
            size_t before = out.size();
            DiscoverTreeFromBrowser(hTop, cls, pSB, out);
            found += (int)(out.size() - before);
        }
    }

    if (found == 0)
    {
        IShellBrowser *pSB = (IShellBrowser *)SendMessageW(hTop, WM_USER + 7, 0, 0);
        if (pSB)
            DiscoverTreeFromBrowser(hTop, cls, pSB, out);
    }
    return TRUE;
}

void Wh_ModAfterInit()
{
#ifdef _WIN64
    const size_t OFFSET_SAME_TEB_FLAGS = 0x17EE;
#else
    const size_t OFFSET_SAME_TEB_FLAGS = 0x0FCA;
#endif
    bool isInitialThread = *(USHORT *)((BYTE *)NtCurrentTeb() +
        OFFSET_SAME_TEB_FLAGS) & 0x0400;
    if (isInitialThread)
        return;

    std::vector<DiscoveredTree> discovered;

    EnumWindows(EnumWindowsForTrees, (LPARAM)&discovered);

    for (auto& d : discovered)
    {
        if (!IsWindow(d.hTree))
        {
            d.pNsc->Release();
            continue;
        }

        {
            std::lock_guard<std::recursive_mutex> lock(g_treesMutex);
            TreeState& ts = g_trees[d.hTree];
            ts.pNscTree = (void *)d.pNsc;
            ts.enumFlags = d.enumFlags;
            ts.pendingWork |= WORK_FULL_REBUILD;
            ts.ownsNscRef = true;
        }

        SetPropW(d.hTree, L"WH_NscTree", (HANDLE)d.pNsc);
        SetPropW(d.hTree, L"WH_EnumFlags", (HANDLE)(ULONG_PTR)d.enumFlags);
        WindhawkUtils::SetWindowSubclassFromAnyThread(d.hTree, RestoreSubclassProc, 0);

        PostMessage(d.hTree, WM_DEFERRED_REBUILD, 0, 0);
        Wh_Log(L"[ENABLE] window=%04X tree=%04X nsc=%04X", PTR4(d.hTop), PTR4(d.hTree), PTR4(d.pNsc));
    }

    if (!discovered.empty())
        LogSettings();
}

ChangeTier ClassifySettingsChange(const Settings& prev, const Settings& cur)
{
    ChangeTier tier = TIER_NONE;

    // Per-item transitions
    for (int i = 0; i < NAV_COUNT; i++)
    {
        auto& p = prev.items[i];
        auto& c = cur.items[i];

        if (p.hide && !c.hide)
            tier = std::max(tier, TIER_REBUILD);
        if (!p.hide && c.hide)
            tier = std::max(tier, TIER_REPAINT);

        // hideFromQA toggle in either direction when parent enabled
        if (c.showAtTop && p.hideFromQA != c.hideFromQA)
            tier = std::max(tier, TIER_REBUILD);

        // Parent going off while hideFromQA was on: dups were deleted
        if (p.showAtTop && !c.showAtTop && p.hideFromQA)
            tier = std::max(tier, TIER_REBUILD);

        // Item enable/disable
        if (p.showAtTop != c.showAtTop)
            tier = std::max(tier, TIER_REFRESH);

        // Sub-settings only matter when item is/was at top
        if (c.showAtTop || p.showAtTop)
        {
            if (p.expandable != c.expandable || p.startExpanded != c.startExpanded)
                tier = std::max(tier, TIER_REFRESH);
        }
    }

    // Cross-item: order only matters when both are at top
    if (cur.items[NAV_THISPC].showAtTop && cur.items[NAV_DESKTOP].showAtTop)
    {
        if (prev.desktopAboveThisPC != cur.desktopAboveThisPC)
            tier = std::max(tier, TIER_REFRESH);
    }

    // Spacer lifecycle: rebuild when desktopAtBottom changes (spacer collapsed boundary iIntegral)
    bool prevBottom = prev.items[NAV_DESKTOP].showAtTop &&
        (!prev.items[NAV_THISPC].showAtTop || !prev.desktopAboveThisPC);
    bool curBottom = cur.items[NAV_DESKTOP].showAtTop &&
        (!cur.items[NAV_THISPC].showAtTop || !cur.desktopAboveThisPC);
    if (prevBottom != curBottom)
        tier = std::max(tier, TIER_REBUILD);

    if (prev.hasItemsAtTop && !cur.hasItemsAtTop)
        tier = std::max(tier, TIER_REBUILD);

    // Non-item settings
    if (prev.removeSepBelowNav != cur.removeSepBelowNav)
        tier = std::max(tier, TIER_REBUILD);
    if (prev.removeSepBelowQA && !cur.removeSepBelowQA)
        tier = std::max(tier, TIER_REBUILD);
    if (!prev.removeSepBelowQA && cur.removeSepBelowQA)
        tier = std::max(tier, TIER_REPAINT);

    if (prev.fixChevronDrawing != cur.fixChevronDrawing ||
        prev.chevronScale != cur.chevronScale ||
        prev.hidePinButtons != cur.hidePinButtons)
        tier = std::max(tier, TIER_REPAINT);

    return tier;
}

void Wh_ModSettingsChanged()
{
    static bool inProgress = false, rerunNeeded = false;
    if (inProgress) { rerunNeeded = true; return; }
    inProgress = true;

    do {
        rerunNeeded = false;

        auto prev = g_settings;
        LoadSettings();
        LogSettings();

        ChangeTier tier = ClassifySettingsChange(prev, g_settings);
        if (tier == TIER_NONE)
            break;

        bool sepChanged = (prev.removeSepBelowNav != g_settings.removeSepBelowNav ||
                           prev.removeSepBelowQA  != g_settings.removeSepBelowQA);
        if (sepChanged)
            ResetSepColor();

        SettingsChangeInfo info = { tier, prev, sepChanged };

        std::vector<HWND> treeList;
        {
            std::lock_guard<std::recursive_mutex> lock(g_treesMutex);
            treeList.reserve(g_trees.size());
            for (auto& [hTree, ts] : g_trees)
                treeList.push_back(hTree);
        }

        for (HWND hTree : treeList)
        {
            if (IsWindow(hTree))
                SendMessage(hTree, WM_SETTINGS_CHANGED, 0, (LPARAM)&info);
        }

        if (!prev.hasItemsAtTop && g_settings.hasItemsAtTop)
            Wh_ModAfterInit();
    } while (rerunNeeded);

    inProgress = false;
}

void Wh_ModUninit()
{
    g_settings = {};
    ResetSepColor();

    std::vector<HWND> uninitList;
    {
        std::lock_guard<std::recursive_mutex> lock(g_treesMutex);
        uninitList.reserve(g_trees.size());
        for (auto& [hTree, ts] : g_trees)
            uninitList.push_back(hTree);
    }

    for (HWND hTree : uninitList)
    {
        if (!IsWindow(hTree))
            continue;

        DWORD treeThread = GetWindowThreadProcessId(hTree, nullptr);
        if (treeThread != GetCurrentThreadId())
        {
            LRESULT lr = SendMessage(hTree, WM_RESTORE_TREE, 0, 0);
            if (lr != 0x5748)
            {
                Wh_Log(L"[UNINIT] tree=%04X restore subclass missing, direct fallback", PTR4(hTree));
                g_mutatingTree = hTree;
                RestoreTree(hTree);
                g_mutatingTree = nullptr;
            }
        }
        else
        {
            g_mutatingTree = hTree;
            RestoreTree(hTree);
            g_mutatingTree = nullptr;
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hTree, RestoreSubclassProc);
        }
    }

    std::vector<HWND> parents;
    {
        std::lock_guard<std::recursive_mutex> lock(g_treesMutex);
        parents.assign(g_subclassedParents.begin(), g_subclassedParents.end());
        g_subclassedParents.clear();
    }
    for (HWND parent : parents)
    {
        if (IsWindow(parent))
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(parent, SepParentSubclassProc);
    }

    {
        std::lock_guard<std::recursive_mutex> lock(g_treesMutex);
        for (auto& [h, ts] : g_trees)
        {
            if (ts.pFilter)
            {
                ts.pFilter->Release();
                ts.pFilter = nullptr;
            }
            if (ts.ownsNscRef && ts.pNscTree)
            {
                ((INameSpaceTreeControl *)ts.pNscTree)->Release();
                ts.pNscTree = nullptr;
                ts.ownsNscRef = false;
            }
        }
        g_trees.clear();
    }

    if (g_gdipToken) { Gdiplus::GdiplusShutdown(g_gdipToken); g_gdipToken = 0; }
    for (int i = 0; i < NAV_COUNT; i++)
        if (g_navItems[i].pidl) { CoTaskMemFree(g_navItems[i].pidl); g_navItems[i].pidl = nullptr; }
}