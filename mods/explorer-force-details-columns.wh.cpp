// ==WindhawkMod==
// @id              explorer-force-details-columns
// @name            Explorer Details View Columns
// @description     Forces a fixed set of columns in File Explorer Details view. Columns and their order are configurable. Has no effect on other view modes.
// @version         1.1
// @author          ernisn
// @github          https://github.com/ernisn
// @include         explorer.exe
// @compilerOptions -lole32 -lshlwapi -lpropsys -lgdi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Details View Columns

Forces a specific set of columns whenever a File Explorer folder is in _Details_ view. **Other view modes (_Icons_, _Tiles_, _List_, etc.) are not affected.**

Configure which columns to show and in what order in Settings. Each entry takes a Shell property name and a width in **logical pixels at 100% DPI scaling** (for consistent widths in case of changing between multiple monitors with different DPI settings).

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
- There is a minimum value for column widths; lower values will be raised automatically.
- You can still change the columns manually but it will be temporary.
- Changes should take effect immediately, otherwise re-opening a window will work.
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
  $description: "Columns to show in order in Details view. Width is in logical pixels at 100% DPI scaling. Leave 0 for default width."
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <shobjidl.h>
#include <propkey.h>
#include <propsys.h>
#include <shellapi.h>
#include <exdisp.h>
#include <shlguid.h>
#include <servprov.h>
#include <vector>
#include <windhawk_utils.h>

// IID_IServiceProvider from Windows SDK {6D5140C1-7436-11CE-8034-00AA006009FA}
DEFINE_GUID(IID_IServiceProvider_,
    0x6D5140C1, 0x7436, 0x11CE,
    0x80, 0x34, 0x00, 0xAA, 0x00, 0x60, 0x09, 0xFA);

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

struct ColumnEntry {
    PROPERTYKEY key;
    int         width; // logical pixels at 100% DPI (96 dpi); 0 = keep default
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

    if (g_columns.empty()) {
        Wh_Log(L"No valid columns configured, using defaults");
        g_columns.push_back({ PKEY_ItemNameDisplay, 300 });
        g_columns.push_back({ PKEY_Size,             80 });
        g_columns.push_back({ PKEY_DateModified,    105 });
        g_columns.push_back({ PKEY_ItemTypeText,    130 });
    }
}

// ---------------------------------------------------------------------------
// DPI helpers
// ---------------------------------------------------------------------------

// Returns the DPI of the primary monitor, or 96 if unavailable.
static UINT GetSystemDpi() {
    // GetDpiForSystem is available on Win10+. Use it if present.
    using GetDpiForSystem_t = UINT(WINAPI*)();
    static auto pGetDpiForSystem = reinterpret_cast<GetDpiForSystem_t>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForSystem"));
    if (pGetDpiForSystem)
        return pGetDpiForSystem();

    // Fallback: read from desktop DC.
    HDC hdc = GetDC(nullptr);
    UINT dpi = hdc ? static_cast<UINT>(GetDeviceCaps(hdc, LOGPIXELSX)) : 96;
    if (hdc) ReleaseDC(nullptr, hdc);
    return dpi;
}

// Scale a logical-pixel value (at 96 dpi baseline) to physical pixels.
static UINT ScaleToDpi(int logicalPx) {
    if (logicalPx <= 0) return 0;
    UINT dpi = GetSystemDpi();
    return static_cast<UINT>(MulDiv(logicalPx, static_cast<int>(dpi), 96));
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

    std::vector<PROPERTYKEY> keys;
    keys.reserve(g_columns.size());
    for (const auto& col : g_columns)
        keys.push_back(col.key);

    pCM->SetColumns(keys.data(), static_cast<UINT>(keys.size()));

    for (const auto& col : g_columns) {
        if (col.width <= 0)
            continue;

        CM_COLUMNINFO ci = {};
        ci.cbSize = sizeof(ci);
        ci.dwMask = CM_MASK_WIDTH;
        ci.uWidth = ScaleToDpi(col.width);
        pCM->SetColumnInfo(col.key, &ci);
    }

    pCM->Release();
}

// ---------------------------------------------------------------------------
// Apply to all currently open Explorer windows
// Called on mod init/reload so existing windows are also covered
// ---------------------------------------------------------------------------

static void ApplyToAllOpenWindows() {
    // Enumerate all shell windows via IShellWindows.
    IShellWindows* psw = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER,
                                IID_IShellWindows, reinterpret_cast<void**>(&psw))) || !psw)
        return;

    long count = 0;
    psw->get_Count(&count);

    for (long i = 0; i < count; i++) {
        VARIANT vi;
        vi.vt = VT_I4;
        vi.lVal = i;

        IDispatch* pDisp = nullptr;
        if (FAILED(psw->Item(vi, &pDisp)) || !pDisp)
            continue;

        IWebBrowserApp* pWBA = nullptr;
        if (SUCCEEDED(pDisp->QueryInterface(IID_IWebBrowserApp,
                                            reinterpret_cast<void**>(&pWBA))) && pWBA) {
            IServiceProvider* pSP = nullptr;
            if (SUCCEEDED(pWBA->QueryInterface(IID_IServiceProvider_,
                                               reinterpret_cast<void**>(&pSP))) && pSP) {
                IShellBrowser* pSB = nullptr;
                if (SUCCEEDED(pSP->QueryService(SID_STopLevelBrowser, IID_IShellBrowser,
                                                reinterpret_cast<void**>(&pSB))) && pSB) {
                    IShellView* pSV = nullptr;
                    if (SUCCEEDED(pSB->QueryActiveShellView(&pSV)) && pSV) {
                        ApplyForcedColumns(pSV);
                        pSV->Release();
                    }
                    pSB->Release();
                }
                pSP->Release();
            }
            pWBA->Release();
        }
        pDisp->Release();
    }
    psw->Release();
}

// ---------------------------------------------------------------------------
// Hook: CDefView::UIActivate  (shell32.dll)
// Triggered whenever a folder view is activated
// ---------------------------------------------------------------------------

using CDefView_UIActivate_t = HRESULT(__thiscall*)(void* pThis, UINT uState);

CDefView_UIActivate_t CDefView_UIActivate_orig = nullptr;

HRESULT __thiscall CDefView_UIActivate_hook(void* pThis, UINT uState) {
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
        Wh_Log(L"ERROR: Could not resolve CDefView::UIActivate");
        return FALSE;
    }

    Wh_Log(L"CDefView::UIActivate hooked successfully");
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"AfterInit - applying to existing windows");
    ApplyToAllOpenWindows();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
    ApplyToAllOpenWindows();
}
