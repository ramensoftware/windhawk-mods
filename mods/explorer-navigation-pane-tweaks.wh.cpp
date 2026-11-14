// ==WindhawkMod==
// @id              explorer-navigation-pane-tweaks
// @name            Explorer Navigation Tree Offset
// @description     Adjusts the Explorer navigation tree offset and optional tree view visual styles
// @version         1.1
// @author          Languster
// @github          https://github.com/Languster
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -luxtheme
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Navigation Tree Offset

Adjusts the horizontal position and visual tree style of the navigation pane in Windows Explorer.

> Recommended: use this mod together with [Explorer TreeItem Tweaker](https://windhawk.net/mods/explorer-treeitem-tweaker) and [Explorer TreeLine Killer](https://windhawk.net/mods/explorer-treeline-killer) for the best navigation tree appearance.

## Before / After

### Default Explorer

![Before](https://raw.githubusercontent.com/Languster/ExplorerNavHook/main/screenshots/before.jpg)

### With Explorer Navigation Tree Offset

![After](https://raw.githubusercontent.com/Languster/ExplorerNavHook/main/screenshots/after.jpg)

## Features

- shifts the whole Explorer navigation tree left or right
- keeps parent and child tree levels moving together
- can remove all expand/collapse glyphs
- can remove only the first/root expand glyph while keeping child glyphs
- can remove connector lines
- item background position compensation

## Settings

- **Navigation tree left shift**: moves the whole navigation tree. `0` keeps Explorer's original position. Positive values move the tree left.
- **Remove all expand/collapse glyphs**: removes the tree expand/collapse glyphs in the navigation pane.
- **Remove first expand glyph**: removes only the first/root expand/collapse glyph while keeping child glyphs. Works when **Remove all expand/collapse glyphs** is disabled.
- **Remove connector lines**: removes the tree connector lines in the navigation pane.
- **Compensate item background position**: when the tree is shifted, the hover/selected item background shifts with it. This option moves the background back into place.

## Notes

This version intentionally doesn't use `TVM_SETINDENT` as the main shift mechanism. `TVM_SETINDENT` changes the distance between tree hierarchy levels, which is why the original behavior depended on whether root/tree glyphs were visible.

The live-layout correction is done after Explorer positions the tree, without hooking `SetWindowPos`, `MoveWindow`, or `DeferWindowPos`. This keeps Explorer's own scrollbar calculation intact.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- TargetIndent: 25
  $name: Navigation tree left shift
  $description: Moves the whole Explorer navigation tree. 0 keeps Explorer's original position. Positive values move the tree left.

- RemoveHasButtons: true
  $name: Remove all expand/collapse glyphs
  $description: Removes the tree expand/collapse glyphs in the navigation pane

- RemoveLinesAtRoot: true
  $name: Remove first expand glyph
  $description: Removes the first/root expand/collapse glyph while keeping child glyphs. Works when "Remove all expand/collapse glyphs" is disabled.

- RemoveHasLines: true
  $name: Remove connector lines
  $description: Removes the tree connector lines in the navigation pane

- CompensateItemBackgroundPosition: true
  $name: Compensate item background position
  $description: When the tree is shifted, the hover/selected item background shifts with it. This option moves the background back into place.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <windhawk_utils.h>
#include <vector>

struct ExplorerState
{
    HWND hExplorer;
    HWND hTree;
    bool repatchPending;
};

struct TreeState
{
    HWND hExplorer;
    HWND hTree;

    bool capturedStyle;
    LONG_PTR originalStyle;

    bool capturedLayout;
    RECT baseRect;
    RECT appliedRect;
    int appliedShift;
};

struct RepatchContext
{
    HWND hExplorer;
    DWORD delayMs;
};

static HANDLE g_hWorkerThread = nullptr;
static DWORD g_WorkerThreadId = 0;
static HWINEVENTHOOK g_hEventHookShow = nullptr;
static HWINEVENTHOOK g_hEventHookCreate = nullptr;
static HWINEVENTHOOK g_hEventHookMoveSizeEnd = nullptr;

static int g_TargetIndent = 25;
static BOOL g_RemoveHasButtons = TRUE;
static BOOL g_RemoveHasLines = TRUE;
static BOOL g_RemoveLinesAtRoot = TRUE;
static BOOL g_CompensateItemBackgroundPosition = TRUE;

static CRITICAL_SECTION g_StateLock;
static bool g_StateLockInitialized = false;
static bool g_Uninitializing = false;
static std::vector<ExplorerState> g_ExplorerStates;
static std::vector<TreeState> g_TreeStates;

static void LoadSettings();
static bool IsExplorerTopWindow(HWND hwnd);
static HWND FindExplorerTopWindowFromChild(HWND hwnd);
static bool IsLikelyNavTree(HWND hwnd);
static bool IsTreeViewClassName(LPCWSTR className);
static void PatchExplorerWindow(HWND hExplorer);
static void PatchTree(HWND hExplorer, HWND hTree);
static void AdjustTreeWindowPosChanging(HWND hTree, WINDOWPOS* wp);
static void RemoveAllSubclasses();
static void WaitForRepatchThreadsToFinish();
static bool ShouldCompensateTreeItemBackground(int partId, LPCRECT pRect);

#define TREE_SUBCLASS_ID 0x4E505431u
#define EXPLORER_SUBCLASS_ID 0x4E505432u

static LONG g_InternalWindowPosDepth = 0;
static volatile LONG g_ActiveRepatchThreads = 0;
static HANDLE g_hNoRepatchThreadsEvent = nullptr;

static HWND (WINAPI* CreateWindowExW_Original)(
    DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int,
    HWND, HMENU, HINSTANCE, LPVOID) = nullptr;

static HRESULT (WINAPI* DrawThemeBackground_Original)(
    HTHEME, HDC, int, int, LPCRECT, LPCRECT) = nullptr;

static HRESULT (WINAPI* DrawThemeBackgroundEx_Original)(
    HTHEME, HDC, int, int, LPCRECT, const DTBGOPTS*) = nullptr;

static thread_local HWND g_CurrentPaintingTree = nullptr;
static thread_local int g_CurrentPaintingTreeDepth = 0;

static int AbsInt(int value)
{
    return value < 0 ? -value : value;
}

static bool AlmostEqualInt(int a, int b)
{
    return AbsInt(a - b) <= 1;
}

static bool RectAlmostEquals(const RECT& a, const RECT& b)
{
    return AlmostEqualInt(a.left, b.left) &&
           AlmostEqualInt(a.top, b.top) &&
           AlmostEqualInt(a.right, b.right) &&
           AlmostEqualInt(a.bottom, b.bottom);
}

static int RectWidth(const RECT& rc)
{
    return rc.right - rc.left;
}

static int RectHeight(const RECT& rc)
{
    return rc.bottom - rc.top;
}

static bool GetWindowRectInParent(HWND hwnd, RECT* rc)
{
    if (!IsWindow(hwnd) || !rc)
        return false;

    HWND hParent = GetParent(hwnd);
    if (!hParent)
        return false;

    RECT windowRect{};
    if (!GetWindowRect(hwnd, &windowRect))
        return false;

    POINT points[2] = {
        {windowRect.left, windowRect.top},
        {windowRect.right, windowRect.bottom},
    };

    MapWindowPoints(HWND_DESKTOP, hParent, points, 2);

    rc->left = points[0].x;
    rc->top = points[0].y;
    rc->right = points[1].x;
    rc->bottom = points[1].y;
    return true;
}

static void LoadSettings()
{
    // Keep the original key for compatibility with existing installs.
    // The value now shifts the whole tree control instead of changing TVM_SETINDENT.
    g_TargetIndent = Wh_GetIntSetting(L"TargetIndent");
    g_RemoveHasButtons = Wh_GetIntSetting(L"RemoveHasButtons");
    g_RemoveHasLines = Wh_GetIntSetting(L"RemoveHasLines");
    g_RemoveLinesAtRoot = Wh_GetIntSetting(L"RemoveLinesAtRoot");
    g_CompensateItemBackgroundPosition = Wh_GetIntSetting(L"CompensateItemBackgroundPosition");

    Wh_Log(L"Settings loaded, treeLeftShift=%d, compensateItemBackground=%d", g_TargetIndent, g_CompensateItemBackgroundPosition);
}

static bool IsExplorerTopWindow(HWND hwnd)
{
    if (!IsWindow(hwnd))
        return false;

    wchar_t cls[128] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));

    return lstrcmpiW(cls, L"CabinetWClass") == 0 ||
           lstrcmpiW(cls, L"ExploreWClass") == 0;
}

static HWND FindExplorerTopWindowFromChild(HWND hwnd)
{
    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (root && IsExplorerTopWindow(root))
        return root;

    return nullptr;
}

static bool IsTreeViewClassName(LPCWSTR className)
{
    if (!className || IS_INTRESOURCE(className))
        return false;

    return lstrcmpiW(className, WC_TREEVIEWW) == 0 ||
           lstrcmpiW(className, L"SysTreeView32") == 0;
}

static bool IsTreeViewClass(HWND hwnd)
{
    if (!IsWindow(hwnd))
        return false;

    wchar_t cls[128] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));

    return IsTreeViewClassName(cls);
}

