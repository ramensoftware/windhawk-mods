// ==WindhawkMod==
// @id              rounded-corners-when-snapped
// @name            Rounded corners for snapped windows
// @description     Keeps window corners rounded when a window is snapped, without changing the window state
// @version         1.0.0
// @author          Alexey Lavrinenko
// @github          https://github.com/YOUR-GITHUB-USERNAME
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
# Rounded corners for snapped windows

Windows 11 squares off a window's corners as soon as it is snapped. This mod
keeps them rounded.

The window state is untouched: it is really snapped, and Snap Layouts, Snap
Assist and Win+Arrow behave exactly as before. Only the compositor's drawing
changes, and disabling the mod restores the default look immediately.

## ⚠ Important usage note ⚠

This mod needs to hook into `dwm.exe` to work. Please navigate to Windhawk's
Settings > Advanced settings > More advanced settings > Process inclusion list,
and make sure that `dwm.exe` is in the list.

## Why maximized windows are left alone

A window maximized over the whole screen is presented through *direct flip*:
its buffer goes to the display without DWM composing the frame, which is also
why Windows doesn't round it in the first place. Rounded corners drawn for such
a window only show up while something forces composition — the Start menu, a
notification, Alt+Tab — and disappear again the moment the overlay goes away.
In apps that draw their own frame (browsers, Electron apps) that reads as
corners flickering between round and square.

Snapped windows are composed normally, so their corners stay rounded at all
times. Maximized windows are therefore skipped by default. The behaviour can be
turned on with the *Also round maximized windows* option, with the caveat
above — it looks fine in apps with a standard window frame.

## How it works

Two `CTopLevelWindow` methods of `uDWM.dll` are involved:
`GetEffectiveCornerStyle`, which decides the corner style of a composed
surface, and `IsMaximizedOrSnapped`, which is DWM's own answer about that very
surface. Depending on the build, a snapped window is squared off either through
the style or by zeroing the corner radius, so the radius getters of the same
class are hooked as well. Every replacement is gated on `IsMaximizedOrSnapped`,
so the Start menu backdrop, the virtual-desktop switch animation, drag previews
and fullscreen windows are left exactly as DWM wanted them.

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
- roundMaximized: false
  $name: Also round maximized windows
  $description: >-
    Maximized windows are presented without DWM composing them, so their
    rounded corners only appear while the Start menu, a notification or Alt+Tab
    is on screen. Fine with a standard window frame, flickers in apps that draw
    their own.
- roundStyle: round
  $name: Corner style
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
    bool roundMaximized;
    int roundStyle;
    float radius;
} g_settings;

// Captured, not hooked: DWM's own verdict about the surface being composed.
using IsMaximizedOrSnapped_t = bool(WINAPI*)(void* pThis);
IsMaximizedOrSnapped_t IsMaximizedOrSnapped;

// True for a window filling its monitor's work area, as opposed to one snapped
// to part of it.
bool CoversWorkArea(const RECT& rect) {
    MONITORINFO mi{sizeof(MONITORINFO)};
    if (!GetMonitorInfoW(MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST),
                         &mi)) {
        return false;
    }

    LONG width = rect.right - rect.left;
    LONG height = rect.bottom - rect.top;
    LONG workWidth = mi.rcWork.right - mi.rcWork.left;
    LONG workHeight = mi.rcWork.bottom - mi.rcWork.top;

    return width * 100 >= workWidth * 95 && height * 100 >= workHeight * 95;
}

// Builds that square a snapped window by reporting a "don't round" style.
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

// Already DPI-scaled, hence the separate hook. The unscaled getter above feeds
// it on current builds, so it only fires where that isn't the case.
RadiusGetter_t GetDpiAdjustedFloatCornerRadius_Original;
float WINAPI GetDpiAdjustedFloatCornerRadius_Hook(void* pThis) {
    float orig = GetDpiAdjustedFloatCornerRadius_Original(pThis);
    if (orig > 0.0f || !IsMaximizedOrSnapped(pThis)) {
        return orig;
    }

    UINT dpi = GetDpiForSystem();
    float value = g_settings.radius * (dpi ? dpi / 96.0f : 1.0f);
    Wh_Log(L"> dpi adjusted radius 0 -> %f", value);
    return value;
}

// The window border is where the rounding actually becomes visible, and the
// only place with a rectangle to tell a snapped window from a maximized one.
using SetBorderParameters_t = long(WINAPI*)(void* pThis,
                                            const RECT& borderRect,
                                            float cornerRadius,
                                            int dpi,
                                            const void* color,
                                            int borderStyle,
                                            int shadowStyle);
SetBorderParameters_t SetBorderParameters_Original;
long WINAPI SetBorderParameters_Hook(void* pThis,
                                     const RECT& borderRect,
                                     float cornerRadius,
                                     int dpi,
                                     const void* color,
                                     int borderStyle,
                                     int shadowStyle) {
    if (cornerRadius > 0.0f && !g_settings.roundMaximized &&
        CoversWorkArea(borderRect)) {
        // Maximized: DWM presents it without composing, so a rounded border
        // would only show while something else is drawn on top.
        Wh_Log(L"> maximized, leaving the border square");
        cornerRadius = 0.0f;
    }
    return SetBorderParameters_Original(pThis, borderRect, cornerRadius, dpi,
                                        color, borderStyle, shadowStyle);
}

void LoadSettings() {
    WindhawkUtils::StringSetting style =
        WindhawkUtils::StringSetting::make(L"roundStyle");
    g_settings.roundStyle =
        wcscmp(style.get(), L"small") == 0 ? DWMWCP_ROUNDSMALL : DWMWCP_ROUND;

    g_settings.roundMaximized = Wh_GetIntSetting(L"roundMaximized") != 0;

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
        // Keeps maximized windows square unless the user asks otherwise.
        {
            {LR"(public: long __cdecl CWindowBorder::SetBorderParameters(struct tagRECT const &,float,int,struct _D3DCOLORVALUE const &,enum CWindowBorder::BorderStyle,enum CWindowBorder::ShadowStyle))"},
            &SetBorderParameters_Original,
            SetBorderParameters_Hook,
            true,  // Optional.
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
