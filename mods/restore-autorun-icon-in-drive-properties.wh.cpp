// ==WindhawkMod==
// @id              restore-autorun-icon-in-drive-properties
// @name            Restore AutoRun Icon in Drive Properties
// @description     Restores the missing AutoRun icon in drive properties
// @version         1.0
// @author          Kitsune
// @github          https://github.com/AromaKitsune
// @include         *
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Restore AutoRun Icon in Drive Properties

Since Windows 2000, the drive properties dialog never displays the AutoRun icon
on the General tab, leaving a blank space.

This mod restores the AutoRun icon back where it belongs.

| Before | After |
| :----: | :---: |
| ![](https://raw.githubusercontent.com/AromaKitsune/My-Windhawk-Mods/main/screenshots/restore-autorun-icon-in-drive-properties_before.png) | ![](https://raw.githubusercontent.com/AromaKitsune/My-Windhawk-Mods/main/screenshots/restore-autorun-icon-in-drive-properties_after.png) |

## Compatibility with other mods
Any other mods that hook the `DrawPie` function will conflict with this mod and
prevent it from functioning.

### Conflicting mods
* [Disk Usage Bar in Drive Properties](https://windhawk.net/mods/disk-usage-bar-in-drive-properties)
  by me
* [Disk Pie Chart](https://windhawk.net/mods/disk-pie-chart)
  by **aubymori**

**Note:** The "Disk Usage Bar in Drive Properties" mod already has the AutoRun
icon restoration code integrated, so you don't need to use both mods together.

## Supported Windows versions
* Windows 11
* Windows 10
* Windows 8.1
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <shellapi.h>

#ifdef _WIN64
#   define DRAWPIE L"DrawPie"
#else
#   define DRAWPIE L"_DrawPie@20"
#endif

// Helper to restore the AutoRun icon
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

// Hook DrawPie to trigger the AutoRun icon restoration logic
// This function executes whenever the "General" tab is re-painted, providing a
// valid HDC and window handle needed to target the icon and label windows.
using DrawPie_t = int (__fastcall *)(HDC, LPRECT, DWORD, DWORD, const DWORD *);
DrawPie_t DrawPie_Original;
int __fastcall DrawPie_Hook(
    HDC hdc,
    LPRECT prcChart,
    DWORD dwUsagePer1000,
    DWORD dwCachePer1000,
    const DWORD *lpColors
)
{
    HWND hChartWnd = WindowFromDC(hdc);
    if (hChartWnd)
    {
        HWND hPropPageWnd = GetParent(hChartWnd);
        if (hPropPageWnd)
        {
            // Restore the AutoRun icon
            RestoreCustomDriveIcon(hPropPageWnd);
        }
    }

    // Call the original function to draw the disk usage pie/donut chart
    return DrawPie_Original(
        hdc,
        prcChart,
        dwUsagePer1000,
        dwCachePer1000,
        lpColors
    );
}

const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
    {
        { DRAWPIE },
        &DrawPie_Original,
        DrawPie_Hook,
        false
    }
};

BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

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

    return TRUE;
}