static bool IsLikelyNavTree(HWND hwnd)
{
    if (!IsTreeViewClass(hwnd))
        return false;

    if (!FindExplorerTopWindowFromChild(hwnd))
        return false;

    RECT rc{};
    if (!GetWindowRect(hwnd, &rc))
        return false;

    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;

    if (w < 80 || h < 80)
        return false;

    return true;
}

static ExplorerState* FindExplorerState_NoLock(HWND hExplorer)
{
    for (auto& state : g_ExplorerStates)
    {
        if (state.hExplorer == hExplorer)
            return &state;
    }

    return nullptr;
}

static ExplorerState* GetOrCreateExplorerState_NoLock(HWND hExplorer)
{
    ExplorerState* existing = FindExplorerState_NoLock(hExplorer);
    if (existing)
        return existing;

    ExplorerState state{};
    state.hExplorer = hExplorer;
    state.hTree = nullptr;
    state.repatchPending = false;
    g_ExplorerStates.push_back(state);
    return &g_ExplorerStates.back();
}

static TreeState* FindTreeState_NoLock(HWND hTree)
{
    for (auto& state : g_TreeStates)
    {
        if (state.hTree == hTree)
            return &state;
    }

    return nullptr;
}

static TreeState* GetOrCreateTreeState_NoLock(HWND hExplorer, HWND hTree)
{
    TreeState* existing = FindTreeState_NoLock(hTree);
    if (existing)
    {
        existing->hExplorer = hExplorer;
        return existing;
    }

    TreeState state{};
    state.hExplorer = hExplorer;
    state.hTree = hTree;
    state.capturedStyle = false;
    state.originalStyle = 0;
    state.capturedLayout = false;
    state.baseRect = {};
    state.appliedRect = {};
    state.appliedShift = 0;
    g_TreeStates.push_back(state);
    return &g_TreeStates.back();
}

