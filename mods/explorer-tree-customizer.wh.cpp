// ==WindhawkMod==
// @id              explorer-tree-customizer
// @name            Explorer Navigation Tree Customizer
// @description     Compact Mode; Hide Gallery, Home, OneDrive, This PC, Network.
// @version         1.0.2
// @author          Padl0
// @github          https://github.com/sukapadl0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32 -lshlwapi -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# This mod customizes the Windows 11 File Explorer navigation tree:
- Compact mode - changing vertical spacing between navigation groups.
- Deleting anoying elements (Home, Gallery, OneDrive).
- Deleting "This PC", "Network".

# Preview
![Preview](https://raw.githubusercontent.com/sukapadl0/Hlam/76bd7fc0dcd76366d7e2218450873df8863b5a51/hlam1.gif)

## Mod authorship
The submitter, with AI assistance
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- compact:
  - mode: true
    $name: Compact Mode
    $description: Tightens base row height for items
  - itemHeight: 32
    $name: Item Height (px)
    $description: "Row height in pixels (range: 18 to 48)"
  $name: Layout & Density
  $description: Control row density and item heights

- topNav:
  - hideGallery: true
    $name: Hide Gallery
    $description: Removes the Gallery item
  - hideHome: true
    $name: Hide Home
    $description: Removes the Home / Quick Access item
  - hideOneDrive: true
    $name: Hide OneDrive
    $description: Removes any OneDrive folders
  $name: "Top Navigation Items"
  $description: Toggle items in the upper system block

- coreNodes:
  - hideThisPC: false
    $name: Hide "This PC"
    $description: Removes the This PC root node
  - hideNetwork: false
    $name: Hide "Network"
    $description: Removes the Network root node
  $name: "Core System Nodes"
  $description: Toggle This PC and Network roots
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <windhawk_utils.h>
#include <string>
#include <unordered_set>

#define WM_USER_CLEAN_TREE (WM_USER + 0x4321)

struct Settings {
    bool compactMode;
    int itemHeight;
    bool hideGallery;
    bool hideHome;
    bool hideOneDrive;
    bool hideThisPC;
    bool hideNetwork;
};

static Settings g_settings = {};
static DWORD g_currentPid = 0;
static std::unordered_set<HWND> g_seenTreeWindows;
static std::unordered_set<HWND> g_attachedTreeWindows;
static std::unordered_set<HWND> g_attachedParentWindows;
static thread_local int g_cleaningDepth = 0;

using CreateWindowExW_t = decltype(&CreateWindowExW);
using CreateWindowExA_t = decltype(&CreateWindowExA);
static CreateWindowExW_t CreateWindowExW_Original = nullptr;
static CreateWindowExA_t CreateWindowExA_Original = nullptr;

static void SetRegDword(HKEY hRoot, LPCWSTR subKey, LPCWSTR valName, DWORD val) {
    HKEY hKey;
    if (RegCreateKeyExW(hRoot, subKey, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        RegSetValueExW(hKey, valName, 0, REG_DWORD, (const BYTE*)&val, sizeof(DWORD));
        RegCloseKey(hKey);
    }
}

static void SyncRegistryKeys(const Settings& s) {
    DWORD oneDriveVal = s.hideOneDrive ? 0 : 1;
    SetRegDword(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID\\{018D5C66-4533-4307-9B53-224DE2ED1FE6}", L"System.IsPinnedToNameSpaceTree", oneDriveVal);
    SetRegDword(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID\\{04271989-C4D2-40F9-ADC2-0E10204F4BE5}", L"System.IsPinnedToNameSpaceTree", oneDriveVal);

    DWORD homeVal = s.hideHome ? 0 : 1;
    SetRegDword(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID\\{f874310e-b6b7-47dc-bc84-b9e6b38f5903}", L"System.IsPinnedToNameSpaceTree", homeVal);

    DWORD galleryVal = s.hideGallery ? 0 : 1;
    SetRegDword(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID\\{e88865ea-0e1c-4e20-9aa6-ed3ce5676b77}", L"System.IsPinnedToNameSpaceTree", galleryVal);

    DWORD thisPCVal = s.hideThisPC ? 0 : 1;
    SetRegDword(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID\\{20D04FE0-3AEA-1069-A2D8-08002B30309D}", L"System.IsPinnedToNameSpaceTree", thisPCVal);

    DWORD networkVal = s.hideNetwork ? 0 : 1;
    SetRegDword(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID\\{F02C1A0D-BE21-4350-88B0-7367FC96EF3C}", L"System.IsPinnedToNameSpaceTree", networkVal);
}

static void LoadSettings() {
    g_settings.compactMode = Wh_GetIntSetting(L"compact.mode") != 0;
    g_settings.itemHeight = Wh_GetIntSetting(L"compact.itemHeight");
    
    g_settings.hideGallery = Wh_GetIntSetting(L"topNav.hideGallery") != 0;
    g_settings.hideHome = Wh_GetIntSetting(L"topNav.hideHome") != 0;
    g_settings.hideOneDrive = Wh_GetIntSetting(L"topNav.hideOneDrive") != 0;

    g_settings.hideThisPC = Wh_GetIntSetting(L"coreNodes.hideThisPC") != 0;
    g_settings.hideNetwork = Wh_GetIntSetting(L"coreNodes.hideNetwork") != 0;

    if (g_settings.itemHeight < 18) g_settings.itemHeight = 18;
    if (g_settings.itemHeight > 48) g_settings.itemHeight = 48;

    SyncRegistryKeys(g_settings);
}

static std::wstring GetWindowClassString(HWND hwnd) {
    wchar_t buf[256] = {};
    GetClassNameW(hwnd, buf, ARRAYSIZE(buf));
    return buf;
}

static bool HasAncestorClass(HWND hwnd, const wchar_t* targetClass, int maxLevels) {
    HWND current = hwnd;
    for (int level = 0; current && level < maxLevels; level++) {
        if (GetWindowClassString(current) == targetClass) {
            return true;
        }
        current = GetParent(current);
    }
    return false;
}

static bool IsExplorerNavigationTree(HWND hwnd) {
    if (!IsWindow(hwnd) || GetWindowClassString(hwnd) != L"SysTreeView32") {
        return false;
    }
    if (!HasAncestorClass(hwnd, L"NamespaceTreeControl", 6)) {
        return false;
    }
    if (!HasAncestorClass(hwnd, L"CabinetWClass", 12)) {
        return false;
    }
    return true;
}

static std::wstring ResolveTreeItemTextFast(HWND tree, HTREEITEM item) {
    if (!item || !IsWindow(tree)) return L"";

    wchar_t text[512] = {};
    TVITEMW tvi = {};
    tvi.mask = TVIF_TEXT | TVIF_HANDLE;
    tvi.hItem = item;
    tvi.pszText = text;
    tvi.cchTextMax = ARRAYSIZE(text);

    if (SendMessageW(tree, TVM_GETITEMW, 0, (LPARAM)&tvi)) {
        if (tvi.pszText != LPSTR_TEXTCALLBACKW && text[0] != L'\0') {
            return text;
        }
    }

    HWND parent = GetParent(tree);
    if (parent && IsWindow(parent)) {
        NMTVDISPINFOW disp = {};
        disp.hdr.hwndFrom = tree;
        disp.hdr.idFrom = GetDlgCtrlID(tree);
        disp.hdr.code = TVN_GETDISPINFOW;
        disp.item.mask = TVIF_TEXT | TVIF_HANDLE;
        disp.item.hItem = item;
        disp.item.pszText = text;
        disp.item.cchTextMax = ARRAYSIZE(text);

        if (SendMessageW(parent, WM_NOTIFY, (WPARAM)disp.hdr.idFrom, (LPARAM)&disp)) {
            if (disp.item.pszText && disp.item.pszText != LPSTR_TEXTCALLBACKW && disp.item.pszText[0] != L'\0') {
                return disp.item.pszText;
            }
        }
    }

    return text[0] != L'\0' ? text : L"";
}

static bool IsOneDriveText(const std::wstring& text) {
    if (text.empty()) return false;
    return StrStrIW(text.c_str(), L"OneDrive") != nullptr ||
           StrStrIW(text.c_str(), L"SkyDrive") != nullptr;
}

static bool IsHomeText(const std::wstring& text) {
    if (text.empty()) return false;
    return StrStrIW(text.c_str(), L"Главная") != nullptr ||
           StrStrIW(text.c_str(), L"Головна") != nullptr ||
           StrStrIW(text.c_str(), L"Home") != nullptr ||
           StrStrIW(text.c_str(), L"Быстрый доступ") != nullptr ||
           StrStrIW(text.c_str(), L"Швидкий доступ") != nullptr ||
           StrStrIW(text.c_str(), L"Quick access") != nullptr;
}

static bool IsGalleryText(const std::wstring& text) {
    if (text.empty()) return false;
    return StrStrIW(text.c_str(), L"Галерея") != nullptr ||
           StrStrIW(text.c_str(), L"Gallery") != nullptr;
}

static bool IsThisPCText(const std::wstring& text) {
    if (text.empty()) return false;
    return StrStrIW(text.c_str(), L"Этот компьютер") != nullptr ||
           StrStrIW(text.c_str(), L"Цей ПК") != nullptr ||
           StrStrIW(text.c_str(), L"This PC") != nullptr;
}

static bool IsNetworkText(const std::wstring& text) {
    if (text.empty()) return false;
    return StrStrIW(text.c_str(), L"Сеть") != nullptr ||
           StrStrIW(text.c_str(), L"Мережа") != nullptr ||
           StrStrIW(text.c_str(), L"Network") != nullptr;
}

static bool ShouldRemoveItem(HWND tree, HTREEITEM item, const std::wstring& text) {
    if (g_settings.hideOneDrive && IsOneDriveText(text)) return true;
    if (g_settings.hideGallery && IsGalleryText(text)) return true;
    if (g_settings.hideHome && IsHomeText(text)) return true;
    if (g_settings.hideThisPC && IsThisPCText(text)) return true;
    if (g_settings.hideNetwork && IsNetworkText(text)) return true;

    return false;
}

static COLORREF GetCurrentTreeBgColor(HWND tree, HDC hdc, const RECT& client) {
    COLORREF bgColor = (COLORREF)SendMessageW(tree, TVM_GETBKCOLOR, 0, 0);
    if (bgColor == CLR_NONE || bgColor == (COLORREF)-1) {
        int sampleX = client.right > 12 ? client.right - 12 : client.right - 2;
        if (sampleX > 0 && hdc) {
            bgColor = GetPixel(hdc, sampleX, client.bottom > 12 ? client.bottom - 12 : 2);
        }
    }
    if (bgColor == CLR_INVALID || bgColor == CLR_NONE) {
        bgColor = GetSysColor(COLOR_WINDOW);
    }
    return bgColor;
}

static void PaintEraseAllDividers(HWND tree, HDC hdc) {
    if (!IsWindow(tree) || !hdc) return;

    RECT client = {};
    GetClientRect(tree, &client);
    if (client.right <= 0 || client.bottom <= 0) return;

    COLORREF bgColor = GetCurrentTreeBgColor(tree, hdc, client);
    HBRUSH bgBrush = CreateSolidBrush(bgColor);
    if (!bgBrush) return;

    HTREEITEM item = TreeView_GetFirstVisible(tree);
    while (item) {
        RECT rc = {};
        rc.left = 1;
        if (TreeView_GetItemRect(tree, item, &rc, FALSE)) {
            RECT lineMask = { client.left, rc.top - 4, client.right, rc.top + 1 };
            if (lineMask.top >= 0 && lineMask.bottom <= client.bottom) {
                FillRect(hdc, &lineMask, bgBrush);
            }
        }
        item = TreeView_GetNextVisible(tree, item);
    }

    DeleteObject(bgBrush);
}

static void CleanTreeNodesSafe(HWND tree) {
    if (!IsWindow(tree) || g_cleaningDepth > 0) return;

    g_cleaningDepth++;

    HTREEITEM item = TreeView_GetRoot(tree);
    while (item) {
        HTREEITEM next = TreeView_GetNextSibling(tree, item);
        std::wstring text = ResolveTreeItemTextFast(tree, item);

        if (ShouldRemoveItem(tree, item, text)) {
            TreeView_DeleteItem(tree, item);
        }
        item = next;
    }

    int curHeight = (int)SendMessageW(tree, TVM_GETITEMHEIGHT, 0, 0);
    int targetHeight = g_settings.compactMode ? g_settings.itemHeight : 36;
    if (curHeight != targetHeight) {
        SendMessageW(tree, TVM_SETITEMHEIGHT, (WPARAM)targetHeight, 0);
    }

    g_cleaningDepth--;
}

static LRESULT CALLBACK ParentSubclassProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR dwRefData) {

    HWND tree = (HWND)dwRefData;

    if (uMsg == WM_NOTIFY && tree && IsWindow(tree)) {
        LPNMHDR hdr = (LPNMHDR)lParam;
        if (hdr && hdr->hwndFrom == tree) {
            if (hdr->code == (UINT)NM_CUSTOMDRAW) {
                LPNMTVCUSTOMDRAW cd = (LPNMTVCUSTOMDRAW)lParam;
                if (cd->nmcd.dwDrawStage == CDDS_PREPAINT) {
                    return CDRF_NOTIFYPOSTPAINT;
                }
                if (cd->nmcd.dwDrawStage == CDDS_POSTPAINT) {
                    PaintEraseAllDividers(tree, cd->nmcd.hdc);
                }
            } else if (hdr->code == TVN_GETDISPINFOW) {
                LPNMTVDISPINFO tdi = (LPNMTVDISPINFO)lParam;
                if ((tdi->item.mask & TVIF_TEXT) && tdi->item.pszText && tdi->item.pszText != LPSTR_TEXTCALLBACKW) {
                    if (ShouldRemoveItem(tree, tdi->item.hItem, tdi->item.pszText) && g_cleaningDepth == 0) {
                        PostMessageW(tree, WM_USER_CLEAN_TREE, 0, 0);
                    }
                }
            }
        }
    } else if (uMsg == WM_NCDESTROY) {
        g_attachedParentWindows.erase(hwnd);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, ParentSubclassProc);
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

static LRESULT CALLBACK TreeSubclassProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR dwRefData) {

    switch (uMsg) {
        case WM_USER_CLEAN_TREE:
            CleanTreeNodesSafe(hwnd);
            return 0;

        case TVM_INSERTITEMW:
        case TVM_INSERTITEMA:
        case TVM_SETITEMW:
        case TVM_SETITEMA: {
            LRESULT res = DefSubclassProc(hwnd, uMsg, wParam, lParam);
            if (g_cleaningDepth == 0) {
                PostMessageW(hwnd, WM_USER_CLEAN_TREE, 0, 0);
            }
            return res;
        }

        case WM_PAINT:
            if (g_cleaningDepth == 0) {
                CleanTreeNodesSafe(hwnd);
            }
            break;

        case WM_NCDESTROY:
            g_seenTreeWindows.erase(hwnd);
            g_attachedTreeWindows.erase(hwnd);
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, TreeSubclassProc);
            break;
    }

    LRESULT result = DefSubclassProc(hwnd, uMsg, wParam, lParam);

    if (uMsg == WM_PAINT) {
        HDC hdc = GetDC(hwnd);
        if (hdc) {
            PaintEraseAllDividers(hwnd, hdc);
            ReleaseDC(hwnd, hdc);
        }
    }

    return result;
}

static void AttachTreeHooks(HWND tree) {
    if (!IsWindow(tree)) return;

    if (!g_attachedTreeWindows.insert(tree).second) {
        return;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(tree, TreeSubclassProc, 0)) {
        g_attachedTreeWindows.erase(tree);
        return;
    }

    HWND parent = GetParent(tree);
    if (parent && g_attachedParentWindows.insert(parent).second) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(parent, ParentSubclassProc, (DWORD_PTR)tree);
    }

    PostMessageW(tree, WM_USER_CLEAN_TREE, 0, 0);
}

static void FindAndAttachTreesRecursive(HWND hwnd) {
    if (!IsWindow(hwnd)) return;

    if (IsExplorerNavigationTree(hwnd)) {
        if (g_seenTreeWindows.insert(hwnd).second) {
            AttachTreeHooks(hwnd);
        }
    }

    for (HWND child = GetWindow(hwnd, GW_CHILD); child; child = GetWindow(child, GW_HWNDNEXT)) {
        FindAndAttachTreesRecursive(child);
    }
}

static BOOL CALLBACK EnumTopWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != g_currentPid) return TRUE;

    std::wstring cls = GetWindowClassString(hwnd);
    if (cls == L"CabinetWClass" || cls == L"ExploreWClass") {
        FindAndAttachTreesRecursive(hwnd);
    }
    return TRUE;
}

static void ScanExplorerWindows() {
    EnumWindows(EnumTopWindowsProc, 0);
}

static HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {

    HWND hwnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hwnd && IsExplorerNavigationTree(hwnd)) {
        if (g_seenTreeWindows.insert(hwnd).second) {
            AttachTreeHooks(hwnd);
        }
    }
    return hwnd;
}

static HWND WINAPI CreateWindowExA_Hook(
    DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {

    HWND hwnd = CreateWindowExA_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hwnd && IsExplorerNavigationTree(hwnd)) {
        if (g_seenTreeWindows.insert(hwnd).second) {
            AttachTreeHooks(hwnd);
        }
    }
    return hwnd;
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);

    EnumWindows([](HWND hwnd, LPARAM) -> BOOL {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == g_currentPid) {
            std::wstring cls = GetWindowClassString(hwnd);
            if (cls == L"CabinetWClass" || cls == L"ExploreWClass") {
                PostMessageW(hwnd, WM_COMMAND, 41504, 0);
                PostMessageW(hwnd, WM_COMMAND, 41028, 0);
            }
        }
        return TRUE;
    }, 0);

    for (HWND tree : g_attachedTreeWindows) {
        if (IsWindow(tree)) {
            PostMessageW(tree, WM_USER_CLEAN_TREE, 0, 0);
            RedrawWindow(tree, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
        }
    }
}

BOOL Wh_ModInit() {
    g_currentPid = GetCurrentProcessId();

    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_TREEVIEW_CLASSES };
    InitCommonControlsEx(&icc);

    LoadSettings();

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original);
    Wh_SetFunctionHook((void*)CreateWindowExA, (void*)CreateWindowExA_Hook, (void**)&CreateWindowExA_Original);

    Wh_Log(L"Nav Customizer initialized.");
    return TRUE;
}

void Wh_ModAfterInit() {
    ScanExplorerWindows();
}

void Wh_ModBeforeUninit() {
    Settings defaultSettings = {};
    defaultSettings.hideOneDrive = false;
    defaultSettings.hideHome = false;
    defaultSettings.hideGallery = false;
    defaultSettings.hideThisPC = false;
    defaultSettings.hideNetwork = false;
    SyncRegistryKeys(defaultSettings);
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);

    for (HWND tree : g_attachedTreeWindows) {
        if (IsWindow(tree)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(tree, TreeSubclassProc);
        }
    }
    for (HWND parent : g_attachedParentWindows) {
        if (IsWindow(parent)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(parent, ParentSubclassProc);
        }
    }
    g_attachedTreeWindows.clear();
    g_attachedParentWindows.clear();
    g_seenTreeWindows.clear();
}
