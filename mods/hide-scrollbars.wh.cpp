// ==WindhawkMod==
// @id              hide-scrollbars
// @name            Hide Scrollbars
// @description     Hide vertical/horizontal scrollbars in selected processes (default: File Explorer) and reclaim the gutter space
// @version         1.0.0
// @author          AmazingBodilyFluids
// @github          https://github.com/AmazingBodilyFluids
// @include         *
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Scrollbars

Removes the vertical and/or horizontal scrollbars from windows in the
processes you choose. By default it targets `explorer.exe`, so the File
Explorer file list loses its scrollbar and the freed strip is used by the
content instead of sitting empty.

Mouse-wheel and keyboard scrolling keep working. Precision-touchpad
two-finger scrolling keeps working at the default scrollbar size of 1px;
setting the size to 0 breaks it (see the limitation below).

## How it works

The mod hooks `GetSystemMetrics`, `GetSystemMetricsForDpi` and
`GetThemeSysSize`, and reports a reduced size for `SM_CXVSCROLL` /
`SM_CYHSCROLL` (the vertical/horizontal scrollbar thickness). Controls
that lay themselves out from the scrollbar size then shrink it
accordingly.

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

## Notes and limitations

- The metric override is process-wide. Every classic control in a
  targeted process loses its scrollbar, not only Explorer's file list.
- Apps rendered with newer UI frameworks (WinUI/XAML) that do not read
  these metrics are unaffected.
- With the default size of `1`, a 1px sliver remains where the scrollbar
  was. Set **Scrollbar size** to `0` for full removal (see the touchpad
  limitation above).
- If a control in a targeted process misbehaves, remove that process
  from the list.

## Settings

- **Hide vertical scrollbars** - toggle the vertical bar.
- **Hide horizontal scrollbars** - toggle the horizontal bar.
- **Scrollbar size** - reported thickness in pixels. `1` (default) keeps
  precision-touchpad scrolling working; `0` hides the bar fully but
  breaks two-finger touchpad scroll.
- **Processes** - executable names the mod applies to, one per line. The
  mod is loaded into every process but stays inactive outside this list.
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
    Reported scrollbar thickness in pixels. 1 (default) keeps
    precision-touchpad two-finger scrolling working while leaving only a
    1px sliver. 0 hides the scrollbar completely but breaks
    precision-touchpad scrolling (mouse wheel and keyboard still work).
- processList:
  - explorer.exe
  $name: Processes
  $description: Executable names (with .exe) the mod applies to, one per line.
*/
// ==/WindhawkModSettings==

#include <uxtheme.h>

#include <string>
#include <vector>

struct {
    bool hideVertical;
    bool hideHorizontal;
    int scrollbarSize;
} g_settings;

std::vector<std::wstring> g_processList;

// --- hooks ----------------------------------------------------------------

using GetSystemMetrics_t = int(WINAPI*)(int);
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

using GetSystemMetricsForDpi_t = int(WINAPI*)(int, UINT);
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

using GetThemeSysSize_t = int(WINAPI*)(HTHEME, int);
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
    g_settings.hideVertical = Wh_GetIntSetting(L"hideVertical");
    g_settings.hideHorizontal = Wh_GetIntSetting(L"hideHorizontal");
    g_settings.scrollbarSize = Wh_GetIntSetting(L"scrollbarSize");
    if (g_settings.scrollbarSize < 0) {
        g_settings.scrollbarSize = 0;
    }

    g_processList.clear();
    for (int i = 0;; i++) {
        PCWSTR value = Wh_GetStringSetting(L"processList[%d]", i);
        bool done = !*value;
        if (!done) {
            std::wstring s = value;
            size_t a = s.find_first_not_of(L" \t");
            size_t b = s.find_last_not_of(L" \t");
            if (a != std::wstring::npos) {
                g_processList.push_back(s.substr(a, b - a + 1));
            }
        }
        Wh_FreeStringSetting(value);
        if (done) {
            break;
        }
    }
}

bool IsTargetProcess() {
    WCHAR path[MAX_PATH];
    DWORD n = GetModuleFileName(nullptr, path, ARRAYSIZE(path));
    if (n == 0 || n >= ARRAYSIZE(path)) {
        return false;
    }

    PCWSTR base = wcsrchr(path, L'\\');
    base = base ? base + 1 : path;

    for (const auto& name : g_processList) {
        if (_wcsicmp(base, name.c_str()) == 0) {
            return true;
        }
    }
    return false;
}

BOOL CALLBACK RefreshChildProc(HWND hChild, LPARAM) {
    SetWindowPos(hChild, nullptr, 0, 0, 0, 0,
                 SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOACTIVATE);
    return TRUE;
}

BOOL CALLBACK RefreshTopProc(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return TRUE;
    }

    SendMessageTimeout(hWnd, WM_THEMECHANGED, 0, 0, SMTO_ABORTIFHUNG, 200,
                       nullptr);
    SetWindowPos(hWnd, nullptr, 0, 0, 0, 0,
                 SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    EnumChildWindows(hWnd, RefreshChildProc, 0);
    return TRUE;
}

void RefreshWindows() {
    EnumWindows(RefreshTopProc, 0);
}

// --- mod entry points -------------------------------------------------

BOOL Wh_ModInit() {
    LoadSettings();

    if (!IsTargetProcess()) {
        Wh_Log(L"hide-scrollbars: process not targeted, staying inactive");
        return FALSE;
    }

    Wh_SetFunctionHook((void*)GetSystemMetrics, (void*)GetSystemMetrics_Hook,
                       (void**)&GetSystemMetrics_Original);

    if (HMODULE user32 = GetModuleHandle(L"user32.dll")) {
        if (void* p = (void*)GetProcAddress(user32, "GetSystemMetricsForDpi")) {
            Wh_SetFunctionHook(p, (void*)GetSystemMetricsForDpi_Hook,
                               (void**)&GetSystemMetricsForDpi_Original);
        }
    }

    if (HMODULE uxtheme = LoadLibrary(L"uxtheme.dll")) {
        if (void* p = (void*)GetProcAddress(uxtheme, "GetThemeSysSize")) {
            Wh_SetFunctionHook(p, (void*)GetThemeSysSize_Hook,
                               (void**)&GetThemeSysSize_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    RefreshWindows();
}

void Wh_ModUninit() {
    // Neutralise the hooks before Windhawk removes them, then relayout so
    // the scrollbars come back.
    g_settings.hideVertical = false;
    g_settings.hideHorizontal = false;
    RefreshWindows();
}

void Wh_ModSettingsChanged(BOOL* bReload) {
    // A processList change alters which processes should load the mod, so
    // ask Windhawk for a full reload rather than a live settings swap.
    *bReload = TRUE;
}