static void CleanupDeadStates()
{
    if (!g_StateLockInitialized)
        return;

    EnterCriticalSection(&g_StateLock);

    for (size_t i = 0; i < g_ExplorerStates.size();)
    {
        if (!IsWindow(g_ExplorerStates[i].hExplorer))
            g_ExplorerStates.erase(g_ExplorerStates.begin() + i);
        else
            ++i;
    }

    for (size_t i = 0; i < g_TreeStates.size();)
    {
        if (!IsWindow(g_TreeStates[i].hTree))
            g_TreeStates.erase(g_TreeStates.begin() + i);
        else
            ++i;
    }

    LeaveCriticalSection(&g_StateLock);
}

class ScopedInternalWindowPos
{
public:
    ScopedInternalWindowPos()
    {
        InterlockedIncrement(&g_InternalWindowPosDepth);
    }

    ~ScopedInternalWindowPos()
    {
        InterlockedDecrement(&g_InternalWindowPosDepth);
    }
};


class ScopedTreePaintContext
{
public:
    explicit ScopedTreePaintContext(HWND hTree)
    {
        m_previousTree = g_CurrentPaintingTree;
        g_CurrentPaintingTree = hTree;
        ++g_CurrentPaintingTreeDepth;
    }

    ~ScopedTreePaintContext()
    {
        if (g_CurrentPaintingTreeDepth > 0)
            --g_CurrentPaintingTreeDepth;

        g_CurrentPaintingTree = m_previousTree;

        if (g_CurrentPaintingTreeDepth <= 0)
        {
            g_CurrentPaintingTreeDepth = 0;
            g_CurrentPaintingTree = nullptr;
        }
    }

private:
    HWND m_previousTree = nullptr;
};

#ifndef TVP_TREEITEM
#define TVP_TREEITEM 1
#endif

static bool ShouldCompensateTreeItemBackground(int partId, LPCRECT pRect)
{
    if (!g_CompensateItemBackgroundPosition || g_Uninitializing || g_TargetIndent == 0)
        return false;

    if (g_CurrentPaintingTreeDepth <= 0 || !IsWindow(g_CurrentPaintingTree))
        return false;

    if (partId != TVP_TREEITEM)
        return false;

    if (!pRect)
        return false;

    int rectWidth = RectWidth(*pRect);
    int rectHeight = RectHeight(*pRect);
    if (rectWidth <= 0 || rectHeight <= 0)
        return false;

    if (!IsLikelyNavTree(g_CurrentPaintingTree))
        return false;

    if (rectHeight < 8 || rectHeight > 80)
        return false;

    // DrawThemeBackground is also used by the themed scrollbar hosted by the
    // TreeView. Scrollbar arrow buttons can use the same numeric part id as
    // TVP_TREEITEM. The previous protection filtered out all narrow rectangles,
    // but short tree items such as "Videos" also have narrow background rects.
    // Exclude only rectangles that are actually inside the vertical scrollbar
    // strip at the right edge of the TreeView.
    RECT clientRect{};
    if (GetClientRect(g_CurrentPaintingTree, &clientRect))
    {
        int clientWidth = RectWidth(clientRect);
        int clientHeight = RectHeight(clientRect);
        int scrollbarWidth = GetSystemMetrics(SM_CXVSCROLL);
        int scrollbarHeight = GetSystemMetrics(SM_CYVSCROLL);

        int scrollbarLeft = clientWidth - scrollbarWidth - 4;
        if (scrollbarLeft < 0)
            scrollbarLeft = 0;

        bool inVerticalScrollbarStrip =
            pRect->left >= scrollbarLeft &&
            pRect->right <= clientWidth + 4;

        bool looksLikeScrollbarButton =
            rectWidth <= scrollbarWidth + 8 &&
            rectHeight <= scrollbarHeight + 8 &&
            (pRect->top <= 4 || pRect->bottom >= clientHeight - 4);

        if (inVerticalScrollbarStrip && looksLikeScrollbarButton)
            return false;
    }

    int maxExpectedLeft = g_TargetIndent + 16;
    if (maxExpectedLeft < 32)
        maxExpectedLeft = 32;

    if (pRect->left > maxExpectedLeft)
        return false;

    return true;
}

