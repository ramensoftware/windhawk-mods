// ==WindhawkMod==
// @id              desktop-icon-hover-reveal
// @name            Desktop Icon Hover Reveal
// @description     Automatically detects the desktop icon grid and reveals hidden rows when the cursor enters the expanded grid area.
// @version         1.0.0
// @author          Arun Prashath
// @github          arunprashath06
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -luser32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- HideRows: 2
  $name: Rows to hide
  $description: Number of bottom occupied desktop rows to hide.

- HoverRows: 2
  $name: Hover rows
  $description: Extra grid cells above and below the hidden area.

- HoverColumns: 2
  $name: Hover columns
  $description: Extra grid cells left and right of the hidden area.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
Desktop Grid Hover Reveal

The desktop icon layout is detected automatically.

Example:

    Rows to hide   = 2
    Hover rows     = 2
    Hover columns  = 2

The bottom two occupied rows are hidden.

The reveal area is expanded by two complete grid cells:

    2 cells above
    2 cells below
    2 cells left
    2 cells right

The extra cells also exist when there are no icons in them.

Moving the cursor inside the expanded area reveals the hidden icons.

Moving outside the expanded area hides them again.

No animation is used.

Icons are never moved or deleted.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <commctrl.h>

#include <algorithm>
#include <vector>

#include <windhawk_api.h>
#include <windhawk_utils.h>


// ============================================================================
// SETTINGS
// ============================================================================

struct Settings {
    int hideRows = 2;
    int hoverRows = 2;
    int hoverColumns = 2;
};

static Settings g_settings;


// ============================================================================
// DESKTOP STATE
// ============================================================================

struct DesktopState {
    HWND listView = nullptr;
    HWND shellView = nullptr;

    int itemCount = 0;

    bool rowsValid = false;

    // Occupied row center positions.
    std::vector<int> rowYs;

    // Occupied column center positions.
    std::vector<int> columnXs;

    // Position of every icon.
    std::vector<POINT> itemPositions;

    // Hidden item cache.
    std::vector<unsigned char> hiddenItems;

    // Current reveal state.
    bool revealState = false;
};

static std::vector<DesktopState*> g_states;


// ============================================================================
// PROPERTIES / TIMER
// ============================================================================

static constexpr wchar_t kStateProperty[] =
    L"WindhawkDesktopGridHoverState";

static constexpr wchar_t kShellProperty[] =
    L"WindhawkDesktopGridShellState";

static constexpr wchar_t kRevealProperty[] =
    L"WindhawkDesktopGridReveal";

static constexpr UINT_PTR kHoverTimerId =
    0x4517;

static constexpr UINT kHoverTimerInterval =
    30;


// ============================================================================
// CLASS CHECK
// ============================================================================

bool IsClassName(
    HWND hwnd,
    const wchar_t* wantedClass) {

    if (!hwnd) {
        return false;
    }

    wchar_t className[128] = {};

    if (GetClassNameW(
            hwnd,
            className,
            ARRAYSIZE(className)) == 0) {

        return false;
    }

    return wcscmp(
        className,
        wantedClass) == 0;
}


// ============================================================================
// DESKTOP SHELL VIEW CHECK
// ============================================================================

bool IsDesktopShellView(HWND hwnd) {

    if (!hwnd) {
        return false;
    }

    if (!IsClassName(
            hwnd,
            L"SHELLDLL_DefView")) {

        return false;
    }

    HWND parent =
        GetParent(hwnd);

    if (!parent) {
        return false;
    }

    return
        IsClassName(parent, L"Progman") ||
        IsClassName(parent, L"WorkerW");
}


// ============================================================================
// LOAD SETTINGS
// ============================================================================

void LoadSettings() {

    int hideRows =
        Wh_GetIntSetting(
            L"HideRows");

    int hoverRows =
        Wh_GetIntSetting(
            L"HoverRows");

    int hoverColumns =
        Wh_GetIntSetting(
            L"HoverColumns");


    // Safety limits.
    if (hideRows < 1) {
        hideRows = 1;
    }

    if (hideRows > 10) {
        hideRows = 10;
    }


    if (hoverRows < 0) {
        hoverRows = 0;
    }

    if (hoverRows > 20) {
        hoverRows = 20;
    }


    if (hoverColumns < 0) {
        hoverColumns = 0;
    }

    if (hoverColumns > 20) {
        hoverColumns = 20;
    }


    g_settings.hideRows =
        hideRows;

    g_settings.hoverRows =
        hoverRows;

    g_settings.hoverColumns =
        hoverColumns;


    Wh_Log(
        L"Settings: hideRows=%d hoverRows=%d hoverColumns=%d",
        g_settings.hideRows,
        g_settings.hoverRows,
        g_settings.hoverColumns);
}


