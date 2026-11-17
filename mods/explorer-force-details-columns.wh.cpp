// ==WindhawkMod==
// @id              explorer-force-details-columns
// @name            Explorer Details View Columns
// @description     Forces a fixed set of columns, and optionally sorting, in File Explorer Details view. Columns and their order are configurable. Has no effect on other view modes.
// @version         1.5
// @author          ernisn
// @github          https://github.com/ernisn
// @include         explorer.exe
// @compilerOptions -lole32 -lshell32 -lpropsys -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Details View Columns

Forces a specific set of columns whenever a File Explorer folder is in _Details_ view. **Other view modes (_Icons_, _Tiles_, _List_, etc.) are not affected.**

Configure which columns to show and in what order in Settings. Each entry takes a Shell property name and a width **(if _Force Width_ enabled)** in logical pixels at 100% DPI scaling (for consistent widths in case of multiple monitors with different DPI settings). A sort column and direction can optionally be enforced as well.

Common property names:
- `System.ItemNameDisplay` - Name
- `System.Size` - Size
- `System.DateModified` - Date modified
- `System.DateCreated` - Date created
- `System.ItemTypeText` - Type
- `System.FileAttributes` - Attributes
- `System.Author` - Authors
- `System.Music.Artist` - Contributing artists
- `System.Media.Duration` - Length
- `System.Image.Dimensions` - Dimensions

For a full list of available Shell property names, see: https://learn.microsoft.com/en-us/windows/win32/properties/props

**Note:**
- Any property name not recognised by Windows will be skipped. Duplicate property entries will only take the first occurrence.
- To allow a column to be freely resized, turn off "Force Width". To auto-size a column to its content instead, set its Width to -1. Like Explorer's own "Size column to fit", only the items currently shown in the window are measured, so longer values further down the list can still be cut off.
- If "Sort By" is set, manual sorting changes stay temporary, e.g. manual width changes of forced-width columns.
- By default, virtual folders that have their own specialized columns (such as the Recycle Bin with _Original Location_ and _Date Deleted_) are not affected. Turn off "Exclude virtual folders" to force the columns there as well.

**Minimum Column Width:**
- Windows doesn't allow a column to be narrower than about 10 characters (around 90 logical pixels on Windows 11), neither when forced here nor when dragged manually. "Minimum Column Width" lowers that limit, so that smaller widths take effect and columns can be dragged narrower. Set it to 0 to keep the Windows limit.
- The lowered limit applies to all folders in File Explorer, including the virtual folders that are excluded from the forced columns.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- columns:
  - - property: System.ItemNameDisplay
      $name: Column Property Name
    - force_width: true
      $name: Force Width
      $description: If enabled, the column width will always use the value below, regardless of manual changes, which will stay temporary and be reverted after re-opening the folder.
    - width: 270
      $name: Width
      $description: Width in logical pixels at 100% DPI scaling, or -1 to auto-size to the items shown in the window. Only takes effect if Force Width is turned on.
  - - property: System.Size
    - force_width: false
    - width: 30
  - - property: System.DateModified
    - force_width: true
    - width: 110
  - - property: System.ItemTypeText
    - force_width: false
    - width: 60
  $name: Columns
  $description: "Columns to show in order in Details view. Width is in logical pixels at 100% DPI scaling."
- min_width: 20
  $name: Minimum Column Width
  $description: Lowers the narrowest width Windows allows for a column (about 90 logical pixels by default), for forced widths as well as manual dragging. In logical pixels at 100% DPI scaling. Set to 0 to keep the Windows limit.
- sort_property: ""
  $name: Sort By
  $description: Shell property name to sort by, e.g. System.DateModified. Leave empty to not enforce sorting.
- sort_descending: false
  $name: Sort Descending
  $description: Only takes effect if Sort By is set.
- exclude_virtual: true
  $name: Exclude virtual folders
  $description: Do not force columns in virtual folders that have their own specialized columns, such as the Recycle Bin, This PC and search results.
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <shobjidl.h>
#include <propkey.h>
#include <propsys.h>
#include <exdisp.h>
#include <shlguid.h>
#include <servprov.h>
#include <commctrl.h>
#include <wrl/client.h>
#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <mutex>
#include <unordered_set>
#include <utility>
#include <vector>
#include <windhawk_utils.h>

using Microsoft::WRL::ComPtr;

