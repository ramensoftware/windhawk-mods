// ==WindhawkMod==
// @id              classic-theme-explorer
// @name            Classic Theme Explorer
// @description     Classic Theme mitigations for Explorer as a Windhawk mod
// @version         1.0
// @author          Cynosphere
// @github          https://github.com/Cynosphere
// @homepage        https://c7.pm/
// @include         explorer.exe
// @compilerOptions -lcomctl32 -luxtheme -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*...*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*...*/
// ==/WindhawkModSettings==

#include <minwindef.h>
#include <windows.h>
#include <objbase.h>
#include <initguid.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <wingdi.h>

#include <string>
#include <regex>
#include <locale>
#include <codecvt>

struct {
    bool clientEdge;
    bool rebarBorder;
    bool rebarFixedHeight;
    bool hideNavigation;
    bool addressSync;
    bool addressSyncFullPath;
    bool addressHeight;
    bool hideRefresh;
    bool epMitigate;
} settings;

void LoadSettings() {
    settings.clientEdge = Wh_GetIntSetting(L"clientEdge");
    settings.rebarBorder = Wh_GetIntSetting(L"rebarBorder");
    settings.rebarFixedHeight = Wh_GetIntSetting(L"rebarFixedHeight");
    settings.hideNavigation = Wh_GetIntSetting(L"hideNavigation");
    settings.addressSync = Wh_GetIntSetting(L"addressSync");
    settings.addressSyncFullPath = Wh_GetIntSetting(L"addressSyncFullPath");
    settings.addressHeight = Wh_GetIntSetting(L"addressHeight");
    settings.hideRefresh = Wh_GetIntSetting(L"hideRefresh");
    settings.epMitigate = Wh_GetIntSetting(L"epMitigate");
}

// Explorer modifications
typedef struct EnumChildProcData
{
    HWND result;
    int index;
    int instancesFound;
    const wchar_t* className;
};
BOOL CALLBACK EnumChildProc(_In_ HWND hwnd, _In_ LPARAM lParam)
{
    wchar_t buffer[256];
    EnumChildProcData* data = (EnumChildProcData*)lParam;
    GetClassNameW(hwnd, buffer, 256);
    if (lstrcmpW(buffer, data->className) == 0)
    {
        if (data->instancesFound + 1 == data->index)
        {
            data->result = hwnd;
            return FALSE;
        }
        else
        {
            data->instancesFound++;
        }
    }
    return TRUE;
};
HWND GetChildWindow(HWND parent, const wchar_t* className, int index)
{
    EnumChildProcData data = { 0 };
    data.className = className;
    data.index = index;
    EnumChildWindows(parent, EnumChildProc, (LPARAM)&data);
    return data.result;
}

// QtTabBar's hooking taints the wParam for some reason, so we strip off the extra bits if needed.
BOOL ShouldApply(WPARAM wParam, BOOL fallback, BOOL checkSix) {
    Wh_Log(L"0x%x, %d", wParam, fallback);
    if (wParam > 0xffff) {
        Wh_Log(L"above 0xffff");
        if ((wParam & 1) == 1) {
            Wh_Log(L"1 set");
            return true;
        } else if (checkSix && (wParam & 6) == 6) {
            Wh_Log(L"6 set");
            return true;
        } else {
            Wh_Log(L"falling back");
            return fallback;
        }
    } else {
        Wh_Log(L"falling back");
        return fallback;
    }
}

