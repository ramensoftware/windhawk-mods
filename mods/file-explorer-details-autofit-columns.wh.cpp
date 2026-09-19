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
- `Scan Entire Folder` measures every item in the folder instead, so nothing is left truncated after opening, navigating to, or explicitly refreshing a folder. (Known limitation: scanning the folder adds its own delay on top of the Refresh Delay setting, so fitting can take noticeably longer in large folders.)
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
    $description: "If a folder has more items than this, skip the exhaustive scan to avoid a delay: Scan Entire Folder falls back to Visible Rows Only, and Elastic still stretches the Name column using native auto-fit results for the other columns. Very large values add wall-clock overhead from the scan itself, independent of how fast each item measures."
  $name: Fit Mode Settings
  $description: Controls how columns are measured and sized.
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <shobjidl.h>
#include <propsys.h>
#include <propvarutil.h>
#include <propkey.h>
#include <shlobj.h>
#include <shlguid.h>
#include <exdisp.h>
#include <servprov.h>
#include <commctrl.h>
#include <oleauto.h>
#include <uiautomation.h>
#include <algorithm>
#include <atomic>
#include <memory>
#include <vector>
#include <unordered_map>
#include <optional>
#include <cwchar>
#include <windhawk_utils.h>

static constexpr UINT EXPLORER_REFRESH_CMD_1 = 0xA220;  // Ctrl+R / F5
static constexpr UINT EXPLORER_REFRESH_CMD_2 = 0x7103;  // Context menu Refresh (legacy menu)

// Derived from static-variable addresses rather than hardcoded literals, so
// this can't collide with another mod or Explorer itself using the same
// timer ID on a window this file also subclasses.
static char g_autofitTimerIdAnchor = 0;
static char g_fitstepTimerIdAnchor = 0;
static const UINT_PTR AUTOFIT_TIMER_ID = reinterpret_cast<UINT_PTR>(&g_autofitTimerIdAnchor);  // "start a fit after the configured delay"
static const UINT_PTR FITSTEP_TIMER_ID = reinterpret_cast<UINT_PTR>(&g_fitstepTimerIdAnchor);  // "advance one step of an in-progress fit"

// Window classes that receive refresh commands. ReBarWindow32/ToolbarWindow32
// are required: without them, F5/Ctrl+R/context-menu Refresh stop working --
// the toolbar Refresh button specifically only reaches CBrowserHost::Refresh below.
static const PCWSTR kSubclassTargets[] = {
    L"ShellTabWindowClass",
    L"ReBarWindow32",
    L"ToolbarWindow32",
};

// Delay between the calibration autosize step and reading its result back.
static constexpr UINT kFitStepDelayMs = 40;

// Elastic mode's verify/correct loop needs settle time for slower-relayout views.
static constexpr UINT kElasticVerifyDelayMs = 90;

// Wall-clock budget for the whole scan, spread across chunks (not one call).
// Kept at 2000ms, not lowered: slow metadata reads (audio tags) still need
// real time even when chunked, and this value already fixed a real regression.
static constexpr ULONGLONG kScanTimeBudgetMs = 2000;

// Items measured per WM_TIMER tick during a scan, so the UI thread is only
// ever blocked for one chunk's worth of GetDetailsEx calls, not the folder.
static constexpr int kScanChunkItems = 75;
static constexpr UINT kScanChunkTimerDelayMs = 1;

// Caps how long any single chunk can block the UI thread, independent of how
// much of the total budget remains -- a slow property handler could otherwise
// spend the entire remaining budget inside one chunk before yielding.
static constexpr ULONGLONG kScanChunkMaxBlockMs = 20;

// g_cs guards all global map access. Settings are three independent atomics
// (no critical section, no struct copy on every read -- see GetCachedSettings).
static CRITICAL_SECTION g_cs;

// Set at the top of Wh_ModUninit so in-flight window messages stop arming new timers mid-teardown.
static std::atomic<bool> g_unloading{false};

enum class FitMode { Visible, Full, Elastic };

struct Settings {
    UINT delayMs = 400;
    int maxScanItems = 500;
    FitMode fitMode = FitMode::Visible;
};
static std::atomic<UINT> g_delayMs{400};
static std::atomic<int> g_maxScanItems{500};
static std::atomic<FitMode> g_fitMode{FitMode::Visible};

static UINT g_shellNotifyMsg = 0;  // registered in Wh_ModInit; replaces a WM_APP-relative id
static UINT g_cleanupMsg = 0;      // registered in Wh_ModInit; marshals teardown onto the owning thread
static UINT g_runOnThreadMsg = 0;  // registered in Wh_ModInit; marshals Wh_ModAfterInit's lookup onto each window's own thread

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

// Per-window minimized state, so a restore-from-minimize WM_SIZE can be told
// apart from an ordinary resize and always re-fit regardless of Fit Mode.
static std::unordered_map<HWND, bool> g_windowWasMinimized;

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

// Priority order when merging pending triggers for the same window: an
// explicit navigate/refresh always wins; a passive content-change fit still
// beats a plain resize, but skips the exact-measurement scroll queue.
enum class TriggerKind { ResizeOnly, ChangeNotify, Full };
static std::unordered_map<HWND, TriggerKind> g_pendingTriggerKind;

static void MarkFitTrigger(HWND hwndTimer, TriggerKind newKind) {
    EnterCriticalSection(&g_cs);
    auto [it, inserted] = g_pendingTriggerKind.try_emplace(hwndTimer, newKind);
    if (!inserted && static_cast<int>(newKind) > static_cast<int>(it->second))
        it->second = newKind;
    LeaveCriticalSection(&g_cs);
}

static void ScheduleFit(HWND hwndTimer, TriggerKind kind);  // defined near IsWindowSubclassed, below

static TriggerKind ConsumeTriggerKind(HWND hwndTimer) {
    TriggerKind kind = TriggerKind::Full;
    EnterCriticalSection(&g_cs);
    auto it = g_pendingTriggerKind.find(hwndTimer);
    if (it != g_pendingTriggerKind.end()) {
        kind = it->second;
        g_pendingTriggerKind.erase(it);
    }
    LeaveCriticalSection(&g_cs);
    return kind;
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

    g_delayMs.store(delay, std::memory_order_relaxed);
    g_maxScanItems.store(maxScan, std::memory_order_relaxed);
    g_fitMode.store(mode, std::memory_order_relaxed);
}

