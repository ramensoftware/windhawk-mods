// ==WindhawkMod==
// @id              explorer-force-details-columns
// @name            Explorer Details View Columns
// @description     Forces a fixed set of columns, and optionally sorting, in File Explorer Details view. Columns and their order are configurable. Has no effect on other view modes.
// @version         1.3
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
- Any property name not recognised by Windows will be skipped.
- Duplicate property entries will only take the first occurrence.
- To allow a column to be freely resized, turn off "Force Width". Set Width to -1 to auto-size the column to its content.
- If "Sort By" is set, manual sorting changes stay temporary, like manual width changes of forced-width columns.
- By default, virtual folders that have their own specialized columns (such as the Recycle Bin with _Original Location_ and _Date Deleted_) are not affected. Turn off "Exclude virtual folders" to force the columns there as well.
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
      $description: Width in logical pixels at 100% DPI scaling, or -1 to auto-size to content. Only takes effect if Force Width is turned on.
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
#include <shlobj.h>
#include <propkey.h>
#include <propsys.h>
#include <exdisp.h>
#include <shlguid.h>
#include <servprov.h>
#include <commctrl.h>
#include <wrl/client.h>
#include <algorithm>
#include <cstdlib>
#include <mutex>
#include <unordered_set>
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

static std::mutex               g_subclassedMutex;
static std::unordered_set<HWND> g_subclassed;

static void LoadSettings() {
    g_columns.clear();

    g_excludeVirtualFolders = Wh_GetIntSetting(L"exclude_virtual");
    g_sortDescending = Wh_GetIntSetting(L"sort_descending");

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

static bool ApplyForcedColumns(IUnknown* view, bool subclassForDpi);

// Re-applies widths when the view moves to a monitor with a different DPI
static LRESULT CALLBACK ViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                         LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_DPICHANGED_AFTERPARENT) {
        ApplyForcedColumns((IUnknown*)dwRefData, false);
    } else if (uMsg == WM_NCDESTROY) {
        std::lock_guard<std::mutex> lock(g_subclassedMutex);
        g_subclassed.erase(hWnd);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Virtual folders (Recycle Bin, This PC, search results, etc.) have no file system path + with specialized columns
static bool IsVirtualFolder(IFolderView2* folderView) {
    ComPtr<IPersistFolder2> persist;
    if (FAILED(folderView->GetFolder(IID_PPV_ARGS(&persist))))
        return false;

    PIDLIST_ABSOLUTE pidl;
    if (FAILED(persist->GetCurFolder(&pidl)))
        return false;

    WCHAR path[1024];
    bool isVirtual =
        !SHGetPathFromIDListEx(pidl, path, ARRAYSIZE(path), GPFIDL_DEFAULT);
    CoTaskMemFree(pidl);
    return isVirtual;
}

// Returns true if anything actually changed. subclassForDpi is set only when view can be retained
static bool ApplyForcedColumns(IUnknown* view, bool subclassForDpi) {
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

    if (g_excludeVirtualFolders && IsVirtualFolder(folderView.Get()))
        return false;

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
        std::vector<PROPERTYKEY> keys;
        for (const auto& col : g_columns)
            keys.push_back(col.key);
        changed |= SUCCEEDED(
            columnManager->SetColumns(keys.data(), (UINT)keys.size()));
    }

    // Widths
    HWND hwnd = nullptr;
    shellView->GetWindow(&hwnd);
    UINT dpi = hwnd ? GetDpiForWindow(hwnd) : GetDpiForSystem();

    for (const auto& col : g_columns) {
        if (!col.width)
            continue;

        CM_COLUMNINFO ci = {sizeof(ci), CM_MASK_WIDTH};
        if (FAILED(columnManager->GetColumnInfo(col.key, &ci)))
            continue;

        UINT width =
            col.width < 0 ? CM_WIDTH_AUTOSIZE : MulDiv(col.width, dpi, 96);
        // ±1px tolerance avoids update loops from DPI rounding
        if (col.width < 0 || std::abs((int)ci.uWidth - (int)width) > 1) {
            ci.uWidth = width;
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

    if (subclassForDpi && hwnd) {
        std::lock_guard<std::mutex> lock(g_subclassedMutex);
        if (g_subclassed.insert(hwnd).second)
            WindhawkUtils::SetWindowSubclassFromAnyThread(
                hwnd, ViewSubclassProc, (DWORD_PTR)view);
    }

    return changed;
}

// ---------------------------------------------------------------------------
// Apply to all currently open Explorer windows
// ---------------------------------------------------------------------------

static void ApplyToAllOpenWindows() {
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
            if (SUCCEEDED(browser.As(&services)) &&
                SUCCEEDED(services->QueryService(SID_STopLevelBrowser,
                                                 IID_PPV_ARGS(&shellBrowser))) &&
                SUCCEEDED(shellBrowser->QueryActiveShellView(&shellView)) &&
                // shellView may be a proxy, don't retain it for the subclass
                ApplyForcedColumns(shellView.Get(), false)) {
                shellView->Refresh();
            }
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
        ApplyForcedColumns((IUnknown*)pThis, true);
    return hr;
}

using CDefView_SetCurrentViewMode_t = HRESULT(__thiscall*)(void*, UINT);
CDefView_SetCurrentViewMode_t CDefView_SetCurrentViewMode_orig;

// Catches switching an already open folder to Details view
HRESULT __thiscall CDefView_SetCurrentViewMode_hook(void* pThis,
                                                    UINT viewMode) {
    HRESULT hr = CDefView_SetCurrentViewMode_orig(pThis, viewMode);
    if (SUCCEEDED(hr) && viewMode == FVM_DETAILS)
        ApplyForcedColumns((IUnknown*)pThis, true);
    return hr;
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
    };

    if (!WindhawkUtils::HookSymbols(shell32, shell32DllHooks,
                                    ARRAYSIZE(shell32DllHooks))) {
        Wh_Log(L"Failed to hook CDefView symbols");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    ApplyToAllOpenWindows();
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
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
    ApplyToAllOpenWindows();
}