// Not declared with the default WINVER
WINUSERAPI UINT WINAPI GetDpiForWindow(HWND hwnd);
WINUSERAPI UINT WINAPI GetDpiForSystem(VOID);
#ifndef WM_DPICHANGED_AFTERPARENT
#define WM_DPICHANGED_AFTERPARENT 0x02E3
#endif

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

struct ColumnEntry {
    PROPERTYKEY key;
    int         width;  // logical px at 96 DPI; 0 = free resize, -1 = auto-size
};

static std::vector<ColumnEntry> g_columns;
static PROPERTYKEY              g_sortKey;
static bool                     g_sortEnabled;
static bool                     g_sortDescending;
static bool                     g_excludeVirtualFolders;
static std::atomic<int>         g_minColumnWidth;  // logical px at 96 DPI; 0 = Windows limit

static std::mutex               g_subclassedMutex;
static std::unordered_set<HWND> g_subclassed;

// Auto-sizing measures the displayed rows, so views wait for their items to be painted first
static bool                     g_deferAutoSize;
static UINT                     g_autoSizeMessage;
static std::mutex               g_pendingAutoSizeMutex;
static std::unordered_set<HWND> g_pendingAutoSize;
static std::atomic<bool>        g_anyPendingAutoSize;

static void LoadSettings() {
    g_columns.clear();

    g_excludeVirtualFolders = Wh_GetIntSetting(L"exclude_virtual");
    g_sortDescending = Wh_GetIntSetting(L"sort_descending");
    g_minColumnWidth = std::max(0, Wh_GetIntSetting(L"min_width"));

    PCWSTR sortProp = Wh_GetStringSetting(L"sort_property");
    g_sortEnabled =
        *sortProp && SUCCEEDED(PSGetPropertyKeyFromName(sortProp, &g_sortKey));
    if (*sortProp && !g_sortEnabled)
        Wh_Log(L"Sort property %s not recognised", sortProp);
    Wh_FreeStringSetting(sortProp);

    for (int i = 0;; i++) {
        PCWSTR prop = Wh_GetStringSetting(L"columns[%d].property", i);
        if (!*prop) {
            Wh_FreeStringSetting(prop);
            break;
        }

        PROPERTYKEY key;
        if (FAILED(PSGetPropertyKeyFromName(prop, &key))) {
            Wh_Log(L"Column[%d]: %s not recognised, skipped", i, prop);
        } else if (std::any_of(g_columns.begin(), g_columns.end(),
                               [&key](const auto& c) { return c.key == key; })) {
            Wh_Log(L"Column[%d]: %s duplicate, skipped", i, prop);
        } else {
            int width = Wh_GetIntSetting(L"columns[%d].force_width", i)
                            ? Wh_GetIntSetting(L"columns[%d].width", i)
                            : 0;
            g_columns.push_back({key, width});
        }
        Wh_FreeStringSetting(prop);
    }

    Wh_Log(L"Loaded %zu columns", g_columns.size());
}

// ---------------------------------------------------------------------------
// Column enforcement
// ---------------------------------------------------------------------------

static bool ApplyForcedColumns(IUnknown* view,
                               bool subclassForDpi,
                               bool autoSizeNow);

static void SetAutoSizePending(HWND hwnd, bool pending) {
    std::lock_guard<std::mutex> lock(g_pendingAutoSizeMutex);
    if (pending)
        g_pendingAutoSize.insert(hwnd);
    else
        g_pendingAutoSize.erase(hwnd);
    g_anyPendingAutoSize = !g_pendingAutoSize.empty();
}

// Re-applies widths when the view moves to a monitor with a different DPI, and auto-sizes once the items are painted
static LRESULT CALLBACK ViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                         LPARAM lParam, DWORD_PTR dwRefData) {
    if (g_autoSizeMessage && uMsg == g_autoSizeMessage) {
        ApplyForcedColumns((IUnknown*)dwRefData, false, true);
        return 0;
    }

    if (uMsg == WM_NCDESTROY) {
        {
            std::lock_guard<std::mutex> lock(g_subclassedMutex);
            g_subclassed.erase(hWnd);
        }
        SetAutoSizePending(hWnd, false);
    }

    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

    // After the view has handled the DPI change itself
    if (uMsg == WM_DPICHANGED_AFTERPARENT)
        ApplyForcedColumns((IUnknown*)dwRefData, false, !g_deferAutoSize);

    return result;
}