VOID ClassicThemeExplorer(HWND hWnd, UINT uMsg, WPARAM wParam)
{
    wchar_t classNameBuffer[256];
    wchar_t pathBuffer[MAX_PATH];
    HKEY key;
    RECT rect;

    HWND NavBarParent = GetChildWindow((HWND)hWnd, L"WorkerW", 1);
    HWND NavBarAddressBandRoot = GetChildWindow(NavBarParent, L"Address Band Root", 1);
    HWND NavBarToolBar = GetChildWindow(NavBarAddressBandRoot, L"ToolbarWindow32", 2);
    HWND ClassicAddressBandRoot = GetChildWindow((HWND)hWnd, L"Address Band Root", 2);
    HWND ClassicProgressControl = GetChildWindow(ClassicAddressBandRoot, L"msctls_progress32", 1);
    HWND ClassicComboBox = GetChildWindow(ClassicAddressBandRoot, L"ComboBoxEx32", 1);
    HWND ClassicRebar = GetChildWindow((HWND)hWnd, L"ReBarWindow32", 2);

    BOOL shouldApply = uMsg == WM_PARENTNOTIFY && ShouldApply(wParam, wParam == 1 || wParam == 6, false);
    BOOL shouldApply2 = uMsg == WM_PARENTNOTIFY && ShouldApply(wParam, wParam == 1, true);

    if (settings.clientEdge && shouldApply) {
        Wh_Log(L"applying client edge");
        HWND TreeView = GetChildWindow((HWND)hWnd, L"SysTreeView32", 1);
        LONG TreeViewExtendedStyle = GetWindowLongPtrW(TreeView, GWL_EXSTYLE);
        TreeViewExtendedStyle |= WS_EX_CLIENTEDGE;
        SetWindowLongPtrW(TreeView, GWL_EXSTYLE, TreeViewExtendedStyle);

        HWND FolderView = GetChildWindow((HWND)hWnd, L"FolderView", 1);
        LONG FolderViewExtendedStyle = GetWindowLongPtrW(FolderView, GWL_EXSTYLE);
        FolderViewExtendedStyle |= WS_EX_CLIENTEDGE;
        SetWindowLongPtrW(FolderView, GWL_EXSTYLE, FolderViewExtendedStyle);

        HWND ShellTabWindowClass = GetChildWindow((HWND)hWnd, L"ShellTabWindowClass", 1);
        GetWindowRect(ShellTabWindowClass, &rect);
        SetWindowPos(ShellTabWindowClass, NULL, NULL, NULL, rect.right - rect.left + 1, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_DEFERERASE);
        SetWindowPos(ShellTabWindowClass, NULL, NULL, NULL, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_DEFERERASE);
    }

    if (settings.rebarBorder && shouldApply) {
        Wh_Log(L"applying rebar border");
        LONG ClassicRebarStyle = GetWindowLongPtrW(ClassicRebar, GWL_STYLE);
        ClassicRebarStyle |= RBS_BANDBORDERS;
        ClassicRebarStyle |= WS_BORDER;
        SetWindowLongPtrW(ClassicRebar, GWL_STYLE, ClassicRebarStyle);
    }

    if (settings.rebarFixedHeight && shouldApply) {
        Wh_Log(L"applying rebar fixed height");
        LONG ClassicRebarStyle = GetWindowLongPtrW(ClassicRebar, GWL_STYLE);
        ClassicRebarStyle &= ~RBS_VARHEIGHT;
        SetWindowLongPtrW(ClassicRebar, GWL_STYLE, ClassicRebarStyle);
    }

    // Apply bar changes
    if ((settings.rebarBorder || settings.rebarFixedHeight) && shouldApply) {
        GetWindowRect(ClassicRebar, &rect);
        SetWindowPos(ClassicRebar, NULL, NULL, NULL, rect.right - rect.left + 1, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_DEFERERASE);
        SetWindowPos(ClassicRebar, NULL, NULL, NULL, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_DEFERERASE);
    }

    if (settings.hideNavigation && shouldApply2) {
        // Save the location of the Rebar
        HWND NavBarRebar = GetChildWindow((HWND)hWnd, L"ReBarWindow32", 1);
        GetWindowRect(NavBarRebar, &rect);
        LONG xRebar = rect.left;
        LONG yRebar = rect.top;
        LONG cxRebar = rect.right - rect.left;
        LONG cyRebar = rect.bottom - rect.top;

        // Destroy the NC area of the rebar
        SendMessageW(NavBarRebar, WM_NCDESTROY, 0, 0);

        // Hide the WorkerW and the Rebar
        ShowWindow(NavBarParent, SW_HIDE);
        ShowWindow(NavBarRebar, SW_HIDE);

        // Save the location of the ShellTabWindowClass
        HWND ShellTabWnd = GetChildWindow((HWND)hWnd, L"ShellTabWindowClass", 1);
        GetWindowRect(NavBarRebar, &rect);
        LONG xTabWnd = rect.left;
        LONG yTabWnd = rect.top;
        LONG cxTabWnd = rect.right - rect.left;
        LONG cyTabWnd = rect.bottom - rect.top;

        // Move ShellTabWindow to (*; yRebar; *; yTabWnd - yRebar + cyTabWnd)
        SetWindowPos(ShellTabWnd, NULL, xTabWnd, yRebar, cxTabWnd, yTabWnd - yRebar + cyTabWnd, SWP_NOZORDER);

        // Move Rebar to (*; *; *; 0)
        SetWindowPos(NavBarRebar, NULL, xRebar, yRebar, cxRebar, 0, SWP_NOZORDER);

        // Resize the window to apply
        GetWindowRect((HWND)hWnd, &rect);
        SetWindowPos((HWND)hWnd, NULL, NULL, NULL, rect.right - rect.left, rect.bottom - rect.top + 1, SWP_NOMOVE | SWP_NOZORDER | SWP_DEFERERASE);
        SetWindowPos((HWND)hWnd, NULL, NULL, NULL, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_DEFERERASE);

        // Redraw the entire explorer window
        RedrawWindow((HWND)hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ERASENOW | RDW_UPDATENOW | RDW_ALLCHILDREN);
    }

    if (settings.addressSync) {
        if (settings.addressSyncFullPath) {
            HWND toolbarwindow = GetChildWindow(NavBarAddressBandRoot, L"ToolbarWindow32", 1);
            GetWindowTextW(toolbarwindow, pathBuffer, MAX_PATH);

            // FIXME: codecvt deprecated as of c++17
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

            // remove "Address: "
            // FIXME: support more locales
            std::wstring _pathStr = pathBuffer;
            std::string pathStr(_pathStr.begin(), _pathStr.end());
            pathStr = std::regex_replace(pathStr, std::regex("Address: "), "");
            _pathStr = converter.from_bytes(pathStr);
            wcscpy(pathBuffer, _pathStr.c_str());
        } else {
            HWND shelltabwindow = GetChildWindow((HWND)hWnd, L"ShellTabWindowClass", 1);
            GetWindowTextW(shelltabwindow, pathBuffer, MAX_PATH);
        }

        HWND AddressBarEdit = GetChildWindow(ClassicAddressBandRoot, L"Edit", 1);
        for (int i = 0; i < 3; i++)
            SendMessageW(AddressBarEdit, WM_SETTEXT, 0, (LPARAM)pathBuffer);

        // TODO: figure out how to get and set folder icon
    }

    if (settings.hideRefresh && shouldApply) {
        HWND GoButtonToolbar = GetChildWindow(ClassicAddressBandRoot, L"ToolbarWindow32", 1);
        SendMessageW(GoButtonToolbar, WM_CLOSE, 0, 0);
    }

    if (settings.addressHeight) {
        // Allocate memory inside Explorer
        DWORD count = SendMessage(GetParent(ClassicAddressBandRoot), RB_GETBANDCOUNT, 0, 0);
        SIZE_T bytesRead = 0;
        DWORD ExplorerPID = 0;
        GetWindowThreadProcessId(ClassicAddressBandRoot, &ExplorerPID);
        HANDLE ExplorerProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ExplorerPID);
        void* ExplorerMemoryRebar = VirtualAllocEx(ExplorerProcess, NULL, sizeof(REBARBANDINFO), MEM_COMMIT, PAGE_READWRITE);
        void* ExplorerMemoryComboBoxItem = VirtualAllocEx(ExplorerProcess, NULL, sizeof(COMBOBOXEXITEM), MEM_COMMIT, PAGE_READWRITE);
        void* ExplorerMemoryToolbarButton = VirtualAllocEx(ExplorerProcess, NULL, sizeof(TBBUTTON), MEM_COMMIT, PAGE_READWRITE);

        // Make the band that's 39 pixels high, 22 pixels high
        // MOD: Except this bar isn't always 39 pixels, so lets check between 39-23px instead
        REBARBANDINFO bandInfo = { 0 };
        for (int i = 0; i < count; i++)
        {
            bandInfo = { 0 };
            bandInfo.cbSize = sizeof(REBARBANDINFO);
            bandInfo.fMask = RBBIM_CHILDSIZE;
            WriteProcessMemory(ExplorerProcess, ExplorerMemoryRebar, &bandInfo, sizeof(REBARBANDINFO), &bytesRead);
            SendMessageW(GetParent(ClassicAddressBandRoot), RB_GETBANDINFO, i, (LPARAM)ExplorerMemoryRebar);
            ReadProcessMemory(ExplorerProcess, ExplorerMemoryRebar, &bandInfo, sizeof(REBARBANDINFO), &bytesRead);
            if (bandInfo.cyMinChild <= 39 && bandInfo.cyMinChild > 22) {
                bandInfo.cyMinChild = 22;
                WriteProcessMemory(ExplorerProcess, ExplorerMemoryRebar, &bandInfo, sizeof(REBARBANDINFO), &bytesRead);
                SendMessageW(GetParent(ClassicAddressBandRoot), RB_SETBANDINFO, i, (LPARAM)ExplorerMemoryRebar);
            }
        }

        // Free Explorer Memory
        VirtualFreeEx(ExplorerProcess, ExplorerMemoryRebar, sizeof(REBARBANDINFO), MEM_DECOMMIT);
        VirtualFreeEx(ExplorerProcess, ExplorerMemoryToolbarButton, sizeof(REBARBANDINFO), MEM_DECOMMIT);
        CloseHandle(ExplorerProcess);
        
        // Set ComboBox height
        SendMessageW(ClassicComboBox, CB_SETITEMHEIGHT, -1, 22 - 6);
        SetParent(ClassicComboBox, ClassicAddressBandRoot);
        
        // Remove ProgressBsr
        SendMessageW(ClassicProgressControl, WM_CLOSE, 0, 0);

        // Fix ComboBox
        GetWindowRect(ClassicComboBox, &rect);
        SetWindowPos(ClassicComboBox, NULL, NULL, NULL, rect.right - rect.left + 1, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_DEFERERASE);
        SetWindowPos(ClassicComboBox, NULL, NULL, NULL, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_DEFERERASE);

        // Redraw the ComboBox
        RedrawWindow(ClassicComboBox, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ERASENOW | RDW_UPDATENOW | RDW_ALLCHILDREN);
    }
}

