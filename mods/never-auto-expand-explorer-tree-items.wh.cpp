// ==WindhawkMod==
// @id              never-auto-expand-explorer-tree-items
// @name            Never Auto-Expand Explorer Tree Items
// @description     Stops the unwanted auto-expansion of navigation pane items even if the "Expand to current folder" option is off
// @version         1.0.1
// @author          Kitsune
// @github          https://github.com/AromaKitsune
// @include         *
// @compilerOptions -lcomctl32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Never Auto-Expand Explorer Tree Items
File Explorer automatically expands navigation pane items (such as "This PC")
even if the "Expand to current folder" option is off, specifically when:
* Opening any folder inside an external drive in a new tab or window.
* Navigating to any drive after manually expanding and collapsing the "This PC"
  item.

This mod prevents this unwanted auto-expansion behavior, keeping the navigation
pane tidy.

**Note:** The top-level "Desktop" item can still auto-expand when the
"Show all folders" option is on, keeping the navigation pane populated.

| Before | After |
| :----: | :---: |
| ![](https://raw.githubusercontent.com/AromaKitsune/My-Windhawk-Mods/main/screenshots/never-auto-expand-explorer-tree-items_before.png) | ![](https://raw.githubusercontent.com/AromaKitsune/My-Windhawk-Mods/main/screenshots/never-auto-expand-explorer-tree-items_after.png) |
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <commctrl.h>

// Helper: Verify if the tree view belongs to the File Explorer navigation pane
bool IsExplorerNavigationPane(HWND hTreeView)
{
    HWND hParentWnd = GetParent(hTreeView);
    if (!hParentWnd)
    {
        return false;
    }

    WCHAR szClassName[32];
    if (GetClassNameW(hParentWnd, szClassName, ARRAYSIZE(szClassName)) &&
        _wcsicmp(szClassName, L"NamespaceTreeControl") == 0)
    {
        // Retrieve the root window to confirm it's a File Explorer window or
        // file picker dialog
        HWND hRootWnd = GetAncestor(hTreeView, GA_ROOT);
        if (!hRootWnd)
        {
            return false;
        }

        if (GetClassNameW(hRootWnd, szClassName, ARRAYSIZE(szClassName)))
        {
            if (_wcsicmp(szClassName, L"CabinetWClass") == 0 ||
                _wcsicmp(szClassName, L"#32770") == 0)
            {
                return true;
            }
        }
    }

    return false;
}

// Helper: Verify if the tree view is receiving keyboard or mouse input
// This allows manual expansion of navigation pane items.
bool IsUserInteractingWithTreeView(HWND hTreeView)
{
    // Verify keyboard interactions
    HWND hFocusWnd = GetFocus();
    if (hFocusWnd == hTreeView || IsChild(hTreeView, hFocusWnd))
    {
        return true;
    }

    // Verify mouse interactions
    if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) ||
        (GetAsyncKeyState(VK_RBUTTON) & 0x8000) ||
        (GetAsyncKeyState(VK_MBUTTON) & 0x8000))
    {
        POINT cursorPos;
        if (GetCursorPos(&cursorPos))
        {
            HWND hHoverWnd = WindowFromPoint(cursorPos);
            if (hHoverWnd == hTreeView || IsChild(hTreeView, hHoverWnd))
            {
                return true;
            }
        }
    }

    return false;
}

// Helper: Check if the tree item is the only root node in the navigation pane
// This ensures the top-level "Desktop" item can still auto-expand when the
// "Show all folders" option is on.
bool IsSingleRootNode(HWND hTreeView, HTREEITEM hTreeItem)
{
    auto hFirstRootItem = TreeView_GetRoot(hTreeView);
    if (hTreeItem == hFirstRootItem)
    {
        return TreeView_GetNextSibling(hTreeView, hFirstRootItem) == nullptr;
    }
    return false;
}

// Hook for SendMessageW
using SendMessageW_t = decltype(&SendMessageW);
SendMessageW_t SendMessageW_Original;
LRESULT WINAPI SendMessageW_Hook(HWND hWnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam)
{
    if (uMsg != TVM_EXPAND && uMsg != WM_NOTIFY)
    {
        return SendMessageW_Original(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == TVM_EXPAND && (wParam & TVE_EXPAND))
    {
        WCHAR szClassName[16];
        if (GetClassNameW(hWnd, szClassName, ARRAYSIZE(szClassName)) &&
            _wcsicmp(szClassName, L"SysTreeView32") == 0)
        {
            if (IsExplorerNavigationPane(hWnd) &&
                !IsUserInteractingWithTreeView(hWnd) &&
                !IsSingleRootNode(hWnd, reinterpret_cast<HTREEITEM>(lParam)))
            {
                return 0;
            }
        }
    }
    else if (uMsg == WM_NOTIFY)
    {
        auto* pNotifyHeader = reinterpret_cast<LPNMHDR>(lParam);
        if (pNotifyHeader != nullptr &&
            pNotifyHeader->code == TVN_ITEMEXPANDINGW)
        {
            auto* pTreeViewNotify = reinterpret_cast<LPNMTREEVIEWW>(lParam);
            if (pTreeViewNotify->action == TVE_EXPAND)
            {
                if (IsExplorerNavigationPane(pNotifyHeader->hwndFrom) &&
                    !IsUserInteractingWithTreeView(pNotifyHeader->hwndFrom) &&
                    !IsSingleRootNode(pNotifyHeader->hwndFrom,
                        pTreeViewNotify->itemNew.hItem))
                {
                    return TRUE;
                }
            }
        }
    }

    return SendMessageW_Original(hWnd, uMsg, wParam, lParam);
}

// Mod initialization
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    WindhawkUtils::SetFunctionHook(
        SendMessageW,
        SendMessageW_Hook,
        &SendMessageW_Original
    );

    return TRUE;
}
