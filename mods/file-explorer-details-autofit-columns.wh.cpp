// ==WindhawkMod==
// @id              file-explorer-details-autofit-columns
// @name            File Explorer Details Auto-Fit Columns
// @description     Automatically fits all column widths to their content when refreshing in Details view. Has no effect on other view modes.
// @version         1.1.0
// @author          Armaninyow
// @github          https://github.com/armaninyow
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lpropsys -lshell32 -lcomctl32 -lgdi32
// @license         MIT
// ==/WindhawkMod==

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
- Any actual change to the folder's contents (files added/removed/renamed), regardless of what caused it

Only affects `Details view`. Other view modes (Icons, Tiles, List, etc.) are untouched.

## Settings

`Refresh Delay (ms)`: How long the mod waits before auto-fitting columns. The default is 400ms, which gives Explorer enough time to finish loading files before measuring content width. If you notice columns fitting too early in large folders, increase this value. If you want a snappier response in small folders, decrease it.

`Fit Mode`: Explorer's built-in auto-fit only measures rows currently rendered on screen, so a long filename further down an unscrolled list can end up truncated even after fitting.

- `Visible Rows Only` keeps that default, fast behavior.
- `Scan Entire Folder` measures every item in the folder instead, so nothing is left truncated. (Known limitation: scanning the folder adds its own delay on top of the Refresh Delay setting, so fitting can take noticeably longer in large folders.)
- `Elastic (macOS-like)` sizes every other column to fit its content, then lets the Name column fill whatever width is left over in the current window. No leftover empty space, and no horizontal scrollbar unless the window gets too narrow even for a small minimum Name width. Reacts to resizing the Explorer window, settling into place shortly after you finish dragging. (Known limitation: on top of the scanning delay and the Refresh Delay, this mode also makes a few extra attempts to find the exact width that avoids a horizontal scrollbar, adding a bit more delay still.)

`Max Items to Scan`: Only used in `Scan Entire Folder` and `Elastic` modes. Folders larger than this item count fall back to `Visible Rows Only` for that folder, to avoid a noticeable delay in very large folders.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- delay: 400
  $name: Refresh Delay (ms)
  $description: "How long to wait after a refresh before fitting columns to content. Increase if columns fit before all files are loaded (e.g. large folders). Decrease for a snappier response. Default: 400ms."
- fitModeSettings:
  - fitMode: visible
    $name: Fit Mode
    $description: >-
      Visible rows only - is fast and matches Explorer's default auto-fit behavior, but can leave long filenames further down the list truncated until you scroll to them.



      Scan entire folder - measures every item in the folder so nothing is left truncated. (Known limitation: scanning the folder adds its own delay on top of the Refresh Delay setting above, so fitting can take noticeably longer in large folders.)



      Elastic (macOS-like) - sizes every other column to fit its content, then lets the Name column fill whatever width is left over in the window, avoiding leftover empty space and (down to a small minimum) horizontal scrolling. (Known limitation: on top of the scanning delay and the Refresh Delay, this mode also makes a few extra attempts to find the exact width that avoids a horizontal scrollbar, adding a bit more delay still.)
    $options:
    - visible: Visible rows only (fast, default)
    - full: Scan entire folder (exact fit)
    - elastic: Elastic (macOS-like, Name column fills remaining space)
  - maxScanItems: 500
    $name: Max Items to Scan
    $description: "If a folder has more items than this, fall back to Visible Rows Only for that folder to avoid a delay. Applies when Fit Mode is set to Scan Entire Folder or Elastic."
  $name: Fit Mode Settings
  $description: Controls how columns are measured and sized.
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <shobjidl.h>
#include <propsys.h>
#include <propkey.h>
#include <shlobj.h>
#include <shlguid.h>
#include <servprov.h>
#include <commctrl.h>
#include <uiautomation.h>
#include <oleauto.h>
#include <algorithm>
#include <atomic>
#include <memory>
#include <vector>
#include <unordered_map>
#include <optional>
#include <windhawk_utils.h>

#define EXPLORER_REFRESH_CMD_1 0xA220  // Ctrl+R / F5
#define EXPLORER_REFRESH_CMD_2 0x7103  // Context menu Refresh (legacy menu)
#define AUTOFIT_TIMER_ID       0xAF17  // "Something changed, start a fit after the configured delay"
#define FITSTEP_TIMER_ID       0xAF18  // "Advance one step of an in-progress elastic verify loop"

// Window classes that receive refresh commands.
static const PCWSTR kSubclassTargets[] = {
    L"ShellTabWindowClass",
    L"ReBarWindow32",
    L"ToolbarWindow32",
};

// Delay between the calibration autosize step and reading its result back.
static constexpr UINT kFitStepDelayMs = 40;

// Elastic mode's verify/correct loop needs settle time for slower-relayout views.
static constexpr UINT kElasticVerifyDelayMs = 90;

// Wall-clock budget for the folder scan, checked per-column (not just per-item).
// Kept at 2000ms: legitimately slow metadata reads (e.g. audio tags) can still
// exceed a small budget on the very first item, losing Full/Elastic entirely.
static constexpr ULONGLONG kScanTimeBudgetMs = 2000;

// g_cs guards all global map access; g_settingsCs guards the cached-settings snapshot.
static CRITICAL_SECTION g_cs;
static CRITICAL_SECTION g_settingsCs;

// Set at the top of Wh_ModUninit so in-flight window messages stop arming new timers mid-teardown.
static std::atomic<bool> g_unloading{false};

enum class FitMode { Visible, Full, Elastic };

struct Settings {
    UINT delayMs = 400;
    int maxScanItems = 500;
    FitMode fitMode = FitMode::Visible;
};
static Settings g_settings;

static UINT g_shellNotifyMsg = 0;  // registered in Wh_ModInit; replaces a WM_APP-relative id
static UINT g_cleanupMsg = 0;      // registered in Wh_ModInit; marshals teardown onto the owning thread

struct FitContext;

static LRESULT CALLBACK ExplorerSubclassProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData);

// Maps: tab -> IShellView, subclassed window -> tab, tab -> notify reg, tab -> in-progress fit.
static std::unordered_map<HWND, IShellView*> g_tabShellViews;
static std::unordered_map<HWND, HWND>        g_windowToTab;
static std::unordered_map<HWND, ULONG>       g_tabNotifyReg;

// Holds thread-affine COM objects; must not auto-destroy at process shutdown (Explorer can terminate without calling Wh_ModUninit).
[[clang::no_destroy]] std::optional<std::unordered_map<HWND, std::unique_ptr<FitContext>>>
    g_fitContexts{std::in_place};

// Elastic mode's resize-only fast path: the last full fit's column widths, keyed by tab.
// Stored as a full snapshot (not "everything except index 0") since Elastic's Name
// column is identified by property key, not position -- it isn't always index 0.
struct TabWidthCache {
    UINT colCount = 0;
    std::vector<PROPERTYKEY> keys;
    std::vector<int> allWidths;
};
static std::unordered_map<HWND, TabWidthCache> g_tabWidthCache;

// Per-tab current folder PIDL, so UIActivate can skip re-registering and
// re-fitting when it fires for focus/tab-switch reasons, not navigation.
static std::unordered_map<HWND, PIDLIST_ABSOLUTE> g_tabFolderPidl;

static bool SameColumnKeys(const std::vector<PROPERTYKEY>& a, const std::vector<PROPERTYKEY>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); i++) {
        if (!IsEqualPropertyKey(a[i], b[i]))
            return false;
    }
    return true;
}

// Elastic mode stretches whichever column is actually the Name column, not
// whichever happens to be first -- a user can reorder columns.
static constexpr UINT kNoNameColumn = static_cast<UINT>(-1);

static UINT FindNameColumnIndex(const std::vector<PROPERTYKEY>& keys) {
    for (UINT i = 0; i < keys.size(); i++) {
        if (IsEqualPropertyKey(keys[i], PKEY_ItemNameDisplay))
            return i;
    }
    return kNoNameColumn;  // Name isn't in the visible column set (shouldn't normally happen)
}

// A resize-only trigger is downgraded back to a full trigger if a real
// content/refresh event also arrives before the delay timer fires.
enum class TriggerKind { ResizeOnly, Full };
static std::unordered_map<HWND, TriggerKind> g_pendingTriggerKind;

static void MarkFitTrigger(HWND hwndTimer, bool resizeOnly) {
    TriggerKind newKind = resizeOnly ? TriggerKind::ResizeOnly : TriggerKind::Full;
    EnterCriticalSection(&g_cs);
    auto [it, inserted] = g_pendingTriggerKind.try_emplace(hwndTimer, newKind);
    if (!inserted && newKind == TriggerKind::Full)
        it->second = newKind;
    LeaveCriticalSection(&g_cs);
}

