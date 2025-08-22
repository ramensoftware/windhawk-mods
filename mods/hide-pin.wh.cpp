// ==WindhawkMod==
// @id              hide-pin
// @name            Hide Pin (in Explorer Navigation Pane)
// @description     Hides pin icons in File Explorer navigation pane and Quick Access
// @version         1.0.6
// @author          @danalec
// @github          https://github.com/danalec
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lshell32 -lshlwapi
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
#include <psapi.h>
#include <string>

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

std::vector<HWND> g_subclassedTreeViews;
std::vector<HWND> g_subclassedListViews;
UINT_PTR g_timerId = 0;

typedef LRESULT (WINAPI *SendMessageW_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
SendMessageW_t SendMessageW_Original;

LRESULT WINAPI SendMessageW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
        // Log all messages to ListView controls for debugging
        wchar_t className[256];
        if (GetClassNameW(hWnd, className, 256)) {
            if (wcscmp(className, WC_LISTVIEW) == 0 || wcscmp(className, L"SysListView32") == 0) {
                Wh_Log(L"ListView Message: 0x%X to window class %s", Msg, className);
            }
        }
        
        // Handle TreeView messages
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
        
        if (Msg == TVM_INSERTITEM || Msg == TVM_INSERTITEMW) {
            if (lParam) {
                TVINSERTSTRUCTW* pInsert = (TVINSERTSTRUCTW*)lParam;
                if (pInsert->item.mask & TVIF_STATE) {
                    pInsert->item.state &= ~TVIS_OVERLAYMASK;
                }
            }
        }
        
        // Handle ListView messages for Quick Access
        if (Msg == LVM_GETITEM || Msg == LVM_GETITEMW) {
            LRESULT result = SendMessageW_Original(hWnd, Msg, wParam, lParam);
            
            if (result && lParam) {
                LVITEMW* pItem = (LVITEMW*)lParam;
                if (pItem->mask & LVIF_STATE) {
                    pItem->state &= ~LVIS_OVERLAYMASK;
                }
            }
            
            return result;
        }
        
        if (Msg == LVM_SETITEM || Msg == LVM_SETITEMW) {
            if (lParam) {
                LVITEMW* pItem = (LVITEMW*)lParam;
                if (pItem->mask & LVIF_STATE) {
                    pItem->state &= ~LVIS_OVERLAYMASK;
                }
            }
        }
        
        if (Msg == LVM_INSERTITEM || Msg == LVM_INSERTITEMW) {
            if (lParam) {
                LVITEMW* pItem = (LVITEMW*)lParam;
                if (pItem->mask & LVIF_STATE) {
                    pItem->state &= ~LVIS_OVERLAYMASK;
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
            if (pNmhdr) {
                // Handle TreeView notifications
                if (pNmhdr->code == TVN_GETDISPINFOW) {
                    NMTVDISPINFOW* pDispInfo = (NMTVDISPINFOW*)lParam;
                    
                    if (pDispInfo->item.mask & TVIF_STATE) {
                        pDispInfo->item.state &= ~TVIS_OVERLAYMASK;
                        pDispInfo->item.stateMask &= ~TVIS_OVERLAYMASK;
                    }
                }
                else if (pNmhdr->code == TVN_ITEMEXPANDING || pNmhdr->code == TVN_ITEMEXPANDED) {
                    NMTREEVIEW* pTreeView = (NMTREEVIEW*)lParam;
                    if (pTreeView->itemNew.mask & TVIF_STATE) {
                        pTreeView->itemNew.state &= ~TVIS_OVERLAYMASK;
                    }
                }
                // Handle ListView notifications for Quick Access
                else if (pNmhdr->code == LVN_GETDISPINFOW) {
                    NMLVDISPINFOW* pDispInfo = (NMLVDISPINFOW*)lParam;
                    
                    if (pDispInfo->item.mask & LVIF_STATE) {
                        pDispInfo->item.state &= ~LVIS_OVERLAYMASK;
                        pDispInfo->item.stateMask &= ~LVIS_OVERLAYMASK;
                    }
                }
                else if (pNmhdr->code == LVN_ITEMCHANGING || pNmhdr->code == LVN_ITEMCHANGED) {
                    NMLISTVIEW* pListView = (NMLISTVIEW*)lParam;
                    if (pListView->uNewState & LVIS_OVERLAYMASK) {
                        pListView->uNewState &= ~LVIS_OVERLAYMASK;
                    }
                }
            }
        }
        
        return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
    }
    
LRESULT CALLBACK TreeViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    // Block state image list to prevent pin icons
    if (uMsg == TVM_SETIMAGELIST && wParam == TVSIL_STATE) {
        return 0;
    }
    
    // Intercept item state queries and remove overlay masks
    if (uMsg == TVM_GETITEM || uMsg == TVM_GETITEMW) {
        LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        if (result && lParam) {
            TVITEMW* pItem = (TVITEMW*)lParam;
            if (pItem->mask & TVIF_STATE) {
                pItem->state &= ~TVIS_OVERLAYMASK;
                pItem->stateMask &= ~TVIS_OVERLAYMASK;
            }
        }
        return result;
    }
    
    // Intercept item state changes and remove overlay masks
    if (uMsg == TVM_SETITEM || uMsg == TVM_SETITEMW) {
        if (lParam) {
            TVITEMW* pItem = (TVITEMW*)lParam;
            if (pItem->mask & TVIF_STATE) {
                pItem->state &= ~TVIS_OVERLAYMASK;
                pItem->stateMask &= ~TVIS_OVERLAYMASK;
            }
        }
    }
    
    // Handle new item insertion
    if (uMsg == TVM_INSERTITEM || uMsg == TVM_INSERTITEMW) {
        if (lParam) {
            TVINSERTSTRUCTW* pInsert = (TVINSERTSTRUCTW*)lParam;
            if (pInsert->item.mask & TVIF_STATE) {
                pInsert->item.state &= ~TVIS_OVERLAYMASK;
            }
        }
    }
    
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ListViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    // Log all messages to this ListView for debugging
    Wh_Log(L"ListView Subclass: 0x%p received message 0x%X", hWnd, uMsg);
    
    // Block state image list to prevent pin icons
    if (uMsg == LVM_SETIMAGELIST && wParam == LVSIL_STATE) {
        Wh_Log(L"Blocking LVM_SETIMAGELIST with LVSIL_STATE");
        return 0;
    }
    
    // Intercept item state queries and remove overlay masks
    if (uMsg == LVM_GETITEM || uMsg == LVM_GETITEMW) {
        LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        if (result && lParam) {
            LVITEMW* pItem = (LVITEMW*)lParam;
            if (pItem->mask & LVIF_STATE) {
                Wh_Log(L"Removing overlay mask from LVM_GETITEM, original state: 0x%X", pItem->state);
                pItem->state &= ~LVIS_OVERLAYMASK;
                pItem->stateMask &= ~LVIS_OVERLAYMASK;
            }
        }
        return result;
    }
    
    // Intercept item state changes and remove overlay masks
    if (uMsg == LVM_SETITEM || uMsg == LVM_SETITEMW) {
        if (lParam) {
            LVITEMW* pItem = (LVITEMW*)lParam;
            if (pItem->mask & LVIF_STATE) {
                Wh_Log(L"Removing overlay mask from LVM_SETITEM, original state: 0x%X", pItem->state);
                pItem->state &= ~LVIS_OVERLAYMASK;
                pItem->stateMask &= ~LVIS_OVERLAYMASK;
            }
        }
    }
    
    // Handle new item insertion
    if (uMsg == LVM_INSERTITEM || uMsg == LVM_INSERTITEMW) {
        if (lParam) {
            LVITEMW* pItem = (LVITEMW*)lParam;
            if (pItem->mask & LVIF_STATE) {
                pItem->state &= ~LVIS_OVERLAYMASK;
            }
        }
    }
    
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
    
typedef HWND (WINAPI *CreateWindowExW_t)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hwnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    
    // Log all window creation for debugging
    if (hwnd && lpClassName && HIWORD(lpClassName)) {
        Wh_Log(L"Window created: class=%s, name=%s", lpClassName, lpWindowName ? lpWindowName : L"(null)");
    }
    
    if (hwnd && lpClassName && HIWORD(lpClassName)) {
        bool shouldSubclass = false;
        bool isTreeView = false;
        bool isListView = false;
        
        if (wcscmp(lpClassName, WC_TREEVIEW) == 0 || 
            wcscmp(lpClassName, L"SysTreeView32") == 0) {
            isTreeView = true;
        }
        else if (wcscmp(lpClassName, WC_LISTVIEW) == 0 || 
                 wcscmp(lpClassName, L"SysListView32") == 0) {
            isListView = true;
        }
        
        // Also check for DirectUI and other modern controls
        else if (wcscmp(lpClassName, L"DirectUIHWND") == 0 ||
                 wcscmp(lpClassName, L"CtrlNotifySink") == 0 ||
                 wcscmp(lpClassName, L"UIRibbonWorkPane") == 0) {
            Wh_Log(L"Found modern UI control: %s", lpClassName);
        }
        
        if (isTreeView || isListView) {
            
            // Check if this is in Explorer by walking up the parent chain
            HWND hTopLevel = hwnd;
            while (hTopLevel) {
                HWND hParentWindow = GetParent(hTopLevel);
                if (!hParentWindow) break;
                hTopLevel = hParentWindow;
            }
            
            wchar_t topLevelClass[256];
            if (GetClassNameW(hTopLevel, topLevelClass, 256)) {
                // Apply to all tree views in Explorer windows
                if (wcsstr(topLevelClass, L"CabinetWClass") || 
                    wcsstr(topLevelClass, L"ExploreWClass") ||
                    wcsstr(topLevelClass, L"Shell_Flyout") ||
                    wcsstr(topLevelClass, L"ShellTabWindowClass") ||
                    wcsstr(topLevelClass, L"Progman") ||
                    wcsstr(topLevelClass, L"WorkerW")) {
                    shouldSubclass = true;
                }
            }
            
            // Also check immediate parent for Quick Access specific classes
            if (hWndParent) {
                wchar_t parentClass[256];
                if (GetClassNameW(hWndParent, parentClass, 256)) {
                    if (wcsstr(parentClass, L"NamespaceTreeControl") ||
                        wcsstr(parentClass, L"ShellNamespaceTreeControl") ||
                        wcsstr(parentClass, L"QuickAccess") ||
                        wcsstr(parentClass, L"ShellView") ||
                        wcsstr(parentClass, L"SHELLDLL_DefView")) {
                        shouldSubclass = true;
                    }
                }
            }
            
            // If no specific parent found, but this is a tree view, check if we're in explorer.exe
            if (!shouldSubclass) {
                wchar_t processPath[MAX_PATH];
                DWORD pid = 0;
                GetWindowThreadProcessId(hwnd, &pid);
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
                if (hProcess) {
                    if (GetModuleFileNameExW(hProcess, nullptr, processPath, MAX_PATH)) {
                        // Check if the filename is explorer.exe
                        size_t len = wcslen(processPath);
                        if (len >= 11) { // "explorer.exe" is 11 chars
                            const wchar_t* filename = processPath + len - 11;
                            if (_wcsicmp(filename, L"explorer.exe") == 0) {
                                shouldSubclass = true;
                            }
                        }
                    }
                    CloseHandle(hProcess);
                }
            }
            
            if (shouldSubclass) {
                if (pSetWindowSubclass) {
                    if (isTreeView) {
                        Wh_Log(L"Subclassing TreeView: 0x%p", hwnd);
                        WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, TreeViewSubclassProc, 0);
                        g_subclassedTreeViews.push_back(hwnd);
                    }
                    else if (isListView) {
                        Wh_Log(L"Subclassing ListView: 0x%p", hwnd);
                        WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, ListViewSubclassProc, 0);
                        g_subclassedListViews.push_back(hwnd);
                    }
                }
            }
        }
    }
    
    return hwnd;
}

