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

Mouse-wheel and keyboard scrolling are unaffected.

## Screenshots

Windows default (slim scrollbar in its own gutter):

![Default](https://raw.githubusercontent.com/AmazingBodilyFluids/windhawk-mods/assets/hide-scrollbars-default.png)

With the mod (1px sliver, gutter reclaimed):

![With the mod](https://raw.githubusercontent.com/AmazingBodilyFluids/windhawk-mods/assets/hide-scrollbars-1px.png)

## How it works

The mod hooks `GetSystemMetrics`, `GetSystemMetricsForDpi` and
`GetThemeSysSize`, and reports a size of 1px for `SM_CXVSCROLL` /
`SM_CYHSCROLL` (the vertical/horizontal scrollbar thickness). Code that
lays itself out from that metric - such as Explorer's DirectUI file list
- then shrinks the scrollbar to a 1px sliver. Plain Win32 windows that
draw standard non-client scrollbars are mostly unaffected, because
user32 lays those out from an internal metric table rather than the
exported function.

## Why 1px and not 0

Reporting 0 makes the scrollbar disappear entirely, but a 0-width
scrollbar reads as "not scrollable" to the Windows precision-touchpad
pan handler, so two-finger scrolling stops working in the affected
windows. A 1px sliver is visually negligible and keeps precision-touchpad
scrolling working, so the mod uses 1px and does not expose a 0 option.
Mouse wheel and keyboard scrolling (arrows, PageUp/PageDown, Home/End)
work either way.

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
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideVertical: true
  $name: Hide vertical scrollbars
- hideHorizontal: true
  $name: Hide horizontal scrollbars
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>

#include <string.h>
#include <uxtheme.h>

// A 1px sliver rather than 0: a 0-width scrollbar reads as "not
// scrollable" to the Windows precision-touchpad pan handler, which breaks
// two-finger scrolling. 1px is visually negligible.
constexpr int kScrollbarSize = 1;

struct {
    std::atomic<bool> hideVertical;
    std::atomic<bool> hideHorizontal;
} g_settings;

// Non-null only if this mod (rather than the process) loaded uxtheme.dll.
HMODULE g_loadedUxtheme;

// Whether the host process is explorer.exe (set in Wh_ModInit).
bool g_isExplorer;

// --- hooks ----------------------------------------------------------------

using GetSystemMetrics_t = decltype(&GetSystemMetrics);
GetSystemMetrics_t GetSystemMetrics_Original;

int WINAPI GetSystemMetrics_Hook(int nIndex) {
    if (nIndex == SM_CXVSCROLL && g_settings.hideVertical) {
        return kScrollbarSize;
    }
    if (nIndex == SM_CYHSCROLL && g_settings.hideHorizontal) {
        return kScrollbarSize;
    }
    return GetSystemMetrics_Original(nIndex);
}

using GetSystemMetricsForDpi_t = decltype(&GetSystemMetricsForDpi);
GetSystemMetricsForDpi_t GetSystemMetricsForDpi_Original;

int WINAPI GetSystemMetricsForDpi_Hook(int nIndex, UINT dpi) {
    if (nIndex == SM_CXVSCROLL && g_settings.hideVertical) {
        return kScrollbarSize;
    }
    if (nIndex == SM_CYHSCROLL && g_settings.hideHorizontal) {
        return kScrollbarSize;
    }
    return GetSystemMetricsForDpi_Original(nIndex, dpi);
}

using GetThemeSysSize_t = decltype(&GetThemeSysSize);
GetThemeSysSize_t GetThemeSysSize_Original;

int WINAPI GetThemeSysSize_Hook(HTHEME hTheme, int iSizeId) {
    if (iSizeId == SM_CXVSCROLL && g_settings.hideVertical) {
        return kScrollbarSize;
    }
    if (iSizeId == SM_CYHSCROLL && g_settings.hideHorizontal) {
        return kScrollbarSize;
    }
    return GetThemeSysSize_Original(hTheme, iSizeId);
}

// --- helpers ------------------------------------------------------------

void LoadSettings() {
    g_settings.hideVertical = Wh_GetIntSetting(L"hideVertical") != 0;
    g_settings.hideHorizontal = Wh_GetIntSetting(L"hideHorizontal") != 0;
}

BOOL CALLBACK RefreshChildProc(HWND hChild, LPARAM) {
    // Async: we run on the Windhawk engine thread, the target windows
    // belong to other threads, and we don't need to wait for the relayout.
    SendNotifyMessageW(hChild, WM_THEMECHANGED, 0, 0);
    return TRUE;
}

BOOL CALLBACK RefreshTopProc(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return TRUE;
    }

    // In Explorer only the browser windows host the affected scrollbars,
    // so don't re-theme the shell (tray, desktop, flyouts). In a process
    // added via the inclusion list we have no such knowledge - refresh
    // everything the process owns.
    if (g_isExplorer) {
        WCHAR className[64];
        if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) == 0 ||
            _wcsicmp(className, L"CabinetWClass") != 0) {
            return TRUE;
        }
    }

    // WM_THEMECHANGED doesn't forward to children on its own, so walk them
    // too.
    SendNotifyMessageW(hWnd, WM_THEMECHANGED, 0, 0);
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

    WCHAR exePath[MAX_PATH];
    if (GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath))) {
        const WCHAR* exeName = wcsrchr(exePath, L'\\');
        g_isExplorer = exeName && _wcsicmp(exeName + 1, L"explorer.exe") == 0;
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

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    bool prevVertical = g_settings.hideVertical;
    bool prevHorizontal = g_settings.hideHorizontal;

    LoadSettings();

    // Nothing left to hide: reload so Wh_ModInit can return FALSE and the
    // pass-through hooks go away. Wh_ModUninit's RefreshWindows() restores
    // the layout on the way out.
    if (!g_settings.hideVertical && !g_settings.hideHorizontal) {
        *bReload = TRUE;
        return TRUE;
    }

    if (g_settings.hideVertical != prevVertical ||
        g_settings.hideHorizontal != prevHorizontal) {
        RefreshWindows();
    }

    return TRUE;
}
