// ==WindhawkMod==
// @id              native-shadow-tuner
// @name            Windows Shadows Tuner
// @description     Adjust the size, blur and intensity of native Windows shadows.
// @version         0.6.0
// @author          HaVeN80
// @github          https://github.com/haven80
// @include         dwm.exe
// @architecture    x86-64
// @compilerOptions -lwevtapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows Shadows Tuner

Customize native Windows window shadows: make them lighter or more pronounced,
and adjust their size and blur.

## Examples

**Shadow intensity and size comparison**

![Windows Shadows TUNER comparison](https://i.imgur.com/epfTlMZ.png)

## Setup

`dwm.exe` is a critical system process and Windhawk doesn't inject mods into it
by default. Open Windhawk Settings, go to **Advanced settings**, then
**More advanced settings**, and add `dwm.exe` to the process inclusion list.

![Windhawk advanced settings](https://i.imgur.com/LRhREtJ.png)

## Controls

- **Intensity**: controls the opacity of the native shadow. The default value is
  180%. Set it to 100% to restore the original Windows intensity, 50% for a
  lighter shadow, or 0% to make it invisible.
- **Size and blur**: controls the shadow spread and blur. The default value is
  150%. Set it to 100% to restore the original Windows size.

When both controls are set to 100%, the mod doesn't install any hooks because
the requested appearance is identical to the original Windows appearance.

## How it works
 
The mod hooks a single uDWM function, `CWindowBorder::GetShadowParameters`,
and scales the radius and alpha values it produces. No overlay windows and no
DWM cache manipulation, so disabling the mod restores the stock shadows
immediately — no `dwm.exe` restart, no sign-out, and nothing persisted.

## Compatibility

The mod resolves uDWM functions through Microsoft public symbols. Tested on
Windows 11 25H2 build 26200.9445. On other
builds, if the required uDWM shadow function can't be resolved, the mod logs
this and does not load.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- opacityPercent: 180
  $name: Shadow Intensity (% of the original value)
  $description: From 0 to 300. 100 original, 50 less visible, 150 more visible. 0 to make invisible.
- sizePercent: 150
  $name: Dimension and Blur (% of the original value)
  $description: From 50 to 150. 100 keeps the original value.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winevt.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <cstdint>

namespace {

constexpr int kOpacityMinimum = 0;
constexpr int kOpacityMaximum = 300;
constexpr int kSizeMinimum = 50;
constexpr int kSizeMaximum = 150;

using GetShadowParameters =
    void(__cdecl*)(int, int, float*, float*, float*, float*);

GetShadowParameters getShadowParameters_Original = nullptr;

float opacityScale = 1.0f;
float sizeScale = 1.0f;

void __cdecl GetShadowParameters_Hook(int style,
                                      int dpi,
                                      float* radius1,
                                      float* radius2,
                                      float* alpha1,
                                      float* alpha2) {
    getShadowParameters_Original(
        style,
        dpi,
        radius1,
        radius2,
        alpha1,
        alpha2);

    // In the analyzed uDWM shadow path, value 0 selects the no-shadow case.
    if (style == 0) {
        Wh_Log(L"SHADOW style=%d dpi=%d not scaled (no-shadow style)", style,
               dpi);
        return;
    }

    *radius1 *= sizeScale;
    *radius2 *= sizeScale;

    *alpha1 = std::clamp(
        *alpha1 * opacityScale,
        0.0f,
        1.0f);

    *alpha2 = std::clamp(
        *alpha2 * opacityScale,
        0.0f,
        1.0f);
}

void RequestDwmRefresh() {
    if (HWND hDwm = FindWindowW(L"dwm", nullptr)) {
        PostMessageW(
            hDwm,
            WM_DWMCOLORIZATIONCOLORCHANGED,
            0,
            0);
    } else {
        Wh_Log(L"DWM refresh window wasn't found.");
    }
}

// Returns true if at least two Dwminit warnings (Level=3) were logged in the
// Application event log within the last 60 seconds. DWM logs warnings here when
// it crashes and is restarted by the session manager, so repeated warnings are
// a strong signal that something in the desktop pipeline is unstable.
bool HasMultipleDwminitWarningsInLastMinute() {
    const WCHAR* queryPath = L"Application";
    const WCHAR* query =
        L"*[System[Provider[@Name='Dwminit'] and (Level=3) and "
        L"TimeCreated[timediff(@SystemTime) <= 60000]]]";
    EVT_HANDLE queryHandle = EvtQuery(nullptr,    // Local machine
                                      queryPath,  // Application log
                                      query, EvtQueryChannelPath);
    if (!queryHandle) {
        Wh_Log(L"EvtQuery failed with error: %u", GetLastError());
        return false;
    }
    EVT_HANDLE events[2] = {};
    DWORD returned = 0;
    constexpr DWORD kTimeout = 1000;
    BOOL ok =
        EvtNext(queryHandle, ARRAYSIZE(events), events, kTimeout, 0, &returned);
    if (!ok && GetLastError() != ERROR_NO_MORE_ITEMS) {
        Wh_Log(L"EvtNext failed with error: %u", GetLastError());
    }
    for (DWORD i = 0; i < returned; i++) {
        EvtClose(events[i]);
    }

    EvtClose(queryHandle);
    return ok && returned >= ARRAYSIZE(events);
}

}  // namespace

BOOL Wh_ModInit() {
    if (HasMultipleDwminitWarningsInLastMinute()) {
        Wh_Log(L"Refusing to load: multiple recent Dwminit warnings");
        return FALSE;
    }

    HMODULE module = GetModuleHandleW(L"uDWM.dll");

    if (!module) {
        Wh_Log(L"uDWM.dll is not loaded.");
        return FALSE;
    }

    const int opacityPercent = std::clamp(
        Wh_GetIntSetting(L"opacityPercent"),
        kOpacityMinimum,
        kOpacityMaximum);

    const int sizePercent = std::clamp(
        Wh_GetIntSetting(L"sizePercent"),
        kSizeMinimum,
        kSizeMaximum);

    // Windhawk reloads the mod after every settings change. If both settings
    // request the original Windows appearance, no hooks need to be installed.
    if (opacityPercent == 100 && sizePercent == 100) {
        Wh_Log(
            L"Neutral shadow settings selected; "
            L"no DWM hooks are required.");
        return FALSE;
    }

    opacityScale = opacityPercent / 100.0f;
    sizeScale = sizePercent / 100.0f;

    Wh_Log(
        L"CONFIG opacity=%.2f size=%.2f",
        static_cast<double>(opacityScale),
        static_cast<double>(sizeScale));

    WindhawkUtils::SYMBOL_HOOK udwmDllHooks[] = {
        {
            {
                LR"(private: static void __cdecl CWindowBorder::GetShadowParameters(enum CWindowBorder::ShadowStyle,int,float *,float *,float *,float *))",
                LR"(public: static void __cdecl CWindowBorder::GetShadowParameters(enum CWindowBorder::ShadowStyle,int,float *,float *,float *,float *))",
            },
            &getShadowParameters_Original,
            GetShadowParameters_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(
            module,
            udwmDllHooks,
            ARRAYSIZE(udwmDllHooks))) {
        Wh_Log(L"Required uDWM shadow symbols were not found.");
        return FALSE;
    }

    Wh_Log(
        L"Native shadow symbols resolved and hooks registered.");

    return TRUE;
}

void Wh_ModAfterInit() {
    RequestDwmRefresh();
}

void Wh_ModUninit() {
    // The mod's hooks are removed once Wh_ModBeforeUninit returns, so by the
    // time Wh_ModUninit runs this refresh necessarily rebuilds shadows
    // through DWM's original, unmodified code path.
    RequestDwmRefresh();
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}
