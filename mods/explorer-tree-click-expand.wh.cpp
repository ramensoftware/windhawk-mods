// ==WindhawkMod==
// @id              explorer-tree-click-expand
// @name            Expand folder on click (Explorer nav pane)
// @description     Left click expands a folder one level; Ctrl+click collapses the whole tree; Alt+click expands all subfolders
// @version         1.2
// @author          Didi
// @github          https://github.com/diegoalejo15
// @include         explorer.exe
// @compilerOptions -lcomctl32
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Expand folder on click

By default, in File Explorer's navigation pane (tree view), left-clicking
a folder only selects it; you have to click the small chevron/arrow to
expand it.

![Demo](https://i.imgur.com/9pmZye3.gif)

This mod adds four click behaviors to the navigation pane tree:

- **Left click** on a folder's icon or label: expands it one level (like
  pressing Numpad +). The native chevron behavior is untouched.
- **Ctrl + left click** on any folder: collapses the entire tree back to
  its original state (all folders collapsed).
- **Alt + left click** on a folder: expands it and all its subfolders
  recursively, down to the last level (like pressing the Numpad * key).
- **Shift + left click** on a folder: fully collapses just that folder,
  resetting the expanded state of its subfolders so that when you expand
  it again, its children come back collapsed instead of showing
  everything you had open before.

Ctrl+click, Alt+click, and Shift+click behaviors can each be individually
enabled or disabled in the mod's settings. All are enabled by default.

The mod works by subclassing the navigation pane's tree view control
directly (it's identified by its parent being a `NamespaceTreeControl`,
which is specific to Explorer's nav pane). Tree views shown inside other
programs' Open/Save file dialogs (Cubase, Excel, etc.), or any other tree
view elsewhere in Explorer, are never touched, since only the nav pane's
own tree view control is ever subclassed.

By default this mod only runs inside `explorer.exe`, so it only affects
the navigation pane of File Explorer windows. Many other programs' Open/
Save dialogs use the same navigation pane control, so if you'd like this
mod's click behaviors to also work there, you can add those programs'
executable names (e.g. `EXCEL.EXE`) to the mod's process inclusion list
from the "Advanced" tab of this mod's settings in Windhawk - no code
changes needed. The mod only ever touches the process it's running in, so
this is safe to do for as many or as few programs as you'd like.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ctrlClickCollapseAll: true
  $name: Ctrl+Click to collapse all
  $description: >-
    When enabled, Ctrl + left click on any folder in the navigation pane
    collapses the entire tree back to its original state.
- altClickExpandAll: true
  $name: Alt+Click to expand all subfolders
  $description: >-
    When enabled, Alt + left click on a folder in the navigation pane
    expands it and all its subfolders recursively (same as pressing the
    Numpad * key).
- shiftClickCollapseFolder: true
  $name: Shift+Click to collapse just that folder
  $description: >-
    When enabled, Shift + left click on a folder in the navigation pane
    fully collapses that single folder, resetting the expanded state of
    its subfolders so they come back collapsed the next time it's opened.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include <algorithm>
#include <mutex>
#include <vector>

bool g_settingCtrlClickCollapseAll = true;
bool g_settingAltClickExpandAll = true;
bool g_settingShiftClickCollapseFolder = true;

// The nav pane's tree view is a direct child of a NamespaceTreeControl
// window. This is specific to Explorer's own navigation pane, so checking
// for it lets us subclass exactly (and only) the tree we care about -
// nothing in other processes, and nothing else within Explorer itself.
constexpr WCHAR kNavPaneParentClassName[] = L"NamespaceTreeControl";
constexpr WCHAR kTreeViewClassName[] = L"SysTreeView32";

// Tracks every tree view we've subclassed so Wh_ModUninit can remove all
// subclasses before the mod DLL is unloaded (otherwise a leftover subclass
// pointing into unloaded code would crash Explorer on the next click).
std::mutex g_subclassedTreesMutex;
std::vector<HWND> g_subclassedTrees;

void CollapseAllRecursive(HWND hwndTree, HTREEITEM hItem) {
    while (hItem) {
        HTREEITEM hChild = TreeView_GetChild(hwndTree, hItem);
        if (hChild) {
            CollapseAllRecursive(hwndTree, hChild);
        }
        TreeView_Expand(hwndTree, hItem, TVE_COLLAPSE);
        hItem = TreeView_GetNextSibling(hwndTree, hItem);
    }
}

// Collapses everything below each top-level root item (e.g. "Home",
// "This PC", "Network" on the default layout, or just "Desktop" when
// "Show all folders" is enabled), keeping the roots themselves visible -
// matching the tree's original out-of-the-box state instead of hiding
// everything. The default nav pane layout has several sibling root items,
// so all of them need to be walked, not just the first one.
void CollapseAllKeepingRoot(HWND hwndTree) {
    for (HTREEITEM hRoot = TreeView_GetRoot(hwndTree); hRoot;
         hRoot = TreeView_GetNextSibling(hwndTree, hRoot)) {
        CollapseAllRecursive(hwndTree, TreeView_GetChild(hwndTree, hRoot));
    }
}

void ExpandAll(HWND hwndTree, HTREEITEM hItem) {
    // Select the item first - the native "*" (Numpad Multiply) behavior
    // expands the currently selected/focused item and all its descendants,
    // and it's implemented internally in a fast, non-blocking way (unlike
    // manually walking the tree with TVM_EXPAND calls). This all runs on
    // the tree's own thread now, so these SendMessage calls are same-
    // thread and safe.
    TreeView_SelectItem(hwndTree, hItem);
    SendMessage(hwndTree, WM_KEYDOWN, VK_MULTIPLY, 0);
    SendMessage(hwndTree, WM_KEYUP, VK_MULTIPLY, 0);
}

void LoadSettings() {
    g_settingCtrlClickCollapseAll = Wh_GetIntSetting(L"ctrlClickCollapseAll") != 0;
    g_settingAltClickExpandAll = Wh_GetIntSetting(L"altClickExpandAll") != 0;
    g_settingShiftClickCollapseFolder = Wh_GetIntSetting(L"shiftClickCollapseFolder") != 0;
}

void ForgetSubclassedTree(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_subclassedTreesMutex);
    auto it = std::find(g_subclassedTrees.begin(), g_subclassedTrees.end(), hWnd);
    if (it != g_subclassedTrees.end()) {
        g_subclassedTrees.erase(it);
    }
}

