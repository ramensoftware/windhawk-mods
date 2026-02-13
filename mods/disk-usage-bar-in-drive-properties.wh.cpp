// ==WindhawkMod==
// @id              disk-usage-bar-in-drive-properties
// @name            Disk Usage Bar in Drive Properties
// @description     Replaces the pie/donut chart in drive properties with a usage bar
// @version         1.0
// @author          Kitsune
// @github          https://github.com/AromaKitsune
// @include         *
// @compilerOptions -luxtheme -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disk Usage Bar in Drive Properties

This mod replaces the disk usage pie/donut chart in the drive properties dialog
with a usage bar.

![](https://raw.githubusercontent.com/AromaKitsune/My-Windhawk-Mods/main/screenshots/disk-usage-bar-in-drive-properties.png)

## Features
* Replaces the pie/donut chart with a blue usage bar, like in "This PC".
* Switches the bar color to red when the disk is almost full.
* Displays the usage percentage text below the bar.

## Supported Windows versions
* Windows 11
* Windows 10
* Windows 8.1

## Configuration
You can enable the option to switch the usage bar color to red when disk usage
exceeds 90%.

---

Based on the "[Disk Pie Chart](https://windhawk.net/mods/disk-pie-chart)" mod by
**aubymori**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showRedUsageBar: false
  $name: Switch to red bar when disk is almost full
  $description: Switches the usage bar color to red when disk usage exceeds 90%.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <uxtheme.h>
#include <vssym32.h>
#include <shellapi.h>

#ifdef _WIN64
#   define SSTDCALL L"__cdecl"
#   define SHELL32_DRAWPIE L"DrawPie"
#else
#   define SSTDCALL L"__stdcall"
#   define SHELL32_DRAWPIE L"_DrawPie@20"
#endif

#define WPDSHEXT_DRAWPIE \
    L"void " \
    SSTDCALL \
    L" _DrawPie(struct HDC__ *,struct tagRECT const *,unsigned int," \
    L"unsigned int,unsigned long const *)"

struct {
    bool showRedUsageBar;
} settings;

// Helper to restore the AutoRun icon
// Since Windows 2000, the drive properties dialog never displays the AutoRun
// icon on the General tab, leaving a blank space.
// This function restores the AutoRun icon back where it belongs.
void RestoreCustomDriveIcon(HWND hPropPageWnd)
{
    // Find the static control with the SS_ICON style
    HWND hIconWnd = nullptr;
    HWND hChildWnd = GetWindow(hPropPageWnd, GW_CHILD);
    while (hChildWnd)
    {
        WCHAR szClassName[32];
        if (GetClassNameW(hChildWnd, szClassName, ARRAYSIZE(szClassName)) &&
            lstrcmpiW(szClassName, L"Static") == 0)
        {
            LONG_PTR lStyle = GetWindowLongPtrW(hChildWnd, GWL_STYLE);
            if ((lStyle & 0x1F) == SS_ICON)
            {
                hIconWnd = hChildWnd;
                break;
            }
        }
        hChildWnd = GetWindow(hChildWnd, GW_HWNDNEXT);
    }

    if (!hIconWnd) return;

    // Check if the icon is already assigned. If so, skip the restoration
    if (SendMessage(hIconWnd, STM_GETICON, 0, 0)) return;

    // Find the static control with the SS_CENTER style
    // Expected text format: "Drive C:" or similar
    HWND hLabelWnd = nullptr;
    hChildWnd = GetWindow(hPropPageWnd, GW_CHILD);
    while (hChildWnd)
    {
        WCHAR szClassName[32];
        if (GetClassNameW(hChildWnd, szClassName, ARRAYSIZE(szClassName)) &&
            lstrcmpiW(szClassName, L"Static") == 0)
        {
            LONG_PTR lStyle = GetWindowLongPtrW(hChildWnd, GWL_STYLE);
            if ((lStyle & 0x1F) == SS_CENTER)
            {
                hLabelWnd = hChildWnd;
                break;
            }
        }
        hChildWnd = GetWindow(hChildWnd, GW_HWNDNEXT);
    }

    if (!hLabelWnd) return;

    // Extract the drive root path from the label window
    WCHAR szLabelText[256];
    if (!GetWindowTextW(hLabelWnd, szLabelText, ARRAYSIZE(szLabelText))) return;

    WCHAR szDriveRoot[] = L"A:\\";
    bool isDriveLetterFound = false;

    // Scan for the "*:" pattern
    // * represents a drive letter (A-Z)
    int cchLabelText = lstrlenW(szLabelText);
    for (int i = 0; i < cchLabelText - 1; i++)
    {
        if (szLabelText[i] >= L'A' &&
                szLabelText[i] <= L'Z' &&
                szLabelText[i + 1] == L':')
        {
            szDriveRoot[0] = szLabelText[i];
            isDriveLetterFound = true;
            break;
        }
    }

    if (!isDriveLetterFound) return;

    // Load the correct shell icon to restore the AutoRun icon
    SHFILEINFOW shFileInfo = { 0 };
    if (SHGetFileInfoW(szDriveRoot, 0, &shFileInfo, sizeof(shFileInfo),
            SHGFI_ICON | SHGFI_LARGEICON))
    {
        SendMessage(hIconWnd, STM_SETICON, (WPARAM)shFileInfo.hIcon, 0);
    }
}

// Helper to update the disk usage percentage label below the disk usage bar
void UpdateDiskUsagePercentLabel(HWND hPropPageWnd, DWORD dwUsagePercent, const RECT& rcChart)
{
    // Define the unique control ID for the custom usage percentage label
    const int IDC_USAGE_PERCENT_LABEL = 14999;

    // Find the static control with the SS_CENTER style
    // Expected text format: "Drive C:" or similar
    HWND hLabelWnd = nullptr;
    HWND hChildWnd = GetWindow(hPropPageWnd, GW_CHILD);
    while (hChildWnd)
    {
        WCHAR szClassName[32];
        if (GetClassNameW(hChildWnd, szClassName, ARRAYSIZE(szClassName)) &&
            lstrcmpiW(szClassName, L"Static") == 0)
        {
            LONG_PTR lStyle = GetWindowLongPtrW(hChildWnd, GWL_STYLE);
            if ((lStyle & 0x1F) == SS_CENTER)
            {
                hLabelWnd = hChildWnd;
                break;
            }
        }
        hChildWnd = GetWindow(hChildWnd, GW_HWNDNEXT);
    }

    // Create a custom usage percentage label window for the portable device
    // storage properties page
    bool isUsagePercentLabel = false;
    if (!hLabelWnd)
    {
        // Check if the custom usage percentage label window has already been
        // created
        hLabelWnd = GetDlgItem(hPropPageWnd, IDC_USAGE_PERCENT_LABEL);

        // If the custom usage percentage label window does not exist, create it
        // now
        if (!hLabelWnd)
        {
            hLabelWnd = CreateWindowExW(0, L"Static", L"",
                WS_CHILD | WS_VISIBLE | SS_CENTER | SS_NOPREFIX,
                0, 0, 0, 0, // Positioned dynamically
                hPropPageWnd, (HMENU)IDC_USAGE_PERCENT_LABEL,
                (HINSTANCE)GetWindowLongPtrW(hPropPageWnd, GWLP_HINSTANCE),
                nullptr);

            // Apply the property page's font to the custom usage percentage
            // label window
            if (hLabelWnd)
            {
                HFONT hPropPageFont = (HFONT)SendMessage(hPropPageWnd, WM_GETFONT, 0, 0);
                SendMessage(hLabelWnd, WM_SETFONT, (WPARAM)hPropPageFont, TRUE);
            }
        }
        isUsagePercentLabel = true;
    }

    if (!hLabelWnd) return;

    // Load the localized "Space used" string from propsys.dll
    WCHAR szLabelTemplate[64];
    bool isStringLoaded = false;
    HMODULE hPropSys = LoadLibraryExW(L"propsys.dll", nullptr,
        LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hPropSys)
    {
        if (LoadStringW(hPropSys, 38652, szLabelTemplate, ARRAYSIZE(szLabelTemplate)))
        {
            isStringLoaded = true;
        }
        FreeLibrary(hPropSys);
    }
    else
    {
        Wh_Log(L"Failed to load propsys.dll");
    }

    // Fall back to the hardcoded English string if the module or string
    // resource fails to load
    if (!isStringLoaded)
    {
        lstrcpyW(szLabelTemplate, L"Space used");
    }

    // Format the string: "Space used: 64%"
    WCHAR szUpdatedText[128];
    wsprintfW(szUpdatedText, L"%s: %u%%", szLabelTemplate, dwUsagePercent);

    // Prevent infinite re-paint loops and flickering by updating only when
    // necessary
    WCHAR szCurrentText[128];
    GetWindowTextW(hLabelWnd, szCurrentText, ARRAYSIZE(szCurrentText));
    if (lstrcmpW(szCurrentText, szUpdatedText) != 0)
    {
        // Dynamically resize the label window to fit the text area exactly
        // The label window's default boundary is large, which causes it to draw
        // over the adjacent horizontal separator and "Details" / "Disk
        // Clean-up" button when the text changes.
        HDC hTextDC = GetDC(hLabelWnd);
        HFONT hCurrentFont = (HFONT)SendMessage(hLabelWnd, WM_GETFONT, 0, 0);
        HFONT hOriginalFont = (HFONT)SelectObject(hTextDC,
            hCurrentFont
                ? hCurrentFont
                : GetStockObject(DEFAULT_GUI_FONT));

        // Calculate the required dimensions of the text area
        RECT rcLabelText = { 0 };
        if (DrawTextW(hTextDC, szUpdatedText, -1, &rcLabelText,
                DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX))
        {
            // Add padding
            int cxNeeded = (rcLabelText.right - rcLabelText.left) + 4;
            int cyNeeded = (rcLabelText.bottom - rcLabelText.top) + 2;

            // Retrieve the geometry of the label window
            RECT rcLabelWnd;
            GetWindowRect(hLabelWnd, &rcLabelWnd);
            MapWindowPoints(nullptr, hPropPageWnd, reinterpret_cast<LPPOINT>(&rcLabelWnd), 2);

            // Align with the horizontal separator window's geometry by finding
            // the static control with the SS_ETCHEDHORZ style
            HWND hSeparatorWnd = nullptr;
            hChildWnd = GetWindow(hPropPageWnd, GW_CHILD);
            while (hChildWnd)
            {
                WCHAR szClassName[32];
                if (GetClassNameW(hChildWnd, szClassName,
                        ARRAYSIZE(szClassName)) &&
                    lstrcmpiW(szClassName, L"Static") == 0)
                {
                    LONG_PTR lStyle = GetWindowLongPtrW(hChildWnd, GWL_STYLE);
                    if ((lStyle & 0x1F) == SS_ETCHEDHORZ)
                    {
                        hSeparatorWnd = hChildWnd;
                        break;
                    }
                }
                hChildWnd = GetWindow(hChildWnd, GW_HWNDNEXT);
            }

            // Initialize the horizontal geometry using the original label
            // window dimensions
            int centerX = (rcLabelWnd.left + rcLabelWnd.right) / 2;

            // Adopt the horizontal separator window's geometry
            if (hSeparatorWnd)
            {
                RECT rcSeparatorWnd;
                GetWindowRect(hSeparatorWnd, &rcSeparatorWnd);
                MapWindowPoints(nullptr, hPropPageWnd, reinterpret_cast<LPPOINT>(&rcSeparatorWnd), 2);
                centerX = (rcSeparatorWnd.left + rcSeparatorWnd.right) / 2;
            }

            int xLeft = centerX - (cxNeeded / 2);
            int yTop = rcLabelWnd.top;

            // Position the custom usage percentage label window below the
            // original chart area
            if (isUsagePercentLabel)
            {
                // Define the vertical margin in Dialog Units
                RECT rcLabelMargin = { 0, 0, 0, 7 };
                MapDialogRect(hPropPageWnd, &rcLabelMargin);
                int cyLabelMargin = rcLabelMargin.bottom;
                yTop = rcChart.bottom + cyLabelMargin;
            }

            SetWindowPos(hLabelWnd, nullptr,
                xLeft, yTop,
                cxNeeded, cyNeeded,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }

        SelectObject(hTextDC, hOriginalFont);
        ReleaseDC(hLabelWnd, hTextDC);

        SetWindowTextW(hLabelWnd, szUpdatedText);
    }
}

// Helper to hide legends and align labels to the left
void HideLegendsAndAlignLabels(HWND hPropPageWnd)
{
    // Define control IDs for the standard drive storage properties page
    const int IDC_SHELL32_LEGEND_USED     = 14403;
    const int IDC_SHELL32_LEGEND_FREE     = 14404;
    const int IDC_SHELL32_LABEL_USED      = 14416;
    const int IDC_SHELL32_LABEL_FREE      = 14417;
    const int IDC_SHELL32_LABEL_CAPACITY  = 14415;

    // Define control IDs for the portable device storage properties page
    const int IDC_WPDSHEXT_LEGEND_USED    = 834;
    const int IDC_WPDSHEXT_LEGEND_FREE    = 838;
    const int IDC_WPDSHEXT_LABEL_USED     = 835;
    const int IDC_WPDSHEXT_LABEL_FREE     = 839;
    const int IDC_WPDSHEXT_LABEL_CAPACITY = 843;

    // Handle the standard drive storage properties page
    HWND hLegendWnd = GetDlgItem(hPropPageWnd, IDC_SHELL32_LEGEND_USED);
    if (hLegendWnd)
    {
        // Check if the layout has already been adjusted
        if (IsWindowVisible(hLegendWnd))
        {
            // Retrieve the X position of the legend window as the margin
            RECT rcLegendWnd;
            GetWindowRect(hLegendWnd, &rcLegendWnd);
            MapWindowPoints(nullptr, hPropPageWnd, reinterpret_cast<LPPOINT>(&rcLegendWnd), 2);
            int xLeft = rcLegendWnd.left;

            // Hide the legend windows
            ShowWindow(hLegendWnd, SW_HIDE);
            ShowWindow(GetDlgItem(hPropPageWnd, IDC_SHELL32_LEGEND_FREE), SW_HIDE);

            // Move the label windows to the left margin
            const int rgLabelIds[] = {
                IDC_SHELL32_LABEL_USED,
                IDC_SHELL32_LABEL_FREE,
                IDC_SHELL32_LABEL_CAPACITY
            };

            for (int id : rgLabelIds)
            {
                HWND hLabelWnd = GetDlgItem(hPropPageWnd, id);
                if (hLabelWnd)
                {
                    RECT rcLabelWnd;
                    GetWindowRect(hLabelWnd, &rcLabelWnd);
                    MapWindowPoints(nullptr, hPropPageWnd, reinterpret_cast<LPPOINT>(&rcLabelWnd), 2);
                    SetWindowPos(hLabelWnd, nullptr, xLeft, rcLabelWnd.top,
                        0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                }
            }
        }
        return;
    }

    // Handle the portable device storage properties page
    hLegendWnd = GetDlgItem(hPropPageWnd, IDC_WPDSHEXT_LEGEND_USED);
    if (hLegendWnd)
    {
        // Check if the layout has already been adjusted
        if (IsWindowVisible(hLegendWnd))
        {
            // Retrieve the X position of the legend window as the margin
            RECT rcLegendWnd;
            GetWindowRect(hLegendWnd, &rcLegendWnd);
            MapWindowPoints(nullptr, hPropPageWnd, reinterpret_cast<LPPOINT>(&rcLegendWnd), 2);
            int xLeft = rcLegendWnd.left;

            // Hide the legend windows
            ShowWindow(hLegendWnd, SW_HIDE);
            ShowWindow(GetDlgItem(hPropPageWnd, IDC_WPDSHEXT_LEGEND_FREE), SW_HIDE);

            // Move the label windows to the left margin
            const int rgLabelIds[] = {
                IDC_WPDSHEXT_LABEL_USED,
                IDC_WPDSHEXT_LABEL_FREE,
                IDC_WPDSHEXT_LABEL_CAPACITY
            };

            for (int id : rgLabelIds)
            {
                HWND hLabelWnd = GetDlgItem(hPropPageWnd, id);
                if (hLabelWnd)
                {
                    RECT rcLabelWnd;
                    GetWindowRect(hLabelWnd, &rcLabelWnd);
                    MapWindowPoints(nullptr, hPropPageWnd, reinterpret_cast<LPPOINT>(&rcLabelWnd), 2);
                    SetWindowPos(hLabelWnd, nullptr, xLeft, rcLabelWnd.top,
                        0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                }
            }
        }
        return;
    }
}

// Helper to draw the disk usage bar
void DrawDiskUsageBar(HDC hChartDC, LPCRECT prcChart, DWORD dwUsagePer1000)
{
    HWND hChartWnd = WindowFromDC(hChartDC);
    if (!hChartWnd) return;

    HWND hPropPageWnd = GetParent(hChartWnd);
    if (!hPropPageWnd) return;

    HDC hPropPageDC = GetDC(hPropPageWnd);
    if (!hPropPageDC) return;

    // Restore the AutoRun icon
    RestoreCustomDriveIcon(hPropPageWnd);

    // Convert the raw disk usage value (0-1000) to a rounded 0-100 integer for
    // the disk usage percentage label and the disk usage bar fill color state
    // Example: 635-644 → 64%
    //          645-654 → 65%
    DWORD dwUsagePercent = (dwUsagePer1000 + 5) / 10;
    if (dwUsagePercent > 100) dwUsagePercent = 100;

    // Retrieve the geometry of the original chart area
    RECT rcChart = *prcChart;
    MapWindowPoints(hChartWnd, hPropPageWnd, reinterpret_cast<LPPOINT>(&rcChart), 2);

    // Update the disk usage percentage label below the disk usage bar
    UpdateDiskUsagePercentLabel(hPropPageWnd, dwUsagePercent, rcChart);

    // Hide legends and align labels to the left
    HideLegendsAndAlignLabels(hPropPageWnd);

    // Define the usage bar height in Dialog Units
    RECT rcBarDlu = { 0, 0, 0, 14 };
    MapDialogRect(hPropPageWnd, &rcBarDlu);
    int cyBar = rcBarDlu.bottom;

    // Calculate the vertical center of the original chart area
    int centerY = (rcChart.top + rcChart.bottom) / 2;

    // Align with the horizontal separator window's geometry by finding the
    // static control with the SS_ETCHEDHORZ style
    HWND hSeparatorWnd = nullptr;
    HWND hChildWnd = GetWindow(hPropPageWnd, GW_CHILD);
    while (hChildWnd)
    {
        WCHAR szClassName[32];
        if (GetClassNameW(hChildWnd, szClassName, ARRAYSIZE(szClassName)) &&
            lstrcmpiW(szClassName, L"Static") == 0)
        {
            LONG_PTR lStyle = GetWindowLongPtrW(hChildWnd, GWL_STYLE);
            if ((lStyle & 0x1F) == SS_ETCHEDHORZ)
            {
                hSeparatorWnd = hChildWnd;
                break;
            }
        }
        hChildWnd = GetWindow(hChildWnd, GW_HWNDNEXT);
    }

    // Initialize the horizontal geometry using the original chart area
    // dimensions
    int cxBar = rcChart.right - rcChart.left;
    int centerX = (rcChart.left + rcChart.right) / 2;

    // Adopt the horizontal separator window's geometry
    if (hSeparatorWnd)
    {
        RECT rcSeparatorWnd;
        GetWindowRect(hSeparatorWnd, &rcSeparatorWnd);
        MapWindowPoints(nullptr, hPropPageWnd, reinterpret_cast<LPPOINT>(&rcSeparatorWnd), 2);
        cxBar = rcSeparatorWnd.right - rcSeparatorWnd.left;
        centerX = (rcSeparatorWnd.left + rcSeparatorWnd.right) / 2;
    }

    RECT rcBar;
    rcBar.left   = centerX - (cxBar / 2);
    rcBar.right  = rcBar.left + cxBar;
    rcBar.top    = centerY - (cyBar / 2);
    rcBar.bottom = rcBar.top + cyBar;

    // Resize the original chart window to match the usage bar's dimensions
    // This prevents the re-draw glitch caused by partially moving the
    // properties window off-screen and back in, by ensuring the clipping
    // region covers the entire usage bar.
    RECT rcChartWnd;
    GetWindowRect(hChartWnd, &rcChartWnd);
    MapWindowPoints(nullptr, hPropPageWnd, reinterpret_cast<LPPOINT>(&rcChartWnd), 2);
    if (!EqualRect(&rcBar, &rcChartWnd))
    {
        SetWindowPos(hChartWnd, nullptr,
            rcBar.left, rcBar.top,
            rcBar.right - rcBar.left, rcBar.bottom - rcBar.top,
            SWP_NOZORDER | SWP_NOACTIVATE);
    }

    // Determine the usage bar fill state
    int iFillState = (settings.showRedUsageBar && dwUsagePercent > 90)
        ? PBFS_ERROR    // Red (when disk is >90% full)
        : PBFS_PARTIAL; // Blue

    // Load the Explorer::Progress class to render the blue fill texture
    HTHEME hTheme = OpenThemeData(hPropPageWnd, L"Explorer::Progress");
    if (!hTheme)
    {
        // Fall back to the generic Progress class if Explorer::Progress fails
        // to load
        hTheme = OpenThemeData(hPropPageWnd, L"Progress");

        // The generic Progress class does not support the Partial (blue) fill
        // state; revert to Normal (green)
        if (iFillState == PBFS_PARTIAL) iFillState = PBFS_NORMAL;
    }

    // Draw the usage bar
    if (hTheme)
    {
        // Draw the track (background)
        DrawThemeBackground(hTheme, hPropPageDC, PP_BAR, 0, &rcBar, nullptr);

        // Draw the fill (foreground)
        if (dwUsagePer1000 > 0)
        {
            RECT rcFill = rcBar;

            // Cap usage at 100% to prevent the fill from overflowing
            if (dwUsagePer1000 > 1000) dwUsagePer1000 = 1000;

            int cxFill = (cxBar * static_cast<int>(dwUsagePer1000)) / 1000;
            rcFill.right = rcFill.left + cxFill;
            if (cxFill > 0)
                DrawThemeBackground(hTheme, hPropPageDC, PP_FILL, iFillState, &rcFill, nullptr);
        }

        CloseThemeData(hTheme);
    }

    // Fallback for Classic theme
    else
    {
        // Draw the track (background)
        FillRect(hPropPageDC, &rcBar, GetSysColorBrush(COLOR_BTNFACE));

        // Determine the usage bar fill color state
        COLORREF crFill = (iFillState == PBFS_ERROR)
            ? RGB(196, 43, 28)              // Red (when disk is >90% full)
            : GetSysColor(COLOR_HIGHLIGHT); // System color: Highlight

        HBRUSH hFillBrush = CreateSolidBrush(crFill);

        // Draw the fill (foreground)
        if (dwUsagePer1000 > 0)
        {
            RECT rcFill = rcBar;

            // Cap usage at 100% to prevent the fill from overflowing
            if (dwUsagePer1000 > 1000) dwUsagePer1000 = 1000;

            rcFill.right = rcFill.left + ((cxBar * static_cast<int>(dwUsagePer1000)) / 1000);
            FillRect(hPropPageDC, &rcFill, hFillBrush);
        }

        // Draw the border
        FrameRect(hPropPageDC, &rcBar, hFillBrush);

        DeleteObject(hFillBrush);
    }

    ReleaseDC(hPropPageWnd, hPropPageDC);
}

// Hook DrawPie (shell32.dll) for standard drive storage properties
using Shell32_DrawPie_t = int (__fastcall *)(HDC, LPRECT, DWORD, DWORD, const DWORD *);
Shell32_DrawPie_t Shell32_DrawPie_Original;
int __fastcall Shell32_DrawPie_Hook(
    HDC hdc,
    LPRECT prcChart,
    DWORD dwUsagePer1000,
    DWORD dwCachePer1000,
    const DWORD *lpColors
)
{
    DrawDiskUsageBar(hdc, prcChart, dwUsagePer1000);
    return 0; // Suppress the original chart
}

const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
    {
        { SHELL32_DRAWPIE },
        &Shell32_DrawPie_Original,
        Shell32_DrawPie_Hook,
        false
    }
};

// Hook _DrawPie (wpdshext.dll) for portable device storage properties
using WpdShExt_DrawPie_t = void (__fastcall *)(HDC, LPCRECT, DWORD, DWORD, const DWORD *);
WpdShExt_DrawPie_t WpdShExt_DrawPie_Original;
void __fastcall WpdShExt_DrawPie_Hook(
    HDC hdc,
    LPCRECT prcChart,
    DWORD dwUsagePer1000,
    DWORD dwCachePer1000,
    const DWORD *lpColors
)
{
    DrawDiskUsageBar(hdc, prcChart, dwUsagePer1000);
}

const WindhawkUtils::SYMBOL_HOOK wpdshextDllHooks[] = {
    {
        { WPDSHEXT_DRAWPIE },
        &WpdShExt_DrawPie_Original,
        WpdShExt_DrawPie_Hook,
        false
    }
};

// Load settings
void LoadSettings()
{
    settings.showRedUsageBar = Wh_GetIntSetting(L"showRedUsageBar");
}

// Mod initialization
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    LoadSettings();

    HMODULE hShell32 = LoadLibraryExW(L"shell32.dll", nullptr,
        LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hShell32)
    {
        if (!WindhawkUtils::HookSymbols(
            hShell32,
            shell32DllHooks,
            ARRAYSIZE(shell32DllHooks)
        ))
        {
            Wh_Log(L"Failed to hook DrawPie in shell32.dll");
        }
    }

    HMODULE hWpdShExt = LoadLibraryExW(L"wpdshext.dll", nullptr,
        LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hWpdShExt)
    {
        if (!WindhawkUtils::HookSymbols(
            hWpdShExt,
            wpdshextDllHooks,
            ARRAYSIZE(wpdshextDllHooks)
        ))
        {
            Wh_Log(L"Failed to hook _DrawPie in wpdshext.dll");
        }
    }

    return TRUE;
}

// Reload settings
void Wh_ModSettingsChanged()
{
    LoadSettings();
}
