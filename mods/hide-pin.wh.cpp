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
#include <vector>

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

struct SubclassedWindow {
    HWND hWnd;
    WNDPROC originalProc;
};

std::vector<SubclassedWindow> g_subclassedWindows;
CRITICAL_SECTION g_cs;

LRESULT CALLBACK TreeViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == TVM_SETIMAGELIST && wParam == TVSIL_STATE) {
        return 0;
    }
    
    EnterCriticalSection(&g_cs);
    for (const auto& window : g_subclassedWindows) {
        if (window.hWnd == hWnd) {
            WNDPROC originalProc = window.originalProc;
            LeaveCriticalSection(&g_cs);
            return CallWindowProc(originalProc, hWnd, uMsg, wParam, lParam);
        }
    }
    LeaveCriticalSection(&g_cs);
    
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
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
                    WNDPROC originalProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)TreeViewSubclassProc);
                    if (originalProc) {
                        EnterCriticalSection(&g_cs);
                        g_subclassedWindows.push_back({hWnd, originalProc});
                        LeaveCriticalSection(&g_cs);
                    }
                }
            }
        }
    }
    
    return hWnd;
}

BOOL Wh_ModInit() {
    InitializeCriticalSection(&g_cs);
    
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
    EnterCriticalSection(&g_cs);
    for (const auto& window : g_subclassedWindows) {
        if (IsWindow(window.hWnd)) {
            SetWindowLongPtr(window.hWnd, GWLP_WNDPROC, (LONG_PTR)window.originalProc);
        }
    }
    g_subclassedWindows.clear();
    LeaveCriticalSection(&g_cs);
    
    DeleteCriticalSection(&g_cs);
}