static HRESULT WINAPI DrawThemeBackground_Hook(
    HTHEME hTheme,
    HDC hdc,
    int iPartId,
    int iStateId,
    LPCRECT pRect,
    LPCRECT pClipRect)
{
    if (ShouldCompensateTreeItemBackground(iPartId, pRect))
    {
        RECT adjustedRect = *pRect;

        // The shifted tree is wider by g_TargetIndent pixels: its left edge moves
        // left, while the right edge stays aligned with Explorer's layout. Moving
        // the whole background rect to the right fixes the left edge, but pushes
        // the right edge too far. Only trim the left side instead.
        adjustedRect.left += g_TargetIndent;
        if (adjustedRect.left > adjustedRect.right)
            adjustedRect.left = adjustedRect.right;

        RECT adjustedClipRect{};
        LPCRECT finalClipRect = pClipRect;
        if (pClipRect)
        {
            adjustedClipRect = *pClipRect;
            adjustedClipRect.left += g_TargetIndent;
            if (adjustedClipRect.left > adjustedClipRect.right)
                adjustedClipRect.left = adjustedClipRect.right;
            finalClipRect = &adjustedClipRect;
        }

        return DrawThemeBackground_Original(
            hTheme,
            hdc,
            iPartId,
            iStateId,
            &adjustedRect,
            finalClipRect);
    }

    return DrawThemeBackground_Original(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}

static HRESULT WINAPI DrawThemeBackgroundEx_Hook(
    HTHEME hTheme,
    HDC hdc,
    int iPartId,
    int iStateId,
    LPCRECT pRect,
    const DTBGOPTS* pOptions)
{
    if (ShouldCompensateTreeItemBackground(iPartId, pRect))
    {
        RECT adjustedRect = *pRect;

        // Keep the right edge unchanged and compensate only the left edge.
        // This prevents the background from stretching toward the content area.
        adjustedRect.left += g_TargetIndent;
        if (adjustedRect.left > adjustedRect.right)
            adjustedRect.left = adjustedRect.right;

        DTBGOPTS adjustedOptions{};
        const DTBGOPTS* finalOptions = pOptions;
        if (pOptions)
        {
            adjustedOptions = *pOptions;
            if (adjustedOptions.dwFlags & DTBG_CLIPRECT)
            {
                adjustedOptions.rcClip.left += g_TargetIndent;
                if (adjustedOptions.rcClip.left > adjustedOptions.rcClip.right)
                    adjustedOptions.rcClip.left = adjustedOptions.rcClip.right;
            }
            finalOptions = &adjustedOptions;
        }

        return DrawThemeBackgroundEx_Original(
            hTheme,
            hdc,
            iPartId,
            iStateId,
            &adjustedRect,
            finalOptions);
    }

    return DrawThemeBackgroundEx_Original(hTheme, hdc, iPartId, iStateId, pRect, pOptions);
}


static void AdjustTreeWindowPosChanging(HWND hTree, WINDOWPOS* wp)
{
    if (!wp || g_Uninitializing || g_InternalWindowPosDepth > 0)
        return;

    if ((wp->flags & SWP_NOMOVE) && (wp->flags & SWP_NOSIZE))
        return;

    HWND hExplorer = FindExplorerTopWindowFromChild(hTree);
    if (!hExplorer)
        return;

    RECT currentRect{};
    if (!GetWindowRectInParent(hTree, &currentRect))
        return;

    int currentWidth = RectWidth(currentRect);
    int currentHeight = RectHeight(currentRect);

    if (currentWidth < 50 || currentHeight < 50)
        return;

    int shift = g_TargetIndent;
    if (shift == 0)
        return;

    int proposedX = (wp->flags & SWP_NOMOVE) ? currentRect.left : wp->x;
    int proposedY = (wp->flags & SWP_NOMOVE) ? currentRect.top : wp->y;
    int proposedCx = (wp->flags & SWP_NOSIZE) ? currentWidth : wp->cx;
    int proposedCy = (wp->flags & SWP_NOSIZE) ? currentHeight : wp->cy;

    if (proposedCx < 50 || proposedCy < 50)
        return;

    bool alreadyShifted = false;

    if (g_StateLockInitialized)
    {
        EnterCriticalSection(&g_StateLock);

        TreeState* state = FindTreeState_NoLock(hTree);
        if (state && state->capturedLayout)
        {
            // Important: only the coordinates proposed by Explorer matter here.
            // In reset-v4 this also checked currentRect.left. During live resize the
            // current rect is often still our shifted rect, while Explorer proposes
            // the unshifted/default x. Treating that as already shifted caused the
            // tree to temporarily jump back to the default position.
            alreadyShifted =
                AlmostEqualInt(proposedX, state->appliedRect.left) ||
                AlmostEqualInt(proposedX, state->baseRect.left - state->appliedShift);
        }

        LeaveCriticalSection(&g_StateLock);
    }

    RECT baseRect{};
    RECT targetRect{};

    if (alreadyShifted)
    {
        targetRect.left = proposedX;
        targetRect.top = proposedY;
        targetRect.right = proposedX + proposedCx;
        targetRect.bottom = proposedY + proposedCy;

        baseRect = targetRect;
        baseRect.left += shift;
    }
    else
    {
        baseRect.left = proposedX;
        baseRect.top = proposedY;
        baseRect.right = proposedX + proposedCx;
        baseRect.bottom = proposedY + proposedCy;

        targetRect = baseRect;
        targetRect.left = baseRect.left - shift;
    }

    int targetWidth = RectWidth(targetRect);
    int targetHeight = RectHeight(targetRect);

    if (targetWidth < 50)
        targetWidth = 50;

    if (targetHeight < 50)
        targetHeight = 50;

    targetRect.right = targetRect.left + targetWidth;
    targetRect.bottom = targetRect.top + targetHeight;

    if (g_StateLockInitialized)
    {
        EnterCriticalSection(&g_StateLock);

        TreeState* state = GetOrCreateTreeState_NoLock(hExplorer, hTree);
        if (state)
        {
            state->capturedLayout = true;
            state->baseRect = baseRect;
            state->appliedRect = targetRect;
            state->appliedShift = shift;
        }

        LeaveCriticalSection(&g_StateLock);
    }

    wp->x = targetRect.left;
    wp->y = targetRect.top;
    wp->cx = targetWidth;
    wp->cy = targetHeight;
    wp->flags &= ~(SWP_NOMOVE | SWP_NOSIZE);
}

static LRESULT CALLBACK TreeSubclassProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR)
{
    if (msg == WM_NCDESTROY)
    {
        LRESULT result = DefSubclassProc(hwnd, msg, wParam, lParam);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, TreeSubclassProc);
        return result;
    }

    if (msg == WM_WINDOWPOSCHANGING && !g_Uninitializing && g_InternalWindowPosDepth <= 0)
    {
        AdjustTreeWindowPosChanging(hwnd, (WINDOWPOS*)lParam);
    }

    LRESULT result = 0;
    if (msg == WM_PAINT || msg == WM_PRINTCLIENT)
    {
        ScopedTreePaintContext paintContext(hwnd);
        result = DefSubclassProc(hwnd, msg, wParam, lParam);
    }
    else
    {
        result = DefSubclassProc(hwnd, msg, wParam, lParam);
    }

    if (g_Uninitializing || g_InternalWindowPosDepth > 0)
        return result;

    if (msg == WM_WINDOWPOSCHANGED ||
        msg == WM_SHOWWINDOW)
    {
        HWND hExplorer = FindExplorerTopWindowFromChild(hwnd);
        if (hExplorer)
            PatchTree(hExplorer, hwnd);
    }

    return result;
}


