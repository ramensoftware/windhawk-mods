// ==WindhawkMod==
// @id              hide-scrollbars
// @name            Hide Scrollbars
// @description     Hide vertical/horizontal scrollbars in selected processes (default: File Explorer) and reclaim the gutter space
// @version         1.0.0
// @author          AmazingBodilyFluids
// @github          https://github.com/AmazingBodilyFluids
// @include         explorer.exe
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Scrollbars

Shrinks the vertical and/or horizontal scrollbars away in the target
process. By default it targets `explorer.exe`, so the File Explorer file
list loses its scrollbar and the freed strip is used by the content
instead of sitting empty.

To apply it to other programs, add them to the mod's **Custom process
inclusion list** under the mod's Advanced settings in Windhawk.

Mouse-wheel and keyboard scrolling keep working. Precision-touchpad
two-finger scrolling keeps working at the default scrollbar size of 1px;
setting the size to 0 breaks it (see the limitation below).

## Screenshots

Windows default (slim scrollbar in its own gutter):

![Default](https://raw.githubusercontent.com/AmazingBodilyFluids/windhawk-mods/assets/hide-scrollbars-default.png)

Scrollbar size 1 px:

![Size 1](https://raw.githubusercontent.com/AmazingBodilyFluids/windhawk-mods/assets/hide-scrollbars-1px.png)

Scrollbar size 0 px (fully hidden):

![Size 0](https://raw.githubusercontent.com/AmazingBodilyFluids/windhawk-mods/assets/hide-scrollbars-0px.png)

## How it works

The mod hooks `GetSystemMetrics`, `GetSystemMetricsForDpi` and
`GetThemeSysSize`, and reports a reduced size for `SM_CXVSCROLL` /
`SM_CYHSCROLL` (the vertical/horizontal scrollbar thickness). Code that
lays itself out from that metric - such as Explorer's DirectUI file list
- then shrinks the scrollbar accordingly. Plain Win32 windows that draw
standard non-client scrollbars are mostly unaffected, because user32
lays those out from an internal metric table rather than the exported
function.

## Known limitation: precision-touchpad scrolling

Reporting a size of **0** makes the scrollbar vanish completely, but a
0-width scrollbar reads as "not scrollable" to the Windows
precision-touchpad pan handler, so **two-finger scrolling stops working**
in affected windows. Mouse wheel and keyboard scrolling (arrows,
PageUp/PageDown, Home/End) are unaffected.

Reporting **1** leaves a 1px sliver that is visually negligible and keeps
two-finger scrolling working.

The **Scrollbar size** setting lets you choose:

- `1` (default) - touchpad-safe, 1px sliver remains.
- `0` - fully hidden, breaks precision-touchpad two-finger scroll.

## Notes

- The override is process-wide. Other mods running in the same process
  that read `SM_CXVSCROLL` / `SM_CYHSCROLL` (for example to hit-test the
  scrollbar strip) will see the reduced value too.
- `SM_CXVSCROLL` also drives the width of combo-box drop-down buttons
  and the status-bar sizing grip. Code that reads the exported metric
  (rather than user32's internal table) shrinks those too.
- The scrollbar *arrow button* metrics (`SM_CXHSCROLL` / `SM_CYVSCROLL`)
  and `SPI_GETNONCLIENTMETRICS` are left untouched, so a control that
  mixes those with `SM_CXVSCROLL` may lay out slightly oddly.
- If a targeted process misbehaves, remove it from the inclusion list.

## Settings

- **Hide vertical scrollbars** - toggle the vertical bar.
- **Hide horizontal scrollbars** - toggle the horizontal bar.
- **Scrollbar size** - reported thickness in pixels; use `0` or `1`.
  `1` (default) keeps precision-touchpad scrolling working; `0` hides
  the bar fully but breaks two-finger touchpad scroll.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideVertical: true
  $name: Hide vertical scrollbars
- hideHorizontal: true
  $name: Hide horizontal scrollbars
- scrollbarSize: 1
  $name: Scrollbar size
  $description: >-
    Reported scrollbar thickness in pixels; use 0 or 1. 1 (default)
    keeps precision-touchpad two-finger scrolling working while leaving
    only a 1px sliver. 0 hides the scrollbar completely but breaks
    precision-touchpad scrolling (mouse wheel and keyboard still work).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>

#include <uxtheme.h>

struct {
    std::atomic<bool> hideVertical;
    std::atomic<bool> hideHorizontal;
    std::atomic<int> scrollbarSize;
} g_settings;

// Non-null only if this mod (rather than the process) loaded uxtheme.dll.
HMODULE g_loadedUxtheme;

// --- hooks ----------------------------------------------------------------

using GetSystemMetrics_t = decltype(&GetSystemMetrics);
GetSystemMetrics_t GetSystemMetrics_Original;

int WINAPI GetSystemMetrics_Hook(int nIndex) {
    if (nIndex == SM_CXVSCROLL && g_settings.hideVertical) {
        return g_settings.scrollbarSize;
    }
    if (nIndex == SM_CYHSCROLL && g_settings.hideHorizontal) {
        return g_settings.scrollbarSize;
    }
    return GetSystemMetrics_Original(nIndex);
}

using GetSystemMetricsForDpi_t = decltype(&GetSystemMetricsForDpi);
GetSystemMetricsForDpi_t GetSystemMetricsForDpi_Original;

int WINAPI GetSystemMetricsForDpi_Hook(int nIndex, UINT dpi) {
    if (nIndex == SM_CXVSCROLL && g_settings.hideVertical) {
        return g_settings.scrollbarSize;
    }
    if (nIndex == SM_CYHSCROLL && g_settings.hideHorizontal) {
        return g_settings.scrollbarSize;
    }
    return GetSystemMetricsForDpi_Original(nIndex, dpi);
}

using GetThemeSysSize_t = decltype(&GetThemeSysSize);
GetThemeSysSize_t GetThemeSysSize_Original;

int WINAPI GetThemeSysSize_Hook(HTHEME hTheme, int iSizeId) {
    if (iSizeId == SM_CXVSCROLL && g_settings.hideVertical) {
        return g_settings.scrollbarSize;
    }
    if (iSizeId == SM_CYHSCROLL && g_settings.hideHorizontal) {
        return g_settings.scrollbarSize;
    }
    return GetThemeSysSize_Original(hTheme, iSizeId);
}

// --- helpers ------------------------------------------------------------

void LoadSettings() {
    g_settings.hideVertical = Wh_GetIntSetting(L"hideVertical") != 0;
    g_settings.hideHorizontal = Wh_GetIntSetting(L"hideHorizontal") != 0;
    int size = Wh_GetIntSetting(L"scrollbarSize");
    g_settings.scrollbarSize = size < 0 ? 0 : size;
}

BOOL CALLBACK RefreshChildProc(HWND hChild, LPARAM) {
    SendMessageTimeoutW(hChild, WM_THEMECHANGED, 0, 0, SMTO_ABORTIFHUNG, 200,
                        nullptr);
    return TRUE;
}

BOOL CALLBACK RefreshTopProc(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return TRUE;
    }

    // Only Explorer browser windows host the file-list scrollbar. Don't
    // re-theme the shell (tray, desktop, flyouts) over a scrollbar metric.
    WCHAR className[64];
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) == 0 ||
        _wcsicmp(className, L"CabinetWClass") != 0) {
        return TRUE;
    }

    // WM_THEMECHANGED doesn't forward to children on its own, so walk them
    // too. Guarded because each CabinetWClass window runs on its own thread.
    SendMessageTimeoutW(hWnd, WM_THEMECHANGED, 0, 0, SMTO_ABORTIFHUNG, 200,
                        nullptr);
    EnumChildWindows(hWnd, RefreshChildProc, 0);
    return TRUE;
}

void RefreshWindows() {
    EnumWindows(RefreshTopProc, 0);
}

// --- mod entry points -------------------------------------------------

BOOL Wh_ModInit() {
    LoadSettings();

    if (!g_settings.hideVertical && !g_settings.hideHorizontal) {
        Wh_Log(L"Nothing to hide, staying inactive");
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(GetSystemMetrics, GetSystemMetrics_Hook,
                                   &GetSystemMetrics_Original);

    if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
        auto pGetSystemMetricsForDpi = (GetSystemMetricsForDpi_t)GetProcAddress(
            user32, "GetSystemMetricsForDpi");
        if (pGetSystemMetricsForDpi) {
            WindhawkUtils::SetFunctionHook(pGetSystemMetricsForDpi,
                                          GetSystemMetricsForDpi_Hook,
                                          &GetSystemMetricsForDpi_Original);
        }
    }

    HMODULE uxtheme = GetModuleHandleW(L"uxtheme.dll");
    if (!uxtheme) {
        uxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr,
                                 LOAD_LIBRARY_SEARCH_SYSTEM32);
        g_loadedUxtheme = uxtheme;
    }
    if (uxtheme) {
        auto pGetThemeSysSize =
            (GetThemeSysSize_t)GetProcAddress(uxtheme, "GetThemeSysSize");
        if (pGetThemeSysSize) {
            WindhawkUtils::SetFunctionHook(pGetThemeSysSize, GetThemeSysSize_Hook,
                                          &GetThemeSysSize_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    RefreshWindows();
}

void Wh_ModUninit() {
    // Hooks are already removed by the time this runs, so the windows
    // relayout against the real metrics.
    RefreshWindows();

    if (g_loadedUxtheme) {
        FreeLibrary(g_loadedUxtheme);
        g_loadedUxtheme = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    bool prevVertical = g_settings.hideVertical;
    bool prevHorizontal = g_settings.hideHorizontal;
    int prevSize = g_settings.scrollbarSize;

    LoadSettings();

    if (g_settings.hideVertical != prevVertical ||
        g_settings.hideHorizontal != prevHorizontal ||
        g_settings.scrollbarSize != prevSize) {
        RefreshWindows();
    }
}
