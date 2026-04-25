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

Configure which columns to show and in what order in Settings. Each entry takes a Shell property name and a width **(if _Force Width_ enabled)** in logical pixels at 100% DPI scaling (for consistent widths in case of multiple monitors with different DPI settings).

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
- To allow a column to be freely resized, turn off "Force Width".
- Changes should take effect immediately to opened folders, but if the opened window is on **another monitor with a differnt DPI settings**, the width will be updated **after re-opening the folder once**.
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
      $description: Only takes effect if Force Width is turned on.
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
#include <cmath>
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
    int         width; // logical pixels at 100% DPI (96 dpi); 0 = free resize
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
            BOOL forceWidth = Wh_GetIntSetting(L"columns[%d].force_width", i);
            int width = Wh_GetIntSetting(L"columns[%d].width", i);
            
            if (!forceWidth) {
                width = 0; 
            }

            g_columns.push_back({ key, width });
            Wh_Log(L"Column[%d]: %s force=%d width=%d -> OK", i, rawProp, forceWidth, width);
        } else {
            Wh_Log(L"Column[%d]: %s -> not recognised, skipped", i, rawProp);
        }

        Wh_FreeStringSetting(rawProp);
    }

    if (g_columns.empty()) {
        Wh_Log(L"No valid columns configured, using defaults");
        g_columns.push_back({ PKEY_ItemNameDisplay, 270 });
        g_columns.push_back({ PKEY_Size,             30 });
        g_columns.push_back({ PKEY_DateModified,    110 });
        g_columns.push_back({ PKEY_ItemTypeText,     60 });
    }
}

// ---------------------------------------------------------------------------
// DPI helpers
// Try GetDpiForWindow
// ---------------------------------------------------------------------------

static UINT GetSystemDpi() {
    using GetDpiForSystem_t = UINT(WINAPI*)();
    static auto pGetDpiForSystem = reinterpret_cast<GetDpiForSystem_t>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForSystem"));
    if (pGetDpiForSystem)
        return pGetDpiForSystem();

    HDC hdc = GetDC(nullptr);
    UINT dpi = hdc ? static_cast<UINT>(GetDeviceCaps(hdc, LOGPIXELSX)) : 96;
    if (hdc) ReleaseDC(nullptr, hdc);
    return dpi;
}

static UINT GetWindowDpi(HWND hwnd) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
    static auto pGetDpiForWindow = reinterpret_cast<GetDpiForWindow_t>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow"));
    if (pGetDpiForWindow && hwnd)
        return pGetDpiForWindow(hwnd);
        
    return GetSystemDpi();
}

static UINT ScaleToDpi(int logicalPx, UINT dpi) {
    if (logicalPx <= 0) return 0;
    return static_cast<UINT>(std::round(logicalPx * (static_cast<double>(dpi) / 96.0)));
}

// ---------------------------------------------------------------------------
// Column enforcement
// ---------------------------------------------------------------------------

static void ApplyForcedColumns(void* pThis) {
    auto* pShellView = reinterpret_cast<IShellView*>(pThis);

    HWND hwnd = nullptr;
    pShellView->GetWindow(&hwnd);
    UINT windowDpi = GetWindowDpi(hwnd);

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

    // Check if the column settings need update
    UINT colCount = 0;
    bool orderNeedsUpdate = false;
    
    if (FAILED(pCM->GetColumnCount(CM_ENUM_VISIBLE, &colCount)) || colCount != g_columns.size()) {
        orderNeedsUpdate = true;
    } else {
        std::vector<PROPERTYKEY> currentKeys(colCount);
        if (SUCCEEDED(pCM->GetColumns(CM_ENUM_VISIBLE, currentKeys.data(), colCount))) {
            for (size_t i = 0; i < colCount; i++) {
                if (currentKeys[i].fmtid != g_columns[i].key.fmtid ||
                    currentKeys[i].pid != g_columns[i].key.pid) {
                    orderNeedsUpdate = true;
                    break;
                }
            }
        } else {
            orderNeedsUpdate = true;
        }
    }

    if (orderNeedsUpdate) {
        std::vector<PROPERTYKEY> keys;
        keys.reserve(g_columns.size());
        for (const auto& col : g_columns) {
            keys.push_back(col.key);
        }
        pCM->SetColumns(keys.data(), static_cast<UINT>(keys.size()));
    }

    // Column width enforcement for those using a width value
    for (const auto& col : g_columns) {
        if (col.width <= 0)
            continue; 

        CM_COLUMNINFO ci = {};
        ci.cbSize = sizeof(ci);
        ci.dwMask = CM_MASK_WIDTH;
        if (SUCCEEDED(pCM->GetColumnInfo(col.key, &ci))) {
            UINT expectedWidth = ScaleToDpi(col.width, windowDpi);
            if (ci.uWidth != expectedWidth) {
                ci.uWidth = expectedWidth;
                pCM->SetColumnInfo(col.key, &ci);
            }
        }
    }

    pCM->Release();
}

// ---------------------------------------------------------------------------
// Apply to all currently open Explorer windows
// ---------------------------------------------------------------------------

static void ApplyToAllOpenWindows() {
    IShellWindows* psw = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER,
                                IID_IShellWindows, reinterpret_cast<void**>(&psw))) || !psw)
        return;

    long count = 0;
    psw->get_Count(&count);

    DWORD currentProcessId = GetCurrentProcessId();

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
            
            HWND wnd = nullptr;
            pWBA->get_HWND(reinterpret_cast<SHANDLE_PTR*>(&wnd));
            
            DWORD windowProcessId = 0;
            if (wnd) {
                GetWindowThreadProcessId(wnd, &windowProcessId);
            }

            if (windowProcessId == currentProcessId) {
                IServiceProvider* pSP = nullptr;
                if (SUCCEEDED(pWBA->QueryInterface(IID_IServiceProvider_,
                                                   reinterpret_cast<void**>(&pSP))) && pSP) {
                    IShellBrowser* pSB = nullptr;
                    if (SUCCEEDED(pSP->QueryService(SID_STopLevelBrowser, IID_IShellBrowser,
                                                    reinterpret_cast<void**>(&pSB))) && pSB) {
                        IShellView* pSV = nullptr;
                        if (SUCCEEDED(pSB->QueryActiveShellView(&pSV)) && pSV) {
                            ApplyForcedColumns(pSV);
                            // Refresh directly here
                            pSV->Refresh();
                            pSV->Release();
                        }
                        pSB->Release();
                    }
                    pSP->Release();
                }
            }
            pWBA->Release();
        }
        pDisp->Release();
    }
    psw->Release();
}

// ---------------------------------------------------------------------------
// Hook: CDefView::UIActivate  (shell32.dll)
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