// boilerplate from hide-search-bar
#if defined(__GNUC__) && __GNUC__ > 8
#define WINAPI_LAMBDA_RETURN(return_t) -> return_t WINAPI
#elif defined(__GNUC__)
#define WINAPI_LAMBDA_RETURN(return_t) WINAPI -> return_t
#else
#define WINAPI_LAMBDA_RETURN(return_t) -> return_t
#endif

DWORD g_uiThreadId;

inline LSTATUS SHRegGetValueFromHKCUHKLMWithOpt(PCWSTR pwszKey, PCWSTR pwszValue, REGSAM samDesired, void* pvData, DWORD* pcbData)
{
    LSTATUS lRes = ERROR_FILE_NOT_FOUND;
    HKEY hKey = NULL;

    RegOpenKeyExW(HKEY_CURRENT_USER, pwszKey, 0, samDesired, &hKey);
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE) hKey = NULL;
    if (hKey)
    {
        lRes = RegQueryValueExW(hKey, pwszValue, 0, NULL, (LPBYTE) pvData, (LPDWORD) pcbData);

        RegCloseKey(hKey);
        if (lRes == ERROR_SUCCESS || lRes == ERROR_MORE_DATA) return lRes;
    }

    RegOpenKeyExW(HKEY_LOCAL_MACHINE, pwszKey, 0, samDesired, &hKey);
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE) hKey = NULL;

    if (hKey)
    {
        lRes = RegQueryValueExW(hKey, pwszValue, 0, NULL, (LPBYTE) pvData, (LPDWORD) pcbData);
        RegCloseKey(hKey);

        if (lRes == ERROR_SUCCESS || lRes == ERROR_MORE_DATA) return lRes;
    }

    return lRes;
}

