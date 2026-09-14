// ==WindhawkMod==
// @id              file-explorer-details-autofit-columns
// @name            File Explorer Details Auto-Fit Columns
// @description     Automatically fits all column widths to their content when refreshing in Details view. Has no effect on other view modes.
// @version         1.1.0
// @author          Armaninyow
// @github          https://github.com/armaninyow
// @include         explorer.exe
// @compilerOptions -lole32 -lshlwapi -lpropsys -lshell32 -lcomctl32 -loleaut32 -lgdi32
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
  $name: Fit Mode Settings
  $description: Controls how columns are measured and sized.
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <shobjidl.h>
#include <propkey.h>
#include <propsys.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shlguid.h>
#include <servprov.h>
#include <exdisp.h>
#include <vector>
#include <unordered_map>
#include <windhawk_utils.h>
#include <uiautomation.h>

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
#define AUTOFIT_TIMER_ID       0xAF17
#define WM_AUTOFIT_SHELLNOTIFY (WM_APP + 0x745)

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
static std::unordered_map<HWND, ULONG>       g_tabNotifyReg; // hwndTab -> SHChangeNotifyRegister id

// ---------------------------------------------------------------------------
// Auto-fit all visible columns to content
// ---------------------------------------------------------------------------

static int MeasureTextWidth(HDC hdc, PCWSTR text) {
    if (!text || !*text) return 0;
    SIZE sz = {0, 0};
    GetTextExtentPoint32W(hdc, text, static_cast<int>(wcslen(text)), &sz);
    return sz.cx;
}

// Scans every item in the folder (not just currently-rendered rows) and
// computes an exact column width from the real content, so long filenames
// further down an unscrolled list are never left truncated. More work per
// refresh than the built-in visible-rows autosize, so callers should cap
// this to reasonably-sized folders.
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

// Scrolls the given item into view, asks the built-in autosize to measure
// the given column with it visible, and returns the exact resulting
// width. Used to get a truly precise width for any column whose real
// widest content might not be in the presumed-visible calibration set,
// without guessing at padding.
// Checks whether the given window currently needs horizontal scrolling,
// via UI Automation's ScrollPattern — this works across UI frameworks
// (classic Win32, WinUI/XAML, etc.) unlike GetScrollInfo, which only
// reflects classic scrollbar state and returns nothing useful for this
// system's WinUI-based folder view.
static bool ViewNeedsHorizontalScroll(HWND hwndView, bool& outDetected) {
    outDetected = false;
    if (!hwndView) return false;

    IUIAutomation* pAutomation = nullptr;
    if (FAILED(CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                                 IID_PPV_ARGS(&pAutomation))) || !pAutomation)
        return false;

    IUIAutomationElement* pRoot = nullptr;
    bool result = false;
    if (SUCCEEDED(pAutomation->ElementFromHandle(hwndView, &pRoot)) && pRoot) {
        IUIAutomationElement* pScrollable = nullptr;

        // Try the root element itself first.
        VARIANT varTrue;
        varTrue.vt = VT_BOOL;
        varTrue.boolVal = VARIANT_TRUE;
        IUIAutomationCondition* pCondition = nullptr;
        if (SUCCEEDED(pAutomation->CreatePropertyCondition(
                UIA_IsScrollPatternAvailablePropertyId, varTrue, &pCondition)) && pCondition) {
            pRoot->FindFirst(TreeScope_Element, pCondition, &pScrollable);
            if (!pScrollable) {
                pRoot->FindFirst(TreeScope_Descendants, pCondition, &pScrollable);
            }
            pCondition->Release();
        }

        IUIAutomationElement* pTarget = pScrollable ? pScrollable : pRoot;
        IUIAutomationScrollPattern* pScrollPattern = nullptr;
        if (SUCCEEDED(pTarget->GetCurrentPatternAs(
                UIA_ScrollPatternId, IID_PPV_ARGS(&pScrollPattern))) && pScrollPattern) {
            BOOL horizontallyScrollable = FALSE;
            if (SUCCEEDED(pScrollPattern->get_CurrentHorizontallyScrollable(&horizontallyScrollable))) {
                outDetected = true;
                result = horizontallyScrollable != FALSE;
            }
            pScrollPattern->Release();
        }
        if (pScrollable) pScrollable->Release();
        pRoot->Release();
    }
    pAutomation->Release();
    return result;
}