// ============================================================================
// SORT UNIQUE
// ============================================================================

void SortUnique(
    std::vector<int>& values) {

    std::sort(
        values.begin(),
        values.end());

    values.erase(
        std::unique(
            values.begin(),
            values.end()),
        values.end());
}


// ============================================================================
// GET LISTVIEW GRID SPACING
// ============================================================================
//
// Uses the actual ListView spacing first.
// Falls back to measured occupied icon spacing.
// ============================================================================

int GetHorizontalGridSpacing(
    DesktopState* state) {

    if (!state ||
        !IsWindow(state->listView)) {

        return 80;
    }


    LRESULT spacing =
        SendMessageW(
            state->listView,
            LVM_GETITEMSPACING,
            FALSE,
            0);


    int value =
        LOWORD(spacing);


    if (value > 0) {
        return value;
    }


    // Fallback: calculate from occupied columns.
    if (state->columnXs.size() >= 2) {

        int smallestGap = 0;

        for (size_t i = 1;
             i < state->columnXs.size();
             ++i) {

            int gap =
                state->columnXs[i] -
                state->columnXs[i - 1];

            if (gap <= 0) {
                continue;
            }

            if (smallestGap == 0 ||
                gap < smallestGap) {

                smallestGap = gap;
            }
        }

        if (smallestGap > 0) {
            return smallestGap;
        }
    }


    return 80;
}


int GetVerticalGridSpacing(
    DesktopState* state) {

    if (!state ||
        !IsWindow(state->listView)) {

        return 75;
    }


    LRESULT spacing =
        SendMessageW(
            state->listView,
            LVM_GETITEMSPACING,
            FALSE,
            0);


    int value =
        HIWORD(spacing);


    if (value > 0) {
        return value;
    }


    // Fallback: calculate from occupied rows.
    if (state->rowYs.size() >= 2) {

        int smallestGap = 0;

        for (size_t i = 1;
             i < state->rowYs.size();
             ++i) {

            int gap =
                state->rowYs[i] -
                state->rowYs[i - 1];

            if (gap <= 0) {
                continue;
            }

            if (smallestGap == 0 ||
                gap < smallestGap) {

                smallestGap = gap;
            }
        }

        if (smallestGap > 0) {
            return smallestGap;
        }
    }


    return 75;
}


// ============================================================================
// REBUILD GRID
// ============================================================================

void RebuildGrid(
    DesktopState* state) {

    if (!state ||
        !IsWindow(state->listView)) {

        return;
    }


    HWND listView =
        state->listView;


    int count =
        static_cast<int>(
            SendMessageW(
                listView,
                LVM_GETITEMCOUNT,
                0,
                0));


    state->itemCount =
        count;


    state->rowYs.clear();
    state->columnXs.clear();
    state->itemPositions.clear();
    state->hiddenItems.clear();


    if (count <= 0) {

        state->rowsValid =
            true;

        return;
    }


    state->itemPositions.resize(
        count);

    state->hiddenItems.resize(
        count,
        0);


    for (int i = 0;
         i < count;
         ++i) {

        POINT position = {};

        if (ListView_GetItemPosition(
                listView,
                i,
                &position)) {

            state->itemPositions[i] =
                position;

            state->rowYs.push_back(
                position.y);

            state->columnXs.push_back(
                position.x);
        }
    }


    SortUnique(
        state->rowYs);

    SortUnique(
        state->columnXs);


    state->rowsValid =
        true;


    Wh_Log(
        L"Grid rebuilt: items=%d rows=%d columns=%d",
        state->itemCount,
        static_cast<int>(
            state->rowYs.size()),
        static_cast<int>(
            state->columnXs.size()));
}


// ============================================================================
// ENSURE GRID
// ============================================================================

void EnsureGridValid(
    DesktopState* state) {

    if (!state ||
        !IsWindow(state->listView)) {

        return;
    }


    int count =
        static_cast<int>(
            SendMessageW(
                state->listView,
                LVM_GETITEMCOUNT,
                0,
                0));


    if (!state->rowsValid ||
        count != state->itemCount) {

        RebuildGrid(state);
    }
}


// ============================================================================
// FIND ROW INDEX
// ============================================================================