UINT g_subclassRegisteredMsg = RegisterWindowMessage(L"Windhawk_SetWindowSubclassFromAnyThread_classic-theme-explorer");

LRESULT CALLBACK ClassicThemeExplorerSubClass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_DESTROY) {
        Wh_Log(L"[Destroy] uMsg: 0x%04x, wParam: 0x%x", uMsg, wParam);
        RemoveWindowSubclass(hWnd, ClassicThemeExplorerSubClass, 0);
    } else if (uMsg == g_subclassRegisteredMsg && !wParam) {
        Wh_Log(L"[Unsub] uMsg: 0x%04x, wParam: 0x%x", uMsg, wParam);
        RemoveWindowSubclass(hWnd, ClassicThemeExplorerSubClass, 0);
    } else if (uMsg == WM_PARENTNOTIFY || uMsg == WM_SIZE || uMsg == WM_GETICON) {
        Wh_Log(L"[Target] uMsg: 0x%04x, wParam: 0x%x", uMsg, wParam);
        ClassicThemeExplorer(hWnd, uMsg, wParam);
    //} else {
        //Wh_Log(L"[Unknown] uMsg: 0x%04x, wParam: 0x%x", uMsg, wParam);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

struct SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM {
    SUBCLASSPROC pfnSubclass;
    UINT_PTR uIdSubclass;
    DWORD_PTR dwRefData;
    BOOL result;
};

LRESULT CALLBACK CallWndProcForWindowSubclass(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == g_subclassRegisteredMsg && cwp->wParam) {
            SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM* param = (SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM*)cwp->lParam;
            param->result = SetWindowSubclass(cwp->hwnd, param->pfnSubclass, param->uIdSubclass, param->dwRefData);
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

BOOL SetWindowSubclassFromAnyThread(HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    
    if (dwThreadId == 0) return FALSE;
    if (dwThreadId == GetCurrentThreadId()) return SetWindowSubclass(hWnd, pfnSubclass, uIdSubclass, dwRefData);

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProcForWindowSubclass, nullptr, dwThreadId);

    if (!hook) return FALSE;

    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM param;
    param.pfnSubclass = pfnSubclass;
    param.uIdSubclass = uIdSubclass;
    param.dwRefData = dwRefData;
    param.result = FALSE;
    
    SendMessage(hWnd, g_subclassRegisteredMsg, TRUE, (WPARAM)&param);

    UnhookWindowsHookEx(hook);

    return param.result;
}

BOOL CALLBACK EnumBrowserWindowsUnsubclassFunc(HWND hWnd, LPARAM lParam) {
    SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0);

    return TRUE;
}


