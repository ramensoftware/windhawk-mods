// ==WindhawkMod==
// @id              explorer-middle-click-duplicate
// @name            Explorer Middle Click Duplicate Tab
// @description     Middle click empty space to duplicate the current tab in a new tab
// @version         1.0.0
// @author          LiHua81
// @github          https://github.com/LiHua81
// @include         explorer.exe
// @compilerOptions -lcomctl32 -loleaut32 -lole32 -lshlwapi
// @architecture    x86-64
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Based on "Explorer Double Click Up" by wrldspawn (GPL-3.0).

// ==WindhawkModReadme==
/*
# Explorer Middle Click Duplicate Tab

Middle click on empty space in File Explorer to open the current folder in a new tab.

## Windows version support

Requires Windows 11 for tabbed Explorer support.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
// ==/WindhawkModSettings==

#include <initguid.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <windowsx.h>
#include <shdeprecated.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shobjidl.h>

#include <commctrl.h>
#include <UIAnimation.h>
#include <UIAutomationClient.h>
#include <UIAutomationCore.h>
#include <comutil.h>
#include <winrt/base.h>

#include <vector>

using bstr_ptr = _bstr_t;

static wchar_t g_pendingNavPath[MAX_PATH] = {};
static winrt::com_ptr<IShellBrowser> g_pendingNavBrowser;
static HWND g_pendingNavHwnd = NULL;

static VOID CALLBACK NavigateNewTabProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

class ExplorerWrapper {
    winrt::com_ptr<IShellBrowser> hBrowser;

public:
    HWND hShellTab = NULL;
    HWND hListView = NULL;

    ExplorerWrapper(HWND hWnd, IShellBrowser* hShellBrowser) {
        hShellTab = hWnd;
        hBrowser.copy_from(hShellBrowser);
    }

    void DuplicateTab() {
        Wh_Log(L"DuplicateTab called");

        IShellView* psv = nullptr;
        if (FAILED(hBrowser->QueryActiveShellView(&psv)) || !psv) {
            Wh_Log(L"QueryActiveShellView failed");
            return;
        }

        IFolderView* pfv = nullptr;
        if (FAILED(psv->QueryInterface(IID_PPV_ARGS(&pfv)))) {
            Wh_Log(L"QueryInterface IFolderView failed");
            psv->Release();
            return;
        }

        IPersistIDList* pidl_list = nullptr;
        if (FAILED(pfv->GetFolder(IID_PPV_ARGS(&pidl_list)))) {
            Wh_Log(L"GetFolder IPersistIDList failed");
            pfv->Release();
            psv->Release();
            return;
        }

        PIDLIST_ABSOLUTE pidl = nullptr;
        if (SUCCEEDED(pidl_list->GetIDList(&pidl)) && pidl) {
            wchar_t path[MAX_PATH] = {};
            if (SHGetPathFromIDListW(pidl, path) && path[0]) {
                Wh_Log(L"Duplicating tab to: %s", path);

                wcsncpy(g_pendingNavPath, path, MAX_PATH - 1);
                g_pendingNavPath[MAX_PATH - 1] = L'\0';
                g_pendingNavBrowser = nullptr;
                g_pendingNavHwnd = hShellTab;

                INPUT inputs[4] = {};
                inputs[0].type = INPUT_KEYBOARD;
                inputs[0].ki.wVk = VK_CONTROL;
                inputs[1].type = INPUT_KEYBOARD;
                inputs[1].ki.wVk = 'T';
                inputs[2].type = INPUT_KEYBOARD;
                inputs[2].ki.wVk = 'T';
                inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
                inputs[3].type = INPUT_KEYBOARD;
                inputs[3].ki.wVk = VK_CONTROL;
                inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(4, inputs, sizeof(INPUT));
                Wh_Log(L"Sent Ctrl+T");

                SetTimer(hShellTab, 0x4D43, 500, NavigateNewTabProc);
            } else {
                Wh_Log(L"SHGetPathFromIDListW failed or empty path");
            }
            CoTaskMemFree(pidl);
        } else {
            Wh_Log(L"GetIDList failed or null pidl");
        }

        pidl_list->Release();
        pfv->Release();
        psv->Release();
    }
};

class COMInitializer
{
public:
    COMInitializer() : initialized(false) {}

    bool Init()
    {
        if (!initialized)
        {
            initialized = SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED));
        }
        return initialized;
    }

    void Uninit()
    {
        if (initialized)
        {
            CoUninitialize();
            initialized = false;
            Wh_Log(L"COM de-initialized");
        }
    }

    ~COMInitializer()
    {
        Uninit();
    }

    bool IsInitialized() { return initialized; }

protected:
    bool initialized;
} g_comInitializer;

std::vector<ExplorerWrapper> g_Wrappers;
static winrt::com_ptr<IUIAutomation> g_pUIAutomation;

static VOID CALLBACK NavigateNewTabProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    KillTimer(hwnd, 0x4D43);
    Wh_Log(L"NavigateNewTabProc fired, browser=%p path=%s", (void*)g_pendingNavBrowser.get(), g_pendingNavPath);

    if (!g_pendingNavPath[0] || !g_pendingNavBrowser) {
        Wh_Log(L"NavigateNewTabProc: no pending nav");
        g_pendingNavPath[0] = L'\0';
        g_pendingNavBrowser = nullptr;
        g_pendingNavHwnd = NULL;
        return;
    }

    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(SHParseDisplayName(g_pendingNavPath, NULL, &pidl, 0, NULL)) && pidl) {
        if (SUCCEEDED(g_pendingNavBrowser->BrowseObject(pidl, SBSP_SAMEBROWSER | SBSP_ABSOLUTE))) {
            Wh_Log(L"NavigateNewTabProc: navigated to %s", g_pendingNavPath);
        } else {
            Wh_Log(L"NavigateNewTabProc: BrowseObject failed");
        }
        CoTaskMemFree(pidl);
    }

    g_pendingNavPath[0] = L'\0';
    g_pendingNavBrowser = nullptr;
    g_pendingNavHwnd = NULL;
}

LRESULT CALLBACK SysListViewSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_MBUTTONDOWN) {
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(hWnd, &mousePos);

        LVHITTESTINFO hitTestInfo;
        hitTestInfo.flags = LVHT_NOWHERE;
        hitTestInfo.pt.x = mousePos.x;
        hitTestInfo.pt.y = mousePos.y;

        int hitRes = ListView_SubItemHitTest(hWnd, &hitTestInfo);
        if (hitRes == -1) {
            bool found = false;
            HWND parent = GetParent(hWnd);
            while (parent != NULL) {
                wchar_t className[256];
                GetClassName(parent, className, 256);

                if (wcscmp(className, L"ShellTabWindowClass") != 0) {
                    parent = GetParent(parent);
                    continue;
                }

                for (ExplorerWrapper& wrapper : g_Wrappers) {
                    if (wrapper.hShellTab == parent) {
                        found = true;
                        wrapper.DuplicateTab();
                        break;
                    }
                }
                if (found) break;
            }
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK DUISubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_PARENTNOTIFY && wParam == WM_MBUTTONDOWN) {
        POINT mousePos;
        GetCursorPos(&mousePos);

        winrt::com_ptr<IUIAutomationElement> pElement = NULL;
        if (SUCCEEDED(g_pUIAutomation->ElementFromPoint(mousePos, pElement.put())) && pElement) {
            bstr_ptr _className;
            if (SUCCEEDED(pElement->get_CurrentClassName(_className.GetAddress()))) {
                wchar_t* className = _className.GetBSTR();

                if (wcscmp(className, L"UIGroupItem") == 0 || wcscmp(className, L"UIItemsView") == 0) {
                    bool found = false;
                    HWND parent = GetParent(hWnd);
                    while (parent != NULL) {
                        wchar_t parentClassName[256];
                        GetClassName(parent, parentClassName, 256);

                        if (wcscmp(parentClassName, L"ShellTabWindowClass") != 0) {
                            parent = GetParent(parent);
                            continue;
                        }

                        for (ExplorerWrapper& wrapper : g_Wrappers) {
                            if (wrapper.hShellTab == parent) {
                                found = true;
                                wrapper.DuplicateTab();
                                break;
                            }
                        }
                        if (found) break;
                    }
                }
            }
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_original;
HWND WINAPI CreateWindowExW_hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hWnd != NULL) {
        wchar_t className[256];
        GetClassName(hWnd, className, 256);

        if (wcscmp(className, L"SysListView32") == 0 || wcscmp(className, L"DirectUIHWND") == 0) {
            HWND shellTab = NULL;
            HWND defView = NULL;
            HWND p = GetParent(hWnd);
            while (p != NULL) {
                wchar_t pc[256];
                GetClassName(p, pc, 256);
                if (wcscmp(pc, L"SHELLDLL_DefView") == 0) {
                    defView = p;
                }
                if (wcscmp(pc, L"ShellTabWindowClass") == 0) {
                    shellTab = p;
                    break;
                }
                p = GetParent(p);
            }

            if (shellTab && defView) {
                if (wcscmp(className, L"SysListView32") == 0) {
                    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, SysListViewSubclass, 0)) {
                        Wh_Log(L"SysListView32 Subclassed %p", hWnd);
                        for (ExplorerWrapper& wrapper : g_Wrappers) {
                            if (wrapper.hShellTab == shellTab) {
                                wrapper.hListView = hWnd;
                                break;
                            }
                        }
                    }
                } else if (wcscmp(className, L"DirectUIHWND") == 0) {
                    if (WindhawkUtils::SetWindowSubclassFromAnyThread(defView, DUISubclass, 0)) {
                        Wh_Log(L"DUI Subclassed via DefView %p for shellTab %p", defView, shellTab);
                        for (ExplorerWrapper& wrapper : g_Wrappers) {
                            if (wrapper.hShellTab == shellTab) {
                                wrapper.hListView = defView;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return hWnd;
}

typedef HRESULT(*__cdecl FileCabinet_CreateViewWindow2_t)(IShellBrowser*, void*, IShellView*, IShellView*, void*, HWND*);
FileCabinet_CreateViewWindow2_t FileCabinet_CreateViewWindow2Original;
HRESULT __cdecl FileCabinet_CreateViewWindow2Hook(IShellBrowser* pBrowser, void* var1, IShellView* psv1, IShellView* psv2, void* var2, HWND* hWnd) {
    HRESULT hRes = FileCabinet_CreateViewWindow2Original(pBrowser, var1, psv1, psv2, var2, hWnd);

    Wh_Log(L"FileCabinet_CreateViewWindow2Hook: pBrowser=%p", pBrowser);

    HWND hShellTab = GetParent(*hWnd);
    if (hShellTab != NULL) {
        g_Wrappers.push_back(ExplorerWrapper(hShellTab, pBrowser));

        if (g_pendingNavPath[0] && !g_pendingNavBrowser) {
            g_pendingNavBrowser.copy_from(pBrowser);
            Wh_Log(L"FileCabinet hook: captured new tab browser=%p for path=%s", pBrowser, g_pendingNavPath);
        }
    }

    return hRes;
}

BOOL CALLBACK InitEnumChildWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        wchar_t className[256];
        GetClassName(hWnd, className, 256);

        if (wcscmp(className, L"SHELLDLL_DefView") == 0) {
            HWND shellTab = (HWND)lParam;

            auto browser = winrt::com_ptr<IShellBrowser>{
                reinterpret_cast<IShellBrowser*>((void*)SendMessage(shellTab, WM_USER + 7, 0, 0)),
                winrt::take_ownership_from_abi
            };
            if (browser != NULL) {
                ExplorerWrapper wrapper = ExplorerWrapper(shellTab, browser.get());

                HWND listView = FindWindowEx(hWnd, NULL, L"SysListView32", NULL);
                HWND dui = FindWindowEx(hWnd, NULL, L"DirectUIHWND", NULL);
                if (listView) {
                    if (WindhawkUtils::SetWindowSubclassFromAnyThread(listView, SysListViewSubclass, 0)) {
                        Wh_Log(L"SysListView32 Subclassed %p", listView);
                        wrapper.hListView = listView;
                    }
                } else if (dui) {
                    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DUISubclass, 0)) {
                        Wh_Log(L"DirectUIHWND Subclassed %p", hWnd);
                        wrapper.hListView = hWnd;
                    }
                }

                if (wrapper.hListView) {
                    g_Wrappers.push_back(wrapper);
                } else {
                    Wh_Log(L"Failed to setup wrapper for %p", shellTab);
                }

                return FALSE;
            }
        }
    }

    return TRUE;
}

BOOL CALLBACK InitEnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        wchar_t className[256];
        GetClassName(hWnd, className, 256);

        if (wcscmp(className, L"CabinetWClass") == 0) {
            HWND shellTab = FindWindowEx(hWnd, NULL, L"ShellTabWindowClass", NULL);
            if (shellTab != NULL) {
                EnumChildWindows(shellTab, InitEnumChildWindowsProc, (LPARAM)shellTab);
            }
        }
    }

    return TRUE;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Explorer Middle Click Duplicate Tab Init");

    HMODULE hExplorerFrame = LoadLibraryExW(L"explorerframe.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (!hExplorerFrame) {
        Wh_Log(L"Failed to load explorerframe.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK explorerframe_dll_hooks[] = {
        {
            {
                L"long __cdecl FileCabinet_CreateViewWindow2(struct IShellBrowser *,struct tagFolderSetDataBase *,struct IShellView *,struct IShellView *,struct tagRECT *,struct HWND__ * *)"
            },
            (void**)&FileCabinet_CreateViewWindow2Original,
            (void*)FileCabinet_CreateViewWindow2Hook,
            FALSE
        }
    };
    if (!WindhawkUtils::HookSymbols(hExplorerFrame, explorerframe_dll_hooks, ARRAYSIZE(explorerframe_dll_hooks))) {
        Wh_Log(L"Failed to hook ExplorerFrame.dll");
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(
        (void*)CreateWindowExW,
        (void*)CreateWindowExW_hook,
        (void**)&CreateWindowExW_original
    );

    if (!g_comInitializer.Init()) {
        Wh_Log(L"COM initialization failed");
        return FALSE;
    }
    if (
        FAILED(CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), g_pUIAutomation.put_void())) ||
        !g_pUIAutomation
    ) {
        Wh_Log(L"Failed to create UIAutomation COM instance");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(InitEnumWindowsProc, 0);
}

void Wh_ModUninit() {
    if (g_pendingNavHwnd) {
        KillTimer(g_pendingNavHwnd, 0x4D43);
        g_pendingNavHwnd = NULL;
    }
    g_pendingNavBrowser = nullptr;
    g_pendingNavPath[0] = L'\0';

    for (ExplorerWrapper& wrapper : g_Wrappers) {
        HWND hWnd = wrapper.hListView;
        if (hWnd) {
            wchar_t className[256];
            GetClassName(hWnd, className, 256);

            if (wcscmp(className, L"SysListView32") == 0) {
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, SysListViewSubclass);
                Wh_Log(L"SysListView32 Unsubclassed %p", hWnd);
            } else if (wcscmp(className, L"SHELLDLL_DefView") == 0) {
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, DUISubclass);
                Wh_Log(L"DirectUIHWND Unsubclassed %p", hWnd);
            }
        }
    }
}