// Virtual folders (Recycle Bin, This PC, search results, etc.) aren't part of the file system + with specialized columns
static bool IsVirtualFolder(IFolderView2* folderView) {
    PIDLIST_ABSOLUTE pidl;
    if (FAILED(SHGetIDListFromObject(folderView, &pidl)))
        return false;

    ComPtr<IShellItem> item;
    SFGAOF attributes = 0;
    HRESULT hr = SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&item));
    if (SUCCEEDED(hr))
        hr = item->GetAttributes(SFGAO_FILESYSTEM, &attributes);
    CoTaskMemFree(pidl);

    return SUCCEEDED(hr) && !(attributes & SFGAO_FILESYSTEM);
}

// Returns true if anything actually changed. subclassForDpi is set only when view can be retained. Without autoSizeNow, auto-sized columns are sized once the items are displayed
static bool ApplyForcedColumns(IUnknown* view,
                               bool subclassForDpi,
                               bool autoSizeNow) {
    if (g_columns.empty())
        return false;

    ComPtr<IShellView> shellView;
    ComPtr<IFolderView2> folderView;
    ComPtr<IColumnManager> columnManager;
    if (FAILED(view->QueryInterface(IID_PPV_ARGS(&shellView))) ||
        FAILED(view->QueryInterface(IID_PPV_ARGS(&folderView))) ||
        FAILED(view->QueryInterface(IID_PPV_ARGS(&columnManager))))
        return false;

    FOLDERVIEWMODE viewMode = FVM_AUTO;
    int iconSize;
    if (FAILED(folderView->GetViewModeAndIconSize(&viewMode, &iconSize)) ||
        viewMode != FVM_DETAILS)
        return false;

    if (g_excludeVirtualFolders && IsVirtualFolder(folderView.Get())) {
        Wh_Log(L"Skipping virtual folder");
        return false;
    }

    bool changed = false;

    // Column set and order
    UINT count = 0;
    bool setColumns =
        FAILED(columnManager->GetColumnCount(CM_ENUM_VISIBLE, &count)) ||
        count != g_columns.size();
    if (!setColumns) {
        std::vector<PROPERTYKEY> current(count);
        setColumns = FAILED(
            columnManager->GetColumns(CM_ENUM_VISIBLE, current.data(), count));
        for (UINT i = 0; !setColumns && i < count; i++)
            setColumns = !(current[i] == g_columns[i].key);
    }
    if (setColumns) {
        // SetColumns resets widths to the defaults, which Explorer won't let the user drag back down, so widths that aren't forced are kept
        std::vector<PROPERTYKEY> keys;
        std::vector<std::pair<PROPERTYKEY, UINT>> keptWidths;
        for (const auto& col : g_columns) {
            keys.push_back(col.key);

            CM_COLUMNINFO ci = {sizeof(ci), CM_MASK_WIDTH};
            if (!col.width &&
                SUCCEEDED(columnManager->GetColumnInfo(col.key, &ci)) &&
                ci.uWidth)
                keptWidths.push_back({col.key, ci.uWidth});
        }

        changed |= SUCCEEDED(
            columnManager->SetColumns(keys.data(), (UINT)keys.size()));

        for (const auto& [key, width] : keptWidths) {
            CM_COLUMNINFO ci = {sizeof(ci), CM_MASK_WIDTH};
            ci.uWidth = width;
            columnManager->SetColumnInfo(key, &ci);
        }
    }

    // Widths
    HWND hwnd = nullptr;
    shellView->GetWindow(&hwnd);
    UINT dpi = hwnd ? GetDpiForWindow(hwnd) : GetDpiForSystem();
    bool autoSizeLater = false;

    for (const auto& col : g_columns) {
        if (!col.width)
            continue;

        if (col.width < 0 && !autoSizeNow) {
            autoSizeLater = true;
            continue;
        }

        CM_COLUMNINFO ci = {sizeof(ci), CM_MASK_WIDTH};
        if (FAILED(columnManager->GetColumnInfo(col.key, &ci)))
            continue;

        UINT width =
            col.width < 0 ? CM_WIDTH_AUTOSIZE : MulDiv(col.width, dpi, 96);
        // ±1px tolerance avoids update loops from DPI rounding
        if (col.width < 0 || std::abs((int)ci.uWidth - (int)width) > 1) {
            ci.uWidth = width;
            if (col.width > 0) {
                // Used when the column is resized or auto-sized
                ci.dwMask |= CM_MASK_DEFAULTWIDTH | CM_MASK_IDEALWIDTH;
                ci.uDefaultWidth = ci.uIdealWidth = width;
            }
            changed |= SUCCEEDED(columnManager->SetColumnInfo(col.key, &ci));
        }
    }

    // Sorting
    if (g_sortEnabled) {
        SORTCOLUMN sort = {g_sortKey,
                           g_sortDescending ? SORT_DESCENDING : SORT_ASCENDING};
        SORTCOLUMN current;
        int sortCount = 0;
        folderView->GetSortColumnCount(&sortCount);
        if (sortCount != 1 || FAILED(folderView->GetSortColumns(&current, 1)) ||
            !(current.propkey == sort.propkey) ||
            current.direction != sort.direction) {
            changed |= SUCCEEDED(folderView->SetSortColumns(&sort, 1));
        }
    }

    if (autoSizeLater && hwnd)
        SetAutoSizePending(hwnd, true);

    if (subclassForDpi && hwnd) {
        std::lock_guard<std::mutex> lock(g_subclassedMutex);
        if (g_subclassed.insert(hwnd).second)
            WindhawkUtils::SetWindowSubclassFromAnyThread(
                hwnd, ViewSubclassProc, (DWORD_PTR)view);
    }

    return changed;
}