static Settings GetCachedSettings() {
    Settings s;
    s.delayMs = g_delayMs.load(std::memory_order_relaxed);
    s.maxScanItems = g_maxScanItems.load(std::memory_order_relaxed);
    s.fitMode = g_fitMode.load(std::memory_order_relaxed);
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

// UI Automation fallback for Elastic's horizontal-scroll check when there is
// no classic SysListView32 (stock Explorer can host a DirectUIHWND instead).
// Removing this previously caused a real regression (a persistent scrollbar) -- keep it.
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

// Header label width per column, used only as a floor under the measured width.
static std::vector<int> ComputeHeaderFloors(HDC hdc, HFONT hFontHeader,
                                             const std::vector<PROPERTYKEY>& keys, UINT colCount)
{
    std::vector<int> floors(colCount, 0);
    HFONT hOldFont = static_cast<HFONT>(SelectObject(hdc, hFontHeader));
    for (UINT c = 0; c < colCount; c++) {
        IPropertyDescription* pDesc = nullptr;
        if (SUCCEEDED(PSGetPropertyDescription(keys[c], IID_PPV_ARGS(&pDesc))) && pDesc) {
            PWSTR pszHeader = nullptr;
            if (SUCCEEDED(pDesc->GetDisplayName(&pszHeader)) && pszHeader) {
                floors[c] = MeasureTextWidth(hdc, pszHeader);
                CoTaskMemFree(pszHeader);
            }
            pDesc->Release();
        }
    }
    SelectObject(hdc, hOldFont);
    return floors;
}

// Measures one item's displayed value for a column via GetDetailsEx (cached,
// fast) -- only to identify the widest item, not for the final pixel width.
// Returns -1 if the value was empty or unreadable.
static int MeasureItemColumnWidth(IShellFolder2* pFolder2, PITEMID_CHILD pidl,
                                   const PROPERTYKEY& key, HDC hdc, WCHAR* dispBuf, size_t dispBufLen)
{
    VARIANT v;
    VariantInit(&v);
    if (FAILED(pFolder2->GetDetailsEx(pidl, reinterpret_cast<const SHCOLUMNID*>(&key), &v)))
        return -1;

    // VariantToPropVariant handles both scalars and VT_ARRAY|VT_BSTR (e.g.
    // Contributing Artists) correctly, unlike a raw VARIANT->PROPVARIANT cast,
    // and PSFormatForDisplay then joins multi-value properties the same way Explorer does.
    PROPVARIANT pv;
    PropVariantInit(&pv);
    HRESULT hrConv = VariantToPropVariant(&v, &pv);
    VariantClear(&v);
    if (FAILED(hrConv))
        return -1;

    PCWSTR text = nullptr;
    if (SUCCEEDED(PSFormatForDisplay(key, pv, PDFF_DEFAULT, dispBuf, static_cast<UINT>(dispBufLen))))
        text = dispBuf;

    int width = text ? MeasureTextWidth(hdc, text) : -1;
    PropVariantClear(&pv);
    return width;
}

// Computes Name's width from leftover viewport space after other columns; never returns <= 0 (0 would mean CM_WIDTH_AUTOSIZE to the caller).
static int ComputeElasticNameWidth(HWND hwndView, HWND hwndListView, IUIAutomationElement* pListElement,
                                    const std::vector<int>& widths, UINT nameColIndex, double dpiScale)
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
    IFolderView2* pFV2 = nullptr;
    IColumnManager* pCM = nullptr;

    HWND hwndView = nullptr;
    HWND hwndListView = nullptr;
    HWND hwndHeader = nullptr;

    std::vector<PROPERTYKEY> keys;
    UINT colCount = 0;
    double dpiScale = 1.0;
    bool elasticMode = false;
    UINT nameColumnIndex = 0;  // index of PKEY_ItemNameDisplay within keys; not always 0
    int itemCount = 0;

    // Scan state (Phase::WaitScanChunk only): owned resources released once
    // the scan finishes or aborts, so a chunk never blocks the UI thread for
    // more than kScanChunkItems items at a time.
    IShellFolder2* pFolderScan = nullptr;
    HDC hdcScan = nullptr;
    HFONT hFontItemScan = nullptr;
    HGDIOBJ hOldFontScan = nullptr;  // restored into hdcScan before hFontItemScan is deleted
    std::vector<int> scanRunningMax;
    int scanIndex = 0;
    ULONGLONG scanBudgetRemainingUs = 0;  // counts down (microseconds) by time actually spent scanning, not idle time between chunks

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

    std::vector<int> maxWidths;  // final widths; only nameColumnIndex changes during elastic verify

    int elasticAttempt = 0;
    bool elasticConfirmed = false;
    static constexpr int kMaxElasticAttempts = 8;

    // Only populated when hwndListView couldn't be found (no classic
    // SysListView32 window on this build). See FindListElementViaUIA.
    IUIAutomationElement* pElasticListElement = nullptr;

    PCWSTR fastAutosizeReason = nullptr;  // static literal; only for the log line after WaitFastAutosizeSettle

    // Set around each Step_* call; checked by CleanupWindowState so a teardown
    // that re-enters mid-step (same-thread SendMessage) defers destruction
    // instead of freeing the context while a step is still using it.
    bool inStep = false;
    bool pendingDestroy = false;

    enum class Phase { WaitScanChunk, WaitFastAutosizeSettle, WaitSelectWidest, WaitExactAutosize, WaitRestoreScroll, WaitElasticVerify }
        phase = Phase::WaitSelectWidest;

    ~FitContext() {
        if (pElasticListElement) pElasticListElement->Release();
        if (pidlFocusedFallback) ILFree(pidlFocusedFallback);
        for (auto p : widestPidl)
            if (p) ILFree(p);
        if (hdcScan && hOldFontScan) SelectObject(hdcScan, hOldFontScan);
        if (hFontItemScan) DeleteObject(hFontItemScan);
        if (hdcScan) DeleteDC(hdcScan);
        if (pFolderScan) pFolderScan->Release();
        if (pCM) pCM->Release();
        if (pFV2) pFV2->Release();
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

static void Step_SelectWidest(FitContext* ctx, HWND hwndOwner);

// Releases the scan's owned GDI/COM resources. The old font is restored into
// hdcScan before hFontItemScan is deleted: DeleteObject silently fails (and
// leaks the handle) on a font still selected into a DC.
static void ReleaseScanResources(FitContext* ctx) {
    if (ctx->hdcScan && ctx->hOldFontScan) {
        SelectObject(ctx->hdcScan, ctx->hOldFontScan);
        ctx->hOldFontScan = nullptr;
    }
    if (ctx->hFontItemScan) { DeleteObject(ctx->hFontItemScan); ctx->hFontItemScan = nullptr; }
    if (ctx->hdcScan) { DeleteDC(ctx->hdcScan); ctx->hdcScan = nullptr; }
    if (ctx->pFolderScan) { ctx->pFolderScan->Release(); ctx->pFolderScan = nullptr; }
}

// Ran past the wall-clock budget (slow property handlers, network/cloud
// storage, etc.). Elastic mode still gets its Name stretch, same as the
// large-folder path below, instead of dropping to plain Visible-rows.
static void Step_AbortScan(FitContext* ctx, HWND hwndOwner) {
    ReleaseScanResources(ctx);
    Wh_Log(L"Auto-fitted %u column(s) (fallback: scan exceeded time budget)", ctx->colCount);

    if (ctx->elasticMode) {
        ApplyColumnWidthsLiteral(ctx->pCM, ctx->keys, std::vector<int>(ctx->colCount, 0));
        ctx->fastAutosizeReason = L"scan timed out";
        ctx->phase = FitContext::Phase::WaitFastAutosizeSettle;
        SetTimer(hwndOwner, FITSTEP_TIMER_ID, kFitStepDelayMs, nullptr);
        return;
    }

    ApplyColumnWidthsLiteral(ctx->pCM, ctx->keys, std::vector<int>(ctx->colCount, 0));
    Step_Finalize(ctx, hwndOwner);
}

// Reads back the CM_WIDTH_AUTOSIZE results applied one settle-tick ago (same
// wait Step_ApplyExactAutosize/Step_ReadExactAndRestore use -- an immediate
// read-back can be stale) and stretches Name into the leftover space.
static void Step_ReadFastAutosizeAndStretch(FitContext* ctx, HWND hwndOwner) {
    std::vector<int> widths(ctx->colCount, 0);
    for (UINT c = 0; c < ctx->colCount; c++) {
        if (c == ctx->nameColumnIndex) continue;
        CM_COLUMNINFO ci = {};
        ci.cbSize = sizeof(ci);
        ci.dwMask = CM_MASK_WIDTH;
        if (SUCCEEDED(ctx->pCM->GetColumnInfo(ctx->keys[c], &ci)))
            widths[c] = static_cast<int>(ci.uWidth);
    }
    ctx->pElasticListElement = ctx->hwndListView ? nullptr : FindListElementViaUIA(ctx->hwndView);
    widths[ctx->nameColumnIndex] = ComputeElasticNameWidth(
        ctx->hwndView, ctx->hwndListView, ctx->pElasticListElement, widths,
        ctx->nameColumnIndex, ctx->dpiScale);
    ApplyColumnWidthsLiteral(ctx->pCM, ctx->keys, widths);
    Wh_Log(L"Auto-fitted %u column(s) (elastic, %s: name-stretch only)", ctx->colCount,
           ctx->fastAutosizeReason ? ctx->fastAutosizeReason : L"fast path");
    ctx->maxWidths = std::move(widths);
    ctx->phase = FitContext::Phase::WaitElasticVerify;
    SetTimer(hwndOwner, FITSTEP_TIMER_ID, kElasticVerifyDelayMs, nullptr);
}

// Finishes bookkeeping once every item has been scanned (or the folder was too
// small to need more than one chunk): elastic empty-column padding, then the
// scroll position to restore later, before moving on to exact measurement.
static void Step_FinishScan(FitContext* ctx, HWND hwndOwner) {
    ReleaseScanResources(ctx);

    ctx->maxWidths.assign(ctx->colCount, 0);
    if (ctx->elasticMode) {
        // Elastic needs a known fixed width per column to compute Name's leftover space --
        // CM_WIDTH_AUTOSIZE's actual result is decided by Explorer afterward and could
        // exceed what was assumed here, overflowing the row.
        int emptyColPad = static_cast<int>(20 * ctx->dpiScale);
        for (UINT c = 0; c < ctx->colCount; c++)
            if (!ctx->widestPidl[c]) ctx->maxWidths[c] = ctx->headerFloor[c] + emptyColPad;
    }

    // Capture where the user actually is before any scrolling starts, so
    // it can be restored exactly once every column has been measured.
    if (ctx->hwndListView) {
        ctx->topIndexBeforeFit = static_cast<int>(SendMessageW(ctx->hwndListView, LVM_GETTOPINDEX, 0, 0));
    } else {
        int focusIdx = -1;
        ctx->pFV2->GetFocusedItem(&focusIdx);
        if (focusIdx >= 0) {
            PITEMID_CHILD pidlFocused = nullptr;
            if (SUCCEEDED(ctx->pFV2->Item(focusIdx, &pidlFocused)) && pidlFocused) {
                ctx->pidlFocusedFallback = ILCloneChild(pidlFocused);
                CoTaskMemFree(pidlFocused);
            }
        }
    }

    Step_SelectWidest(ctx, hwndOwner);
}

// Elapsed time in microseconds via QueryPerformanceCounter. GetTickCount64's
// ~15.6ms resolution let a chunk that starts and ends inside the same tick
// cost 0 against the scan budget; this doesn't have that gap.
static ULONGLONG QpcNowUs() {
    static LARGE_INTEGER freq = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f;
    }();
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    ULONGLONG whole = static_cast<ULONGLONG>(now.QuadPart) / freq.QuadPart;
    ULONGLONG rem = static_cast<ULONGLONG>(now.QuadPart) % freq.QuadPart;
    return whole * 1000000ULL + (rem * 1000000ULL) / freq.QuadPart;
}

// Measures one chunk of items per call so the UI thread is never blocked for
// longer than kScanChunkMaxBlockMs at a time, regardless of how much budget
// remains. Only aborts once the total budget is actually exhausted.
static void Step_ScanChunk(FitContext* ctx, HWND hwndOwner) {
    WCHAR dispBuf[512];
    int end = std::min(ctx->scanIndex + kScanChunkItems, ctx->itemCount);

    ULONGLONG chunkStartUs = QpcNowUs();
    ULONGLONG chunkBlockUs = std::min<ULONGLONG>(ctx->scanBudgetRemainingUs, kScanChunkMaxBlockMs * 1000ULL);
    ULONGLONG chunkDeadlineUs = chunkStartUs + chunkBlockUs;

    int i = ctx->scanIndex;
    for (; i < end; i++) {
        if (QpcNowUs() >= chunkDeadlineUs) break;  // yield; this item is picked up again next tick

        PITEMID_CHILD pidl = nullptr;
        if (FAILED(ctx->pFV2->Item(i, &pidl)) || !pidl)
            continue;

        for (UINT c = 0; c < ctx->colCount; c++) {
            int w = MeasureItemColumnWidth(ctx->pFolderScan, pidl, ctx->keys[c],
                                            ctx->hdcScan, dispBuf, ARRAYSIZE(dispBuf));
            if (w > ctx->scanRunningMax[c]) {
                ctx->scanRunningMax[c] = w;
                if (ctx->widestPidl[c]) ILFree(ctx->widestPidl[c]);
                ctx->widestPidl[c] = ILCloneChild(pidl);
            }
        }
        CoTaskMemFree(pidl);
    }

    ULONGLONG elapsedUs = QpcNowUs() - chunkStartUs;
    ctx->scanBudgetRemainingUs = (elapsedUs >= ctx->scanBudgetRemainingUs) ? 0 : ctx->scanBudgetRemainingUs - elapsedUs;
    ctx->scanIndex = i;

    if (ctx->scanBudgetRemainingUs == 0 && ctx->scanIndex < ctx->itemCount) {
        Step_AbortScan(ctx, hwndOwner);
        return;
    }

    if (ctx->scanIndex >= ctx->itemCount) {
        Step_FinishScan(ctx, hwndOwner);
        return;
    }

    SetTimer(hwndOwner, FITSTEP_TIMER_ID, kScanChunkTimerDelayMs, nullptr);
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
                ctx->nameColumnIndex, ctx->dpiScale);
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
            ctx->nameColumnIndex, ctx->dpiScale);
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