int GetRowIndex(
    const DesktopState* state,
    int y) {

    if (!state) {
        return -1;
    }


    auto it =
        std::lower_bound(
            state->rowYs.begin(),
            state->rowYs.end(),
            y);


    if (it ==
        state->rowYs.end()) {

        return -1;
    }


    if (*it != y) {
        return -1;
    }


    return static_cast<int>(
        it -
        state->rowYs.begin());
}


// ============================================================================
// ITEM HIDDEN?
// ============================================================================

bool IsItemHidden(
    const DesktopState* state,
    int itemIndex) {

    if (!state) {
        return false;
    }


    if (itemIndex < 0 ||
        itemIndex >= state->itemCount) {

        return false;
    }


    if (state->rowYs.empty()) {
        return false;
    }


    POINT position =
        state->itemPositions[
            itemIndex];


    int rowIndex =
        GetRowIndex(
            state,
            position.y);


    if (rowIndex < 0) {
        return false;
    }


    int rowCount =
        static_cast<int>(
            state->rowYs.size());


    int firstHiddenRow =
        std::max(
            0,
            rowCount -
                g_settings.hideRows);


    return
        rowIndex >=
        firstHiddenRow;
}


// ============================================================================
// REBUILD HIDDEN CACHE
// ============================================================================

void RebuildHiddenItems(
    DesktopState* state) {

    if (!state) {
        return;
    }


    EnsureGridValid(
        state);


    if (state->itemCount <= 0) {
        return;
    }


    std::fill(
        state->hiddenItems.begin(),
        state->hiddenItems.end(),
        0);


    for (int i = 0;
         i < state->itemCount;
         ++i) {

        if (IsItemHidden(
                state,
                i)) {

            state->hiddenItems[i] =
                1;
        }
    }
}


// ============================================================================
// HOVER AREA
// ============================================================================
//
// IMPORTANT:
//
// This version does NOT clamp the hover area to occupied rows/columns.
//
// Example:
//
// Hidden rows = bottom 2
// HoverRows = 2
//
// If there are only 2 occupied rows:
//
//     [virtual]
//     [virtual]
//     [hidden ]
//     [hidden ]
//
// Both virtual rows are valid hover space.
//
// Same for columns:
//
//     [virtual][virtual][icons...][virtual][virtual]
//
// ============================================================================

bool IsMouseInHoverArea(
    DesktopState* state) {

    if (!state ||
        !IsWindow(state->listView)) {

        return false;
    }


    EnsureGridValid(
        state);


    if (state->rowYs.empty() ||
        state->columnXs.empty()) {

        return false;
    }


    POINT cursor = {};


    if (!GetCursorPos(
            &cursor)) {

        return false;
    }


    // ------------------------------------------------------------
    // Convert global cursor to ListView coordinates.
    // ------------------------------------------------------------

    ScreenToClient(
        state->listView,
        &cursor);


    // ------------------------------------------------------------
    // Hidden rows.
    // ------------------------------------------------------------

    int rowCount =
        static_cast<int>(
            state->rowYs.size());


    int columnCount =
        static_cast<int>(
            state->columnXs.size());


    int firstHiddenRow =
        std::max(
            0,
            rowCount -
                g_settings.hideRows);


    int lastHiddenRow =
        rowCount - 1;


    // ------------------------------------------------------------
    // Get actual grid spacing.
    // ------------------------------------------------------------

    int rowSpacing =
        GetVerticalGridSpacing(
            state);


    int columnSpacing =
        GetHorizontalGridSpacing(
            state);


    // ------------------------------------------------------------
    // Hidden block boundaries.
    //
    // We use the icon centers as the grid anchors.
    // ------------------------------------------------------------

    int hiddenTop =
        state->rowYs[
            firstHiddenRow];


    int hiddenBottom =
        state->rowYs[
            lastHiddenRow];


    int hiddenLeft =
        state->columnXs[
            0];


    int hiddenRight =
        state->columnXs[
            columnCount - 1];


    // ------------------------------------------------------------
    // Build actual hover rectangle.
    //
    // Half a cell around the outer icon centers forms the
    // normal hidden-cell boundary.
    // Then add the configured number of COMPLETE cells.
    // ------------------------------------------------------------

    int halfRowSpacing =
        rowSpacing / 2;


    int halfColumnSpacing =
        columnSpacing / 2;


    int top =
        hiddenTop -
        halfRowSpacing -
        (g_settings.hoverRows *
         rowSpacing);


    int bottom =
        hiddenBottom +
        halfRowSpacing +
        (g_settings.hoverRows *
         rowSpacing);


    int left =
        hiddenLeft -
        halfColumnSpacing -
        (g_settings.hoverColumns *
         columnSpacing);


    int right =
        hiddenRight +
        halfColumnSpacing +
        (g_settings.hoverColumns *
         columnSpacing);


    // ------------------------------------------------------------
    // Cursor test.
    // ------------------------------------------------------------

    bool inside =
        cursor.x >= left &&
        cursor.x <= right &&
        cursor.y >= top &&
        cursor.y <= bottom;


    return inside;
}


