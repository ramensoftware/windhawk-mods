// ==WindhawkMod==
// @id              explorer-tabs-session-saver-revamped
// @name            Explorer Tabs Session Saver Revamped
// @description     Saves and restores File Explorer tabs on reopen, without blank extra tabs or sidebar scroll glitches
// @version         1.0.0
// @author          Peter2oo1
// @github          https://github.com/Peter2oo1
// @include         explorer.exe
// @compilerOptions -lshlwapi -lole32 -loleaut32 -lshell32 -luuid -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Tabs Session Saver Revamped

This mod saves your open File Explorer tabs and automatically restores them whenever you reopen File Explorer.

It is an enhanced successor to [Explorer Tabs Session Saver](https://windhawk.net/mods/explorer-tabs-session-saver) by [noX1st](https://github.com/noX1st), fixing several core issues and edge cases.

![Demonstration (original)](https://i.imgur.com/5ehPFaL.gif)

### Improvements Over the Original Mod
- **No Extra "Blank" Tab on Restore:** Navigates the initial left-most tab in-place (`IWebBrowser2::Navigate` / `IShellBrowser::BrowseObject`) instead of spawning a separate `ShellExecuteExW` call for the first saved tab, preventing a blank or duplicate default tab from being left on the far left.
- **Fixes Sidebar Auto-Scrolling to the Bottom:** Prevents File Explorer's Navigation Pane (`SysTreeView32`) from jumping or flickering to the bottom when opening a window and restoring tabs with Explorer Tab Utility.
- **Persistent Session Protection:** Opening File Explorer and closing it immediately without interacting with the restored tabs will no longer delete or lose your saved session.
- **Atomic Multi-Tab Window Close:** Ignores tabs belonging to a top-level Explorer window that is already in the middle of closing, preventing partial tab lists from overwriting your session when closing a multi-tab window.
- **Full Unicode (UTF-8) Path Support:** Properly saves and restores folder paths containing non-ASCII, accented, or special Unicode characters.
- **Instant Mod Reload / Unload:** Uses waitable stop events and clean cross-thread unsubclassing instead of blocking sleeps.

### Notes
- Works independently of the built-in Windows feature *"Restore previous folder windows at log-on"*.
- Session file location: `%AppData%\Windhawk\ModsData\explorer-tabs-session-saver-plus\LastExplorerTabs.txt` (or under your Windhawk `ModsData` directory).
- **Important:** To restore tabs into a single File Explorer window, [Explorer Tab Utility](https://github.com/w4po/ExplorerTabUtility) (with its **"Window Hook"** feature enabled) or a similar tab-merging utility is required. Otherwise, each tab will be restored in its own separate window.
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <exdisp.h>
#include <vector>
#include <string>
#include <olectl.h>
#include <algorithm>

HANDLE g_hMainThread = NULL;
HANDLE g_hStopEvent  = NULL;
bool   g_bIsRunning  = false;
std::vector<std::wstring> g_lastKnownTabs;
static volatile LONG g_restoreScheduled = 0;
static volatile LONG g_isRestoring = 0;
static volatile LONG g_activeRestoreThreads = 0;
static volatile LONGLONG g_windowOpenLockUntilTick = 0;

static const UINT_PTR SUBCLASS_ID = 1;
static UINT g_msgUnsubclass = 0;
static CRITICAL_SECTION g_treeCs;
static std::vector<HWND> g_subclassedTrees;
static thread_local HWND tls_lockedTreeHwnd = NULL;

typedef int (WINAPI *SetScrollInfo_t)(HWND, int, LPCSCROLLINFO, BOOL);
SetScrollInfo_t pSetScrollInfo_Orig = NULL;

typedef int (WINAPI *SetScrollPos_t)(HWND, int, int, BOOL);
SetScrollPos_t pSetScrollPos_Orig = NULL;

typedef int (WINAPI *ScrollWindowEx_t)(HWND, int, int, const RECT*, const RECT*, HRGN, LPRECT, UINT);
ScrollWindowEx_t pScrollWindowEx_Orig = NULL;

typedef BOOL (WINAPI *UpdateWindow_t)(HWND);
UpdateWindow_t pUpdateWindow_Orig = NULL;

int WINAPI SetScrollInfo_Hook(HWND hwnd, int nBar, LPCSCROLLINFO lpsi, BOOL redraw) {
    if (tls_lockedTreeHwnd && hwnd == tls_lockedTreeHwnd && nBar == SB_VERT && lpsi) {
        if ((lpsi->fMask & SIF_POS) && lpsi->nPos > 0) {
            SCROLLINFO si = *lpsi;
            si.nPos = 0;
            // Allow redraw if range/page is also being updated so thumb size stays accurate at pos 0
            BOOL allowRedraw = (lpsi->fMask & (SIF_RANGE | SIF_PAGE)) ? redraw : FALSE;
            return pSetScrollInfo_Orig(hwnd, nBar, &si, allowRedraw);
        }
    }
    return pSetScrollInfo_Orig(hwnd, nBar, lpsi, redraw);
}

int WINAPI SetScrollPos_Hook(HWND hwnd, int nBar, int nPos, BOOL bRedraw) {
    if (tls_lockedTreeHwnd && hwnd == tls_lockedTreeHwnd && nBar == SB_VERT && nPos > 0) {
        return pSetScrollPos_Orig(hwnd, nBar, 0, FALSE);
    }
    return pSetScrollPos_Orig(hwnd, nBar, nPos, bRedraw);
}

int WINAPI ScrollWindowEx_Hook(
    HWND hWnd, int dx, int dy, const RECT* prcScroll, const RECT* prcClip,
    HRGN hrgnUpdate, LPRECT prcUpdate, UINT flags
) {
    if (tls_lockedTreeHwnd && hWnd == tls_lockedTreeHwnd && dy != 0) {
        return SIMPLEREGION;
    }
    return pScrollWindowEx_Orig(hWnd, dx, dy, prcScroll, prcClip, hrgnUpdate, prcUpdate, flags);
}

BOOL WINAPI UpdateWindow_Hook(HWND hWnd) {
    if (tls_lockedTreeHwnd && hWnd == tls_lockedTreeHwnd) {
        return TRUE;
    }
    return pUpdateWindow_Orig(hWnd);
}

struct TreeViewLockState {
    ULONGLONG createdTick;
    ULONGLONG lockUntilTick;
    ULONGLONG firstVisibleWithItemsTick;
    bool userInteracted;
    bool inScrollReset;
};

void RegisterSubclassedTree(HWND hwnd) {
    EnterCriticalSection(&g_treeCs);
    if (std::find(g_subclassedTrees.begin(), g_subclassedTrees.end(), hwnd) == g_subclassedTrees.end()) {
        g_subclassedTrees.push_back(hwnd);
    }
    LeaveCriticalSection(&g_treeCs);
}

void UnregisterSubclassedTree(HWND hwnd) {
    EnterCriticalSection(&g_treeCs);
    auto it = std::find(g_subclassedTrees.begin(), g_subclassedTrees.end(), hwnd);
    if (it != g_subclassedTrees.end()) {
        g_subclassedTrees.erase(it);
    }
    LeaveCriticalSection(&g_treeCs);
}

bool IsExplorerSidebarTreeView(HWND hwnd, HWND hWndParent) {
    HWND curr = hWndParent ? hWndParent : GetParent(hwnd);
    while (curr) {
        wchar_t cls[64];
        if (GetClassNameW(curr, cls, ARRAYSIZE(cls))) {
            if (lstrcmpW(cls, L"NamespaceTreeControl") == 0 ||
                lstrcmpW(cls, L"CabinetWClass") == 0) {
                return true;
            }
        }
        curr = GetParent(curr);
    }
    return false;
}

bool IsTreeViewScrolledDownViaSubclass(HWND hwnd) {
    if (GetScrollPos(hwnd, SB_VERT) > 0) {
        return true;
    }
    HTREEITEM hRoot = (HTREEITEM)DefSubclassProc(hwnd, TVM_GETNEXTITEM, TVGN_ROOT, 0);
    if (hRoot) {
        HTREEITEM hFirstVis = (HTREEITEM)DefSubclassProc(hwnd, TVM_GETNEXTITEM, TVGN_FIRSTVISIBLE, 0);
        if (hFirstVis && hFirstVis != hRoot) {
            return true;
        }
    }
    return false;
}

void ResetTreeViewScrollToTopViaSubclass(HWND hwnd) {
    HTREEITEM hRoot = (HTREEITEM)DefSubclassProc(hwnd, TVM_GETNEXTITEM, TVGN_ROOT, 0);
    if (hRoot) {
        DefSubclassProc(hwnd, TVM_SELECTITEM, TVGN_FIRSTVISIBLE, (LPARAM)hRoot);
    }
    DefSubclassProc(hwnd, WM_VSCROLL, MAKEWPARAM(SB_TOP, 0), 0);
}

LRESULT CALLBACK TreeViewSubclassProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    UINT_PTR uIdSubclass,
    DWORD_PTR dwRefData
) {
    TreeViewLockState* state = reinterpret_cast<TreeViewLockState*>(dwRefData);

    if (g_msgUnsubclass != 0 && uMsg == g_msgUnsubclass) {
        RemoveWindowSubclass(hwnd, TreeViewSubclassProc, uIdSubclass);
        UnregisterSubclassedTree(hwnd);
        delete state;
        return 0;
    }

    if (uMsg == WM_NCDESTROY) {
        RemoveWindowSubclass(hwnd, TreeViewSubclassProc, uIdSubclass);
        UnregisterSubclassedTree(hwnd);
        delete state;
        return DefSubclassProc(hwnd, uMsg, wParam, lParam);
    }

    if (!state) {
        return DefSubclassProc(hwnd, uMsg, wParam, lParam);
    }

    // Detect direct user interaction with the sidebar to immediately release the scroll lock
    switch (uMsg) {
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_NCLBUTTONDOWN:
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            state->userInteracted = true;
            break;
    }

    // Fast-path: ignore high-frequency read-only / mouse-hover messages
    switch (uMsg) {
        case WM_NCHITTEST:
        case WM_SETCURSOR:
        case WM_MOUSEMOVE:
        case WM_NCMOUSEMOVE:
        case WM_MOUSELEAVE:
        case WM_GETOBJECT:
        case TVM_GETNEXTITEM:
        case TVM_GETITEMW:
        case TVM_GETITEMA:
        case TVM_GETCOUNT:
        case TVM_GETITEMRECT:
        case TVM_HITTEST:
            return DefSubclassProc(hwnd, uMsg, wParam, lParam);
    }

    ULONGLONG now = GetTickCount64();
    ULONGLONG globalLockUntil = (ULONGLONG)InterlockedCompareExchange64(&g_windowOpenLockUntilTick, 0, 0);
    if (globalLockUntil > state->lockUntilTick) {
        state->lockUntilTick = globalLockUntil;
    }

    if (!state->userInteracted && state->firstVisibleWithItemsTick == 0) {
        if (IsWindowVisible(hwnd)) {
            LRESULT itemCount = DefSubclassProc(hwnd, TVM_GETCOUNT, 0, 0);
            if (itemCount > 0) {
                state->firstVisibleWithItemsTick = now;
                ULONGLONG visibleGrace = now + 3000;
                if (visibleGrace > state->lockUntilTick) {
                    state->lockUntilTick = visibleGrace;
                }
            }
        }
    }

    bool waitingForFirstVisible = (state->firstVisibleWithItemsTick == 0) &&
                                  (now < state->createdTick + 30000);

    bool isLocked = !state->userInteracted &&
                    (InterlockedCompareExchange(&g_isRestoring, 0, 0) != 0 ||
                     now < state->lockUntilTick ||
                     waitingForFirstVisible);

    if (!isLocked) {
        // If the lock window has completely expired, unsubclass cleanly when not in a nested reset
        if (!state->inScrollReset) {
            RemoveWindowSubclass(hwnd, TreeViewSubclassProc, uIdSubclass);
            UnregisterSubclassedTree(hwnd);
            delete state;
        }
        return DefSubclassProc(hwnd, uMsg, wParam, lParam);
    }

    if (!state->inScrollReset) {
        if (uMsg == TVM_ENSUREVISIBLE) {
            return 0;
        }

        if (uMsg == TVM_SELECTITEM && wParam == TVGN_FIRSTVISIBLE) {
            HTREEITEM hRoot = (HTREEITEM)DefSubclassProc(hwnd, TVM_GETNEXTITEM, TVGN_ROOT, 0);
            if (hRoot && (HTREEITEM)lParam != hRoot) {
                lParam = (LPARAM)hRoot;
            }
        }

        if (uMsg == WM_VSCROLL && LOWORD(wParam) != SB_TOP && LOWORD(wParam) != SB_ENDSCROLL) {
            return 0;
        }
    }

    HWND prevLockedHwnd = tls_lockedTreeHwnd;
    tls_lockedTreeHwnd = hwnd;

    LRESULT res = DefSubclassProc(hwnd, uMsg, wParam, lParam);

    if (!state->inScrollReset && IsTreeViewScrolledDownViaSubclass(hwnd)) {
        state->inScrollReset = true;
        ResetTreeViewScrollToTopViaSubclass(hwnd);
        state->inScrollReset = false;
    }

    tls_lockedTreeHwnd = prevLockedHwnd;
    return res;
}

void SubclassExplorerTreeView(HWND hwnd) {
    ULONGLONG now = GetTickCount64();
    ULONGLONG globalLockUntil = (ULONGLONG)InterlockedCompareExchange64(&g_windowOpenLockUntilTick, 0, 0);
    ULONGLONG lockUntil = (std::max)(now + 6000, globalLockUntil);

    TreeViewLockState* state = new TreeViewLockState{
        now,
        lockUntil,
        0,
        false,
        false
    };

    if (SetWindowSubclass(hwnd, TreeViewSubclassProc, SUBCLASS_ID, reinterpret_cast<DWORD_PTR>(state))) {
        RegisterSubclassedTree(hwnd);
    } else {
        delete state;
    }
}

const std::wstring& GetTabListFilePath() {
    static std::wstring s_cachedFilePath = []() -> std::wstring {
        wchar_t modPathBuffer[MAX_PATH];
        if (!Wh_GetModStoragePath(modPathBuffer, MAX_PATH)) {
            Wh_Log(L"Failed to get mod storage path.");
            return L"";
        }
        std::wstring modPath = modPathBuffer;
        SHCreateDirectoryExW(NULL, modPath.c_str(), NULL);
        modPath += L"\\LastExplorerTabs.txt";
        return modPath;
    }();
    return s_cachedFilePath;
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
                    // Ignore tabs belonging to a top-level Explorer window that is already hidden/closing
                    HWND hwndBrowser = NULL;
                    HWND hwndRoot = NULL;
                    if (SUCCEEDED(pShellBrowser->GetWindow(&hwndBrowser)) && hwndBrowser) {
                        hwndRoot = GetAncestor(hwndBrowser, GA_ROOT);
                    }
                    if (!hwndRoot || IsWindowVisible(hwndRoot)) {
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
    const std::wstring& filePath = GetTabListFilePath();
    if (filePath.empty()) return;

    std::wstring joined;
    for (const auto& tab : tabs) {
        joined += tab;
        joined += L"\r\n";
    }

    std::string utf8Data;
    if (!joined.empty()) {
        int needed = WideCharToMultiByte(CP_UTF8, 0, joined.data(), (int)joined.size(), NULL, 0, NULL, NULL);
        if (needed > 0) {
            utf8Data.resize(needed);
            WideCharToMultiByte(CP_UTF8, 0, joined.data(), (int)joined.size(), &utf8Data[0], needed, NULL, NULL);
        }
    }

    HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        if (!utf8Data.empty()) {
            DWORD bytesWritten = 0;
            WriteFile(hFile, utf8Data.data(), (DWORD)utf8Data.size(), &bytesWritten, NULL);
        }
        CloseHandle(hFile);
    }
}

bool TryNavigateInitialTab(HWND targetHwnd, const std::wstring& targetPath) {
    PIDLIST_ABSOLUTE targetPidl = ILCreateFromPathW(targetPath.c_str());
    if (!targetPidl) {
        return false;
    }

    bool confirmedAtTarget = false;
    bool sentNavigate = false;
    int postNavChecks = 0;

    // Wait up to ~2.5 seconds for targetHwnd's initial tab to register and navigate in-place
    for (int attempt = 0; attempt < 50 && g_bIsRunning; ++attempt) {
        if (!IsWindow(targetHwnd)) break;

        IShellWindows* pShellWindows = NULL;
        if (SUCCEEDED(CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pShellWindows))) && pShellWindows) {
            long count = 0;
            pShellWindows->get_Count(&count);
            for (long i = 0; i < count; ++i) {
                VARIANT v;
                VariantInit(&v);
                v.vt = VT_I4;
                v.lVal = i;
                IDispatch* pDispatch = NULL;
                if (SUCCEEDED(pShellWindows->Item(v, &pDispatch)) && pDispatch) {
                    IServiceProvider* pServiceProvider = NULL;
                    if (SUCCEEDED(pDispatch->QueryInterface(IID_PPV_ARGS(&pServiceProvider))) && pServiceProvider) {
                        IShellBrowser* pShellBrowser = NULL;
                        if (SUCCEEDED(pServiceProvider->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&pShellBrowser))) && pShellBrowser) {
                            HWND hwndBrowser = NULL;
                            if (SUCCEEDED(pShellBrowser->GetWindow(&hwndBrowser)) && hwndBrowser &&
                                GetAncestor(hwndBrowser, GA_ROOT) == targetHwnd) {
                                IShellView* pShellView = NULL;
                                if (SUCCEEDED(pShellBrowser->QueryActiveShellView(&pShellView)) && pShellView) {
                                    bool readyForNav = false;
                                    IFolderView* pFolderView = NULL;
                                    if (SUCCEEDED(pShellView->QueryInterface(IID_PPV_ARGS(&pFolderView))) && pFolderView) {
                                        IPersistFolder2* pPersistFolder2 = NULL;
                                        if (SUCCEEDED(pFolderView->GetFolder(IID_PPV_ARGS(&pPersistFolder2))) && pPersistFolder2) {
                                            PIDLIST_ABSOLUTE curPidl = NULL;
                                            if (SUCCEEDED(pPersistFolder2->GetCurFolder(&curPidl)) && curPidl) {
                                                readyForNav = true;
                                                wchar_t curPath[MAX_PATH];
                                                if (SHGetPathFromIDListW(curPidl, curPath) &&
                                                    lstrcmpiW(curPath, targetPath.c_str()) == 0) {
                                                    confirmedAtTarget = true;
                                                }
                                                CoTaskMemFree(curPidl);
                                            }
                                            pPersistFolder2->Release();
                                        }
                                        pFolderView->Release();
                                    }
                                    pShellView->Release();

                                    if (!confirmedAtTarget && readyForNav) {
                                        if (!sentNavigate || (postNavChecks % 4 == 3)) {
                                            bool navCalled = false;
                                            IWebBrowser2* pWebBrowser2 = NULL;
                                            if (SUCCEEDED(pDispatch->QueryInterface(IID_PPV_ARGS(&pWebBrowser2))) && pWebBrowser2) {
                                                BSTR bstrPath = SysAllocString(targetPath.c_str());
                                                if (bstrPath) {
                                                    VARIANT vEmpty;
                                                    VariantInit(&vEmpty);
                                                    if (SUCCEEDED(pWebBrowser2->Navigate(bstrPath, &vEmpty, &vEmpty, &vEmpty, &vEmpty))) {
                                                        navCalled = true;
                                                    }
                                                    SysFreeString(bstrPath);
                                                }
                                                pWebBrowser2->Release();
                                            }
                                            if (!navCalled) {
                                                if (SUCCEEDED(pShellBrowser->BrowseObject(targetPidl, SBSP_SAMEBROWSER | SBSP_ABSOLUTE))) {
                                                    navCalled = true;
                                                }
                                            }
                                            if (navCalled) {
                                                sentNavigate = true;
                                            }
                                        }
                                        if (sentNavigate) {
                                            ++postNavChecks;
                                        }
                                    }
                                }
                            }
                            pShellBrowser->Release();
                        }
                        pServiceProvider->Release();
                    }
                    pDispatch->Release();
                }
                if (confirmedAtTarget) break;
            }
            pShellWindows->Release();
        }

        if (confirmedAtTarget) break;
        if (sentNavigate && postNavChecks >= 10) break;

        if (g_hStopEvent) {
            if (WaitForSingleObject(g_hStopEvent, 50) == WAIT_OBJECT_0) break;
        } else {
            Sleep(50);
        }
    }

    ILFree(targetPidl);
    return confirmedAtTarget || sentNavigate;
}