static bool ConsumeResizeOnlyFlag(HWND hwndTimer) {
    bool resizeOnly = false;
    EnterCriticalSection(&g_cs);
    auto it = g_pendingTriggerKind.find(hwndTimer);
    if (it != g_pendingTriggerKind.end()) {
        resizeOnly = (it->second == TriggerKind::ResizeOnly);
        g_pendingTriggerKind.erase(it);
    }
    LeaveCriticalSection(&g_cs);
    return resizeOnly;
}

// Cached settings

static void LoadSettings() {
    int delayRaw = Wh_GetIntSetting(L"delay");
    UINT delay = static_cast<UINT>(std::clamp(delayRaw, 0, 10000));

    int maxScan = Wh_GetIntSetting(L"fitModeSettings.maxScanItems");
    if (maxScan <= 0) maxScan = 500;
    // No hard ceiling here: the scan is now bounded by wall-clock time
    // (kScanTimeBudgetMs) regardless of how high this is set.

    FitMode mode = FitMode::Visible;
    auto fitModeStr = WindhawkUtils::StringSetting::make(L"fitModeSettings.fitMode");
    if (wcscmp(fitModeStr.get(), L"full") == 0) mode = FitMode::Full;
    else if (wcscmp(fitModeStr.get(), L"elastic") == 0) mode = FitMode::Elastic;

    EnterCriticalSection(&g_settingsCs);
    g_settings.delayMs = delay;
    g_settings.maxScanItems = maxScan;
    g_settings.fitMode = mode;
    LeaveCriticalSection(&g_settingsCs);
}

static Settings GetCachedSettings() {
    EnterCriticalSection(&g_settingsCs);
    Settings s = g_settings;
    LeaveCriticalSection(&g_settingsCs);
    return s;
}

// Small helpers

static int MeasureTextWidth(HDC hdc, PCWSTR text) {
    if (!text || !*text) return 0;
    SIZE sz = {0, 0};
    GetTextExtentPoint32W(hdc, text, static_cast<int>(wcslen(text)), &sz);
    return sz.cx;
}

// Recursively finds the first descendant window matching the given class name.
struct FindClassData { PCWSTR className; HWND result; };

static BOOL CALLBACK FindClassEnumProc(HWND hwnd, LPARAM lp) {
    auto* data = reinterpret_cast<FindClassData*>(lp);
    WCHAR cls[64] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, data->className) == 0) {
        data->result = hwnd;
        return FALSE;
    }
    return TRUE;
}

static HWND FindDescendantByClass(HWND hwndRoot, PCWSTR className) {
    FindClassData data = { className, nullptr };
    EnumChildWindows(hwndRoot, FindClassEnumProc, reinterpret_cast<LPARAM>(&data));
    return data.result;
}

// Builds a font matching hwndSource's own font, falling back to a plain system guess.
// Only used to rank items against each other for the widest-item scan; the final pixel
// width always comes from the native autosize, so this doesn't need to be exact.
static HFONT CreateFontFromWindowOrDefault(HWND hwndSource, UINT dpi) {
    if (hwndSource) {
        HFONT hSrcFont = reinterpret_cast<HFONT>(SendMessageW(hwndSource, WM_GETFONT, 0, 0));
        if (hSrcFont) {
            LOGFONTW lfCopy = {};
            if (GetObjectW(hSrcFont, sizeof(lfCopy), &lfCopy))
                return CreateFontIndirectW(&lfCopy);
        }
    }

    LOGFONTW lf = {};
    BOOL gotFont = SystemParametersInfoForDpi(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0, dpi);
    if (!gotFont) SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0);
    return CreateFontIndirectW(&lf);
}

// UI Automation, used narrowly for Elastic mode's horizontal-scroll check, cached per thread.
static thread_local IUIAutomation* tls_pAutomation = nullptr;

static IUIAutomation* GetThreadAutomation() {
    if (!tls_pAutomation) {
        CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                          IID_PPV_ARGS(&tls_pAutomation));
    }
    return tls_pAutomation;
}

static void ReleaseThreadAutomation() {
    if (tls_pAutomation) {
        tls_pAutomation->Release();
        tls_pAutomation = nullptr;
    }
}

// Returns true if pElement currently reports it can scroll horizontally.
static bool ElementHasHorizontalScroll(IUIAutomationElement* pElement) {
    if (!pElement) return false;

    bool result = false;
    IUIAutomationScrollPattern* pScroll = nullptr;
    HRESULT hrPat = pElement->GetCurrentPatternAs(UIA_ScrollPatternId, IID_PPV_ARGS(&pScroll));
    if (SUCCEEDED(hrPat) && pScroll) {
        BOOL scrollable = FALSE;
        if (SUCCEEDED(pScroll->get_CurrentHorizontallyScrollable(&scrollable)))
            result = (scrollable != FALSE);
        pScroll->Release();
    }
    return result;
}

// Fallback: locates the list/data-grid element via a UI Automation subtree search, once per fit.
static IUIAutomationElement* FindListElementViaUIA(HWND hwndView) {
    if (!hwndView) return nullptr;

    IUIAutomation* pAuto = GetThreadAutomation();
    if (!pAuto) return nullptr;

    IUIAutomationElement* pViewElement = nullptr;
    if (FAILED(pAuto->ElementFromHandle(hwndView, &pViewElement)) || !pViewElement)
        return nullptr;

    VARIANT varList = {};
    varList.vt = VT_I4;
    varList.lVal = UIA_ListControlTypeId;
    VARIANT varGrid = {};
    varGrid.vt = VT_I4;
    varGrid.lVal = UIA_DataGridControlTypeId;

    IUIAutomationCondition* pCondList = nullptr;
    IUIAutomationCondition* pCondGrid = nullptr;
    IUIAutomationCondition* pCondEither = nullptr;
    pAuto->CreatePropertyCondition(UIA_ControlTypePropertyId, varList, &pCondList);
    pAuto->CreatePropertyCondition(UIA_ControlTypePropertyId, varGrid, &pCondGrid);
    if (pCondList && pCondGrid)
        pAuto->CreateOrCondition(pCondList, pCondGrid, &pCondEither);

    IUIAutomationElement* pFound = nullptr;
    if (pCondEither)
        pViewElement->FindFirst(TreeScope_Descendants, pCondEither, &pFound);

    if (pCondEither) pCondEither->Release();
    if (pCondList) pCondList->Release();
    if (pCondGrid) pCondGrid->Release();
    pViewElement->Release();

    return pFound;  // AddRef'd by FindFirst; caller owns it
}

// Identifies each column's widest item via IShellFolder2::GetDetailsEx (the view's own
// cached display data, fast to read) -- not to measure the final pixel width ourselves,
// only to know which single item to scroll into view for an exact native measurement.
struct ScanResult {
    std::vector<int> headerFloor;             // header label width, used as a floor only
    std::vector<PITEMID_CHILD> widestPidl;    // owned; null if every item's value was empty
    bool aborted = false;                     // hit the wall-clock budget before finishing
};

// Bounded by wall-clock time, not just item count: GetDetailsEx can be a real round trip on slow property handlers or network/cloud storage.
static ScanResult ScanForWidestItems(
    IFolderView2* pFV2, IShellFolder2* pFolder2, const std::vector<PROPERTYKEY>& keys,
    UINT colCount, int itemCount, HFONT hFontHeader, HFONT hFontItem, HDC hdc)
{
    ScanResult result;
    result.headerFloor.assign(colCount, 0);
    result.widestPidl.assign(colCount, nullptr);
    std::vector<int> runningMax(colCount, 0);

    HFONT hOldFont = static_cast<HFONT>(SelectObject(hdc, hFontHeader));
    for (UINT c = 0; c < colCount; c++) {
        IPropertyDescription* pDesc = nullptr;
        if (SUCCEEDED(PSGetPropertyDescription(keys[c], IID_PPV_ARGS(&pDesc))) && pDesc) {
            PWSTR pszHeader = nullptr;
            if (SUCCEEDED(pDesc->GetDisplayName(&pszHeader)) && pszHeader) {
                result.headerFloor[c] = MeasureTextWidth(hdc, pszHeader);
                CoTaskMemFree(pszHeader);
            }
            pDesc->Release();
        }
    }

    SelectObject(hdc, hFontItem);

    ULONGLONG deadline = GetTickCount64() + kScanTimeBudgetMs;
    WCHAR dispBuf[512];

    for (int i = 0; i < itemCount && !result.aborted; i++) {
        PITEMID_CHILD pidl = nullptr;
        if (FAILED(pFV2->Item(i, &pidl)) || !pidl)
            continue;

        for (UINT c = 0; c < colCount; c++) {
            if (GetTickCount64() > deadline) {
                result.aborted = true;
                break;
            }

            VARIANT v;
            VariantInit(&v);
            if (SUCCEEDED(pFolder2->GetDetailsEx(pidl, reinterpret_cast<const SHCOLUMNID*>(&keys[c]), &v))) {
                PCWSTR text = nullptr;
                VARIANT converted;
                VariantInit(&converted);
                bool haveConverted = false;

                if ((v.vt & VT_ARRAY) && (v.vt & VT_TYPEMASK) == VT_BSTR) {
                    // Multi-value property (e.g. Contributing Artists): PSFormatForDisplay
                    // takes a PROPVARIANT, whose vector representation differs from a
                    // VARIANT SAFEARRAY, so this is joined manually instead of converted.
                    SAFEARRAY* psa = v.parray;
                    if (psa) {
                        LONG lBound = 0, uBound = -1;
                        SafeArrayGetLBound(psa, 1, &lBound);
                        SafeArrayGetUBound(psa, 1, &uBound);
                        dispBuf[0] = L'\0';
                        for (LONG idx = lBound; idx <= uBound; idx++) {
                            BSTR item = nullptr;
                            if (SUCCEEDED(SafeArrayGetElement(psa, &idx, &item)) && item) {
                                if (dispBuf[0] != L'\0') wcsncat_s(dispBuf, L"; ", _TRUNCATE);
                                wcsncat_s(dispBuf, item, _TRUNCATE);
                                SysFreeString(item);
                            }
                        }
                        text = dispBuf;
                    }
                } else if (!(v.vt & VT_ARRAY)) {
                    // Scalar (string, number, date): layout matches between VARIANT and PROPVARIANT here.
                    if (SUCCEEDED(PSFormatForDisplay(keys[c], *reinterpret_cast<const PROPVARIANT*>(&v),
                                                      PDFF_DEFAULT, dispBuf, ARRAYSIZE(dispBuf))))
                        text = dispBuf;
                    else if (SUCCEEDED(VariantChangeType(&converted, &v, 0, VT_BSTR))) {
                        text = converted.bstrVal;
                        haveConverted = true;
                    }
                }

                if (text) {
                    int w = MeasureTextWidth(hdc, text);
                    if (w > runningMax[c]) {
                        runningMax[c] = w;
                        if (result.widestPidl[c]) ILFree(result.widestPidl[c]);
                        result.widestPidl[c] = ILCloneChild(pidl);
                    }
                }

                if (haveConverted) VariantClear(&converted);
                VariantClear(&v);
            }
        }
        CoTaskMemFree(pidl);
    }

    SelectObject(hdc, hOldFont);
    return result;
}

