// ==WindhawkMod==
// @id              explorer-tabs-session-saver-revamped
// @name            Explorer Tabs Session Saver Revamped
// @description     Saves and restores File Explorer tabs on reopen, without blank extra tabs or sidebar scroll glitches
// @version         1.0.0
// @author          Peter2oo1
// @github          https://github.com/Peter2oo1
// @include         explorer.exe
// @compilerOptions -lshlwapi -lole32 -loleaut32 -lshell32 -luuid -lcomctl32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Tabs Session Saver Revamped

This mod saves your open File Explorer tabs and automatically restores them whenever you reopen File Explorer.

It is an enhanced successor to [Explorer Tabs Session Saver](https://windhawk.net/mods/explorer-tabs-session-saver) by [noX1st](https://github.com/noX1st), fixing several core issues and edge cases.

![Demonstration](https://i.imgur.com/5ehPFaL.gif)

### Improvements Over the Original Mod
- **No Extra "Blank" Tab on Restore:** When File Explorer opens to a default start location (*Home* or *This PC*), the initial left-most tab is navigated in-place (`IWebBrowser2::Navigate` / `IShellBrowser::BrowseObject`) instead of spawning a separate `ShellExecuteExW` call for the first saved tab. If Explorer is opened for a specific folder (e.g. *"Show in folder"* or a folder shortcut), that folder and its selection are preserved and your saved tabs open alongside it.
- **Fixes Sidebar Auto-Scrolling to the Bottom:** Prevents File Explorer's Navigation Pane (`SysTreeView32`) from jumping or flickering to the bottom when restoring tabs with Explorer Tab Utility.
- **Persistent Session Protection:** Opening File Explorer and closing it immediately without interacting with the restored tabs will no longer delete or lose your saved session.
- **Atomic Multi-Tab Window Close:** Ignores tabs belonging to a top-level Explorer window that is already in the middle of closing, preventing partial tab lists from overwriting your session when closing a multi-tab window.
- **Per-User UTF-8 Session Storage:** Stores sessions per Windows user account (`LastExplorerTabs-<SID>.txt`) in UTF-8 so folder paths with accented or Unicode characters are preserved and multi-user accounts do not collide.
- **Clean Mod Reload / Unload:** Joins worker threads and unsubclasses controls deterministically on unload.

### Notes
- Works independently of the built-in Windows feature *"Restore previous folder windows at logon"*.
- **Important:** To restore tabs into a single File Explorer window, [Explorer Tab Utility](https://github.com/w4po/ExplorerTabUtility) (with its **"Window Hook"** feature enabled) or a similar tab-merging utility is required. Otherwise, each tab will be restored in its own separate window.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- reuseInitialTab: true
  $name: Reuse initial tab on restore
  $description: Navigate the initial Home / This PC tab in-place to the first saved tab instead of leaving an extra default tab on the left.
- lockSidebarScroll: true
  $name: Prevent sidebar auto-scroll to bottom on restore
  $description: Lock the Navigation Pane scroll position to the top while tabs are being restored.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <exdisp.h>
#include <sddl.h>
#include <vector>
#include <string>
#include <olectl.h>
#include <algorithm>

struct {
    bool reuseInitialTab = true;
    bool lockSidebarScroll = true;
} g_settings;

HANDLE g_hMainThread    = NULL;
HANDLE g_hRestoreThread = NULL;
HANDLE g_hStopEvent     = NULL;
bool   g_bIsRunning     = false;
std::vector<std::wstring> g_lastKnownTabs;
static volatile LONG g_restoreScheduled = 0;
static volatile LONG g_isRestoring = 0;
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
    int depth;
    bool removePending;
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

LRESULT TreeViewSubclassProcBody(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    TreeViewLockState* state
) {
    if (g_msgUnsubclass != 0 && uMsg == g_msgUnsubclass) {
        state->removePending = true;
        return 0;
    }

    if (uMsg == WM_NCDESTROY) {
        state->removePending = true;
        return DefSubclassProc(hwnd, uMsg, wParam, lParam);
    }

    if (state->removePending) {
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

    bool isRestoring = (InterlockedCompareExchange(&g_isRestoring, 0, 0) != 0);

    // While tabs are still being restored and switched by Explorer Tab Utility, do not consume
    // the visibility grace period (especially for the first tab which starts visible on Home
    // and then gets hidden when subsequent tabs open).
    if (isRestoring) {
        state->firstVisibleWithItemsTick = 0;
    } else if (!state->userInteracted && state->firstVisibleWithItemsTick == 0) {
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
                                  (now < state->createdTick + 120000);

    bool isLocked = g_settings.lockSidebarScroll &&
                    !state->userInteracted &&
                    (isRestoring ||
                     now < state->lockUntilTick ||
                     waitingForFirstVisible);

    if (!isLocked) {
        if (!state->inScrollReset) {
            state->removePending = true;
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

    if (!state->removePending && !state->inScrollReset && IsTreeViewScrolledDownViaSubclass(hwnd)) {
        state->inScrollReset = true;
        ResetTreeViewScrollToTopViaSubclass(hwnd);
        state->inScrollReset = false;
    }

    tls_lockedTreeHwnd = prevLockedHwnd;
    return res;
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
    if (!state) {
        return DefSubclassProc(hwnd, uMsg, wParam, lParam);
    }

    state->depth++;
    LRESULT res = TreeViewSubclassProcBody(hwnd, uMsg, wParam, lParam, state);
    if (--state->depth == 0 && state->removePending) {
        RemoveWindowSubclass(hwnd, TreeViewSubclassProc, uIdSubclass);
        UnregisterSubclassedTree(hwnd);
        delete state;
    }
    return res;
}

void SubclassExplorerTreeView(HWND hwnd) {
    EnterCriticalSection(&g_treeCs);
    bool alreadySubclassed =
        (std::find(g_subclassedTrees.begin(), g_subclassedTrees.end(), hwnd) != g_subclassedTrees.end());
    LeaveCriticalSection(&g_treeCs);
    if (alreadySubclassed) {
        return;
    }

    ULONGLONG now = GetTickCount64();
    ULONGLONG globalLockUntil = (ULONGLONG)InterlockedCompareExchange64(&g_windowOpenLockUntilTick, 0, 0);
    ULONGLONG lockUntil = (std::max)(now + 6000, globalLockUntil);

    TreeViewLockState* state = new TreeViewLockState{
        now,
        lockUntil,
        0,
        0,
        false,
        false,
        false
    };

    if (SetWindowSubclass(hwnd, TreeViewSubclassProc, SUBCLASS_ID, reinterpret_cast<DWORD_PTR>(state))) {
        RegisterSubclassedTree(hwnd);
        if (GetScrollPos(hwnd, SB_VERT) > 0) {
            HTREEITEM hRoot = (HTREEITEM)SendMessageW(hwnd, TVM_GETNEXTITEM, TVGN_ROOT, 0);
            if (hRoot) {
                SendMessageW(hwnd, TVM_SELECTITEM, TVGN_FIRSTVISIBLE, (LPARAM)hRoot);
            }
            SendMessageW(hwnd, WM_VSCROLL, MAKEWPARAM(SB_TOP, 0), 0);
        }
    } else {
        delete state;
    }
}

BOOL CALLBACK SubclassChildTreeViewsProc(HWND hwnd, LPARAM) {
    if (GetWindowThreadProcessId(hwnd, NULL) == GetCurrentThreadId()) {
        wchar_t className[64];
        if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
            lstrcmpW(className, L"SysTreeView32") == 0 &&
            IsExplorerSidebarTreeView(hwnd, NULL)) {
            SubclassExplorerTreeView(hwnd);
        }
    }
    return TRUE;
}

std::wstring GetCurrentUserSidString() {
    std::wstring sidStr;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        DWORD dwSize = 0;
        GetTokenInformation(hToken, TokenUser, NULL, 0, &dwSize);
        if (dwSize > 0) {
            std::vector<BYTE> buffer(dwSize);
            PTOKEN_USER pTokenUser = reinterpret_cast<PTOKEN_USER>(buffer.data());
            if (GetTokenInformation(hToken, TokenUser, pTokenUser, dwSize, &dwSize)) {
                LPWSTR lpSid = NULL;
                if (ConvertSidToStringSidW(pTokenUser->User.Sid, &lpSid) && lpSid) {
                    sidStr = lpSid;
                    LocalFree(lpSid);
                }
            }
        }
        CloseHandle(hToken);
    }
    return sidStr;
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

        std::wstring sid = GetCurrentUserSidString();
        if (!sid.empty()) {
            modPath += L"\\LastExplorerTabs-" + sid + L".txt";
        } else {
            modPath += L"\\LastExplorerTabs.txt";
        }
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
                                        if (SHGetPathFromIDListW(pidl, path) && PathIsDirectoryW(path)) {
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

enum class InitialTabAction {
    NavigatedInPlace,
    KeepSpecificFolderTab,
    SkipNonFolderWindow
};

InitialTabAction InspectAndNavigateInitialTab(HWND targetHwnd, const std::wstring& targetPath) {
    PIDLIST_ABSOLUTE targetPidl = ILCreateFromPathW(targetPath.c_str());
    if (!targetPidl) {
        return InitialTabAction::KeepSpecificFolderTab;
    }

    PIDLIST_ABSOLUTE pidlHome = NULL;
    SHParseDisplayName(L"shell:::{f874310e-b6b7-47dc-bc84-b9e6b38f5903}", NULL, &pidlHome, 0, NULL);
    PIDLIST_ABSOLUTE pidlThisPC = NULL;
    SHParseDisplayName(L"shell:::{20D04FE0-3AEA-1069-A2D8-08002B30309D}", NULL, &pidlThisPC, 0, NULL);

    bool confirmedAtTarget = false;
    bool sentNavigate = false;
    bool decidedKeepSpecificFolder = false;
    bool decidedSkipNonFolderWindow = false;
    int postNavChecks = 0;

    // Wait up to ~2.5 seconds for targetHwnd's initial tab to register in IShellWindows
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
                                    bool canNavigateInPlace = false;
                                    IFolderView* pFolderView = NULL;
                                    if (SUCCEEDED(pShellView->QueryInterface(IID_PPV_ARGS(&pFolderView))) && pFolderView) {
                                        IPersistFolder2* pPersistFolder2 = NULL;
                                        if (SUCCEEDED(pFolderView->GetFolder(IID_PPV_ARGS(&pPersistFolder2))) && pPersistFolder2) {
                                            PIDLIST_ABSOLUTE curPidl = NULL;
                                            if (SUCCEEDED(pPersistFolder2->GetCurFolder(&curPidl)) && curPidl) {
                                                bool isDefaultStart =
                                                    (pidlHome && ILIsEqual(curPidl, pidlHome)) ||
                                                    (pidlThisPC && ILIsEqual(curPidl, pidlThisPC));

                                                if (sentNavigate) {
                                                    wchar_t curPath[MAX_PATH];
                                                    if (SHGetPathFromIDListW(curPidl, curPath) &&
                                                        lstrcmpiW(curPath, targetPath.c_str()) == 0) {
                                                        confirmedAtTarget = true;
                                                    } else {
                                                        canNavigateInPlace = true;
                                                    }
                                                } else if (isDefaultStart) {
                                                    if (g_settings.reuseInitialTab) {
                                                        canNavigateInPlace = true;
                                                    } else {
                                                        decidedKeepSpecificFolder = true;
                                                    }
                                                } else {
                                                    wchar_t curPath[MAX_PATH];
                                                    if (SHGetPathFromIDListW(curPidl, curPath) && PathIsDirectoryW(curPath)) {
                                                        // Window was opened for a specific folder (e.g. "Show in folder", CLI path); keep it
                                                        decidedKeepSpecificFolder = true;
                                                    } else {
                                                        // Non-folder virtual shell view (e.g. Control Panel); skip restoring on this window
                                                        decidedSkipNonFolderWindow = true;
                                                    }
                                                }
                                                CoTaskMemFree(curPidl);
                                            }
                                            pPersistFolder2->Release();
                                        } else {
                                            decidedSkipNonFolderWindow = true;
                                        }
                                        pFolderView->Release();
                                    } else {
                                        decidedSkipNonFolderWindow = true;
                                    }
                                    pShellView->Release();

                                    if (!confirmedAtTarget && canNavigateInPlace) {
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
                if (confirmedAtTarget || decidedKeepSpecificFolder || decidedSkipNonFolderWindow) break;
            }
            pShellWindows->Release();
        }

        if (confirmedAtTarget || decidedKeepSpecificFolder || decidedSkipNonFolderWindow) break;
        if (sentNavigate && postNavChecks >= 10) break;

        if (g_hStopEvent) {
            if (WaitForSingleObject(g_hStopEvent, 50) == WAIT_OBJECT_0) break;
        } else {
            Sleep(50);
        }
    }

    if (pidlHome) ILFree(pidlHome);
    if (pidlThisPC) ILFree(pidlThisPC);
    ILFree(targetPidl);

    if (decidedSkipNonFolderWindow) {
        return InitialTabAction::SkipNonFolderWindow;
    }
    if (confirmedAtTarget || sentNavigate) {
        return InitialTabAction::NavigatedInPlace;
    }
    return InitialTabAction::KeepSpecificFolderTab;
}

bool RestoreTabs(HWND targetHwnd) {
    const std::wstring& filePath = GetTabListFilePath();
    if (filePath.empty()) return false;

    HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return false;

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

    if (rawData.empty()) return false;

    int wlen = MultiByteToWideChar(CP_UTF8, 0, rawData.data(), (int)rawData.size(), NULL, 0);
    if (wlen <= 0) return false;

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
            std::wstring candidate = content.substr(start, end - start);
            if (PathIsDirectoryW(candidate.c_str())) {
                paths.push_back(std::move(candidate));
            }
        }
        start = content.find_first_not_of(L"\r\n", end);
        if (start == std::wstring::npos) {
            break;
        }
    }

    if (paths.empty()) return false;

    InitialTabAction action = InspectAndNavigateInitialTab(targetHwnd, paths[0]);
    if (action == InitialTabAction::SkipNonFolderWindow) {
        // Target window is not a folder window (e.g. Control Panel); re-arm restore for the next Explorer window
        InterlockedExchange(&g_restoreScheduled, 0);
        return false;
    }

    EnterCriticalSection(&g_treeCs);
    g_lastKnownTabs = paths;
    LeaveCriticalSection(&g_treeCs);

    size_t startIndex = (action == InitialTabAction::NavigatedInPlace) ? 1 : 0;

    for (size_t i = startIndex; i < paths.size(); ++i) {
        if (!g_bIsRunning) break;
        if (!PathIsDirectoryW(paths[i].c_str())) continue;

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
    return true;
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
        InterlockedExchange64(&g_windowOpenLockUntilTick, 0);
        InterlockedExchange(&g_isRestoring, 0);
        return 1;
    }

    Wh_Log(L"RestoreTabsThread: Triggered for window %p.", targetHwnd);

    bool restored = false;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        restored = RestoreTabs(targetHwnd);
        CoUninitialize();
    }

    if (restored) {
        InterlockedExchange64(&g_windowOpenLockUntilTick, (LONGLONG)(GetTickCount64() + 5000));

        // Brief grace period for Explorer Tab Utility to finish injecting the restored tabs
        if (g_hStopEvent) {
            WaitForSingleObject(g_hStopEvent, 1200);
        } else {
            Sleep(1200);
        }
    } else {
        InterlockedExchange64(&g_windowOpenLockUntilTick, 0);
    }

    InterlockedExchange(&g_isRestoring, 0);
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

    if (hwnd && g_bIsRunning) {
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
                // Trigger restore once when opening the first Explorer window
                if (InterlockedCompareExchange(&g_restoreScheduled, 1, 0) == 0) {
                    const std::wstring& filePath = GetTabListFilePath();
                    if (!filePath.empty() && PathFileExistsW(filePath.c_str())) {
                        InterlockedExchange64(&g_windowOpenLockUntilTick, (LONGLONG)(GetTickCount64() + 6000));
                        InterlockedExchange(&g_isRestoring, 1);
                        if (g_settings.lockSidebarScroll) {
                            EnumChildWindows(hwnd, SubclassChildTreeViewsProc, 0);
                        }
                        HANDLE hThread = CreateThread(NULL, 0, RestoreTabsThread, (LPVOID)hwnd, 0, NULL);
                        if (hThread) {
                            HANDLE prev = (HANDLE)InterlockedExchangePointer(&g_hRestoreThread, hThread);
                            if (prev) {
                                WaitForSingleObject(prev, INFINITE);
                                CloseHandle(prev);
                            }
                        } else {
                            InterlockedExchange64(&g_windowOpenLockUntilTick, 0);
                            InterlockedExchange(&g_isRestoring, 0);
                        }
                    }
                }
            } else if (lstrcmpW(className, L"SysTreeView32") == 0) {
                // Only subclass Navigation Pane TreeViews created while a session restore is pending or active
                if (g_settings.lockSidebarScroll &&
                    (InterlockedCompareExchange(&g_isRestoring, 0, 0) != 0 ||
                     InterlockedCompareExchange(&g_restoreScheduled, 0, 0) == 0 ||
                     GetTickCount64() < (ULONGLONG)InterlockedCompareExchange64(&g_windowOpenLockUntilTick, 0, 0)) &&
                    IsExplorerSidebarTreeView(hwnd, hWndParent)) {
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

void LoadSettings() {
    g_settings.reuseInitialTab = Wh_GetIntSetting(L"reuseInitialTab") != 0;
    g_settings.lockSidebarScroll = Wh_GetIntSetting(L"lockSidebarScroll") != 0;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Explorer Tabs Session Saver...");

    LoadSettings();
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
        WaitForSingleObject(g_hMainThread, INFINITE);
        CloseHandle(g_hMainThread);
        g_hMainThread = NULL;
    }

    HANDLE hRestore = (HANDLE)InterlockedExchangePointer(&g_hRestoreThread, NULL);
    if (hRestore) {
        WaitForSingleObject(hRestore, INFINITE);
        CloseHandle(hRestore);
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
            SendMessageW(hwnd, g_msgUnsubclass, 0, 0);
        }
    }

    DeleteCriticalSection(&g_treeCs);
}
