// ==WindhawkMod==
// @id              custom-corner-radius
// @name            Custom Window Corner Radius
// @description     Customizes window corner radius in Windows 11, making corners more or less rounded
// @version         1.2
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         dwm.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lwevtapi
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Custom Window Corner Radius

Customizes Windows 11 app window corner radius. Make corners more rounded than
the default 8px, or reduce the radius for less rounded or completely sharp
corners.

The mod was [originally
submitted](https://github.com/ramensoftware/windhawk-mods/pull/3587) by
[Kanak415](https://github.com/kanak-buet19).

![Screenshot](https://i.imgur.com/mMGkBwc.png)

## ⚠ Important usage note ⚠

This mod needs to hook into `dwm.exe` to work. Please navigate to Windhawk's
Settings > Advanced settings > More advanced settings > Process inclusion list,
and make sure that `dwm.exe` is in the list.

![Advanced settings screenshot](https://i.imgur.com/LRhREtJ.png)

## Additional notes

- Some elements, such as context menus, use a smaller radius (4px by default).
  This can be customized separately with the "Small corner radius" option.
- Standard tooltips can be customized separately with the "Tooltip corner
  radius" option.
- Some elements, such as the taskbar, the Start menu, and the notification
  center, are unaffected by this mod. Some of them can be customized using other
  mods, such as Windows 11 Taskbar Styler.
- Disabling the mod instantly restores default behavior - no system files are
  modified.

## Compatibility

- When using this mod alongside Translucent Flyouts, set its `CornerType` option
  to `0` ("Don't Change") to prevent conflicts between the two.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- radius: 12
  $name: Corner radius
  $description: >-
    Corner radius in pixels. Default Win11 is 8. Use smaller values (e.g. 4 or
    0) for less rounded or sharp corners, or larger values (e.g. 10-20) for more
    rounded corners. Values above 20 may cause visual artifacts depending on
    your DPI scaling.

    Set to -1 to keep the original radius.
- smallRadius: 6
  $name: Small corner radius
  $description: >-
    Corner radius for elements that use a smaller radius, such as context menus.
    Default Win11 is 4.

    Set to -1 to keep the original radius.
- tooltipRadius: -1
  $name: Tooltip corner radius
  $description: >-
    Corner radius for standard tooltips. Note that this doesn't affect modern
    (WinUI) tooltips. Values above 8 may cause visual artifacts depending on
    your DPI scaling.

    Set to -1 to leave tooltips unchanged.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <winevt.h>

#include <cmath>
#include <limits>
#include <regex>
#include <string_view>

struct {
    float radius;
    float smallRadius;
    float tooltipRadius;
} g_settings;

using GetWindowData_t = void*(WINAPI*)(void* pThis);
GetWindowData_t GetWindowData_Original;

// Captured (not hooked) address of CWindowData::IsGhostWindow. We disassemble
// its first few instructions at init time to recover the HWND member offset.
void* IsGhostWindow_Func;

// HWND offset within CWindowData. Recovered at init time from the first
// `mov rcx, qword ptr [rcx+disp]` instruction in CWindowData::IsGhostWindow,
// which loads its HWND member as the first argument to GetPropW. Stays at
// SIZE_MAX if the pattern can't be matched, in which case HWND lookup is
// disabled and tooltip-specific behavior gracefully degrades.
size_t g_windowDataHwndOffset = SIZE_MAX;

// Scans the first `limit` instructions of `func` for a match against `regex`
// and returns the value of the first capture group parsed as hex. Mirrors the
// helper used by taskbar-button-scroll / taskbar-icon-size.
size_t OffsetFromAssemblyRegex(void* func,
                               size_t defValue,
                               std::regex regex,
                               int limit = 30) {
    BYTE* p = (BYTE*)func;
    for (int i = 0; i < limit; i++) {
        WH_DISASM_RESULT result;
        if (!Wh_Disasm(p, &result)) {
            break;
        }

        p += result.length;

        std::string_view s = result.text;
        if (s == "ret") {
            break;
        }

        std::match_results<std::string_view::const_iterator> match;
        if (std::regex_match(s.begin(), s.end(), match, regex)) {
            // Wh_Log(L"%S", result.text);
            return std::stoull(match[1], nullptr, 16);
        }
    }

    Wh_Log(L"Failed for %p", func);
    return defValue;
}

HWND HwndFromTopLevelWindow(void* pThis) {
    if (g_windowDataHwndOffset == SIZE_MAX || !GetWindowData_Original) {
        return nullptr;
    }
    void* pData = GetWindowData_Original(pThis);
    if (!pData) {
        return nullptr;
    }
    HWND hwnd = *(HWND*)((BYTE*)pData + g_windowDataHwndOffset);
    return IsWindow(hwnd) ? hwnd : nullptr;
}

bool HwndHasClass(HWND hwnd, PCWSTR className) {
    if (!hwnd) {
        return false;
    }
    WCHAR buf[32];
    return GetClassNameW(hwnd, buf, ARRAYSIZE(buf)) &&
           _wcsicmp(buf, className) == 0;
}

bool IsTopLevelWindowTooltip(void* pThis) {
    return HwndHasClass(HwndFromTopLevelWindow(pThis), L"tooltips_class32");
}

float RadiusForOriginal(float orig, bool isTooltip) {
    // In new builds, multiple hooks fire in sequence (GetRadiusFromCornerStyle
    // -> GetFloatCornerRadiusForCurrentStyle -> SetBorderParameters), so a
    // downstream hook may see a value already replaced by an upstream hook.
    // Skip replacement if the value already matches a configured radius to keep
    // the function idempotent.
    if (orig == g_settings.radius || orig == g_settings.smallRadius ||
        orig == g_settings.tooltipRadius) {
        return orig;
    }

    if (isTooltip && g_settings.tooltipRadius >= 0.0f) {
        return g_settings.tooltipRadius;
    }

    // Win11 defaults: 4.0 for smaller radius, 8.0 for larger radius. Use middle
    // point as a threshold. Don't override if new value is negative.
    float newValue = orig < 6.0f ? g_settings.smallRadius : g_settings.radius;
    if (newValue < 0.0f) {
        return orig;
    }

    return newValue;
}

// Forces an empty window region on a SysShadow companion HWND so the
// legacy rectangular drop shadow stops being composited. Idempotent: once
// the window already has the (empty) region we own, the call is a no-op.
void HideSysShadowWindow(HWND hwnd) {
    HRGN rgn = CreateRectRgn(0, 0, 0, 0);
    if (!rgn) {
        return;
    }
    if (!SetWindowRgn(hwnd, rgn, FALSE)) {
        DeleteObject(rgn);
    }
    // SetWindowRgn takes ownership of the region on success.
}

using UpdateWindowVisuals_t = long(WINAPI*)(void* pThis);
UpdateWindowVisuals_t UpdateWindowVisuals_Original;
long WINAPI UpdateWindowVisuals_Hook(void* pThis) {
    if (g_settings.tooltipRadius >= 0.0f) {
        HWND hwnd = HwndFromTopLevelWindow(pThis);
        if (HwndHasClass(hwnd, L"SysShadow")) {
            Wh_Log(L"> hiding SysShadow hwnd=%p", hwnd);
            HideSysShadowWindow(hwnd);
            return 0;
        }
    }
    return UpdateWindowVisuals_Original(pThis);
}

using GetEffectiveCornerStyle_t = int(WINAPI*)(void* pThis);
GetEffectiveCornerStyle_t GetEffectiveCornerStyle_Original;
int WINAPI GetEffectiveCornerStyle_Hook(void* pThis) {
    int orig = GetEffectiveCornerStyle_Original(pThis);
    // Tooltips report DWMWCP_DONOTROUND, meaning DWM won't round them.
    // Promote them to DWMWCP_ROUNDSMALL so the rounding pipeline kicks in:
    // GetShadowStyle returns a rounded-shadow style, and
    // GetRadiusFromCornerStyle returns a non-zero radius that our hooks
    // override to the configured tooltipRadius via RadiusForOriginal.
    if (orig == DWMWCP_DONOTROUND && g_settings.tooltipRadius >= 0.0f &&
        IsTopLevelWindowTooltip(pThis)) {
        Wh_Log(L"> cornerStyle DONOTROUND -> ROUNDSMALL (tooltip)");
        return DWMWCP_ROUNDSMALL;
    }
    return orig;
}

using GetRadiusFromCornerStyle_t = float(WINAPI*)(void* pThis);
GetRadiusFromCornerStyle_t GetRadiusFromCornerStyle_Original;
float WINAPI GetRadiusFromCornerStyle_Hook(void* pThis) {
    float orig = GetRadiusFromCornerStyle_Original(pThis);
    if (orig > 0) {
        bool isTooltip =
            g_settings.tooltipRadius >= 0.0f && IsTopLevelWindowTooltip(pThis);
        Wh_Log(L"> %f isTooltip=%d", orig, isTooltip);
        return RadiusForOriginal(orig, isTooltip);
    }
    return orig;
}

using GetFloatCornerRadiusForCurrentStyle_t = float(WINAPI*)(void* pThis);
GetFloatCornerRadiusForCurrentStyle_t
    GetFloatCornerRadiusForCurrentStyle_Original;
float WINAPI GetFloatCornerRadiusForCurrentStyle_Hook(void* pThis) {
    float orig = GetFloatCornerRadiusForCurrentStyle_Original(pThis);
    if (orig > 0) {
        bool isTooltip =
            g_settings.tooltipRadius >= 0.0f && IsTopLevelWindowTooltip(pThis);
        Wh_Log(L"> %f isTooltip=%d", orig, isTooltip);
        return RadiusForOriginal(orig, isTooltip);
    }
    return orig;
}

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
    if (cornerRadius > 0) {
        Wh_Log(L"> %f", cornerRadius);
        // pThis here is a CWindowBorder, not a CTopLevelWindow, so there's no
        // straightforward way to recover the HWND for tooltip detection. This
        // path is only used by old builds where the radius is computed inline.
        cornerRadius = RadiusForOriginal(cornerRadius, false);
    }
    return SetBorderParameters_Original(pThis, borderRect, cornerRadius, dpi,
                                        color, borderStyle, shadowStyle);
}

void LoadSettings() {
    // Use `std::nextafter` to get a value that's just slightly above the
    // integer, for two reasons:
    // 1. The original radius values are integer-based, so if the new value is
    //    exactly the same as the original value, it's impossible to determine
    //    whether the mod should override it or not if the custom value is
    //    identical to one of the original values (see RadiusForOriginal).
    // 2. If the zero value is used, some functions may treat it as a special
    //    case, for example dark mode menus will have a white border.
    g_settings.radius =
        std::nextafter(static_cast<float>(Wh_GetIntSetting(L"radius")),
                       std::numeric_limits<float>::max());
    g_settings.smallRadius =
        std::nextafter(static_cast<float>(Wh_GetIntSetting(L"smallRadius")),
                       std::numeric_limits<float>::max());
    g_settings.tooltipRadius =
        std::nextafter(static_cast<float>(Wh_GetIntSetting(L"tooltipRadius")),
                       std::numeric_limits<float>::max());
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

    // Call tree for corner radius in each version:
    //
    // Old builds (e.g. 10.0.22621.6199):
    //   UpdateWindowVisuals
    //     -> GetEffectiveCornerStyle (inlined radius mapping, 8.0/4.0)
    //     -> SetBorderParameters (receives radius as param)
    //   CTopLevelWindow3D::UpdateAnimatedResources
    //     -> GetRadiusFromCornerStyle (DPI scaling inlined)
    //     -> ResourceHelper::CreateRectangleGeometry
    //
    // New builds (e.g. 10.0.26100.7920):
    //   UpdateWindowVisuals
    //     -> GetFloatCornerRadiusForCurrentStyle
    //       -> GetRadiusFromCornerStyle
    //     -> SetBorderParameters (receives radius as param)
    //   CTopLevelWindow3D::UpdateAnimatedResources
    //     -> GetDpiAdjustedFloatCornerRadius
    //       -> GetRadiusFromCornerStyle
    //     -> ResourceHelper::CreateRectangleGeometry

    WindhawkUtils::SYMBOL_HOOK udwmDllHooks[] = {
        // Used to recover the HWND for tooltip detection. Returns the
        // CWindowData* stored on the CTopLevelWindow. Capture only, no hook.
        {
            {LR"(public: class CWindowData * __cdecl CTopLevelWindow::GetWindowData(void)const )"},
            &GetWindowData_Original,
            nullptr,
            true,  // Optional - tooltip detection is skipped if missing.
        },
        // Used to derive the HWND member offset on CWindowData at runtime by
        // disassembling the function's first `mov rcx, [rcx+disp]`. Capture
        // only, no hook.
        {
            {LR"(public: bool __cdecl CWindowData::IsGhostWindow(struct HWND__ * *)const )"},
            &IsGhostWindow_Func,
            nullptr,
            true,  // Optional - tooltip detection is skipped if missing.
        },
        // Skips visual updates for SysShadow companion windows so the legacy
        // rectangular drop shadow doesn't poke out beside the rounded tooltip.
        {
            {LR"(private: long __cdecl CTopLevelWindow::UpdateWindowVisuals(void))"},
            &UpdateWindowVisuals_Original,
            UpdateWindowVisuals_Hook,
            true,  // Optional - SysShadow remains visible if missing.
        },
        // Used to promote tooltips from "no rounding" to "round small" so the
        // full DWM rounding pipeline kicks in (border + shadow + clip), instead
        // of us trying to force a radius onto a window DWM thinks is square.
        {
            {LR"(private: enum CORNER_STYLE __cdecl CTopLevelWindow::GetEffectiveCornerStyle(void))"},
            &GetEffectiveCornerStyle_Original,
            GetEffectiveCornerStyle_Hook,
            true,  // Optional - tooltip rounding skipped if missing.
        },
        // Covers the 3D animation path in both old and new builds.
        {
            {LR"(private: float __cdecl CTopLevelWindow::GetRadiusFromCornerStyle(void))"},
            &GetRadiusFromCornerStyle_Original,
            GetRadiusFromCornerStyle_Hook,
        },
        // Covers UpdateWindowVisuals in new builds (calls
        // GetRadiusFromCornerStyle, but hooked separately in case a future
        // build inlines that call).
        {
            {LR"(private: float __cdecl CTopLevelWindow::GetFloatCornerRadiusForCurrentStyle(void))"},
            &GetFloatCornerRadiusForCurrentStyle_Original,
            GetFloatCornerRadiusForCurrentStyle_Hook,
            true,  // Missing in earlier builds (e.g. 10.0.22621.6199).
        },
        // Covers UpdateWindowVisuals in old builds where the radius is
        // computed inline (no call to GetRadiusFromCornerStyle) and passed
        // directly to this function.
        {
            {LR"(public: long __cdecl CWindowBorder::SetBorderParameters(struct tagRECT const &,float,int,struct _D3DCOLORVALUE const &,enum CWindowBorder::BorderStyle,enum CWindowBorder::ShadowStyle))"},
            &SetBorderParameters_Original,
            SetBorderParameters_Hook,
        },
    };

    if (!HookSymbols(udwm, udwmDllHooks, ARRAYSIZE(udwmDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return FALSE;
    }

    // Hooks queued by HookSymbols aren't applied until Wh_ModInit returns, so
    // IsGhostWindow's bytes are still original here. Disassemble its prologue
    // to recover the HWND member offset on CWindowData. The function loads
    // this->hwnd as the first argument to GetPropW, so the first load whose
    // base register is the `this` pointer (rcx on x64, x0 on ARM64) is the
    // HWND member - the destination register is left unconstrained because
    // compilers may stage the value through a scratch register first.
    if (IsGhostWindow_Func) {
        g_windowDataHwndOffset = OffsetFromAssemblyRegex(
            IsGhostWindow_Func, SIZE_MAX,
#if defined(_M_X64)
            std::regex(R"(mov \w+, \[rcx\+0x([0-9a-f]+)\])",
                       std::regex_constants::icase),
#elif defined(_M_ARM64)
            std::regex(R"(ldr\s+\w+, \[x0, #0x([0-9a-f]+)\])",
                       std::regex_constants::icase),
#else
#error "Unsupported architecture"
#endif
            10);
        Wh_Log(L"windowDataHwndOffset=0x%zx", g_windowDataHwndOffset);
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