// Called after the view is painted, auto-sizes it if it's waiting and its items are displayed by now
// Posted to the view, since changing the columns while the items view is still painting leaves the rows without text
static void AutoSizeIfPending(IUnknown* view) {
    ComPtr<IShellView> shellView;
    ComPtr<IFolderView> folderView;
    HWND hwnd = nullptr;
    if (FAILED(view->QueryInterface(IID_PPV_ARGS(&shellView))) ||
        FAILED(shellView->GetWindow(&hwnd)) || !hwnd)
        return;

    {
        std::lock_guard<std::mutex> lock(g_pendingAutoSizeMutex);
        if (!g_pendingAutoSize.contains(hwnd))
            return;
    }

    // The count of the items view itself, so the counted items are displayed once it's painted
    int itemCount = 0;
    if (FAILED(view->QueryInterface(IID_PPV_ARGS(&folderView))) ||
        FAILED(folderView->ItemCount(SVGIO_ALLVIEW, &itemCount)) ||
        itemCount <= 0)
        return;

    SetAutoSizePending(hwnd, false);
    PostMessageW(hwnd, g_autoSizeMessage, 0, 0);
}

// ---------------------------------------------------------------------------
// Apply to all currently open Explorer windows
// ---------------------------------------------------------------------------

// The header keeps the minimum widths it read when its columns were last updated, re-setting the widths makes it read the current ones
static void RefreshMinimumWidths(IShellView* view) {
    ComPtr<IFolderView2> folderView;
    ComPtr<IColumnManager> columnManager;
    FOLDERVIEWMODE viewMode = FVM_AUTO;
    int iconSize;
    UINT count = 0;
    if (FAILED(view->QueryInterface(IID_PPV_ARGS(&folderView))) ||
        FAILED(view->QueryInterface(IID_PPV_ARGS(&columnManager))) ||
        FAILED(folderView->GetViewModeAndIconSize(&viewMode, &iconSize)) ||
        viewMode != FVM_DETAILS ||
        FAILED(columnManager->GetColumnCount(CM_ENUM_VISIBLE, &count)) ||
        !count)
        return;

    std::vector<PROPERTYKEY> keys(count);
    if (FAILED(columnManager->GetColumns(CM_ENUM_VISIBLE, keys.data(), count)))
        return;

    for (const auto& key : keys) {
        CM_COLUMNINFO ci = {sizeof(ci), CM_MASK_WIDTH};
        if (SUCCEEDED(columnManager->GetColumnInfo(key, &ci)))
            columnManager->SetColumnInfo(key, &ci);
    }
}

