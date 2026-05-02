// ==WindhawkMod==
// @id              explorer-taskbar-overlay-icon
// @name            Explorer Taskbar Overlay Icon
// @description     Shows an overlay icon on the taskbar of Explorer windows that displays the icon of the current directory.
// @version         1.0
// @author          FireBlade
// @github          https://github.com/FireBlade211
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lshell32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Taskbar Overlay Icon
Shows a taskbar overlay icon on Explorer windows that displays the icon of the current directory.

![Screenshot](https://raw.githubusercontent.com/FireBlade211/FireBlade211/refs/heads/main/WindhawkModReadmeImages/ExplorerTaskbarOverlayIcon/screenshot.png)

*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- icon-refresh-delay: 500
  $name: Icon refresh delay
  $description: Interval in milliseconds between icon updates.
*/
// ==/WindhawkModSettings==

//#include <vector>
#include <exdisp.h>
#include <shldisp.h>
#include <shlobj.h>
#include <guiddef.h>
#include <initguid.h>
#include <string>
#include <unordered_map>

BOOL g_bEndPollThread = FALSE;
HANDLE g_hThread = nullptr;
std::unordered_map<HWND, std::wstring> g_lastPath = {};
int nDelay = 500;

DWORD WINAPI ThreadProc(LPVOID) {
    LPITEMIDLIST pidlLast = 0;
    HICON hLastIcon = 0;
    std::wstring lastDesc = {};

    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (SUCCEEDED(hr))
    {
        IShellWindows* pShellWindows;
        hr = CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&pShellWindows));

        if (SUCCEEDED(hr))
        {
            while (true) {
                long count = 0;
                pShellWindows->get_Count(&count);

                Wh_Log(L"Found %d windows", count);

                for (long i = 0; i < count; i++) {
                    VARIANT vtIndex;
                    VariantInit(&vtIndex);
                    vtIndex.vt = VT_I4;
                    vtIndex.lVal = i;

                    // this took way too long
                    // i hate explorer
                    IDispatch* pDisp = nullptr;
                    if (SUCCEEDED(pShellWindows->Item(vtIndex, &pDisp)) && pDisp) {
                        Wh_Log(L"Window %d found", i);

                        IServiceProvider* sp = nullptr;
                        hr = pDisp->QueryInterface(IID_IServiceProvider, (void**)&sp);

                        if (SUCCEEDED(hr)) {
                            IShellBrowser* sb = nullptr;
                            hr = sp->QueryService(SID_STopLevelBrowser, &sb);

                            if (SUCCEEDED(hr)) {
                                IWebBrowserApp* pBrowser;
                                hr = pDisp->QueryInterface(IID_IWebBrowserApp, (void**)&pBrowser);

                                if (SUCCEEDED(hr)) {
                                    SHANDLE_PTR hwndPtr;
                                    hr = pBrowser->get_HWND(&hwndPtr);

                                    if (SUCCEEDED(hr)) {
                                        HWND hwnd = (HWND)hwndPtr;

                                        // we do this to prevent the default overriding
                                        // behavior of SetOverlayIcon
                                        // (by default last set wins)
                                        if (hwnd == GetForegroundWindow())
                                        {
                                            IShellView* view = nullptr;
                                            hr = sb->QueryActiveShellView(&view);

                                            if (SUCCEEDED(hr)) {
                                                IFolderView* fv = nullptr;
                                                hr = view->QueryInterface(IID_IFolderView, (void**)&fv);

                                                if (SUCCEEDED(hr)) {
                                                    IPersistFolder2* pf = nullptr;
                                                    hr = fv->GetFolder(IID_PPV_ARGS(&pf));

                                                    if (SUCCEEDED(hr)) {
                                                        PIDLIST_ABSOLUTE pidl;
                                                        hr = pf->GetCurFolder(&pidl);

                                                        if (SUCCEEDED(hr)) {
                                                            ITaskbarList3* pTaskbarList;
                                                            hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pTaskbarList));

                                                            if (SUCCEEDED(hr)) {
                                                                if (!ILIsEqual(pidlLast, pidl)) {
                                                                    SHFILEINFOW sfi = {};
                                                                    SHGetFileInfoW(
                                                                        (LPCWSTR)pidl,
                                                                        0,
                                                                        &sfi,
                                                                        sizeof(sfi),
                                                                        SHGFI_PIDL | SHGFI_ICON | SHGFI_SMALLICON | SHGFI_DISPLAYNAME
                                                                    );

                                                                    if (sfi.hIcon)
                                                                    {
                                                                        pTaskbarList->SetOverlayIcon(hwnd, sfi.hIcon, sfi.szDisplayName);
                                                                        
                                                                        lastDesc = std::wstring(sfi.szDisplayName);
                                                                        DestroyIcon(hLastIcon);
                                                                        hLastIcon = sfi.hIcon;
                                                                    }

                                                                    CoTaskMemFree(pidlLast);
                                                                    pidlLast = pidl;
                                                                } else {
                                                                    if (hLastIcon != NULL)
                                                                        pTaskbarList->SetOverlayIcon(hwnd, hLastIcon, lastDesc.c_str());

                                                                    CoTaskMemFree(pidl);
                                                                }

                                                                pTaskbarList->Release();
                                                            }
    
                                                        } else {
                                                            Wh_Log(L"Error fetching item ID list for window %d with code: %d", i, hr);
                                                        }

                                                        pf->Release();
                                                    } else {
                                                        Wh_Log(L"Error fetching persist folder for window %d with code: %d", i, hr);
                                                    }

                                                    fv->Release();
                                                } else {
                                                    Wh_Log(L"Error fetching folder view for window %d with code: %d", i, hr);
                                                }

                                                view->Release();
                                            } else {
                                                Wh_Log(L"Error fetching shell view for window %d with code: %d", i, hr);
                                            }
                                        }
                                    }

                                    pBrowser->Release();
                                }

                                sb->Release();
                            } else {
                                Wh_Log(L"Error fetching shell browser for window %d with code: %d", i, hr);
                            }

                            sp->Release();
                        } else {
                            Wh_Log(L"Error fetching service provider for window %d with code: %d", i, hr);
                        }

                        pDisp->Release();
                    }

                    VariantClear(&vtIndex);
                }

                if (g_bEndPollThread) break;

                Sleep(nDelay);
            }

            pShellWindows->Release();
        } else {
            Wh_Log(L"Shell window creation failed: %d", hr);
        }

        CoUninitialize();
    } else {
        Wh_Log(L"COM init failed: %d", hr);
    }

    if (pidlLast)
        CoTaskMemFree(pidlLast);

    if (hLastIcon)
        DestroyIcon(hLastIcon);

    return 0;
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL WhTool_ModInit() {
    Wh_Log(L"Init");

    nDelay = Wh_GetIntSetting(L"icon-refresh-delay");

    // Explorer does not call SetCurrentDirectory
    // so we have to poll instead (yes I tested that)
    g_hThread = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void WhTool_ModUninit() {
    Wh_Log(L"Uninit");

    g_bEndPollThread = TRUE;
    WaitForSingleObject(g_hThread, INFINITE);
    CloseHandle(g_hThread);

    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (SUCCEEDED(hr))
    {
        IShellWindows* pShellWindows;
        hr = CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&pShellWindows));

        if (SUCCEEDED(hr))
        {
            long count = 0;
            pShellWindows->get_Count(&count);

            for (long i = 0; i < count; i++) {
                VARIANT vtIndex;
                VariantInit(&vtIndex);
                vtIndex.vt = VT_I4;
                vtIndex.lVal = i;

                IDispatch* pDisp = nullptr;
                if (SUCCEEDED(pShellWindows->Item(vtIndex, &pDisp)) && pDisp) {
                    IWebBrowserApp* pBrowser;
                    hr = pDisp->QueryInterface(IID_IWebBrowserApp, (void**)&pBrowser);

                    if (SUCCEEDED(hr)) {
                        SHANDLE_PTR hwndPtr;
                        hr = pBrowser->get_HWND(&hwndPtr);

                        if (SUCCEEDED(hr)) {
                            HWND hwnd = (HWND)hwndPtr;
                            
                            ITaskbarList3* pTaskbarList;
                            hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pTaskbarList));

                            if (SUCCEEDED(hr)) {
                                // clear the icon
                                pTaskbarList->SetOverlayIcon(hwnd, NULL, L"");
                            
                                pTaskbarList->Release();
                            }
                        }

                        pBrowser->Release();
                    }

                    pDisp->Release();
                }
            }

            pShellWindows->Release();
        }

        CoUninitialize();
    }
}

// The mod setting were changed, reload them.
void WhTool_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    nDelay = Wh_GetIntSetting(L"icon-refresh-delay");
}

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

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
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
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

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
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
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