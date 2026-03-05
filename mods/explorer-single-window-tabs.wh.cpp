// ==WindhawkMod==
// @id              explorer-single-window-tabs
// @name            Explorer Single Window Tabs
// @description     Redirects new File Explorer windows to open as tabs in a single existing window
// @version         0.8
// @author          ALMAS CP
// @github          https://github.com/almas-cp
// @homepage        https://github.com/almas-cp
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lshlwapi -lshell32 -luuid -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Single Window Tabs

Prevents File Explorer from opening multiple windows. Every new folder is
redirected into a **new tab** inside the first (primary) Explorer window.

## How It Works

Uses internal Windows Shell COM interfaces and Explorer's undocumented
`WM_COMMAND` messages to programmatically create tabs and navigate them.

1. Hooks `CreateWindowExW` to detect new Explorer windows.
2. Extracts the navigation path via `IShellWindows` / `IWebBrowser2`.
3. Closes the new window and sends `WM_COMMAND 0xA21B` to
   `ShellTabWindowClass` to create a tab.
4. Navigates the new tab using `IWebBrowser2::Navigate2`.

## Bypass

Hold **Shift** while opening a folder to force a separate window.

## Requirements

- **Windows 11 22H2** or later (tabbed File Explorer).
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <shlobj.h>
#include <exdisp.h>
#include <shlwapi.h>
#include <string>

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------
#define WC_CABINET   L"CabinetWClass"
#define WC_SHELLTAB  L"ShellTabWindowClass"
#define CMD_NEW_TAB  0xA21B  // Internal Explorer "new tab" command

// CLSIDs for special windows that should NOT be redirected into tabs
static const WCHAR kControlPanelCLSID[] =
    L"::{26EE0668-A00A-44D7-9371-BEB064C98683}";
static const WCHAR kRecycleBinCLSID[] =
    L"::{645FF040-5081-101B-9F08-00AA002F954E}";

static HWND g_lastPrimaryWindow = nullptr;
static thread_local bool g_creatingExplorerWindow = false;

// Forward declaration (needed by RedirectThread before ShowWindow hook)
using ShowWindow_t = decltype(&ShowWindow);
static ShowWindow_t ShowWindow_Original;

struct RedirectInfo {
    HWND newWindow;
    HWND primaryWindow;
};

// ---------------------------------------------------------------------------
// Check whether a path is a normal folder that can be opened as a tab.
// Returns false for Control Panel and other special shell locations.
// ---------------------------------------------------------------------------
static bool IsRedirectablePath(const std::wstring& path) {
    if (path.find(kControlPanelCLSID) != std::wstring::npos)
        return false;

    if (path.find(kRecycleBinCLSID) != std::wstring::npos)
        return false;

    if (path.compare(0, 9, L"shell:::{") == 0)
        return false;

    return true;
}

// ---------------------------------------------------------------------------
// Restore a window that was hidden for redirect but needs to be shown
// (e.g. Control Panel, or abort cases).
// ---------------------------------------------------------------------------
static void AbortRedirect(HWND hwnd) {
    if (g_lastPrimaryWindow == hwnd)
        g_lastPrimaryWindow = nullptr;

    if (!IsWindow(hwnd))
        return;

    // Remove transparency and restore the window
    LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_LAYERED) {
        SetWindowLongW(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
    }
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                 SWP_NOACTIVATE | SWP_FRAMECHANGED);
    ShowWindow_Original(hwnd, SW_SHOW);
}

