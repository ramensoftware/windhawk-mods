// ==WindhawkMod==
// @id              classic-expand-buttons
// @name            Classic Expand Buttons
// @description     Draws classic +/- expand buttons in Explorer
// @github          https://github.com/n1d3v
// @version         0.2
// @author          bricktapper!
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lgdi32
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Expand Buttons
This mod adds the +/- expand buttons found in Windows 7 when Classic Theme is enabled.

In Windows 7, if you hovered on the treeview it showed all of the buttons, this mod replicates that functionality.

*Forked off of [Classic Explorer Treeview](https://windhawk.net/mods/classic-explorer-treeview)*
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <commctrl.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

static bool g_hoveringTree = false;
static HWND g_hTree = NULL;
static HWND g_hNtc = NULL;

void DrawExpandoButton(HWND hTree, HDC hdc, HTREEITEM hItem) {
    RECT rect;
    if (!TreeView_GetItemRect(hTree, hItem, &rect, TRUE))
        return;

    DWORD windowColor = GetSysColor(COLOR_WINDOW);
    DWORD shadowColor = GetSysColor(COLOR_3DSHADOW);

    UINT itemHeight = TreeView_GetItemHeight(hTree);
    switch (itemHeight) {
    default:
    case 16: rect.left -= 34; rect.top += 6; break;
    case 18: rect.left -= 33; rect.top += 8; break;
    }

    TVITEM tvi{};
    tvi.mask = TVIF_CHILDREN;
    tvi.hItem = hItem;
    if (!TreeView_GetItem(hTree, &tvi) || tvi.cChildren <= 0)
        return;

    UINT state = TreeView_GetItemState(hTree, hItem, TVIS_EXPANDED);
    RECT buttonRect{ rect.left, rect.top, rect.left + 9, rect.top + 9 };

    HBRUSH brush = CreateSolidBrush(windowColor);
    FillRect(hdc, &buttonRect, brush);
    DeleteObject(brush);

    brush = CreateSolidBrush(shadowColor);
    FrameRect(hdc, &buttonRect, brush);
    DeleteObject(brush);

    HPEN hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNTEXT));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    MoveToEx(hdc, rect.left + 2, rect.top + 4, NULL);
    LineTo(hdc, rect.left + 7, rect.top + 4);

    if (!(state & TVIS_EXPANDED)) {
        MoveToEx(hdc, rect.left + 4, rect.top + 2, NULL);
        LineTo(hdc, rect.left + 4, rect.top + 7);
    }

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

LRESULT CALLBACK TVSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    switch (uMsg) {
    case WM_MOUSEMOVE: {
        if (!g_hoveringTree) {
            g_hoveringTree = true;
            InvalidateRect(hWnd, NULL, FALSE);
        }
        TRACKMOUSEEVENT tme{ sizeof(TRACKMOUSEEVENT), TME_LEAVE, hWnd, 0 };
        TrackMouseEvent(&tme);
        break;
    }
    case WM_MOUSELEAVE:
        if (g_hoveringTree) {
            g_hoveringTree = false;
            InvalidateRect(hWnd, NULL, FALSE);
        }
        break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK NTCSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_NOTIFY) {
        LPNMHDR lpnmh = (LPNMHDR)lParam;
        if (lpnmh->code == NM_CUSTOMDRAW) {
            LPNMTVCUSTOMDRAW pcd = (LPNMTVCUSTOMDRAW)lParam;
            if (pcd->nmcd.dwDrawStage == CDDS_PREPAINT)
                return CDRF_NOTIFYITEMDRAW;
            if (pcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
                return CDRF_NOTIFYPOSTPAINT;
            if (pcd->nmcd.dwDrawStage == CDDS_ITEMPOSTPAINT) {
                if (g_hoveringTree) {
                    HTREEITEM item = reinterpret_cast<HTREEITEM>(pcd->nmcd.dwItemSpec);
                    DrawExpandoButton(lpnmh->hwndFrom, pcd->nmcd.hdc, item);
                }
                return CDRF_DODEFAULT;
            }
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

typedef HWND (*__cdecl NSCCreateTreeview_t)(void*, HWND);
NSCCreateTreeview_t NSCCreateTreeviewOriginal;
HWND __cdecl NSCCreateTreeviewHook(void* pThis, HWND hWnd) {
    HWND treeview = NSCCreateTreeviewOriginal(pThis, hWnd);

    g_hNtc = hWnd;
    WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, NTCSubclassProc, 0);

    g_hTree = FindWindowExW(hWnd, NULL, L"SysTreeView32", NULL);
    if (g_hTree)
        WindhawkUtils::SetWindowSubclassFromAnyThread(g_hTree, TVSubclassProc, 0);

    return treeview;
}

BOOL Wh_ModInit() {
    HMODULE hExplorerFrame = LoadLibraryW(L"explorerframe.dll");
    if (!hExplorerFrame)
        return FALSE;

    WindhawkUtils::SYMBOL_HOOK explorerframe_dll_hooks[] = {
        {
            { L"private: struct HWND__ * __cdecl CNscTree::_CreateTreeview(struct HWND__ *)" },
            (void**)&NSCCreateTreeviewOriginal,
            (void*)NSCCreateTreeviewHook,
            FALSE
        }
    };
    return WindhawkUtils::HookSymbols(hExplorerFrame, explorerframe_dll_hooks, 1);
}

void Wh_ModUninit() {
    if (g_hTree)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_hTree, TVSubclassProc);
    if (g_hNtc)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_hNtc, NTCSubclassProc);

    g_hTree = NULL;
    g_hNtc = NULL;
}
