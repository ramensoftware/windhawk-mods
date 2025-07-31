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

typedef BOOL (WINAPI *SetWindowSubclass_t)(HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
typedef BOOL (WINAPI *RemoveWindowSubclass_t)(HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass);
typedef LRESULT (WINAPI *DefSubclassProc_t)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

SetWindowSubclass_t pSetWindowSubclass;
RemoveWindowSubclass_t pRemoveWindowSubclass;
DefSubclassProc_t pDefSubclassProc;

#define SetWindowSubclass pSetWindowSubclass
#define RemoveWindowSubclass pRemoveWindowSubclass
#define DefSubclassProc pDefSubclassProc

#include <windhawk_utils.h>

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

LRESULT CALLBACK TreeViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == TVM_SETIMAGELIST && wParam == TVSIL_STATE) {
        return 0;
    }
    
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

typedef HWND (WINAPI *CreateWindowExW_t)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    
    if (hWnd && lpClassName && HIWORD(lpClassName)) {
        if (wcsicmp(lpClassName, WC_TREEVIEW) == 0 || 
            wcsicmp(lpClassName, L"SysTreeView32") == 0) {
            wchar_t parentClass[256];
            HWND hParent = GetParent(hWnd);
            if (hParent && GetClassNameW(hParent, parentClass, 256)) {
                if (wcsstr(parentClass, L"SHELLDLL_DefView") || 
                    wcsstr(parentClass, L"NamespaceTreeControl")) {
                    WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TreeViewSubclassProc, 0);
                }
            }
        }
    }
    
    return hWnd;
}

BOOL Wh_ModInit() {
    HMODULE hComctl32 = LoadLibrary(L"comctl32.dll");
    if (hComctl32) {
        pSetWindowSubclass = (SetWindowSubclass_t)GetProcAddress(hComctl32, "SetWindowSubclass");
        pRemoveWindowSubclass = (RemoveWindowSubclass_t)GetProcAddress(hComctl32, "RemoveWindowSubclass");
        pDefSubclassProc = (DefSubclassProc_t)GetProcAddress(hComctl32, "DefSubclassProc");
    }
    
    if (!pSetWindowSubclass || !pRemoveWindowSubclass || !pDefSubclassProc) {
        return FALSE;
    }
    
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

void Wh_ModUninit() {}