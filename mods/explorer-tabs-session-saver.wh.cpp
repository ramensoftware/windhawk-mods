// ==WindhawkMod==
// @id              explorer-tabs-session-saver
// @name            Explorer Tabs Session Saver
// @description     Saves and restores your Explorer tab session
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

// ==WindhawkModSettings==
/*
- enableSaving: true
  $name: Enable saving and restoring
  $description: >-
    Enables or disables the main feature of the mod: saving and restoring tabs.
*/
// ==/WindhawkModSettings==

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
bool g_isSavingEnabled = true;

void LoadSettings() {
    g_isSavingEnabled = Wh_GetIntSetting(L"enableSaving");
}

std::wstring GetTabListFilePath() {
    wchar_t* appDataPath = NULL;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &appDataPath))) {
        std::wstring modPath = appDataPath;
        CoTaskMemFree(appDataPath);

        modPath += L"\\WindhawkModsData\\explorer-tabs-session-saver";
        SHCreateDirectoryExW(NULL, modPath.c_str(), NULL);
        modPath += L"\\LastExplorerTabs.txt";
        return modPath;
    }
    return L"";
}

std::vector<std::wstring> GetCurrentTabs() {
    std::vector<std::wstring> currentTabs;
    IShellWindows* pShellWindows = NULL;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pShellWindows)))) {
        return currentTabs;
    }

    long count = 0;
    pShellWindows->get_Count(&count);
    for (long i = 0; i < count; i++) {
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

typedef HWND (WINAPI *CreateWindowExW_t)(DWORD, LPCWSTR, LPCWSTR, DWORD,
    int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
CreateWindowExW_t pCreateWindowExW_Orig = NULL;

DWORD WINAPI RestoreTabsThread(LPVOID) {
    // wait until the first Explorer window is ready
    HWND hwnd = NULL;
    for (int i = 0; i < 30 && !(hwnd = FindWindowW(L"CabinetWClass", NULL)); i++) {
        Sleep(100);
    }

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr)) {
        RestoreTabs();
        CoUninitialize();
    }
    return 0;
}

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) 
{
    HWND hwnd = pCreateWindowExW_Orig(dwExStyle, lpClassName, lpWindowName,
                                      dwStyle, X, Y, nWidth, nHeight,
                                      hWndParent, hMenu, hInstance, lpParam);

    if (!hwnd) {
        return hwnd;
    }

    wchar_t cls[64] = {};
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) && lstrcmpW(cls, L"CabinetWClass") == 0) {
        // trigger restore once per Explorer restart
        if (InterlockedCompareExchange(&g_restoreScheduled, 1, 0) == 0) {
            std::wstring filePath = GetTabListFilePath();
            if (!filePath.empty() && PathFileExistsW(filePath.c_str())) {
                HANDLE h = CreateThread(NULL, 0, RestoreTabsThread, NULL, 0, NULL);
                if (h) CloseHandle(h);
            }
        }
    }

    return hwnd;
}

DWORD WINAPI MainThread(LPVOID) {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    while (g_bIsRunning) {
        std::vector<std::wstring> tabs = GetCurrentTabs();
        if (!tabs.empty()) {
            std::sort(tabs.begin(), tabs.end());
            if (tabs != g_lastKnownTabs) {
                WriteTabsToFile(tabs);
                g_lastKnownTabs = std::move(tabs);
            }
        } else {
            // reset restore flag when no Explorer windows
            InterlockedExchange(&g_restoreScheduled, 0);
        }
        Sleep(2000);
    }
    CoUninitialize();
    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();

    if (!g_isSavingEnabled) {
        return TRUE;
    }

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&pCreateWindowExW_Orig);
    g_bIsRunning = true;
    g_hMainThread = CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
    return TRUE;
}

void Wh_ModUninit() {
    if (!g_isSavingEnabled) {
        return;
    }
    
    g_bIsRunning = false;
    if (g_hMainThread) {
        WaitForSingleObject(g_hMainThread, 2500);
        CloseHandle(g_hMainThread);
        g_hMainThread = NULL;
    }

    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    std::vector<std::wstring> finalTabs = GetCurrentTabs();
    if (!finalTabs.empty()) {
        WriteTabsToFile(finalTabs);
    }
    CoUninitialize();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}