static LRESULT CALLBACK ExplorerSubclassProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR)
{
    LRESULT result = DefSubclassProc(hwnd, msg, wParam, lParam);

    if (msg == WM_NCDESTROY)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, ExplorerSubclassProc);
        return result;
    }

    if (g_Uninitializing || g_InternalWindowPosDepth > 0)
        return result;

    if (msg == WM_WINDOWPOSCHANGED ||
        msg == WM_SIZE ||
        msg == WM_EXITSIZEMOVE)
    {
        PatchExplorerWindow(hwnd);
    }

    return result;
}

static void TrySubclassTree(HWND hTree)
{
    if (!IsWindow(hTree))
        return;

    WindhawkUtils::SetWindowSubclassFromAnyThread(hTree, TreeSubclassProc, 0);
}

static void TrySubclassExplorer(HWND hExplorer)
{
    if (!IsWindow(hExplorer))
        return;

    WindhawkUtils::SetWindowSubclassFromAnyThread(hExplorer, ExplorerSubclassProc, 0);
}

static void CaptureAndApplyTreeStyle(HWND hExplorer, HWND hTree)
{
    if (!IsWindow(hTree) || !g_StateLockInitialized)
        return;

    LONG_PTR style = GetWindowLongPtrW(hTree, GWL_STYLE);
    LONG_PTR originalStyle = style;

    EnterCriticalSection(&g_StateLock);
    TreeState* state = GetOrCreateTreeState_NoLock(hExplorer, hTree);
    if (state)
    {
        if (!state->capturedStyle)
        {
            state->capturedStyle = true;
            state->originalStyle = style;
        }

        originalStyle = state->originalStyle;
    }
    LeaveCriticalSection(&g_StateLock);

    LONG_PTR newStyle = style;

    if (g_RemoveHasButtons)
        newStyle &= ~TVS_HASBUTTONS;
    else
        newStyle = (newStyle & ~TVS_HASBUTTONS) | (originalStyle & TVS_HASBUTTONS);

    if (g_RemoveHasLines)
        newStyle &= ~TVS_HASLINES;
    else
        newStyle = (newStyle & ~TVS_HASLINES) | (originalStyle & TVS_HASLINES);

    if (g_RemoveLinesAtRoot)
        newStyle &= ~TVS_LINESATROOT;
    else
        newStyle = (newStyle & ~TVS_LINESATROOT) | (originalStyle & TVS_LINESATROOT);

    if (newStyle != style)
    {
        SetWindowLongPtrW(hTree, GWL_STYLE, newStyle);

        ScopedInternalWindowPos internalMove;

        SetWindowPos(
            hTree,
            nullptr,
            0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
}

static void ApplyTreeLeftShift(HWND hExplorer, HWND hTree)
{
    if (!IsWindow(hExplorer) || !IsWindow(hTree) || !g_StateLockInitialized)
        return;

    RECT currentRect{};
    if (!GetWindowRectInParent(hTree, &currentRect))
        return;

    if (RectWidth(currentRect) < 50 || RectHeight(currentRect) < 50)
        return;

    int shift = g_TargetIndent;
    RECT baseRect = currentRect;
    bool hadCapturedLayout = false;
    int previousShift = 0;

    EnterCriticalSection(&g_StateLock);

    TreeState* state = GetOrCreateTreeState_NoLock(hExplorer, hTree);
    if (state)
    {
        hadCapturedLayout = state->capturedLayout;
        previousShift = state->appliedShift;

        if (state->capturedLayout && previousShift != 0)
        {
            bool currentLooksShifted =
                AlmostEqualInt(currentRect.left, state->appliedRect.left) ||
                AlmostEqualInt(currentRect.left, state->baseRect.left - previousShift);

            if (currentLooksShifted)
            {
                // The tree is already in our shifted coordinate space, even if Explorer
                // changed only height/width during resize. Convert it back to base space
                // before calculating the new target. This prevents cumulative shifting.
                baseRect.left = currentRect.left + previousShift;
                baseRect.top = currentRect.top;
                baseRect.right = currentRect.right;
                baseRect.bottom = currentRect.bottom;
            }
        }
    }

    LeaveCriticalSection(&g_StateLock);

    if (!hadCapturedLayout && shift > 0 && currentRect.left < 0)
    {
        // Defensive recovery after reloading from an older shifted build.
        baseRect.left = currentRect.left + shift;
        baseRect.top = currentRect.top;
        baseRect.right = currentRect.right;
        baseRect.bottom = currentRect.bottom;
    }
    else if (!hadCapturedLayout || RectAlmostEquals(baseRect, currentRect))
    {
        // Explorer's latest unshifted layout becomes the base.
        baseRect = currentRect;
    }

    RECT targetRect = baseRect;
    targetRect.left = baseRect.left - shift;

    int targetWidth = targetRect.right - targetRect.left;
    int targetHeight = targetRect.bottom - targetRect.top;

    if (targetWidth < 50)
        targetWidth = 50;

    if (targetHeight < 50)
        targetHeight = 50;

    targetRect.right = targetRect.left + targetWidth;
    targetRect.bottom = targetRect.top + targetHeight;

    EnterCriticalSection(&g_StateLock);
    state = GetOrCreateTreeState_NoLock(hExplorer, hTree);
    if (state)
    {
        state->capturedLayout = true;
        state->baseRect = baseRect;
        state->appliedRect = targetRect;
        state->appliedShift = shift;
    }
    LeaveCriticalSection(&g_StateLock);

    if (RectAlmostEquals(currentRect, targetRect))
        return;

    ScopedInternalWindowPos internalMove;

    SetWindowPos(
        hTree,
        nullptr,
        targetRect.left,
        targetRect.top,
        targetWidth,
        targetHeight,
        SWP_NOZORDER | SWP_NOACTIVATE);
}

static void PatchTree(HWND hExplorer, HWND hTree)
{
    if (!IsWindow(hExplorer) || !IsWindow(hTree))
        return;

    CaptureAndApplyTreeStyle(hExplorer, hTree);
    ApplyTreeLeftShift(hExplorer, hTree);

}

static BOOL CALLBACK FindTreeEnumProc(HWND hwnd, LPARAM lParam)
{
    HWND* pFound = (HWND*)lParam;
    if (*pFound)
        return FALSE;

    if (IsLikelyNavTree(hwnd))
    {
        *pFound = hwnd;
        return FALSE;
    }

    return TRUE;
}

static HWND FindNavTreeInExplorer(HWND hExplorer)
{
    if (!IsExplorerTopWindow(hExplorer))
        return nullptr;

    HWND found = nullptr;
    EnumChildWindows(hExplorer, FindTreeEnumProc, (LPARAM)&found);
    return found;
}

static void PatchExplorerWindow(HWND hExplorer)
{
    if (!IsExplorerTopWindow(hExplorer))
        return;

    HWND hTree = FindNavTreeInExplorer(hExplorer);
    if (!hTree)
        return;

    TrySubclassExplorer(hExplorer);
    TrySubclassTree(hTree);

    if (g_StateLockInitialized)
    {
        EnterCriticalSection(&g_StateLock);
        ExplorerState* state = GetOrCreateExplorerState_NoLock(hExplorer);
        if (state)
            state->hTree = hTree;
        LeaveCriticalSection(&g_StateLock);
    }

    PatchTree(hExplorer, hTree);
}


static void FinishRepatchThread()
{
    LONG remaining = InterlockedDecrement(&g_ActiveRepatchThreads);
    if (remaining <= 0)
    {
        InterlockedExchange(&g_ActiveRepatchThreads, 0);
        if (g_hNoRepatchThreadsEvent)
            SetEvent(g_hNoRepatchThreadsEvent);
    }
}

static void WaitForRepatchThreadsToFinish()
{
    if (!g_hNoRepatchThreadsEvent)
        return;

    WaitForSingleObject(g_hNoRepatchThreadsEvent, 3000);
}

static void RemoveAllSubclasses()
{
    if (!g_StateLockInitialized)
        return;

    std::vector<HWND> trees;
    std::vector<HWND> explorers;

    EnterCriticalSection(&g_StateLock);

    for (const auto& state : g_TreeStates)
    {
        if (state.hTree)
            trees.push_back(state.hTree);
    }

    for (const auto& state : g_ExplorerStates)
    {
        if (state.hExplorer)
            explorers.push_back(state.hExplorer);
    }

    LeaveCriticalSection(&g_StateLock);

    for (HWND hTree : trees)
    {
        if (IsWindow(hTree))
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hTree, TreeSubclassProc);
    }

    for (HWND hExplorer : explorers)
    {
        if (IsWindow(hExplorer))
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hExplorer, ExplorerSubclassProc);
    }
}