void RestoreTabs(HWND targetHwnd) {
    const std::wstring& filePath = GetTabListFilePath();
    if (filePath.empty()) return;

    HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return;

    std::string rawData;
    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize > 0 && fileSize != INVALID_FILE_SIZE && fileSize < (4 * 1024 * 1024)) {
        rawData.resize(fileSize);
        DWORD bytesRead = 0;
        if (!ReadFile(hFile, &rawData[0], fileSize, &bytesRead, NULL)) {
            rawData.clear();
        } else {
            rawData.resize(bytesRead);
        }
    }
    CloseHandle(hFile);

    if (rawData.empty()) return;

    int wlen = MultiByteToWideChar(CP_UTF8, 0, rawData.data(), (int)rawData.size(), NULL, 0);
    if (wlen <= 0) return;

    std::wstring content(wlen, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, rawData.data(), (int)rawData.size(), &content[0], wlen);

    std::vector<std::wstring> paths;
    size_t start = 0;
    while (start < content.size()) {
        size_t end = content.find_first_of(L"\r\n", start);
        if (end == std::wstring::npos) {
            end = content.size();
        }
        if (end > start) {
            paths.push_back(content.substr(start, end - start));
        }
        start = content.find_first_not_of(L"\r\n", end);
        if (start == std::wstring::npos) {
            break;
        }
    }

    if (paths.empty()) return;

    EnterCriticalSection(&g_treeCs);
    g_lastKnownTabs = paths;
    LeaveCriticalSection(&g_treeCs);

    // Navigate the window's initial left-most tab in-place to paths[0] so no extra blank/default tab remains
    size_t startIndex = 0;
    if (TryNavigateInitialTab(targetHwnd, paths[0])) {
        startIndex = 1;
    }

    for (size_t i = startIndex; i < paths.size(); ++i) {
        if (!g_bIsRunning) break;
        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.fMask = SEE_MASK_DEFAULT;
        sei.lpVerb = L"open";
        sei.lpFile = paths[i].c_str();
        sei.nShow = SW_SHOWNORMAL;
        ShellExecuteExW(&sei);
        if (g_hStopEvent) {
            if (WaitForSingleObject(g_hStopEvent, 50) == WAIT_OBJECT_0) break;
        } else {
            Sleep(50);
        }
    }
}

