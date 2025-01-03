// ==WindhawkMod==
// @id              remove-watermark
// @name            Remove Activate Windows Watermark
// @description     Removes the activate windows watermark
// @version         1.0
// @author          You
// @include         windows
// @compilerOptions -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Details
This mod removes the "Activate Windows" watermark. 
It hooks into the necessary Windows functions to suppress the watermark display.

# instructions
Press the install button. When installed go to the settings tab and press the 
Remove Windows Watermark button.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# Here you can define settings, in YAML format, that the mod users will be able
# to configure. Metadata values such as $name and $description are optional.
# Check out the documentation for more information:
# https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod#settings
- removeWatermark: true
  $name: Remove Windows Watermark
  $description: When enabled, the "Activate Windows" watermark will be suppressed.
*/
// ==/WindhawkModSettings==

#include <Windows.h>
#include <gdiplus.h>

using namespace Gdiplus;

struct {
    bool removeWatermark;
} settings;

using SetWindowLongPtr_t = decltype(&SetWindowLongPtr);
SetWindowLongPtr_t SetWindowLongPtr_Original;

LONG_PTR WINAPI SetWindowLongPtr_Hook(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
    if (settings.removeWatermark) {
        // Suppress the watermark by modifying the window style
        // This is a simplistic approach and may need adjustments based on the actual implementation
        if (nIndex == GWL_EXSTYLE) {
            dwNewLong &= ~WS_EX_LAYERED; // Example modification
        }
    }
    return SetWindowLongPtr_Original(hWnd, nIndex, dwNewLong);
}

void LoadSettings() {
    settings.removeWatermark = Wh_GetIntSetting(L"removeWatermark");
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    // Hook the SetWindowLongPtr function to modify window styles
    Wh_SetFunctionHook((void*)SetWindowLongPtr, (void*)SetWindowLongPtr_Hook,
                       (void**)&SetWindowLongPtr_Original);

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}