// Computes Name's width from leftover viewport space after other columns; never returns <= 0 (0 would mean CM_WIDTH_AUTOSIZE to the caller).
static int ComputeElasticNameWidth(HWND hwndView, HWND hwndListView, IUIAutomationElement* pListElement,
                                    const std::vector<int>& widths, UINT nameColIndex, UINT /*dpi*/, double dpiScale)
{
    int minNameWidth = static_cast<int>(60 * dpiScale);

    int viewportWidth = 0;

    if (hwndListView) {
        // GetClientRect already excludes the non-client vertical scrollbar,
        // so its width isn't subtracted again below.
        RECT rc = {};
        GetClientRect(hwndListView, &rc);
        viewportWidth = rc.right - rc.left;
    } else if (pListElement) {
        RECT rcElem = {};
        if (SUCCEEDED(pListElement->get_CurrentBoundingRectangle(&rcElem)))
            viewportWidth = rcElem.right - rcElem.left;
    } else if (hwndView) {
        RECT rc = {};
        GetClientRect(hwndView, &rc);
        viewportWidth = rc.right - rc.left;
    }

    if (viewportWidth <= 0) {
        if (nameColIndex < widths.size() && widths[nameColIndex] > 0) return widths[nameColIndex];
        return minNameWidth;
    }

    int minColWidth = static_cast<int>(40 * dpiScale);
    int otherColumnsTotal = 0;
    for (size_t c = 0; c < widths.size(); c++) {
        if (c == nameColIndex) continue;
        otherColumnsTotal += (widths[c] < minColWidth) ? minColWidth : widths[c];
    }

    int initialSafetyMargin = static_cast<int>(8 * dpiScale);
    int nameWidth = viewportWidth - otherColumnsTotal - initialSafetyMargin;
    return (nameWidth < minNameWidth) ? minNameWidth : nameWidth;
}

static void ApplyColumnWidthsLiteral(IColumnManager* pCM, const std::vector<PROPERTYKEY>& keys,
                                      const std::vector<int>& widths)
{
    for (size_t c = 0; c < keys.size(); c++) {
        CM_COLUMNINFO ci = {};
        ci.cbSize = sizeof(ci);
        ci.dwMask = CM_MASK_WIDTH;
        ci.uWidth = widths[c] > 0 ? static_cast<UINT>(widths[c]) : CM_WIDTH_AUTOSIZE;
        pCM->SetColumnInfo(keys[c], &ci);
    }
}

// Timer-driven verify/correct loop for Elastic mode: everything else about a fit is
// synchronous (no scrolling, no scan spread across ticks), so this is the only piece
// that still needs to wait for the view to settle and possibly retry.
struct FitContext {
    IShellView* pShellView = nullptr;
    IFolderView2* pFV2 = nullptr;
    IColumnManager* pCM = nullptr;

    HWND hwndView = nullptr;
    HWND hwndListView = nullptr;
    HWND hwndHeader = nullptr;

    std::vector<PROPERTYKEY> keys;
    UINT colCount = 0;
    UINT dpi = 96;
    double dpiScale = 1.0;
    bool elasticMode = false;
    UINT nameColumnIndex = 0;  // index of PKEY_ItemNameDisplay within keys; not always 0
    int itemCount = 0;

    std::vector<int> headerFloor;               // used only as a floor under the measured width
    std::vector<PITEMID_CHILD> widestPidl;       // owned; per column, from the initial scan
    std::vector<UINT> exactColumnQueue;          // column indices still needing scroll+autosize+read
    size_t exactQueueIndex = 0;
    int preAutosizeWidth = 0;                    // width just before the current column's autosize
    bool retriedRead = false;                    // one extra settle tick if the read looked stale

    // Real scroll-position restore: captured before the exact-measurement
    // queue starts, re-applied once it's done, so a fit never leaves the
    // user somewhere other than where they were.
    int topIndexBeforeFit = -1;                  // valid only when hwndListView exists
    PITEMID_CHILD pidlFocusedFallback = nullptr; // used only when hwndListView is unavailable

    std::vector<int> maxWidths;  // final widths; only index 0 (Name) changes during elastic verify

    int elasticAttempt = 0;
    bool elasticConfirmed = false;
    static constexpr int kMaxElasticAttempts = 8;

    // Only populated when hwndListView couldn't be found (no classic
    // SysListView32 window on this build). See FindListElementViaUIA.
    IUIAutomationElement* pElasticListElement = nullptr;

    enum class Phase { WaitSelectWidest, WaitExactAutosize, WaitRestoreScroll, WaitElasticVerify }
        phase = Phase::WaitSelectWidest;

    ~FitContext() {
        if (pElasticListElement) pElasticListElement->Release();
        if (pidlFocusedFallback) ILFree(pidlFocusedFallback);
        for (auto p : widestPidl)
            if (p) ILFree(p);
        if (pCM) pCM->Release();
        if (pFV2) pFV2->Release();
        if (pShellView) pShellView->Release();
    }
};

static void Step_Finalize(FitContext* ctx, HWND hwndOwner) {
    if (ctx->elasticMode && ctx->colCount > 0) {
        TabWidthCache cache;
        cache.colCount = ctx->colCount;
        cache.keys = ctx->keys;
        cache.allWidths = ctx->maxWidths;
        EnterCriticalSection(&g_cs);
        g_tabWidthCache[hwndOwner] = std::move(cache);
        LeaveCriticalSection(&g_cs);
    }

    std::unique_ptr<FitContext> ctxToDestroy;
    EnterCriticalSection(&g_cs);
    if (auto it = g_fitContexts->find(hwndOwner); it != g_fitContexts->end()) {
        ctxToDestroy = std::move(it->second);
        g_fitContexts->erase(it);
    }
    LeaveCriticalSection(&g_cs);
    // ctxToDestroy is destroyed here, outside the lock, on the owning thread.
}

