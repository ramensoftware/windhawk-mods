// ==WindhawkMod==
// @id              explorer-single-window-tabs
// @name            Explorer Single Window Tabs
// @description     Redirects new File Explorer windows to open as tabs in a single existing window
// @version         0.7
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

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enable single-window mode
  $description: >-
    When enabled, new Explorer windows are redirected to tabs in the primary
    window. Hold Shift to bypass.
*/
// ==/WindhawkModSettings==

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

static bool g_enabled = true;

struct RedirectInfo {
    HWND newWindow;
    HWND primaryWindow;
};

// ---------------------------------------------------------------------------
// Find an existing visible CabinetWClass window (cross-process)
// ---------------------------------------------------------------------------
struct FindExplorerCtx {
    HWND exclude;
    HWND result;
};

static BOOL CALLBACK EnumFindExplorer(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<FindExplorerCtx*>(lParam);
    if (hwnd == ctx->exclude)
        return TRUE;

    WCHAR cls[64];
    if (GetClassNameW(hwnd, cls, _countof(cls)) &&
        wcscmp(cls, WC_CABINET) == 0 &&
        IsWindowVisible(hwnd)) {
        ctx->result = hwnd;
        return FALSE;
    }
    return TRUE;
}

static HWND FindExistingExplorer(HWND exclude) {
    FindExplorerCtx ctx{exclude, nullptr};
    EnumWindows(EnumFindExplorer, reinterpret_cast<LPARAM>(&ctx));
    return ctx.result;
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
    auto* info      = static_cast<RedirectInfo*>(param);
    HWND  newWnd    = info->newWindow;
    HWND  primary   = info->primaryWindow;
    delete info;

    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
        return 1;

    // Wait for the new window to finish navigating (max ~6s)
    std::wstring path;
    for (int attempt = 0; attempt < 60 && IsWindow(newWnd); attempt++) {
        Sleep(100);
        path = GetExplorerWindowPath(newWnd);
        if (!path.empty()) break;
    }

    // Re-verify primary is still alive
    if (!IsWindow(primary))
        primary = FindExistingExplorer(newWnd);

    if (path.empty() || !primary || !IsWindow(primary) ||
        newWnd == primary) {
        CoUninitialize();
        return 0;
    }

    // Hide and close the duplicate window
    ShowWindow(newWnd, SW_HIDE);
    Sleep(30);
    PostMessageW(newWnd, WM_CLOSE, 0, 0);

    // Snapshot tab count before creating a new tab
    int prevCount = GetShellWindowCount();

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
    bool isCabinet = lpClassName && !IS_INTRESOURCE(lpClassName) &&
                     wcscmp(lpClassName, WC_CABINET) == 0;

    HWND hwnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                          dwStyle, X, Y, nWidth, nHeight,
                                          hWndParent, hMenu, hInstance,
                                          lpParam);

    if (hwnd && isCabinet && g_enabled) {
        HWND existing = FindExistingExplorer(hwnd);
        if (existing) {
            // Shift held = bypass, allow separate window
            if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000)) {
                auto* ri = new RedirectInfo{hwnd, existing};
                HANDLE hThread =
                    CreateThread(nullptr, 0, RedirectThread, ri, 0, nullptr);
                if (hThread) CloseHandle(hThread);
            }
        }
    }

    return hwnd;
}

// ---------------------------------------------------------------------------
// Mod lifecycle
// ---------------------------------------------------------------------------
static void LoadSettings() {
    g_enabled = Wh_GetIntSetting(L"enabled") != 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    Wh_SetFunctionHook(reinterpret_cast<void*>(CreateWindowExW),
                        reinterpret_cast<void*>(CreateWindowExW_Hook),
                        reinterpret_cast<void**>(&CreateWindowExW_Original));
    return TRUE;
}

void Wh_ModUninit() {}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