static void ApplyToAllOpenWindows(bool minWidthChanged) {
    HRESULT hrInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    {
        ComPtr<IShellWindows> shellWindows;
        long count = 0;
        if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr,
                                    CLSCTX_LOCAL_SERVER,
                                    IID_PPV_ARGS(&shellWindows))) ||
            FAILED(shellWindows->get_Count(&count))) {
            Wh_Log(L"Can't enumerate shell windows");
            count = 0;
        }

        DWORD processId = GetCurrentProcessId();

        for (long i = 0; i < count; i++) {
            VARIANT index;
            index.vt = VT_I4;
            index.lVal = i;

            ComPtr<IDispatch> dispatch;
            ComPtr<IWebBrowserApp> browser;
            if (FAILED(shellWindows->Item(index, &dispatch)) || !dispatch ||
                FAILED(dispatch.As(&browser)))
                continue;

            SHANDLE_PTR hwndValue = 0;
            browser->get_HWND(&hwndValue);
            DWORD windowProcessId = 0;
            GetWindowThreadProcessId((HWND)hwndValue, &windowProcessId);
            if (windowProcessId != processId)
                continue;

            ComPtr<IServiceProvider> services;
            ComPtr<IShellBrowser> shellBrowser;
            ComPtr<IShellView> shellView;
            if (FAILED(browser.As(&services)) ||
                FAILED(services->QueryService(SID_STopLevelBrowser,
                                              IID_PPV_ARGS(&shellBrowser))) ||
                FAILED(shellBrowser->QueryActiveShellView(&shellView)))
                continue;

            // shellView may be a proxy, don't retain it for the subclass. The items are already displayed
            if (ApplyForcedColumns(shellView.Get(), false, true))
                shellView->Refresh();
            if (minWidthChanged)
                RefreshMinimumWidths(shellView.Get());
        }
    }
    if (SUCCEEDED(hrInit))
        CoUninitialize();
}

// ---------------------------------------------------------------------------
// Hooks: CDefView (shell32.dll)
// ---------------------------------------------------------------------------

using CDefView_UIActivate_t = HRESULT(__thiscall*)(void*, UINT);
CDefView_UIActivate_t CDefView_UIActivate_orig;

HRESULT __thiscall CDefView_UIActivate_hook(void* pThis, UINT uState) {
    HRESULT hr = CDefView_UIActivate_orig(pThis, uState);
    if (SUCCEEDED(hr) &&
        (uState == SVUIA_ACTIVATE_FOCUS || uState == SVUIA_ACTIVATE_NOFOCUS))
        ApplyForcedColumns((IUnknown*)pThis, true, !g_deferAutoSize);
    return hr;
}

using CDefView_SetCurrentViewMode_t = HRESULT(__thiscall*)(void*, UINT);
CDefView_SetCurrentViewMode_t CDefView_SetCurrentViewMode_orig;

// Catches switching an already open folder to Details view
HRESULT __thiscall CDefView_SetCurrentViewMode_hook(void* pThis,
                                                    UINT viewMode) {
    HRESULT hr = CDefView_SetCurrentViewMode_orig(pThis, viewMode);
    if (SUCCEEDED(hr) && viewMode == FVM_DETAILS)
        ApplyForcedColumns((IUnknown*)pThis, true, !g_deferAutoSize);
    return hr;
}

using CDefView__OnEnumDoneMessage_t = void(__thiscall*)(void*);
CDefView__OnEnumDoneMessage_t CDefView__OnEnumDoneMessage_orig;

// The folder finished loading, e.g. after a refresh, auto-size again once the items are displayed
void __thiscall CDefView__OnEnumDoneMessage_hook(void* pThis) {
    CDefView__OnEnumDoneMessage_orig(pThis);
    ApplyForcedColumns((IUnknown*)pThis, true, !g_deferAutoSize);
}

using CDefView_PaintComplete_t = void(__thiscall*)(void*, BOOL);
CDefView_PaintComplete_t CDefView_PaintComplete_orig;

// Called after each paint of the items view. pThis is an interface of the view, not the view itself
void __thiscall CDefView_PaintComplete_hook(void* pThis,
                                            BOOL viewModeChanged) {
    CDefView_PaintComplete_orig(pThis, viewModeChanged);
    if (g_anyPendingAutoSize)
        AutoSizeIfPending((IUnknown*)pThis);
}

// ---------------------------------------------------------------------------
// Hooks: CViewSettings (windows.storage.dll)
// ---------------------------------------------------------------------------

using CViewSettings_GetMinimumColumnWidth_t = UINT(__thiscall*)(void*, UINT);
CViewSettings_GetMinimumColumnWidth_t CViewSettings_GetMinimumColumnWidth_orig;

