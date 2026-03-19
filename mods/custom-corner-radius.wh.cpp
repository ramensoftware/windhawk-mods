// ==WindhawkMod==
// @id              custom-corner-radius
// @name            Custom Window Corner Radius
// @description     Increases window corner radius beyond the default 8px in Windows 11
// @version         1.0.0
// @author          kanak-buet19
// @homepage        https://github.com/kanak-buet19
// @github          kanak-buet19
// @include         dwm.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- radius: 12
  $name: Corner Radius
  $description: Corner radius in pixels. Default Win11 is 8. Recommended range is 10-20. Values above 20 may cause visual artifacts depending on your DPI scaling.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Custom Window Corner Radius

Makes Windows 11 app window corners more rounded than the default 8px by hooking
into `dwm.exe` via the `uDWM.dll` symbol table.

## Setup (Required)
Before enabling this mod you must do the following or it will not work:

1. Open Windhawk → **Settings** → **Advanced settings**
2. **Uncheck** "Exclude critical system processes"
3. Add `dwm.exe` to the **Process inclusion list**
4. Click **Save and restart Windhawk**

## Settings
- **Corner Radius**: Set any integer value above 8 to increase rounding.
  Recommended range: **10–20**. Values above 20 may cause shadow artifacts
  depending on your Windows build and DPI scaling.

## How it works
Hooks three internal `uDWM.dll` functions responsible for returning the corner
radius float value to DWM:
- `CTopLevelWindow::GetRadiusFromCornerStyle`
- `CTopLevelWindow::GetFloatCornerRadiusForCurrentStyle`
- `CTopLevelWindow::GetDpiAdjustedFloatCornerRadius`

Popups and flyouts (taskbar, wifi, volume, context menus) use a smaller radius
(4px) and are detected and left unchanged to avoid shadow artifacts on those.

## Notes
- Symbols are downloaded automatically from Microsoft's public symbol server
  on first load. This may take a few seconds.
- Disabling the mod instantly restores default behavior — no system files
  are modified.
- Tested on Windows 11 23H2 at 115% DPI scaling on a 1440p display.
- If corners don't change after updating the radius setting, use the
  **Settings tab** in Windhawk (not the code) to update the value.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>

struct {
    int radius;
} settings;

typedef float (*FloatFunc_t)(void* pThis);

FloatFunc_t GetRadiusFromCornerStyle_orig = nullptr;
FloatFunc_t GetFloatCornerRadiusForCurrentStyle_orig = nullptr;
FloatFunc_t GetDpiAdjustedFloatCornerRadius_orig = nullptr;

#define DEFAULT_LARGE_RADIUS 8.0f
#define EPSILON 0.5f

float GetRadiusFromCornerStyle_hook(void* pThis) {
    float orig = GetRadiusFromCornerStyle_orig(pThis);
    if (orig >= DEFAULT_LARGE_RADIUS - EPSILON)
        return (float)settings.radius;
    return orig;
}

float GetFloatCornerRadius_hook(void* pThis) {
    float orig = GetFloatCornerRadiusForCurrentStyle_orig(pThis);
    if (orig >= DEFAULT_LARGE_RADIUS - EPSILON)
        return (float)settings.radius;
    return orig;
}

float GetDpiAdjustedFloatCornerRadius_hook(void* pThis) {
    float orig = GetDpiAdjustedFloatCornerRadius_orig(pThis);
    // Values >= 6.0 are DPI-adjusted large radius (regular windows)
    // Values < 6.0 are small radius (popups/flyouts) - leave unchanged
    if (orig >= 6.0f) {
        float scale = (float)settings.radius / DEFAULT_LARGE_RADIUS;
        return orig * scale;
    }
    return orig;
}

void LoadSettings() {
    settings.radius = Wh_GetIntSetting(L"radius");
    if (settings.radius < 1) settings.radius = 8;
    if (settings.radius > 60) settings.radius = 60;
}

BOOL Wh_ModInit() {
    LoadSettings();
    Wh_Log(L"Target radius: %d", settings.radius);

    HMODULE hUDWM = GetModuleHandleW(L"uDWM.dll");
    if (!hUDWM) {
        Wh_Log(L"uDWM.dll not found - make sure dwm.exe is in the process inclusion list");
        return FALSE;
    }

    WH_FIND_SYMBOL_OPTIONS opts = { sizeof(opts) };
    WH_FIND_SYMBOL sym = {};

    void* addr_1 = nullptr;
    void* addr_2 = nullptr;
    void* addr_3 = nullptr;

    HANDLE hFind = Wh_FindFirstSymbol(hUDWM, &opts, &sym);
    if (hFind) {
        do {
            if (sym.symbol) {
                if (wcsstr(sym.symbol, L"GetRadiusFromCornerStyle"))
                    addr_1 = sym.address;
                else if (wcsstr(sym.symbol, L"GetFloatCornerRadiusForCurrentStyle"))
                    addr_2 = sym.address;
                else if (wcsstr(sym.symbol, L"GetDpiAdjustedFloatCornerRadius"))
                    addr_3 = sym.address;
            }
        } while (Wh_FindNextSymbol(hFind, &sym));
        Wh_FindCloseSymbol(hFind);
    }

    int hooked = 0;

    if (addr_1 && Wh_SetFunctionHook(addr_1, (void*)GetRadiusFromCornerStyle_hook, (void**)&GetRadiusFromCornerStyle_orig)) {
        Wh_Log(L"Hooked GetRadiusFromCornerStyle"); hooked++;
    }
    if (addr_2 && Wh_SetFunctionHook(addr_2, (void*)GetFloatCornerRadius_hook, (void**)&GetFloatCornerRadiusForCurrentStyle_orig)) {
        Wh_Log(L"Hooked GetFloatCornerRadiusForCurrentStyle"); hooked++;
    }
    if (addr_3 && Wh_SetFunctionHook(addr_3, (void*)GetDpiAdjustedFloatCornerRadius_hook, (void**)&GetDpiAdjustedFloatCornerRadius_orig)) {
        Wh_Log(L"Hooked GetDpiAdjustedFloatCornerRadius"); hooked++;
    }

    Wh_Log(L"Hooked %d functions. Radius: %d", hooked, settings.radius);
    return hooked > 0 ? TRUE : FALSE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"Settings updated. New radius: %d", settings.radius);
}

void Wh_ModUninit() {
    Wh_Log(L"Unloaded.");
}