// Processes exact-measurement columns one at a time so each column's widest item is guaranteed visible for its own autosize step.
static void Step_SelectWidest(FitContext* ctx, HWND hwndOwner) {
    ctx->exactColumnQueue.clear();
    for (UINT c = 0; c < ctx->colCount; c++) {
        // Elastic mode's Name column comes from leftover viewport space, not content.
        if (c == ctx->nameColumnIndex && ctx->elasticMode) continue;
        if (ctx->widestPidl[c]) ctx->exactColumnQueue.push_back(c);
    }
    ctx->exactQueueIndex = 0;

    if (ctx->exactColumnQueue.empty()) {
        if (ctx->elasticMode) {
            ctx->pElasticListElement = ctx->hwndListView ? nullptr : FindListElementViaUIA(ctx->hwndView);
            ctx->maxWidths[ctx->nameColumnIndex] = ComputeElasticNameWidth(
                ctx->hwndView, ctx->hwndListView, ctx->pElasticListElement, ctx->maxWidths,
                ctx->nameColumnIndex, ctx->dpi, ctx->dpiScale);
            ApplyColumnWidthsLiteral(ctx->pCM, ctx->keys, ctx->maxWidths);
            ctx->phase = FitContext::Phase::WaitElasticVerify;
            SetTimer(hwndOwner, FITSTEP_TIMER_ID, kElasticVerifyDelayMs, nullptr);
        } else {
            ApplyColumnWidthsLiteral(ctx->pCM, ctx->keys, ctx->maxWidths);
            Step_Finalize(ctx, hwndOwner);
        }
        return;
    }

    UINT c = ctx->exactColumnQueue[0];
    PITEMID_CHILD apidl[1] = { ctx->widestPidl[c] };
    ctx->pFV2->SelectAndPositionItems(1, const_cast<PCUITEMID_CHILD_ARRAY>(apidl), nullptr,
        SVSI_ENSUREVISIBLE | SVSI_NOTAKEFOCUS | SVSI_NOSTATECHANGE);

    ctx->phase = FitContext::Phase::WaitSelectWidest;
    SetTimer(hwndOwner, FITSTEP_TIMER_ID, kFitStepDelayMs, nullptr);
}

// Asks the built-in autosize to measure the current queue column, now that its (and only
// its) widest item is visible -- this is the exact, pixel-perfect width Explorer's own
// rendering produces, not an approximation from our own font/metric guesses.
static void Step_ApplyExactAutosize(FitContext* ctx, HWND hwndOwner) {
    UINT c = ctx->exactColumnQueue[ctx->exactQueueIndex];
    CM_COLUMNINFO ciBefore = {};
    ciBefore.cbSize = sizeof(ciBefore);
    ciBefore.dwMask = CM_MASK_WIDTH;
    ctx->pCM->GetColumnInfo(ctx->keys[c], &ciBefore);
    ctx->preAutosizeWidth = static_cast<int>(ciBefore.uWidth);
    ctx->retriedRead = false;

    CM_COLUMNINFO ci = {};
    ci.cbSize = sizeof(ci);
    ci.dwMask = CM_MASK_WIDTH;
    ci.uWidth = CM_WIDTH_AUTOSIZE;
    ctx->pCM->SetColumnInfo(ctx->keys[c], &ci);

    ctx->phase = FitContext::Phase::WaitExactAutosize;
    SetTimer(hwndOwner, FITSTEP_TIMER_ID, kFitStepDelayMs, nullptr);
}

// Reads the current column's exact width, advances the queue, then restores the real
// scroll position once every column has been measured.
static void Step_ReadExactAndRestore(FitContext* ctx, HWND hwndOwner) {
    UINT c = ctx->exactColumnQueue[ctx->exactQueueIndex];
    CM_COLUMNINFO ciResult = {};
    ciResult.cbSize = sizeof(ciResult);
    ciResult.dwMask = CM_MASK_WIDTH;
    bool gotResult = SUCCEEDED(ctx->pCM->GetColumnInfo(ctx->keys[c], &ciResult)) && ciResult.uWidth > 0;

    // A read-back equal to the pre-autosize width likely means the view
    // hadn't finished relayout yet; wait one more tick and re-read once.
    if (gotResult && !ctx->retriedRead &&
        static_cast<int>(ciResult.uWidth) == ctx->preAutosizeWidth) {
        ctx->retriedRead = true;
        ctx->phase = FitContext::Phase::WaitExactAutosize;
        SetTimer(hwndOwner, FITSTEP_TIMER_ID, kFitStepDelayMs, nullptr);
        return;
    }

    if (gotResult) {
        int w = static_cast<int>(ciResult.uWidth);
        ctx->maxWidths[c] = (w < ctx->headerFloor[c]) ? ctx->headerFloor[c] : w;
    }

    ctx->exactQueueIndex++;
    if (ctx->exactQueueIndex < ctx->exactColumnQueue.size()) {
        UINT nextCol = ctx->exactColumnQueue[ctx->exactQueueIndex];
        PITEMID_CHILD apidl[1] = { ctx->widestPidl[nextCol] };
        ctx->pFV2->SelectAndPositionItems(1, const_cast<PCUITEMID_CHILD_ARRAY>(apidl), nullptr,
            SVSI_ENSUREVISIBLE | SVSI_NOTAKEFOCUS | SVSI_NOSTATECHANGE);
        ctx->phase = FitContext::Phase::WaitSelectWidest;
        SetTimer(hwndOwner, FITSTEP_TIMER_ID, kFitStepDelayMs, nullptr);
        return;
    }

    // Ensuring the last item visible first, then the original top item, lands it exactly at the top again (not just "somewhere visible").
    if (ctx->hwndListView && ctx->topIndexBeforeFit >= 0) {
        int lastIndex = static_cast<int>(SendMessageW(ctx->hwndListView, LVM_GETITEMCOUNT, 0, 0)) - 1;
        if (lastIndex >= 0)
            SendMessageW(ctx->hwndListView, LVM_ENSUREVISIBLE, lastIndex, FALSE);
        SendMessageW(ctx->hwndListView, LVM_ENSUREVISIBLE, ctx->topIndexBeforeFit, FALSE);
    } else if (ctx->pidlFocusedFallback) {
        PITEMID_CHILD apidlR[1] = { ctx->pidlFocusedFallback };
        ctx->pFV2->SelectAndPositionItems(1, const_cast<PCUITEMID_CHILD_ARRAY>(apidlR), nullptr,
            SVSI_ENSUREVISIBLE | SVSI_NOTAKEFOCUS | SVSI_NOSTATECHANGE);
    }
    ctx->phase = FitContext::Phase::WaitRestoreScroll;
    SetTimer(hwndOwner, FITSTEP_TIMER_ID, kFitStepDelayMs, nullptr);
}

// The scroll restore has settled; finish up (Elastic still needs its Name width computed
// from the now-known other-column widths and its own verify/correct loop).
static void Step_FinishAfterRestore(FitContext* ctx, HWND hwndOwner) {
    if (ctx->elasticMode) {
        ctx->pElasticListElement = ctx->hwndListView ? nullptr : FindListElementViaUIA(ctx->hwndView);
        ctx->maxWidths[ctx->nameColumnIndex] = ComputeElasticNameWidth(
            ctx->hwndView, ctx->hwndListView, ctx->pElasticListElement, ctx->maxWidths,
            ctx->nameColumnIndex, ctx->dpi, ctx->dpiScale);
    }

    ApplyColumnWidthsLiteral(ctx->pCM, ctx->keys, ctx->maxWidths);
    Wh_Log(L"Auto-fitted %u column(s) (%s, %d items scanned)", ctx->colCount,
           ctx->elasticMode ? L"elastic" : L"full folder scan", ctx->itemCount);

    if (!ctx->elasticMode) {
        Step_Finalize(ctx, hwndOwner);
        return;
    }

    ctx->phase = FitContext::Phase::WaitElasticVerify;
    SetTimer(hwndOwner, FITSTEP_TIMER_ID, kElasticVerifyDelayMs, nullptr);
}

// Verifies the Name width estimate against reality (header extent, scroll overflow, UI Automation) and shrinks it if still overflowing.
static void Step_VerifyElasticFit(FitContext* ctx, HWND hwndOwner) {
    HWND hwndMeasure = ctx->hwndListView ? ctx->hwndListView : ctx->hwndView;
    int viewportWidth = 0;
    if (hwndMeasure) {
        RECT rc = {};
        GetClientRect(hwndMeasure, &rc);
        viewportWidth = rc.right - rc.left;
    }
    if (!ctx->hwndListView && ctx->pElasticListElement) {
        RECT rcElem = {};
        if (SUCCEEDED(ctx->pElasticListElement->get_CurrentBoundingRectangle(&rcElem))) {
            int elemWidth = rcElem.right - rcElem.left;
            if (elemWidth > 0) viewportWidth = elemWidth;
        }
    }

    int overflow = 0;
    if (ctx->hwndHeader && viewportWidth > 0) {
        int headerItemCount = static_cast<int>(SendMessageW(ctx->hwndHeader, HDM_GETITEMCOUNT, 0, 0));
        if (headerItemCount > 0) {
            RECT rcLast = {};
            if (SendMessageW(ctx->hwndHeader, HDM_GETITEMRECT, headerItemCount - 1,
                              reinterpret_cast<LPARAM>(&rcLast))) {
                int headerOverflow = rcLast.right - viewportWidth;
                if (headerOverflow > overflow) overflow = headerOverflow;
            }
        }
    }

    if (ctx->hwndListView) {
        SCROLLINFO si = {};
        si.cbSize = sizeof(si);
        si.fMask = SIF_RANGE | SIF_PAGE;
        if (GetScrollInfo(ctx->hwndListView, SB_HORZ, &si) && si.nPage > 0) {
            int scrollOverflow = si.nMax - si.nMin - static_cast<int>(si.nPage) + 1;
            if (scrollOverflow > overflow) overflow = scrollOverflow;
        }
    }

    bool uiaSaysScrolling = false;
    if (!ctx->hwndListView && ctx->pElasticListElement)
        uiaSaysScrolling = ElementHasHorizontalScroll(ctx->pElasticListElement);

    int minNameWidth = static_cast<int>(60 * ctx->dpiScale);
    bool canShrink = ctx->maxWidths[ctx->nameColumnIndex] > minNameWidth;
    bool haveAttemptsLeft = ctx->elasticAttempt < FitContext::kMaxElasticAttempts;

    if ((overflow > 0 || uiaSaysScrolling) && haveAttemptsLeft && canShrink) {
        int safetyMargin = static_cast<int>(10 * ctx->dpiScale);
        int shrinkBy = (overflow > 0) ? (overflow + safetyMargin) : static_cast<int>(24 * ctx->dpiScale);
        int newNameWidth = ctx->maxWidths[ctx->nameColumnIndex] - shrinkBy;
        if (newNameWidth < minNameWidth) newNameWidth = minNameWidth;

        if (newNameWidth < ctx->maxWidths[ctx->nameColumnIndex]) {
            ctx->maxWidths[ctx->nameColumnIndex] = newNameWidth;
            CM_COLUMNINFO ci = {};
            ci.cbSize = sizeof(ci);
            ci.dwMask = CM_MASK_WIDTH;
            ci.uWidth = static_cast<UINT>(newNameWidth);
            ctx->pCM->SetColumnInfo(ctx->keys[ctx->nameColumnIndex], &ci);

            ctx->elasticAttempt++;
            ctx->elasticConfirmed = false;
            SetTimer(hwndOwner, FITSTEP_TIMER_ID, kElasticVerifyDelayMs, nullptr);
            return;
        }
    }

    if (!ctx->elasticConfirmed && haveAttemptsLeft) {
        ctx->elasticConfirmed = true;
        ctx->elasticAttempt++;
        SetTimer(hwndOwner, FITSTEP_TIMER_ID, kElasticVerifyDelayMs, nullptr);
        return;
    }

    Step_Finalize(ctx, hwndOwner);
}