// Looks up the context fresh each call, safe against it being erased between
// ticks. inStep/pendingDestroy close the previously-documented re-entrancy
// gap: a teardown mid-step defers its destroy instead of freeing it live.
static void AdvanceFitContext(HWND hwndOwner) {
    FitContext* ctx = nullptr;
    EnterCriticalSection(&g_cs);
    auto it = g_fitContexts->find(hwndOwner);
    if (it != g_fitContexts->end()) ctx = it->second.get();
    if (ctx) ctx->inStep = true;
    LeaveCriticalSection(&g_cs);
    if (!ctx) return;

    switch (ctx->phase) {
        case FitContext::Phase::WaitScanChunk:          Step_ScanChunk(ctx, hwndOwner); break;
        case FitContext::Phase::WaitFastAutosizeSettle: Step_ReadFastAutosizeAndStretch(ctx, hwndOwner); break;
        case FitContext::Phase::WaitSelectWidest:       Step_ApplyExactAutosize(ctx, hwndOwner); break;
        case FitContext::Phase::WaitExactAutosize:      Step_ReadExactAndRestore(ctx, hwndOwner); break;
        case FitContext::Phase::WaitRestoreScroll:      Step_FinishAfterRestore(ctx, hwndOwner); break;
        case FitContext::Phase::WaitElasticVerify:      Step_VerifyElasticFit(ctx, hwndOwner); break;
    }

    // Re-validate under the lock before touching ctx again: a normal
    // completion (Step_Finalize) may have already freed it, and touching ctx
    // without this check would be a use-after-free even in that ordinary case.
    std::unique_ptr<FitContext> ctxToDestroy;
    EnterCriticalSection(&g_cs);
    auto it2 = g_fitContexts->find(hwndOwner);
    if (it2 != g_fitContexts->end() && it2->second.get() == ctx) {
        ctx->inStep = false;
        if (ctx->pendingDestroy) {
            // The step that just ran may have re-armed FITSTEP_TIMER_ID for
            // its next tick; nothing else will kill it once ctx is gone.
            KillTimer(hwndOwner, FITSTEP_TIMER_ID);
            ctxToDestroy = std::move(it2->second);
            g_fitContexts->erase(it2);
        }
    }
    LeaveCriticalSection(&g_cs);
    // ctxToDestroy destructs here, outside the lock, if a deferred teardown was pending.
}