HWND(WINAPI *pOriginalSHCreateWorkerWindow)(WNDPROC wndProc, HWND hWndParent, DWORD dwExStyle, DWORD dwStyle, HMENU hMenu, LONG_PTR wnd_extra);

HWND WINAPI SHCreateWorkerWindowHook(WNDPROC wndProc, HWND hWndParent, DWORD dwExStyle, DWORD dwStyle, HMENU hMenu, LONG_PTR wnd_extra)
{
    HWND result;
    LSTATUS lRes = ERROR_FILE_NOT_FOUND;
    DWORD dwSize = 0;

    Wh_Log(L"g_uiThreadId: %d, GetCurrentThreadId: %d", g_uiThreadId, GetCurrentThreadId());

    // is this even needed? it changes thread after some time anyways
    //if (g_uiThreadId && g_uiThreadId != GetCurrentThreadId()) return pOriginalSHCreateWorkerWindow(wndProc, hWndParent, dwExStyle, dwStyle, hMenu, wnd_extra);
    /*if (!g_uiThreadId)*/ g_uiThreadId = GetCurrentThreadId();


    if (SHRegGetValueFromHKCUHKLMWithOpt(
        TEXT("SOFTWARE\\Classes\\CLSID\\{056440FD-8568-48e7-A632-72157243B55B}\\InProcServer32"),
        TEXT(""),
        KEY_READ | KEY_WOW64_64KEY,
        NULL,
        (LPDWORD)(&dwSize)
    ) == ERROR_SUCCESS && (dwSize < 4) && dwExStyle == 0x10000 && dwStyle == 1174405120) result = 0;

    else result = pOriginalSHCreateWorkerWindow(wndProc, hWndParent, dwExStyle, dwStyle, hMenu, wnd_extra);

    Wh_Log(L"dwExStyle: 0x%x, dwStyle: 0x%x, result: 0x%x", dwExStyle, dwStyle, result);

    if (dwExStyle == 0x10000 && dwStyle == 0x46000000 && result) SetWindowSubclassFromAnyThread(hWndParent, ClassicThemeExplorerSubClass, 0, 0);

    return result;
}

// taskbar fixes from ExplorerPatcher
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Orig;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,LPCWSTR lpClassName,LPCWSTR lpWindowName,DWORD dwStyle,int X,int Y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam) {
    if (settings.epMitigate && (*((WORD*)&(lpClassName)+1)) && !wcscmp(lpClassName, L"TrayNotifyWnd"))
    {
        dwExStyle |= WS_EX_STATICEDGE;
    }
    if (settings.epMitigate && (*((WORD*)&(lpClassName)+1)) && !wcscmp(lpClassName, L"NotifyIconOverflowWindow"))
    {
        dwExStyle |= WS_EX_STATICEDGE;
    }
    if (settings.clientEdge && (*((WORD*)&(lpClassName)+1)) && (!wcscmp(lpClassName, L"SysListView32") || !wcscmp(lpClassName, L"SysTreeView32"))) // !wcscmp(lpClassName, L"FolderView")
    {
        wchar_t wszClassName[200];
        ZeroMemory(wszClassName, 200);
        GetClassNameW(GetAncestor(hWndParent, GA_ROOT), wszClassName, 200);
        if (!wcscmp(wszClassName, L"CabinetWClass"))
        {
            dwExStyle |= WS_EX_CLIENTEDGE;
        }
    }
    if (settings.epMitigate && (*((WORD*)&(lpClassName)+1)) && !wcscmp(lpClassName, L"ReBarWindow32"))
    {
        wchar_t wszClassName[200];
        ZeroMemory(wszClassName, 200);
        GetClassNameW(hWndParent, wszClassName, 200);
        if (!wcscmp(wszClassName, L"Shell_TrayWnd"))
        {
            dwStyle |= RBS_BANDBORDERS;
        }
    }

    HWND hWnd = CreateWindowExW_Orig(dwExStyle,lpClassName,lpWindowName,dwStyle,X,Y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);

    return hWnd;
}
using SetWindowLongPtrW_t = decltype(&SetWindowLongPtrW);
SetWindowLongPtrW_t SetWindowLongPtrW_Orig;
LONG_PTR SetWindowLongPtrW_Hook(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
    WCHAR lpClassName[200];
    ZeroMemory(lpClassName, 200);
    GetClassNameW(hWnd, lpClassName, 200);
    HWND hWndParent = GetParent(hWnd);

    if (settings.epMitigate && (*((WORD*)&(lpClassName)+1)) && !wcscmp(lpClassName, L"TrayNotifyWnd"))
    {
        if (nIndex == GWL_EXSTYLE)
        {
            dwNewLong |= WS_EX_STATICEDGE;
        }
    }
    if (settings.epMitigate && (*((WORD*)&(lpClassName)+1)) && !wcscmp(lpClassName, L"NotifyIconOverflowWindow"))
    {
        if (nIndex == GWL_EXSTYLE)
        {
            dwNewLong |= WS_EX_STATICEDGE;
        }
    }
    if (settings.clientEdge && (*((WORD*)&(lpClassName)+1)) && (!wcscmp(lpClassName, L"SysListView32") || !wcscmp(lpClassName, L"SysTreeView32"))) // !wcscmp(lpClassName, L"FolderView")
    {
        wchar_t wszClassName[200];
        ZeroMemory(wszClassName, 200);
        GetClassNameW(GetAncestor(hWndParent, GA_ROOT), wszClassName, 200);
        if (!wcscmp(wszClassName, L"CabinetWClass"))
        {
            if (nIndex == GWL_EXSTYLE)
            {
                dwNewLong |= WS_EX_CLIENTEDGE;
            }
        }
    }
    if (settings.epMitigate && (*((WORD*)&(lpClassName)+1)) && !wcscmp(lpClassName, L"ReBarWindow32"))
    {
        wchar_t wszClassName[200];
        ZeroMemory(wszClassName, 200);
        GetClassNameW(hWndParent, wszClassName, 200);
        if (!wcscmp(wszClassName, L"Shell_TrayWnd"))
        {
            if (nIndex == GWL_STYLE)
            {
                dwNewLong |= RBS_BANDBORDERS;
            }
        }
    }

    return SetWindowLongPtrW_Orig(hWnd, nIndex, dwNewLong);
}

