// ==WindhawkMod==
// @id              file-explorer-details-autofit-columns
// @name            File Explorer Details Auto-Fit Columns
// @description     Automatically fits all column widths to their content when refreshing in Details view. Has no effect on other view modes.
// @version         1.0.0
// @author          Armaninyow
// @github          https://github.com/armaninyow
// @include         explorer.exe
// @compilerOptions -lole32 -lshlwapi -lpropsys -lshell32 -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- delay: 400
  $name: Refresh Delay (ms)
  $description: "How long to wait after a refresh before fitting columns to content. Increase if columns fit before all files are loaded (e.g. large folders). Decrease for a snappier response. Default: 400ms."
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# File Explorer Details Auto-Fit Columns

Automatically fits all visible column widths to their content whenever a folder is opened or refreshed in `Details view`, eliminating truncated text and ellipsis (`...`).

![File Explorer Details Auto-Fit Columns](https://raw.githubusercontent.com/armaninyow/Remove-Context-Menu-Items/refs/heads/main/FEDAFC.gif)

## Triggers

- Opening or navigating to a folder
- Minimizing and restoring the Explorer window
- `Ctrl + R`
- `F5`
- Right-click context menu → Refresh
- The refresh button in the toolbar

Only affects `Details view`. Other view modes (Icons, Tiles, List, etc.) are untouched.

## Settings

`Refresh Delay (ms)`: How long the mod waits before auto-fitting columns. The default is 400ms, which gives Explorer enough time to finish loading files before measuring content width. If you notice columns fitting too early in large folders, increase this value. If you want a snappier response in small folders, decrease it.
*/
// ==/WindhawkModReadme==

#include <initguid.h>
#include <shobjidl.h>
#include <propkey.h>
#include <shlobj.h>
#include <shlguid.h>
#include <servprov.h>
#include <exdisp.h>
#include <vector>
#include <unordered_map>
#include <windhawk_utils.h>

DEFINE_GUID(IID_IFolderView2_,
    0x1af3a467, 0x214f, 0x4298,
    0x90, 0x8e, 0x06, 0xb0, 0x3e, 0x0b, 0x39, 0xf9);

DEFINE_GUID(IID_IColumnManager_,
    0xd8ec27bb, 0x3f3b, 0x4042,
    0xb1, 0x0a, 0x4a, 0xcf, 0xd9, 0x24, 0xd4, 0x53);

DEFINE_GUID(IID_IServiceProvider_,
    0x6d5140c1, 0x7436, 0x11ce,
    0x80, 0x34, 0x00, 0xaa, 0x00, 0x60, 0x09, 0xfa);

#define EXPLORER_REFRESH_CMD_1 0xA220  // Ctrl+R / F5
#define EXPLORER_REFRESH_CMD_2 0x7103  // Context menu Refresh
#define AUTOFIT_TIMER_ID       0xAF17

// Window classes that receive refresh commands
// ShellTabWindowClass : WM_COMMAND (Ctrl+R, F5, context menu)
// ReBarWindow32       : WM_NOTIFY (toolbar button, via ToolbarWindow32 child)
// ToolbarWindow32     : WM_NOTIFY (toolbar button)
static const PCWSTR kSubclassTargets[] = {
    L"ShellTabWindowClass",
    L"ReBarWindow32",
    L"ToolbarWindow32",
};

// ---------------------------------------------------------------------------
// Thread safety: single CRITICAL_SECTION guards all global map access
// ---------------------------------------------------------------------------

static CRITICAL_SECTION g_cs;

// ---------------------------------------------------------------------------
// Map: ShellTabWindowClass HWND -> IShellView* (AddRef'd)
// Map: subclassed window HWND -> its ShellTabWindowClass HWND
// ---------------------------------------------------------------------------

static std::unordered_map<HWND, IShellView*> g_tabShellViews;
static std::unordered_map<HWND, HWND>        g_windowToTab;

// ---------------------------------------------------------------------------
// Auto-fit all visible columns to content
// ---------------------------------------------------------------------------

static void AutoFitColumns(IShellView* pShellView) {
    IFolderView2* pFV2 = nullptr;
    if (FAILED(pShellView->QueryInterface(IID_IFolderView2_,
                                          reinterpret_cast<void**>(&pFV2))) || !pFV2)
        return;

    FOLDERVIEWMODE viewMode = FVM_AUTO;
    int iconSize = 0;
    HRESULT hr = pFV2->GetViewModeAndIconSize(&viewMode, &iconSize);
    pFV2->Release();

    if (FAILED(hr) || viewMode != FVM_DETAILS)
        return;

    IColumnManager* pCM = nullptr;
    if (FAILED(pShellView->QueryInterface(IID_IColumnManager_,
                                          reinterpret_cast<void**>(&pCM))) || !pCM)
        return;

    UINT colCount = 0;
    if (FAILED(pCM->GetColumnCount(CM_ENUM_VISIBLE, &colCount)) || colCount == 0) {
        pCM->Release();
        return;
    }

    std::vector<PROPERTYKEY> keys(colCount);
    if (SUCCEEDED(pCM->GetColumns(CM_ENUM_VISIBLE, keys.data(), colCount))) {
        for (UINT i = 0; i < colCount; i++) {
            CM_COLUMNINFO ci = {};
            ci.cbSize = sizeof(ci);
            ci.dwMask = CM_MASK_WIDTH;
            ci.uWidth = CM_WIDTH_AUTOSIZE;
            pCM->SetColumnInfo(keys[i], &ci);
        }
        Wh_Log(L"Auto-fitted %u column(s)", colCount);
    }

    pCM->Release();
}

// ---------------------------------------------------------------------------
// Find the ShellTabWindowClass ancestor of a HWND
// ---------------------------------------------------------------------------

static HWND FindTabWindow(HWND hwnd) {
    WCHAR cls[64] = {};
    HWND cur = hwnd;
    while (cur) {
        GetClassNameW(cur, cls, 64);
        if (wcscmp(cls, L"ShellTabWindowClass") == 0)
            return cur;
        cur = GetParent(cur);
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Check if a window class is one we want to subclass
// ---------------------------------------------------------------------------

static bool IsSubclassTarget(HWND hwnd) {
    WCHAR cls[64] = {};
    GetClassNameW(hwnd, cls, 64);
    for (PCWSTR target : kSubclassTargets) {
        if (wcscmp(cls, target) == 0)
            return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// Subclass proc
// ---------------------------------------------------------------------------

static LRESULT CALLBACK ExplorerSubclassProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    DWORD_PTR dwRefData)
{
    bool isRefresh = false;

    if (uMsg == WM_COMMAND) {
        WORD cmdId = LOWORD(wParam);
        if (cmdId == EXPLORER_REFRESH_CMD_1 || cmdId == EXPLORER_REFRESH_CMD_2)
            isRefresh = true;
    }

    if (uMsg == WM_KEYDOWN && wParam == VK_F5)
        isRefresh = true;

    if (uMsg == WM_NOTIFY) {
        NMHDR* hdr = reinterpret_cast<NMHDR*>(lParam);
        if (hdr && hdr->code == -715 && hdr->idFrom == 0)
            isRefresh = true;
    }

    if (isRefresh) {
        HWND hwndTab = nullptr;
        {
            EnterCriticalSection(&g_cs);
            auto it = g_windowToTab.find(hwnd);
            if (it != g_windowToTab.end())
                hwndTab = it->second;
            LeaveCriticalSection(&g_cs);
        }

        UINT delay = static_cast<UINT>(Wh_GetIntSetting(L"delay"));
        if (delay == 0) delay = 400;
        HWND hwndTimer = hwndTab ? hwndTab : hwnd;
        SetTimer(hwndTimer, AUTOFIT_TIMER_ID, delay, nullptr);
    }

    if (uMsg == WM_TIMER && wParam == AUTOFIT_TIMER_ID) {
        KillTimer(hwnd, AUTOFIT_TIMER_ID);

        IShellView* pSV = nullptr;
        {
            EnterCriticalSection(&g_cs);
            auto it = g_tabShellViews.find(hwnd);
            if (it != g_tabShellViews.end() && it->second) {
                pSV = it->second;
                pSV->AddRef(); // keep alive outside the lock
            }
            LeaveCriticalSection(&g_cs);
        }

        if (pSV) {
            AutoFitColumns(pSV);
            pSV->Release();
        }
        return 0;
    }

    if (uMsg == WM_NCDESTROY) {
        KillTimer(hwnd, AUTOFIT_TIMER_ID);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, ExplorerSubclassProc);

        EnterCriticalSection(&g_cs);
        g_windowToTab.erase(hwnd);
        auto it = g_tabShellViews.find(hwnd);
        if (it != g_tabShellViews.end()) {
            if (it->second) it->second->Release();
            g_tabShellViews.erase(it);
        }
        LeaveCriticalSection(&g_cs);
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Subclass a window if it's a target class, updating tab association
// ---------------------------------------------------------------------------

static void SubclassTargetIfNeeded(HWND hwnd, HWND hwndTab) {
    if (!hwnd || !IsSubclassTarget(hwnd)) return;

    EnterCriticalSection(&g_cs);
    bool isNew = g_windowToTab.find(hwnd) == g_windowToTab.end();
    g_windowToTab[hwnd] = hwndTab; // always update tab association
    LeaveCriticalSection(&g_cs);

    if (isNew)
        WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, ExplorerSubclassProc, 0);
}

// ---------------------------------------------------------------------------
// EnumChildWindows callback — only subclasses target window classes
// ---------------------------------------------------------------------------

struct EnumChildData { HWND hwndTab; };

static BOOL CALLBACK SubclassChildProc(HWND child, LPARAM lp) {
    auto* data = reinterpret_cast<EnumChildData*>(lp);
    SubclassTargetIfNeeded(child, data->hwndTab);
    return TRUE;
}

// ---------------------------------------------------------------------------
// Hook: CDefView::UIActivate
// ---------------------------------------------------------------------------

using CDefView_UIActivate_t = HRESULT(__thiscall*)(void* pThis, UINT uState);
CDefView_UIActivate_t CDefView_UIActivate_orig = nullptr;

HRESULT __thiscall CDefView_UIActivate_hook(void* pThis, UINT uState) {
    HRESULT hr = CDefView_UIActivate_orig(pThis, uState);

    if (SUCCEEDED(hr) &&
        (uState == SVUIA_ACTIVATE_FOCUS || uState == SVUIA_ACTIVATE_NOFOCUS)) {

        auto* pShellView = reinterpret_cast<IShellView*>(pThis);
        HWND hwndView = nullptr;
        pShellView->GetWindow(&hwndView);
        if (!hwndView) return hr;

        HWND hwndTab = FindTabWindow(hwndView);

        HWND hwndTop = hwndView;
        while (GetParent(hwndTop))
            hwndTop = GetParent(hwndTop);

        // Update stored IShellView for this tab
        if (hwndTab) {
            EnterCriticalSection(&g_cs);
            auto it = g_tabShellViews.find(hwndTab);
            if (it != g_tabShellViews.end() && it->second)
                it->second->Release();
            pShellView->AddRef();
            g_tabShellViews[hwndTab] = pShellView;
            LeaveCriticalSection(&g_cs);
        }

        // Subclass only target window classes under hwndTop
        SubclassTargetIfNeeded(hwndTop, hwndTab);
        EnumChildData data = { hwndTab };
        EnumChildWindows(hwndTop, SubclassChildProc, reinterpret_cast<LPARAM>(&data));

        // Auto-fit on open/navigate/tab switch
        UINT delay = static_cast<UINT>(Wh_GetIntSetting(L"delay"));
        if (delay == 0) delay = 400;
        HWND hwndTimer = hwndTab ? hwndTab : hwndTop;
        SetTimer(hwndTimer, AUTOFIT_TIMER_ID, delay, nullptr);
    }
    return hr;
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    InitializeCriticalSection(&g_cs);

    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32) {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
        {
            {
                L"public: virtual long __cdecl CDefView::UIActivate(unsigned int)",
                L"long __cdecl CDefView::UIActivate(unsigned int)",
                L"public: virtual long __thiscall CDefView::UIActivate(unsigned int)",
                L"long __thiscall CDefView::UIActivate(unsigned int)",
            },
            &CDefView_UIActivate_orig,
            CDefView_UIActivate_hook,
            false
        },
    };

    if (!WindhawkUtils::HookSymbols(hShell32, shell32DllHooks, ARRAYSIZE(shell32DllHooks))) {
        Wh_Log(L"ERROR: Could not hook CDefView::UIActivate");
        DeleteCriticalSection(&g_cs);
        return FALSE;
    }

    Wh_Log(L"CDefView::UIActivate hooked successfully");
    return TRUE;
}

void Wh_ModAfterInit() {}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    // Collect handles and release shell views under the lock,
    // then remove subclasses outside the lock to avoid deadlock
    std::vector<HWND> windowsToUnsubclass;
    std::vector<IShellView*> viewsToRelease;

    EnterCriticalSection(&g_cs);
    for (auto& [hwnd, _] : g_windowToTab) {
        KillTimer(hwnd, AUTOFIT_TIMER_ID);
        windowsToUnsubclass.push_back(hwnd);
    }
    g_windowToTab.clear();
    for (auto& [hwnd, pSV] : g_tabShellViews) {
        if (pSV) viewsToRelease.push_back(pSV);
    }
    g_tabShellViews.clear();
    LeaveCriticalSection(&g_cs);

    // Safe to call outside the lock
    for (HWND hwnd : windowsToUnsubclass)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, ExplorerSubclassProc);
    for (IShellView* pSV : viewsToRelease)
        pSV->Release();

    DeleteCriticalSection(&g_cs);
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged — new delay: %dms", Wh_GetIntSetting(L"delay"));
}