BOOL CALLBACK CheckExplorerWindowProc(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId() && IsWindowVisible(hwnd)) {
        wchar_t className[64];
        if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
            lstrcmpW(className, L"CabinetWClass") == 0) {
            *reinterpret_cast<bool*>(lParam) = true;
            return FALSE;
        }
    }
    return TRUE;
}

bool HasOpenExplorerWindow() {
    bool found = false;
    EnumWindows(CheckExplorerWindowProc, reinterpret_cast<LPARAM>(&found));
    return found;
}

DWORD WINAPI RestoreTabsThread(LPVOID lpParam) {
    HWND targetHwnd = (HWND)lpParam;
    if (!IsWindow(targetHwnd)) {
        Wh_Log(L"RestoreTabsThread: Invalid window handle received.");
        InterlockedExchange(&g_isRestoring, 0);
        InterlockedDecrement(&g_activeRestoreThreads);
        return 1;
    }

    Wh_Log(L"RestoreTabsThread: Triggered for window %p.", targetHwnd);

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        RestoreTabs(targetHwnd);
        CoUninitialize();
    }

    InterlockedExchange64(&g_windowOpenLockUntilTick, (LONGLONG)(GetTickCount64() + 5000));

    // Brief grace period for Explorer Tab Utility to finish injecting the restored tabs
    if (g_hStopEvent) {
        WaitForSingleObject(g_hStopEvent, 1200);
    } else {
        Sleep(1200);
    }

    InterlockedExchange(&g_isRestoring, 0);
    InterlockedDecrement(&g_activeRestoreThreads);
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
        // Fast check before calling GetClassNameW if lpClassName is a string pointer
        if (!IS_INTRESOURCE(lpClassName)) {
            wchar_t firstChar = lpClassName[0];
            if (firstChar != L'C' && firstChar != L'c' &&
                firstChar != L'S' && firstChar != L's') {
                return hwnd;
            }
        }

        wchar_t className[64];
        if (GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
            if (lstrcmpW(className, L"CabinetWClass") == 0) {
                InterlockedExchange64(&g_windowOpenLockUntilTick, (LONGLONG)(GetTickCount64() + 6000));

                // trigger restore once per Explorer restart
                if (InterlockedCompareExchange(&g_restoreScheduled, 1, 0) == 0) {
                    const std::wstring& filePath = GetTabListFilePath();
                    if (!filePath.empty() && PathFileExistsW(filePath.c_str())) {
                        InterlockedExchange(&g_isRestoring, 1);
                        InterlockedIncrement(&g_activeRestoreThreads);
                        HANDLE hThread = CreateThread(NULL, 0, RestoreTabsThread, (LPVOID)hwnd, 0, NULL);
                        if (hThread) {
                            CloseHandle(hThread);
                        } else {
                            InterlockedExchange(&g_isRestoring, 0);
                            InterlockedDecrement(&g_activeRestoreThreads);
                        }
                    }
                }
            } else if (lstrcmpW(className, L"SysTreeView32") == 0) {
                if (IsExplorerSidebarTreeView(hwnd, hWndParent)) {
                    SubclassExplorerTreeView(hwnd);
                }
            }
        }
    }

    return hwnd;
}