static void AdvanceFitContext(HWND hwndOwner) {
    FitContext* ctx = nullptr;
    EnterCriticalSection(&g_cs);
    auto it = g_fitContexts->find(hwndOwner);
    if (it != g_fitContexts->end()) ctx = it->second.get();
    LeaveCriticalSection(&g_cs);
    if (!ctx) return;

    switch (ctx->phase) {
        case FitContext::Phase::WaitSelectWidest:  Step_ApplyExactAutosize(ctx, hwndOwner); break;
        case FitContext::Phase::WaitExactAutosize: Step_ReadExactAndRestore(ctx, hwndOwner); break;
        case FitContext::Phase::WaitRestoreScroll: Step_FinishAfterRestore(ctx, hwndOwner); break;
        case FitContext::Phase::WaitElasticVerify: Step_VerifyElasticFit(ctx, hwndOwner); break;
    }
}

// Starts the Elastic verify loop, taking ownership of pFV2Owned/pCMOwned/pListElementOwned.
static void StartElasticVerify(IShellView* pShellView, IFolderView2* pFV2Owned, IColumnManager* pCMOwned,
                                HWND hwndOwner, HWND hwndView, HWND hwndListView, HWND hwndHeader,
                                std::vector<PROPERTYKEY> keys, UINT colCount, UINT nameColIndex,
                                UINT dpi, double dpiScale,
                                std::vector<int> widths, IUIAutomationElement* pListElementOwned)
{
    auto ctx = std::make_unique<FitContext>();
    pShellView->AddRef();
    ctx->pShellView = pShellView;
    ctx->pFV2 = pFV2Owned;
    ctx->pCM = pCMOwned;
    ctx->hwndView = hwndView;
    ctx->hwndListView = hwndListView;
    ctx->hwndHeader = hwndHeader;
    ctx->keys = std::move(keys);
    ctx->colCount = colCount;
    ctx->nameColumnIndex = nameColIndex;
    ctx->dpi = dpi;
    ctx->dpiScale = dpiScale;
    ctx->maxWidths = std::move(widths);
    ctx->pElasticListElement = pListElementOwned;
    ctx->elasticMode = true;
    ctx->phase = FitContext::Phase::WaitElasticVerify;

    EnterCriticalSection(&g_cs);
    (*g_fitContexts)[hwndOwner] = std::move(ctx);
    LeaveCriticalSection(&g_cs);

    SetTimer(hwndOwner, FITSTEP_TIMER_ID, kElasticVerifyDelayMs, nullptr);
}