// Every width, forced or dragged, is raised to this minimum, which is otherwise up to 10 average characters. In logical px at 96 DPI
UINT __thiscall CViewSettings_GetMinimumColumnWidth_hook(void* pThis,
                                                         UINT column) {
    UINT width = CViewSettings_GetMinimumColumnWidth_orig(pThis, column);
    int minWidth = g_minColumnWidth;
    return minWidth > 0 ? std::min(width, (UINT)minWidth) : width;
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    HMODULE shell32 = LoadLibraryW(L"shell32.dll");
    if (!shell32) {
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
        },
        {
            {
                L"public: virtual long __cdecl CDefView::SetCurrentViewMode(unsigned int)",
                L"long __cdecl CDefView::SetCurrentViewMode(unsigned int)",
                L"public: virtual long __thiscall CDefView::SetCurrentViewMode(unsigned int)",
                L"long __thiscall CDefView::SetCurrentViewMode(unsigned int)",
            },
            &CDefView_SetCurrentViewMode_orig,
            CDefView_SetCurrentViewMode_hook,
        },
        {
            {
                L"private: void __cdecl CDefView::_OnEnumDoneMessage(void)",
                L"void __cdecl CDefView::_OnEnumDoneMessage(void)",
                L"private: void __thiscall CDefView::_OnEnumDoneMessage(void)",
                L"void __thiscall CDefView::_OnEnumDoneMessage(void)",
            },
            &CDefView__OnEnumDoneMessage_orig,
            CDefView__OnEnumDoneMessage_hook,
            true,
        },
        {
            {
                L"public: virtual void __cdecl CDefView::PaintComplete(int)",
                L"void __cdecl CDefView::PaintComplete(int)",
                L"public: virtual void __thiscall CDefView::PaintComplete(int)",
                L"void __thiscall CDefView::PaintComplete(int)",
            },
            &CDefView_PaintComplete_orig,
            CDefView_PaintComplete_hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(shell32, shell32DllHooks,
                                    ARRAYSIZE(shell32DllHooks))) {
        Wh_Log(L"Failed to hook CDefView symbols");
        return FALSE;
    }

    // Without it, auto-sizing happens right away, possibly before the items are displayed
    g_autoSizeMessage = RegisterWindowMessageW(
        L"Windhawk_explorer-force-details-columns_AutoSize");
    g_deferAutoSize = CDefView_PaintComplete_orig && g_autoSizeMessage;
    if (!g_deferAutoSize)
        Wh_Log(L"CDefView::PaintComplete not found, auto-sizing right away");

    // windows.storage.dll
    const WindhawkUtils::SYMBOL_HOOK windowsStorageHooks[] = {
        {
            {
                L"private: unsigned int __cdecl CViewSettings::_GetMinimumColumnWidth(unsigned int)",
                L"unsigned int __cdecl CViewSettings::_GetMinimumColumnWidth(unsigned int)",
                L"private: unsigned int __thiscall CViewSettings::_GetMinimumColumnWidth(unsigned int)",
                L"unsigned int __thiscall CViewSettings::_GetMinimumColumnWidth(unsigned int)",
            },
            &CViewSettings_GetMinimumColumnWidth_orig,
            CViewSettings_GetMinimumColumnWidth_hook,
        },
    };

    // Not essential, columns just keep the Windows minimum width without it
    HMODULE windowsStorage = LoadLibraryExW(L"windows.storage.dll", nullptr,
                                            LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!windowsStorage ||
        !WindhawkUtils::HookSymbols(windowsStorage, windowsStorageHooks,
                                    ARRAYSIZE(windowsStorageHooks))) {
        Wh_Log(L"Failed to hook CViewSettings symbols, minimum column width "
               L"can't be lowered");
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    ApplyToAllOpenWindows(g_minColumnWidth > 0);
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    std::unordered_set<HWND> subclassed;
    {
        std::lock_guard<std::mutex> lock(g_subclassedMutex);
        subclassed.swap(g_subclassed);
    }
    for (HWND hwnd : subclassed)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd,
                                                         ViewSubclassProc);

    std::lock_guard<std::mutex> lock(g_pendingAutoSizeMutex);
    g_pendingAutoSize.clear();
    g_anyPendingAutoSize = false;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    int oldMinColumnWidth = g_minColumnWidth;
    LoadSettings();
    ApplyToAllOpenWindows(g_minColumnWidth != oldMinColumnWidth);
}
