// ==WindhawkMod==
// @id              fix-windows-tools-view
// @name            Fix Windows Tools Folder View
// @description     Saves/restores view mode for Windows Tools folder
// @version         1.0
// @author          Anixx
// @github       https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -lole32 -luuid -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod makes the `Windows Tools` (formerly `Administrative Tools`) folder to save and restore its view properties, like any other folder
instead of using the hardcoded tile view.
Particularly, this works around the issue that in the default, tile mode the elements of this folders have other content
instead of names displayed when using SysListView32.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <exdisp.h>
#include <unordered_map>
#include <set>

static const CLSID CLSID_WindowsTools =
    {0x4234D49B, 0x0245, 0x4DF3,
     {0xB7, 0x80, 0x38, 0x93, 0x94, 0x34, 0x56, 0xE1}};

struct FolderInfo {
    int bagNumber = -1;
    IFolderView2* pFolderView = nullptr;
};

std::unordered_map<HWND, FolderInfo> g_windows;
std::set<HWND> g_subclassedDefViews;
HWND g_lastCreatedCabinet = NULL;

typedef HWND (WINAPI *CREATEWINDOWEXW)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
typedef LONG (WINAPI *REGQUERYVALUEEXW)(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);

CREATEWINDOWEXW pOriginalCreateWindowExW;
REGQUERYVALUEEXW pOriginalRegQueryValueExW;

LRESULT CALLBACK DefViewSubclassProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

int ExtractBagNumber(HKEY hKey) {
    typedef LONG (WINAPI *NTQUERYKEY)(HANDLE, int, PVOID, ULONG, PULONG);
    static NTQUERYKEY pNtQueryKey = (NTQUERYKEY)GetProcAddress(
        GetModuleHandleW(L"ntdll.dll"), "NtQueryKey");
    if (!pNtQueryKey) return -1;

    BYTE buffer[1024];
    ULONG resultLength;
    if (pNtQueryKey(hKey, 3, buffer, sizeof(buffer), &resultLength) != 0)
        return -1;

    WCHAR* keyPath = (WCHAR*)(buffer + sizeof(ULONG));
    const WCHAR* bags = wcsstr(keyPath, L"\\Bags\\");
    if (!bags) return -1;
    bags += 6;
    if (wcsstr(bags, L"AllFolders") == bags) return -1;

    return _wtoi(bags);
}

IFolderView2* GetFolderViewFromCabinet(HWND hwndCabinet) {
    IFolderView2* result = nullptr;
    IShellWindows* psw = nullptr;
    
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_LOCAL_SERVER,
                                 IID_IShellWindows, (void**)&psw)) || !psw) return nullptr;
    
    long count = 0;
    psw->get_Count(&count);
    
    for (long i = 0; i < count && !result; i++) {
        VARIANT vi = {VT_I4};
        vi.lVal = i;
        
        IDispatch* pdisp = nullptr;
        if (FAILED(psw->Item(vi, &pdisp)) || !pdisp) continue;
        
        IWebBrowser2* pwb = nullptr;
        if (SUCCEEDED(pdisp->QueryInterface(IID_IWebBrowser2, (void**)&pwb)) && pwb) {
            HWND hwnd = NULL;
            pwb->get_HWND((SHANDLE_PTR*)&hwnd);
            
            if (hwnd == hwndCabinet) {
                IServiceProvider* psp = nullptr;
                if (SUCCEEDED(pwb->QueryInterface(IID_IServiceProvider, (void**)&psp)) && psp) {
                    IShellBrowser* psb = nullptr;
                    if (SUCCEEDED(psp->QueryService(SID_STopLevelBrowser, IID_IShellBrowser, (void**)&psb)) && psb) {
                        IShellView* psv = nullptr;
                        if (SUCCEEDED(psb->QueryActiveShellView(&psv)) && psv) {
                            psv->QueryInterface(IID_IFolderView2, (void**)&result);
                            psv->Release();
                        }
                        psb->Release();
                    }
                    psp->Release();
                }
            }
            pwb->Release();
        }
        pdisp->Release();
    }
    psw->Release();
    return result;
}

bool IsWindowsToolsFolder(IFolderView2* pfv) {
    bool result = false;
    IPersistFolder2* ppf = nullptr;
    if (SUCCEEDED(pfv->GetFolder(IID_IPersistFolder2, (void**)&ppf)) && ppf) {
        CLSID clsid;
        if (SUCCEEDED(ppf->GetClassID(&clsid)))
            result = IsEqualCLSID(clsid, CLSID_WindowsTools);
        ppf->Release();
    }
    return result;
}

void SaveViewMode(IFolderView2* pfv, int bagNum) {
    UINT viewMode = 0;
    int iconSize = 0;
    if (FAILED(pfv->GetViewModeAndIconSize((FOLDERVIEWMODE*)&viewMode, &iconSize))) return;

    WCHAR regPath[256];
    wsprintfW(regPath, L"Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\Bags\\%d\\Shell", bagNum);

    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, regPath, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        RegSetValueExW(hKey, L"WTViewMode", 0, REG_DWORD, (BYTE*)&viewMode, sizeof(viewMode));
        RegSetValueExW(hKey, L"WTIconSize", 0, REG_DWORD, (BYTE*)&iconSize, sizeof(iconSize));
        RegCloseKey(hKey);
    }
}

