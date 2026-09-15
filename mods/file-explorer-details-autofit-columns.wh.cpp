// ==WindhawkMod==
// @id              file-explorer-details-autofit-columns
// @name            File Explorer Details Auto-Fit Columns
// @description     Automatically fits all column widths to their content when refreshing in Details view. Has no effect on other view modes.
// @version         1.1.0
// @author          Armaninyow
// @github          https://github.com/armaninyow
// @include         explorer.exe
// @compilerOptions -lole32 -lpropsys -lshell32 -lcomctl32 -lgdi32
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
  - maxScanItems: 100
    $name: Max Items to Scan
    $description: "If a folder has more items than this, fall back to Visible Rows Only for that folder to avoid a delay. Applies when Fit Mode is set to Scan Entire Folder or Elastic."
  - maxColumnWidth: 800
    $name: Max Column Width (px)
    $description: "Upper bound on any single fitted column's width, at 96 DPI (scaled with display scaling). Does not apply to the Name column in Elastic mode, which is already bounded by the window width."
  $name: Fit Mode Settings
  $description: Controls how columns are measured and sized.
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <shobjidl.h>
#include <propsys.h>
#include <shlobj.h>
#include <shlguid.h>
#include <servprov.h>
#include <commctrl.h>
#include <uiautomation.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <optional>
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
#define EXPLORER_REFRESH_CMD_2 0x7103  // Context menu Refresh (legacy menu)
#define AUTOFIT_TIMER_ID       0xAF17  // "Something changed, start a fit after the configured delay"
#define FITSTEP_TIMER_ID       0xAF18  // "Advance one step of an in-progress full/elastic fit"

// Window classes that receive refresh commands.
static const PCWSTR kSubclassTargets[] = {
    L"ShellTabWindowClass",
    L"ReBarWindow32",
    L"ToolbarWindow32",
};

// Delay between steps of the timer-driven fit state machine below.
static constexpr UINT kFitStepDelayMs = 40;

// Elastic mode's verify/correct loop needs extra settle time for slower-relayout views.
static constexpr UINT kElasticVerifyDelayMs = 90;

// Thread safety: g_cs guards all global map access; g_settingsCs guards the
// small cached-settings snapshot below.

static CRITICAL_SECTION g_cs;
static CRITICAL_SECTION g_settingsCs;

enum class FitMode { Visible, Full, Elastic };

struct Settings {
    UINT delayMs = 400;
    int maxScanItems = 100;
    FitMode fitMode = FitMode::Visible;
    int maxColumnWidthDips = 800;
};
static Settings g_settings;

static UINT g_shellNotifyMsg = 0;  // registered in Wh_ModInit; replaces a WM_APP-relative id
static UINT g_cleanupMsg = 0;      // registered in Wh_ModInit; marshals teardown onto the owning thread

// Maps: tab -> IShellView, subclassed window -> tab, tab -> notify reg, tab -> in-progress fit.

struct FitContext;

static std::unordered_map<HWND, IShellView*> g_tabShellViews;
static std::unordered_map<HWND, HWND>        g_windowToTab;
static std::unordered_map<HWND, ULONG>       g_tabNotifyReg;

// Holds thread-affine COM objects; must not auto-destroy at process shutdown (Explorer can terminate without calling Wh_ModUninit).
[[clang::no_destroy]] std::optional<std::unordered_map<HWND, std::unique_ptr<FitContext>>>
    g_fitContexts{std::in_place};

// Cached settings