static bool ScrollAndMeasureColumn(IFolderView2* pFV2, IColumnManager* pCM,
                                    const PROPERTYKEY& key, PITEMID_CHILD pidlTarget,
                                    PITEMID_CHILD pidlRestore,
                                    UINT colIndex, int& outWidth) {
    (void)colIndex;
    if (!pidlTarget) return false;

    PITEMID_CHILD apidl[1] = { pidlTarget };
    pFV2->SelectAndPositionItems(1, const_cast<PCUITEMID_CHILD_ARRAY>(apidl), nullptr,
                                  SVSI_ENSUREVISIBLE | SVSI_NOTAKEFOCUS | SVSI_NOSTATECHANGE);

    MSG msg;
    for (int p = 0; p < 30; p++) {
        if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        } else {
            Sleep(2);
        }
    }

    CM_COLUMNINFO ciAuto = {};
    ciAuto.cbSize = sizeof(ciAuto);
    ciAuto.dwMask = CM_MASK_WIDTH;
    ciAuto.uWidth = CM_WIDTH_AUTOSIZE;
    pCM->SetColumnInfo(key, &ciAuto);

    for (int p = 0; p < 30; p++) {
        if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        } else {
            Sleep(2);
        }
    }

    CM_COLUMNINFO ciResult = {};
    ciResult.cbSize = sizeof(ciResult);
    ciResult.dwMask = CM_MASK_WIDTH;
    bool ok = false;
    if (SUCCEEDED(pCM->GetColumnInfo(key, &ciResult)) && ciResult.uWidth > 0) {
        outWidth = static_cast<int>(ciResult.uWidth);
        ok = true;
    }

    if (pidlRestore) {
        PITEMID_CHILD apidlR[1] = { pidlRestore };
        pFV2->SelectAndPositionItems(1, const_cast<PCUITEMID_CHILD_ARRAY>(apidlR), nullptr,
                                      SVSI_ENSUREVISIBLE | SVSI_NOTAKEFOCUS | SVSI_NOSTATECHANGE);
        for (int p = 0; p < 10; p++) {
            if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            } else {
                Sleep(2);
            }
        }
    }

    return ok;
}
static void AutoFitColumnsFull(IShellView* pShellView, IFolderView2* pFV2,
                                IColumnManager* pCM,
                                const std::vector<PROPERTYKEY>& keys,
                                UINT colCount, int itemCount, bool elasticMode) {
    IShellFolder* pFolder = nullptr;
    if (FAILED(pFV2->GetFolder(IID_PPV_ARGS(&pFolder))) || !pFolder)
        return;

    PIDLIST_ABSOLUTE pidlFolder = nullptr;
    HRESULT hrPidl = SHGetIDListFromObject(pFolder, &pidlFolder);
    pFolder->Release();
    if (FAILED(hrPidl) || !pidlFolder)
        return;

    HWND hwndView = nullptr;
    pShellView->GetWindow(&hwndView);
    UINT dpi = hwndView ? GetDpiForWindow(hwndView) : 96;
    if (dpi == 0) dpi = 96;
    double dpiScale = dpi / 96.0;

    // Prefer the real font actually used for rendering, taken directly
    // from the list/header control, over guessing via SPI_* system
    // parameters (which may reflect a different UI element's font, e.g.
    // icon labels rather than Details-view report text).
    HFONT hFont = nullptr;
    HWND hwndHeader = hwndView ? FindDescendantByClass(hwndView, L"SysHeader32") : nullptr;
    HWND hwndListView = hwndView ? FindDescendantByClass(hwndView, L"SysListView32") : nullptr;
    HWND hwndFontSource = hwndHeader ? hwndHeader : hwndListView;
    if (hwndFontSource) {
        HFONT hSrcFont = reinterpret_cast<HFONT>(SendMessageW(hwndFontSource, WM_GETFONT, 0, 0));
        if (hSrcFont) {
            LOGFONTW lfCopy = {};
            if (GetObjectW(hSrcFont, sizeof(lfCopy), &lfCopy))
                hFont = CreateFontIndirectW(&lfCopy);
        }
    }

    if (!hFont) {
        LOGFONTW lf = {};
        BOOL gotFont = SystemParametersInfoForDpi(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0, dpi);
        if (!gotFont) SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0);

        // Modern WinUI/Fluent surfaces (like this system's folder view)
        // typically render with "Segoe UI Variable", not classic
        // "Segoe UI" — try that face first, since it has different
        // (generally wider) metrics that may better match what's
        // actually rendered on screen.
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
                hFont = hTestFont;
            } else {
                DeleteObject(hTestFont);  // font not actually available, fall through
            }
        }

        if (!hFont) hFont = CreateFontIndirectW(&lf);
    }

    HDC hdcScreen = GetDC(nullptr);
    HDC hdc = CreateCompatibleDC(hdcScreen);
    ReleaseDC(nullptr, hdcScreen);
    HFONT hOldFont = hFont ? static_cast<HFONT>(SelectObject(hdc, hFont)) : nullptr;

    std::vector<int> maxWidths(colCount, 0);
    std::vector<int> rawTextMax(colCount, 0);  // widest raw (unpadded) text, all items
    std::vector<int> rawTextMaxVisible(colCount, 0);  // widest raw text among presumed-visible items
    PITEMID_CHILD pidlWidestName = nullptr;  // owns a clone, for precise re-measurement
    PITEMID_CHILD pidlFirstItem = nullptr;   // owns a clone, to restore scroll position after
    std::vector<PITEMID_CHILD> pidlWidestForColumn(colCount, nullptr);  // owns clones, cols 1+

    // Seed with header label widths so a column is never narrower than its
    // own header plus a little room for the sort arrow.
    int sortArrowRoom = static_cast<int>(20 * dpiScale);
    for (UINT c = 0; c < colCount; c++) {
        IPropertyDescription* pDesc = nullptr;
        if (SUCCEEDED(PSGetPropertyDescription(keys[c], IID_PPV_ARGS(&pDesc))) && pDesc) {
            PWSTR pszHeader = nullptr;
            if (SUCCEEDED(pDesc->GetDisplayName(&pszHeader)) && pszHeader) {
                maxWidths[c] = MeasureTextWidth(hdc, pszHeader) + sortArrowRoom;
                CoTaskMemFree(pszHeader);
            }
            pDesc->Release();
        }
    }

    // Items near the top of a freshly opened/refreshed folder are the ones
    // realized by the built-in autosize, so use them to calibrate the
    // real per-column overhead (icon width, text margins, etc.) against
    // Explorer's own behavior, rather than guessing fixed pixel constants.
    const int kPresumedVisibleCount = 25;

    for (int i = 0; i < itemCount; i++) {
        PITEMID_CHILD pidl = nullptr;
        if (FAILED(pFV2->Item(i, &pidl)) || !pidl)
            continue;

        if (i == 0) pidlFirstItem = reinterpret_cast<PITEMID_CHILD>(ILClone(reinterpret_cast<PCIDLIST_ABSOLUTE>(pidl)));

        PIDLIST_ABSOLUTE pidlFull = ILCombine(pidlFolder, pidl);
        if (pidlFull) {
            IShellItem2* pItem2 = nullptr;
            if (SUCCEEDED(SHCreateItemFromIDList(pidlFull, IID_PPV_ARGS(&pItem2))) && pItem2) {
                for (UINT c = 0; c < colCount; c++) {
                    PWSTR pszDisplay = nullptr;

                    // Prefer GetProperty + PSFormatForDisplayAlloc, which
                    // respects the user's actual display format (locale,
                    // custom date/time format, etc.). GetString can
                    // succeed even for non-string properties like dates,
                    // but returns a canonical/machine string
                    // representation rather than what's really shown on
                    // screen — measuring that gave wrong (too-small)
                    // widths for a custom date format.
                    PROPVARIANT pv;
                    PropVariantInit(&pv);
                    HRESULT hrGet = E_FAIL;
                    if (SUCCEEDED(pItem2->GetProperty(keys[c], &pv))) {
                        hrGet = PSFormatForDisplayAlloc(keys[c], pv, PDFF_DEFAULT, &pszDisplay);
                        PropVariantClear(&pv);
                    }

                    if (FAILED(hrGet)) {
                        hrGet = pItem2->GetString(keys[c], &pszDisplay);
                    }

                    if (pszDisplay) {
                        int w = MeasureTextWidth(hdc, pszDisplay);
                        if (c == 1) {
                        }
                        if (w > rawTextMax[c]) {
                            rawTextMax[c] = w;
                            if (c == 0) {
                                if (pidlWidestName) CoTaskMemFree(pidlWidestName);
                                pidlWidestName = reinterpret_cast<PITEMID_CHILD>(ILClone(reinterpret_cast<PCIDLIST_ABSOLUTE>(pidl)));
                            } else {
                                if (pidlWidestForColumn[c]) CoTaskMemFree(pidlWidestForColumn[c]);
                                pidlWidestForColumn[c] = reinterpret_cast<PITEMID_CHILD>(ILClone(reinterpret_cast<PCIDLIST_ABSOLUTE>(pidl)));
                            }
                        }
                        if (i < kPresumedVisibleCount && w > rawTextMaxVisible[c])
                            rawTextMaxVisible[c] = w;
                        CoTaskMemFree(pszDisplay);
                    }
                }
                pItem2->Release();
            }
            ILFree(pidlFull);
        }
        CoTaskMemFree(pidl);
    }


    // Column 0 (Name) shows per-item icons/thumbnails, and different items
    // can reserve different icon-column widths (e.g. a file with a real
    // thumbnail preview vs. one with a plain generic file-type icon) — so
    // a single calibrated padding value can't reliably be exact even when
    // the calibration item mathematically matches the folder's overall
    // widest item (confirmed by testing: exact-match cases still
    // truncated). Scroll the actual widest-name item into view and ask
    // the built-in autosize to measure it directly, unconditionally —
    // this path has been reliable in every tested case, unlike the
    // formula-based shortcut for the "already matches" case.
    bool col0ExactFromScroll = false;
    if (!elasticMode && colCount > 0 && pidlWidestName) {
        int exactWidth0 = 0;
        if (ScrollAndMeasureColumn(pFV2, pCM, keys[0], pidlWidestName, pidlFirstItem, 0, exactWidth0)) {
            maxWidths[0] = exactWidth0;
            col0ExactFromScroll = true;
        }
    }
    if (pidlWidestName) CoTaskMemFree(pidlWidestName);

    // Apply the same exact-measurement technique to every other column
    // (Date, Type, Size, etc.), fixing the same class of issue Name had:
    // content whose true rendered width can't be reliably predicted from
    // a single calibration sample (e.g. varying date/time string lengths
    // with a custom format). Each column's widest item is scrolled into
    // view, measured via the real built-in autosize, then the original
    // position is restored, fully self-contained per column.
    std::vector<bool> colExactFromScroll(colCount, false);
    colExactFromScroll[0] = col0ExactFromScroll || elasticMode;
    for (UINT c = 1; c < colCount; c++) {
        if (!pidlWidestForColumn[c]) continue;
        int exactWidth = 0;
        if (ScrollAndMeasureColumn(pFV2, pCM, keys[c], pidlWidestForColumn[c],
                                    pidlFirstItem, c, exactWidth)) {
            maxWidths[c] = exactWidth;
            colExactFromScroll[c] = true;
        }
        CoTaskMemFree(pidlWidestForColumn[c]);
    }
    if (pidlFirstItem) CoTaskMemFree(pidlFirstItem);

    // Calibrate: ask the built-in autosize what it computes for whichever
    // items are actually currently realized/visible, then derive the real
    // per-column overhead by comparing that to our own raw-text measurement
    // of the presumed-visible set. This overhead (icon width, margins,
    // selection padding) is constant per column regardless of content
    // length, so it can be safely reused for the full-folder max.
    //
    // The autosize call is likely asynchronous on this system's WinUI-based
    // folder view, so a brief message pump after triggering it (before
    // reading the result back) is necessary — reading back immediately can
    // race the view's own re-layout and capture a stale value, which
    // showed up as sort-order-dependent inconsistency in testing.
    std::vector<int> padding(colCount, 0);
    for (UINT c = 0; c < colCount; c++) {
        if (colExactFromScroll[c]) {
            padding[c] = 0;  // maxWidths[c] already set precisely; skip recalibration
            continue;
        }

        CM_COLUMNINFO ciAuto = {};
        ciAuto.cbSize = sizeof(ciAuto);
        ciAuto.dwMask = CM_MASK_WIDTH;
        ciAuto.uWidth = CM_WIDTH_AUTOSIZE;
        pCM->SetColumnInfo(keys[c], &ciAuto);

        MSG msg;
        for (int pump = 0; pump < 30; pump++) {
            if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            } else {
                Sleep(2);
            }
        }

        CM_COLUMNINFO ciResult = {};
        ciResult.cbSize = sizeof(ciResult);
        ciResult.dwMask = CM_MASK_WIDTH;
        int measuredPad = (c == 0) ? static_cast<int>(48 * dpiScale) : static_cast<int>(24 * dpiScale);
        if (SUCCEEDED(pCM->GetColumnInfo(keys[c], &ciResult)) && rawTextMaxVisible[c] > 0) {
            int delta = static_cast<int>(ciResult.uWidth) - rawTextMaxVisible[c];
            if (delta > 0) measuredPad = delta;
        }
        padding[c] = measuredPad;

    }

    for (UINT c = 0; c < colCount; c++) {
        int w = rawTextMax[c] + padding[c];
        if (w > maxWidths[c]) maxWidths[c] = w;
    }

    if (hOldFont) SelectObject(hdc, hOldFont);
    if (hFont) DeleteObject(hFont);
    DeleteDC(hdc);
    ILFree(pidlFolder);

    // Elastic mode: instead of sizing the Name column to its own content,
    // give it whatever width is left over after every other column has
    // taken its natural (content-fit) width, based on the actual current
    // width of the folder view — so columns always exactly fill the
    // available window width with no leftover empty space and (down to a
    // small minimum) no horizontal scrollbar either.
    if (elasticMode && colCount > 0) {
        HWND hwndView = nullptr;
        pShellView->GetWindow(&hwndView);

        int viewportWidth = 0;
        if (hwndView) {
            RECT rc = {};
            if (GetClientRect(hwndView, &rc)) {
                viewportWidth = rc.right - rc.left;
            }
        }

        int otherColumnsTotal = 0;
        for (UINT c = 1; c < colCount; c++) {
            int minColWidth = static_cast<int>(40 * dpiScale);
            if (maxWidths[c] < minColWidth) maxWidths[c] = minColWidth;
            otherColumnsTotal += maxWidths[c];
        }

        int minNameWidth = static_cast<int>(60 * dpiScale);

        if (viewportWidth > 0 && hwndView) {
            // Start from a near-zero-margin estimate, then iteratively
            // shrink Name until the view actually reports no horizontal
            // scroll need — asking Explorer directly rather than trying
            // to precompute an exact number, since the real per-folder
            // overhead has proven too inconsistent to predict reliably.
            int nameWidth = viewportWidth - otherColumnsTotal - GetSystemMetrics(SM_CXVSCROLL);
            if (nameWidth < minNameWidth) nameWidth = minNameWidth;

            for (int attempt = 0; attempt < 15 && nameWidth > minNameWidth; attempt++) {
                CM_COLUMNINFO ciTry = {};
                ciTry.cbSize = sizeof(ciTry);
                ciTry.dwMask = CM_MASK_WIDTH;
                ciTry.uWidth = static_cast<UINT>(nameWidth);
                pCM->SetColumnInfo(keys[0], &ciTry);

                MSG msg;
                for (int p = 0; p < 20; p++) {
                    if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                        TranslateMessage(&msg);
                        DispatchMessageW(&msg);
                    } else {
                        Sleep(2);
                    }
                }

                bool detectedCheck = false;
                bool hasHScroll = ViewNeedsHorizontalScroll(hwndView, detectedCheck);

                if (!detectedCheck || !hasHScroll) break;
                nameWidth -= 10;
            }

            maxWidths[0] = nameWidth;
        }
        // If we couldn't determine the viewport width for some reason,
        // fall back to leaving maxWidths[0] as whatever it was seeded to
        // (the header width) rather than guessing further.
    }

    for (UINT c = 0; c < colCount; c++) {
        CM_COLUMNINFO ci = {};
        ci.cbSize = sizeof(ci);
        ci.dwMask = CM_MASK_WIDTH;
        ci.uWidth = maxWidths[c] > 0 ? static_cast<UINT>(maxWidths[c]) : CM_WIDTH_AUTOSIZE;
        pCM->SetColumnInfo(keys[c], &ci);
    }

    Wh_Log(L"Auto-fitted %u column(s) via full folder scan (%d items)", colCount, itemCount);
}

