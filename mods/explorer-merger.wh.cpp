// ==WindhawkMod==
// @id              explorer-merger
// @name            Explorer Merger
// @description     A tool to converts new Explorer windows into tabs.
// @version         1.2.0
// @author          Wu Junkai
// @github          https://github.com/WuJunkai2004
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lshell32 -luser32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# [Explorer-Merger](https://github.com/WuJunkai2004/Explorer-Merger)

**Explorer-Merger** is a Windows utility designed to automatically merge newly opened File Explorer windows into tabs of an existing Explorer window. This project aims to reduce window clutter and provide a more organized file management experience on Windows 11.

This project is inspired by [ExplorerTabUtility](https://github.com/w4po/ExplorerTabUtility).

The TrashBin tab merging logic is learned from [ALMAS CP](https://github.com/almas-cp/windhawk-mods/blob/explorer-tabs-only/mods/explorer-single-window-tabs.wh.cpp)
*/
// ==/WindhawkModReadme==
#include <initguid.h>
#include <exdisp.h>
#include <shldisp.h>
#include <shlguid.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <windows.h>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#if !defined(WH_MOD_ID) && !defined(Wh_Log)
#define Wh_Log wprintf
#endif

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "shell32.lib")

#define WM_COMMAND 0x0111
#define CMD_NEW_TAB 0xA21B
#define CMD_SELECT_TAB_BASE 0xA221

std::wstring GetComparisonPath(std::wstring path) {
    if (path.empty()) return L"";
    if (path.back() == L'\\' || path.back() == L'/') path.pop_back();
    if (path.find(L"file:///") == 0) path = path.substr(8);
    std::replace(path.begin(), path.end(), L'/', L'\\');
    if (path.compare(0, 2, L"::") != 0) {
        std::transform(path.begin(), path.end(), path.begin(), ::towlower);
    }
    return path;
}

std::wstring GetRawBrowserUrl(IWebBrowser2* pWebBrowser) {
    if (!pWebBrowser) return L"";
    std::wstring result;
    BSTR bstrUrl = NULL;
    if (SUCCEEDED(pWebBrowser->get_LocationURL(&bstrUrl)) && bstrUrl && wcslen(bstrUrl) > 0) {
        result = bstrUrl;
        SysFreeString(bstrUrl);
    } else {
        if (bstrUrl) SysFreeString(bstrUrl);
        IServiceProvider* pSP = NULL;
        if (SUCCEEDED(pWebBrowser->QueryInterface(IID_IServiceProvider, (void**)&pSP))) {
            IShellBrowser* pSB = NULL;
            if (SUCCEEDED(pSP->QueryService(SID_STopLevelBrowser, IID_IShellBrowser, (void**)&pSB))) {
                IShellView* pSV = NULL;
                if (SUCCEEDED(pSB->QueryActiveShellView(&pSV))) {
                    IFolderView* pFV = NULL;
                    if (SUCCEEDED(pSV->QueryInterface(IID_IFolderView, (void**)&pFV))) {
                        IPersistFolder2* pPF = NULL;
                        if (SUCCEEDED(pFV->GetFolder(IID_IPersistFolder2, (void**)&pPF))) {
                            PIDLIST_ABSOLUTE pidl = NULL;
                            if (SUCCEEDED(pPF->GetCurFolder(&pidl))) {
                                PWSTR name = NULL;
                                if (SUCCEEDED(SHGetNameFromIDList(pidl, SIGDN_DESKTOPABSOLUTEPARSING, &name))) {
                                    result = name;
                                    CoTaskMemFree(name);
                                }
                                CoTaskMemFree(pidl);
                            }
                            pPF->Release();
                        }
                        pFV->Release();
                    }
                    pSV->Release();
                }
                pSB->Release();
            }
            pSP->Release();
        }
    }
    return result;
}

HWND GetTabHandle(IWebBrowser2* pWebBrowser) {
    if (!pWebBrowser) return NULL;
    IServiceProvider* pServiceProvider = NULL;
    if (SUCCEEDED(pWebBrowser->QueryInterface(IID_IServiceProvider, (void**)&pServiceProvider))) {
        IShellBrowser* pShellBrowser = NULL;
        if (SUCCEEDED(pServiceProvider->QueryService(SID_SShellBrowser, IID_IShellBrowser, (void**)&pShellBrowser))) {
            HWND hwndTab = NULL;
            if (SUCCEEDED(pShellBrowser->GetWindow(&hwndTab))) {
                pShellBrowser->Release();
                pServiceProvider->Release();
                return hwndTab;
            }
            pShellBrowser->Release();
        }
        pServiceProvider->Release();
    }
    return NULL;
}

std::set<HWND> GetWindowTabHandles(HWND mainWindow) {
    std::set<HWND> tabs;
    HWND hTab = FindWindowExW(mainWindow, NULL, L"ShellTabWindowClass", NULL);
    while (hTab) {
        tabs.insert(hTab);
        hTab = FindWindowExW(mainWindow, hTab, L"ShellTabWindowClass", NULL);
    }
    return tabs;
}

HRESULT NavigateToPath(IWebBrowser2* pWB, const std::wstring& path) {
    HRESULT hrFinal = E_FAIL;
    IServiceProvider* pServiceProvider = NULL;
    if (SUCCEEDED(pWB->QueryInterface(IID_IServiceProvider, (void**)&pServiceProvider))) {
        IShellBrowser* pShellBrowser = NULL;
        if (SUCCEEDED(pServiceProvider->QueryService(SID_SShellBrowser, IID_IShellBrowser, (void**)&pShellBrowser))) {
            LPITEMIDLIST pidl = NULL;
            if (SUCCEEDED(SHParseDisplayName(path.c_str(), NULL, &pidl, 0, NULL))) {
                hrFinal = pShellBrowser->BrowseObject(pidl, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
                ILFree(pidl);
            }
            pShellBrowser->Release();
        }
        pServiceProvider->Release();
    }
    if (FAILED(hrFinal)) {
        Wh_Log(L"[Log] BrowseObject failed, falling back to Navigate2...\n");
        VARIANT vUrl;
        vUrl.vt = VT_BSTR;
        vUrl.bstrVal = SysAllocString(path.c_str());
        hrFinal = pWB->Navigate2(&vUrl, NULL, NULL, NULL, NULL);
        VariantClear(&vUrl);
    }
    return hrFinal;
}

void RestoreWindowToForeground(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;
    if (IsIconic(hwnd)) ShowWindow(hwnd, SW_RESTORE);
    // virtually press F23 to bypass SetForegroundWindow restrictions
    keybd_event(0x86, 0, 0, 0);
    keybd_event(0x86, 0, KEYEVENTF_KEYUP, 0);
    if (!SetForegroundWindow(hwnd)) {
        Sleep(50);
        SetForegroundWindow(hwnd);
    }
}

std::set<HWND> g_allowedHwnds;
RECT g_lastRect;
IShellWindows* g_pShellWindows = NULL;
#define WM_CHECK_WINDOWS (WM_USER + 1)

void HideWindowFast(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;

    RECT rc;
    GetWindowRect(hwnd, &rc);
    if (rc.left <= -30000) return;  // Already hidden or off-screen, do not record or move
    g_lastRect = rc;

    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    if (!(exStyle & WS_EX_LAYERED)) SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
    SetWindowPos(hwnd, NULL, -32000, -32000, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void ShowWindowFast(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;
    SetWindowPos(hwnd, NULL, g_lastRect.left, g_lastRect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
}

class CShellWindowsSink : public IDispatch {
public:
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) {
        if (riid == IID_IUnknown || riid == IID_IDispatch || riid == __uuidof(DShellWindowsEvents)) {
            *ppv = static_cast<IDispatch*>(this);
            return S_OK;
        }
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() { return 2; }
    STDMETHODIMP_(ULONG) Release() { return 1; }
    STDMETHODIMP GetTypeInfoCount(UINT*) { return E_NOTIMPL; }
    STDMETHODIMP GetTypeInfo(UINT, LCID, ITypeInfo**) { return E_NOTIMPL; }
    STDMETHODIMP GetIDsOfNames(REFIID, LPOLESTR*, UINT, LCID, DISPID*) { return E_NOTIMPL; }
    STDMETHODIMP Invoke(DISPID dispIdMember, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*) {
        if (dispIdMember == 200) PostMessage(NULL, WM_CHECK_WINDOWS, 0, 0);
        return S_OK;
    }
};

void CheckNewShellWindows() {
    if (!g_pShellWindows) return;
    long count = 0;
    g_pShellWindows->get_Count(&count);
    for (long i = 0; i < count; ++i) {
        VARIANT vIdx;
        vIdx.vt = VT_I4;
        vIdx.lVal = i;
        IDispatch* pDisp = NULL;
        if (SUCCEEDED(g_pShellWindows->Item(vIdx, &pDisp)) && pDisp) {
            IWebBrowser2* pWB = NULL;
            if (SUCCEEDED(pDisp->QueryInterface(IID_IWebBrowser2, (void**)&pWB))) {
                SHANDLE_PTR hPtr = 0;
                if (SUCCEEDED(pWB->get_HWND(&hPtr)) && hPtr) {
                    HWND hwnd = (HWND)hPtr;
                    HWND mainHwnd = NULL;
                    HWND tempHwnd = FindWindowExW(NULL, NULL, L"CabinetWClass", NULL);
                    while (tempHwnd) {
                        if (tempHwnd != hwnd && IsWindowVisible(tempHwnd)) {
                            mainHwnd = tempHwnd;
                            break;
                        }
                        tempHwnd = FindWindowExW(NULL, tempHwnd, L"CabinetWClass", NULL);
                    }
                    if (mainHwnd && !g_allowedHwnds.count(hwnd)) HideWindowFast(hwnd);
                }
                pWB->Release();
            }
            pDisp->Release();
        }
    }
}

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild,
                           DWORD dwEventThread, DWORD dwmsEventTime) {
    if (event != EVENT_OBJECT_SHOW || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }
    if (g_allowedHwnds.count(hwnd)) {
        g_allowedHwnds.erase(hwnd);
        return;
    }

    wchar_t className[256];
    if (GetClassNameW(hwnd, className, 256) && wcscmp(className, L"CabinetWClass") == 0) {
        // 1. Decide if we should keep it or merge it
        HWND mainHwnd = NULL;
        HWND tempHwnd = FindWindowExW(NULL, NULL, L"CabinetWClass", NULL);
        while (tempHwnd) {
            if (tempHwnd != hwnd && IsWindowVisible(tempHwnd)) {
                mainHwnd = tempHwnd;
                break;
            }
            tempHwnd = FindWindowExW(NULL, tempHwnd, L"CabinetWClass", NULL);
        }

        if (!mainHwnd) return;
        HideWindowFast(hwnd);

        // 3. Merging logic
        if (!g_pShellWindows) return;
        IShellWindows* pShellWindows = g_pShellWindows;
        pShellWindows->AddRef();

        IWebBrowser2* pNewWebBrowser = NULL;
        for (int r = 0; r < 20; ++r) {
            long count = 0;
            pShellWindows->get_Count(&count);
            for (long i = 0; i < count; ++i) {
                VARIANT vIdx;
                vIdx.vt = VT_I4;
                vIdx.lVal = i;
                IDispatch* pDisp = NULL;
                if (SUCCEEDED(pShellWindows->Item(vIdx, &pDisp)) && pDisp) {
                    IWebBrowser2* pWB = NULL;
                    if (SUCCEEDED(pDisp->QueryInterface(IID_IWebBrowser2, (void**)&pWB))) {
                        SHANDLE_PTR hPtr = 0;
                        if (SUCCEEDED(pWB->get_HWND(&hPtr)) && (HWND)hPtr == hwnd) {
                            pNewWebBrowser = pWB;
                            pDisp->Release();
                            break;
                        }
                        pWB->Release();
                    }
                    pDisp->Release();
                }
            }
            if (pNewWebBrowser) break;
            Sleep(20);  // Very short poll
        }

        if (pNewWebBrowser) {
            std::wstring normalizedUrl = GetComparisonPath(GetRawBrowserUrl(pNewWebBrowser));

            if (normalizedUrl.find(L"::{26EE0668-A00A-44D7-9371-BEB064C98683}") != std::wstring::npos ||
                normalizedUrl.find(L"::{21EC2020-3AEA-1069-A2DD-08002B30309D}") != std::wstring::npos) {
                ShowWindowFast(hwnd);
                g_allowedHwnds.insert(hwnd);
                pNewWebBrowser->Release();
                pShellWindows->Release();
                return;
            }

            HWND existingMainHwnd = NULL;
            HWND existingTabHwnd = NULL;
            long count = 0;
            pShellWindows->get_Count(&count);
            for (long i = 0; i < count; ++i) {
                VARIANT vIdx;
                vIdx.vt = VT_I4;
                vIdx.lVal = i;
                IDispatch* pDisp = NULL;
                if (SUCCEEDED(pShellWindows->Item(vIdx, &pDisp)) && pDisp) {
                    IWebBrowser2* pWB = NULL;
                    if (SUCCEEDED(pDisp->QueryInterface(IID_IWebBrowser2, (void**)&pWB))) {
                        SHANDLE_PTR hPtr = 0;
                        if (SUCCEEDED(pWB->get_HWND(&hPtr)) && (HWND)hPtr != hwnd) {
                            if (GetComparisonPath(GetRawBrowserUrl(pWB)) == normalizedUrl) {
                                existingMainHwnd = (HWND)hPtr;
                                existingTabHwnd = GetTabHandle(pWB);
                                pWB->Release();
                                pDisp->Release();
                                break;
                            }
                        }
                        pWB->Release();
                    }
                    pDisp->Release();
                }
            }

            if (existingMainHwnd && existingTabHwnd) {
                std::vector<HWND> tabList;
                HWND hTab = FindWindowExW(existingMainHwnd, NULL, L"ShellTabWindowClass", NULL);
                while (hTab) {
                    tabList.push_back(hTab);
                    hTab = FindWindowExW(existingMainHwnd, hTab, L"ShellTabWindowClass", NULL);
                }
                auto it = std::find(tabList.begin(), tabList.end(), existingTabHwnd);
                if (it != tabList.end()) {
                    int idx = (int)std::distance(tabList.begin(), it);
                    SendMessageW(existingMainHwnd, WM_COMMAND, CMD_SELECT_TAB_BASE + idx + 1, 0);
                }
                RestoreWindowToForeground(existingMainHwnd);
                pNewWebBrowser->Quit();
            } else {
                std::set<HWND> oldTabs = GetWindowTabHandles(mainHwnd);
                PostMessageW(mainHwnd, WM_COMMAND, CMD_NEW_TAB, 0);
                pNewWebBrowser->Quit();

                HWND newTabHwnd = NULL;
                for (int r = 0; r < 30; ++r) {
                    Sleep(50);
                    std::set<HWND> currentTabs = GetWindowTabHandles(mainHwnd);
                    for (HWND h : currentTabs) {
                        if (oldTabs.find(h) == oldTabs.end()) {
                            newTabHwnd = h;
                            break;
                        }
                    }
                    if (newTabHwnd) break;
                }

                if (newTabHwnd) {
                    bool navigated = false;
                    for (int retry = 0; retry < 15; ++retry) {
                        pShellWindows->get_Count(&count);
                        for (long i = 0; i < count; ++i) {
                            VARIANT vIdx;
                            vIdx.vt = VT_I4;
                            vIdx.lVal = i;
                            IDispatch* pDisp = NULL;
                            if (SUCCEEDED(pShellWindows->Item(vIdx, &pDisp)) && pDisp) {
                                IWebBrowser2* pWB = NULL;
                                if (SUCCEEDED(pDisp->QueryInterface(IID_IWebBrowser2, (void**)&pWB))) {
                                    if (GetTabHandle(pWB) == newTabHwnd) {
                                        NavigateToPath(pWB, normalizedUrl);
                                        navigated = true;
                                        pWB->Release();
                                        pDisp->Release();
                                        break;
                                    }
                                    pWB->Release();
                                }
                                pDisp->Release();
                            }
                        }
                        if (navigated) break;
                        Sleep(100);
                    }
                }
                RestoreWindowToForeground(mainHwnd);
            }
            pNewWebBrowser->Release();
        }
        pShellWindows->Release();
    }
}

int main() {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    Wh_Log(L"[Log] Explorer Merger start...\n");

    IConnectionPoint* pCP = NULL;
    DWORD dwCookie = 0;
    if (SUCCEEDED(
            CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL, IID_IShellWindows, (void**)&g_pShellWindows))) {
        IConnectionPointContainer* pCPC = NULL;
        if (SUCCEEDED(g_pShellWindows->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC))) {
            pCPC->FindConnectionPoint(__uuidof(DShellWindowsEvents), &pCP);
            if (pCP) pCP->Advise(new CShellWindowsSink(), &dwCookie);
            pCPC->Release();
        }
    }

    HWINEVENTHOOK hHook =
        SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, NULL, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_CHECK_WINDOWS) {
            CheckNewShellWindows();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (pCP) {
        pCP->Unadvise(dwCookie);
        pCP->Release();
    }
    if (g_pShellWindows) g_pShellWindows->Release();
    UnhookWinEvent(hHook);
    CoUninitialize();
    return 0;
}

// --- CALLBACKS ---
void MainThread() { main(); }

#include <cstdio>
#include <thread>

std::thread* g_pTabThread = nullptr;
bool g_Running = true;

BOOL WhTool_ModInit() {
    g_Running = true;
    g_pTabThread = new std::thread(MainThread);
    return TRUE;
}

void WhTool_ModUninit() {
    g_Running = false;
    PostThreadMessage(GetThreadId(g_pTabThread->native_handle()), WM_QUIT, 0, 0);
    if (g_pTabThread) {
        if (g_pTabThread->joinable()) g_pTabThread->join();
        delete g_pTabThread;
        g_pTabThread = nullptr;
    }
}

void WhTool_ModSettingsChanged() {}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() { ExitThread(0); }

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 || wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine, nullptr, nullptr, FALSE,
                                 NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi, nullptr)) {
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