// Entry point: performs a fit for pShellView, whose window messages/timers are owned by
// hwndOwner. resizeOnly hints that this is a pure Elastic-mode window resize, letting a
// cached fit skip the folder scan entirely and just recompute the Name column.
static void StartAutoFit(IShellView* pShellView, HWND hwndOwner, bool resizeOnly) {
    Settings settings = GetCachedSettings();

    EnterCriticalSection(&g_cs);
    bool alreadyRunning = g_fitContexts->find(hwndOwner) != g_fitContexts->end();
    LeaveCriticalSection(&g_cs);
    if (alreadyRunning) {
        if (!g_unloading.load(std::memory_order_relaxed)) {
            MarkFitTrigger(hwndOwner, resizeOnly);  // don't drop the hint on the retry
            SetTimer(hwndOwner, AUTOFIT_TIMER_ID, settings.delayMs, nullptr);
        }
        return;
    }

    IFolderView2* pFV2 = nullptr;
    if (FAILED(pShellView->QueryInterface(IID_PPV_ARGS(&pFV2))) || !pFV2)
        return;

    FOLDERVIEWMODE viewMode = FVM_AUTO;
    int iconSize = 0;
    if (FAILED(pFV2->GetViewModeAndIconSize(&viewMode, &iconSize)) || viewMode != FVM_DETAILS) {
        pFV2->Release();
        return;
    }

    IColumnManager* pCM = nullptr;
    if (FAILED(pShellView->QueryInterface(IID_PPV_ARGS(&pCM))) || !pCM) {
        pFV2->Release();
        return;
    }

    UINT colCount = 0;
    if (FAILED(pCM->GetColumnCount(CM_ENUM_VISIBLE, &colCount)) || colCount == 0) {
        pCM->Release();
        pFV2->Release();
        return;
    }

    std::vector<PROPERTYKEY> keys(colCount);
    if (FAILED(pCM->GetColumns(CM_ENUM_VISIBLE, keys.data(), colCount))) {
        pCM->Release();
        pFV2->Release();
        return;
    }

    if (settings.fitMode == FitMode::Visible) {
        ApplyColumnWidthsLiteral(pCM, keys, std::vector<int>(colCount, 0));
        Wh_Log(L"Auto-fitted %u column(s)", colCount);
        pCM->Release();
        pFV2->Release();
        return;
    }

    bool elasticMode = (settings.fitMode == FitMode::Elastic);
    UINT nameColIndex = FindNameColumnIndex(keys);
    if (elasticMode && nameColIndex == kNoNameColumn) {
        // No Name column to stretch -- degrade to Full-mode behavior rather
        // than silently mangling an arbitrary column.
        elasticMode = false;
    }
    HWND hwndView = nullptr;
    pShellView->GetWindow(&hwndView);
    HWND hwndListView = hwndView ? FindDescendantByClass(hwndView, L"SysListView32") : nullptr;
    HWND hwndHeader = hwndView ? FindDescendantByClass(hwndView, L"SysHeader32") : nullptr;
    UINT dpi = hwndView ? GetDpiForWindow(hwndView) : 96;
    if (dpi == 0) dpi = 96;
    double dpiScale = dpi / 96.0;

    // Fast path: a pure-resize trigger in Elastic mode only needs Name's
    // width recomputed against the new viewport -- every other column's
    // width is unchanged from the last full fit, so skip the scan entirely.
    if (resizeOnly && elasticMode) {
        TabWidthCache cached;
        bool haveCache = false;
        EnterCriticalSection(&g_cs);
        auto it = g_tabWidthCache.find(hwndOwner);
        if (it != g_tabWidthCache.end() && it->second.colCount == colCount && SameColumnKeys(it->second.keys, keys)) {
            cached = it->second;
            haveCache = true;
        }
        LeaveCriticalSection(&g_cs);

        if (haveCache) {
            std::vector<int> widths = cached.allWidths;

            IUIAutomationElement* pListElement = hwndListView ? nullptr : FindListElementViaUIA(hwndView);
            widths[nameColIndex] = ComputeElasticNameWidth(hwndView, hwndListView, pListElement, widths,
                                                            nameColIndex, dpi, dpiScale);
            ApplyColumnWidthsLiteral(pCM, keys, widths);
            Wh_Log(L"Auto-fitted %u column(s) (elastic, resize)", colCount);

            StartElasticVerify(pShellView, pFV2, pCM, hwndOwner, hwndView, hwndListView, hwndHeader,
                                std::move(keys), colCount, nameColIndex, dpi, dpiScale,
                                std::move(widths), pListElement);
            return;
        }
        // No usable cache -- fall through to a full scan.
    }

    int itemCount = 0;
    HRESULT hrCount = pFV2->ItemCount(SVGIO_ALLVIEW, &itemCount);
    if (FAILED(hrCount) || itemCount > settings.maxScanItems) {
        // Folder too large (or count unavailable) for a full scan this time.
        // Elastic can still stretch Name using native-autosize results for
        // the other columns, without the exhaustive per-item scan.
        if (elasticMode) {
            ApplyColumnWidthsLiteral(pCM, keys, std::vector<int>(colCount, 0));
            std::vector<int> widths(colCount, 0);
            for (UINT c = 0; c < colCount; c++) {
                if (c == nameColIndex) continue;
                CM_COLUMNINFO ci = {};
                ci.cbSize = sizeof(ci);
                ci.dwMask = CM_MASK_WIDTH;
                if (SUCCEEDED(pCM->GetColumnInfo(keys[c], &ci)))
                    widths[c] = static_cast<int>(ci.uWidth);
            }
            IUIAutomationElement* pListElement = hwndListView ? nullptr : FindListElementViaUIA(hwndView);
            widths[nameColIndex] = ComputeElasticNameWidth(hwndView, hwndListView, pListElement, widths,
                                                            nameColIndex, dpi, dpiScale);
            ApplyColumnWidthsLiteral(pCM, keys, widths);
            Wh_Log(L"Auto-fitted %u column(s) (elastic, large folder: name-stretch only)", colCount);
            StartElasticVerify(pShellView, pFV2, pCM, hwndOwner, hwndView, hwndListView, hwndHeader,
                                std::move(keys), colCount, nameColIndex, dpi, dpiScale,
                                std::move(widths), pListElement);
            return;
        }
        ApplyColumnWidthsLiteral(pCM, keys, std::vector<int>(colCount, 0));
        Wh_Log(L"Auto-fitted %u column(s) (fallback: visible rows only)", colCount);
        pCM->Release();
        pFV2->Release();
        return;
    }

    IShellFolder2* pFolder2 = nullptr;
    if (FAILED(pFV2->GetFolder(IID_PPV_ARGS(&pFolder2))) || !pFolder2) {
        pCM->Release();
        pFV2->Release();
        return;
    }

    HFONT hFontHeader = CreateFontFromWindowOrDefault(hwndHeader, dpi);
    HFONT hFontItem = CreateFontFromWindowOrDefault(hwndListView, dpi);
    HDC hdcScreen = GetDC(nullptr);
    HDC hdc = CreateCompatibleDC(hdcScreen);
    ReleaseDC(nullptr, hdcScreen);

    ScanResult scan = ScanForWidestItems(pFV2, pFolder2, keys, colCount, itemCount,
                                          hFontHeader, hFontItem, hdc);

    DeleteDC(hdc);
    if (hFontHeader) DeleteObject(hFontHeader);
    if (hFontItem) DeleteObject(hFontItem);
    pFolder2->Release();

    if (scan.aborted) {
        // Ran past the wall-clock budget (slow property handlers, network/cloud
        // storage, etc.) -- fall back to the fast built-in autosize rather than
        // freezing the shell for an unbounded amount of time.
        for (auto p : scan.widestPidl)
            if (p) ILFree(p);
        ApplyColumnWidthsLiteral(pCM, keys, std::vector<int>(colCount, 0));
        Wh_Log(L"Auto-fitted %u column(s) (fallback: scan exceeded time budget)", colCount);
        pCM->Release();
        pFV2->Release();
        return;
    }

    auto ctx = std::make_unique<FitContext>();
    pShellView->AddRef();
    ctx->pShellView = pShellView;
    ctx->pFV2 = pFV2;
    ctx->pCM = pCM;
    ctx->hwndView = hwndView;
    ctx->hwndListView = hwndListView;
    ctx->hwndHeader = hwndHeader;
    ctx->keys = std::move(keys);
    ctx->colCount = colCount;
    ctx->nameColumnIndex = nameColIndex;
    ctx->dpi = dpi;
    ctx->dpiScale = dpiScale;
    ctx->elasticMode = elasticMode;
    ctx->itemCount = itemCount;
    ctx->headerFloor = std::move(scan.headerFloor);
    ctx->widestPidl = std::move(scan.widestPidl);
    ctx->maxWidths.assign(colCount, 0);
    if (elasticMode) {
        // Elastic needs a known fixed width per column to compute Name's leftover space --
        // CM_WIDTH_AUTOSIZE's actual result is decided by Explorer afterward and could
        // exceed what was assumed here, overflowing the row.
        int emptyColPad = static_cast<int>(20 * dpiScale);
        for (UINT c = 0; c < colCount; c++)
            if (!ctx->widestPidl[c]) ctx->maxWidths[c] = ctx->headerFloor[c] + emptyColPad;
    }

    // Capture where the user actually is before any scrolling starts, so
    // it can be restored exactly once every column has been measured.
    if (hwndListView) {
        ctx->topIndexBeforeFit = static_cast<int>(SendMessageW(hwndListView, LVM_GETTOPINDEX, 0, 0));
    } else {
        int focusIdx = -1;
        pFV2->GetFocusedItem(&focusIdx);
        if (focusIdx >= 0) {
            PITEMID_CHILD pidlFocused = nullptr;
            if (SUCCEEDED(pFV2->Item(focusIdx, &pidlFocused)) && pidlFocused) {
                ctx->pidlFocusedFallback = ILCloneChild(pidlFocused);
                CoTaskMemFree(pidlFocused);
            }
        }
    }

    FitContext* rawCtx = ctx.get();
    EnterCriticalSection(&g_cs);
    (*g_fitContexts)[hwndOwner] = std::move(ctx);
    LeaveCriticalSection(&g_cs);

    Step_SelectWidest(rawCtx, hwndOwner);
}

// Finds the ShellTabWindowClass ancestor of a HWND. GetAncestor(GA_PARENT) is used rather
// than GetParent, which returns the owner (not the parent) for top-level windows.
static HWND FindTabWindow(HWND hwnd) {
    WCHAR cls[64] = {};
    HWND cur = hwnd;
    while (cur) {
        GetClassNameW(cur, cls, ARRAYSIZE(cls));
        if (wcscmp(cls, L"ShellTabWindowClass") == 0)
            return cur;
        cur = GetAncestor(cur, GA_PARENT);
    }
    return nullptr;
}

// True if hwnd is subclassed and will process WM_TIMER (and thus can KillTimer it).
static bool IsWindowSubclassed(HWND hwnd) {
    EnterCriticalSection(&g_cs);
    bool found = g_windowToTab.find(hwnd) != g_windowToTab.end();
    LeaveCriticalSection(&g_cs);
    return found;
}

static bool IsSubclassTarget(HWND hwnd) {
    WCHAR cls[64] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    for (PCWSTR target : kSubclassTargets) {
        if (wcscmp(cls, target) == 0)
            return true;
    }
    return false;
}

// Records hwnd's tab association and subclasses it if not already tracked, only recording
// the association if subclassing actually succeeded (or had already succeeded earlier).
static void TrackAndSubclass(HWND hwnd, HWND hwndTab) {
    EnterCriticalSection(&g_cs);
    bool alreadyTracked = g_windowToTab.find(hwnd) != g_windowToTab.end();
    LeaveCriticalSection(&g_cs);

    if (alreadyTracked || WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, ExplorerSubclassProc, 0)) {
        EnterCriticalSection(&g_cs);
        g_windowToTab[hwnd] = hwndTab;
        LeaveCriticalSection(&g_cs);
    }
}

// Subclass a window if it's a target class, resolving its own tab rather than trusting the caller's.
static void SubclassTargetIfNeeded(HWND hwnd, HWND hwndTab) {
    if (!hwnd || !IsSubclassTarget(hwnd)) return;

    // A window that lives under a tab belongs to that tab -- don't let the
    // tab that happens to be activating claim other tabs in the same frame.
    if (HWND hwndOwnTab = FindTabWindow(hwnd))
        hwndTab = hwndOwnTab;

    TrackAndSubclass(hwnd, hwndTab);
}

struct EnumChildData { HWND hwndTab; };

static BOOL CALLBACK SubclassChildProc(HWND child, LPARAM lp) {
    auto* data = reinterpret_cast<EnumChildData*>(lp);
    SubclassTargetIfNeeded(child, data->hwndTab);
    return TRUE;
}

// Returns the shell view's current folder as an owned absolute PIDL, or null.
static PIDLIST_ABSOLUTE GetShellViewFolderPidl(IShellView* pShellView) {
    IFolderView2* pFV2 = nullptr;
    if (FAILED(pShellView->QueryInterface(IID_PPV_ARGS(&pFV2))) || !pFV2)
        return nullptr;

    IShellFolder* pShellFolder = nullptr;
    HRESULT hr = pFV2->GetFolder(IID_PPV_ARGS(&pShellFolder));
    pFV2->Release();
    if (FAILED(hr) || !pShellFolder)
        return nullptr;

    PIDLIST_ABSOLUTE pidl = nullptr;
    hr = SHGetIDListFromObject(pShellFolder, &pidl);
    pShellFolder->Release();
    return SUCCEEDED(hr) ? pidl : nullptr;
}