// ---------------------------------------------------------------------------
// Find an existing visible File Explorer window (not Control Panel).
// Uses IShellWindows COM to check each window's actual path.
// ---------------------------------------------------------------------------
static HWND FindExistingExplorer(HWND exclude) {
    IShellWindows* pSW = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&pSW))))
        return nullptr;

    long count = 0;
    pSW->get_Count(&count);

    HWND result = nullptr;
    for (long i = 0; i < count; i++) {
        VARIANT idx;
        VariantInit(&idx);
        idx.vt   = VT_I4;
        idx.lVal = i;

        IDispatch* pDisp = nullptr;
        if (FAILED(pSW->Item(idx, &pDisp)) || !pDisp)
            continue;

        IWebBrowser2* pBrowser = nullptr;
        if (SUCCEEDED(pDisp->QueryInterface(IID_PPV_ARGS(&pBrowser)))) {
            SHANDLE_PTR hwndPtr = 0;
            pBrowser->get_HWND(&hwndPtr);
            HWND hwnd =
                reinterpret_cast<HWND>(static_cast<ULONG_PTR>(hwndPtr));

            if (hwnd && hwnd != exclude && IsWindowVisible(hwnd)) {
                // Get the window's path to verify it's real File Explorer
                std::wstring path;

                BSTR url = nullptr;
                if (SUCCEEDED(pBrowser->get_LocationURL(&url)) && url &&
                    wcslen(url) > 0) {
                    WCHAR filePath[MAX_PATH * 2];
                    DWORD len = MAX_PATH * 2;
                    if (SUCCEEDED(PathCreateFromUrlW(url, filePath, &len, 0)))
                        path = filePath;
                    else
                        path = url;
                    SysFreeString(url);
                } else {
                    if (url) SysFreeString(url);

                    // Fallback: PIDL path (for "This PC", etc.)
                    IServiceProvider* pSP = nullptr;
                    if (SUCCEEDED(
                            pDisp->QueryInterface(IID_PPV_ARGS(&pSP)))) {
                        IShellBrowser* pSB = nullptr;
                        if (SUCCEEDED(pSP->QueryService(
                                SID_STopLevelBrowser, IID_PPV_ARGS(&pSB)))) {
                            IShellView* pSV = nullptr;
                            if (SUCCEEDED(
                                    pSB->QueryActiveShellView(&pSV))) {
                                IFolderView* pFV = nullptr;
                                if (SUCCEEDED(pSV->QueryInterface(
                                        IID_PPV_ARGS(&pFV)))) {
                                    IPersistFolder2* pPF = nullptr;
                                    if (SUCCEEDED(pFV->GetFolder(
                                            IID_PPV_ARGS(&pPF)))) {
                                        PIDLIST_ABSOLUTE pidl = nullptr;
                                        if (SUCCEEDED(
                                                pPF->GetCurFolder(&pidl))) {
                                            PWSTR name = nullptr;
                                            if (SUCCEEDED(
                                                    SHGetNameFromIDList(
                                                        pidl,
                                                        SIGDN_DESKTOPABSOLUTEPARSING,
                                                        &name))) {
                                                path = name;
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

                // Only use this window if its path is redirectable
                // (i.e. not Control Panel or other special shell locations)
                if (!path.empty() && IsRedirectablePath(path)) {
                    result = hwnd;
                    pBrowser->Release();
                    pDisp->Release();
                    break;
                }
            }
            pBrowser->Release();
        }
        pDisp->Release();
    }
    pSW->Release();
    return result;
}

// ---------------------------------------------------------------------------
// COM: Extract navigation path from an Explorer window by HWND
// ---------------------------------------------------------------------------
static std::wstring GetExplorerWindowPath(HWND targetHwnd) {
    std::wstring result;

    IShellWindows* pSW = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&pSW))))
        return result;

    long count = 0;
    pSW->get_Count(&count);

    for (long i = 0; i < count; i++) {
        VARIANT idx;
        VariantInit(&idx);
        idx.vt   = VT_I4;
        idx.lVal = i;

        IDispatch* pDisp = nullptr;
        if (FAILED(pSW->Item(idx, &pDisp)) || !pDisp)
            continue;

        IWebBrowser2* pBrowser = nullptr;
        if (SUCCEEDED(pDisp->QueryInterface(IID_PPV_ARGS(&pBrowser)))) {
            SHANDLE_PTR hwndPtr = 0;
            pBrowser->get_HWND(&hwndPtr);

            if (reinterpret_cast<HWND>(static_cast<ULONG_PTR>(hwndPtr)) ==
                targetHwnd) {
                // Method 1: LocationURL → filesystem path
                BSTR url = nullptr;
                if (SUCCEEDED(pBrowser->get_LocationURL(&url)) && url &&
                    wcslen(url) > 0) {
                    WCHAR path[MAX_PATH * 2];
                    DWORD pathLen = MAX_PATH * 2;
                    if (SUCCEEDED(PathCreateFromUrlW(url, path, &pathLen, 0)))
                        result = path;
                    else
                        result = url;
                    SysFreeString(url);
                } else {
                    if (url) SysFreeString(url);
                }

                // Method 2: PIDL via IShellBrowser (for special folders)
                if (result.empty()) {
                    IServiceProvider* pSP = nullptr;
                    if (SUCCEEDED(
                            pDisp->QueryInterface(IID_PPV_ARGS(&pSP)))) {
                        IShellBrowser* pSB = nullptr;
                        if (SUCCEEDED(pSP->QueryService(
                                SID_STopLevelBrowser, IID_PPV_ARGS(&pSB)))) {
                            IShellView* pSV = nullptr;
                            if (SUCCEEDED(
                                    pSB->QueryActiveShellView(&pSV))) {
                                IFolderView* pFV = nullptr;
                                if (SUCCEEDED(pSV->QueryInterface(
                                        IID_PPV_ARGS(&pFV)))) {
                                    IPersistFolder2* pPF = nullptr;
                                    if (SUCCEEDED(pFV->GetFolder(
                                            IID_PPV_ARGS(&pPF)))) {
                                        PIDLIST_ABSOLUTE pidl = nullptr;
                                        if (SUCCEEDED(
                                                pPF->GetCurFolder(&pidl))) {
                                            PWSTR name = nullptr;
                                            if (SUCCEEDED(
                                                    SHGetNameFromIDList(
                                                        pidl,
                                                        SIGDN_DESKTOPABSOLUTEPARSING,
                                                        &name))) {
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

                pBrowser->Release();
                pDisp->Release();
                break;
            }
            pBrowser->Release();
        }
        pDisp->Release();
    }
    pSW->Release();
    return result;
}

// ---------------------------------------------------------------------------
// COM: Count current IShellWindows entries
// ---------------------------------------------------------------------------
static int GetShellWindowCount() {
    IShellWindows* pSW = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&pSW))))
        return 0;
    long count = 0;
    pSW->get_Count(&count);
    pSW->Release();
    return static_cast<int>(count);
}

// ---------------------------------------------------------------------------
// COM: Navigate the most recently added tab to the target path
// ---------------------------------------------------------------------------
static bool NavigateNewTab(HWND primaryHwnd, const std::wstring& path,
                           int prevCount) {
    IShellWindows* pSW = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&pSW))))
        return false;

    long count = 0;
    pSW->get_Count(&count);

    bool success = false;
    for (long i = count - 1; i >= 0; i--) {
        VARIANT idx;
        VariantInit(&idx);
        idx.vt   = VT_I4;
        idx.lVal = i;

        IDispatch* pDisp = nullptr;
        if (FAILED(pSW->Item(idx, &pDisp)) || !pDisp)
            continue;

        IWebBrowser2* pBrowser = nullptr;
        if (SUCCEEDED(pDisp->QueryInterface(IID_PPV_ARGS(&pBrowser)))) {
            SHANDLE_PTR hwndPtr = 0;
            pBrowser->get_HWND(&hwndPtr);

            if (reinterpret_cast<HWND>(static_cast<ULONG_PTR>(hwndPtr)) ==
                primaryHwnd) {
                // Only navigate empty/home tabs to avoid clobbering
                BSTR url = nullptr;
                pBrowser->get_LocationURL(&url);
                bool isEmptyOrNew =
                    (!url || wcslen(url) == 0 || i >= prevCount);
                if (url) SysFreeString(url);

                if (isEmptyOrNew) {
                    VARIANT target;
                    VariantInit(&target);
                    target.vt      = VT_BSTR;
                    target.bstrVal = SysAllocString(path.c_str());

                    VARIANT empty;
                    VariantInit(&empty);

                    HRESULT hr = pBrowser->Navigate2(&target, &empty,
                                                      &empty, &empty, &empty);
                    SysFreeString(target.bstrVal);

                    Wh_Log(L"Navigate2 [%d] hr=0x%08X", static_cast<int>(i),
                           hr);
                    success = SUCCEEDED(hr);

                    pBrowser->Release();
                    pDisp->Release();
                    break;
                }
            }
            pBrowser->Release();
        }
        pDisp->Release();
    }
    pSW->Release();
    return success;
}

// ---------------------------------------------------------------------------
// Redirect thread: close new window → create tab → navigate
// ---------------------------------------------------------------------------
static DWORD WINAPI RedirectThread(LPVOID param) {
    auto* info    = static_cast<RedirectInfo*>(param);
    HWND  newWnd  = info->newWindow;
    HWND  primary = info->primaryWindow;
    delete info;

    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
        return 1;

    // Wait for the new window to finish navigating (max ~2s)
    std::wstring path;
    for (int attempt = 0; attempt < 40 && IsWindow(newWnd); attempt++) {
        Sleep(50);
        path = GetExplorerWindowPath(newWnd);
        if (!path.empty()) break;
    }

    // Re-verify primary is still alive
    if (!IsWindow(primary))
        primary = FindExistingExplorer(newWnd);

    if (path.empty() || !primary || !IsWindow(primary) ||
        newWnd == primary) {
        AbortRedirect(newWnd);
        CoUninitialize();
        return 0;
    }

    // Don't redirect Control Panel or other special shell windows
    if (!IsRedirectablePath(path)) {
        Wh_Log(L"Non-redirectable path, allowing window: %s", path.c_str());
        AbortRedirect(newWnd);
        CoUninitialize();
        return 0;
    }

    // Snapshot tab count before creating a new tab
    int prevCount = GetShellWindowCount() - 1;

    // Close the duplicate window (already hidden via WS_VISIBLE strip)
    PostMessageW(newWnd, WM_CLOSE, 0, 0);

    // Create a new tab via internal WM_COMMAND
    HWND shellTab = FindWindowExW(primary, nullptr, WC_SHELLTAB, nullptr);
    if (!shellTab) {
        Wh_Log(L"ShellTabWindowClass not found on primary %p", primary);
        CoUninitialize();
        return 0;
    }
    PostMessageW(shellTab, WM_COMMAND, CMD_NEW_TAB, 0);

    // Wait for the new tab to register in IShellWindows (max ~3s)
    bool navigated = false;
    for (int wait = 0; wait < 30; wait++) {
        Sleep(100);
        if (GetShellWindowCount() > prevCount) {
            navigated = NavigateNewTab(primary, path, prevCount);
            break;
        }
    }

    // Fallback: try navigating the last matching entry
    if (!navigated)
        navigated = NavigateNewTab(primary, path, 0);

    // Bring primary window to foreground
    if (IsIconic(primary))
        ShowWindow(primary, SW_RESTORE);

    DWORD pid = 0;
    GetWindowThreadProcessId(primary, &pid);
    AllowSetForegroundWindow(pid);
    SetForegroundWindow(primary);

    Wh_Log(L"%s → %s", navigated ? L"Redirected" : L"Failed", path.c_str());

    CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// Hook: CreateWindowExW
// ---------------------------------------------------------------------------
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName,
                                  LPCWSTR lpWindowName, DWORD dwStyle,
                                  int X, int Y, int nWidth, int nHeight,
                                  HWND hWndParent, HMENU hMenu,
                                  HINSTANCE hInstance, LPVOID lpParam) {
    auto original = [=]() {
        return CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                        dwStyle, X, Y, nWidth, nHeight,
                                        hWndParent, hMenu, hInstance, lpParam);
    };

    bool isCabinet = lpClassName && !IS_INTRESOURCE(lpClassName) &&
                     wcscmp(lpClassName, WC_CABINET) == 0;
    if (!isCabinet) {
        return original();
    }

    // Bypass if Shift is held
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
        return original();
    }

    HWND existing = FindExistingExplorer(nullptr);

    // First window
    if (!existing) {
        return original();
    }

    Wh_Log(L"Creating Explorer window");
    g_creatingExplorerWindow = true;

    HWND hwnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                          dwStyle, X, Y, nWidth, nHeight,
                                          hWndParent, hMenu, hInstance,
                                          lpParam);

    Wh_Log(L"Created window %p", hwnd);
    g_creatingExplorerWindow = false;

    if (hwnd) {
        // Make window fully transparent instead of stripping WS_VISIBLE.
        // This keeps WS_VISIBLE set so the window registers in IShellWindows
        // (required for path extraction of special folders like Recycle Bin)
        // but the user never sees it.
        SetWindowLongW(hwnd, GWL_EXSTYLE,
                       GetWindowLongW(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);

        g_lastPrimaryWindow = hwnd;

        // Redirect — window is transparent, thread will close it
        auto* ri = new RedirectInfo{hwnd, existing};
        HANDLE hThread =
            CreateThread(nullptr, 0, RedirectThread, ri, 0, nullptr);
        if (hThread) CloseHandle(hThread);
    }

    return hwnd;
}

// ---------------------------------------------------------------------------
// Hook: ShowWindow — prevent the redirected window from flashing
// ---------------------------------------------------------------------------
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (g_creatingExplorerWindow || hWnd == g_lastPrimaryWindow) {
        WCHAR className[64];
        if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
            wcscmp(className, WC_CABINET) == 0) {
            Wh_Log(
                L"Blocked ShowWindow for %p %s", hWnd,
                g_creatingExplorerWindow ? L"(creating)" : L"(last created)");
            return TRUE;
        }
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

// ---------------------------------------------------------------------------
// Mod lifecycle
// ---------------------------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_SetFunctionHook(reinterpret_cast<void*>(CreateWindowExW),
                        reinterpret_cast<void*>(CreateWindowExW_Hook),
                        reinterpret_cast<void**>(&CreateWindowExW_Original));
    Wh_SetFunctionHook(reinterpret_cast<void*>(ShowWindow),
                        reinterpret_cast<void*>(ShowWindow_Hook),
                        reinterpret_cast<void**>(&ShowWindow_Original));
    return TRUE;
}

void Wh_ModUninit() {}
