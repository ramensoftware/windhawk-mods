// ==WindhawkMod==
// @id              never-auto-expand-explorer-tree-items
// @name            Never Auto-Expand Explorer Tree Items
// @description     Stops the unwanted auto-expansion of navigation pane items even if the "Expand to current folder" option is off
// @version         1.1
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

**Note:** The "Desktop" root item can still auto-expand when the
"Show all folders" option is on, keeping the navigation pane populated.

| Before | After |
| :----: | :---: |
| ![](https://raw.githubusercontent.com/AromaKitsune/My-Windhawk-Mods/main/screenshots/never-auto-expand-explorer-tree-items_before.png) | ![](https://raw.githubusercontent.com/AromaKitsune/My-Windhawk-Mods/main/screenshots/never-auto-expand-explorer-tree-items_after.png) |

## Configuration
* **Allow top-level items to auto-expand:** Allows top-level items to
  auto-expand while keeping their nested items collapsed.
  * Enable this option if you want the "This PC" item to auto-expand while
    keeping its drive items collapsed.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- allowTopLevelItemAutoExpansion: false
  $name: Allow top-level items to auto-expand
  $description: >-
    Allows top-level items to auto-expand while keeping their nested items
    collapsed
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <commctrl.h>

struct
{
    bool allowTopLevelItemAutoExpansion;
} settings;

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

        if (GetClassNameW(hRootWnd, szClassName, ARRAYSIZE(szClassName)) &&
            (_wcsicmp(szClassName, L"CabinetWClass") == 0 ||
                _wcsicmp(szClassName, L"#32770") == 0))
        {
            return true;
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

// Helper: Check if the tree item is the only root item in the navigation pane
// This ensures the "Desktop" root item can still auto-expand when the
// "Show all folders" option is on.
bool IsSingleRootItem(HWND hTreeView, HTREEITEM hTreeItem)
{
    auto hFirstRootItem = TreeView_GetRoot(hTreeView);
    return (hTreeItem == hFirstRootItem &&
        TreeView_GetNextSibling(hTreeView, hFirstRootItem) == nullptr);
}

// Helper: Check if the tree item is a top-level item
// This allows top-level items such as "This PC" to auto-expand while keeping
// their nested items (such as drives) collapsed.
bool IsTopLevelItem(HWND hTreeView, HTREEITEM hTreeItem)
{
    HTREEITEM hParentItem = TreeView_GetParent(hTreeView, hTreeItem);
    return (hParentItem == nullptr ||
        hParentItem == TreeView_GetRoot(hTreeView));
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
            auto hTreeItem = reinterpret_cast<HTREEITEM>(lParam);
            if (IsExplorerNavigationPane(hWnd) &&
                !IsUserInteractingWithTreeView(hWnd) &&
                !IsSingleRootItem(hWnd, hTreeItem) &&
                !(settings.allowTopLevelItemAutoExpansion &&
                    IsTopLevelItem(hWnd, hTreeItem)))
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
                auto hTreeView = pNotifyHeader->hwndFrom;
                auto hTreeItem = pTreeViewNotify->itemNew.hItem;
                if (IsExplorerNavigationPane(hTreeView) &&
                    !IsUserInteractingWithTreeView(hTreeView) &&
                    !IsSingleRootItem(hTreeView, hTreeItem) &&
                    !(settings.allowTopLevelItemAutoExpansion &&
                        IsTopLevelItem(hTreeView, hTreeItem)))
                {
                    return TRUE;
                }
            }
        }
    }

    return SendMessageW_Original(hWnd, uMsg, wParam, lParam);
}

// Load settings
void LoadSettings()
{
    settings.allowTopLevelItemAutoExpansion =
        Wh_GetIntSetting(L"allowTopLevelItemAutoExpansion");
}

// Mod initialization
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    LoadSettings();

    WindhawkUtils::SetFunctionHook(
        SendMessageW,
        SendMessageW_Hook,
        &SendMessageW_Original
    );

    return TRUE;
}

// Reload settings
void Wh_ModSettingsChanged()
{
    LoadSettings();
}