// Registers (or re-registers) shell change notifications for hwndTab's current folder.
// Takes ownership of pidl (caller-provided, already resolved via GetShellViewFolderPidl).
static void RegisterFolderChangeNotify(HWND hwndTab, PIDLIST_ABSOLUTE pidl) {
    if (!hwndTab || !pidl || g_shellNotifyMsg == 0) {
        if (pidl) CoTaskMemFree(pidl);
        return;
    }

    EnterCriticalSection(&g_cs);
    auto it = g_tabNotifyReg.find(hwndTab);
    ULONG oldReg = (it != g_tabNotifyReg.end()) ? it->second : 0;
    if (it != g_tabNotifyReg.end())
        g_tabNotifyReg.erase(it);
    LeaveCriticalSection(&g_cs);

    if (oldReg)
        SHChangeNotifyDeregister(oldReg);

    SHChangeNotifyEntry entry = { pidl, FALSE };
    ULONG newReg = SHChangeNotifyRegister(
        hwndTab,
        SHCNRF_ShellLevel | SHCNRF_InterruptLevel | SHCNRF_NewDelivery,
        SHCNE_UPDATEDIR | SHCNE_MKDIR | SHCNE_RMDIR |
            SHCNE_CREATE | SHCNE_DELETE | SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER,
        g_shellNotifyMsg,
        1,
        &entry);

    CoTaskMemFree(pidl);

    if (newReg) {
        EnterCriticalSection(&g_cs);
        g_tabNotifyReg[hwndTab] = newReg;
        LeaveCriticalSection(&g_cs);
    }
}

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
        if (!hwndTab) return hr;  // no tab ancestor -- e.g. the desktop's own CDefView
        HWND hwndTop = GetAncestor(hwndView, GA_ROOT);

        bool viewChanged = false;
        {
            IShellView* pOld = nullptr;
            EnterCriticalSection(&g_cs);
            if (auto it = g_tabShellViews.find(hwndTab); it != g_tabShellViews.end())
                pOld = it->second;
            viewChanged = (pOld != pShellView);
            pShellView->AddRef();
            g_tabShellViews[hwndTab] = pShellView;
            LeaveCriticalSection(&g_cs);
            if (pOld) pOld->Release();  // outside the lock -- can run arbitrary shell teardown
        }

        if (viewChanged) {
            // Drop any fit still in flight for this tab: it's stepping
            // against a now-detached view and would cache the wrong widths.
            std::unique_ptr<FitContext> staleCtx;
            EnterCriticalSection(&g_cs);
            if (auto it = g_fitContexts->find(hwndTab); it != g_fitContexts->end()) {
                staleCtx = std::move(it->second);
                g_fitContexts->erase(it);
            }
            LeaveCriticalSection(&g_cs);
        }

        PIDLIST_ABSOLUTE currentPidl = GetShellViewFolderPidl(pShellView);
        bool sameFolder = false;
        if (!viewChanged && currentPidl) {
            EnterCriticalSection(&g_cs);
            auto it = g_tabFolderPidl.find(hwndTab);
            if (it != g_tabFolderPidl.end() && ILIsEqual(it->second, currentPidl))
                sameFolder = true;
            LeaveCriticalSection(&g_cs);
        }

        if (sameFolder) {
            CoTaskMemFree(currentPidl);
            return hr;  // focus/tab-switch activation, folder unchanged: nothing to do
        }

        if (currentPidl) {
            PIDLIST_ABSOLUTE ownedCopy = ILCloneFull(currentPidl);
            EnterCriticalSection(&g_cs);
            auto it = g_tabFolderPidl.find(hwndTab);
            if (it != g_tabFolderPidl.end()) {
                ILFree(it->second);
                it->second = ownedCopy;
            } else {
                g_tabFolderPidl[hwndTab] = ownedCopy;
            }
            LeaveCriticalSection(&g_cs);
        }

        EnumChildData data = { hwndTab };
        EnumChildWindows(hwndTop, SubclassChildProc, reinterpret_cast<LPARAM>(&data));

        // Also subclass the frame and hwndView directly (not in kSubclassTargets) for the Fluent and classic context-menu refresh paths.
        TrackAndSubclass(hwndTop, hwndTab);
        TrackAndSubclass(hwndView, hwndTab);

        RegisterFolderChangeNotify(hwndTab, currentPidl);  // takes ownership

        MarkFitTrigger(hwndTab, /*resizeOnly=*/false);
        if (!g_unloading.load(std::memory_order_relaxed))
            SetTimer(hwndTab, AUTOFIT_TIMER_ID, GetCachedSettings().delayMs, nullptr);
    }
    return hr;
}

using CDefView_Refresh_t = HRESULT(__thiscall*)(void* pThis);
CDefView_Refresh_t CDefView_Refresh_orig = nullptr;

HRESULT __thiscall CDefView_Refresh_hook(void* pThis) {
    HRESULT hr = CDefView_Refresh_orig(pThis);

    if (SUCCEEDED(hr)) {
        auto* pShellView = reinterpret_cast<IShellView*>(pThis);
        HWND hwndView = nullptr;
        pShellView->GetWindow(&hwndView);

        if (hwndView) {
            HWND hwndTab = FindTabWindow(hwndView);
            HWND hwndTimer = hwndTab ? hwndTab : hwndView;

            if (IsWindowSubclassed(hwndTimer)) {
                MarkFitTrigger(hwndTimer, /*resizeOnly=*/false);
                if (!g_unloading.load(std::memory_order_relaxed))
                    SetTimer(hwndTimer, AUTOFIT_TIMER_ID, GetCachedSettings().delayMs, nullptr);
            }
        }
    }

    return hr;
}

static void TriggerAutoFitFromShellBrowser(IUnknown* pUnk) {
    if (!pUnk) return;

    IServiceProvider* pSP = nullptr;
    if (FAILED(pUnk->QueryInterface(IID_PPV_ARGS(&pSP))) || !pSP)
        return;

    IShellBrowser* pSB = nullptr;
    HRESULT hrQS = pSP->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&pSB));
    pSP->Release();
    if (FAILED(hrQS) || !pSB)
        return;

    IShellView* pSV = nullptr;
    if (FAILED(pSB->QueryActiveShellView(&pSV)) || !pSV) {
        pSB->Release();
        return;
    }

    HWND hwndView = nullptr;
    pSV->GetWindow(&hwndView);

    if (hwndView) {
        HWND hwndTab = FindTabWindow(hwndView);
        HWND hwndTimer = hwndTab ? hwndTab : hwndView;

        if (IsWindowSubclassed(hwndTimer)) {
            MarkFitTrigger(hwndTimer, /*resizeOnly=*/false);
            if (!g_unloading.load(std::memory_order_relaxed))
                SetTimer(hwndTimer, AUTOFIT_TIMER_ID, GetCachedSettings().delayMs, nullptr);
        }
    }
    pSV->Release();
    pSB->Release();
}

using CBrowserHost_Refresh_t = HRESULT(__thiscall*)(void* pThis, long param);
CBrowserHost_Refresh_t CBrowserHost_Refresh_orig = nullptr;

HRESULT __thiscall CBrowserHost_Refresh_hook(void* pThis, long param) {
    HRESULT hr = CBrowserHost_Refresh_orig(pThis, param);
    if (SUCCEEDED(hr))
        TriggerAutoFitFromShellBrowser(reinterpret_cast<IUnknown*>(pThis));
    return hr;
}

// Tears down all per-window state for hwnd; must run on hwnd's own owning thread.
static void CleanupWindowState(HWND hwnd) {
    KillTimer(hwnd, AUTOFIT_TIMER_ID);
    KillTimer(hwnd, FITSTEP_TIMER_ID);

    IShellView* pSVToRelease = nullptr;
    ULONG regToDeregister = 0;
    std::unique_ptr<FitContext> ctxToDestroy;

    EnterCriticalSection(&g_cs);
    auto itSV = g_tabShellViews.find(hwnd);
    if (itSV != g_tabShellViews.end()) {
        pSVToRelease = itSV->second;
        g_tabShellViews.erase(itSV);
    }
    auto itReg = g_tabNotifyReg.find(hwnd);
    if (itReg != g_tabNotifyReg.end()) {
        regToDeregister = itReg->second;
        g_tabNotifyReg.erase(itReg);
    }
    auto itCtx = g_fitContexts->find(hwnd);
    if (itCtx != g_fitContexts->end()) {
        ctxToDestroy = std::move(itCtx->second);
        g_fitContexts->erase(itCtx);
    }
    g_tabWidthCache.erase(hwnd);
    g_pendingTriggerKind.erase(hwnd);
    if (auto itPidl = g_tabFolderPidl.find(hwnd); itPidl != g_tabFolderPidl.end()) {
        ILFree(itPidl->second);
        g_tabFolderPidl.erase(itPidl);
    }
    LeaveCriticalSection(&g_cs);

    bool wasTabWindow = (pSVToRelease != nullptr);
    if (pSVToRelease) pSVToRelease->Release();
    if (regToDeregister) SHChangeNotifyDeregister(regToDeregister);

    // Only drop the cached UIA client when the tab itself is torn down, not for incidental toolbar/rebar teardowns on the same thread.
    if (wasTabWindow)
        ReleaseThreadAutomation();
}