// Prevents AutoFitColumns from ever running re-entrantly. The full-scan
// path pumps messages while waiting on the view's own asynchronous
// layout, and if something triggered during that pump (e.g. sorting,
// which likely re-fires CDefView::UIActivate) caused a second call to
// stack on top of the first, both calls would fight over the same
// IColumnManager/IFolderView2 state — a plausible cause of the view
// becoming unresponsive to further interaction until Explorer restarts.
struct ReentrancyGuard {
    bool acquired = false;
    explicit ReentrancyGuard(volatile bool& flag) : flagRef(flag) {
        if (!flag) {
            flag = true;
            acquired = true;
        }
    }
    ~ReentrancyGuard() {
        if (acquired) flagRef = false;
    }
    volatile bool& flagRef;
};
static volatile bool g_autoFitRunning = false;

static void AutoFitColumns(IShellView* pShellView) {
    ReentrancyGuard guard(g_autoFitRunning);
    if (!guard.acquired) {
        return;
    }

    IFolderView2* pFV2 = nullptr;
    if (FAILED(pShellView->QueryInterface(IID_IFolderView2_,
                                          reinterpret_cast<void**>(&pFV2))) || !pFV2)
        return;

    FOLDERVIEWMODE viewMode = FVM_AUTO;
    int iconSize = 0;
    HRESULT hr = pFV2->GetViewModeAndIconSize(&viewMode, &iconSize);

    if (FAILED(hr) || viewMode != FVM_DETAILS) {
        pFV2->Release();
        return;
    }

    IColumnManager* pCM = nullptr;
    if (FAILED(pShellView->QueryInterface(IID_IColumnManager_,
                                          reinterpret_cast<void**>(&pCM))) || !pCM) {
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

    PCWSTR fitMode = Wh_GetStringSetting(L"fitModeSettings.fitMode");
    bool wantFullScan = fitMode && wcscmp(fitMode, L"full") == 0;
    bool wantElastic = fitMode && wcscmp(fitMode, L"elastic") == 0;
    Wh_FreeStringSetting(fitMode);

    bool didFullScan = false;
    if (wantFullScan || wantElastic) {
        int itemCount = 0;
        HRESULT hrCount = pFV2->ItemCount(SVGIO_ALLVIEW, &itemCount);
        if (SUCCEEDED(hrCount)) {
            int maxScan = Wh_GetIntSetting(L"fitModeSettings.maxScanItems");
            if (maxScan <= 0) maxScan = 100;
            if (itemCount <= maxScan) {
                AutoFitColumnsFull(pShellView, pFV2, pCM, keys, colCount, itemCount, wantElastic);
                didFullScan = true;
            }
        }
    }

    if (!didFullScan) {
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
    pFV2->Release();
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

    // Shell-level change notification for this tab's folder (covers any
    // trigger — including external programs — that actually changes
    // folder content, regardless of how it was triggered).
    if (uMsg == WM_AUTOFIT_SHELLNOTIFY) {
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

    // Live re-layout on resize, but only when Elastic mode is selected —
    // that's the only mode whose column widths actually depend on the
    // window's current size, so there's no reason to react to resizing
    // for the other modes.
    if (uMsg == WM_SIZE) {
        PCWSTR fitModeCheck = Wh_GetStringSetting(L"fitModeSettings.fitMode");
        bool isElastic = fitModeCheck && wcscmp(fitModeCheck, L"elastic") == 0;
        Wh_FreeStringSetting(fitModeCheck);
        if (isElastic) isRefresh = true;
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

        ULONG reg = 0;
        EnterCriticalSection(&g_cs);
        g_windowToTab.erase(hwnd);
        auto it = g_tabShellViews.find(hwnd);
        if (it != g_tabShellViews.end()) {
            if (it->second) it->second->Release();
            g_tabShellViews.erase(it);
        }
        auto itReg = g_tabNotifyReg.find(hwnd);
        if (itReg != g_tabNotifyReg.end()) {
            reg = itReg->second;
            g_tabNotifyReg.erase(itReg);
        }
        LeaveCriticalSection(&g_cs);

        if (reg)
            SHChangeNotifyDeregister(reg);
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
// Register (or re-register) shell change notifications for the folder
// currently shown in this tab. Deregisters any previous registration for
// the same tab first. Fires WM_AUTOFIT_SHELLNOTIFY on hwndTab whenever the
// folder's content actually changes, regardless of what triggered it
// (toolbar refresh, right-click refresh, external file changes, etc.).
// ---------------------------------------------------------------------------

static void RegisterFolderChangeNotify(HWND hwndTab, IShellView* pShellView) {
    if (!hwndTab || !pShellView) return;

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
        SHCNE_UPDATEDIR | SHCNE_UPDATEITEM | SHCNE_MKDIR | SHCNE_RMDIR |
            SHCNE_CREATE | SHCNE_DELETE | SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER,
        WM_AUTOFIT_SHELLNOTIFY,
        1,
        &entry);

    CoTaskMemFree(pidl);

    if (newReg) {
        EnterCriticalSection(&g_cs);
        g_tabNotifyReg[hwndTab] = newReg;
        LeaveCriticalSection(&g_cs);
    }
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

        // Also subclass the top-level frame window (CabinetWClass) itself,
        // bypassing IsSubclassTarget — the modern Fluent context menu's
        // Refresh command sends its signal to both the tab window and the
        // frame window, so subclassing both gives redundant coverage.
        {
            EnterCriticalSection(&g_cs);
            bool isNewTop = g_windowToTab.find(hwndTop) == g_windowToTab.end();
            g_windowToTab[hwndTop] = hwndTab;
            LeaveCriticalSection(&g_cs);
            if (isNewTop)
                WindhawkUtils::SetWindowSubclassFromAnyThread(hwndTop, ExplorerSubclassProc, 0);
        }

        // Also subclass hwndView (SHELLDLL_DefView) itself, bypassing
        // IsSubclassTarget — the classic background context menu (right-
        // click on empty space → Refresh) is created by CDefView and its
        // WM_COMMAND routes here directly, not to the tab/frame windows.
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
        if (hwndTab)
            RegisterFolderChangeNotify(hwndTab, pShellView);

        // Auto-fit on open/navigate/tab switch
        UINT delay = static_cast<UINT>(Wh_GetIntSetting(L"delay"));
        if (delay == 0) delay = 400;
        HWND hwndTimer = hwndTab ? hwndTab : hwndTop;
        SetTimer(hwndTimer, AUTOFIT_TIMER_ID, delay, nullptr);
    }
    return hr;
}

// ---------------------------------------------------------------------------
// Hook: CDefView::Refresh
//
// This is the actual IShellView method Explorer invokes for every refresh
// trigger — right-click "Refresh", the toolbar refresh button, F5, Ctrl+R,
// and even programmatic refreshes. Hooking it directly (instead of sniffing
// undocumented WM_COMMAND ids / WM_NOTIFY codes on various windows) is
// robust across Windows versions/updates.
// ---------------------------------------------------------------------------

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

            UINT delay = static_cast<UINT>(Wh_GetIntSetting(L"delay"));
            if (delay == 0) delay = 400;
            SetTimer(hwndTimer, AUTOFIT_TIMER_ID, delay, nullptr);
        }
    }

    return hr;
}

