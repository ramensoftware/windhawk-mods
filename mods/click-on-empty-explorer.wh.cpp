// ==WindhawkMod==
// @id              click-on-empty-explorer
// @name            Click on Empty Explorer
// @description     Configure double click, middle click and double middle click actions on empty space in File Explorer
// @version         2.1.1
// @author          LiHua81
// @github          https://github.com/LiHua81
// @include         explorer.exe
// @compilerOptions -lcomctl32 -loleaut32 -lole32
// @architecture    x86-64
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Based on "Explorer Double Click Up" by wrldspawn (GPL-3.0) and
// "Explorer Middle Click Duplicate Tab" by LiHua81 (GPL-3.0).
// UIAutomation usage from "Click on empty taskbar space" (GPL-3.0).
// FileCabinet_CreateViewWindow2 hook from "Classic Explorer Treeview".
// Going up a directory referenced from Open-Shell.

// ==WindhawkModReadme==
/*
# Click on Empty Explorer

Configure what happens when you double click, middle click, or double middle click on
empty space in File Explorer. Supports 12 different actions.

## How it works

This mod intercepts mouse clicks on the blank area of File Explorer's file list (where
no file or folder is located) and performs the action you've configured.

- **Double left click** — Windows natively detects double clicks. No delay at all.
- **Middle click** — If only a single-click action is set (double middle click is
  disabled), the action fires instantly with no delay, just like left click.
- **Double middle click** — Windows does not natively support double middle click, so
  the mod uses a timer-based detection. When you middle click, the mod waits for your
  system's double-click time (typically 500ms). If you middle click again within that
  window, it counts as a double click and fires the double-click action instead.
  - If both single and double middle click are set: single click is delayed by the
    double-click detection window.
  - If only double middle click is set: only double clicks trigger the action.

## Actions

- **Go Up** — Navigate to the parent folder
- **Go Back** — Navigate back in history
- **Go Forward** — Navigate forward in history
- **Refresh** — Refresh the current view
- **New Tab** — Open a new blank tab (Win11)
- **Duplicate Tab** — Open the current folder in a new tab (Win11)
- **Close Tab** — Close the current tab (Win11)
- **New Folder** — Create a new folder in the current view
- **Copy Path** — Copy the current folder path to clipboard
- **Go to Desktop** — Navigate to the Desktop
- **Go to Home** — Navigate to Quick Access / Home
- **None** — Do nothing

## Windows version support

Requires Windows 10 or later. Tab-related actions (New Tab, Duplicate Tab, Close Tab)
require Windows 11 for tabbed Explorer support.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- doubleClickAction: goUp
  $name: Double Click Action
  $description: What to do when double left clicking empty space. No delay (native Windows support).
  $options:
    - goUp: Go Up
    - goBack: Go Back
    - goForward: Go Forward
    - refresh: Refresh
    - newTab: New Tab (Win11)
    - duplicateTab: Duplicate Tab (Win11)
    - closeTab: Close Tab (Win11)
    - newFolder: New Folder
    - copyPath: Copy Path
    - goToDesktop: Go to Desktop
    - goToHome: Go to Home
    - none: None
- middleClickAction: none
  $name: Middle Click Action
  $description: What to do when single middle clicking empty space. If only single click is set, fires instantly. If both single and double are set, single is delayed ~500ms to detect double clicks.
  $options:
    - goUp: Go Up
    - goBack: Go Back
    - goForward: Go Forward
    - refresh: Refresh
    - newTab: New Tab (Win11)
    - duplicateTab: Duplicate Tab (Win11)
    - closeTab: Close Tab (Win11)
    - newFolder: New Folder
    - copyPath: Copy Path
    - goToDesktop: Go to Desktop
    - goToHome: Go to Home
    - none: None
- doubleMiddleClickAction: none
  $name: Double Middle Click Action
  $description: What to do when double middle clicking empty space. Two middle clicks within ~500ms count as a double click. If only double is set, single middle clicks are ignored.
  $options:
    - goUp: Go Up
    - goBack: Go Back
    - goForward: Go Forward
    - refresh: Refresh
    - newTab: New Tab (Win11)
    - duplicateTab: Duplicate Tab (Win11)
    - closeTab: Close Tab (Win11)
    - newFolder: New Folder
    - copyPath: Copy Path
    - goToDesktop: Go to Desktop
    - goToHome: Go to Home
    - none: None
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <windowsx.h>
#include <shdeprecated.h>
#include <shlobj.h>
#include <shobjidl.h>

#include <commctrl.h>
#include <UIAutomationClient.h>
#include <UIAutomationCore.h>
#include <comutil.h>
#include <winrt/base.h>

#include <mutex>
#include <string>
#include <vector>

using bstr_ptr = _bstr_t;

// ---- Global init/shutdown guard ----

static volatile LONG g_initialized = 0;

#define CHECK_INIT_OR_DEFER(hWnd, uMsg, wParam, lParam) \
    if (!g_initialized) return DefSubclassProc(hWnd, uMsg, wParam, lParam);
#define CHECK_INIT_OR_RETURN_VOID()  if (!g_initialized) return

// ---- Settings cache (RAII, thread-safe against settings-changed reload) ----

class StringSetting {
    PCWSTR m_str = nullptr;
public:
    void Load(PCWSTR name) {
        if (m_str) Wh_FreeStringSetting(m_str);
        m_str = Wh_GetStringSetting(name);
    }
    ~StringSetting() { if (m_str) Wh_FreeStringSetting(m_str); }
    PCWSTR Get() const { return m_str; }
    StringSetting(const StringSetting&) = delete;
    StringSetting& operator=(const StringSetting&) = delete;
    StringSetting() = default;
};

static std::mutex g_settingsMutex;
static StringSetting g_doubleClickAction;
static StringSetting g_middleClickAction;
static StringSetting g_doubleMiddleClickAction;

static void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_doubleClickAction.Load(L"doubleClickAction");
    g_middleClickAction.Load(L"middleClickAction");
    g_doubleMiddleClickAction.Load(L"doubleMiddleClickAction");
}

// Read settings under lock, deep-copy strings so they outlive the lock
struct SettingsSnapshot {
    std::wstring doubleClick;
    std::wstring middleClick;
    std::wstring doubleMiddleClick;
};

static SettingsSnapshot CopySettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return {
        g_doubleClickAction.Get() ? g_doubleClickAction.Get() : L"",
        g_middleClickAction.Get() ? g_middleClickAction.Get() : L"",
        g_doubleMiddleClickAction.Get() ? g_doubleMiddleClickAction.Get() : L""
    };
}

// ---- Helper: Send key combination ----

static void SendKeyCombo(WORD vk1, WORD vk2, WORD vk3 = 0) {
    INPUT inputs[6] = {};
    int count = 0;
    auto Press = [&](WORD vk) {
        inputs[count].type = INPUT_KEYBOARD;
        inputs[count].ki.wVk = vk;
        count++;
    };
    auto Release = [&](WORD vk) {
        inputs[count].type = INPUT_KEYBOARD;
        inputs[count].ki.wVk = vk;
        inputs[count].ki.dwFlags = KEYEVENTF_KEYUP;
        count++;
    };
    Press(vk1);
    Press(vk2);
    if (vk3) Press(vk3);
    Release(vk2);
    if (vk3) Release(vk3);
    Release(vk1);
    SendInput(count / 2, inputs, sizeof(INPUT));
    SendInput(count / 2, inputs + count / 2, sizeof(INPUT));
}

// ---- Duplicate Tab infrastructure ----

static wchar_t g_pendingNavPath[MAX_PATH] = {};
static winrt::com_ptr<IShellBrowser> g_pendingNavBrowser;
static HWND g_pendingNavHwnd = NULL;

static VOID CALLBACK NavigateNewTabProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
static VOID CALLBACK MidClickTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

// ---- Middle-click double-click detection (timer-based) ----
// Single-click fires immediately if only single is configured.
// If both single and double are configured, single is delayed by
// GetDoubleClickTime() (~500ms) to detect double clicks.

static HWND g_midClickPendingHwnd = NULL;
static UINT_PTR g_midClickTimerId = 0;

static bool FindShellTabAndDoAction(HWND hWnd, PCWSTR action);

static VOID CALLBACK MidClickTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    KillTimer(hwnd, idEvent);
    g_midClickTimerId = 0;
    if (g_midClickPendingHwnd && IsWindow(g_midClickPendingHwnd) && g_initialized) {
        std::wstring action = CopySettings().middleClick;
        if (wcscmp(action.c_str(), L"none") != 0)
            FindShellTabAndDoAction(g_midClickPendingHwnd, action.c_str());
    }
    g_midClickPendingHwnd = NULL;
}

static void CancelPendingMidClick() {
    if (g_midClickTimerId && g_midClickPendingHwnd && IsWindow(g_midClickPendingHwnd)) {
        KillTimer(g_midClickPendingHwnd, g_midClickTimerId);
    }
    g_midClickTimerId = 0;
    g_midClickPendingHwnd = NULL;
}

// ---- ExplorerWrapper ----

class ExplorerWrapper {
    winrt::com_ptr<IShellBrowser> hBrowser;

    // Helper: get current folder path via IShellView chain
    bool GetCurrentFolderPath(wchar_t* outPath, size_t outLen) {
        IShellView* psv = nullptr;
        if (FAILED(hBrowser->QueryActiveShellView(&psv)) || !psv)
            return false;
        IFolderView* pfv = nullptr;
        if (FAILED(psv->QueryInterface(IID_PPV_ARGS(&pfv)))) {
            psv->Release();
            return false;
        }
        IPersistIDList* pidlList = nullptr;
        if (FAILED(pfv->GetFolder(IID_PPV_ARGS(&pidlList)))) {
            pfv->Release();
            psv->Release();
            return false;
        }
        PIDLIST_ABSOLUTE pidl = nullptr;
        bool ok = SUCCEEDED(pidlList->GetIDList(&pidl)) && pidl
                  && SHGetPathFromIDListW(pidl, outPath) && outPath[0];
        if (pidl) CoTaskMemFree(pidl);
        pidlList->Release();
        pfv->Release();
        psv->Release();
        return ok;
    }

public:
    HWND hShellTab = NULL;
    HWND hListView = NULL;

    ExplorerWrapper(HWND hWnd, IShellBrowser* hShellBrowser) {
        hShellTab = hWnd;
        hBrowser.copy_from(hShellBrowser);
    }

    // Return a ref-counted copy of the browser for thread-safe access
    winrt::com_ptr<IShellBrowser> GetBrowser() const { return hBrowser; }

    // ---- Navigation ----

    void GoUp() {
        hBrowser->BrowseObject(NULL, SBSP_SAMEBROWSER | SBSP_PARENT);
    }

    void GoBack() {
        hBrowser->BrowseObject(NULL, SBSP_SAMEBROWSER | SBSP_NAVIGATEBACK);
    }

    void GoForward() {
        hBrowser->BrowseObject(NULL, SBSP_SAMEBROWSER | SBSP_NAVIGATEFORWARD);
    }

    void GoToDesktop() {
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (SUCCEEDED(SHGetFolderLocation(NULL, CSIDL_DESKTOP, NULL, 0, &pidl)) && pidl) {
            hBrowser->BrowseObject(pidl, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
            CoTaskMemFree(pidl);
        }
    }

    void GoToHome() {
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (SUCCEEDED(SHParseDisplayName(
                L"shell:::{679f85cb-0220-4080-b29b-5540cc05aab6}",
                NULL, &pidl, 0, NULL)) && pidl) {
            hBrowser->BrowseObject(pidl, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
            CoTaskMemFree(pidl);
        }
    }

    // ---- View ----

    void Refresh() {
        IShellView* psv = nullptr;
        if (SUCCEEDED(hBrowser->QueryActiveShellView(&psv)) && psv) {
            psv->Refresh();
            psv->Release();
        }
    }

    // ---- Tab operations ----

    void NewTab() {
        SendKeyCombo(VK_CONTROL, 'T');
    }

    void DuplicateTab() {
        wchar_t path[MAX_PATH] = {};
        if (!GetCurrentFolderPath(path, MAX_PATH)) return;
        wcsncpy(g_pendingNavPath, path, MAX_PATH - 1);
        g_pendingNavPath[MAX_PATH - 1] = L'\0';
        g_pendingNavBrowser = nullptr;
        g_pendingNavHwnd = hShellTab;
        SendKeyCombo(VK_CONTROL, 'T');
        if (IsWindow(hShellTab))
            SetTimer(hShellTab, 0x4D43, 500, NavigateNewTabProc);
    }

    void CloseTab() {
        SendKeyCombo(VK_CONTROL, 'W');
    }

    // ---- File operations ----

    void NewFolder() {
        SendKeyCombo(VK_CONTROL, VK_SHIFT, 'N');
    }

    void CopyPath() {
        wchar_t path[MAX_PATH] = {};
        if (!GetCurrentFolderPath(path, MAX_PATH)) return;
        size_t len = wcslen(path) + 1;
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len * sizeof(wchar_t));
        if (!hMem) return;
        wchar_t* pMem = (wchar_t*)GlobalLock(hMem);
        if (!pMem) { GlobalFree(hMem); return; }
        wcsncpy(pMem, path, len);
        GlobalUnlock(hMem);
        if (OpenClipboard(NULL)) {
            EmptyClipboard();
            if (!SetClipboardData(CF_UNICODETEXT, hMem))
                GlobalFree(hMem);
            CloseClipboard();
        } else {
            GlobalFree(hMem);
        }
    }

    // ---- Dispatch ----

    void DoAction(PCWSTR action) {
        if (!action || !hBrowser || !IsWindow(hShellTab)) return;

        if (wcscmp(action, L"goUp") == 0)            GoUp();
        else if (wcscmp(action, L"goBack") == 0)     GoBack();
        else if (wcscmp(action, L"goForward") == 0)  GoForward();
        else if (wcscmp(action, L"goToDesktop") == 0) GoToDesktop();
        else if (wcscmp(action, L"goToHome") == 0)    GoToHome();
        else if (wcscmp(action, L"refresh") == 0)     Refresh();
        else if (wcscmp(action, L"newTab") == 0)      NewTab();
        else if (wcscmp(action, L"duplicateTab") == 0) DuplicateTab();
        else if (wcscmp(action, L"closeTab") == 0)    CloseTab();
        else if (wcscmp(action, L"newFolder") == 0)   NewFolder();
        else if (wcscmp(action, L"copyPath") == 0)    CopyPath();
        // "none" or unknown — do nothing
    }
};

// ---- Globals ----

std::vector<ExplorerWrapper> g_Wrappers;
static std::mutex g_wrappersMutex;

// Lazily initialized on first use (Explorer UI thread has COM already).
// Intentionally leaked (raw pointer) to avoid Release() during DLL_PROCESS_DETACH.
static IUIAutomation* GetUIAutomation() {
    static IUIAutomation* s_pUIAutomation = []{
        IUIAutomation* p = nullptr;
        CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER,
                         __uuidof(IUIAutomation), (void**)&p);
        return p;
    }();
    return s_pUIAutomation;
}

// ---- NavigateNewTabProc (timer callback for duplicate tab) ----

static VOID CALLBACK NavigateNewTabProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    CHECK_INIT_OR_RETURN_VOID();
    KillTimer(hwnd, 0x4D43);

    if (!g_pendingNavPath[0] || !g_pendingNavBrowser) {
        g_pendingNavPath[0] = L'\0';
        g_pendingNavBrowser = nullptr;
        g_pendingNavHwnd = NULL;
        return;
    }

    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(SHParseDisplayName(g_pendingNavPath, NULL, &pidl, 0, NULL)) && pidl) {
        g_pendingNavBrowser->BrowseObject(pidl, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
        CoTaskMemFree(pidl);
    }

    g_pendingNavPath[0] = L'\0';
    g_pendingNavBrowser = nullptr;
    g_pendingNavHwnd = NULL;
}

// ---- Helper: find ExplorerWrapper by shellTab HWND and run action ----

static bool FindShellTabAndDoAction(HWND hWnd, PCWSTR action) {
    if (!hWnd || !IsWindow(hWnd) || !action) return false;
    int limit = 16;
    HWND parent = GetParent(hWnd);
    while (parent && limit-- > 0) {
        wchar_t className[256];
        if (!GetClassName(parent, className, 256)) break;
        if (wcscmp(className, L"ShellTabWindowClass") == 0) {
            // Copy the browser under lock, then act outside the lock
            winrt::com_ptr<IShellBrowser> browser;
            HWND shellTab = parent;
            {
                std::lock_guard<std::mutex> lock(g_wrappersMutex);
                for (ExplorerWrapper& w : g_Wrappers) {
                    if (w.hShellTab == shellTab) {
                        browser = w.GetBrowser();
                        break;
                    }
                }
            }
            if (browser) {
                ExplorerWrapper tmp(shellTab, browser.get());
                tmp.DoAction(action);
                return true;
            }
            break;
        }
        parent = GetParent(parent);
    }
    return false;
}

// ---- SysListView32 subclass ----

LRESULT CALLBACK SysListViewSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                      DWORD_PTR dwRefData) {
    CHECK_INIT_OR_DEFER(hWnd, uMsg, wParam, lParam);

    SettingsSnapshot s = CopySettings();

    if (uMsg == WM_LBUTTONDBLCLK) {
        if (wcscmp(s.doubleClick.c_str(), L"none") == 0)
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(hWnd, &mousePos);
        LVHITTESTINFO ht = {};
        ht.flags = LVHT_NOWHERE;
        ht.pt = mousePos;
        if (ListView_SubItemHitTest(hWnd, &ht) == -1)
            FindShellTabAndDoAction(hWnd, s.doubleClick.c_str());

    } else if (uMsg == WM_MBUTTONDOWN) {
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(hWnd, &mousePos);
        LVHITTESTINFO ht = {};
        ht.flags = LVHT_NOWHERE;
        ht.pt = mousePos;
        if (ListView_SubItemHitTest(hWnd, &ht) != -1)
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        bool singleOn = wcscmp(s.middleClick.c_str(), L"none") != 0;
        bool doubleOn = wcscmp(s.doubleMiddleClick.c_str(), L"none") != 0;
        if (!singleOn && !doubleOn)
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        if (singleOn && !doubleOn) {
            FindShellTabAndDoAction(hWnd, s.middleClick.c_str());
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }

        bool isDouble = (g_midClickTimerId != 0 && g_midClickPendingHwnd == hWnd && IsWindow(hWnd));

        if (isDouble) {
            CancelPendingMidClick();
            FindShellTabAndDoAction(hWnd, s.doubleMiddleClick.c_str());
        } else {
            CancelPendingMidClick();
            g_midClickPendingHwnd = hWnd;
            g_midClickTimerId = SetTimer(hWnd, 0x4D44,
                GetDoubleClickTime(), MidClickTimerProc);
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---- DirectUI subclass ----

struct ClickHelper {
    DWORD time = 0;
    wchar_t className[256];
    HWND hWnd = NULL;
};

static ClickHelper g_currentClick;
static ClickHelper g_lastClick;

LRESULT CALLBACK DUISubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                              DWORD_PTR dwRefData) {
    CHECK_INIT_OR_DEFER(hWnd, uMsg, wParam, lParam);

    if (uMsg != WM_PARENTNOTIFY)
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);

    auto pUIA = GetUIAutomation();
    if (!pUIA)
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);

    SettingsSnapshot s = CopySettings();

    // Middle click — timer-based double-click detection
    if (wParam == WM_MBUTTONDOWN) {
        bool singleOn = wcscmp(s.middleClick.c_str(), L"none") != 0;
        bool doubleOn = wcscmp(s.doubleMiddleClick.c_str(), L"none") != 0;
        if (!singleOn && !doubleOn)
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        POINT mousePos;
        GetCursorPos(&mousePos);
        winrt::com_ptr<IUIAutomationElement> pElement = NULL;
        if (SUCCEEDED(pUIA->ElementFromPoint(mousePos, pElement.put())) && pElement) {
            bstr_ptr clsName;
            if (SUCCEEDED(pElement->get_CurrentClassName(clsName.GetAddress()))) {
                wchar_t* cn = clsName.GetBSTR();
                if (cn && (wcscmp(cn, L"UIGroupItem") == 0 || wcscmp(cn, L"UIItemsView") == 0)) {

                    if (singleOn && !doubleOn) {
                        FindShellTabAndDoAction(hWnd, s.middleClick.c_str());
                        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
                    }

                    bool isDouble = (g_midClickTimerId != 0 && g_midClickPendingHwnd == hWnd);

                    if (isDouble) {
                        CancelPendingMidClick();
                        FindShellTabAndDoAction(hWnd, s.doubleMiddleClick.c_str());
                    } else {
                        CancelPendingMidClick();
                        g_midClickPendingHwnd = hWnd;
                        g_midClickTimerId = SetTimer(hWnd, 0x4D44,
                            GetDoubleClickTime(), MidClickTimerProc);
                    }
                }
            }
        }
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    // Left click — track for double-click detection
    if (wParam == WM_LBUTTONDOWN) {
        if (wcscmp(s.doubleClick.c_str(), L"none") == 0)
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        DWORD now = GetTickCount();
        g_currentClick.time = now;
        g_currentClick.hWnd = hWnd;

        POINT mousePos;
        GetCursorPos(&mousePos);
        winrt::com_ptr<IUIAutomationElement> pElement = NULL;
        if (SUCCEEDED(pUIA->ElementFromPoint(mousePos, pElement.put())) && pElement) {
            bstr_ptr clsName;
            if (SUCCEEDED(pElement->get_CurrentClassName(clsName.GetAddress()))) {
                wchar_t* cn = clsName.GetBSTR();
                if (!cn || (wcscmp(cn, L"UIGroupItem") != 0 && wcscmp(cn, L"UIItemsView") != 0)) {
                    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
                }
                wcsncpy(g_currentClick.className, cn, 255);
                g_currentClick.className[255] = L'\0';

                DWORD delta = g_currentClick.time - g_lastClick.time;
                if (g_currentClick.hWnd == g_lastClick.hWnd &&
                    ((wcscmp(cn, L"UIGroupItem") == 0 &&
                      wcscmp(g_lastClick.className, L"UIGroupItem") == 0) ||
                     (wcscmp(cn, L"UIItemsView") == 0 &&
                      wcscmp(g_lastClick.className, L"UIItemsView") == 0)) &&
                    delta <= GetDoubleClickTime()) {
                    FindShellTabAndDoAction(hWnd, s.doubleClick.c_str());
                    g_lastClick.time = 0;  // prevent triple-click from double-firing
                } else {
                    g_lastClick.time = now;
                }

                g_lastClick.hWnd = hWnd;
                wcsncpy(g_lastClick.className, cn, 255);
                g_lastClick.className[255] = L'\0';
            }
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---- CreateWindowExW hook ----

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_original;

HWND WINAPI CreateWindowExW_hook(DWORD dwExStyle, LPCWSTR lpClassName,
                                  LPCWSTR lpWindowName, DWORD dwStyle,
                                  int X, int Y, int nWidth, int nHeight,
                                  HWND hWndParent, HMENU hMenu,
                                  HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_original(dwExStyle, lpClassName, lpWindowName,
                                          dwStyle, X, Y, nWidth, nHeight,
                                          hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd || !g_initialized) return hWnd;

    wchar_t className[256];
    if (!GetClassName(hWnd, className, 256)) return hWnd;
    if (wcscmp(className, L"SysListView32") != 0 &&
        wcscmp(className, L"DirectUIHWND") != 0) return hWnd;

    HWND shellTab = NULL, defView = NULL;
    HWND p = GetParent(hWnd);
    int limit = 16;
    while (p && limit-- > 0) {
        wchar_t pc[256];
        if (!GetClassName(p, pc, 256)) break;
        if (wcscmp(pc, L"SHELLDLL_DefView") == 0) defView = p;
        if (wcscmp(pc, L"ShellTabWindowClass") == 0) { shellTab = p; break; }
        p = GetParent(p);
    }
    if (!shellTab || !defView || !IsWindow(shellTab)) return hWnd;

    if (wcscmp(className, L"SysListView32") == 0) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, SysListViewSubclass, 0);
        { std::lock_guard<std::mutex> lk(g_wrappersMutex);
          for (auto& w : g_Wrappers)
            if (w.hShellTab == shellTab) { w.hListView = hWnd; break; }
        }
    } else {
        if (IsWindow(defView))
            WindhawkUtils::SetWindowSubclassFromAnyThread(defView, DUISubclass, 0);
        { std::lock_guard<std::mutex> lk(g_wrappersMutex);
          for (auto& w : g_Wrappers)
            if (w.hShellTab == shellTab) { w.hListView = defView; break; }
        }
    }
    return hWnd;
}

// ---- FileCabinet_CreateViewWindow2 hook ----

typedef HRESULT (*__cdecl FileCabinet_CreateViewWindow2_t)(
    IShellBrowser*, void*, IShellView*, IShellView*, void*, HWND*);
FileCabinet_CreateViewWindow2_t FileCabinet_CreateViewWindow2Original;

HRESULT __cdecl FileCabinet_CreateViewWindow2Hook(
    IShellBrowser* pBrowser, void* var1, IShellView* psv1,
    IShellView* psv2, void* var2, HWND* hWnd) {
    HRESULT hRes = FileCabinet_CreateViewWindow2Original(pBrowser, var1, psv1, psv2, var2, hWnd);
    if (!g_initialized || FAILED(hRes) || !hWnd || !*hWnd) return hRes;

    HWND shellTab = GetParent(*hWnd);
    if (shellTab && IsWindow(shellTab)) {
        {
            std::lock_guard<std::mutex> lock(g_wrappersMutex);
            g_Wrappers.push_back(ExplorerWrapper(shellTab, pBrowser));
        }
        if (g_pendingNavPath[0] && !g_pendingNavBrowser) {
            g_pendingNavBrowser.copy_from(pBrowser);
        }
    }
    return hRes;
}

// ---- Enumeration for already-open Explorer windows ----

BOOL CALLBACK InitEnumChildWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        wchar_t className[256];
        GetClassName(hWnd, className, 256);
        if (wcscmp(className, L"SHELLDLL_DefView") == 0) {
            HWND shellTab = (HWND)lParam;
            auto browser = winrt::com_ptr<IShellBrowser>{
                reinterpret_cast<IShellBrowser*>((void*)SendMessage(shellTab, WM_USER + 7, 0, 0)),
                winrt::take_ownership_from_abi
            };
            if (browser != NULL) {
                ExplorerWrapper wrapper(shellTab, browser.get());
                HWND lv = FindWindowEx(hWnd, NULL, L"SysListView32", NULL);
                HWND dui = FindWindowEx(hWnd, NULL, L"DirectUIHWND", NULL);
                if (lv) {
                    if (WindhawkUtils::SetWindowSubclassFromAnyThread(lv, SysListViewSubclass, 0)) {
                        Wh_Log(L"SysListView32 Subclassed %p", lv);
                        wrapper.hListView = lv;
                    }
                } else if (dui) {
                    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DUISubclass, 0)) {
                        Wh_Log(L"DirectUIHWND Subclassed %p", hWnd);
                        wrapper.hListView = hWnd;
                    }
                }
                if (wrapper.hListView) {
                    std::lock_guard<std::mutex> lk(g_wrappersMutex);
                    g_Wrappers.push_back(wrapper);
                } else Wh_Log(L"Failed to setup wrapper for %p", shellTab);
                return FALSE;
            }
        }
    }
    return TRUE;
}

BOOL CALLBACK InitEnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        wchar_t className[256];
        GetClassName(hWnd, className, 256);
        if (wcscmp(className, L"CabinetWClass") == 0) {
            HWND shellTab = FindWindowEx(hWnd, NULL, L"ShellTabWindowClass", NULL);
            if (shellTab != NULL)
                EnumChildWindows(shellTab, InitEnumChildWindowsProc, (LPARAM)shellTab);
        }
    }
    return TRUE;
}

// ---- Windhawk lifecycle ----

BOOL Wh_ModInit() {
    Wh_Log(L"Click on Empty Explorer Init");

    LoadSettings();

    HMODULE hExplorerFrame = LoadLibraryExW(L"explorerframe.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hExplorerFrame) {
        Wh_Log(L"Failed to load explorerframe.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK explorerframe_dll_hooks[] = {
        {{
            L"long __cdecl FileCabinet_CreateViewWindow2(struct IShellBrowser *,struct tagFolderSetDataBase *,struct IShellView *,struct IShellView *,struct tagRECT *,struct HWND__ * *)"
        },
        (void**)&FileCabinet_CreateViewWindow2Original,
        (void*)FileCabinet_CreateViewWindow2Hook,
        FALSE}
    };
    if (!WindhawkUtils::HookSymbols(hExplorerFrame, explorerframe_dll_hooks, ARRAYSIZE(explorerframe_dll_hooks))) {
        Wh_Log(L"Failed to hook ExplorerFrame.dll");
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_hook, (void**)&CreateWindowExW_original);

    InterlockedExchange(&g_initialized, 1);
    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(InitEnumWindowsProc, 0);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    // Block subclass callbacks and hook code before cleanup
    InterlockedExchange(&g_initialized, 0);

    CancelPendingMidClick();

    if (g_pendingNavHwnd && IsWindow(g_pendingNavHwnd)) {
        KillTimer(g_pendingNavHwnd, 0x4D43);
    }
    g_pendingNavHwnd = NULL;
    g_pendingNavBrowser = nullptr;
    g_pendingNavPath[0] = L'\0';

    // Collect HWNDs under lock, then remove subclasses outside the lock
    // (RemoveWindowSubclassFromAnyThread does SendMessage which can deadlock
    //  if the target thread is waiting on g_wrappersMutex)
    struct SubclassInfo { HWND hWnd; bool isListView; };
    std::vector<SubclassInfo> toRemove;
    {
        std::lock_guard<std::mutex> lk(g_wrappersMutex);
        for (ExplorerWrapper& wrapper : g_Wrappers) {
            HWND hWnd = wrapper.hListView;
            if (hWnd && IsWindow(hWnd)) {
                wchar_t className[256];
                if (GetClassName(hWnd, className, 256))
                    toRemove.push_back({ hWnd, wcscmp(className, L"SysListView32") == 0 });
            }
        }
        g_Wrappers.clear();
    }
    for (auto& info : toRemove)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            info.hWnd, info.isListView ? SysListViewSubclass : DUISubclass);
}