static void LoadSettings() {
    UINT delay = static_cast<UINT>(Wh_GetIntSetting(L"delay"));
    if (delay == 0) delay = 400;

    int maxScan = Wh_GetIntSetting(L"fitModeSettings.maxScanItems");
    if (maxScan <= 0) maxScan = 100;

    int maxColWidth = Wh_GetIntSetting(L"fitModeSettings.maxColumnWidth");
    if (maxColWidth <= 0) maxColWidth = 800;

    FitMode mode = FitMode::Visible;
    auto fitModeStr = WindhawkUtils::StringSetting::make(L"fitModeSettings.fitMode");
    if (wcscmp(fitModeStr.get(), L"full") == 0) mode = FitMode::Full;
    else if (wcscmp(fitModeStr.get(), L"elastic") == 0) mode = FitMode::Elastic;

    EnterCriticalSection(&g_settingsCs);
    g_settings.delayMs = delay;
    g_settings.maxScanItems = maxScan;
    g_settings.fitMode = mode;
    g_settings.maxColumnWidthDips = maxColWidth;
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

// Recursively finds the first descendant window matching the given class
// name, searching the whole subtree (not just direct children).
struct FindClassData { PCWSTR className; HWND result; };

static BOOL CALLBACK FindClassEnumProc(HWND hwnd, LPARAM lp) {
    auto* data = reinterpret_cast<FindClassData*>(lp);
    WCHAR cls[64] = {};
    GetClassNameW(hwnd, cls, 64);
    if (wcscmp(cls, data->className) == 0) {
        data->result = hwnd;
        return FALSE;  // stop enumeration
    }
    return TRUE;
}

static HWND FindDescendantByClass(HWND hwndRoot, PCWSTR className) {
    FindClassData data = { className, nullptr };
    EnumChildWindows(hwndRoot, FindClassEnumProc, reinterpret_cast<LPARAM>(&data));
    return data.result;
}

// UI Automation, used narrowly for Elastic mode's horizontal-scroll check, cached per thread.
static thread_local IUIAutomation* tls_pAutomation = nullptr;

static IUIAutomation* GetThreadAutomation() {
    if (!tls_pAutomation) {
        CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                          IID_IUIAutomation, reinterpret_cast<void**>(&tls_pAutomation));
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

// Timer-driven full/elastic fit state machine: no nested pump, no Sleep, per-window re-entrancy.

struct FitContext {
    IShellView* pShellView = nullptr;   // AddRef'd for the lifetime of this context
    IFolderView2* pFV2 = nullptr;       // AddRef'd (ownership transferred from QueryInterface)
    IColumnManager* pCM = nullptr;      // AddRef'd (ownership transferred from QueryInterface)

    HWND hwndView = nullptr;
    HWND hwndListView = nullptr;

    std::vector<PROPERTYKEY> keys;
    UINT colCount = 0;
    bool elasticMode = false;
    int itemCount = 0;

    UINT dpi = 96;
    double dpiScale = 1.0;
    HDC hdc = nullptr;
    HFONT hFont = nullptr;
    HFONT hOldFont = nullptr;

    PIDLIST_ABSOLUTE pidlFolder = nullptr;

    std::vector<int> maxWidths;
    std::vector<int> rawTextMax;          // widest raw (unpadded) text, all items
    std::vector<int> rawTextMaxVisible;   // widest raw text among presumed-visible items
    std::vector<bool> colExactFromScroll;
    std::vector<int> padding;
    std::vector<PITEMID_CHILD> pidlWidestForColumn;  // owns clones; may be null per column

    PITEMID_CHILD pidlRestoreTarget = nullptr;  // the item to restore focus/scroll to when done

    // Exact-measurement columns, processed one at a time (not batched, so each one's widest item is guaranteed visible for its own autosize).
    std::vector<UINT> exactColumnQueue;
    size_t exactQueueIndex = 0;

    enum class Phase {
        WaitSelectWidest,
        WaitExactAutosize,
        WaitRestoreScroll,
        WaitCalibrateAutosize,
        WaitElasticVerify,
    } phase = Phase::WaitSelectWidest;

    // Elastic mode: bounded, timer-driven correction loop (see
    // Step_VerifyElasticFit) that shrinks the Name column if the initial
    // deterministic estimate still leaves the row wider than the viewport.
    int elasticAttempt = 0;
    bool elasticConfirmed = false;
    static constexpr int kMaxElasticAttempts = 8;

    // Only populated (once, lazily) when hwndListView couldn't be found --
    // some Explorer builds host Details view with no classic
    // SysListView32 window at all. See FindListElementViaUIA.
    IUIAutomationElement* pElasticListElement = nullptr;

    ~FitContext() {
        if (pElasticListElement) pElasticListElement->Release();
        if (hOldFont && hdc) SelectObject(hdc, hOldFont);
        if (hFont) DeleteObject(hFont);
        if (hdc) DeleteDC(hdc);
        if (pidlFolder) ILFree(pidlFolder);
        if (pidlRestoreTarget) ILFree(pidlRestoreTarget);
        for (auto p : pidlWidestForColumn)
            if (p) ILFree(p);
        if (pCM) pCM->Release();
        if (pFV2) pFV2->Release();
        if (pShellView) pShellView->Release();
    }
};

static void Step_ApplyExactAutosize(FitContext* ctx, HWND hwndOwner);
static void Step_ReadExactAndRestore(FitContext* ctx, HWND hwndOwner);
static void Step_ApplyCalibration(FitContext* ctx, HWND hwndOwner);
static void Step_ReadCalibrationAndFinish(FitContext* ctx, HWND hwndOwner);
static void Step_VerifyElasticFit(FitContext* ctx, HWND hwndOwner);
static void Step_Finalize(FitContext* ctx, HWND hwndOwner);
static void ApplyAllColumnWidths(FitContext* ctx);

// Scans folder items once to seed header widths and find each column's widest item.
static void ScanItemsAndSeedColumns(FitContext* ctx) {
    int focusIdx = -1;
    ctx->pFV2->GetFocusedItem(&focusIdx);

    // Seed each column with its header label width so it's never narrower
    // than its own header plus a little room for the sort arrow.
    int sortArrowRoom = static_cast<int>(20 * ctx->dpiScale);
    for (UINT c = 0; c < ctx->colCount; c++) {
        IPropertyDescription* pDesc = nullptr;
        if (SUCCEEDED(PSGetPropertyDescription(ctx->keys[c], IID_PPV_ARGS(&pDesc))) && pDesc) {
            PWSTR pszHeader = nullptr;
            if (SUCCEEDED(pDesc->GetDisplayName(&pszHeader)) && pszHeader) {
                ctx->maxWidths[c] = MeasureTextWidth(ctx->hdc, pszHeader) + sortArrowRoom;
                CoTaskMemFree(pszHeader);
            }
            pDesc->Release();
        }
    }

    // Derive presumed visible row count from the list control's actual geometry.
    int presumedVisibleCount = 25;
    if (ctx->hwndListView) {
        RECT rcClient = {};
        GetClientRect(ctx->hwndListView, &rcClient);
        int clientHeight = rcClient.bottom - rcClient.top;
        RECT rcItem = {};
        rcItem.left = LVIR_BOUNDS;
        if (SendMessageW(ctx->hwndListView, LVM_GETITEMRECT, 0, reinterpret_cast<LPARAM>(&rcItem))) {
            int rowHeight = rcItem.bottom - rcItem.top;
            if (rowHeight > 0 && clientHeight > 0)
                presumedVisibleCount = (clientHeight / rowHeight) + 2;  // a couple extra for partial rows
        }
    }
    if (presumedVisibleCount < 10) presumedVisibleCount = 10;

    PITEMID_CHILD pidlWidestName = nullptr;
    int restoreIndex = (focusIdx >= 0) ? focusIdx : 0;

    for (int i = 0; i < ctx->itemCount; i++) {
        PITEMID_CHILD pidl = nullptr;
        if (FAILED(ctx->pFV2->Item(i, &pidl)) || !pidl)
            continue;

        if (i == restoreIndex) {
            if (ctx->pidlRestoreTarget) ILFree(ctx->pidlRestoreTarget);
            ctx->pidlRestoreTarget = ILCloneChild(pidl);
        }

        PIDLIST_ABSOLUTE pidlFull = ILCombine(ctx->pidlFolder, pidl);
        if (pidlFull) {
            IShellItem2* pItem2 = nullptr;
            if (SUCCEEDED(SHCreateItemFromIDList(pidlFull, IID_PPV_ARGS(&pItem2))) && pItem2) {
                for (UINT c = 0; c < ctx->colCount; c++) {
                    PWSTR pszDisplay = nullptr;

                    // Prefer GetProperty + PSFormatForDisplayAlloc for accurate display formatting.
                    PROPVARIANT pv;
                    PropVariantInit(&pv);
                    HRESULT hrGet = E_FAIL;
                    if (SUCCEEDED(pItem2->GetProperty(ctx->keys[c], &pv))) {
                        hrGet = PSFormatForDisplayAlloc(ctx->keys[c], pv, PDFF_DEFAULT, &pszDisplay);
                        PropVariantClear(&pv);
                    }
                    if (FAILED(hrGet))
                        hrGet = pItem2->GetString(ctx->keys[c], &pszDisplay);

                    if (pszDisplay) {
                        int w = MeasureTextWidth(ctx->hdc, pszDisplay);
                        if (w > ctx->rawTextMax[c]) {
                            ctx->rawTextMax[c] = w;
                            if (c == 0) {
                                if (pidlWidestName) ILFree(pidlWidestName);
                                pidlWidestName = ILCloneChild(pidl);
                            } else {
                                if (ctx->pidlWidestForColumn[c]) ILFree(ctx->pidlWidestForColumn[c]);
                                ctx->pidlWidestForColumn[c] = ILCloneChild(pidl);
                            }
                        }
                        if (i < presumedVisibleCount && w > ctx->rawTextMaxVisible[c])
                            ctx->rawTextMaxVisible[c] = w;
                        CoTaskMemFree(pszDisplay);
                    }
                }
                pItem2->Release();
            }
            ILFree(pidlFull);
        }
        CoTaskMemFree(pidl);
    }

    // Column 0 (Name) is measured exactly via built-in autosize, not calibrated padding.
    if (!ctx->elasticMode && pidlWidestName) {
        ctx->pidlWidestForColumn[0] = pidlWidestName;
        ctx->colExactFromScroll[0] = true;
    } else if (pidlWidestName) {
        ILFree(pidlWidestName);
    }
    for (UINT c = 1; c < ctx->colCount; c++) {
        if (ctx->pidlWidestForColumn[c])
            ctx->colExactFromScroll[c] = true;
    }
}

// Starts processing the exact-column queue one column at a time: each
// column's widest item must be the only thing scrolled into view for its
// own autosize step (see the comment on FitContext::exactColumnQueue).
static void Step_SelectWidest(FitContext* ctx, HWND hwndOwner) {
    ctx->exactColumnQueue.clear();
    for (UINT c = 0; c < ctx->colCount; c++)
        if (ctx->colExactFromScroll[c] && ctx->pidlWidestForColumn[c])
            ctx->exactColumnQueue.push_back(c);
    ctx->exactQueueIndex = 0;

    if (ctx->exactColumnQueue.empty()) {
        Step_ApplyCalibration(ctx, hwndOwner);
        return;
    }

    UINT c = ctx->exactColumnQueue[0];
    PITEMID_CHILD apidl[1] = { ctx->pidlWidestForColumn[c] };
    ctx->pFV2->SelectAndPositionItems(1, const_cast<PCUITEMID_CHILD_ARRAY>(apidl), nullptr,
        SVSI_ENSUREVISIBLE | SVSI_NOTAKEFOCUS | SVSI_NOSTATECHANGE);

    ctx->phase = FitContext::Phase::WaitSelectWidest;
    SetTimer(hwndOwner, FITSTEP_TIMER_ID, kFitStepDelayMs, nullptr);
}

// Asks the built-in autosize to measure the current queue column, now that
// its (and only its) widest item is visible.
static void Step_ApplyExactAutosize(FitContext* ctx, HWND hwndOwner) {
    UINT c = ctx->exactColumnQueue[ctx->exactQueueIndex];
    CM_COLUMNINFO ci = {};
    ci.cbSize = sizeof(ci);
    ci.dwMask = CM_MASK_WIDTH;
    ci.uWidth = CM_WIDTH_AUTOSIZE;
    ctx->pCM->SetColumnInfo(ctx->keys[c], &ci);

    ctx->phase = FitContext::Phase::WaitExactAutosize;
    SetTimer(hwndOwner, FITSTEP_TIMER_ID, kFitStepDelayMs, nullptr);
}

// Reads the current column's width, advances the queue, then restores scroll to where the user was.
static void Step_ReadExactAndRestore(FitContext* ctx, HWND hwndOwner) {
    UINT c = ctx->exactColumnQueue[ctx->exactQueueIndex];
    CM_COLUMNINFO ciResult = {};
    ciResult.cbSize = sizeof(ciResult);
    ciResult.dwMask = CM_MASK_WIDTH;
    if (SUCCEEDED(ctx->pCM->GetColumnInfo(ctx->keys[c], &ciResult)) && ciResult.uWidth > 0)
        ctx->maxWidths[c] = static_cast<int>(ciResult.uWidth);

    ctx->exactQueueIndex++;
    if (ctx->exactQueueIndex < ctx->exactColumnQueue.size()) {
        UINT nextCol = ctx->exactColumnQueue[ctx->exactQueueIndex];
        PITEMID_CHILD apidl[1] = { ctx->pidlWidestForColumn[nextCol] };
        ctx->pFV2->SelectAndPositionItems(1, const_cast<PCUITEMID_CHILD_ARRAY>(apidl), nullptr,
            SVSI_ENSUREVISIBLE | SVSI_NOTAKEFOCUS | SVSI_NOSTATECHANGE);
        ctx->phase = FitContext::Phase::WaitSelectWidest;
        SetTimer(hwndOwner, FITSTEP_TIMER_ID, kFitStepDelayMs, nullptr);
        return;
    }

    if (ctx->pidlRestoreTarget) {
        PITEMID_CHILD apidlR[1] = { ctx->pidlRestoreTarget };
        ctx->pFV2->SelectAndPositionItems(1, const_cast<PCUITEMID_CHILD_ARRAY>(apidlR), nullptr,
            SVSI_ENSUREVISIBLE | SVSI_NOTAKEFOCUS | SVSI_NOSTATECHANGE);
        ctx->phase = FitContext::Phase::WaitRestoreScroll;
        SetTimer(hwndOwner, FITSTEP_TIMER_ID, kFitStepDelayMs, nullptr);
        return;
    }

    Step_ApplyCalibration(ctx, hwndOwner);
}

// For every column that wasn't measured exactly, ask the built-in autosize
// to measure whatever happens to be visible now, so its result can be
// compared against our own raw-text measurement to derive real padding.
static void Step_ApplyCalibration(FitContext* ctx, HWND hwndOwner) {
    bool any = false;
    for (UINT c = 0; c < ctx->colCount; c++) {
        if (!ctx->colExactFromScroll[c] && !(c == 0 && ctx->elasticMode)) {
            any = true;
            CM_COLUMNINFO ci = {};
            ci.cbSize = sizeof(ci);
            ci.dwMask = CM_MASK_WIDTH;
            ci.uWidth = CM_WIDTH_AUTOSIZE;
            ctx->pCM->SetColumnInfo(ctx->keys[c], &ci);
        }
    }
    if (!any) {
        Step_ReadCalibrationAndFinish(ctx, hwndOwner);
        return;
    }
    ctx->phase = FitContext::Phase::WaitCalibrateAutosize;
    SetTimer(hwndOwner, FITSTEP_TIMER_ID, kFitStepDelayMs, nullptr);
}

// Reads back calibration widths, derives padding, computes the Elastic Name width, and finalizes.
static void Step_ReadCalibrationAndFinish(FitContext* ctx, HWND hwndOwner) {
    for (UINT c = 0; c < ctx->colCount; c++) {
        if (!ctx->colExactFromScroll[c] && !(c == 0 && ctx->elasticMode)) {
            CM_COLUMNINFO ciResult = {};
            ciResult.cbSize = sizeof(ciResult);
            ciResult.dwMask = CM_MASK_WIDTH;
            int measuredPad = (c == 0) ? static_cast<int>(48 * ctx->dpiScale) : static_cast<int>(24 * ctx->dpiScale);
            if (SUCCEEDED(ctx->pCM->GetColumnInfo(ctx->keys[c], &ciResult)) && ctx->rawTextMaxVisible[c] > 0) {
                int delta = static_cast<int>(ciResult.uWidth) - ctx->rawTextMaxVisible[c];
                if (delta > 0) measuredPad = delta;
            }
            ctx->padding[c] = measuredPad;
        }
    }

    for (UINT c = 0; c < ctx->colCount; c++) {
        int w = ctx->rawTextMax[c] + ctx->padding[c];
        if (w > ctx->maxWidths[c]) ctx->maxWidths[c] = w;
    }

    // Elastic mode: give Name whatever viewport width is left after other columns.
    if (ctx->elasticMode && ctx->colCount > 0) {
        // Some Explorer builds have no classic SysListView32; fall back to UI Automation.
        if (!ctx->hwndListView && !ctx->pElasticListElement)
            ctx->pElasticListElement = FindListElementViaUIA(ctx->hwndView);

        int viewportWidth = 0;
        bool hasVScroll = false;

        HWND hwndMeasure = ctx->hwndListView ? ctx->hwndListView : ctx->hwndView;
        if (hwndMeasure) {
            RECT rc = {};
            GetClientRect(hwndMeasure, &rc);
            viewportWidth = rc.right - rc.left;
        }
        if (ctx->hwndListView) {
            hasVScroll = (GetWindowLongPtrW(ctx->hwndListView, GWL_STYLE) & WS_VSCROLL) != 0;
        } else if (ctx->pElasticListElement) {
            // No classic control to measure -- prefer the list element's own bounding width.
            RECT rcElem = {};
            if (SUCCEEDED(ctx->pElasticListElement->get_CurrentBoundingRectangle(&rcElem))) {
                int elemWidth = rcElem.right - rcElem.left;
                if (elemWidth > 0) viewportWidth = elemWidth;
            }
        }

        int minColWidth = static_cast<int>(40 * ctx->dpiScale);
        int otherColumnsTotal = 0;
        for (UINT c = 1; c < ctx->colCount; c++) {
            if (ctx->maxWidths[c] < minColWidth) ctx->maxWidths[c] = minColWidth;
            otherColumnsTotal += ctx->maxWidths[c];
        }

        int minNameWidth = static_cast<int>(60 * ctx->dpiScale);
        if (viewportWidth > 0) {
            int scrollbarAllowance = hasVScroll ? GetSystemMetricsForDpi(SM_CXVSCROLL, ctx->dpi) : 0;
            // Small baked-in safety margin; Step_VerifyElasticFit corrects further if needed.
            int initialSafetyMargin = static_cast<int>(8 * ctx->dpiScale);
            int nameWidth = viewportWidth - otherColumnsTotal - scrollbarAllowance - initialSafetyMargin;
            if (nameWidth < minNameWidth) nameWidth = minNameWidth;
            ctx->maxWidths[0] = nameWidth;
        }
        // If the viewport width couldn't be determined, leave maxWidths[0]
        // at its seeded header width rather than guessing further.
    }

    // Guard against pathologically long content (e.g. a single very long
    // filename) blowing a column out to an unreasonable width. Name in
    // Elastic mode is exempt -- it's already bounded by the viewport above.
    int capPx = static_cast<int>(GetCachedSettings().maxColumnWidthDips * ctx->dpiScale);
    for (UINT c = 0; c < ctx->colCount; c++) {
        if (ctx->elasticMode && c == 0) continue;
        if (ctx->maxWidths[c] > capPx) ctx->maxWidths[c] = capPx;
    }

    if (ctx->elasticMode) {
        ApplyAllColumnWidths(ctx);
        ctx->elasticAttempt = 0;
        ctx->elasticConfirmed = false;
        ctx->phase = FitContext::Phase::WaitElasticVerify;
        SetTimer(hwndOwner, FITSTEP_TIMER_ID, kElasticVerifyDelayMs, nullptr);
        return;
    }

    Step_Finalize(ctx, hwndOwner);
}

// Verifies the Name width estimate against reality (header extent, scroll range, UI Automation) and shrinks it if still overflowing.
static void Step_VerifyElasticFit(FitContext* ctx, HWND hwndOwner) {
    HWND hwndHeader = ctx->hwndView ? FindDescendantByClass(ctx->hwndView, L"SysHeader32") : nullptr;
    HWND hwndMeasure = ctx->hwndListView ? ctx->hwndListView : ctx->hwndView;

    int viewportWidth = 0;
    if (hwndMeasure) {
        RECT rc = {};
        GetClientRect(hwndMeasure, &rc);
        viewportWidth = rc.right - rc.left;
    }
    if (!ctx->hwndListView && ctx->pElasticListElement) {
        // No classic control to measure -- prefer the actual list
        // element's own bounding width over the outer view's.
        RECT rcElem = {};
        if (SUCCEEDED(ctx->pElasticListElement->get_CurrentBoundingRectangle(&rcElem))) {
            int elemWidth = rcElem.right - rcElem.left;
            if (elemWidth > 0) viewportWidth = elemWidth;
        }
    }

    int overflow = 0;
    int headerExtent = -1;
    int headerOverflow = 0;

    if (hwndHeader && viewportWidth > 0) {
        int headerItemCount = static_cast<int>(SendMessageW(hwndHeader, HDM_GETITEMCOUNT, 0, 0));
        if (headerItemCount > 0) {
            RECT rcLast = {};
            if (SendMessageW(hwndHeader, HDM_GETITEMRECT, headerItemCount - 1,
                              reinterpret_cast<LPARAM>(&rcLast))) {
                headerExtent = rcLast.right;
                headerOverflow = headerExtent - viewportWidth;
                if (headerOverflow > overflow) overflow = headerOverflow;
            }
        }
    }

    int scrollLo = 0, scrollHi = 0;
    bool haveScrollRange = false;
    if (ctx->hwndListView) {
        haveScrollRange = GetScrollRange(ctx->hwndListView, SB_HORZ, &scrollLo, &scrollHi) != 0;
        if (haveScrollRange && scrollHi > scrollLo) {
            int scrollOverflow = scrollHi - scrollLo;
            if (scrollOverflow > overflow) overflow = scrollOverflow;
        }
    }

    // Only fall back to UI Automation when there's no classic list view to
    // query; the header/scroll-range checks above are authoritative when there is one.
    bool uiaSaysScrolling = false;
    if (!ctx->hwndListView && ctx->pElasticListElement)
        uiaSaysScrolling = ElementHasHorizontalScroll(ctx->pElasticListElement);

    int minNameWidth = static_cast<int>(60 * ctx->dpiScale);
    bool canShrink = ctx->maxWidths[0] > minNameWidth;
    bool haveAttemptsLeft = ctx->elasticAttempt < FitContext::kMaxElasticAttempts;

    if ((overflow > 0 || uiaSaysScrolling) && haveAttemptsLeft && canShrink) {
        int safetyMargin = static_cast<int>(10 * ctx->dpiScale);
        int shrinkBy = (overflow > 0)
            ? (overflow + safetyMargin)
            // UIA flagged scrolling but neither pixel-based check agreed,
            // so there's no precise amount to remove -- back off a fixed
            // step and let the next verify pass check again.
            : static_cast<int>(24 * ctx->dpiScale);

        int newNameWidth = ctx->maxWidths[0] - shrinkBy;
        if (newNameWidth < minNameWidth) newNameWidth = minNameWidth;

        if (newNameWidth < ctx->maxWidths[0]) {
            ctx->maxWidths[0] = newNameWidth;
            CM_COLUMNINFO ci = {};
            ci.cbSize = sizeof(ci);
            ci.dwMask = CM_MASK_WIDTH;
            ci.uWidth = static_cast<UINT>(newNameWidth);
            ctx->pCM->SetColumnInfo(ctx->keys[0], &ci);

            ctx->elasticAttempt++;
            ctx->elasticConfirmed = false;  // any change needs re-confirming
            SetTimer(hwndOwner, FITSTEP_TIMER_ID, kElasticVerifyDelayMs, nullptr);
            return;
        }
    }

    if (!ctx->elasticConfirmed && haveAttemptsLeft) {
        // Neither signal shows overflow right now, but re-check once more
        // after another settle delay before trusting it, in case this read
        // happened to land before the view finished catching up.
        ctx->elasticConfirmed = true;
        ctx->elasticAttempt++;
        SetTimer(hwndOwner, FITSTEP_TIMER_ID, kElasticVerifyDelayMs, nullptr);
        return;
    }

    Step_Finalize(ctx, hwndOwner);
}

static void ApplyAllColumnWidths(FitContext* ctx) {
    for (UINT c = 0; c < ctx->colCount; c++) {
        CM_COLUMNINFO ci = {};
        ci.cbSize = sizeof(ci);
        ci.dwMask = CM_MASK_WIDTH;
        ci.uWidth = ctx->maxWidths[c] > 0 ? static_cast<UINT>(ctx->maxWidths[c]) : CM_WIDTH_AUTOSIZE;
        ctx->pCM->SetColumnInfo(ctx->keys[c], &ci);
    }
}

static void Step_Finalize(FitContext* ctx, HWND hwndOwner) {
    ApplyAllColumnWidths(ctx);
    Wh_Log(L"Auto-fitted %u column(s) via full folder scan (%d items)", ctx->colCount, ctx->itemCount);

    std::unique_ptr<FitContext> ctxToDestroy;
    EnterCriticalSection(&g_cs);
    if (auto it = g_fitContexts->find(hwndOwner); it != g_fitContexts->end()) {
        ctxToDestroy = std::move(it->second);
        g_fitContexts->erase(it);
    }
    LeaveCriticalSection(&g_cs);
    // ctxToDestroy is destroyed here, outside the lock, on the owning thread.
}

// Called from WM_TIMER(FITSTEP_TIMER_ID) on the owning window's thread to
// advance whichever fit is in progress for that window, if any.
static void AdvanceFitContext(HWND hwndOwner) {
    FitContext* ctx = nullptr;
    EnterCriticalSection(&g_cs);
    auto it = g_fitContexts->find(hwndOwner);
    if (it != g_fitContexts->end()) ctx = it->second.get();
    LeaveCriticalSection(&g_cs);
    if (!ctx) return;

    switch (ctx->phase) {
        case FitContext::Phase::WaitSelectWidest:      Step_ApplyExactAutosize(ctx, hwndOwner); break;
        case FitContext::Phase::WaitExactAutosize:     Step_ReadExactAndRestore(ctx, hwndOwner); break;
        case FitContext::Phase::WaitRestoreScroll:     Step_ApplyCalibration(ctx, hwndOwner); break;
        case FitContext::Phase::WaitCalibrateAutosize: Step_ReadCalibrationAndFinish(ctx, hwndOwner); break;
        case FitContext::Phase::WaitElasticVerify:     Step_VerifyElasticFit(ctx, hwndOwner); break;
    }
}

// Entry point: starts (or, for Visible mode, immediately performs) a fit
// for pShellView, whose window messages/timers are owned by hwndOwner.
static void StartAutoFit(IShellView* pShellView, HWND hwndOwner) {
    Settings settings = GetCachedSettings();

    // One fit per window/tab at a time; re-arms the trigger timer instead of dropping it if one's already running.
    EnterCriticalSection(&g_cs);
    bool alreadyRunning = g_fitContexts->find(hwndOwner) != g_fitContexts->end();
    LeaveCriticalSection(&g_cs);
    if (alreadyRunning) {
        SetTimer(hwndOwner, AUTOFIT_TIMER_ID, settings.delayMs, nullptr);
        return;
    }

    IFolderView2* pFV2 = nullptr;
    if (FAILED(pShellView->QueryInterface(IID_IFolderView2_, reinterpret_cast<void**>(&pFV2))) || !pFV2)
        return;

    FOLDERVIEWMODE viewMode = FVM_AUTO;
    int iconSize = 0;
    HRESULT hr = pFV2->GetViewModeAndIconSize(&viewMode, &iconSize);
    if (FAILED(hr) || viewMode != FVM_DETAILS) {
        pFV2->Release();
        return;
    }

    IColumnManager* pCM = nullptr;
    if (FAILED(pShellView->QueryInterface(IID_IColumnManager_, reinterpret_cast<void**>(&pCM))) || !pCM) {
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
        for (UINT i = 0; i < colCount; i++) {
            CM_COLUMNINFO ci = {};
            ci.cbSize = sizeof(ci);
            ci.dwMask = CM_MASK_WIDTH;
            ci.uWidth = CM_WIDTH_AUTOSIZE;
            pCM->SetColumnInfo(keys[i], &ci);
        }
        Wh_Log(L"Auto-fitted %u column(s)", colCount);
        pCM->Release();
        pFV2->Release();
        return;
    }

    int itemCount = 0;
    HRESULT hrCount = pFV2->ItemCount(SVGIO_ALLVIEW, &itemCount);
    if (FAILED(hrCount) || itemCount > settings.maxScanItems) {
        // Folder too large (or count unavailable) for a full/elastic scan
        // this time -- fall back to the fast built-in autosize so the user
        // still gets a reasonable result without the extra delay.
        for (UINT i = 0; i < colCount; i++) {
            CM_COLUMNINFO ci = {};
            ci.cbSize = sizeof(ci);
            ci.dwMask = CM_MASK_WIDTH;
            ci.uWidth = CM_WIDTH_AUTOSIZE;
            pCM->SetColumnInfo(keys[i], &ci);
        }
        Wh_Log(L"Auto-fitted %u column(s) (fallback: visible rows only)", colCount);
        pCM->Release();
        pFV2->Release();
        return;
    }

    auto ctx = std::make_unique<FitContext>();
    pShellView->AddRef();
    ctx->pShellView = pShellView;
    ctx->pFV2 = pFV2;  // ownership of the QueryInterface AddRef transfers to ctx
    ctx->pCM = pCM;    // ownership of the QueryInterface AddRef transfers to ctx
    ctx->keys = std::move(keys);
    ctx->colCount = colCount;
    ctx->elasticMode = (settings.fitMode == FitMode::Elastic);
    ctx->itemCount = itemCount;
    ctx->maxWidths.assign(colCount, 0);
    ctx->rawTextMax.assign(colCount, 0);
    ctx->rawTextMaxVisible.assign(colCount, 0);
    ctx->colExactFromScroll.assign(colCount, false);
    ctx->padding.assign(colCount, 0);
    ctx->pidlWidestForColumn.assign(colCount, nullptr);

    pShellView->GetWindow(&ctx->hwndView);
    if (ctx->hwndView)
        ctx->hwndListView = FindDescendantByClass(ctx->hwndView, L"SysListView32");

    ctx->dpi = ctx->hwndView ? GetDpiForWindow(ctx->hwndView) : 96;
    if (ctx->dpi == 0) ctx->dpi = 96;
    ctx->dpiScale = ctx->dpi / 96.0;

    IShellFolder* pFolder = nullptr;
    if (FAILED(ctx->pFV2->GetFolder(IID_PPV_ARGS(&pFolder))) || !pFolder)
        return;  // ctx's destructor releases pShellView/pFV2/pCM; nothing registered yet
    HRESULT hrPidl = SHGetIDListFromObject(pFolder, &ctx->pidlFolder);
    pFolder->Release();
    if (FAILED(hrPidl) || !ctx->pidlFolder)
        return;

    // Prefer the real rendering font from the list/header control over SPI_* guessing.
    HWND hwndHeader = ctx->hwndView ? FindDescendantByClass(ctx->hwndView, L"SysHeader32") : nullptr;
    HWND hwndFontSource = hwndHeader ? hwndHeader : ctx->hwndListView;
    if (hwndFontSource) {
        HFONT hSrcFont = reinterpret_cast<HFONT>(SendMessageW(hwndFontSource, WM_GETFONT, 0, 0));
        if (hSrcFont) {
            LOGFONTW lfCopy = {};
            if (GetObjectW(hSrcFont, sizeof(lfCopy), &lfCopy))
                ctx->hFont = CreateFontIndirectW(&lfCopy);
        }
    }

    if (!ctx->hFont) {
        LOGFONTW lf = {};
        BOOL gotFont = SystemParametersInfoForDpi(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0, ctx->dpi);
        if (!gotFont) SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0);

        // Modern WinUI/Fluent folder views typically render with "Segoe UI Variable".
        LOGFONTW lfVariable = lf;
        wcscpy_s(lfVariable.lfFaceName, LF_FACESIZE, L"Segoe UI Variable Text");
        HFONT hTestFont = CreateFontIndirectW(&lfVariable);
        if (hTestFont) {
            HDC hdcTest = GetDC(nullptr);
            HFONT hOldTest = static_cast<HFONT>(SelectObject(hdcTest, hTestFont));
            WCHAR actualFace[LF_FACESIZE] = {};
            GetTextFaceW(hdcTest, LF_FACESIZE, actualFace);
            SelectObject(hdcTest, hOldTest);
            ReleaseDC(nullptr, hdcTest);
            if (_wcsicmp(actualFace, L"Segoe UI Variable Text") == 0) {
                ctx->hFont = hTestFont;
            } else {
                DeleteObject(hTestFont);  // font not actually available, fall through
            }
        }

        if (!ctx->hFont) ctx->hFont = CreateFontIndirectW(&lf);
    }

    HDC hdcScreen = GetDC(nullptr);
    ctx->hdc = CreateCompatibleDC(hdcScreen);
    ReleaseDC(nullptr, hdcScreen);
    if (ctx->hFont)
        ctx->hOldFont = static_cast<HFONT>(SelectObject(ctx->hdc, ctx->hFont));

    // Scanning item properties/text is bounded (<= maxScanItems, default
    // 100) and never touches selection/scroll/column state, so it's safe
    // to do inline here rather than as its own timer step.
    ScanItemsAndSeedColumns(ctx.get());

    FitContext* rawCtx = ctx.get();
    EnterCriticalSection(&g_cs);
    (*g_fitContexts)[hwndOwner] = std::move(ctx);
    LeaveCriticalSection(&g_cs);

    Step_SelectWidest(rawCtx, hwndOwner);
}

// Find the ShellTabWindowClass ancestor of a HWND

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

// True if hwnd is subclassed and will process WM_TIMER (and thus can KillTimer it).
static bool IsWindowSubclassed(HWND hwnd) {
    EnterCriticalSection(&g_cs);
    bool found = g_windowToTab.find(hwnd) != g_windowToTab.end();
    LeaveCriticalSection(&g_cs);
    return found;
}

// Check if a window class is one we want to subclass

static bool IsSubclassTarget(HWND hwnd) {
    WCHAR cls[64] = {};
    GetClassNameW(hwnd, cls, 64);
    for (PCWSTR target : kSubclassTargets) {
        if (wcscmp(cls, target) == 0)
            return true;
    }
    return false;
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
    LeaveCriticalSection(&g_cs);

    // Both of these -- and ctxToDestroy's destructor, which releases
    // pFV2/pCM -- run here, on the window's own owning thread, so this is
    // valid for the apartment-threaded COM objects involved.
    bool wasTabWindow = (pSVToRelease != nullptr);
    if (pSVToRelease) pSVToRelease->Release();
    if (regToDeregister) SHChangeNotifyDeregister(regToDeregister);

    // Only drop the cached UIA client when the tab itself is torn down, not for incidental toolbar/rebar teardowns on the same thread.
    if (wasTabWindow)
        ReleaseThreadAutomation();
}

// Subclass proc

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

    // Shell-level change notification for this tab's folder, via a registered message id.
    if (g_shellNotifyMsg != 0 && uMsg == g_shellNotifyMsg) {
        LONG lEvent = 0;
        PIDLIST_ABSOLUTE* rgpidl = nullptr;
        HANDLE hLock = SHChangeNotification_Lock(
            reinterpret_cast<HANDLE>(wParam), static_cast<DWORD>(lParam),
            &rgpidl, &lEvent);
        if (hLock) {
            SHChangeNotification_Unlock(hLock);
            isRefresh = true;
        }
    }

    // Live re-layout on resize, only in Elastic mode; settings read from cache.
    if (uMsg == WM_SIZE) {
        if (GetCachedSettings().fitMode == FitMode::Elastic) isRefresh = true;
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

        HWND hwndTimer = hwndTab ? hwndTab : hwnd;
        SetTimer(hwndTimer, AUTOFIT_TIMER_ID, GetCachedSettings().delayMs, nullptr);
    }

    if (uMsg == WM_TIMER && wParam == AUTOFIT_TIMER_ID) {
        KillTimer(hwnd, AUTOFIT_TIMER_ID);

        IShellView* pSV = nullptr;
        {
            EnterCriticalSection(&g_cs);
            auto it = g_tabShellViews.find(hwnd);
            if (it != g_tabShellViews.end() && it->second) {
                pSV = it->second;
                pSV->AddRef();  // keep alive outside the lock
            }
            LeaveCriticalSection(&g_cs);
        }

        if (pSV) {
            StartAutoFit(pSV, hwnd);
            pSV->Release();
        }
        return 0;
    }

    if (uMsg == WM_TIMER && wParam == FITSTEP_TIMER_ID) {
        KillTimer(hwnd, FITSTEP_TIMER_ID);
        AdvanceFitContext(hwnd);
        return 0;
    }

    // Marshals CleanupWindowState onto this window's own thread.
    if (g_cleanupMsg != 0 && uMsg == g_cleanupMsg) {
        CleanupWindowState(hwnd);
        return 0;
    }

    if (uMsg == WM_NCDESTROY) {
        CleanupWindowState(hwnd);

        EnterCriticalSection(&g_cs);
        g_windowToTab.erase(hwnd);
        LeaveCriticalSection(&g_cs);
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

// Subclass a window if it's a target class, updating tab association

static void SubclassTargetIfNeeded(HWND hwnd, HWND hwndTab) {
    if (!hwnd || !IsSubclassTarget(hwnd)) return;

    // A window that lives under a tab belongs to that tab -- don't let the
    // tab that happens to be activating claim other tabs in the same frame.
    if (HWND hwndOwnTab = FindTabWindow(hwnd))
        hwndTab = hwndOwnTab;

    EnterCriticalSection(&g_cs);
    bool isNew = g_windowToTab.find(hwnd) == g_windowToTab.end();
    g_windowToTab[hwnd] = hwndTab;
    LeaveCriticalSection(&g_cs);

    if (isNew)
        WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, ExplorerSubclassProc, 0);
}

// EnumChildWindows callback — only subclasses target window classes

struct EnumChildData { HWND hwndTab; };

static BOOL CALLBACK SubclassChildProc(HWND child, LPARAM lp) {
    auto* data = reinterpret_cast<EnumChildData*>(lp);
    SubclassTargetIfNeeded(child, data->hwndTab);
    return TRUE;
}

// Registers shell change notifications for this tab's current folder.

static void RegisterFolderChangeNotify(HWND hwndTab, IShellView* pShellView) {
    if (!hwndTab || !pShellView || g_shellNotifyMsg == 0) return;

    IFolderView2* pFV2 = nullptr;
    if (FAILED(pShellView->QueryInterface(IID_IFolderView2_,
                                          reinterpret_cast<void**>(&pFV2))) || !pFV2)
        return;

    IShellFolder* pShellFolder = nullptr;
    HRESULT hr = pFV2->GetFolder(IID_PPV_ARGS(&pShellFolder));
    pFV2->Release();
    if (FAILED(hr) || !pShellFolder)
        return;

    PIDLIST_ABSOLUTE pidl = nullptr;
    hr = SHGetIDListFromObject(pShellFolder, &pidl);
    pShellFolder->Release();
    if (FAILED(hr) || !pidl)
        return;

    // Deregister any previous registration for this tab
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
        SHCNRF_ShellLevel | SHCNRF_NewDelivery,
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

// Hook: CDefView::UIActivate

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
        if (!hwndTab) return hr;  // no tab ancestor -- e.g. the desktop's own CDefView; nothing to fit
        HWND hwndTop = GetAncestor(hwndView, GA_ROOT);

        // Update stored IShellView for this tab
        {
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

        // Also subclass the top-level frame window for the modern Fluent context menu.
        {
            EnterCriticalSection(&g_cs);
            bool isNewTop = g_windowToTab.find(hwndTop) == g_windowToTab.end();
            g_windowToTab[hwndTop] = hwndTab;
            LeaveCriticalSection(&g_cs);
            if (isNewTop)
                WindhawkUtils::SetWindowSubclassFromAnyThread(hwndTop, ExplorerSubclassProc, 0);
        }

        // Also subclass hwndView directly for the classic background-refresh context menu.
        {
            EnterCriticalSection(&g_cs);
            bool isNewView = g_windowToTab.find(hwndView) == g_windowToTab.end();
            g_windowToTab[hwndView] = hwndTab;
            LeaveCriticalSection(&g_cs);
            if (isNewView)
                WindhawkUtils::SetWindowSubclassFromAnyThread(hwndView, ExplorerSubclassProc, 0);
        }

        // Register for shell-level change notifications on this folder so
        // we catch refreshes triggered by the modern command bar (or any
        // other mechanism) that don't produce classic window messages.
        RegisterFolderChangeNotify(hwndTab, pShellView);

        // Auto-fit on open/navigate/tab switch
        SetTimer(hwndTab, AUTOFIT_TIMER_ID, GetCachedSettings().delayMs, nullptr);
    }
    return hr;
}

// Hook: CDefView::Refresh -- the actual method behind every refresh trigger.

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

            // hwndView is only reliably subclassed once UIActivate has run
            // for it; if activation hasn't happened yet, arming a timer
            // here would leave nothing to ever KillTimer it.
            if (IsWindowSubclassed(hwndTimer))
                SetTimer(hwndTimer, AUTOFIT_TIMER_ID, GetCachedSettings().delayMs, nullptr);
        }
    }

    return hr;
}