// ============================================================================
// UPDATE VISIBILITY
// ============================================================================

void UpdateVisibility(
    DesktopState* state) {

    if (!state ||
        !IsWindow(state->listView)) {

        return;
    }


    bool reveal =
        IsMouseInHoverArea(
            state);


    if (reveal ==
        state->revealState) {

        return;
    }


    state->revealState =
        reveal;


    SetPropW(
        state->listView,
        kRevealProperty,
        reinterpret_cast<HANDLE>(
            static_cast<INT_PTR>(
                reveal ? 1 : 0)));


    InvalidateRect(
        state->listView,
        nullptr,
        FALSE);
}


// ============================================================================
// LISTVIEW SUBCLASS
// ============================================================================

LRESULT CALLBACK DesktopListViewSubclassProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR dwRefData) {

    DesktopState* state =
        reinterpret_cast<DesktopState*>(
            dwRefData);


    switch (uMsg) {

        // --------------------------------------------------------
        // Mouse movement.
        // --------------------------------------------------------

        case WM_MOUSEMOVE: {

            UpdateVisibility(
                state);

            break;
        }


        // --------------------------------------------------------
        // Global cursor polling.
        // --------------------------------------------------------

        case WM_TIMER: {

            if (wParam ==
                kHoverTimerId) {

                UpdateVisibility(
                    state);

                return 0;
            }

            break;
        }


        // --------------------------------------------------------
        // New icon.
        // --------------------------------------------------------

        case LVM_INSERTITEMA:
        case LVM_INSERTITEMW: {

            LRESULT result =
                DefSubclassProc(
                    hWnd,
                    uMsg,
                    wParam,
                    lParam);


            if (state) {

                state->rowsValid =
                    false;

                RebuildGrid(
                    state);

                RebuildHiddenItems(
                    state);

                UpdateVisibility(
                    state);
            }


            InvalidateRect(
                hWnd,
                nullptr,
                FALSE);


            return result;
        }


        // --------------------------------------------------------
        // Icon removed.
        // --------------------------------------------------------

        case LVM_DELETEITEM:
        case LVM_DELETEALLITEMS: {

            LRESULT result =
                DefSubclassProc(
                    hWnd,
                    uMsg,
                    wParam,
                    lParam);


            if (state) {

                state->rowsValid =
                    false;

                RebuildGrid(
                    state);

                RebuildHiddenItems(
                    state);

                UpdateVisibility(
                    state);
            }


            InvalidateRect(
                hWnd,
                nullptr,
                FALSE);


            return result;
        }


        // --------------------------------------------------------
        // Icon arrangement changed.
        // --------------------------------------------------------

        case LVM_SETITEMPOSITION:
        case LVM_SETITEMPOSITION32:
        case LVM_ARRANGE: {

            LRESULT result =
                DefSubclassProc(
                    hWnd,
                    uMsg,
                    wParam,
                    lParam);


            if (state) {

                state->rowsValid =
                    false;

                RebuildGrid(
                    state);

                RebuildHiddenItems(
                    state);

                UpdateVisibility(
                    state);
            }


            InvalidateRect(
                hWnd,
                nullptr,
                FALSE);


            return result;
        }


        // --------------------------------------------------------
        // Display changes.
        // --------------------------------------------------------

        case WM_SIZE:
        case WM_DISPLAYCHANGE:
        case WM_SETTINGCHANGE:
        case WM_DPICHANGED: {

            if (state) {

                state->rowsValid =
                    false;

                RebuildGrid(
                    state);

                RebuildHiddenItems(
                    state);

                UpdateVisibility(
                    state);
            }


            InvalidateRect(
                hWnd,
                nullptr,
                FALSE);

            break;
        }


        // --------------------------------------------------------
        // Cleanup.
        // --------------------------------------------------------

        case WM_NCDESTROY: {

            if (state) {

                KillTimer(
                    hWnd,
                    kHoverTimerId);


                RemovePropW(
                    hWnd,
                    kStateProperty);


                RemovePropW(
                    hWnd,
                    kRevealProperty);


                auto it =
                    std::find(
                        g_states.begin(),
                        g_states.end(),
                        state);


                if (it !=
                    g_states.end()) {

                    g_states.erase(it);
                }


                delete state;
            }


            break;
        }
    }


    return DefSubclassProc(
        hWnd,
        uMsg,
        wParam,
        lParam);
}