HTHEME(*pOriginalOpenThemeDataForDpi)(HWND hWnd, LPCWSTR pszClassList, UINT dpi);
HTHEME OpenThemeDataForDpiHook(HWND hWnd, LPCWSTR pszClassList, UINT dpi)
{
    if (settings.epMitigate && (*((WORD*)&(pszClassList)+1)) && !wcscmp(pszClassList, L"Taskband2")) {
        Wh_Log(L"Redirecting Taskband2 hTheme");
        return (HTHEME)0xDEADBEEF;
    } else if (settings.epMitigate && (*((WORD*)&(pszClassList)+1)) && !wcscmp(pszClassList, L"TrayNotifyFlyout")) {
        Wh_Log(L"Redirecting TrayNotifyFlyout hTheme");
        return (HTHEME)0xDEADBEFF;
    }

    return pOriginalOpenThemeDataForDpi(hWnd, pszClassList, dpi);
}

HRESULT(*pOriginalGetThemeMetric)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, int* piVal);
HRESULT GetThemeMetricHook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, int* piVal) {
    Wh_Log(L"hTheme: %08X", hTheme);

    if (!settings.epMitigate || (hTheme != (HTHEME)0xDEADBEFF)) {
        return pOriginalGetThemeMetric(hTheme, hdc, iPartId, iStateId, iPropId, piVal);
    }

    const int TMT_WIDTH = 2416;
    const int TMT_HEIGHT = 2417;
    if (hTheme == (HTHEME)0xDEADBEFF && iPropId == TMT_WIDTH && iPartId == 3 && iStateId == 0)
    {
        *piVal = GetSystemMetrics(SM_CXICON);
    }
    else if (hTheme == (HTHEME)0xDEADBEFF && iPropId == TMT_HEIGHT && iPartId == 3 && iStateId == 0)
    {
        *piVal = GetSystemMetrics(SM_CYICON);
    }
    return S_OK;
}


