// ==WindhawkMod==
// @id              explorer-tabs-session-saver
// @name            Explorer Tabs Session Saver
// @description     Saves and restores Explorer tabs when reopening
// @github          https://github.com/noX1st
// @version         1.0.0
// @author          noX1st
// @include         explorer.exe
// @compilerOptions -lshlwapi -lole32 -loleaut32 -lshell32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Tabs Session Saver

This mod saves open Explorer tabs and automatically restores them when Explorer restarts.

![Demonstration](https://i.imgur.com/5ehPFaL.gif)

It works independently of the built-in Windows feature "Restore previous folder windows at logon".

Tab session save path - C:\Users\"User"\AppData\Roaming\WindhawkModsData\explorer-tabs-session-saver\LastExplorerTabs.txt

**NOTE:** To restore tabs into a single Explorer window, the [Explorer Tab Utility](https://github.com/w4po/ExplorerTabUtility) program is required with its "Window Hook" feature enabled (or any similar program with such functionality). Otherwise, each tab will be restored in its own separate window.

*Tested on Windows 11 24H2 26100.4946.*
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <vector>
#include <string>
#include <fstream>
#include <olectl.h>
#include <algorithm>

HANDLE g_hMainThread = NULL;
bool   g_bIsRunning  = false;
std::vector<std::wstring> g_lastKnownTabs;
static volatile LONG g_restoreScheduled = 0;

std::wstring GetTabListFilePath() {
    wchar_t modPathBuffer[MAX_PATH];
    if (!Wh_GetModStoragePath(modPathBuffer, MAX_PATH)) {
        Wh_Log(L"Failed to get mod storage path.");
        return L"";
    }

    std::wstring modPath = modPathBuffer;

    SHCreateDirectoryExW(NULL, modPath.c_str(), NULL);

    modPath += L"\\LastExplorerTabs.txt";
    return modPath;
}

std::vector<std::wstring> GetCurrentTabs() {
    std::vector<std::wstring> currentTabs;
    IShellWindows* pShellWindows = NULL;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pShellWindows)))) {
        return currentTabs;
    }

    long count = 0;
    pShellWindows->get_Count(&count);
    for (long i = count - 1; i >= 0; i--) {
        VARIANT v; VariantInit(&v); v.vt = VT_I4; v.lVal = i;
        IDispatch* pDispatch = NULL;
        if (SUCCEEDED(pShellWindows->Item(v, &pDispatch)) && pDispatch) {
            IServiceProvider* pServiceProvider = NULL;
            if (SUCCEEDED(pDispatch->QueryInterface(IID_PPV_ARGS(&pServiceProvider)))) {
                IShellBrowser* pShellBrowser = NULL;
                if (SUCCEEDED(pServiceProvider->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&pShellBrowser)))) {
                    IShellView* pShellView = NULL;
                    if (SUCCEEDED(pShellBrowser->QueryActiveShellView(&pShellView))) {
                        IFolderView* pFolderView = NULL;
                        if (SUCCEEDED(pShellView->QueryInterface(IID_PPV_ARGS(&pFolderView)))) {
                            IPersistFolder2* pPersistFolder2 = NULL;
                            if (SUCCEEDED(pFolderView->GetFolder(IID_PPV_ARGS(&pPersistFolder2)))) {
                                PIDLIST_ABSOLUTE pidl;
                                if (SUCCEEDED(pPersistFolder2->GetCurFolder(&pidl))) {
                                    wchar_t path[MAX_PATH];
                                    if (SHGetPathFromIDListW(pidl, path)) {
                                        currentTabs.push_back(path);
                                    }
                                    CoTaskMemFree(pidl);
                                }
                                pPersistFolder2->Release();
                            }
                            pFolderView->Release();
                        }
                        pShellView->Release();
                    }
                    pShellBrowser->Release();
                }
                pServiceProvider->Release();
            }
            pDispatch->Release();
        }
    }
    pShellWindows->Release();
    std::reverse(currentTabs.begin(), currentTabs.end());
    return currentTabs;
}

void WriteTabsToFile(const std::vector<std::wstring>& tabs) {
    std::wstring filePath = GetTabListFilePath();
    if (filePath.empty()) return;

    std::wofstream tabFile(filePath.c_str());
    if (tabFile.is_open()) {
        for (const auto& tab : tabs) {
            tabFile << tab << std::endl;
        }
    }
}

void RestoreTabs() {
    std::wstring filePath = GetTabListFilePath();
    if (filePath.empty()) return;

    std::wifstream tabFile(filePath.c_str());
    if (!tabFile.is_open()) return;

    std::vector<std::wstring> paths;
    std::wstring line;
    while (std::getline(tabFile, line)) {
        if (!line.empty()) paths.push_back(line);
    }
    tabFile.close();

    DeleteFileW(filePath.c_str());

    if (paths.empty()) return;

    for (const auto& path : paths) {
        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.fMask = SEE_MASK_DEFAULT;
        sei.lpVerb = L"open";
        sei.lpFile = path.c_str();
        sei.nShow = SW_SHOWNORMAL;
        ShellExecuteExW(&sei);
        Sleep(50);
    }
}

DWORD WINAPI RestoreTabsThread(LPVOID lpParam) {
    // wait until the first Explorer window is ready
    HWND targetHwnd = (HWND)lpParam;
    if (!IsWindow(targetHwnd)) {
        Wh_Log(L"RestoreTabsThread: Invalid window handle received.");
        return 1;
    }

    Wh_Log(L"RestoreTabsThread: Triggered for window %p.", targetHwnd);

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        RestoreTabs();
        CoUninitialize();
    }
    return 0;
}

typedef HWND (WINAPI *CreateWindowExW_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
CreateWindowExW_t pCreateWindowExW_Orig = NULL;

HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam
) {
    HWND hwnd = pCreateWindowExW_Orig(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (hwnd) {
        wchar_t className[64];
        if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) && lstrcmpW(className, L"CabinetWClass") == 0) {
            // trigger restore once per Explorer restart
            if (InterlockedCompareExchange(&g_restoreScheduled, 1, 0) == 0) {
                std::wstring filePath = GetTabListFilePath();
                if (!filePath.empty() && PathFileExistsW(filePath.c_str())) {
                    HANDLE hThread = CreateThread(NULL, 0, RestoreTabsThread, (LPVOID)hwnd, 0, NULL);
                    if (hThread) {
                        CloseHandle(hThread);
                    }
                }
            }
        }
    }

    return hwnd;
}

DWORD WINAPI MainThread(LPVOID) {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    while (g_bIsRunning) {
        std::vector<std::wstring> currentTabs = GetCurrentTabs();
        
        if (currentTabs != g_lastKnownTabs) {
            if (currentTabs.empty() && !g_lastKnownTabs.empty()) {
                WriteTabsToFile(g_lastKnownTabs);
                Wh_Log(L"Last explorer window closed, saving final session.");
            }
            else {
                WriteTabsToFile(currentTabs);
            }
            
            g_lastKnownTabs = currentTabs;
        }

        if (currentTabs.empty()) {
            InterlockedExchange(&g_restoreScheduled, 0);
        }

        Sleep(2000);
    }

    CoUninitialize();
    return 0;
}


BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Explorer Tabs Session Saver...");

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&pCreateWindowExW_Orig);
    
    g_bIsRunning = true;
    g_hMainThread = CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
    
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Explorer Tabs Session Saver...");

    g_bIsRunning = false;
    if (g_hMainThread) {
        WaitForSingleObject(g_hMainThread, 2500);
        CloseHandle(g_hMainThread);
        g_hMainThread = NULL;
    }
}