static LRESULT CALLBACK ExplorerSubclassProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    DWORD_PTR dwRefData)
{
    bool isFullRefresh = false;
    bool isResizeRefresh = false;

    if (uMsg == WM_COMMAND) {
        WORD cmdId = LOWORD(wParam);
        if (cmdId == EXPLORER_REFRESH_CMD_1 || cmdId == EXPLORER_REFRESH_CMD_2)
            isFullRefresh = true;
    }

    if (uMsg == WM_KEYDOWN && wParam == VK_F5)
        isFullRefresh = true;

    // Shell-level change notification for this tab's folder, via a registered message id.
    if (g_shellNotifyMsg != 0 && uMsg == g_shellNotifyMsg) {
        PIDLIST_ABSOLUTE* rgpidl = nullptr;
        LONG lEvent = 0;  // required out-param; the event itself isn't filtered on
        HANDLE hLock = SHChangeNotification_Lock(
            reinterpret_cast<HANDLE>(wParam), static_cast<DWORD>(lParam),
            &rgpidl, &lEvent);
        if (hLock) {
            SHChangeNotification_Unlock(hLock);
            isFullRefresh = true;
        }
    }

    // SIZE_MINIMIZED gives an empty client rect, which would otherwise
    // collapse the Name column to CM_WIDTH_AUTOSIZE until the next real fit.
    if (uMsg == WM_SIZE && wParam != SIZE_MINIMIZED && GetCachedSettings().fitMode == FitMode::Elastic)
        isResizeRefresh = true;

    if (isFullRefresh || isResizeRefresh) {
        HWND hwndTab = nullptr;
        {
            EnterCriticalSection(&g_cs);
            auto it = g_windowToTab.find(hwnd);
            if (it != g_windowToTab.end())
                hwndTab = it->second;
            LeaveCriticalSection(&g_cs);
        }

        HWND hwndTimer = hwndTab ? hwndTab : hwnd;
        MarkFitTrigger(hwndTimer, /*resizeOnly=*/isResizeRefresh && !isFullRefresh);
        if (!g_unloading.load(std::memory_order_relaxed))
            SetTimer(hwndTimer, AUTOFIT_TIMER_ID, GetCachedSettings().delayMs, nullptr);
    }

    if (uMsg == WM_TIMER && wParam == AUTOFIT_TIMER_ID) {
        KillTimer(hwnd, AUTOFIT_TIMER_ID);
        bool resizeOnly = ConsumeResizeOnlyFlag(hwnd);

        IShellView* pSV = nullptr;
        {
            EnterCriticalSection(&g_cs);
            auto it = g_tabShellViews.find(hwnd);
            if (it != g_tabShellViews.end() && it->second) {
                pSV = it->second;
                pSV->AddRef();
            }
            LeaveCriticalSection(&g_cs);
        }

        if (pSV) {
            StartAutoFit(pSV, hwnd, resizeOnly);
            pSV->Release();
        }
        return 0;
    }

    if (uMsg == WM_TIMER && wParam == FITSTEP_TIMER_ID) {
        KillTimer(hwnd, FITSTEP_TIMER_ID);
        AdvanceFitContext(hwnd);
        return 0;
    }

    if (g_cleanupMsg != 0 && uMsg == g_cleanupMsg) {
        CleanupWindowState(hwnd);
        return 0;
    }

    if (uMsg == WM_NCDESTROY) {
        // WindhawkUtils' own subclass wrapper already removes the subclass
        // on WM_NCDESTROY before invoking this proc.
        CleanupWindowState(hwnd);

        EnterCriticalSection(&g_cs);
        g_windowToTab.erase(hwnd);
        LeaveCriticalSection(&g_cs);
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    InitializeCriticalSection(&g_cs);
    InitializeCriticalSection(&g_settingsCs);
    LoadSettings();

    g_shellNotifyMsg = RegisterWindowMessageW(L"WindhawkFileExplorerDetailsAutoFitColumns_ShellNotify");
    g_cleanupMsg = RegisterWindowMessageW(L"WindhawkFileExplorerDetailsAutoFitColumns_Cleanup");

    // shell32.dll and explorerframe.dll are already loaded in explorer.exe by the time hooks apply.
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32)
        hShell32 = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hShell32) {
        Wh_Log(L"Failed to load shell32.dll");
        DeleteCriticalSection(&g_settingsCs);
        DeleteCriticalSection(&g_cs);
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
        {
            {
                L"public: virtual long __cdecl CDefView::Refresh(void)",
                L"long __cdecl CDefView::Refresh(void)",
                L"public: virtual long __thiscall CDefView::Refresh(void)",
                L"long __thiscall CDefView::Refresh(void)",
            },
            &CDefView_Refresh_orig,
            CDefView_Refresh_hook,
            true  // optional: not fatal if this symbol doesn't resolve on a given build
        },
    };

    if (!WindhawkUtils::HookSymbols(hShell32, shell32DllHooks, ARRAYSIZE(shell32DllHooks))) {
        Wh_Log(L"ERROR: Could not hook CDefView::UIActivate");
        DeleteCriticalSection(&g_settingsCs);
        DeleteCriticalSection(&g_cs);
        return FALSE;
    }

    Wh_Log(L"CDefView::Refresh hook installed: %s",
           CDefView_Refresh_orig ? L"yes" : L"no (optional, not fatal)");
    Wh_Log(L"CDefView::UIActivate hooked successfully");

    HMODULE hExplorerFrame = GetModuleHandleW(L"explorerframe.dll");
    if (!hExplorerFrame)
        hExplorerFrame = LoadLibraryExW(L"explorerframe.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hExplorerFrame) {
        const WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[] = {
            {
                {
                    L"public: virtual long __cdecl CBrowserHost::Refresh(long)",
                    L"public: virtual long __thiscall CBrowserHost::Refresh(long)",
                },
                &CBrowserHost_Refresh_orig,
                CBrowserHost_Refresh_hook,
                true  // optional: not fatal if this symbol doesn't resolve on a given build
            },
        };
        WindhawkUtils::HookSymbols(hExplorerFrame, explorerFrameDllHooks, ARRAYSIZE(explorerFrameDllHooks));
        Wh_Log(L"CBrowserHost::Refresh hook installed: %s",
               CBrowserHost_Refresh_orig ? L"yes" : L"no (optional, not fatal)");
    } else {
        Wh_Log(L"Failed to load explorerframe.dll (non-fatal, skipping that hook)");
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    g_unloading.store(true, std::memory_order_relaxed);

    std::vector<HWND> windows;
    EnterCriticalSection(&g_cs);
    for (auto& [hwnd, _] : g_windowToTab)
        windows.push_back(hwnd);
    LeaveCriticalSection(&g_cs);

    for (HWND hwnd : windows) {
        // Marshal cleanup onto hwnd's own thread before removing its subclass.
        if (g_cleanupMsg != 0)
            SendMessageW(hwnd, g_cleanupMsg, 0, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, ExplorerSubclassProc);
    }

    EnterCriticalSection(&g_cs);
    if (!g_tabShellViews.empty())
        Wh_Log(L"WARNING: %zu shell view(s) leaked at uninit", g_tabShellViews.size());
    if (!g_tabNotifyReg.empty())
        Wh_Log(L"WARNING: %zu notify registration(s) leaked at uninit", g_tabNotifyReg.size());
    g_windowToTab.clear();
    g_tabShellViews.clear();       // should already be empty via the cleanup messages above
    g_tabNotifyReg.clear();        // should already be empty via the cleanup messages above
    g_tabWidthCache.clear();
    g_pendingTriggerKind.clear();
    for (auto& [hwnd, pidl] : g_tabFolderPidl)
        ILFree(pidl);
    g_tabFolderPidl.clear();
    LeaveCriticalSection(&g_cs);

    // Reset outside g_cs so a surviving FitContext's COM release (if any) never happens while holding a lock other threads block on.
    g_fitContexts.reset();

    DeleteCriticalSection(&g_settingsCs);
    DeleteCriticalSection(&g_cs);
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    EnterCriticalSection(&g_cs);
    g_tabWidthCache.clear();  // fit mode or scan settings may have changed
    LeaveCriticalSection(&g_cs);

    Settings s = GetCachedSettings();
    Wh_Log(L"SettingsChanged — delay: %ums, fit mode: %d, max scan items: %d",
           s.delayMs, static_cast<int>(s.fitMode), s.maxScanItems);
}