HRESULT(*pOriginalGetThemeMargins)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, LPCRECT prc, MARGINS* pMargins);
HRESULT GetThemeMarginsHook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, LPCRECT prc, MARGINS* pMargins) {
    Wh_Log(L"hTheme: %08X", hTheme);

    if (!settings.epMitigate || (hTheme != (HTHEME)0xDEADBEEF && hTheme != (HTHEME)0xDEADBEFF)) {
        return pOriginalGetThemeMargins(hTheme, hdc, iPartId, iStateId, iPropId, prc, pMargins);
    }

    const int TMT_SIZINGMARGINS = 3601;
    const int TMT_CONTENTMARGINS = 3602;
    HRESULT hr = S_OK;
    if (hTheme)
    {
        hr = pOriginalGetThemeMargins(
            hTheme,
            hdc,
            iPartId,
            iStateId,
            iPropId,
            prc,
            pMargins
        );
    }

    if (hTheme == (HTHEME)0xDEADBEEF && iPropId == TMT_CONTENTMARGINS && iPartId == 5 && iStateId == 1) {
        // task list button measurements
        pMargins->cxLeftWidth = 4;
        pMargins->cyTopHeight = 3;
        pMargins->cxRightWidth = 4;
        pMargins->cyBottomHeight = 3;
    } else if (hTheme == (HTHEME)0xDEADBEEF && iPropId == TMT_CONTENTMARGINS && iPartId == 1 && iStateId == 0) {
        // task list measurements
        pMargins->cxLeftWidth = 0;
        pMargins->cyTopHeight = 0;
        pMargins->cxRightWidth = 4;
        pMargins->cyBottomHeight = 0;
    } else if (hTheme == (HTHEME)0xDEADBEEF && iPropId == TMT_SIZINGMARGINS && iPartId == 5 && iStateId == 1) {
        pMargins->cxLeftWidth = 0;
        pMargins->cyTopHeight = 10;
        pMargins->cxRightWidth = 0;
        pMargins->cyBottomHeight = 10;
    } else if (hTheme == (HTHEME)0xDEADBEFF && iPropId == TMT_CONTENTMARGINS && iPartId == 3 && iStateId == 0) {
        pMargins->cxLeftWidth = 4; // GetSystemMetrics(SM_CXICONSPACING);
        pMargins->cyTopHeight = 4; // GetSystemMetrics(SM_CYICONSPACING);
        pMargins->cxRightWidth = 4; //GetSystemMetrics(SM_CXICONSPACING);
        pMargins->cyBottomHeight = 4; // GetSystemMetrics(SM_CYICONSPACING);
    }

    HWND hShell_TrayWnd = FindWindowEx(NULL, NULL, L"Shell_TrayWnd", NULL);
    if (hShell_TrayWnd) {
        LONG dwStyle = 0;
        dwStyle = GetWindowLongW(hShell_TrayWnd, GWL_STYLE);
        dwStyle |= WS_DLGFRAME;
        SetWindowLongW(hShell_TrayWnd, GWL_STYLE, dwStyle);
        dwStyle &= ~WS_DLGFRAME;
        SetWindowLongW(hShell_TrayWnd, GWL_STYLE, dwStyle);
    }

    HWND hWnd = NULL;
    do {
        hWnd = FindWindowEx(
            NULL,
            hWnd,
            L"Shell_SecondaryTrayWnd",
            NULL
        );
        if (hWnd) {
            LONG dwStyle = 0;
            dwStyle = GetWindowLongW(hWnd, GWL_STYLE);
            dwStyle |= WS_DLGFRAME;
            SetWindowLongW(hWnd, GWL_STYLE, dwStyle);
            dwStyle &= ~WS_DLGFRAME;
            SetWindowLongW(hWnd, GWL_STYLE, dwStyle);
        }
    } while (hWnd);

    return S_OK;
}

HRESULT(*pOriginalDrawThemeTextEx)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect, const DTTOPTS* pOptions);
HRESULT DrawThemeTextExHook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect, const DTTOPTS* pOptions) {
    if (!settings.epMitigate) {
        return pOriginalDrawThemeTextEx(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, pRect, pOptions);
    }

    COLORREF bc = GetBkColor(hdc);
    COLORREF fc = GetTextColor(hdc);
    int mode = SetBkMode(hdc, TRANSPARENT);

        wchar_t text[200];
    GetWindowTextW(GetForegroundWindow(), text, 200);

    BOOL bIsActiveUnhovered = (iPartId == 5 && iStateId == 5);
    BOOL bIsInactiveUnhovered = (iPartId == 5 && iStateId == 1);
    BOOL bIsInactiveHovered = (iPartId == 5 && iStateId == 2);
    BOOL bIsActiveHovered = bIsInactiveHovered && !wcscmp(text, pszText);

    SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));

    NONCLIENTMETRICSW ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICSW);
    SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW), &ncm, 0);

    HFONT hFont = NULL;
    if (bIsActiveUnhovered)
    {
        hFont = CreateFontIndirectW(&(ncm.lfCaptionFont));
    }
    else if (bIsInactiveUnhovered)
    {
        hFont = CreateFontIndirectW(&(ncm.lfMenuFont));
    }
    else if (bIsActiveHovered)
    {
        hFont = CreateFontIndirectW(&(ncm.lfCaptionFont));
    }
    else if (bIsInactiveHovered)
    {
        hFont = CreateFontIndirectW(&(ncm.lfMenuFont));
    }
    else
    {
        hFont = CreateFontIndirectW(&(ncm.lfMenuFont));
    }

    if (iPartId == 5 && iStateId == 0) // clock
    {
        pRect->top += 2;
    }

    HGDIOBJ hOldFont = SelectObject(hdc, hFont);
    DrawTextW(
        hdc,
        pszText,
        cchText, 
        pRect,
        dwTextFlags
    );
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    SetBkColor(hdc, bc);
    SetTextColor(hdc, fc);
    SetBkMode(hdc, mode);

    return S_OK;
}

