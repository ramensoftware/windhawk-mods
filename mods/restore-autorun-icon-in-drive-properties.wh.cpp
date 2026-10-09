// ==WindhawkMod==
// @id              restore-autorun-icon-in-drive-properties
// @name            Restore AutoRun Icon in Drive Properties
// @description     Restores the missing AutoRun icon in drive properties
// @version         1.0.1
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
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <shellapi.h>

#ifdef _WIN64
#   define DRAWPIE L"DrawPie"
#else
#   define DRAWPIE L"_DrawPie@20"
#endif

// Helper: Restore the AutoRun icon
void RestoreCustomDriveIcon(HWND hPropPageWnd)
{
    // Find the static control with the SS_ICON style
    HWND hIconWnd = nullptr;
    HWND hChildWnd = GetWindow(hPropPageWnd, GW_CHILD);
    while (hChildWnd)
    {
        WCHAR szClassName[8];
        if (GetClassNameW(hChildWnd, szClassName, ARRAYSIZE(szClassName)) &&
            _wcsicmp(szClassName, L"Static") == 0 &&
            (GetWindowLongPtrW(hChildWnd, GWL_STYLE) & 0x1F) == SS_ICON)
        {
            hIconWnd = hChildWnd;
            break;
        }
        hChildWnd = GetWindow(hChildWnd, GW_HWNDNEXT);
    }

    if (!hIconWnd)
    {
        return;
    }

    // Check if the icon is already assigned. If so, skip the restoration
    if (SendMessageW(hIconWnd, STM_GETICON, 0, 0))
    {
        return;
    }

    // Find the static control with the SS_CENTER style
    // Expected text format: "Drive C:" or similar
    HWND hLabelWnd = nullptr;
    hChildWnd = GetWindow(hPropPageWnd, GW_CHILD);
    while (hChildWnd)
    {
        WCHAR szClassName[8];
        if (GetClassNameW(hChildWnd, szClassName, ARRAYSIZE(szClassName)) &&
            _wcsicmp(szClassName, L"Static") == 0 &&
            (GetWindowLongPtrW(hChildWnd, GWL_STYLE) & 0x1F) == SS_CENTER)
        {
            hLabelWnd = hChildWnd;
            break;
        }
        hChildWnd = GetWindow(hChildWnd, GW_HWNDNEXT);
    }

    if (!hLabelWnd)
    {
        return;
    }

    // Extract the drive root path from the label window
    WCHAR szLabelText[64];
    if (!GetWindowTextW(hLabelWnd, szLabelText, ARRAYSIZE(szLabelText)))
    {
        return;
    }

    WCHAR szDriveRoot[] = L"A:\\";
    bool isDriveLetterFound = false;

    // Scan for the "*:" pattern
    // * represents a drive letter (A-Z)
    int cchLabelText = static_cast<int>(wcslen(szLabelText));
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

    if (!isDriveLetterFound)
    {
        return;
    }

    // Load the correct shell icon to restore the AutoRun icon
    SHFILEINFOW shFileInfo;
    if (SHGetFileInfoW(szDriveRoot, 0, &shFileInfo, sizeof(shFileInfo),
            SHGFI_ICON | SHGFI_LARGEICON))
    {
        SendMessageW(hIconWnd, STM_SETICON,
            reinterpret_cast<WPARAM>(shFileInfo.hIcon), 0);
    }
}

// Hook for DrawPie (shell32.dll) - Trigger the AutoRun icon restoration logic
// This function executes whenever the "General" tab is re-painted, providing a
// valid HDC and window handle needed to target the icon and label windows.
using DrawPie_t = int(__fastcall*)(HDC, LPRECT, DWORD, DWORD, const DWORD*);
DrawPie_t DrawPie_Original;
int __fastcall DrawPie_Hook(
    HDC hChartDC,
    LPRECT prcChart,
    DWORD dwUsagePer1000,
    DWORD dwCachePer1000,
    const DWORD* lpColors
)
{
    HWND hChartWnd = WindowFromDC(hChartDC);
    if (hChartWnd && GetParent(hChartWnd))
    {
        // Restore the AutoRun icon
        RestoreCustomDriveIcon(GetParent(hChartWnd));
    }

    // Call the original function to draw the disk usage pie/donut chart
    return DrawPie_Original(hChartDC, prcChart, dwUsagePer1000, dwCachePer1000,
        lpColors);
}

const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] =
{
    {
        { DRAWPIE },
        &DrawPie_Original,
        DrawPie_Hook,
        false
    }
};

// Mod initialization
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    HMODULE hShell32 = LoadLibraryExW(L"shell32.dll", nullptr,
        LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
    }
    else if (!WindhawkUtils::HookSymbols(hShell32, shell32DllHooks,
            ARRAYSIZE(shell32DllHooks)))
    {
        Wh_Log(L"Failed to hook DrawPie in shell32.dll");
    }

    return TRUE;
}