static DWORD WINAPI RepatchThreadProc(LPVOID param)
{
    RepatchContext* ctx = (RepatchContext*)param;
    if (!ctx)
    {
        FinishRepatchThread();
        return 0;
    }

    HWND hExplorer = ctx->hExplorer;
    DWORD delayMs = ctx->delayMs;
    delete ctx;

    Sleep(delayMs);

    if (!g_Uninitializing && IsWindow(hExplorer))
        PatchExplorerWindow(hExplorer);

    if (g_StateLockInitialized)
    {
        EnterCriticalSection(&g_StateLock);
        ExplorerState* state = FindExplorerState_NoLock(hExplorer);
        if (state)
            state->repatchPending = false;
        LeaveCriticalSection(&g_StateLock);
    }

    FinishRepatchThread();
    return 0;
}

static void ScheduleOneRepatch(HWND hExplorer, DWORD delayMs)
{
    if (!IsWindow(hExplorer) || !g_StateLockInitialized || g_Uninitializing)
        return;

    bool shouldStart = false;

    EnterCriticalSection(&g_StateLock);
    ExplorerState* state = GetOrCreateExplorerState_NoLock(hExplorer);
    if (state && !state->repatchPending)
    {
        state->repatchPending = true;
        shouldStart = true;
    }
    LeaveCriticalSection(&g_StateLock);

    if (!shouldStart)
        return;

    RepatchContext* ctx = new RepatchContext{};
    ctx->hExplorer = hExplorer;
    ctx->delayMs = delayMs;

    if (g_hNoRepatchThreadsEvent)
        ResetEvent(g_hNoRepatchThreadsEvent);
    InterlockedIncrement(&g_ActiveRepatchThreads);

    HANDLE hThread = CreateThread(nullptr, 0, RepatchThreadProc, ctx, 0, nullptr);
    if (hThread)
    {
        CloseHandle(hThread);
    }
    else
    {
        FinishRepatchThread();
        delete ctx;

        EnterCriticalSection(&g_StateLock);
        state = FindExplorerState_NoLock(hExplorer);
        if (state)
            state->repatchPending = false;
        LeaveCriticalSection(&g_StateLock);
    }
}

