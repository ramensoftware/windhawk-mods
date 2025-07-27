// ==WindhawkMod==
// @id              hide-pin
// @name            Hide Pin (in Explorer Navigation Pane)
// @description     Hides pin icon in Explorer navigation pane
// @version         1.0
// @author          @danalec
// @github          https://github.com/danalec
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Removes pin icons from File Explorer's navigation pane by intercepting the tree view's item state queries.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <commctrl.h>
#include <richedit.h>

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

WNDPROC g_originalTreeWndProc = nullptr;

LRESULT CALLBACK TreeViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == TVM_SETIMAGELIST && wParam == TVSIL_STATE) {
        return 0;
    }
    
    return CallWindowProc(g_originalTreeWndProc, hWnd, uMsg, wParam, lParam);
}

typedef HWND (WINAPI *CreateWindowExW_t)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    
    if (hWnd && lpClassName && !IsBadReadPtr(lpClassName, sizeof(WCHAR))) {
        if (wcsicmp(lpClassName, WC_TREEVIEW) == 0 || 
            wcsicmp(lpClassName, L"SysTreeView32") == 0) {
            wchar_t parentClass[256];
            HWND hParent = GetParent(hWnd);
            if (hParent && GetClassNameW(hParent, parentClass, 256)) {
                if (wcsstr(parentClass, L"SHELLDLL_DefView") || 
                    wcsstr(parentClass, L"NamespaceTreeControl")) {
                    g_originalTreeWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)TreeViewSubclassProc);
                }
            }
        }
    }
    
    return hWnd;
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
    
    Wh_SetFunctionHook(
        (void*)GetProcAddress(GetModuleHandle(L"user32.dll"), "CreateWindowExW"),
        (void*)CreateWindowExW_Hook,
        (void**)&CreateWindowExW_Original
    );
    
    return TRUE;
}

void Wh_ModUninit() {
}