// ============================================================================
// SHELL VIEW SUBCLASS
// ============================================================================

LRESULT CALLBACK DesktopShellViewSubclassProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR dwRefData) {

    (void)wParam;
    (void)dwRefData;


    if (uMsg ==
        WM_NOTIFY &&
        lParam != 0) {

        NMHDR* header =
            reinterpret_cast<NMHDR*>(
                lParam);


        if (header &&
            header->code ==
                NM_CUSTOMDRAW &&
            IsClassName(
                header->hwndFrom,
                L"SysListView32")) {


            HWND listView =
                header->hwndFrom;


            DesktopState* state =
                reinterpret_cast<DesktopState*>(
                    GetPropW(
                        listView,
                        kStateProperty));


            if (state) {

                NMLVCUSTOMDRAW*
                    customDraw =
                        reinterpret_cast<
                            NMLVCUSTOMDRAW*>(
                                lParam);


                DWORD stage =
                    customDraw->
                        nmcd.dwDrawStage;


                // ------------------------------------------------
                // Before drawing.
                // ------------------------------------------------

                if (stage ==
                    CDDS_PREPAINT) {

                    EnsureGridValid(
                        state);


                    RebuildHiddenItems(
                        state);


                    bool reveal =
                        GetPropW(
                            listView,
                            kRevealProperty) !=
                        nullptr;


                    if (!reveal) {

                        return
                            CDRF_NOTIFYITEMDRAW;
                    }


                    return
                        CDRF_DODEFAULT;
                }


                // ------------------------------------------------
                // Before drawing each icon.
                // ------------------------------------------------

                if (stage ==
                    CDDS_ITEMPREPAINT) {

                    bool reveal =
                        GetPropW(
                            listView,
                            kRevealProperty) !=
                        nullptr;


                    if (reveal) {

                        return
                            CDRF_DODEFAULT;
                    }


                    int itemIndex =
                        static_cast<int>(
                            customDraw->
                                nmcd.dwItemSpec);


                    if (itemIndex >= 0 &&
                        itemIndex <
                            static_cast<int>(
                                state->
                                    hiddenItems.size())) {


                        if (state->
                            hiddenItems[
                                itemIndex]) {

                            return
                                CDRF_SKIPDEFAULT;
                        }
                    }


                    return
                        CDRF_DODEFAULT;
                }
            }
        }
    }


    return DefSubclassProc(
        hWnd,
        uMsg,
        wParam,
        lParam);
}


// ============================================================================
// ATTACH LISTVIEW
// ============================================================================

void AttachListView(
    HWND listView,
    HWND shellView) {

    if (!listView ||
        !IsWindow(listView)) {

        return;
    }


    if (!IsClassName(
            listView,
            L"SysListView32")) {

        return;
    }


    if (!IsDesktopShellView(
            shellView)) {

        return;
    }


    if (GetPropW(
            listView,
            kStateProperty)) {

        return;
    }


    DesktopState* state =
        new DesktopState();


    state->listView =
        listView;


    state->shellView =
        shellView;


    SetPropW(
        listView,
        kStateProperty,
        reinterpret_cast<HANDLE>(
            state));


    SetPropW(
        listView,
        kRevealProperty,
        reinterpret_cast<HANDLE>(
            static_cast<INT_PTR>(0)));


    if (!WindhawkUtils::
            SetWindowSubclassFromAnyThread(
                listView,
                DesktopListViewSubclassProc,
                reinterpret_cast<DWORD_PTR>(
                    state))) {


        RemovePropW(
            listView,
            kStateProperty);


        RemovePropW(
            listView,
            kRevealProperty);


        delete state;


        Wh_Log(
            L"Failed to subclass desktop ListView");


        return;
    }


    g_states.push_back(
        state);


    RebuildGrid(
        state);


    RebuildHiddenItems(
        state);


    // Global cursor polling.
    SetTimer(
        listView,
        kHoverTimerId,
        kHoverTimerInterval,
        nullptr);


    UpdateVisibility(
        state);


    Wh_Log(
        L"Desktop ListView attached: %p",
        listView);
}