static BOOL CALLBACK EnumTopWindowsProc(HWND hwnd, LPARAM)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);

    if (pid == GetCurrentProcessId() && IsExplorerTopWindow(hwnd))
        PatchExplorerWindow(hwnd);

    return TRUE;
}

static void InitialPatchAllExplorerWindows()
{
    EnumWindows(EnumTopWindowsProc, 0);
}

static void CALLBACK WinEventProc(
    HWINEVENTHOOK,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG,
    DWORD,
    DWORD)
{
    if (!hwnd || g_Uninitializing)
        return;

    if (idObject != OBJID_WINDOW)
        return;

    CleanupDeadStates();

    if (event == EVENT_SYSTEM_MOVESIZEEND)
    {
        HWND hExplorer = IsExplorerTopWindow(hwnd) ? hwnd : FindExplorerTopWindowFromChild(hwnd);
        if (hExplorer)
            ScheduleOneRepatch(hExplorer, 30);
        return;
    }

    if (IsExplorerTopWindow(hwnd))
    {
        PatchExplorerWindow(hwnd);
        ScheduleOneRepatch(hwnd, 120);
        return;
    }

    if (IsLikelyNavTree(hwnd))
    {
        HWND hExplorer = FindExplorerTopWindowFromChild(hwnd);
        if (hExplorer)
        {
            PatchTree(hExplorer, hwnd);
            ScheduleOneRepatch(hExplorer, 120);
        }
        return;
    }

    HWND hExplorer = FindExplorerTopWindowFromChild(hwnd);
    if (hExplorer)
        ScheduleOneRepatch(hExplorer, 120);
}

static HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam)
{
    HWND hwnd = CreateWindowExW_Original(
        dwExStyle,
        lpClassName,
        lpWindowName,
        dwStyle,
        X,
        Y,
        nWidth,
        nHeight,
        hWndParent,
        hMenu,
        hInstance,
        lpParam);

    if (g_Uninitializing || !hwnd)
        return hwnd;

    if (IsExplorerTopWindow(hwnd))
    {
        TrySubclassExplorer(hwnd);
        PatchExplorerWindow(hwnd);
    }
    else if (IsTreeViewClassName(lpClassName) || IsTreeViewClass(hwnd))
    {
        HWND hExplorer = FindExplorerTopWindowFromChild(hwnd);
        if (hExplorer)
        {
            TrySubclassTree(hwnd);
            PatchTree(hExplorer, hwnd);
        }
    }

    return hwnd;
}