BOOL Wh_ModInit() {
    HMODULE hComctl32 = GetModuleHandle(L"comctl32.dll");
    if (!hComctl32) {
        hComctl32 = LoadLibrary(L"comctl32.dll");
    }
    
    if (hComctl32) {
        pSetWindowSubclass = (SetWindowSubclass_t)GetProcAddress(hComctl32, "SetWindowSubclass");
        pRemoveWindowSubclass = (RemoveWindowSubclass_t)GetProcAddress(hComctl32, "RemoveWindowSubclass");
        pDefSubclassProc = (DefSubclassProc_t)GetProcAddress(hComctl32, "DefSubclassProc");
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

BOOL CALLBACK EnumChildProc(HWND child, LPARAM lParam) {
    wchar_t childClass[256];
    if (GetClassNameW(child, childClass, 256)) {
        // Log all child windows for debugging
        Wh_Log(L"Found child window: class=%s, hwnd=0x%p", childClass, child);
        
        if (wcscmp(childClass, WC_TREEVIEW) == 0 || 
            wcscmp(childClass, L"SysTreeView32") == 0) {
            
            // Check if already subclassed
            auto it = std::find(g_subclassedTreeViews.begin(), g_subclassedTreeViews.end(), child);
            if (it == g_subclassedTreeViews.end()) {
                if (pSetWindowSubclass) {
                    Wh_Log(L"Enum: Subclassing TreeView 0x%p", child);
                    WindhawkUtils::SetWindowSubclassFromAnyThread(child, TreeViewSubclassProc, 0);
                    g_subclassedTreeViews.push_back(child);
                }
            }
        }
        else if (wcscmp(childClass, WC_LISTVIEW) == 0 || 
                 wcscmp(childClass, L"SysListView32") == 0) {
            
            // Check if already subclassed
            auto it = std::find(g_subclassedListViews.begin(), g_subclassedListViews.end(), child);
            if (it == g_subclassedListViews.end()) {
                if (pSetWindowSubclass) {
                    Wh_Log(L"Enum: Subclassing ListView 0x%p", child);
                    WindhawkUtils::SetWindowSubclassFromAnyThread(child, ListViewSubclassProc, 0);
                    g_subclassedListViews.push_back(child);
                }
            }
        }
    }
    return TRUE;
}

void SubclassAllTreeViewsInWindow(HWND hwnd) {
    EnumChildWindows(hwnd, EnumChildProc, 0);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    wchar_t className[256];
    if (GetClassNameW(hwnd, className, 256)) {
        if (wcsstr(className, L"CabinetWClass") || 
            wcsstr(className, L"ExploreWClass") ||
            wcsstr(className, L"Shell_Flyout") ||
            wcsstr(className, L"ShellTabWindowClass")) {
            Wh_Log(L"Found Explorer window: class=%s, hwnd=0x%p", className, hwnd);
            SubclassAllTreeViewsInWindow(hwnd);
        }
    }
    return TRUE;
}

void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    EnumWindows(EnumWindowsProc, 0);
}

void Wh_ModAfterInit() {
    // Enumerate existing windows to catch Quick Access tree views that were created before our hooks
    EnumWindows(EnumWindowsProc, 0);
    
    // Set up timer to periodically check for new tree views (every 2 seconds)
    g_timerId = SetTimer(nullptr, 0, 2000, TimerProc);
}

void Wh_ModSettingsChanged() {
}

void Wh_ModUninit() {
    if (g_timerId) {
        KillTimer(nullptr, g_timerId);
        g_timerId = 0;
    }
    
    // Clean up all subclassed TreeView windows
    for (HWND hwnd : g_subclassedTreeViews) {
        if (IsWindow(hwnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, TreeViewSubclassProc);
        }
    }
    g_subclassedTreeViews.clear();
    
    // Clean up all subclassed ListView windows
    for (HWND hwnd : g_subclassedListViews) {
        if (IsWindow(hwnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, ListViewSubclassProc);
        }
    }
    g_subclassedListViews.clear();
}