// ---------------------------------------------------------------------------
// Hook: CBrowserHost::Refresh (explorerframe.dll)
//
// Confirmed (via symbol search + live testing) to be what the modern
// WinUI3 command bar's refresh button actually calls — it does not go
// through CDefView::Refresh or produce any window message at all, so this
// hook is required to support that button.
// ---------------------------------------------------------------------------

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

        UINT delay = static_cast<UINT>(Wh_GetIntSetting(L"delay"));
        if (delay == 0) delay = 400;
        SetTimer(hwndTimer, AUTOFIT_TIMER_ID, delay, nullptr);
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
    HMODULE hExplorerFrame = LoadLibraryW(L"explorerframe.dll");
    if (hExplorerFrame) {
        const WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[] = {
            {
                {
                    L"public: virtual long __cdecl CBrowserHost::Refresh(long)",
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

void Wh_ModAfterInit() {}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    // Collect handles and release shell views under the lock,
    // then remove subclasses outside the lock to avoid deadlock
    std::vector<HWND> windowsToUnsubclass;
    std::vector<IShellView*> viewsToRelease;
    std::vector<ULONG> notifyRegsToRelease;

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
    for (auto& [hwnd, reg] : g_tabNotifyReg) {
        if (reg) notifyRegsToRelease.push_back(reg);
    }
    g_tabNotifyReg.clear();
    LeaveCriticalSection(&g_cs);

    // Safe to call outside the lock
    for (HWND hwnd : windowsToUnsubclass)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, ExplorerSubclassProc);
    for (IShellView* pSV : viewsToRelease)
        pSV->Release();
    for (ULONG reg : notifyRegsToRelease)
        SHChangeNotifyDeregister(reg);

    DeleteCriticalSection(&g_cs);
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged — new delay: %dms", Wh_GetIntSetting(L"delay"));
}
