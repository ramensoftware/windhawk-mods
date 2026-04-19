// ==WindhawkMod==
// @id              explorer-force-details-columns
// @name            Explorer Details View Columns
// @description     Forces a fixed set of columns in File Explorer Details view. Columns and their order are configurable. Has no effect on other view modes.
// @version         1.0
// @author          ernisn
// @github          https://github.com/ernisn
// @include         explorer.exe
// @compilerOptions -lole32 -lshlwapi -lpropsys
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Details View Columns

Forces a specific set of columns whenever a File Explorer folder is in _Details_
view. **Other view modes (_Icons_, _Tiles_, _List_, etc.) are not affected.**

Configure which columns to show and in what order in Settings. Each entry takes a Shell property name and a width in pixels.

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

For a full list of available Shell property names, see:
https://learn.microsoft.com/en-us/windows/win32/properties/props

**Note:** 
- Any property name not recognised by Windows will be skipped.
- There is a minimum value for Column widths (lower value will be ignored).
- You can still change the columns manually but it will be temporary.
- Changes might take effect after re-opening a folder.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- columns:
  - - property: System.ItemNameDisplay
    - width: 300
  - - property: System.Size
    - width: 80
  - - property: System.DateModified
    - width: 105
  - - property: System.ItemTypeText
    - width: 130
  $name: Columns
  $description: "Columns to show in order in Details view. Leave blank or 0 for default width."
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <shobjidl.h>
#include <propkey.h>
#include <propsys.h>
#include <vector>
#include <windhawk_utils.h>

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

struct ColumnEntry {
    PROPERTYKEY key;
    int         width; // pixels; 0 = do not set (keep Explorer default)
};

static std::vector<ColumnEntry> g_columns;

static void LoadSettings() {
    g_columns.clear();

    for (int i = 0; ; i++) {
        PCWSTR rawProp = Wh_GetStringSetting(L"columns[%d].property", i);
        if (!rawProp || rawProp[0] == L'\0') {
            Wh_FreeStringSetting(rawProp);
            break;
        }

        PROPERTYKEY key;
        if (SUCCEEDED(PSGetPropertyKeyFromName(rawProp, &key))) {
            int width = Wh_GetIntSetting(L"columns[%d].width", i);
            g_columns.push_back({ key, width });
            Wh_Log(L"Column[%d]: %s width=%d -> OK", i, rawProp, width);
        } else {
            Wh_Log(L"Column[%d]: %s -> not recognised, skipped", i, rawProp);
        }

        Wh_FreeStringSetting(rawProp);
    }

    // Fallback defaults (pixels)
    if (g_columns.empty()) {
        Wh_Log(L"No valid columns configured, using defaults");
        g_columns.push_back({ PKEY_ItemNameDisplay, 300 });
        g_columns.push_back({ PKEY_Size,             80 });
        g_columns.push_back({ PKEY_DateModified,    105 });
        g_columns.push_back({ PKEY_ItemTypeText,    130 });
    }
}

// ---------------------------------------------------------------------------
// Column enforcement
// ---------------------------------------------------------------------------

static void ApplyForcedColumns(void* pThis) {
    auto* pShellView = reinterpret_cast<IShellView*>(pThis);

    IFolderView2* pFV2 = nullptr;
    if (FAILED(pShellView->QueryInterface(IID_IFolderView2,
                                          reinterpret_cast<void**>(&pFV2))) || !pFV2)
        return;

    FOLDERVIEWMODE viewMode = FVM_AUTO;
    int iconSize = 0;
    HRESULT hr = pFV2->GetViewModeAndIconSize(&viewMode, &iconSize);
    pFV2->Release();

    if (FAILED(hr) || viewMode != FVM_DETAILS)
        return;

    IColumnManager* pCM = nullptr;
    if (FAILED(pShellView->QueryInterface(IID_IColumnManager,
                                          reinterpret_cast<void**>(&pCM))) || !pCM)
        return;

    // Build key array for SetColumns.
    std::vector<PROPERTYKEY> keys;
    keys.reserve(g_columns.size());
    for (const auto& col : g_columns)
        keys.push_back(col.key);

    // SetColumns clears all column state.
    pCM->SetColumns(keys.data(), static_cast<UINT>(keys.size()));

    // Set widths in pixels. CM_MASK_WIDTH targets the current display width.
    for (const auto& col : g_columns) {
        if (col.width <= 0)
            continue;

        CM_COLUMNINFO ci = {};
        ci.cbSize  = sizeof(ci);
        ci.dwMask  = CM_MASK_WIDTH;
        ci.uWidth  = static_cast<UINT>(col.width);
        pCM->SetColumnInfo(col.key, &ci);
    }

    pCM->Release();
}

// ---------------------------------------------------------------------------
// Hook: CDefView::UIActivate  (shell32.dll)
//
// COM virtual method - on x64 the decorated symbol is:
//   public: virtual long __cdecl CDefView::UIActivate(unsigned int)
// On x86:
//   public: virtual long __thiscall CDefView::UIActivate(unsigned int)
// ---------------------------------------------------------------------------

#ifdef _WIN64
using CDefView_UIActivate_t = HRESULT(__cdecl*)(void* pThis, UINT uState);
#else
using CDefView_UIActivate_t = HRESULT(__thiscall*)(void* pThis, UINT uState);
#endif

CDefView_UIActivate_t CDefView_UIActivate_orig = nullptr;

#ifdef _WIN64
HRESULT __cdecl CDefView_UIActivate_hook(void* pThis, UINT uState)
#else
HRESULT __thiscall CDefView_UIActivate_hook(void* pThis, UINT uState)
#endif
{
    HRESULT hr = CDefView_UIActivate_orig(pThis, uState);

    if (SUCCEEDED(hr) &&
        (uState == SVUIA_ACTIVATE_FOCUS || uState == SVUIA_ACTIVATE_NOFOCUS)) {
        ApplyForcedColumns(pThis);
    }
    return hr;
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32) {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    // Try decorated forms of CDefView::UIActivate.
    const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
        // x64 - public virtual, __cdecl (most common on Win11)
        {
            { L"public: virtual long __cdecl CDefView::UIActivate(unsigned int)" },
            reinterpret_cast<void**>(&CDefView_UIActivate_orig),
            reinterpret_cast<void*>(CDefView_UIActivate_hook),
            true
        },
        // x64 - without explicit public/virtual qualifier in PDB
        {
            { L"long __cdecl CDefView::UIActivate(unsigned int)" },
            reinterpret_cast<void**>(&CDefView_UIActivate_orig),
            reinterpret_cast<void*>(CDefView_UIActivate_hook),
            true
        },
        // x86 - __thiscall
        {
            { L"public: virtual long __thiscall CDefView::UIActivate(unsigned int)" },
            reinterpret_cast<void**>(&CDefView_UIActivate_orig),
            reinterpret_cast<void*>(CDefView_UIActivate_hook),
            true
        },
        {
            { L"long __thiscall CDefView::UIActivate(unsigned int)" },
            reinterpret_cast<void**>(&CDefView_UIActivate_orig),
            reinterpret_cast<void*>(CDefView_UIActivate_hook),
            true
        },
    };

    WindhawkUtils::HookSymbols(hShell32, shell32DllHooks, ARRAYSIZE(shell32DllHooks));

    if (!CDefView_UIActivate_orig) {
        Wh_Log(L"ERROR: Could not resolve CDefView::UIActivate - mod will have no effect");
        return FALSE;
    }

    Wh_Log(L"CDefView::UIActivate hooked successfully");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}