// ============================================================================
// ATTACH SHELL VIEW
// ============================================================================

void AttachShellView(
    HWND shellView) {

    if (!IsDesktopShellView(
            shellView)) {

        return;
    }


    HWND listView =
        FindWindowExW(
            shellView,
            nullptr,
            L"SysListView32",
            nullptr);


    if (!listView) {
        return;
    }


    if (!GetPropW(
            shellView,
            kShellProperty)) {


        if (!WindhawkUtils::
                SetWindowSubclassFromAnyThread(
                    shellView,
                    DesktopShellViewSubclassProc,
                    0)) {

            Wh_Log(
                L"Failed to subclass ShellView");

            return;
        }


        SetPropW(
            shellView,
            kShellProperty,
            reinterpret_cast<HANDLE>(
                static_cast<INT_PTR>(1)));
    }


    AttachListView(
        listView,
        shellView);
}


// ============================================================================
// ENUMERATION
// ============================================================================

BOOL CALLBACK EnumDesktopChildren(
    HWND hwnd,
    LPARAM) {

    if (IsDesktopShellView(hwnd)) {
        AttachShellView(hwnd);
    }

    return TRUE;
}


BOOL CALLBACK EnumDesktopWindows(
    HWND hwnd,
    LPARAM) {

    EnumChildWindows(
        hwnd,
        EnumDesktopChildren,
        0);

    return TRUE;
}


void AttachExistingDesktop() {

    EnumWindows(
        EnumDesktopWindows,
        0);
}


// ============================================================================
// WINDHAWK INIT
// ============================================================================

BOOL Wh_ModInit() {

    Wh_Log(
        L"Desktop Grid Hover Reveal v1.9 initializing");


    LoadSettings();


    AttachExistingDesktop();


    return TRUE;
}


// ============================================================================
// SETTINGS CHANGED
// ============================================================================

void Wh_ModSettingsChanged() {

    Wh_Log(
        L"Desktop Grid Hover Reveal settings changed");


    LoadSettings();


    for (DesktopState* state :
         g_states) {


        if (!state ||
            !IsWindow(
                state->listView)) {

            continue;
        }


        state->rowsValid =
            false;


        RebuildGrid(
            state);


        RebuildHiddenItems(
            state);


        UpdateVisibility(
            state);


        InvalidateRect(
            state->listView,
            nullptr,
            FALSE);
    }
}


// ============================================================================
// WINDHAWK UNINIT
// ============================================================================

void Wh_ModUninit() {

    Wh_Log(
        L"Desktop Grid Hover Reveal unloading");


    EnumWindows(
        [](HWND topWindow,
           LPARAM) -> BOOL {


            EnumChildWindows(
                topWindow,


                [](HWND child,
                   LPARAM) -> BOOL {


                    if (!IsDesktopShellView(
                            child)) {

                        return TRUE;
                    }


                    HWND listView =
                        FindWindowExW(
                            child,
                            nullptr,
                            L"SysListView32",
                            nullptr);


                    if (listView) {

                        DesktopState* state =
                            reinterpret_cast<DesktopState*>(
                                GetPropW(
                                    listView,
                                    kStateProperty));


                        KillTimer(
                            listView,
                            kHoverTimerId);


                        WindhawkUtils::
                            RemoveWindowSubclassFromAnyThread(
                                listView,
                                DesktopListViewSubclassProc);


                        RemovePropW(
                            listView,
                            kStateProperty);


                        RemovePropW(
                            listView,
                            kRevealProperty);


                        if (state) {

                            auto it =
                                std::find(
                                    g_states.begin(),
                                    g_states.end(),
                                    state);


                            if (it !=
                                g_states.end()) {

                                g_states.erase(it);
                            }


                            delete state;
                        }
                    }


                    // ------------------------------------------------
                    // Remove ShellView subclass.
                    // ------------------------------------------------

                    if (GetPropW(
                            child,
                            kShellProperty)) {


                        WindhawkUtils::
                            RemoveWindowSubclassFromAnyThread(
                                child,
                                DesktopShellViewSubclassProc);


                        RemovePropW(
                            child,
                            kShellProperty);
                    }


                    return TRUE;

                },
                0);


            return TRUE;

        },
        0);


    for (DesktopState* state :
         g_states) {

        delete state;
    }


    g_states.clear();


    Wh_Log(
        L"Desktop Grid Hover Reveal unloaded");
}