// Hook: CBrowserHost::Refresh (explorerframe.dll) -- backs the modern command bar's refresh button.

static void TriggerAutoFitFromShellBrowser(IUnknown* pUnk) {
    if (!pUnk) return;

    IServiceProvider* pSP = nullptr;
    if (FAILED(pUnk->QueryInterface(IID_IServiceProvider_, reinterpret_cast<void**>(&pSP))) || !pSP)
        return;

    IShellBrowser* pSB = nullptr;
    HRESULT hrQS = pSP->QueryService(SID_STopLevelBrowser, IID_IShellBrowser, reinterpret_cast<void**>(&pSB));
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

        if (IsWindowSubclassed(hwndTimer))
            SetTimer(hwndTimer, AUTOFIT_TIMER_ID, GetCachedSettings().delayMs, nullptr);
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

// Windhawk entry points

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    InitializeCriticalSection(&g_cs);
    InitializeCriticalSection(&g_settingsCs);
    LoadSettings();

    // Process-wide-unique message ids, rather than a raw WM_APP-relative
    // constant, since the windows that receive them belong to Explorer,
    // not to this mod.
    g_shellNotifyMsg = RegisterWindowMessageW(L"WindhawkFileExplorerDetailsAutoFitColumns_ShellNotify");
    g_cleanupMsg = RegisterWindowMessageW(L"WindhawkFileExplorerDetailsAutoFitColumns_Cleanup");

    // Prefer already-loaded module handles over LoadLibrary by bare name.
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
            true  // optional: extra coverage for right-click/toolbar refresh;
                  // must not prevent the mod from loading if this internal
                  // symbol name doesn't resolve on a given Explorer build
        },
    };

    if (!WindhawkUtils::HookSymbols(hShell32, shell32DllHooks, ARRAYSIZE(shell32DllHooks))) {
        Wh_Log(L"ERROR: Could not hook CDefView::UIActivate");
        DeleteCriticalSection(&g_settingsCs);
        DeleteCriticalSection(&g_cs);
        return FALSE;
    }

    // Diagnostic: confirm whether the optional Refresh hook actually
    // resolved and installed (its own required=false setting above means
    // it won't fail the whole HookSymbols call above if not found).
    Wh_Log(L"CDefView::Refresh hook installed: %s",
           CDefView_Refresh_orig ? L"yes" : L"no (optional, not fatal)");

    Wh_Log(L"CDefView::UIActivate hooked successfully");

    // Optional: explorerframe.dll hook for the modern WinUI3 command bar's
    // refresh button, which doesn't go through CDefView or produce any
    // window message we could otherwise detect.
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
                true  // optional: must not prevent the mod from loading if
                      // this internal symbol name doesn't resolve on a
                      // given Explorer build
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
    g_windowToTab.clear();
    g_tabShellViews.clear();  // already emptied via the cleanup messages above
    g_tabNotifyReg.clear();   // already emptied via the cleanup messages above
    g_fitContexts.reset();  // already emptied via the cleanup messages above; frees the map itself
    LeaveCriticalSection(&g_cs);

    DeleteCriticalSection(&g_settingsCs);
    DeleteCriticalSection(&g_cs);
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    Settings s = GetCachedSettings();
    Wh_Log(L"SettingsChanged — delay: %ums, fit mode: %d, max scan items: %d, max column width: %dpx",
           s.delayMs, static_cast<int>(s.fitMode), s.maxScanItems, s.maxColumnWidthDips);
}