HRESULT(*pOriginalDrawThemeBackground)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPRECT pRect, LPCRECT pClipRect);
HRESULT DrawThemeBackgroundHook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPRECT pRect, LPCRECT pClipRect) {
    if (settings.epMitigate) {
        if (iPartId == 4 && iStateId == 1) {
            COLORREF bc = GetBkColor(hdc);
            COLORREF fc = GetTextColor(hdc);
            int mode = SetBkMode(hdc, TRANSPARENT);

            SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));

            NONCLIENTMETRICSW ncm;
            ncm.cbSize = sizeof(NONCLIENTMETRICSW);
            SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW), &ncm, 0);

            HFONT hFont = CreateFontIndirectW(&(ncm.lfCaptionFont));

            HGDIOBJ hOldFont = SelectObject(hdc, hFont);
            DWORD dwTextFlags = DT_SINGLELINE | DT_CENTER | DT_VCENTER;
            RECT rc = *pRect;
            rc.bottom -= 7;
            DrawTextW(
                hdc,
                L"\u2026",
                -1, 
                &rc,
                dwTextFlags
            );
            SelectObject(hdc, hOldFont);
            DeleteObject(hFont);
            SetBkColor(hdc, bc);
            SetTextColor(hdc, fc);
            SetBkMode(hdc, mode);
        }
        return S_OK;
    }

    return pOriginalDrawThemeBackground(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}

struct WINCOMPATTRDATA
{
    DWORD attribute;
    PVOID pData;
    ULONG dataSize;
};

BOOL (WINAPI *pOriginalSetWindowCompositionAttribute)(HWND hWnd, WINCOMPATTRDATA* pData);
BOOL WINAPI SetWindowCompositionAttributeHook(HWND hWnd, WINCOMPATTRDATA* pData) {
    if (settings.epMitigate) {
        return TRUE;
    }

    return pOriginalSetWindowCompositionAttribute(hWnd, pData);
}

// Windhawk boilerplate
BOOL Wh_ModInit() {
    LoadSettings();

    HMODULE hShcore = GetModuleHandle(L"shcore.dll");

    void* origCWW = (void*)GetProcAddress(hShcore, (LPCSTR)188);
    Wh_SetFunctionHook(origCWW, (void*)SHCreateWorkerWindowHook, (void**)&pOriginalSHCreateWorkerWindow);

    HMODULE hUxtheme = GetModuleHandle(L"uxtheme.dll");

    void* origOTDFD = (void*)GetProcAddress(hUxtheme, "OpenThemeDataForDpi");
    Wh_SetFunctionHook(origOTDFD, (void*)OpenThemeDataForDpiHook, (void**)&pOriginalOpenThemeDataForDpi);
    
    void* origGTMe = (void*)GetProcAddress(hUxtheme, "GetThemeMetric");
    Wh_SetFunctionHook(origGTMe, (void*)GetThemeMetricHook, (void**)&pOriginalGetThemeMetric);
    
    void* origGTMa = (void*)GetProcAddress(hUxtheme, "GetThemeMargins");
    Wh_SetFunctionHook(origGTMa, (void*)GetThemeMarginsHook, (void**)&pOriginalGetThemeMargins);

    void* origDTTE = (void*)GetProcAddress(hUxtheme, "DrawThemeTextEx");
    Wh_SetFunctionHook(origDTTE, (void*)DrawThemeTextExHook, (void**)&pOriginalDrawThemeTextEx);

    void* origDTB = (void*)GetProcAddress(hUxtheme, "DrawThemeBackground");
    Wh_SetFunctionHook(origDTB, (void*)DrawThemeBackgroundHook, (void**)&pOriginalDrawThemeBackground);

    //HMODULE hUser32 = GetModuleHandle(L"user32.dll");

    //void* origSWCA = (void*)GetProcAddress(hUser32, "SetWindowCompositionAttribute");
    //Wh_SetFunctionHook(origSWCA, (void*)SetWindowCompositionAttributeHook, (void**)&pOriginalSetWindowCompositionAttribute);

    Wh_SetFunctionHook((void*)CreateWindowExW,
                       (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Orig);
    Wh_SetFunctionHook((void*)SetWindowLongPtrW,
                       (void*)SetWindowLongPtrW_Hook,
                       (void**)&SetWindowLongPtrW_Orig);

    return TRUE;
}

void Wh_ModUninit() {
    if (g_uiThreadId != 0) EnumThreadWindows(g_uiThreadId, EnumBrowserWindowsUnsubclassFunc, 0);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
