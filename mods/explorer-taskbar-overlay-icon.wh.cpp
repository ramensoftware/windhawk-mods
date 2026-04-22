// ==WindhawkMod==
// @id              explorer-taskbar-overlay-icon
// @name            Explorer Taskbar Overlay Icon
// @description     Shows an overlay icon on the taskbar of Explorer windows that displays the icon of the current directory.
// @version         1.0
// @author          FireBlade
// @github          https://github.com/FireBlade211
// @include         explorer.exe
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
    std::wstring last;

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
                                                            SHFILEINFOW sfi = {};
                                                            SHGetFileInfoW(
                                                                (LPCWSTR)pidl,
                                                                0,
                                                                &sfi,
                                                                sizeof(sfi),
                                                                SHGFI_PIDL | SHGFI_ICON | SHGFI_SMALLICON | SHGFI_DISPLAYNAME
                                                            );

                                                            ITaskbarList3* pTaskbarList;
                                                            hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pTaskbarList));

                                                            if (SUCCEEDED(hr)) {
                                                                pTaskbarList->SetOverlayIcon(hwnd, sfi.hIcon, sfi.szDisplayName);
                                                                
                                                                // the taskbar copies the icon
                                                                DestroyIcon(sfi.hIcon);

                                                                pTaskbarList->Release();
                                                            }

                                                            CoTaskMemFree(pidl);
                                                        } else {
                                                            Wh_Log(L"Error fetching item ID list for window %d with code: %d", i, hr);
                                                        }
                                                    } else {
                                                        Wh_Log(L"Error fetching persist folder for window %d with code: %d", i, hr);
                                                    }
                                                } else {
                                                    Wh_Log(L"Error fetching folder view for window %d with code: %d", i, hr);
                                                }
                                            } else {
                                                Wh_Log(L"Error fetching shell view for window %d with code: %d", i, hr);
                                            }
                                        }
                                    }
                                }
                            } else {
                                Wh_Log(L"Error fetching shell browser for window %d with code: %d", i, hr);
                            }
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

    return 0;
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    nDelay = Wh_GetIntSetting(L"icon-refresh-delay");

    // Explorer does not call SetCurrentDirectory
    // so we have to poll instead (yes I tested that)
    g_hThread = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
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
            while (true) {
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
            }
        }

        CoUninitialize();
    }
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    nDelay = Wh_GetIntSetting(L"icon-refresh-delay");
}