BOOL ApplyViewMode(IFolderView2* pfv, int bagNum) {
    WCHAR regPath[256];
    wsprintfW(regPath, L"Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\Bags\\%d\\Shell", bagNum);

    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, regPath, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return FALSE;

    DWORD viewMode, iconSize, dataSize = sizeof(DWORD);
    BOOL result = FALSE;

    if (RegQueryValueExW(hKey, L"WTViewMode", NULL, NULL, (BYTE*)&viewMode, &dataSize) == ERROR_SUCCESS) {
        dataSize = sizeof(DWORD);
        if (RegQueryValueExW(hKey, L"WTIconSize", NULL, NULL, (BYTE*)&iconSize, &dataSize) == ERROR_SUCCESS)
            result = SUCCEEDED(pfv->SetViewModeAndIconSize((FOLDERVIEWMODE)viewMode, iconSize));
    }
    RegCloseKey(hKey);
    return result;
}

LRESULT CALLBACK DefViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                      LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    HWND hwndCabinet = GetAncestor(hWnd, GA_ROOT);
    
    if (uMsg == WM_SHOWWINDOW && wParam == TRUE) {
        IFolderView2* pfv = GetFolderViewFromCabinet(hwndCabinet);
        if (pfv) {
            if (IsWindowsToolsFolder(pfv)) {
                auto it = g_windows.find(hwndCabinet);
                if (it != g_windows.end()) {
                    it->second.pFolderView = pfv;
                    pfv->AddRef();
                    
                    if (it->second.bagNumber > 0 && !ApplyViewMode(pfv, it->second.bagNumber)) {
                        UINT viewMode = 0;
                        if (SUCCEEDED(pfv->GetCurrentViewMode(&viewMode)) && viewMode == FVM_TILE)
                            pfv->SetCurrentViewMode(FVM_LIST);
                    }
                }
            } else {
                pfv->Release();
                g_subclassedDefViews.erase(hWnd);
                RemoveWindowSubclass(hWnd, DefViewSubclassProc, uIdSubclass);
                return DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }
            pfv->Release();
        }
    }
    else if (uMsg == WM_DESTROY) {
        auto it = g_windows.find(hwndCabinet);
        if (it != g_windows.end() && it->second.pFolderView && it->second.bagNumber > 0) {
            SaveViewMode(it->second.pFolderView, it->second.bagNumber);
            it->second.pFolderView->Release();
            it->second.pFolderView = nullptr;
        }
        g_subclassedDefViews.erase(hWnd);
        RemoveWindowSubclass(hWnd, DefViewSubclassProc, uIdSubclass);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

HWND WINAPI CreateWindowExWHook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                 DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                 HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = pOriginalCreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                          X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd || !lpClassName || ((ULONG_PTR)lpClassName <= 0xFFFF)) return hWnd;

    if (wcscmp(lpClassName, L"CabinetWClass") == 0) {
        g_lastCreatedCabinet = hWnd;
        g_windows[hWnd] = {};
    }
    else if (wcscmp(lpClassName, L"SHELLDLL_DefView") == 0) {
        SetWindowSubclass(hWnd, DefViewSubclassProc, 0, 0);
        g_subclassedDefViews.insert(hWnd);
    }

    return hWnd;
}

LONG WINAPI RegQueryValueExWHook(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved,
                                  LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    LONG result = pOriginalRegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);

    if (g_lastCreatedCabinet && IsWindow(g_lastCreatedCabinet)) {
        int bagNum = ExtractBagNumber(hKey);
        if (bagNum > 0) {
            auto it = g_windows.find(g_lastCreatedCabinet);
            if (it != g_windows.end() && it->second.bagNumber == -1)
                it->second.bagNumber = bagNum;
        }
    }
    return result;
}

BOOL Wh_ModInit(void) {
    Wh_SetFunctionHook((void*)GetProcAddress(LoadLibraryW(L"kernelbase.dll"), "RegQueryValueExW"),
                       (void*)RegQueryValueExWHook, (void**)&pOriginalRegQueryValueExW);
    Wh_SetFunctionHook((void*)CreateWindowExW,
                       (void*)CreateWindowExWHook, (void**)&pOriginalCreateWindowExW);
    return TRUE;
}

void Wh_ModUninit(void) {
    for (HWND hwnd : g_subclassedDefViews) {
        if (IsWindow(hwnd))
            RemoveWindowSubclass(hwnd, DefViewSubclassProc, 0);
    }
    g_subclassedDefViews.clear();
    
    for (auto& pair : g_windows) {
        if (pair.second.pFolderView)
            pair.second.pFolderView->Release();
    }
    g_windows.clear();
}