DWORD WINAPI MainThread(LPVOID) {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    while (g_bIsRunning) {
        if (InterlockedCompareExchange(&g_isRestoring, 0, 0) == 0) {
            std::vector<std::wstring> currentTabs = GetCurrentTabs();

            EnterCriticalSection(&g_treeCs);
            if (!currentTabs.empty()) {
                if (currentTabs != g_lastKnownTabs) {
                    WriteTabsToFile(currentTabs);
                    g_lastKnownTabs = currentTabs;
                }
            }
            LeaveCriticalSection(&g_treeCs);

            if (currentTabs.empty() && !HasOpenExplorerWindow()) {
                InterlockedExchange(&g_restoreScheduled, 0);
            }
        }

        if (g_hStopEvent) {
            if (WaitForSingleObject(g_hStopEvent, 2000) == WAIT_OBJECT_0) break;
        } else {
            Sleep(2000);
        }
    }

    CoUninitialize();
    return 0;
}


BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Explorer Tabs Session Saver...");

    InitializeCriticalSection(&g_treeCs);
    g_msgUnsubclass = RegisterWindowMessageW(L"Windhawk_ETSS_Unsubclass");
    g_hStopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&pCreateWindowExW_Orig);
    Wh_SetFunctionHook((void*)SetScrollInfo, (void*)SetScrollInfo_Hook, (void**)&pSetScrollInfo_Orig);
    Wh_SetFunctionHook((void*)SetScrollPos, (void*)SetScrollPos_Hook, (void**)&pSetScrollPos_Orig);
    Wh_SetFunctionHook((void*)ScrollWindowEx, (void*)ScrollWindowEx_Hook, (void**)&pScrollWindowEx_Orig);
    Wh_SetFunctionHook((void*)UpdateWindow, (void*)UpdateWindow_Hook, (void**)&pUpdateWindow_Orig);
    
    g_bIsRunning = true;
    g_hMainThread = CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
    
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Explorer Tabs Session Saver...");

    g_bIsRunning = false;
    if (g_hStopEvent) {
        SetEvent(g_hStopEvent);
    }

    if (g_hMainThread) {
        WaitForSingleObject(g_hMainThread, 2500);
        CloseHandle(g_hMainThread);
        g_hMainThread = NULL;
    }

    for (int i = 0; i < 40 && InterlockedCompareExchange(&g_activeRestoreThreads, 0, 0) > 0; ++i) {
        Sleep(50);
    }

    if (g_hStopEvent) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
    }

    std::vector<HWND> treesToUnsubclass;
    EnterCriticalSection(&g_treeCs);
    treesToUnsubclass = g_subclassedTrees;
    LeaveCriticalSection(&g_treeCs);

    for (HWND hwnd : treesToUnsubclass) {
        if (IsWindow(hwnd) && g_msgUnsubclass != 0) {
            DWORD_PTR result = 0;
            SendMessageTimeoutW(hwnd, g_msgUnsubclass, 0, 0, SMTO_BLOCK, 1000, &result);
        }
    }

    DeleteCriticalSection(&g_treeCs);
}