// Starts the Elastic verify loop, taking ownership of pFV2Owned/pCMOwned/pListElementOwned.
static void StartElasticVerify(IFolderView2* pFV2Owned, IColumnManager* pCMOwned,
                                HWND hwndOwner, HWND hwndView, HWND hwndListView, HWND hwndHeader,
                                std::vector<PROPERTYKEY> keys, UINT colCount, UINT nameColIndex,
                                double dpiScale, std::vector<int> widths,
                                IUIAutomationElement* pListElementOwned)
{
    auto ctx = std::make_unique<FitContext>();
    ctx->pFV2 = pFV2Owned;
    ctx->pCM = pCMOwned;
    ctx->hwndView = hwndView;
    ctx->hwndListView = hwndListView;
    ctx->hwndHeader = hwndHeader;
    ctx->keys = std::move(keys);
    ctx->colCount = colCount;
    ctx->nameColumnIndex = nameColIndex;
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

// Applies CM_WIDTH_AUTOSIZE immediately, then waits one settle tick before
// reading back and stretching Name (same reasoning as the exact-measurement
// path). Takes ownership of pFV2/pCM/keys. reason must be a static literal.
static void StartFastElasticAutosize(IFolderView2* pFV2, IColumnManager* pCM, std::vector<PROPERTYKEY> keys,
                                      UINT colCount, UINT nameColIndex, HWND hwndOwner, HWND hwndView,
                                      HWND hwndListView, HWND hwndHeader, double dpiScale, PCWSTR reason)
{
    ApplyColumnWidthsLiteral(pCM, keys, std::vector<int>(colCount, 0));

    auto ctx = std::make_unique<FitContext>();
    ctx->pFV2 = pFV2;
    ctx->pCM = pCM;
    ctx->hwndView = hwndView;
    ctx->hwndListView = hwndListView;
    ctx->hwndHeader = hwndHeader;
    ctx->keys = std::move(keys);
    ctx->colCount = colCount;
    ctx->nameColumnIndex = nameColIndex;
    ctx->dpiScale = dpiScale;
    ctx->elasticMode = true;
    ctx->fastAutosizeReason = reason;
    ctx->phase = FitContext::Phase::WaitFastAutosizeSettle;

    EnterCriticalSection(&g_cs);
    (*g_fitContexts)[hwndOwner] = std::move(ctx);
    LeaveCriticalSection(&g_cs);

    SetTimer(hwndOwner, FITSTEP_TIMER_ID, kFitStepDelayMs, nullptr);
}

// Applies native CM_WIDTH_AUTOSIZE to every column with no scan or scroll, then
// (Elastic only) stretches Name into the leftover space. Shared by the
// maxScanItems-exceeded and change-notification paths. Takes ownership of pFV2/pCM/keys.
static void ApplyFastAutosizeFit(IFolderView2* pFV2, IColumnManager* pCM, std::vector<PROPERTYKEY> keys,
                                  UINT colCount, UINT nameColIndex, bool elasticMode,
                                  HWND hwndOwner, HWND hwndView, HWND hwndListView, HWND hwndHeader,
                                  double dpiScale, PCWSTR reason)
{
    if (elasticMode) {
        StartFastElasticAutosize(pFV2, pCM, std::move(keys), colCount, nameColIndex,
                                  hwndOwner, hwndView, hwndListView, hwndHeader, dpiScale, reason);
        return;
    }
    ApplyColumnWidthsLiteral(pCM, keys, std::vector<int>(colCount, 0));
    Wh_Log(L"Auto-fitted %u column(s) (fallback: %s)", colCount, reason);
    pCM->Release();
    pFV2->Release();
}

// Entry point: performs a fit for pShellView, whose window messages/timers are owned by
// hwndOwner. kind==ResizeOnly hints a pure Elastic-mode window resize (cached fit, skip the
// scan); kind==ChangeNotify skips the exact-measurement scroll queue for passive folder writes.
static void StartAutoFit(IShellView* pShellView, HWND hwndOwner, TriggerKind kind) {
    Settings settings = GetCachedSettings();

    EnterCriticalSection(&g_cs);
    bool alreadyRunning = g_fitContexts->find(hwndOwner) != g_fitContexts->end();
    LeaveCriticalSection(&g_cs);
    if (alreadyRunning) {
        ScheduleFit(hwndOwner, kind);  // don't drop the hint on the retry
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

    if (kind == TriggerKind::ChangeNotify) {
        // Passive folder-content changes (downloads, syncs, extractions) skip
        // the exact-measurement scroll queue entirely -- it stays reserved
        // for an explicit navigate/refresh, which the user expects to jump.
        ApplyFastAutosizeFit(pFV2, pCM, std::move(keys), colCount, nameColIndex, elasticMode,
                              hwndOwner, hwndView, hwndListView, hwndHeader, dpiScale,
                              L"change notification");
        return;
    }

    // Fast path: a pure-resize trigger in Elastic mode only needs Name's
    // width recomputed against the new viewport -- every other column's
    // width is unchanged from the last full fit, so skip the scan entirely.
    if (kind == TriggerKind::ResizeOnly && elasticMode) {
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
                                                            nameColIndex, dpiScale);
            ApplyColumnWidthsLiteral(pCM, keys, widths);
            Wh_Log(L"Auto-fitted %u column(s) (elastic, resize)", colCount);

            StartElasticVerify(pFV2, pCM, hwndOwner, hwndView, hwndListView, hwndHeader,
                                std::move(keys), colCount, nameColIndex, dpiScale,
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
        ApplyFastAutosizeFit(pFV2, pCM, std::move(keys), colCount, nameColIndex, elasticMode,
                              hwndOwner, hwndView, hwndListView, hwndHeader, dpiScale,
                              L"large folder");
        return;
    }

    IShellFolder2* pFolder2 = nullptr;
    if (FAILED(pFV2->GetFolder(IID_PPV_ARGS(&pFolder2))) || !pFolder2) {
        pCM->Release();
        pFV2->Release();
        return;
    }

    HFONT hFontHeader = CreateFontFromWindowOrDefault(hwndHeader, dpi);
    HDC hdcScreen = GetDC(nullptr);
    HDC hdc = CreateCompatibleDC(hdcScreen);
    ReleaseDC(nullptr, hdcScreen);
    std::vector<int> headerFloor = ComputeHeaderFloors(hdc, hFontHeader, keys, colCount);
    if (hFontHeader) DeleteObject(hFontHeader);

    auto ctx = std::make_unique<FitContext>();
    ctx->pFV2 = pFV2;
    ctx->pCM = pCM;
    ctx->hwndView = hwndView;
    ctx->hwndListView = hwndListView;
    ctx->hwndHeader = hwndHeader;
    ctx->keys = std::move(keys);
    ctx->colCount = colCount;
    ctx->nameColumnIndex = nameColIndex;
    ctx->dpiScale = dpiScale;
    ctx->elasticMode = elasticMode;
    ctx->itemCount = itemCount;
    ctx->headerFloor = std::move(headerFloor);
    ctx->widestPidl.assign(colCount, nullptr);

    // Scan resources: owned by the context and released once the scan
    // finishes or aborts, since it now runs a chunk at a time across ticks.
    ctx->pFolderScan = pFolder2;
    ctx->hdcScan = hdc;
    ctx->hFontItemScan = CreateFontFromWindowOrDefault(hwndListView, dpi);
    ctx->hOldFontScan = SelectObject(ctx->hdcScan, ctx->hFontItemScan);
    ctx->scanRunningMax.assign(colCount, 0);
    ctx->scanIndex = 0;
    ctx->scanBudgetRemainingUs = kScanTimeBudgetMs * 1000ULL;
    ctx->phase = FitContext::Phase::WaitScanChunk;

    FitContext* rawCtx = ctx.get();
    EnterCriticalSection(&g_cs);
    (*g_fitContexts)[hwndOwner] = std::move(ctx);
    rawCtx->inStep = true;
    LeaveCriticalSection(&g_cs);

    Step_ScanChunk(rawCtx, hwndOwner);

    // Same re-validation AdvanceFitContext uses: a re-entrant cleanup during
    // this first chunk defers instead of freeing, so finish that here.
    std::unique_ptr<FitContext> ctxToDestroy;
    EnterCriticalSection(&g_cs);
    auto itSelf = g_fitContexts->find(hwndOwner);
    if (itSelf != g_fitContexts->end() && itSelf->second.get() == rawCtx) {
        rawCtx->inStep = false;
        if (rawCtx->pendingDestroy) {
            KillTimer(hwndOwner, FITSTEP_TIMER_ID);
            ctxToDestroy = std::move(itSelf->second);
            g_fitContexts->erase(itSelf);
        }
    }
    LeaveCriticalSection(&g_cs);
}

// Finds the ShellTabWindowClass ancestor of a HWND. GetAncestor(GA_PARENT) is used rather
// than GetParent, which returns the owner (not the parent) for top-level windows.
static HWND FindTabWindow(HWND hwnd) {
    HWND cur = hwnd;
    while (cur) {
        WCHAR cls[64] = {};  // cleared each iteration: a failed GetClassNameW must not leave a stale match
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

// Centralizes the unloading check before arming a fit-trigger timer, and
// requires the target to still be subclassed so a stale/destroyed window
// can't get a timer armed on it. Used by every trigger site.
static void ScheduleFit(HWND hwndTimer, TriggerKind kind) {
    if (!IsWindowSubclassed(hwndTimer)) return;
    MarkFitTrigger(hwndTimer, kind);
    if (!g_unloading.load(std::memory_order_relaxed))
        SetTimer(hwndTimer, AUTOFIT_TIMER_ID, GetCachedSettings().delayMs, nullptr);
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
    // A UIActivate call already in flight on an Explorer thread during
    // Wh_ModUninit could otherwise install a fresh subclass after the
    // unsubscribe loop has already run, outliving the DLL.
    if (g_unloading.load(std::memory_order_relaxed)) return;

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
    if (!hwndTab) {
        if (pidl) CoTaskMemFree(pidl);
        return;
    }

    // Deregister the old folder's notification unconditionally, even if the
    // new pidl couldn't be resolved -- otherwise the tab keeps reacting to
    // changes in the folder it navigated away from.
    EnterCriticalSection(&g_cs);
    auto it = g_tabNotifyReg.find(hwndTab);
    ULONG oldReg = (it != g_tabNotifyReg.end()) ? it->second : 0;
    if (it != g_tabNotifyReg.end())
        g_tabNotifyReg.erase(it);
    LeaveCriticalSection(&g_cs);

    if (oldReg)
        SHChangeNotifyDeregister(oldReg);

    if (!pidl || g_shellNotifyMsg == 0) {
        if (pidl) CoTaskMemFree(pidl);
        // The cached folder pidl would otherwise misreport a later
        // navigation back to it as "unchanged" and skip the fit.
        EnterCriticalSection(&g_cs);
        if (auto itPidl = g_tabFolderPidl.find(hwndTab); itPidl != g_tabFolderPidl.end()) {
            ILFree(itPidl->second);
            g_tabFolderPidl.erase(itPidl);
        }
        LeaveCriticalSection(&g_cs);
        return;
    }

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

// Swaps in pShellView for hwndTab, releasing any previous entry outside the
// lock. Returns true if this is a different view than was tracked before.
static bool TrackShellView(HWND hwndTab, IShellView* pShellView) {
    IShellView* pOld = nullptr;
    EnterCriticalSection(&g_cs);
    if (auto it = g_tabShellViews.find(hwndTab); it != g_tabShellViews.end())
        pOld = it->second;
    bool changed = (pOld != pShellView);
    pShellView->AddRef();
    g_tabShellViews[hwndTab] = pShellView;
    LeaveCriticalSection(&g_cs);
    if (pOld) pOld->Release();  // outside the lock -- can run arbitrary shell teardown
    return changed;
}

// Replaces hwndTab's cached folder pidl with an owned clone of pidl (caller
// still owns pidl itself), freeing whatever was there before.
static void TrackFolderPidl(HWND hwndTab, PIDLIST_ABSOLUTE pidl) {
    if (!pidl) return;
    PIDLIST_ABSOLUTE ownedCopy = ILCloneFull(pidl);
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

HRESULT __thiscall CDefView_UIActivate_hook(void* pThis, UINT uState) {
    HRESULT hr = CDefView_UIActivate_orig(pThis, uState);

    if (SUCCEEDED(hr) &&
        (uState == SVUIA_ACTIVATE_FOCUS || uState == SVUIA_ACTIVATE_NOFOCUS)) {

        // QI through IUnknown rather than reinterpret_cast<IShellView*>(pThis):
        // layout-independent even if IShellView isn't CDefView's primary base.
        IShellView* pShellView = nullptr;
        reinterpret_cast<IUnknown*>(pThis)->QueryInterface(IID_PPV_ARGS(&pShellView));
        if (!pShellView) return hr;

        HWND hwndView = nullptr;
        pShellView->GetWindow(&hwndView);
        HWND hwndTab = hwndView ? FindTabWindow(hwndView) : nullptr;

        if (hwndView && hwndTab) {
            if (g_unloading.load(std::memory_order_relaxed)) { pShellView->Release(); return hr; }

            HWND hwndTop = GetAncestor(hwndView, GA_ROOT);

            bool viewChanged = TrackShellView(hwndTab, pShellView);

            if (viewChanged) {
                // Drop any fit still in flight for this tab: it's stepping
                // against a now-detached view and would cache the wrong widths.
                std::unique_ptr<FitContext> staleCtx;
                EnterCriticalSection(&g_cs);
                if (auto it = g_fitContexts->find(hwndTab); it != g_fitContexts->end()) {
                    if (it->second->inStep) {
                        // Same re-entrancy hazard as CleanupWindowState: don't
                        // free a context a step is still using higher up the stack.
                        it->second->pendingDestroy = true;
                    } else {
                        KillTimer(hwndTab, FITSTEP_TIMER_ID);
                        staleCtx = std::move(it->second);
                        g_fitContexts->erase(it);
                    }
                }
                // A new view means any cached widths belong to whatever was
                // there before; otherwise a WM_SIZE landing mid-fit could
                // apply the previous folder's widths via the resize fast path.
                g_tabWidthCache.erase(hwndTab);
                LeaveCriticalSection(&g_cs);
            }

            // Re-track before the sameFolder check: a tab switch/close can leave
            // the frame/toolbar mapped to a now-dead tab even when this tab's
            // own folder hasn't changed, and only this re-resolves them.
            EnumChildData data = { hwndTab };
            EnumChildWindows(hwndTop, SubclassChildProc, reinterpret_cast<LPARAM>(&data));
            TrackAndSubclass(hwndTop, hwndTab);
            TrackAndSubclass(hwndView, hwndTab);

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
                CoTaskMemFree(currentPidl);  // focus/tab-switch activation, folder unchanged
                // A background tab can have a stale Elastic Name width if the
                // window was resized while another tab was active; this is the
                // cheap cached fast path, no scan and no scrolling.
                if (GetCachedSettings().fitMode == FitMode::Elastic)
                    ScheduleFit(hwndTab, TriggerKind::ResizeOnly);
            } else {
                TrackFolderPidl(hwndTab, currentPidl);
                RegisterFolderChangeNotify(hwndTab, currentPidl);  // takes ownership
                ScheduleFit(hwndTab, TriggerKind::Full);
            }
        }
        pShellView->Release();
    }
    return hr;
}

using CDefView_Refresh_t = HRESULT(__thiscall*)(void* pThis);
CDefView_Refresh_t CDefView_Refresh_orig = nullptr;

HRESULT __thiscall CDefView_Refresh_hook(void* pThis) {
    HRESULT hr = CDefView_Refresh_orig(pThis);

    if (SUCCEEDED(hr)) {
        // QI through IUnknown rather than reinterpret_cast<IShellView*>(pThis),
        // same as CDefView_UIActivate_hook -- layout-independent either way.
        IShellView* pShellView = nullptr;
        reinterpret_cast<IUnknown*>(pThis)->QueryInterface(IID_PPV_ARGS(&pShellView));
        if (pShellView) {
            HWND hwndView = nullptr;
            pShellView->GetWindow(&hwndView);

            if (hwndView) {
                HWND hwndTab = FindTabWindow(hwndView);
                ScheduleFit(hwndTab ? hwndTab : hwndView, TriggerKind::Full);
            }
            pShellView->Release();
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
        ScheduleFit(hwndTab ? hwndTab : hwndView, TriggerKind::Full);
    }
    pSV->Release();
    pSB->Release();
}

using CBrowserHost_Refresh_t = HRESULT(__thiscall*)(void* pThis, long param);
CBrowserHost_Refresh_t CBrowserHost_Refresh_orig = nullptr;

// Required: the toolbar Refresh button only reliably reaches this hook, not
// CDefView::Refresh, on the tested build. Keep alongside the WM_COMMAND/F5
// detection and the ReBarWindow32/ToolbarWindow32 subclassing above -- all four paths are needed together.
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
    std::unique_ptr<FitContext> staleTabCtx;

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
        if (itCtx->second->inStep) {
            // A step is running higher up this same thread's call stack (a
            // re-entrant teardown); defer the actual destroy until it returns
            // instead of freeing the context out from under it.
            itCtx->second->pendingDestroy = true;
        } else {
            KillTimer(hwnd, FITSTEP_TIMER_ID);
            ctxToDestroy = std::move(itCtx->second);
            g_fitContexts->erase(itCtx);
        }
    }
    // On navigation the old CDefView window can be destroyed before the new
    // view's UIActivate cancels the tab's context, so a fit keyed by the tab
    // (not this hwnd) can still be stepping against this now-dead hwndView.
    if (auto itTab = g_windowToTab.find(hwnd); itTab != g_windowToTab.end() && itTab->second) {
        HWND hwndTab = itTab->second;
        if (auto itTabCtx = g_fitContexts->find(hwndTab);
            itTabCtx != g_fitContexts->end() && itTabCtx->second->hwndView == hwnd) {
            if (itTabCtx->second->inStep) {
                itTabCtx->second->pendingDestroy = true;
            } else {
                KillTimer(hwndTab, FITSTEP_TIMER_ID);
                // Same unique_ptr can't hold two contexts -- release this one
                // right after the lock if the primary slot above is unused.
                if (!ctxToDestroy) {
                    ctxToDestroy = std::move(itTabCtx->second);
                } else {
                    staleTabCtx = std::move(itTabCtx->second);
                }
                g_fitContexts->erase(itTabCtx);
            }
        }
    }
    g_tabWidthCache.erase(hwnd);
    g_pendingTriggerKind.erase(hwnd);
    g_windowWasMinimized.erase(hwnd);
    if (auto itPidl = g_tabFolderPidl.find(hwnd); itPidl != g_tabFolderPidl.end()) {
        ILFree(itPidl->second);
        g_tabFolderPidl.erase(itPidl);
    }
    LeaveCriticalSection(&g_cs);

    bool wasTabWindow = (pSVToRelease != nullptr);
    if (pSVToRelease) pSVToRelease->Release();
    if (regToDeregister) SHChangeNotifyDeregister(regToDeregister);

    // Only drop the cached UIA client when the tab itself is torn down, not
    // for incidental toolbar/rebar teardowns on the same thread.
    if (wasTabWindow)
        ReleaseThreadAutomation();
}

static LRESULT CALLBACK ExplorerSubclassProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    DWORD_PTR dwRefData)
{
    bool isFullRefresh = false;
    bool isChangeNotify = false;
    bool isResizeRefresh = false;

    if (uMsg == WM_COMMAND) {
        WORD cmdId = LOWORD(wParam);
        if (cmdId == EXPLORER_REFRESH_CMD_1 || cmdId == EXPLORER_REFRESH_CMD_2)
            isFullRefresh = true;
    }

    if (uMsg == WM_KEYDOWN && wParam == VK_F5)
        isFullRefresh = true;

    // Shell-level change notification for this tab's folder: a passive write
    // (download, sync, build, extraction), not an explicit user navigate or
    // refresh, so it gets its own, lighter trigger kind (see StartAutoFit).
    if (g_shellNotifyMsg != 0 && uMsg == g_shellNotifyMsg) {
        PIDLIST_ABSOLUTE* rgpidl = nullptr;
        LONG lEvent = 0;  // required out-param; the event itself isn't filtered on
        HANDLE hLock = SHChangeNotification_Lock(
            reinterpret_cast<HANDLE>(wParam), static_cast<DWORD>(lParam),
            &rgpidl, &lEvent);
        if (hLock) {
            SHChangeNotification_Unlock(hLock);
            isChangeNotify = true;
        }
    }

    // Restoring from minimize should re-fit in every mode (1.0.0 always did,
    // via UIActivate); WM_SIZE(SIZE_MINIMIZED) itself must not, since its
    // empty client rect would otherwise collapse Name to CM_WIDTH_AUTOSIZE.
    if (uMsg == WM_SIZE) {
        bool wasMinimized = false;
        EnterCriticalSection(&g_cs);
        auto itMin = g_windowWasMinimized.find(hwnd);
        wasMinimized = (itMin != g_windowWasMinimized.end() && itMin->second);
        g_windowWasMinimized[hwnd] = (wParam == SIZE_MINIMIZED);
        LeaveCriticalSection(&g_cs);

        if (wParam != SIZE_MINIMIZED) {
            if (wasMinimized)
                isFullRefresh = true;
            else if (GetCachedSettings().fitMode == FitMode::Elastic)
                isResizeRefresh = true;
        }
    }

    if (isFullRefresh || isChangeNotify || isResizeRefresh) {
        HWND hwndTab = nullptr;
        {
            EnterCriticalSection(&g_cs);
            auto it = g_windowToTab.find(hwnd);
            if (it != g_windowToTab.end())
                hwndTab = it->second;
            LeaveCriticalSection(&g_cs);
        }

        HWND hwndTimer = hwndTab ? hwndTab : hwnd;
        TriggerKind kind = isFullRefresh    ? TriggerKind::Full
                          : isChangeNotify  ? TriggerKind::ChangeNotify
                                            : TriggerKind::ResizeOnly;
        ScheduleFit(hwndTimer, kind);
    }

    if (uMsg == WM_TIMER && wParam == AUTOFIT_TIMER_ID) {
        KillTimer(hwnd, AUTOFIT_TIMER_ID);
        TriggerKind kind = ConsumeTriggerKind(hwnd);

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
            StartAutoFit(pSV, hwnd, kind);
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
        // Frame/rebar/toolbar entries point AT a tab by value. Null the
        // association instead of erasing: the window (e.g. the frame) is
        // often still alive and subclassed, and Wh_ModUninit needs it in the map to remove that subclass.
        for (auto& kv : g_windowToTab)
            if (kv.second == hwnd) kv.second = nullptr;
        LeaveCriticalSection(&g_cs);
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    InitializeCriticalSection(&g_cs);
    LoadSettings();

    g_shellNotifyMsg = RegisterWindowMessageW(L"WindhawkFileExplorerDetailsAutoFitColumns_ShellNotify");
    g_cleanupMsg = RegisterWindowMessageW(L"WindhawkFileExplorerDetailsAutoFitColumns_Cleanup");
    g_runOnThreadMsg = RegisterWindowMessageW(L"WindhawkFileExplorerDetailsAutoFitColumns_RunOnThread");

    // shell32.dll and explorerframe.dll are already loaded in explorer.exe by the time hooks apply.
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32)
        hShell32 = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
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

// Tracks and fits a shell view discovered by enumeration (Wh_ModAfterInit)
// rather than via the UIActivate hook. Must run on hwndView's own thread --
// see the cross-apartment warning on TrackOpenViewsOnThisThread below.
static void TrackAndFitDiscoveredShellView(IShellView* pShellView) {
    if (!pShellView || g_unloading.load(std::memory_order_relaxed)) return;

    HWND hwndView = nullptr;
    pShellView->GetWindow(&hwndView);
    HWND hwndTab = hwndView ? FindTabWindow(hwndView) : nullptr;
    if (!hwndView || !hwndTab) return;

    HWND hwndTop = GetAncestor(hwndView, GA_ROOT);

    TrackShellView(hwndTab, pShellView);  // no prior entry to compare against here

    EnumChildData data = { hwndTab };
    EnumChildWindows(hwndTop, SubclassChildProc, reinterpret_cast<LPARAM>(&data));
    TrackAndSubclass(hwndTop, hwndTab);
    TrackAndSubclass(hwndView, hwndTab);

    PIDLIST_ABSOLUTE currentPidl = GetShellViewFolderPidl(pShellView);
    TrackFolderPidl(hwndTab, currentPidl);
    RegisterFolderChangeNotify(hwndTab, currentPidl);  // takes ownership
    ScheduleFit(hwndTab, TriggerKind::Full);
}

// Runs fn(param) synchronously on the thread that owns hwnd, via the same
// hook-based marshaling SetWindowSubclassFromAnyThread uses -- SendMessage's
// cross-thread delivery is itself the synchronization. hwnd must belong to this process.
struct RunOnThreadWork { void (*fn)(void*); void* param; };

// Not thread_local: this hook fires on the target thread, not the caller's.
// Safe as a plain static since calls are made serially -- SendMessage blocks
// until the target thread's hook has already run.
static RunOnThreadWork* g_pRunOnThreadWork = nullptr;

static LRESULT CALLBACK RunOnThreadHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        auto* p = reinterpret_cast<CWPSTRUCT*>(lParam);
        if (p->message == g_runOnThreadMsg && g_pRunOnThreadWork)
            g_pRunOnThreadWork->fn(g_pRunOnThreadWork->param);
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static void RunFromWindowThread(HWND hwnd, void (*fn)(void*), void* param) {
    DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);
    if (!threadId) return;
    if (threadId == GetCurrentThreadId()) { fn(param); return; }

    RunOnThreadWork work = { fn, param };
    g_pRunOnThreadWork = &work;
    HHOOK hHook = SetWindowsHookExW(WH_CALLWNDPROC, RunOnThreadHookProc, nullptr, threadId);
    if (hHook) {
        SendMessageW(hwnd, g_runOnThreadMsg, 0, 0);
        UnhookWindowsHookEx(hHook);
    }
    g_pRunOnThreadWork = nullptr;
}

static BOOL CALLBACK CollectExplorerWndProc(HWND hwnd, LPARAM lParam) {
    WCHAR cls[64] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"CabinetWClass") == 0) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == GetCurrentProcessId())
            reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hwnd);
    }
    return TRUE;
}