static void RestorePatchedTrees()
{
    if (!g_StateLockInitialized)
        return;

    struct RestoreInfo
    {
        HWND hTree;
        bool restoreStyle;
        LONG_PTR originalStyle;
        bool restoreLayout;
        RECT baseRect;
    };

    std::vector<RestoreInfo> restoreInfos;

    EnterCriticalSection(&g_StateLock);
    for (const auto& state : g_TreeStates)
    {
        RestoreInfo info{};
        info.hTree = state.hTree;
        info.restoreStyle = state.capturedStyle;
        info.originalStyle = state.originalStyle;
        info.restoreLayout = state.capturedLayout;
        info.baseRect = state.baseRect;
        restoreInfos.push_back(info);
    }
    LeaveCriticalSection(&g_StateLock);

    for (const auto& info : restoreInfos)
    {
        if (!IsWindow(info.hTree))
            continue;

        ScopedInternalWindowPos internalMove;

        if (info.restoreStyle)
        {
            SetWindowLongPtrW(info.hTree, GWL_STYLE, info.originalStyle);
            SetWindowPos(
                info.hTree,
                nullptr,
                0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        }

        if (info.restoreLayout && RectWidth(info.baseRect) >= 50 && RectHeight(info.baseRect) >= 50)
        {
            SetWindowPos(
                info.hTree,
                nullptr,
                info.baseRect.left,
                info.baseRect.top,
                RectWidth(info.baseRect),
                RectHeight(info.baseRect),
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }
}

static DWORD WINAPI WorkerThreadProc(LPVOID)
{
    InitializeCriticalSection(&g_StateLock);
    g_StateLockInitialized = true;

    Wh_Log(L"Worker started");

    InitialPatchAllExplorerWindows();

    DWORD pid = GetCurrentProcessId();

    g_hEventHookShow = SetWinEventHook(
        EVENT_OBJECT_SHOW,
        EVENT_OBJECT_SHOW,
        nullptr,
        WinEventProc,
        pid,
        0,
        WINEVENT_OUTOFCONTEXT);

    g_hEventHookCreate = SetWinEventHook(
        EVENT_OBJECT_CREATE,
        EVENT_OBJECT_CREATE,
        nullptr,
        WinEventProc,
        pid,
        0,
        WINEVENT_OUTOFCONTEXT);

    g_hEventHookMoveSizeEnd = SetWinEventHook(
        EVENT_SYSTEM_MOVESIZEEND,
        EVENT_SYSTEM_MOVESIZEEND,
        nullptr,
        WinEventProc,
        pid,
        0,
        WINEVENT_OUTOFCONTEXT);

    if (!g_hEventHookShow || !g_hEventHookCreate || !g_hEventHookMoveSizeEnd)
        Wh_Log(L"Failed to install one or more WinEvent hooks");
    else
        Wh_Log(L"WinEvent hooks installed");

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hEventHookShow)
    {
        UnhookWinEvent(g_hEventHookShow);
        g_hEventHookShow = nullptr;
    }

    if (g_hEventHookCreate)
    {
        UnhookWinEvent(g_hEventHookCreate);
        g_hEventHookCreate = nullptr;
    }

    if (g_hEventHookMoveSizeEnd)
    {
        UnhookWinEvent(g_hEventHookMoveSizeEnd);
        g_hEventHookMoveSizeEnd = nullptr;
    }

    Wh_Log(L"Worker stopped");
    return 0;
}

BOOL Wh_ModInit()
{
    g_Uninitializing = false;
    InterlockedExchange(&g_ActiveRepatchThreads, 0);

    g_hNoRepatchThreadsEvent = CreateEventW(nullptr, TRUE, TRUE, nullptr);
    if (!g_hNoRepatchThreadsEvent)
        Wh_Log(L"Failed to create repatch thread event");

    LoadSettings();

    if (!Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original))
        Wh_Log(L"Failed to hook CreateWindowExW");

    if (!Wh_SetFunctionHook((void*)DrawThemeBackground, (void*)DrawThemeBackground_Hook, (void**)&DrawThemeBackground_Original))
        Wh_Log(L"Failed to hook DrawThemeBackground");

    if (!Wh_SetFunctionHook((void*)DrawThemeBackgroundEx, (void*)DrawThemeBackgroundEx_Hook, (void**)&DrawThemeBackgroundEx_Original))
        Wh_Log(L"Failed to hook DrawThemeBackgroundEx");

    g_hWorkerThread = CreateThread(
        nullptr,
        0,
        WorkerThreadProc,
        nullptr,
        0,
        &g_WorkerThreadId);

    if (!g_hWorkerThread)
    {
        Wh_Log(L"Failed to create worker thread");
        return FALSE;
    }

    Wh_Log(L"Mod initialized");
    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"Uninitializing");

    g_Uninitializing = true;

    // Stop WinEvent callbacks first. Otherwise Explorer can create/resize/show
    // windows while we're restoring geometry and unloading the DLL.
    if (g_WorkerThreadId)
        PostThreadMessageW(g_WorkerThreadId, WM_QUIT, 0, 0);

    if (g_hWorkerThread)
    {
        WaitForSingleObject(g_hWorkerThread, 3000);
        CloseHandle(g_hWorkerThread);
        g_hWorkerThread = nullptr;
    }

    // Detached one-shot repatch threads could wake up after Windhawk had already
    // unloaded the module. Wait for them before returning from Wh_ModUninit.
    WaitForRepatchThreadsToFinish();

    // Remove subclass procedures before the DLL is unloaded. Leaving a subclass
    // installed with a callback address inside this module can crash Explorer on
    // the next message and make it look like Explorer restarted on mod disable.
    RemoveAllSubclasses();

    RestorePatchedTrees();

    if (g_StateLockInitialized)
    {
        DeleteCriticalSection(&g_StateLock);
        g_StateLockInitialized = false;
    }

    if (g_hNoRepatchThreadsEvent)
    {
        CloseHandle(g_hNoRepatchThreadsEvent);
        g_hNoRepatchThreadsEvent = nullptr;
    }

    g_WorkerThreadId = 0;
    g_ExplorerStates.clear();
    g_TreeStates.clear();
}

void Wh_ModSettingsChanged()
{
    LoadSettings();
    InitialPatchAllExplorerWindows();
}