LRESULT CALLBACK TreeViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                       LPARAM lParam, UINT_PTR uIdSubclass) {
    if (uMsg == WM_LBUTTONDOWN) {
        POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

        TVHITTESTINFO hitTest{};
        hitTest.pt = pt;
        HTREEITEM hItem = TreeView_HitTest(hWnd, &hitTest);

        // Only act if the click landed on the icon or label, NOT on the
        // expand/collapse chevron (that one already toggles natively).
        bool onItemBody =
            (hitTest.flags & (TVHT_ONITEMICON | TVHT_ONITEMLABEL)) != 0;

        if (hItem && onItemBody) {
            bool ctrlDown = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            bool altDown = (GetKeyState(VK_MENU) & 0x8000) != 0;
            bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

            // Let the tree handle the click natively first (selection,
            // chevron toggle, etc.), then layer our behavior on top -
            // matching how a global hook that doesn't consume the message
            // would behave.
            LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

            if (ctrlDown && g_settingCtrlClickCollapseAll) {
                CollapseAllKeepingRoot(hWnd);
            } else if (altDown && g_settingAltClickExpandAll) {
                ExpandAll(hWnd, hItem);
            } else if (shiftDown && g_settingShiftClickCollapseFolder) {
                // TVE_COLLAPSERESET on top of TVE_COLLAPSE also resets the
                // expanded state of the item's children, so re-expanding
                // later starts fresh instead of showing every subfolder
                // that was previously open.
                TreeView_Expand(hWnd, hItem, TVE_COLLAPSE | TVE_COLLAPSERESET);
            } else if (!ctrlDown && !altDown && !shiftDown) {
                TVITEM item{};
                item.mask = TVIF_STATE | TVIF_CHILDREN;
                item.hItem = hItem;
                item.stateMask = TVIS_EXPANDED;
                TreeView_GetItem(hWnd, &item);

                bool hasChildren = item.cChildren != 0;
                bool isExpanded = (item.state & TVIS_EXPANDED) != 0;

                if (hasChildren && !isExpanded) {
                    TreeView_Expand(hWnd, hItem, TVE_EXPAND);
                }
            }

            return result;
        }
    } else if (uMsg == WM_NCDESTROY) {
        ForgetSubclassedTree(hWnd);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, TreeViewSubclassProc);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void TrySubclassTreeView(HWND hTreeWnd) {
    HWND hParent = GetParent(hTreeWnd);
    if (!hParent) {
        return;
    }

    WCHAR parentClassName[64];
    if (!GetClassNameW(hParent, parentClassName, ARRAYSIZE(parentClassName)) ||
        _wcsicmp(parentClassName, kNavPaneParentClassName) != 0) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_subclassedTreesMutex);
        if (std::find(g_subclassedTrees.begin(), g_subclassedTrees.end(),
                       hTreeWnd) != g_subclassedTrees.end()) {
            return;
        }
    }

    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hTreeWnd, TreeViewSubclassProc,
                                                        0)) {
        std::lock_guard<std::mutex> lock(g_subclassedTreesMutex);
        g_subclassedTrees.push_back(hTreeWnd);
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName,
                                  LPCWSTR lpWindowName, DWORD dwStyle, int X,
                                  int Y, int nWidth, int nHeight,
                                  HWND hWndParent, HMENU hMenu,
                                  HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                          dwStyle, X, Y, nWidth, nHeight,
                                          hWndParent, hMenu, hInstance, lpParam);

    // lpClassName can be an atom (a small integer cast to a pointer) rather
    // than a real string pointer, so only touch it as a string once we know
    // it's not one.
    if (hWnd && hWndParent &&
        ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0 &&
        _wcsicmp(lpClassName, kTreeViewClassName) == 0) {
        TrySubclassTreeView(hWnd);
    }

    return hWnd;
}

// Finds nav pane tree views that already exist in a given top-level window
// (used at init time, in case Explorer windows were already open before the
// mod was loaded).
BOOL CALLBACK EnumTreeChildProc(HWND hWnd, LPARAM lParam) {
    WCHAR className[64];
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, kTreeViewClassName) == 0) {
        TrySubclassTreeView(hWnd);
    }
    return TRUE;
}

BOOL CALLBACK EnumTopLevelWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        EnumChildWindows(hWnd, EnumTreeChildProc, 0);
    }
    return TRUE;
}

BOOL Wh_ModInit() {
    LoadSettings();

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                    &CreateWindowExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    // Pick up nav pane tree views in Explorer windows that were already
    // open when the mod was loaded.
    EnumWindows(EnumTopLevelWindowsProc, 0);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    std::vector<HWND> trees;
    {
        std::lock_guard<std::mutex> lock(g_subclassedTreesMutex);
        trees = g_subclassedTrees;
        g_subclassedTrees.clear();
    }

    for (HWND hTree : trees) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hTree, TreeViewSubclassProc);
    }
}