// Windows of this process's CabinetWClass (Explorer frame) currently open.
static std::vector<HWND> GetFileExplorerWnds() {
    std::vector<HWND> result;
    EnumWindows(CollectExplorerWndProc, reinterpret_cast<LPARAM>(&result));
    return result;
}

// Runs entirely on one Explorer frame's own thread (via RunFromWindowThread).
// IShellWindows hands back cross-apartment proxies for tabs on other threads,
// so this only keeps results whose owning thread matches the one it runs on.
static void TrackOpenViewsOnThisThread(void* /*param*/) {
    IShellWindows* pShellWindows = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER,
                                 IID_PPV_ARGS(&pShellWindows))) || !pShellWindows)
        return;

    DWORD thisThreadId = GetCurrentThreadId();
    long count = 0;
    pShellWindows->get_Count(&count);
    for (long i = 0; i < count; i++) {
        VARIANT vi = {};
        vi.vt = VT_I4;
        vi.lVal = i;
        IDispatch* pDisp = nullptr;
        if (FAILED(pShellWindows->Item(vi, &pDisp)) || !pDisp)
            continue;

        IServiceProvider* pSP = nullptr;
        if (SUCCEEDED(pDisp->QueryInterface(IID_PPV_ARGS(&pSP))) && pSP) {
            IShellBrowser* pSB = nullptr;
            if (SUCCEEDED(pSP->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&pSB))) && pSB) {
                HWND hwndTab = nullptr;
                DWORD tabThreadId = 0;
                if (SUCCEEDED(pSB->GetWindow(&hwndTab)) && hwndTab)
                    tabThreadId = GetWindowThreadProcessId(hwndTab, nullptr);

                if (tabThreadId == thisThreadId) {
                    IShellView* pSV = nullptr;
                    if (SUCCEEDED(pSB->QueryActiveShellView(&pSV)) && pSV) {
                        TrackAndFitDiscoveredShellView(pSV);  // direct pointer: safe to retain
                        pSV->Release();
                    }
                }
                pSB->Release();
            }
            pSP->Release();
        }
        pDisp->Release();
    }
    pShellWindows->Release();
}

