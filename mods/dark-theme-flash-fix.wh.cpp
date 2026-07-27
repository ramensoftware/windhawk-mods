// ==WindhawkMod==
// @id dark-theme-flash-fix
// @name Dark Theme Flash Fix
// @description Eliminates white flash when opening windows in dark mode via DWM cloaking
// @version 1.0
// @author grandrange
// @github https://github.com/grandrange
// @compilerOptions -lGdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Dark Theme Flash Fix

Eliminates the white flash (flashbang) when opening windows in dark mode.

Instead of replacing system colors globally (which breaks legacy apps like
Win+R or edit controls), this mod temporarily hides the window via DWM cloak
so the white initial frame is never seen by the user.

Works with Win32 applications using ShowWindow.

## How it works

```
App calls ShowWindow(hWnd)
  → Mod cloaks the window (DWM cloak, invisible to user)
  → Original ShowWindow is called
  → App renders: white background → dark UI (user sees nothing)
  → After N ms, mod uncloaks the window
  → User sees a fully rendered dark window no flash
```

## Usage

1. Install the mod
2. Open mod settings → **Advanced** → **Process inclusion**
3. Add target processes, e.g. `process.exe` (one per line)
4. Restart the target applications

## Settings

- **DelayMs** (50–2000, default 150):
  How long the window stays invisible after being shown.
  Increase if flashes are still visible.
  Decrease if windows appear too slow.

## Notes

- The mod does nothing by default (no `@include`).
  Process inclusion is required for it to activate.
- The delay adds a small perceived latency.
  I recommend 100–200 ms it blends with the native Windows 11
  window animation and feels natural.
- In some programs, the mod may break the window appearance animation.
  I do not recommend applying the mod globally.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- DelayMs: 150
  $name: Delay (ms)
  $description: >-
    How long the window stays invisible after being shown.
    Recommended: 100-300.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>

#define DWMWA_CLOAK 13

static int g_delayMs = 150;
static UINT_PTR g_nextTimerId = 1000;

static HMODULE g_hDwmApi = nullptr;
using DwmSetWindowAttribute_t = HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD);
static DwmSetWindowAttribute_t pDwmSetWindowAttribute = nullptr;

static decltype(&ShowWindow) ShowWindow_Original;

static void LoadSettings() {
    g_delayMs = Wh_GetIntSetting(L"DelayMs");
    if (g_delayMs < 50) g_delayMs = 50;
    if (g_delayMs > 2000) g_delayMs = 2000;
}

static VOID CALLBACK UncloakTimerProc(HWND hwnd, UINT, UINT_PTR idEvent, DWORD) {
    if (!hwnd || !IsWindow(hwnd))
        return;

    if (pDwmSetWindowAttribute) {
        BOOL cloak = FALSE;
        pDwmSetWindowAttribute(hwnd, DWMWA_CLOAK, &cloak, sizeof(cloak));
    }

    KillTimer(hwnd, idEvent);
}

static bool IsShowCommand(int nCmdShow) {
    return nCmdShow == SW_SHOW || nCmdShow == SW_SHOWNORMAL ||
           nCmdShow == SW_RESTORE || nCmdShow == SW_SHOWNA ||
           nCmdShow == SW_SHOWNOACTIVATE || nCmdShow == SW_MAXIMIZE;
}

static BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    bool showing = IsShowCommand(nCmdShow);
    bool wasVisible = IsWindowVisible(hWnd);

    if (showing && !wasVisible && pDwmSetWindowAttribute) {
        BOOL cloak = TRUE;
        pDwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &cloak, sizeof(cloak));

        BOOL result = ShowWindow_Original(hWnd, nCmdShow);

        SetTimer(hWnd, ++g_nextTimerId, (UINT)g_delayMs, UncloakTimerProc);

        return result;
    }

    return ShowWindow_Original(hWnd, nCmdShow);
}

BOOL Wh_ModInit() {
    LoadSettings();

    g_hDwmApi = LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (g_hDwmApi) {
        pDwmSetWindowAttribute = reinterpret_cast<DwmSetWindowAttribute_t>(
            GetProcAddress(g_hDwmApi, "DwmSetWindowAttribute"));
    }

    if (!pDwmSetWindowAttribute)
        return FALSE;

    if (!WindhawkUtils::SetFunctionHook(
            reinterpret_cast<void*>(ShowWindow),
            reinterpret_cast<void*>(ShowWindow_Hook),
            reinterpret_cast<void**>(&ShowWindow_Original)))
        return FALSE;

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    if (g_hDwmApi) {
        FreeLibrary(g_hDwmApi);
        g_hDwmApi = nullptr;
    }
}
