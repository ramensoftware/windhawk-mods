// ==WindhawkMod==
// @id              rounded-corners-when-snapped
// @name            Rounded corners when snapped or maximized
// @description     Keeps window corners rounded when a window is snapped or maximized, without changing the window state
// @version         1.0.0
// @author          Alexey Lavrinenko
// @github          https://github.com/leshaalexey
// @license         GPL-3.0
// @include         dwm.exe
// @architecture    x86-64
// @compilerOptions -lwevtapi
// ==/WindhawkMod==

// HasMultipleDwminitWarningsInLastMinute() is taken from the Custom Window
// Corner Radius mod by m417z, which is published under the GNU General Public
// License v3.0, so this mod carries the same license.

// ==WindhawkModReadme==
/*
# Rounded corners when snapped or maximized

Windows 11 squares off a window's corners as soon as it is maximized or
snapped. This mod keeps them rounded, the way macOS does.

The window state is untouched: it is really snapped, really maximized, and
Snap Layouts, Snap Assist, Win+Arrow and the maximize button all behave exactly
as before. Only the compositor's drawing changes, and disabling the mod
restores the default look immediately.

## ⚠ Important usage note ⚠

This mod needs to hook into `dwm.exe` to work. Please navigate to Windhawk's
Settings > Advanced settings > More advanced settings > Process inclusion list,
and make sure that `dwm.exe` is in the list.

## How it works

Windows are squared off in one of two ways, depending on the build: either the
corner style comes out as `DWMWCP_DONOTROUND`, or the style stays rounded and
the corner radius is zeroed further down the pipeline. The mod covers both, by
hooking `CTopLevelWindow::GetEffectiveCornerStyle` and the corner radius
getters of the same class.

Every replacement is gated on `CTopLevelWindow::IsMaximizedOrSnapped` — DWM's
own answer about the very surface being composed. The Start menu backdrop, the
virtual-desktop switch animation, drag previews and fullscreen windows answer
`false` and are left exactly as DWM wanted them, so no stray outlines appear.

A forced radius is scaled through `CWindowData::ScaleForDpi`, the same way DWM
scales its own, so it comes out the right size on every monitor of a mixed-DPI
setup.

## Compatibility

Other mods and tools that patch DWM corners hook the same functions and will
fight with this one. Don't run it together with **Custom Window Corner
Radius**, **Disable rounded corners in Windows 11**, ExplorerPatcher's rounded
corner option, StartAllBack or Win11DisableRoundedCorners.

## Mod authorship
 
If this pull request introduces a new mod, please complete the section below.
 
This mod was created by:
 
- - [ ] The submitter, without AI assistance
- - [x] The submitter, with AI assistance
- - [x] Claude
- - [ ] ChatGPT
- - [ ] Gemini
- - [ ] Another AI (please specify): 
- - [ ] Other (please specify): 
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- roundStyle: round
  $name: Corner style
  $description: >-
    Which rounding to apply. "Normal" matches an ordinary unsnapped window.
  $options:
  - round: Normal
  - small: Small, like a menu
- radius: 8
  $name: Fallback radius
  $description: >-
    Only used on builds where DWM zeroes the corner radius instead of changing
    the corner style. Windows 11 uses 8 pixels for normal rounding and 4 for
    small rounding.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <winevt.h>

struct {
    int roundStyle;
    float radius;
} g_settings;

// Captured, not hooked: DWM's own verdict about the surface being composed.
using IsMaximizedOrSnapped_t = bool(WINAPI*)(void* pThis);
IsMaximizedOrSnapped_t IsMaximizedOrSnapped;

// Captured, not hooked: DWM's own per-window DPI scaling, so a forced radius
// comes out the right size on every monitor.
using GetWindowData_t = void*(WINAPI*)(void* pThis);
GetWindowData_t GetWindowData_Original;

using ScaleForDpi_t = unsigned int(WINAPI*)(void* pThis, unsigned int value);
ScaleForDpi_t ScaleForDpi_Original;

float ScaledRadius(void* pThis) {
    if (GetWindowData_Original && ScaleForDpi_Original) {
        if (void* data = GetWindowData_Original(pThis)) {
            return static_cast<float>(ScaleForDpi_Original(
                data, static_cast<unsigned int>(g_settings.radius)));
        }
    }
    // Fall back to the system DPI, which is only wrong on mixed-DPI setups.
    UINT dpi = GetDpiForSystem();
    return g_settings.radius * (dpi ? dpi / 96.0f : 1.0f);
}

// Builds that square a maximized window by reporting a "don't round" style.
using GetEffectiveCornerStyle_t = int(WINAPI*)(void* pThis);
GetEffectiveCornerStyle_t GetEffectiveCornerStyle_Original;
int WINAPI GetEffectiveCornerStyle_Hook(void* pThis) {
    int orig = GetEffectiveCornerStyle_Original(pThis);
    if (orig == DWMWCP_DONOTROUND && IsMaximizedOrSnapped(pThis)) {
        Wh_Log(L"> DONOTROUND -> %d", g_settings.roundStyle);
        return g_settings.roundStyle;
    }
    return orig;
}

// Builds that keep the rounded style and zero the radius instead. Which getter
// does the zeroing differs between builds, and a downstream getter may already
// see a value replaced upstream, so every replacement is guarded by "the radius
// is still zero".
using RadiusGetter_t = float(WINAPI*)(void* pThis);

RadiusGetter_t GetRadiusFromCornerStyle_Original;
float WINAPI GetRadiusFromCornerStyle_Hook(void* pThis) {
    float orig = GetRadiusFromCornerStyle_Original(pThis);
    if (orig <= 0.0f && IsMaximizedOrSnapped(pThis)) {
        Wh_Log(L"> radius 0 -> %f", g_settings.radius);
        return g_settings.radius;
    }
    return orig;
}

RadiusGetter_t GetFloatCornerRadiusForCurrentStyle_Original;
float WINAPI GetFloatCornerRadiusForCurrentStyle_Hook(void* pThis) {
    float orig = GetFloatCornerRadiusForCurrentStyle_Original(pThis);
    if (orig <= 0.0f && IsMaximizedOrSnapped(pThis)) {
        Wh_Log(L"> current style radius 0 -> %f", g_settings.radius);
        return g_settings.radius;
    }
    return orig;
}

// Returns an already DPI-scaled value, so the replacement has to be scaled too.
RadiusGetter_t GetDpiAdjustedFloatCornerRadius_Original;
float WINAPI GetDpiAdjustedFloatCornerRadius_Hook(void* pThis) {
    float orig = GetDpiAdjustedFloatCornerRadius_Original(pThis);
    if (orig <= 0.0f && IsMaximizedOrSnapped(pThis)) {
        float scaled = ScaledRadius(pThis);
        Wh_Log(L"> dpi adjusted radius 0 -> %f", scaled);
        return scaled;
    }
    return orig;
}

void LoadSettings() {
    WindhawkUtils::StringSetting style =
        WindhawkUtils::StringSetting::make(L"roundStyle");
    g_settings.roundStyle =
        wcscmp(style.get(), L"small") == 0 ? DWMWCP_ROUNDSMALL : DWMWCP_ROUND;

    int radius = Wh_GetIntSetting(L"radius");
    if (radius < 1) {
        radius = 1;
    } else if (radius > 40) {
        radius = 40;
    }
    g_settings.radius = static_cast<float>(radius);
}

// Returns true if at least two Dwminit warnings (Level=3) were logged in the
// Application event log within the last 60 seconds. DWM logs warnings here when
// it crashes and is restarted by the session manager, so repeated warnings are
// a strong signal that something in the desktop pipeline is unstable, and a mod
// hooking the compositor should stay out of the way.
bool HasMultipleDwminitWarningsInLastMinute() {
    const WCHAR* queryPath = L"Application";
    const WCHAR* query =
        L"*[System[Provider[@Name='Dwminit'] and (Level=3) and "
        L"TimeCreated[timediff(@SystemTime) <= 60000]]]";

    EVT_HANDLE queryHandle =
        EvtQuery(nullptr, queryPath, query, EvtQueryChannelPath);
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

BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (HasMultipleDwminitWarningsInLastMinute()) {
        Wh_Log(L"Refusing to load: multiple recent Dwminit warnings");
        return FALSE;
    }

    LoadSettings();

    HMODULE udwm = GetModuleHandle(L"udwm.dll");
    if (!udwm) {
        Wh_Log(L"udwm.dll isn't loaded");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK udwmDllHooks[] = {
        // The filter. Without it there's no way to tell app windows from
        // internal DWM surfaces, and rounding those leaves visible outlines, so
        // this one is mandatory.
        {
            {LR"(public: bool __cdecl CTopLevelWindow::IsMaximizedOrSnapped(void)const )"},
            &IsMaximizedOrSnapped,
            nullptr,  // Capture only.
        },
        {
            {LR"(private: enum CORNER_STYLE __cdecl CTopLevelWindow::GetEffectiveCornerStyle(void))"},
            &GetEffectiveCornerStyle_Original,
            GetEffectiveCornerStyle_Hook,
        },
        {
            {LR"(private: float __cdecl CTopLevelWindow::GetRadiusFromCornerStyle(void))"},
            &GetRadiusFromCornerStyle_Original,
            GetRadiusFromCornerStyle_Hook,
        },
        // The next two come in const and non-const, private and public flavors
        // depending on the build, and are missing entirely in older ones. Only
        // CTopLevelWindow overloads are listed on purpose: the hooks pass
        // `this` to IsMaximizedOrSnapped, so a same-named method on another
        // class must not match.
        {
            {
                LR"(private: float __cdecl CTopLevelWindow::GetFloatCornerRadiusForCurrentStyle(void))",
                LR"(private: float __cdecl CTopLevelWindow::GetFloatCornerRadiusForCurrentStyle(void)const )",
                LR"(public: float __cdecl CTopLevelWindow::GetFloatCornerRadiusForCurrentStyle(void))",
                LR"(public: float __cdecl CTopLevelWindow::GetFloatCornerRadiusForCurrentStyle(void)const )",
            },
            &GetFloatCornerRadiusForCurrentStyle_Original,
            GetFloatCornerRadiusForCurrentStyle_Hook,
            true,  // Optional.
        },
        {
            {
                LR"(private: float __cdecl CTopLevelWindow::GetDpiAdjustedFloatCornerRadius(void))",
                LR"(private: float __cdecl CTopLevelWindow::GetDpiAdjustedFloatCornerRadius(void)const )",
                LR"(public: float __cdecl CTopLevelWindow::GetDpiAdjustedFloatCornerRadius(void))",
                LR"(public: float __cdecl CTopLevelWindow::GetDpiAdjustedFloatCornerRadius(void)const )",
            },
            &GetDpiAdjustedFloatCornerRadius_Original,
            GetDpiAdjustedFloatCornerRadius_Hook,
            true,  // Optional.
        },
        // Used only to scale a forced radius the way DWM would.
        {
            {LR"(public: class CWindowData * __cdecl CTopLevelWindow::GetWindowData(void)const )"},
            &GetWindowData_Original,
            nullptr,  // Capture only.
            true,     // Optional - falls back to the system DPI.
        },
        {
            {LR"(public: unsigned int __cdecl CWindowData::ScaleForDpi(unsigned int)const )"},
            &ScaleForDpi_Original,
            nullptr,  // Capture only.
            true,     // Optional - falls back to the system DPI.
        },
    };

    if (!HookSymbols(udwm, udwmDllHooks, ARRAYSIZE(udwmDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L">");
}