// Enabling the mod mid-session left every already-open Explorer window
// untracked until its next UIActivate, so even F5 did nothing until then.
// Each frame's own thread does its own lookup, so it gets direct pointers, not cross-apartment proxies.
void Wh_ModAfterInit() {
    for (HWND hwnd : GetFileExplorerWnds())
        RunFromWindowThread(hwnd, TrackOpenViewsOnThisThread, nullptr);
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

    // Populated independently of subclassing succeeding, so a tab whose
    // subclass failed never got a cleanup message and would otherwise leak
    // its shell view reference and live SHChangeNotifyRegister handle here.
    std::vector<IShellView*> leftoverViews;
    std::vector<ULONG> leftoverRegs;
    EnterCriticalSection(&g_cs);
    for (auto& [hwnd, pSV] : g_tabShellViews)
        if (pSV) leftoverViews.push_back(pSV);
    for (auto& [hwnd, reg] : g_tabNotifyReg)
        if (reg) leftoverRegs.push_back(reg);
    if (!leftoverViews.empty() || !leftoverRegs.empty())
        Wh_Log(L"Cleaning up %zu shell view(s), %zu notify registration(s) at uninit",
               leftoverViews.size(), leftoverRegs.size());
    g_windowToTab.clear();
    g_tabShellViews.clear();
    g_tabNotifyReg.clear();
    g_tabWidthCache.clear();
    g_pendingTriggerKind.clear();
    g_windowWasMinimized.clear();
    for (auto& [hwnd, pidl] : g_tabFolderPidl)
        ILFree(pidl);
    g_tabFolderPidl.clear();
    LeaveCriticalSection(&g_cs);

    // Released/deregistered outside the lock, same as everywhere else in this
    // file that can run arbitrary shell teardown.
    for (IShellView* pSV : leftoverViews) pSV->Release();
    for (ULONG reg : leftoverRegs) SHChangeNotifyDeregister(reg);

    // A context whose step was mid-flight when cleanup ran is deferred, not
    // freed (see inStep/pendingDestroy). Wait unbounded, not capped: every
    // step is itself time-bounded, so this always terminates, and giving up early would free a context a step still uses.
    for (;;) {
        EnterCriticalSection(&g_cs);
        bool busy = !g_fitContexts->empty();
        LeaveCriticalSection(&g_cs);
        if (!busy) break;
        Sleep(5);
    }

    // Reset outside g_cs so a surviving FitContext's COM release (if any) never happens while holding a lock other threads block on.
    g_fitContexts.reset();

    // g_cs is intentionally never deleted: a step that outlasted the wait
    // above could still (rarely) EnterCriticalSection after this returns,
    // and a plain CRITICAL_SECTION global has no destructor to race with.
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    std::vector<HWND> tabs;
    EnterCriticalSection(&g_cs);
    g_tabWidthCache.clear();  // fit mode or scan settings may have changed
    for (auto& [hwndTab, pSV] : g_tabShellViews)
        if (pSV) tabs.push_back(hwndTab);
    LeaveCriticalSection(&g_cs);

    // Otherwise switching Fit Mode (or leaving Elastic) looks like nothing
    // happened until the next unrelated trigger fires for that tab.
    for (HWND hwndTab : tabs)
        ScheduleFit(hwndTab, TriggerKind::Full);

    Settings s = GetCachedSettings();
    Wh_Log(L"SettingsChanged — delay: %ums, fit mode: %d, max scan items: %d",
           s.delayMs, static_cast<int>(s.fitMode), s.maxScanItems);
}
