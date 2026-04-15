// ==WindhawkMod==
// @id              explorer-merger
// @name            Explorer-Merger
// @description     A tool to converts new Explorer windows into tabs.
// @version         1.0.0
// @author          Wu Junkai
// @license         MIT
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lshell32 -luser32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# [Explorer-Merger](https://github.com/WuJunkai2004/Explorer-Merger)

**Explorer-Merger** is a Windows utility designed to automatically merge newly opened File Explorer windows into tabs of an existing Explorer window. This project aims to reduce window clutter and provide a more organized file management experience on Windows 11.

This project is inspired by [ExplorerTabUtility](https://github.com/w4po/ExplorerTabUtility).

## Key Features

- **Automatic Tab Merging:** Intercepts newly created Windows Explorer (`CabinetWClass`) windows and automatically merges them into an existing Explorer window as a new tab.
- **Smart Deduplication:** If the target folder path is already open in an existing tab, the utility will focus on that tab instead of creating a duplicate one.
- **Lightweight & Efficient:** Uses Windows WinEvents and COM interfaces (`IShellWindows`, `IWebBrowser2`, `IShellBrowser`) for low-overhead window monitoring and control.
- **Dual Mode Support:**
    - **Windhawk Mod:** Can be built and loaded as a [Windhawk](https://windhawk.net/) mod for seamless background integration.
    - **Standalone Debug Tool:** Can be compiled as a standalone executable for testing and development.

## How it Works

The utility uses `SetWinEventHook` to listen for the `EVENT_OBJECT_SHOW` event on File Explorer windows. When a new Explorer window is detected, the following logic is executed:
1.  **Path Identification:** Resolves the target folder path of the new window using COM interfaces.
2.  **Window Search:** Searches for any already open Explorer window using `IShellWindows`.
3.  **Merge Logic:**
    - If an existing window is found, the new window is immediately hidden to avoid visual flicker.
    - It checks if the target path is already open in one of the existing tabs.
    - If a matching tab is found, it focuses on that tab.
    - If no matching tab is found, it sends a command to the existing window to create a new tab and navigates it to the target path.
4.  **Cleanup:** Closes the redundant new window after the merge is complete.

## Requirements

- **Windows 11:** Requires native File Explorer tab support (introduced in Windows 11 22H2).
- **C++ Compiler:** MSVC (Visual Studio) is recommended.
- **Python 3:** Required for running the build and compilation scripts.

## Installation & Building

The project uses a `Makefile` to manage the build process.
You must have Python 3 installed and added to your system PATH to use the build commands.

### 1. Building as a Windhawk Mod

To assemble the final Windhawk mod (`dist/Explorer-Merger.cpp`):

```powershell
make mod
```

### 2. Building as a Standalone Debug Executable

To compile the standalone debug tool (`dist/Explorer-Merger-Debug.exe`):

```powershell
make debug
```

> **Note:** Compiling the debug tool requires a working MSVC environment (accessible via `vcvarsall.bat`).

### 3. Cleaning Up

To remove the `dist` directory and its contents:

```powershell
make clean
```

## Credits

Special thanks to [w4po](https://github.com/w4po) for the inspiration from [ExplorerTabUtility](https://github.com/w4po/ExplorerTabUtility).

## License

This project is licensed under the [MIT License](LICENSE).


*/
// ==/WindhawkModReadme==
#include <initguid.h>

#ifndef WH_MOD
#define Wh_Log printf
#else
#include <windhawk_api.h>
#endif
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

#ifndef WH_MOD
#ifndef Wh_Log
#define Wh_Log printf
#endif
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
    std::transform(path.begin(), path.end(), path.begin(), ::towlower);
    return path;
}

std::wstring GetRawBrowserUrl(IWebBrowser2* pWebBrowser) {
    if (!pWebBrowser) return L"";
    BSTR bstrUrl = NULL;
    if (SUCCEEDED(pWebBrowser->get_LocationURL(&bstrUrl)) && bstrUrl) {
        std::wstring url = bstrUrl;
        SysFreeString(bstrUrl);
        return url;
    }
    return L"";
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
        Wh_Log("[Log] BrowseObject failed, falling back to Navigate2...\n");
        VARIANT vUrl;
        vUrl.vt = VT_BSTR;
        vUrl.bstrVal = SysAllocString(path.c_str());
        hrFinal = pWB->Navigate2(&vUrl, NULL, NULL, NULL, NULL);
        VariantClear(&vUrl);
    }
    return hrFinal;
}

std::set<HWND> g_allowedHwnds;
IShellWindows* g_pShellWindows = NULL;
#define WM_CHECK_WINDOWS (WM_USER + 1)

void HideWindowFast(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    if (!(exStyle & WS_EX_LAYERED)) SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
    SetWindowPos(hwnd, NULL, -32000, -32000, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);
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
    if (event == EVENT_OBJECT_SHOW && idObject == OBJID_WINDOW && idChild == CHILDID_SELF) {
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
                    SetForegroundWindow(existingMainHwnd);
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
                    SetForegroundWindow(mainHwnd);
                }
                pNewWebBrowser->Release();
            }
            pShellWindows->Release();
        }
    }
}

int main() {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    Wh_Log("[Log] Explorer Merger start...\n");

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
void MainThread() {
    main();
}

#include <thread>
#include <cstdio>

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

void WhTool_ModSettingsChanged() {
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk-mods/pull/1916
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

void WINAPI EntryPoint_Hook() {
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0) {
            isService = true;
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

    if (isService) {
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
