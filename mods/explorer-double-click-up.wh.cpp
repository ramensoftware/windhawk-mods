// ==WindhawkMod==
// @id              explorer-double-click-up
// @name            Explorer Double Click Up
// @description     Double click empty space to go up a folder
// @version         1.0.1
// @author          wrldspawn
// @github          https://github.com/wrldspawn
// @include         explorer.exe
// @compilerOptions -lcomctl32 -loleaut32 -lole32
// @architecture    x86-64
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// The COMInitializer class was taken from the mod "Click on empty taskbar space", which is licensed under GPL-3.0
// Usage of UIAutomation was also referenced from "Click on empty taskbar space"
// FileCabinet_CreateViewWindow2 hook and ExplorerWrapper referenced/modified from "Classic Explorer Treeview"
// Going up a directory referenced from Open-Shell

// ==WindhawkModReadme==
/*
# Explorer Double Click Up

Double click empty space to go up a folder

## Windows version support

Will not work on anything older than Windows 10 due to use of WinRT.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
// ==/WindhawkModSettings==

#include <initguid.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <windowsx.h>
#include <shdeprecated.h>

#include <commctrl.h>
#include <UIAnimation.h>
#include <UIAutomationClient.h>
#include <UIAutomationCore.h>
#include <comutil.h>
#include <winrt/base.h>

#include <vector>

using bstr_ptr = _bstr_t;

class ExplorerWrapper {
    winrt::com_ptr<IShellBrowser> hBrowser;

public:
    HWND hShellTab = NULL;
    HWND hListView = NULL;

    ExplorerWrapper(HWND hWnd, IShellBrowser* hShellBrowser) {
        hShellTab = hWnd;
        hBrowser.copy_from(hShellBrowser);
    }

    void GoUp() {
        hBrowser->BrowseObject(NULL, SBSP_SAMEBROWSER | SBSP_PARENT);
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

LRESULT CALLBACK SysListViewSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_LBUTTONDBLCLK) {
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(hWnd, &mousePos);

        LVHITTESTINFO hitTestInfo;
        hitTestInfo.flags = LVHT_NOWHERE;
        hitTestInfo.pt.x = mousePos.x;
        hitTestInfo.pt.y = mousePos.y;

        // FIXME: Ignores group headers
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

                for (ExplorerWrapper& wrapper: g_Wrappers) {
                    if (wrapper.hShellTab == parent) {
                        found = true;
                        wrapper.GoUp();
                        break;
                    }
                }
                if (found) break;
            }
        }
    };

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

struct ClickHelper {
    DWORD time = 0;
    wchar_t className[256];
    HWND hWnd;
};

ClickHelper currentClick;
ClickHelper lastClick;

LRESULT CALLBACK DUISubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_PARENTNOTIFY && wParam == WM_LBUTTONDOWN) {
        DWORD now = GetTickCount();
        currentClick.time = now;
        currentClick.hWnd = hWnd;

        POINT mousePos;
        GetCursorPos(&mousePos);

        winrt::com_ptr<IUIAutomationElement> pElement = NULL;
        if (SUCCEEDED(g_pUIAutomation->ElementFromPoint(mousePos, pElement.put())) && pElement) {
            bstr_ptr _className;
            if (SUCCEEDED(pElement->get_CurrentClassName(_className.GetAddress()))) {
                wchar_t* className = _className.GetBSTR();
                wcsncpy(currentClick.className, className, 256);

                DWORD delta = currentClick.time - lastClick.time;

                if (
                    currentClick.hWnd == lastClick.hWnd &&
                    (
                        (wcscmp(className, L"UIGroupItem") == 0 && wcscmp(lastClick.className, L"UIGroupItem") == 0) ||
                        (wcscmp(className, L"UIItemsView") == 0 && wcscmp(lastClick.className, L"UIItemsView") == 0)
                    ) && 
                    delta <= GetDoubleClickTime()
                ) {
                    bool found = false;
                    HWND parent = GetParent(hWnd);
                    while (parent != NULL) {
                        wchar_t className[256];
                        GetClassName(parent, className, 256);

                        if (wcscmp(className, L"ShellTabWindowClass") != 0) {
                            parent = GetParent(parent);
                            continue;
                        }

                        for (ExplorerWrapper& wrapper: g_Wrappers) {
                            if (wrapper.hShellTab == parent) {
                                found = true;
                                wrapper.GoUp();
                                break;
                            }
                        }
                        if (found) break;
                    }
                }

                lastClick.time = now;
                lastClick.hWnd = hWnd;
                wcsncpy(lastClick.className, className, 256);
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
        
        HWND parent = GetParent(hWnd);
        if (parent != NULL) {
            wchar_t parentClassName[256];
            GetClassName(parent, parentClassName, 256);

            HWND parentParent = GetParent(parent);
            if (parentParent != NULL) {
                wchar_t parentParentClassName[256];
                GetClassName(parentParent, parentParentClassName, 256);

                if (wcscmp(parentParentClassName, L"ShellTabWindowClass") == 0) {
                    if (wcscmp(className, L"SysListView32") == 0) {
                        if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, SysListViewSubclass, 0)) {
                            Wh_Log(L"SysListView32 Subclassed %p", hWnd);
                            for (ExplorerWrapper& wrapper: g_Wrappers) {
                                if (wrapper.hShellTab == parentParent) {
                                    wrapper.hListView = hWnd;
                                    break;
                                }
                            }
                        }
                    } else if (wcscmp(className, L"DirectUIHWND") == 0 && wcscmp(parentClassName, L"SHELLDLL_DefView") == 0) {
                        if (WindhawkUtils::SetWindowSubclassFromAnyThread(parent, DUISubclass, 0)) {
                            Wh_Log(L"DirectUIHWND Subclassed %p", parent);
                            for (ExplorerWrapper& wrapper: g_Wrappers) {
                                if (wrapper.hShellTab == parentParent) {
                                    wrapper.hListView = parent;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return hWnd;
}

typedef HRESULT (*__cdecl FileCabinet_CreateViewWindow2_t)(IShellBrowser*, void*, IShellView*, IShellView*, void*, HWND*);
FileCabinet_CreateViewWindow2_t FileCabinet_CreateViewWindow2Original;
HRESULT __cdecl FileCabinet_CreateViewWindow2Hook(IShellBrowser* pBrowser, void* var1, IShellView* psv1, IShellView* psv2, void* var2, HWND* hWnd) {\
    HRESULT hRes = FileCabinet_CreateViewWindow2Original(pBrowser, var1, psv1, psv2, var2, hWnd);

    HWND hShellTab = GetParent(*hWnd);
    if (hShellTab != NULL)
        g_Wrappers.push_back(ExplorerWrapper(hShellTab, pBrowser));

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
    Wh_Log(L"Explorer Double Click Up Init");

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
    for (ExplorerWrapper& wrapper: g_Wrappers) {
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
