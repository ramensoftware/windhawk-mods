// ==WindhawkMod==
// @id              hide-pin
// @name            Hide Pin (in Explorer Navigation Pane)
// @description     Hides pin icons in File Explorer navigation pane
// @version         1.0
// @author          @danalec
// @github          https://github.com/danalec
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Removes pin icons from File Explorer's navigation pane by intercepting the tree view's item state queries.

![Screenshot](https://i.imgur.com/cv5tsUn.png)
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <commctrl.h>

typedef LRESULT (WINAPI *SendMessageW_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
SendMessageW_t SendMessageW_Original;

LRESULT WINAPI SendMessageW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == TVM_GETITEM || Msg == TVM_GETITEMW) {
        LRESULT result = SendMessageW_Original(hWnd, Msg, wParam, lParam);
        
        if (result && lParam) {
            TVITEMW* pItem = (TVITEMW*)lParam;
            if (pItem->mask & TVIF_STATE) {
                pItem->state &= ~TVIS_OVERLAYMASK;
            }
        }
        
        return result;
    }
    
    if (Msg == TVM_SETITEM || Msg == TVM_SETITEMW) {
        if (lParam) {
            TVITEMW* pItem = (TVITEMW*)lParam;
            if (pItem->mask & TVIF_STATE) {
                pItem->state &= ~TVIS_OVERLAYMASK;
            }
        }
    }
    
    if (Msg == TVM_SETIMAGELIST && wParam == TVSIL_STATE) {
        wchar_t className[256];
        if (GetClassNameW(hWnd, className, 256)) {
            if (wcsicmp(className, WC_TREEVIEW) == 0 || wcsicmp(className, L"SysTreeView32") == 0) {
                wchar_t parentClass[256];
                HWND hParent = GetParent(hWnd);
                if (hParent && GetClassNameW(hParent, parentClass, 256)) {
                    if (wcsstr(parentClass, L"SHELLDLL_DefView") || wcsstr(parentClass, L"NamespaceTreeControl")) {
                        return 0;
                    }
                }
            }
        }
    }
    
    return SendMessageW_Original(hWnd, Msg, wParam, lParam);
}

typedef LRESULT (WINAPI *DefWindowProcW_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DefWindowProcW_t DefWindowProcW_Original;

LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_NOTIFY) {
        NMHDR* pNmhdr = (NMHDR*)lParam;
        if (pNmhdr && pNmhdr->code == TVN_GETDISPINFOW) {
            NMTVDISPINFOW* pDispInfo = (NMTVDISPINFOW*)lParam;
            
            if (pDispInfo->item.mask & TVIF_STATE) {
                pDispInfo->item.state &= ~TVIS_OVERLAYMASK;
                pDispInfo->item.stateMask &= ~TVIS_OVERLAYMASK;
            }
        }
    }
    
    return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
}

BOOL Wh_ModInit() {
    Wh_SetFunctionHook(
        (void*)GetProcAddress(GetModuleHandle(L"user32.dll"), "SendMessageW"),
        (void*)SendMessageW_Hook,
        (void**)&SendMessageW_Original
    );
    
    Wh_SetFunctionHook(
        (void*)GetProcAddress(GetModuleHandle(L"user32.dll"), "DefWindowProcW"),
        (void*)DefWindowProcW_Hook,
        (void**)&DefWindowProcW_Original
    );
    
    return TRUE;
}

void Wh_ModUninit() {}